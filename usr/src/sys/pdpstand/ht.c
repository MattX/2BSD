/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ht.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 * TM02/3 - TU16/TE16/TU77 standalone tape driver
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/htreg.h"
#include "saio.h"


#define	HTADDR	((struct htdevice *)0172440)

#define	TUUNIT(dev)	(minor(dev) & 03)
#define	H_NOREWIND	004		/* not used in stand alone driver */
#define	H_1600BPI	010

extern int tapemark;	/* flag to indicate tapemark encountered
			   (see sys.c as to how it's used) */

htopen(io)
	register struct iob *io;
{
	register skip;
	int i;

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
}

htclose(io)
	struct iob *io;
{
	htstrategy(io, HT_REW);
}

htstrategy(io, func)
	register struct iob *io;
{
	register unit, com, errcnt;

	unit = io->i_unit;
	errcnt = 0;
retry:
	htquiet();

	HTADDR->httc =
		((unit&H_1600BPI) ? HTTC_1600BPI : HTTC_800BPI)
		| HTTC_PDP11 | TUUNIT(unit);
	HTADDR->htba = io->i_ma;
	HTADDR->htfc = -io->i_cc;
	HTADDR->htwc = -(io->i_cc >> 1);
	com = ((segflag) << 8) | HT_GO;
	if (func == READ)
		com |= HT_RCOM;
	else if (func == WRITE)
		com |= HT_WCOM;
	else if (func == HT_SREV) {
		HTADDR->htfc = -1;
		HTADDR->htcs1 = com | HT_SREV;
		return(0);
	} else
		com |= func;
	HTADDR->htcs1 = com;
	while ((HTADDR->htcs1 & HT_RDY) == 0)
		continue;
	if (HTADDR->htfs & HTFS_TM) {
		tapemark = 1;
		htinit();
		return(0);
	}
	if (HTADDR->htcs1 & HT_TRE) {
		if (errcnt == 0)
			printf("\nHT unit %d tape error: cs2=%o, er=%o",
			    unit, HTADDR->htcs2, HTADDR->hter);
		htinit();
		if (errcnt++ == 10) {
			printf("\n(FATAL ERROR)\n");
			return(-1);
		}
		htstrategy(io, HT_SREV);
		goto retry;
	}
	if (errcnt)
		printf("\n(RECOVERED by retry)\n");
	return(io->i_cc+HTADDR->htfc);
}

htinit()
{
	register int omt, ocs2;

	omt = HTADDR->httc & 03777;
	ocs2 = HTADDR->htcs2 & 07;

	HTADDR->htcs2 = HTCS2_CLR;
	HTADDR->htcs2 = ocs2;
	HTADDR->httc = omt;
	HTADDR->htcs1 = HT_DCLR|HT_GO;
}

htquiet()
{
	while ((HTADDR->htcs1 & HT_RDY) == 0)
		continue;
	while (HTADDR->htfs & HTFS_PIP)
		continue;
}
