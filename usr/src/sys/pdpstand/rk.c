/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)rk.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 * RK disk driver
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/rkreg.h"
#include "saio.h"

#define	RKADDR	((struct rkdevice *)0177400)

rkstrategy(io, func)
	register struct iob *io;
{
	register com;
	daddr_t bn;
	int dn, cn, sn;

	bn = io->i_bn;
	dn = io->i_unit;
	cn = bn/12;
	sn = bn%12;
	RKADDR->rkda = (dn<<13) | (cn<<4) | sn;
	RKADDR->rkba = io->i_ma;
	RKADDR->rkwc = -(io->i_cc>>1);
	com = (segflag<<4)|RKCS_GO;
	if (func == READ)
		com |= RKCS_RCOM;
	else
		com |= RKCS_WCOM;
	RKADDR->rkcs = com;
	while ((RKADDR->rkcs & RKCS_RDY) == 0)
		continue;
	if (RKADDR->rkcs<0) {	/* error bit */
		printf("disk error: cyl=%d, sector=%d, er=%o, ds=%o\n",
		    cn, sn, RKADDR->rker, RKADDR->rkds);
		return(-1);
	}
	return(io->i_cc);
}
