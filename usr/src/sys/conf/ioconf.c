/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ioconf.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

#include "param.h"
#include "systm.h"

dev_t	rootdev = %ROOTDEV%,
	swapdev = %SWAPDEV%,
	pipedev = %PIPEDEV%;
daddr_t swplo = (daddr_t)%SWAPLO%;
int	nswap = %NSWAP%;

dev_t	dumpdev = %DUMPDEV%;
daddr_t	dumplo = (daddr_t)%DUMPLO%;
int	%DUMPROUTINE%();
int	(*dump)() = %DUMPROUTINE%;
