/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)conf.c	2.0 (2.11BSD) 4/20/91
 */

#include "../h/param.h"
#include "../h/inode.h"
#include "saio.h"

devread(io)
	register struct iob *io;
{

	return((*devsw[io->i_ino.i_dev].dv_strategy)(io, READ));
}

devwrite(io)
	register struct iob *io;
{
	return((*devsw[io->i_ino.i_dev].dv_strategy)(io, WRITE));
}

devopen(io)
	register struct iob *io;
{
	return((*devsw[io->i_ino.i_dev].dv_open)(io));
}

devclose(io)
	register struct iob *io;
{
	(*devsw[io->i_ino.i_dev].dv_close)(io);
}

nullsys()
{
	return(-1);
}

extern	int	xpstrategy(), xpopen();
extern	int	brstrategy(), bropen();
extern	int	rkstrategy(), rkopen();
extern	int	hkstrategy(), hkopen();
extern	int	rlstrategy(), rlopen();
extern	int	sistrategy(), siopen();
extern	int	rastrategy(), raopen(), raclose();
extern	int	tmstrategy(), tmopen(), tmclose();
extern	int	htstrategy(), htopen(), htclose();
extern	int	tsstrategy(), tsopen(), tsclose();
extern	int	tmscpstrategy(), tmscpopen(), tmscpclose();

extern	caddr_t	*XPcsr[], *BRcsr[], *RKcsr[], *HKcsr[], *RLcsr[];
extern	caddr_t	*SIcsr[], *RAcsr[], *TMcsr[], *HTcsr[], *TScsr[], *TMScsr[];

/*
 * NOTE!  This table must be in major device number order.  See /sys/pdp/conf.c
 *	  for the major device numbers.
*/

struct devsw devsw[] = {
	"ht",	htstrategy,	htopen,		htclose,	HTcsr, /* 0 */
	"tm",	tmstrategy,	tmopen,		tmclose,	TMcsr, /* 1 */
	"ts",	tsstrategy,	tsopen,		tsclose,	TScsr, /* 2 */
	"ram",	nullsys,	nullsys,	nullsys,	0,     /* 3 */
	"hk",	hkstrategy,	hkopen,		nullsys,	HKcsr, /* 4 */
	"ra",	rastrategy,	raopen,		raclose,	RAcsr, /* 5 */
	"rk",	rkstrategy,	rkopen,		nullsys,	RKcsr, /* 6 */
	"rl",	rlstrategy,	rlopen,		nullsys,	RLcsr, /* 7 */
	"rx",	nullsys,	nullsys,	nullsys,	0,     /* 8 */
	"si",	sistrategy,	siopen,		nullsys,	SIcsr, /* 9 */
	"xp",	xpstrategy,	xpopen,		nullsys,	XPcsr, /* 10 */
	"br",	brstrategy,	bropen,		nullsys,	BRcsr, /* 11 */
	"tms",  tmscpstrategy,	tmscpopen,	tmscpclose,	TMScsr,/* 12 */
	0,	0,		0,		0,
};

	int	ndevsw = (sizeof (devsw) / sizeof (devsw[0])) - 1;

	char	ADJcsr[] =
		{
		0,	/* HT = 0 */
		2,	/* TM = 1 */
		2,	/* TS = 2 */
		0,	/* RAM = 3 */
		0,	/* HK = 4 */
		0,	/* RA = 5 */
		4,	/* RK = 6 */
		0,	/* RL = 7 */
		0,	/* RX = 8 */
		0,	/* XP/SI = 9 */
		0,	/* XP = 10 */
		4,	/* BR =11 */
		0,	/* TMS = 12 */
		};
