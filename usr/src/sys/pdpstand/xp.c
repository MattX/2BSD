/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)xp.c	1.2 (2.10BSD Berkeley) 1/12/88
 */

/*
 * SMD disk driver
 */
#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/hpreg.h"
#include "../machine/iopage.h"
#include "saio.h"

#define	XPADDR	((struct hpdevice *)0176700)

extern	char haveCSW;		/* bool, set if switch register exists */

/*
 * Drive type; declared so we can patch; see hpreg.h for more information.
 */
int	xptype = 0;

xpstrategy(io, func)
	register struct iob *io;
{
	register unit;
	register i;
	register nm_sect_per_cyl, nsect;
	daddr_t bn;
	int sn, cn, tn;

	if (((unit = io->i_unit) & 04) == 0)
		bn = io->i_bn;
	else {
		unit &= 03;
		bn = io->i_bn;
		bn -= io->i_boff;
		i = unit + 1;
		unit = bn%i;
		bn /= i;
		bn += io->i_boff;
	}

	XPADDR->hpcs2.w = unit;

	if ((XPADDR->hpds & HPDS_VV) == 0) {
		XPADDR->hpcs1.c[0] = HP_PRESET|HP_GO;
		XPADDR->hpof = HPOF_FMT22;
	}
	/*
 	 *	This next weirdness handled the look up into the Drive Type
 	 *	register to tell what type of disk we have here.
	 *	Look in switch register first (if there is one).
 	 *
 	 *	Note: No need to look up after the first time.
 	 */
	
	if (xptype == 0) {
		if (haveCSW && (*CSW == RP04 || *CSW == RP05 || *CSW == RP06
		    || *CSW == RM02 || *CSW == RM03 || *CSW == RM05
		    || *CSW == CAP  || *CSW == SI5  || *CSW == SI
		    || *CSW == RM2X || *CSW == RM5X || *CSW == DV))
			xptype = *CSW;
		else
			xptype = (XPADDR->hpdt & 077);
	}

	switch (xptype) {
	case RP04:
	case RP05:
	case RP06:
		nm_sect_per_cyl = HP_SECT * HP_TRAC;
		nsect = HP_SECT;
		break;

	case RM02:
	case RM03:
		nm_sect_per_cyl = RM_SECT * RM_TRAC;
		nsect = RM_SECT;
		break;

	case RM05:
	case RM5X:
		nm_sect_per_cyl = RM5_SECT * RM5_TRAC;
		nsect = RM5_SECT;
		break;

	case CAP:
		nm_sect_per_cyl = CAP_SECT * CAP_TRAC;
		nsect = CAP_SECT;
		break;

	case SI5:
		nm_sect_per_cyl = SI5_SECT * SI5_TRAC;
		nsect = SI5_SECT;
		break;

	case SI:
		nm_sect_per_cyl = SI_SECT * SI_TRAC;
		nsect = SI_SECT;
		break;

	case RM2X:
		nm_sect_per_cyl = RM2X_SECT * RM2X_TRAC;
		nsect = RM2X_SECT;
		break;

	case DV:
		nm_sect_per_cyl = DV_SECT * DV_TRAC;
		nsect = DV_SECT;
		break;

	default:
		printf("xp: unknown device type 0%o\n", xptype);
		return(-1);	
	}
	cn = bn/(nm_sect_per_cyl);
	sn = bn%(nm_sect_per_cyl);
	tn = sn/nsect;
	sn = sn%nsect;

	XPADDR->hpdc = cn;
	XPADDR->hpda = (tn << 8) + sn;
	XPADDR->hpba = io->i_ma;
	XPADDR->hpwc = -(io->i_cc>>1);
	unit = (segflag << 8) | HP_GO;
	if (func == READ)
		unit |= HP_RCOM;
	else if (func == WRITE)
		unit |= HP_WCOM;
	XPADDR->hpcs1.w = unit;
	while ((XPADDR->hpcs1.w & HP_RDY) == 0)
			continue;
	if (XPADDR->hpcs1.w & HP_TRE) {
		printf("disk error: cyl=%d track=%d sect=%d cs2=%o, er1=%o\n",
		    cn, tn, sn, XPADDR->hpcs2, XPADDR->hper1);
		return(-1);
	}
	return(io->i_cc);
}
