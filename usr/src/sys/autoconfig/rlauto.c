/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)rlauto.c	2.1 (2.11BSD GTE) 12/30/92
 */

#include "param.h"
#include "../machine/autoconfig.h"
#include "../machine/machparam.h"

#include "rlreg.h"

rlprobe(addr,vector)
	struct rldevice *addr;
	int vector;
{
	stuff(RLDA_RESET | RLDA_GS, (&(addr->rlda)));
	DELAY(10L);
	stuff(RL_GETSTATUS | RL_IE , (&(addr->rlcs)));
	DELAY(10L);
	stuff(RL_CRDY, (&(addr->rlcs)));
	return(ACP_IFINTR);
}
