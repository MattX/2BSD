/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)dhuauto.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

#include "param.h"
#include "../machine/autoconfig.h"
#include "../machine/machparam.h"
#include "../pdpuba/dhvreg.h"

dhvprobe(addr)
	struct dhvdevice *addr;
{
    if ( grab ( &(addr->dhvcsr) ) & DHV_CS_MCLR )
	DELAY(35000L);
    if ( grab ( &(addr->dhvcsr) ) & (DHV_CS_MCLR|DHV_CS_DFAIL) )
	return ( 0 );
    stuff ( DHV_CS_RI | DHV_CS_RIE, &(addr->dhvcsr) );
    DELAY(3500L);
    stuff ( 0, &(addr->dhvcsr) );
    return(ACP_IFINTR);
}
