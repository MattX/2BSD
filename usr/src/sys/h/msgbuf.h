/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)msgbuf.h	1.1 (2.10BSD Berkeley) 12/1/86
 */

#define	MSG_MAGIC	0x063061
#define	MSG_BSIZE	4096

struct	msgbuf {
	long	msg_magic;
	int	msg_bufx;
	int	msg_bufr;
	u_short	msg_click;
	char	*msg_bufc;
};
#if defined(KERNEL) && !defined(SUPERVISOR)
struct	msgbuf msgbuf;
#endif
