/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)tm.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 * TM11 - TU10/TE10/TS03 standalone tape driver
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/tmreg.h"
#include "saio.h"

#define	TMADDR ((struct tmdevice *)0172520)

extern int tapemark;	/* flag to indicate tapemark 
			has been encountered (see sys.c) 	*/

/*
 * Bits in deivice code.
 */
#define	TEUNIT(dev)	((dev) & 03)
#define	T_NOREWIND	04		/* not used in stand alone driver */
#define	TEDENS(dev)	(((dev) & 030) >> 3)

tmrew(io)
	register struct iob *io;
{
	tmstrategy(io, TM_REW);
}

tmopen(io)
	register struct iob *io;
{
	register skip;

	tmstrategy(io, TM_REW);
	skip = io->i_boff;
	while (skip--) {
		io->i_cc = 0;
		while (tmstrategy(io, TM_SFORW))
			continue;
	}
}

u_short tmdens[4] = { TM_D800, TM_D1600, TM_D6250, TM_D800 };

tmstrategy(io, func)
	register struct iob *io;
{
	register int com, unit, errcnt;

	unit = io->i_unit;
	errcnt = 0;
retry:
	tmquiet();
	com = (TEUNIT(unit)<<8)|(segflag<<4) | tmdens[TEDENS(unit)];
	TMADDR->tmbc = -io->i_cc;
	TMADDR->tmba = io->i_ma;
	if (func == READ)
		TMADDR->tmcs = com | TM_RCOM | TM_GO;
	else if (func == WRITE)
		TMADDR->tmcs = com | TM_WCOM | TM_GO;
	else if (func == TM_SREV) {
		TMADDR->tmbc = -1;
		TMADDR->tmcs = com | TM_SREV | TM_GO;
		return(0);
	} else
		TMADDR->tmcs = com | func | TM_GO;
	while ((TMADDR->tmcs&TM_CUR) == 0)
		continue;
	if (TMADDR->tmer&TMER_EOF) {
		tapemark=1;
		return(0);
	}
	if (TMADDR->tmer & TM_ERR) {
		if (errcnt == 0)
			printf("\nTM unit %d tape error: er=%o cs=%o",
				unit, TMADDR->tmer, TMADDR->tmcs);
		if (errcnt++ == 10) {
			printf("\n(FATAL ERROR)\n");
			return(-1);
		}
		tmstrategy(io, TM_SREV);
		goto retry;
	}
	if (errcnt)
		printf("\n(RECOVERED by retry)\n");
	return(io->i_cc+TMADDR->tmbc);
}

tmquiet()
{
	while ((TMADDR->tmcs&TM_CUR) == 0)
		continue;
	while ((TMADDR->tmer&TMER_TUR) == 0)
		continue;
	while ((TMADDR->tmer&TMER_SDWN) != 0)
		continue;
}
