/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)hk.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 *	RK06/07 disk driver for standalone
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "../pdpuba/hkreg.h"
#include "saio.h"


#define	HKADDR	((struct hkdevice *)0177440)

#define	NHK	8
#define	NSECT	22
#define	NTRAC	3

int hk_drvtyp[NHK];
char hk_mntflg[NHK];


hkstrategy(io, func)
	register struct iob *io;
{
	register unit, com;
	register i;
	daddr_t bn;
	int sn, cn, tn;

	unit = io->i_unit;
	if (hk_mntflg[unit] != '1') {
		hk_drvtyp[unit] = 0;
		HKADDR->hkcs2 = unit;
		HKADDR->hkcs1 = HK_SELECT|HK_GO;
		while ((HKADDR->hkcs1 & HK_CRDY) == 0)
			continue;
		if (HKADDR->hkcs1 & HK_CERR && HKADDR->hker & HKER_DTYE) {
			hk_drvtyp[unit] = 02000;
		}
		hk_mntflg[unit] = '1';
	}
	bn = io->i_bn;
	HKADDR->hkcs2 = HKCS2_SCLR;
	while ((HKADDR->hkcs1 & HK_CRDY) == 0)
		continue;
	HKADDR->hkcs2 = unit;
	HKADDR->hkcs1 = hk_drvtyp[unit]|HK_SELECT|HK_GO;
	while ((HKADDR->hkcs1 & HK_CRDY) == 0)
		continue;

	if ((HKADDR->hkds & HKDS_VV) == 0) {
		HKADDR->hkcs1 = hk_drvtyp[unit]|HK_PACK|HK_GO;
		while ((HKADDR->hkcs1 & HK_CRDY) == 0)
			continue;
	}
	cn = bn/(NSECT*NTRAC);
	sn = bn%(NSECT*NTRAC);
	tn = sn/NSECT;
	sn = sn%NSECT;

	HKADDR->hkcyl = cn;
	HKADDR->hkda = (tn<<8) | sn;
	HKADDR->hkba = io->i_ma;
	HKADDR->hkwc = -(io->i_cc>>1);
	com = hk_drvtyp[unit]|(segflag << 8) | HK_GO;
	if (func == READ)
		com |= HK_READ;
	else if (func == WRITE)
		com |= HK_WRITE;
	HKADDR->hkcs1 = com;

	while ((HKADDR->hkcs1 & HK_CRDY) == 0)
		continue;

	if (HKADDR->hkcs1 & HK_CERR) {
		printf("disk error: cyl=%d track=%d sect=%d cs2=%d err=%o\n",
			cn, tn, sn, HKADDR->hkcs2, HKADDR->hker);
		return(-1);
	}
	return(io->i_cc);
}
