/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)xp.c	2.1 (2.11BSD) 1995/06/08
 */

/*
 * SMD disk driver
 */
#include "../h/param.h"
#include "../pdpuba/hpreg.h"
#include "../machine/iopage.h"
#include "saio.h"

#define	NXP	2

	struct	hpdevice *XPcsr[NXP + 1] =
		{
		(struct hpdevice *)0176700,
		(struct hpdevice *)0,
		(struct hpdevice *)-1
		};

/*
 * Drive type; declared so we can patch; see hpreg.h for more information.
 */
int	xptype[NXP] = 0;

xpstrategy(io, func)
	register struct iob *io;
{
	int unit = io->i_unit;
	int ctlr = io->i_ctlr;
	int i;
	register nm_sect_per_cyl, nsect;
	daddr_t bn;
	int sn, cn, tn, bae, lo16;
	struct hpdevice *xpaddr = XPcsr[ctlr];

	bn = io->i_bn;
	/*
	 * The weirdness involving bit 2 (04) in the unit # was removed
	 * because 1) the xp driver in the kernel didn't know about it,
	 * 2) no mention was made in any .h file or documentation/notes
	*/
	xpaddr->hpcs2.w = unit;

	if ((xpaddr->hpds & HPDS_VV) == 0) {
		xpaddr->hpcs1.c[0] = HP_PRESET|HP_GO;
		xpaddr->hpof = HPOF_FMT22;
	}
	/*
 	 *	The weirdness of (possibly) looking at the console switch
 	 *	register to tell what type of disk we have here was removed
	 *	because 1) most systems do not have one, 2) space contraints
	 *	imposed by adding multi controller support were creating a
	 * 	size problem, and 3) an alternative method is provided for
	 *	setting the type (the address and content is printed).
 	 */
	
	if (xptype[ctlr] == 0) {
		xptype[ctlr] = (xpaddr->hpdt & 077);
		printf("\n&xptype=0%o xptype[%d]=0%o\n", xptype, ctlr, 
			xptype[ctlr]);
	}

	switch (xptype[ctlr]) {
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
		printf("xp%d: unknown type 0%o\n", ctlr, xptype[ctlr]);
		return(-1);	
	}
	cn = bn/(nm_sect_per_cyl);
	sn = bn%(nm_sect_per_cyl);
	tn = sn/nsect;
	sn = sn%nsect;

	iomapadr(io->i_ma, &bae, &lo16);
	xpaddr->hpdc = cn;
	xpaddr->hpda = (tn << 8) + sn;
	xpaddr->hpba = (caddr_t)lo16;
	xpaddr->hpwc = -(io->i_cc>>1);
	i = (bae << 8) | HP_GO;
	if (func == READ)
		i |= HP_RCOM;
	else if (func == WRITE)
		i |= HP_WCOM;
	xpaddr->hpcs1.w = i;
	while ((xpaddr->hpcs1.w & HP_RDY) == 0)
			continue;
	if (xpaddr->hpcs1.w & HP_TRE) {
		printf("xp%d,%d err cy=%d tr=%d sc=%d cs2=%o er1=%o\n",
		    ctlr, unit, cn, tn, sn, xpaddr->hpcs2, xpaddr->hper1);
		return(-1);
	}
	return(io->i_cc);
}

xpopen(io)
	struct iob *io;
{
	return(genopen(NXP, io));
}
