/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)msgbuf.h	1.1 (2.10BSD Berkeley) 12/1/86
 */

#define	MSG_MAGIC	0x063061
#ifdef SMALL
#define	MSG_BSIZE	(128 - 1 * sizeof (long))
#else
#define	MSG_BSIZE	(512 - 1 * sizeof (long))
#endif
struct	msgbuf {
	long	msg_bufx;
	char	msg_bufc[MSG_BSIZE];
};
#if defined(KERNEL) && !defined(SUPERVISOR)
struct	msgbuf msgbuf;
#endif
