#include "params.h"
static char *sccsid = "%W%	%G%";

/*
 * Process inbound network news.
 */
uneti()
{
	register FILE *fp;
	register int c;
	struct hbuf h;
	struct srec srec;

	/* get the local system subscription line. */
	if (s_find(&srec, SYSNAME) == NULL)
		xerror("Cannot find SYSNAME in SYSFILE");

	lock();
	fp = xfopen(TIFILE, "w");
	while ((c = getc(stdin)) != EOF)
		putc(c, fp);
	fclose(fp);
	if ((fp = hread(&h, TIFILE, "")) == NULL)
		xerror("inbound news is garbled");
	ngsquash(h.nbuf, srec.s_nbuf);
	if (h.nbuf[0] == '\0') {
		printf("Article %s not subscribed to\n", h.name);
		canned(TIFILE, h.name);
	} else if (exists(h.name)) {
		printf("Duplicate article file name %s ignored\n", h.name);
		canned(TIFILE, h.name);
	} else {
		getdtln();
		insert(fp, &h);
	}
	fclose(fp);
	unlink(TIFILE);
	unlock();
}

/*
 * Human input routine.
 */
iopt(nflag)
int nflag;
{
	if (header.title[0] == '\0')
		xerror("No title given");
	if (nflag != 1)
		ngcat(strcpy(header.nbuf, DFLTNG));
	ngfcheck(FALSE);
	lock();
	getname();
	unlock();
	getuser();
	getdtln();
	nstrip(header.title);	/* pull trailing space off title */
	if (sigtrap)
		return;
	insert(stdin, &header);
}

/*
 * Process a news article.
 * The header is assumed to be set up in header,
 * and the body is assumed to be readable on stream ifp.
 */
insert(ifp, hp)
register struct hbuf *hp;
register FILE *ifp;
{
	register FILE *ofp;
	struct nrec nrec;
	char ofname[BUFLEN];
	register int empty;

	/*
	 * Open the output file.
	 * Since names are unique network wide, no lock is necessary.
	 * Since input may be from a tty, none is desired.
	 */
	sprintf(ofname, "%s/%s", NEWSD, hp->name);
	ofp = xfopen(ofname, "w");
#ifdef PWB
	chmod(ofname, 0644);
	if (duid != 0)
		chown(ofname, ((dgid << 8) | duid));
#else
	if (duid != 0)
		chown(ofname, duid, dgid);
#endif
	users(hp->ndate-1L);

	hwrite(hp, ofp);

	/* copy the body as is. */
	empty = TRUE;	/* no news in file yet */
	while (!sigtrap && fgets(bfr, BUFLEN, ifp) != NULL) {
		if (strcmp(bfr, ".\n") == 0)
			break;
		if (strncmp(bfr, "From ", 5) == 0)
			putc('>', ofp);
		fputs(bfr, ofp);
		empty = FALSE;
	}
	fclose(ofp);

	if (sigtrap && isatty(fileno(ifp))) {
		if (!empty)
			fwait(fsubr(newssave, NEWSD, hp->name));
		unlink(ofname);
		return;
	}
	strcpy(nrec.n_file, hp->name);
	strcpy(nrec.n_nglist, hp->nbuf);
	getdtln();
	nrec.n_date = header.ndate;

	lock();
	n_openm();
	n_write(&nrec);
	while (n_read(&nrec))
		n_write(&nrec);
	n_close();
	unlock();
	bitup = TRUE;

	fsubr(broadcast, NEWSD, hp->name);
}

/*
 * Save partial news.
 */
newssave(dir, name)
char *dir, *name;
{
	register FILE *to, *from;
	char sfname[BUFLEN];
	register int c;
	struct hbuf h;

	if ((from = hread(&h, dir, name)) == NULL)
		return;
	umask(savmask);
	setgid(gid);
	setuid(uid);

	strcpy(sfname, PARTIAL);
	if ((to = fopen(sfname, "w")) == NULL) {
		sprintf(sfname, "%s/%s", userhome, PARTIAL);
		if ((to = fopen(sfname, "w")) == NULL)
			xerror("Cannot save partial news");
	}
	while ((c = getc(from)) != EOF)
		putc(c, to);
	fclose(from);
	fclose(to);
	printf("News saved in %s\n", sfname);
}

/*
 * Transmit this article to all interested systems.
 */
broadcast(dir, name)
char *dir, *name;
{
	register char *nptr, *hptr;
	register FILE *fp;
	struct srec srec;
	struct hbuf h;

	if ((fp = hread(&h, dir, name)) == NULL)
		xerror("Cannot reread article");
	fclose(fp);

	 /* break path into list of systems. */
	hptr = nptr = h.path;
	while (*hptr != '\0') {
		if (index(NETCHRS, *hptr)) {
			*hptr++ = '\0';
			nptr = hptr;
		} else
			hptr++;
	}
	*nptr = '\0';

	/* loop once per system. */
	s_openr();
	while (s_read(&srec)) {
		if (strncmp(srec.s_name, SYSNAME, SNLN) == 0)
			continue;
		hptr = h.path;
		while (*hptr != '\0') {
			if (strncmp(srec.s_name, hptr, SNLN) == 0)
				goto contin;
			while (*hptr++ != '\0');
		}
		if (ngmatch(h.nbuf, srec.s_nbuf))
			transmit(&srec, dir, name);
	contin:;
	}
	s_close();
}

/*
 * Transmit file to system.
 */
transmit(sp, dir, name)
register struct srec *sp;
char *dir, *name;
{
	register FILE *ifp, *ofp;
	register int c;
	struct hbuf h;

	if ((ifp = hread(&h, dir, name)) == NULL)
		return;
	ngsquash(h.nbuf, sp->s_nbuf);
	if (h.nbuf[0] == '\0') {
		printf("Article not subscribed to by %s\n", sp->s_name);
		return;
	}
	ofp = xfopen(TOFILE, "w");
	hwrite(&h, ofp);
	while ((c = getc(ifp)) != EOF)
		putc(c, ofp);
	fclose(ifp);
	fclose(ofp);
	if (*sp->s_xmit == '\0')
		sprintf(bfr, "uux - -r %s!rnews < %s", sp->s_name, TOFILE);
	else
		sprintf(bfr, "(%s) < %s", sp->s_xmit, TOFILE);
	fwait(fsubr(pshell, bfr, (char *)NULL));
	unlink(TOFILE);
}

/*
 * Return TRUE if file exists in NEWSD.
 */
exists(file)
register char *file;
{
	register FILE *fp;

	sprintf(bfr, "%s/%s", NEWSD, file);
	if ((fp = fopen(bfr, "r")) != NULL) {
		if (getc(fp) == EOF) {
			fclose(fp);
			return(FALSE);
		}
		fclose(fp);
		return(TRUE);
	}
	return(FALSE);
}
