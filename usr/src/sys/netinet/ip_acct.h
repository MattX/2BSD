/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ip_acct.h	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
	This include file describes the data structures needed for ip
	accounting. ip accounting counts the number of packets coming
	in and going out over the local ethernet interface. Currently,
	"il" is the local interface. Change it to your favorite interface
	name.
*/

#ifndef BSD2_10
#define	IP_ACCT				/* doesn't work in 2.10BSD */
#endif

struct ip_acct {
	char c_d[2];			/* the bottom two bytes of ip addr */
	long pkt_cnt;			/* count of packets */
};

#define	N_IPHOSTS	64		/* number of hosts to count */

#define	INTERFACE	"ec0"		/* what interface to account */
