/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)rxauto.c	1.2 (2.11BSD GTE) 12/30/92
 */

#include "param.h"
#include "../machine/autoconfig.h"
#include "../machine/machparam.h"

#include "rxreg.h"

/*
 * rxprobe - check for rx
 */
rxprobe(addr,vector)
	struct rxdevice *addr;
	int vector;
{
	stuff(RX_INIT | RX_IE, (&(addr)->rxcs));
	DELAY(1000L);
	stuff(0, (&(addr)->rxcs));
	return(ACP_IFINTR);
}
