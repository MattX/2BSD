/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)hk.c	2.0 (2.11BSD) 4/20/91
 */

/*
 *	RK06/07 disk driver for standalone
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/hkreg.h"
#include "saio.h"

#define	NHK	2
#define	NSECT	22
#define	NTRAC	3

	struct	hkdevice *HKcsr[NHK + 1] =
		{
		(struct hkdevice *)0177440,
		(struct hkdevice *)0,
		(struct hkdevice *)-1
		};

int hk_drvtyp[NHK][8];
char hk_mntflg[NHK][8];

hkstrategy(io, func)
	register struct iob *io;
{
	register unit, com;
	register struct hkdevice *hkaddr;
	daddr_t bn;
	int sn, cn, tn, ctlr;

	unit = UNITn(io->i_unit);
	ctlr = CTLRn(io->i_unit);
	hkaddr = HKcsr[ctlr];
	if (hk_mntflg[ctlr][unit] != '1') {
		hk_drvtyp[ctlr][unit] = 0;
		hkaddr->hkcs2 = unit;
		hkaddr->hkcs1 = HK_SELECT|HK_GO;
		while ((hkaddr->hkcs1 & HK_CRDY) == 0)
			continue;
		if (hkaddr->hkcs1 & HK_CERR && hkaddr->hker & HKER_DTYE) {
			hk_drvtyp[ctlr][unit] = 02000;
		}
		hk_mntflg[ctlr][unit] = '1';
	}
	bn = io->i_bn;
	hkaddr->hkcs2 = HKCS2_SCLR;
	while ((hkaddr->hkcs1 & HK_CRDY) == 0)
		continue;
	hkaddr->hkcs2 = unit;
	hkaddr->hkcs1 = hk_drvtyp[ctlr][unit]|HK_SELECT|HK_GO;
	while ((hkaddr->hkcs1 & HK_CRDY) == 0)
		continue;

	if ((hkaddr->hkds & HKDS_VV) == 0) {
		hkaddr->hkcs1 = hk_drvtyp[ctlr][unit]|HK_PACK|HK_GO;
		while ((hkaddr->hkcs1 & HK_CRDY) == 0)
			continue;
	}
	cn = bn/(NSECT*NTRAC);
	sn = bn%(NSECT*NTRAC);
	tn = sn/NSECT;
	sn = sn%NSECT;

	hkaddr->hkcyl = cn;
	hkaddr->hkda = (tn<<8) | sn;
	hkaddr->hkba = io->i_ma;
	hkaddr->hkwc = -(io->i_cc>>1);
	com = hk_drvtyp[ctlr][unit]|(segflag << 8) | HK_GO;
	if (func == READ)
		com |= HK_READ;
	else if (func == WRITE)
		com |= HK_WRITE;
	hkaddr->hkcs1 = com;

	while ((hkaddr->hkcs1 & HK_CRDY) == 0)
		continue;

	if (hkaddr->hkcs1 & HK_CERR) {
		printf("hk%d,%d err: cy=%d tr=%d sc=%d cs2=%d er=%o\n",
			ctlr, unit, cn, tn, sn, hkaddr->hkcs2, hkaddr->hker);
		return(-1);
	}
	return(io->i_cc);
}

hkopen(io)
	struct iob *io;
{
	return(genopen(NHK, io));
}
