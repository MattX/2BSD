/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)checksys.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 * checksys
 *	checks the system size and reports any limits exceeded.
 */

#include "param.h"
#include "user.h"
#include "file.h"
#include "ioctl.h"
#include "clist.h"
#include "a.out.h"
#include "stdio.h"

/* Round up to a click boundary. */
#define	cround(bytes)	((bytes + ctob(1) - 1) / ctob(1) * ctob(1));
#define	round(x)	(ctob(stoc(ctos(btoc(x)))))

#define	KB(val)		((u_int)(val * 1024))

#define	N_END		0
#define	N_BSIZE		1
#define	N_NBUF		2
#define	N_PROC		5
#define	N_CLIST		15
#define	N_NOKA5		16
#define	N_RAM		17

struct nlist nl[] = {
	{ "_end" },			/*  0 */
	{ "_bsize" },			/*  1 */
	{ "_nbuf" },			/*  2 */
	{ "_buf" },			/*  3 */
	{ "_nproc" },			/*  4 */
	{ "_proc" },			/*  5 */
	{ "_ntext" },			/*  6 */
	{ "_text" },			/*  7 */
	{ "_nfile" },			/*  8 */
	{ "_file" },			/*  9 */
	{ "_ninode" },			/* 10 */
	{ "_inode" },			/* 11 */
	{ "_ncallou" },			/* 12 */
	{ "_callout" },			/* 13 */
	{ "_ucb_cli" },			/* 14 */
	{ "_nclist" },			/* 15 */
	{ "_noka5" },			/* 16 */
	{ "_ram_siz" },			/* 17 */
	{ "" },
};

static struct exec obj;
static struct ovlhdr ovlhdr;
static int fi;

main(argc, argv)
	int argc;
	char **argv;
{
	register int i;
	long size, totsize, ramdisk, getval();
	int errs = 0, texterrs = 0;

	if (argc != 2) {
		fputs("usage: checksys unix-binary\n", stderr);
		exit(-1);
	}
	if ((fi = open(argv[1], O_RDONLY)) < 0) {
		perror(argv[1]);
		exit(-1);
	}
	if (read(fi, &obj, sizeof(obj)) != sizeof(obj)) {
		fputs("checksys: can't read object header.\n", stderr);
		exit(-1);
	}
	if (obj.a_magic == A_MAGIC5 || obj.a_magic == A_MAGIC6)
		if (read(fi, &ovlhdr, sizeof(ovlhdr)) != sizeof(ovlhdr)) {
			fputs("checksys: can't read overlay header.\n", stderr);
			exit(-1);
		}
	switch(obj.a_magic) {

	/*
	 *	0407-- nonseparate I/D "vanilla"
	 */
	case A_MAGIC1:
	case A_MAGIC2:
		size = obj.a_text + obj.a_data + obj.a_bss;
		if (size > KB(48)) {
			printf("total size is %ld, max is %u, too large by %ld bytes.\n", size, KB(48), size - KB(48));
			errs++;
		}
		totsize = cround(size);
		break;

	/*
	 *	0411-- separate I/D
	 */
	case A_MAGIC3:
		size = obj.a_data + obj.a_bss;
		if (size > KB(48)) {
			printf("data is %ld, max is %u, too large by %ld bytes.\n", size, KB(48), size - KB(48));
			errs++;
		}
		totsize = obj.a_text + cround(size);
		break;

	/*
	 *	0430-- overlaid nonseparate I/D
	 */
	case A_MAGIC5:
		if (obj.a_text > KB(16)) {
			printf("base segment is %u, max is %u, too large by %u bytes.\n", obj.a_text, KB(16), obj.a_text - KB(16));
			errs++;
			texterrs++;
		}
		if (obj.a_text <= KB(8)) {
			printf("base segment is %u, minimum is %u, too small by %u bytes.\n", obj.a_text, KB(8), KB(8) - obj.a_text);
			errs++;
		}
		size = obj.a_data + obj.a_bss;
		if (size > KB(24)) {
			printf("data is %ld, max is %u, too large by %ld bytes.\n", size, KB(24), size - KB(24));
			errs++;
		}
		/*
		 *  Base and overlay 1 occupy 16K and 8K of physical
		 *  memory, respectively, regardless of actual size.
		 */
		totsize = KB(24) + cround(size);
		/*
		 *  Subtract the first overlay, it will be added below
		 *  and it has already been included.
		 */
		totsize -= ovlhdr.ov_siz[0];
		goto checkov;
		break;

	/*
	 *	0431-- overlaid separate I/D
	 */
	case A_MAGIC6:
		if (obj.a_text > KB(56)) {
			printf("base segment is %u, max is %u, too large by %u bytes.\n", obj.a_text, KB(56), obj.a_text - KB(56));
			errs++;
		}
		if (obj.a_text <= KB(48)) {
			printf("base segment is %u, min is %u, too small by %u bytes.\n", obj.a_text, KB(48), KB(48) - obj.a_text);
			errs++;
		}
		size = obj.a_data + obj.a_bss;
		if (size > KB(48)) {
			printf("data is %ld, max is %u, too large by %ld bytes.\n", size, KB(48), size - KB(48));
			errs++;
		}
		totsize = obj.a_text + cround(size);

checkov:
		for (i = 0;i < NOVL;i++) {
			totsize += ovlhdr.ov_siz[i];
			if (ovlhdr.ov_siz[i] > KB(8)) {
				printf("overlay %d is %u, max is %u, too large by %u bytes.\n", i + 1, ovlhdr.ov_siz[i], KB(8), ovlhdr.ov_siz[i] - KB(8));
				errs++;
				texterrs++;
			}
			/*
			 * check for zero length overlay in the middle of
			 * non-zero length overlays (boot croaks when faced
			 * with this) ...
			 */
			if (i < NOVL-1
			    && ovlhdr.ov_siz[i] == 0
			    && ovlhdr.ov_siz[i+1] > 0) {
				printf("overlay %d is empty and there are non-empty overlays following it.\n", i);
				errs++;
				texterrs++;
			}
		}
		break;

	default:
		printf("magic number 0%o not recognized.\n", obj.a_magic);
		exit(-1);
	}

	(void)nlist(argv[1], nl);

	if (!nl[N_NOKA5].n_type) {
		puts("\"noka5\" not found in namelist.");
		exit(-1);
	}
	if (!texterrs)
		if (getval(N_NOKA5)) {
			if (nl[N_END].n_value >= 0120000) {
				printf("Data extends into the remapping area (0120000-0140000, KDSD5)\nby %u bytes; undefine NOKA5 or reduce data size.\n", nl[N_END].n_value - 0120000);
				errs++;
			}
		}
		else {
			if (nl[N_PROC].n_value >= 0120000) {
				printf("The remapping area (0120000-0140000, KDSD5)\n\tcontains data other than the proc, text and file tables.\n\tReduce other data by %u bytes.\n", nl[N_PROC].n_value - 0120000);
				errs++;
			}
			if (nl[N_END].n_value < 0120000)
				printf("Data ends %u bytes below the remapping area (0120000-0140000, KDSD5)\nyou may define NOKA5.\n", 0120000 - nl[N_END].n_value);
		}
	totsize += cround(getval(N_NBUF) * getval(N_BSIZE));
	if (nl[N_CLIST].n_value)
		totsize += cround(getval(N_CLIST) * (long)sizeof(struct cblock));
	if (nl[N_RAM].n_type)
		totsize += getval(N_RAM)*512;
	totsize += ctob(USIZE);
	printf("System will occupy %ld bytes of memory (including buffers and clists).\n", totsize);
	for (i = 0;nl[i].n_name[0];++i) {
		if (!(i % 3))
			putchar('\n');
		printf("\t%7.7s {0%06o}", nl[i].n_name + 1, nl[i].n_value);
	}
	putchar('\n');
	if (errs)
		puts("**** SYSTEM IS NOT BOOTABLE. ****");
	exit(errs);
}

/*
 *  Get the value of an initialized variable from the object file.
 */
static long
getval(indx)
	int indx;
{
	register int novl;
	u_int ret;
	off_t offst;

	if ((nl[indx].n_type&N_TYPE) == N_BSS)
		return((long)0);
	offst = nl[indx].n_value + obj.a_text + sizeof(obj);
	if (obj.a_magic == A_MAGIC2 || obj.a_magic == A_MAGIC5)
		offst -= (off_t)round(obj.a_text);
	if (obj.a_magic == A_MAGIC5 || obj.a_magic == A_MAGIC6) {
		offst += sizeof ovlhdr;
		if (obj.a_magic == A_MAGIC5)
			offst -= (off_t)round(ovlhdr.max_ovl);
		for (novl = 0;novl < NOVL;novl++)
			offst += (off_t)ovlhdr.ov_siz[novl];
	}
	(void)lseek(fi, offst, L_SET);
	read(fi, &ret, sizeof(ret));
	return((long)ret);
}
