/*
 * No Copyright (c).  Placed in the public domain 1993.  Steven Schultz
 * (sms@wlv.iipo.gtegsc.com).
 */

#ifdef SYSLIBC_SCCS
_sccsid: <@(#)wait4.s	1.0 (GTE) 3/10/93\0>
	.even
#endif SYSLIBC_SCCS

#include "SYS.h"

SYSCALL(wait4,norm)
