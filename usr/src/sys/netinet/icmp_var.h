/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Berkeley. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 *
 *	@(#)icmp_var.h	7.3 (Berkeley) 12/7/87
 */

/*
 * Variables related to this implementation
 * of the internet control message protocol.
 */
#ifdef BSD2_10
#define	icps_oldshort	icps_os
#define	icps_oldicmp	icps_oi
#define	icps_badcode	icps_bc
#define	icps_badlen	icps_bl
#endif

struct	icmpstat {
/* statistics related to icmp packets generated */
	long	icps_error;		/* # of calls to icmp_error */
	long	icps_oldshort;		/* no error 'cuz old ip too short */
	long	icps_oldicmp;		/* no error 'cuz old was icmp */
	long	icps_outhist[ICMP_MAXTYPE + 1];
/* statistics related to input messages processed */
 	long	icps_badcode;		/* icmp_code out of range */
	long	icps_tooshort;		/* packet < ICMP_MINLEN */
	long	icps_checksum;		/* bad checksum */
	long	icps_badlen;		/* calculated bound mismatch */
	long	icps_reflect;		/* number of responses */
	long	icps_inhist[ICMP_MAXTYPE + 1];
};

#ifdef SUPERVISOR
struct	icmpstat icmpstat;
#endif
