/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)tsauto.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

#include "param.h"
#include "../machine/autoconfig.h"
#include "../machine/machparam.h"

#include "tmreg.h"

tsprobe(addr)
	struct tsdevice *addr;
{
	extern int errno;

	/*
	 * Unfortunately the TS and TM CSRs overlap.  So simply testing for
	 * presence of a TS register isn't good enough.  So we try to do a
	 * TM select of drive 2.  If we get a bus fault or if the select
	 * works, we don't have a TS.
	 */
	errno = 0;
	stuff(01000, &(((struct tmdevice *)addr)->tmcs));
	if (errno || (grab(&(((struct tmdevice *)addr)->tmcs)) & 01000))
		return(ACP_NXDEV);
	return(ACP_EXISTS);
}
