/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kernel.h	1.2 (2.11BSD GTE) 12/24/92
 */

/*
 * Global variables for the kernel
 */

#ifdef SUPERVISOR
long	startnet;			/* start of network data space */
#else
memaddr	malloc();

/* 1.1 */
long	hostid;
char	hostname[MAXHOSTNAMELEN];
int	hostnamelen;

/* 1.2 */
struct	timeval boottime;
struct	timeval time;
struct	timezone tz;			/* XXX */
int	adjdelta;
int	hz;
int	lbolt;				/* awoken once a second */
int	realitexpire();

short	avenrun[3];
#endif
