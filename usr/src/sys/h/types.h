/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)types.h	1.4 (2.11BSD Berkeley) 1996/1/12
 */

#ifndef _TYPES_
#define	_TYPES_

/*
 * Basic system types and major/minor device constructing/busting macros.
 */

/* major part of a device */
#define	major(x)	((int)(((int)(x)>>8)&0377))

/* minor part of a device */
#define	minor(x)	((int)((x)&0377))

/* make a device number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef unsigned long	u_long;		/* see this! unsigned longs at last! */
typedef	unsigned short	ushort;		/* sys III compat */

#ifdef pdp11
typedef	struct	_physadr { short r[1]; } *physadr;
typedef	struct	label_t	{
	int	val[8];			/* regs 2-7, __ovno and super SP */
} label_t;
#endif
typedef	struct	_quad { long val[2]; } quad;
typedef	long	daddr_t;
typedef	char *	caddr_t;
typedef	u_short	ino_t;
typedef	long	swblk_t;
typedef	u_int	size_t;
typedef	int	ssize_t;
typedef	long	time_t;
typedef	short	dev_t;
typedef	long	off_t;
typedef	u_short	uid_t;
typedef	u_short	gid_t;
typedef	int	pid_t;
typedef	u_short	mode_t;

#define	NBBY	8		/* number of bits in a byte */
/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */
#ifndef	FD_SETSIZE
#define	FD_SETSIZE	32
#endif

typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */
#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[1];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1L << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1L << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1L << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))

typedef char	bool_t;		/* boolean */
typedef u_int	memaddr;	/* core or swap address */
typedef long	ubadr_t;	/* unibus address */

#endif
