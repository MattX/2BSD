/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)br.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 * br03-like disk driver
 *  	modified to handle EATON 1537 and 1711 controllers with
 *	T300, T200, T80 and T50 drives.  NOTE: the boot block is
 *	hard coded to 32 sec/trk and 19 trk/cyl because he can't
 *	be made big enough to autosize like this program can.
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/brreg.h"
#include "saio.h"

#define BRADDR ((struct brdevice *) 0176710)

#define	SEC22	02400	/* T200 or T50 */
#define CYL5	01400	/* T80 or T50 */

int	brsctrk[8], brtrkcyl[8];

brstrategy(io, func)
	register struct iob *io;
{
	int com, cn, tn, sn, unit, sectrk, trkcyl, ctr;

	/* if we haven't gotten the characteristics yet, do so now. */
	trkcyl = brtrkcyl[unit = io->i_unit];
	if (!(sectrk = brsctrk[unit])) {
		/* give a home seek command, then wait for complete */
		BRADDR->brcs.w = (unit << 8) | BR_HSEEK | BR_GO;
		ctr = 0;
		while ((BRADDR->brcs.w & BR_RDY) == 0 && --ctr)
			continue;
		if (BRADDR->brcs.w & BR_HE) {
			printf("br%d not ready\n", unit);
			return(-1);
		}
		com = BRADDR->brae;
		if (com & SEC22)
			sectrk = 22;
		else
			sectrk = 32;
		if (com & CYL5)
			trkcyl = 5;
		else
			trkcyl = 19;
		brsctrk[unit] = sectrk;
		brtrkcyl[unit] = trkcyl;
	}
	cn = io->i_bn/(sectrk * trkcyl);
	sn = io->i_bn%(sectrk * trkcyl);
	tn = sn/sectrk;
	sn = sn%sectrk;
	BRADDR->brcs.w = (unit<<8);
	BRADDR->brda = (tn<<8) | sn;
	BRADDR->brca = cn;
	BRADDR->brba = io->i_ma;
	BRADDR->brwc = -(io->i_cc>>1);
	BRADDR->brae = segflag;
	com = (segflag<<4)|BR_GO;
	if (func == READ)
		com |= BR_RCOM;
	else
		com |= BR_WCOM;
	BRADDR->brcs.w |= com;
	while ((BRADDR->brcs.w& BR_RDY)==0)
		continue;
	if (BRADDR->brcs.w < 0) {	/* error bit */
		printf("disk error: cyl=%d track=%d sect=%d er=%o ds=%o\n",
		    cn, tn, sn, BRADDR->brer, BRADDR->brds);
		return(-1);
	}
	return(io->i_cc);
}
