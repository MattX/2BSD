/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ht.c	2.0 (2.11BSD) 4/20/91
 */

/*
 * TM02/3 - TU16/TE16/TU77 standalone tape driver
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/htreg.h"
#include "saio.h"

#define	NHT		2
#define	H_NOREWIND	004		/* not used in stand alone driver */
#define	H_1600BPI	010

	struct	htdevice *HTcsr[NHT + 1] =
		{
		(struct htdevice *)0172440,
		(struct htdevice *)0,
		(struct htdevice *)-1
		};
extern int tapemark;	/* flag to indicate tapemark encountered
			   (see sys.c as to how it's used) */

htopen(io)
	register struct iob *io;
{
	register skip;
	register int ctlr = CTLRn(io->i_unit);
	int i;

	if (genopen(NHT, io) < 0)
		return(-1);
	htstrategy(io, HT_REW);
	skip = io->i_boff;
	while (skip--) {
		io->i_cc = -1;
		while (htstrategy(io, HT_SFORW))
			continue;
		i = 0;
		while (--i)
			continue;
		htstrategy(io, HT_SENSE);
	}
	return(0);
}

htclose(io)
	struct iob *io;
{
	htstrategy(io, HT_REW);
}

htstrategy(io, func)
	register struct iob *io;
{
	register unit, com;
	int errcnt, ctlr;
	register struct htdevice *htaddr;

	unit = UNITn(io->i_unit);
	ctlr = CTLRn(io->i_unit);
	htaddr = HTcsr[ctlr];
	errcnt = 0;
retry:
	while ((htaddr->htcs1 & HT_RDY) == 0)
		continue;
	while (htaddr->htfs & HTFS_PIP)
		continue;

	htaddr->httc =
		((io->i_unit&H_1600BPI) ? HTTC_1600BPI : HTTC_800BPI)
		| HTTC_PDP11 | unit;
	htaddr->htba = io->i_ma;
	htaddr->htfc = -io->i_cc;
	htaddr->htwc = -(io->i_cc >> 1);
	com = ((segflag) << 8) | HT_GO;
	if (func == READ)
		com |= HT_RCOM;
	else if (func == WRITE)
		com |= HT_WCOM;
	else if (func == HT_SREV) {
		htaddr->htfc = -1;
		htaddr->htcs1 = com | HT_SREV;
		return(0);
	} else
		com |= func;
	htaddr->htcs1 = com;
	while ((htaddr->htcs1 & HT_RDY) == 0)
		continue;
	if (htaddr->htfs & HTFS_TM) {
		tapemark = 1;
		htinit(htaddr);
		return(0);
	}
	if (htaddr->htcs1 & HT_TRE) {
		if (errcnt == 0)
			printf("\nHT%d,%d err: cs2=%o, er=%o",
			    ctlr, unit, htaddr->htcs2, htaddr->hter);
		htinit(htaddr);
		if (errcnt++ == 10) {
			printf("\n(FATAL ERROR)\n");
			return(-1);
		}
		htstrategy(io, HT_SREV);
		goto retry;
	}
	return(io->i_cc+htaddr->htfc);
}

htinit(htaddr)
	register struct htdevice *htaddr;
{
	register int omt, ocs2;

	omt = htaddr->httc & 03777;
	ocs2 = htaddr->htcs2 & 07;

	htaddr->htcs2 = HTCS2_CLR;
	htaddr->htcs2 = ocs2;
	htaddr->httc = omt;
	htaddr->htcs1 = HT_DCLR|HT_GO;
}
