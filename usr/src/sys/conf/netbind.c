/*
 * netbind --
 *
 * Resolve undefined inter-address-space references.
 *
 * Usage:
 *	netbind unix.o netnix.o
 *
 * Produces two files as output:
 *
 *	d.unix.s	-- definitions of all symbols referenced by
 *			   unix.o but defined in netnix.o
 *	d.netnix.s	-- definitions of all symbols referenced by
 *			   netnix.o but defined in unix.o
 */
#include <sys/param.h>
#include <sys/file.h>
#include <a.out.h>
#include <stdio.h>

struct execov {
	struct exec eo_exec;
	struct ovlhdr eo_ovl;
} refExec, defExec;

#define	NSYMS	200
struct symbol {
	char	s_name[8];
	u_int	s_value;
	u_int	s_symnum;
	u_int	s_type;
} symtab[NSYMS], *symfree;

main(argc, argv)
	int argc;
	char **argv;
{
	if (argc != 3) {
		printf("usage: %s unix.bo netnix.bo\n", *argv);
		exit(2);
	}
	resolve(argv[1], argv[2]);	/* Produces d.unix.s */
	resolve(argv[2], argv[1]);	/* Produces d.netnix.s */
	exit(0);
}

/*
 * resolve --
 *
 * Resolve references made by ref to symbols defined in def.
 *
 * Side effects:
 *	Prints a list of undefined symbols if there are any.
 *	Produces the .s file corresponding to ref.
 *	Adds the number of undefined symbols encountered to undef.
 */
resolve(ref, def)
	char *ref;		/* Name of file referencing symbols */
	char *def;		/* Name of file defining symbols */
{
	FILE *refIN, *defIN, *refOUT, *openobj();
	struct symbol *sp;
	struct nlist syment;
	off_t offset;
	u_int symnum;
	int nundef;
	char *rp, refout[MAXPATHLEN], *rindex();
	long SYMOFF();

	if (rp = rindex(ref, '.'))
		*rp = '\0';
	(void)sprintf(refout, "d.%s.s", ref);
	if (rp)
		*rp = '.';
	if ((refIN = openobj(ref, &refExec)) == NULL)
		return;
	if ((defIN = openobj(def, &defExec)) == NULL) {
		fclose(refIN);
		return;
	}
	if ((refOUT = fopen(refout, "w")) == NULL) {
		perror(refout);
		fclose(refIN);
		fclose(defIN);
		return;
	}

	/*
	 * Actually generate the .s file needed.
	 * Assumes that refExec and defExec have been set up to
	 * hold the a.out headers for the reference and definition
	 * object files.
	 *
	 * Algorithm:
	 *	Build a table of undefined symbols in refIN.
	 *	Define them by reading the symbol table of defIN.
	 *	Generate the .s file to refOUT.
	 */
	syminit();
	nundef = 0;

	/* Find the undefined symbols */
	symnum = 0;
	offset = SYMOFF(&refExec);
	fseek(refIN, offset, L_SET);
	while (fread(&syment, sizeof(syment), 1, refIN) > 0) {
#ifdef DEBUG
		printf("%-6d %8.8s %3o %6o\n", symnum,
		    syment.n_name, syment.n_type, syment.n_value);
#endif
		if (syment.n_type == (N_EXT|N_UNDF) && syment.n_value == 0)
			if (strcmp(syment.n_name, "_end") &&
			    strcmp(syment.n_name, "_etext") &&
			    strcmp(syment.n_name, "_edata")) {
				nundef++;
				symadd(symnum, &syment);
			}
		symnum++;
	}

	/* Define the undefined symbols */
	offset = SYMOFF(&defExec);;
	fseek(defIN, offset, L_SET);
	while (fread(&syment, sizeof(syment), 1, defIN) > 0) {
#ifdef DEBUG
		printf("%8.8s %3o %6o\n", syment.n_name,
		    syment.n_type, syment.n_value);
#endif
		if ((syment.n_type & N_EXT) == 0)
			continue;
		nundef -= symdef(&syment, &defExec.eo_exec);
	}

	/* Any undefined symbols left? */
	if (nundef > 0) {
		printf("%s: %d symbols undefined:\n", ref, nundef);
		symundef();
	}
	symprdef(refOUT);

	fclose(refIN);
	fclose(defIN);
	fclose(refOUT);
}

/*
 * openobj --
 *
 * Open the indicated file, check to make sure that it is a
 * valid object file with a symbol table, and read in the a.out header.
 *
 * Returns a pointer to an open FILE if successful, NULL if not.
 * Prints its own error messages.
 */
FILE *
openobj(filename, exechdr)
	char *filename;
	struct execov *exechdr;
{
	FILE *f;

	if (!(f = fopen(filename, "r"))) {
		perror(filename);
		return((FILE *)NULL);
	}
	if (fread(exechdr, sizeof(*exechdr), 1, f) <= 0) {
		printf("%s: no a.out header\n", filename);
		goto bad;
	}
	if (N_BADMAG(exechdr->eo_exec)) {
		printf("%s: bad magic number\n", filename);
		goto bad;
	}
	if (exechdr->eo_exec.a_syms == 0) {
		printf("%s: no symbol table\n", filename);
		goto bad;
	}
	return(f);
bad:	fclose(f);
	return((FILE *)NULL);
}

/* -------------------- Symbol table management ----------------------- */

/*
 * syminit --
 *
 * Clear and initialize the symbol table.
 */
syminit()
{
	symfree = symtab;
}

/*
 * symadd --
 *
 * Add a symbol to the table.
 * We store both the symbol name and the symbol number.
 */
symadd(symnum, np)
	int symnum;
	struct nlist *np;
{
	if (symfree >= &symtab[NSYMS]) {
		printf("Symbol table overflow.  Increase NSYMS.\n");
		exit (1);
	}
	(void)strncpy(symfree->s_name, np->n_name, sizeof(np->n_name));
	symfree->s_symnum = symnum;
	symfree->s_type = N_UNDF;
	symfree->s_value = 0;
	symfree++;
}

/*
 * symdef --
 *
 * Use the supplied symbol to define an undefined entry in
 * the symbol table.
 *
 * Returns 1 if the name of the symbol supplied was found in
 * the table, 0 otherwise.
 */
symdef(np, ep)
	struct nlist *np;
	struct exec *ep;
{
	register struct symbol *sp;

	for (sp = symtab; sp < symfree; sp++)
		if (!strncmp(sp->s_name, np->n_name, sizeof(np->n_name))) {
			int type = (np->n_type & N_TYPE);

			sp->s_type = N_EXT|N_ABS;
			switch (type) {
			case N_TEXT:
			case N_ABS:
				sp->s_value = np->n_value;
				break;
			case N_DATA:
			case N_BSS:
				if (ep->a_flag)
					sp->s_value = np->n_value;
				else
					sp->s_value = np->n_value - ep->a_text;
				break;
			default:
				printf("netbind: symbol %.8s, unhandled type 0x%x\n",
				    np->n_name, np->n_type);
				exit(1);
			}
			return (1);
		}
	return(0);
}

/*
 * symslook --
 *
 * Look for a symbol with a particular symbol number in the table.
 * Returns a pointer to the symbol, or NULL if the symbol is not found.
 */
struct symbol *
symslook(symnum)
	int symnum;
{
	register struct symbol *sp;

	for (sp = symtab; sp < symfree; sp++)
		if (sp->s_symnum == symnum)
			return(sp);
	return((struct symbol *)NULL);
}

/*
 * symundef --
 *
 * Print all undefined symbols in the symbol table.
 * First sorts the table before printing it.
 */
symundef()
{
	register struct symbol *sp;
	int scmp();

	qsort(symtab, symfree - symtab, sizeof(struct symbol), scmp);
	for (sp = symtab; sp < symfree; sp++)
		if (sp->s_type == N_UNDF)
			printf("%.8s\n", sp->s_name);
}

scmp(s1, s2)
	register struct symbol *s1, *s2;
{
	return(strncmp(s1->s_name, s2->s_name, sizeof(s1->s_name)));
}

/*
 * symprdef --
 *
 * Output all defined symbols in the symbol table.
 * First sorts the table before printing it.
 */
symprdef(refOUT)
	FILE *refOUT;
{
	register struct symbol *sp;
	int scmp();

	qsort(symtab, symfree - symtab, sizeof (struct symbol), scmp);
	for (sp = symtab; sp < symfree; sp++)
		if ((sp->s_type & N_TYPE) != N_UNDF) {
			fprintf(refOUT, ".globl %.8s\n", sp->s_name);
			fprintf(refOUT, "%.8s = %o\n", sp->s_name, sp->s_value);
		}
}

long
SYMOFF(eop)
	struct execov *eop;
{
	register struct exec *ep = &eop->eo_exec;
	register int i;
	long data, symoff;

	symoff = (long) N_TXTOFF(*ep);
	data = (long)ep->a_text + (long)ep->a_data;
	if (ep->a_magic == A_MAGIC5 || ep->a_magic == A_MAGIC6)
		for (i = 0; i < NOVL; i++)
			data += (long)eop->eo_ovl.ov_siz[i];
	symoff += data;
	if (ep->a_flag == 0)
		symoff += data;
	return(symoff);
}
