/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)mount.h	7.2.2 (2.11BSD GTE) 1995/12/24
 */

/*
 * file system statistics
 */
#include <sys/fs.h>

#define MNAMELEN 90	/* length of buffer for returned name */

struct statfs {
	short	f_type;			/* type of filesystem (see below) */
	u_short	f_flags;		/* copy of mount flags */
	short	f_bsize;		/* fundamental file system block size */
	short	f_iosize;		/* optimal transfer block size */
	daddr_t	f_blocks;		/* total data blocks in file system */
	daddr_t	f_bfree;		/* free blocks in fs */
	daddr_t	f_bavail;		/* free blocks avail to non-superuser */
	ino_t	f_files;		/* total file nodes in file system */
	ino_t	f_ffree;		/* free file nodes in fs */
	long	f_fsid[2];		/* file system id */
	long	f_spare[5];		/* spare for later */
	char	f_mntonname[MNAMELEN];	/* directory on which mounted */
	char	f_mntfromname[MNAMELEN];/* mounted filesystem */
};

/*
 * File system types.  Since only UFS is supported the others are not
 * specified at this time.
 */
#define	MOUNT_NONE	0
#define	MOUNT_UFS	1	/* Fast Filesystem */

/*
 * Mount structure.
 * One allocated on every mount.
 * Used to find the super block.
 */
struct	mount
{
	dev_t	m_dev;		/* device mounted */
	struct	fs m_filsys;	/* superblock data */
#define	m_flags	m_filsys.fs_flags
	struct	inode *m_inodp;	/* pointer to mounted on inode */
	struct	inode *m_qinod; /* QUOTA: pointer to quota file */
	memaddr	m_extern;	/* click address of mount table extension */
};

struct	xmount
	{
	char	xm_mntfrom[MNAMELEN];	/* /dev/xxxx mounted from */
	char	xm_mnton[MNAMELEN];	/* directory mounted on - this is the
					 * full(er) version of fs_fsmnt.
					*/
	};

#define	XMOUNTDESC	(((btoc(sizeof (struct xmount)) - 1) << 8) | RW)

/*
 * Mount flags.
 */
#define	MNT_RDONLY	0x00000001	/* read only filesystem */
#define	MNT_SYNCHRONOUS	0x00000002	/* file system written synchronously */
#define	MNT_NOEXEC	0x00000004	/* can't exec from filesystem */
#define	MNT_NOSUID	0x00000008	/* don't honor setuid bits on fs */
#define	MNT_NODEV	0x00000010	/* don't interpret special files */
#define	MNT_VISFLAGMASK	0x000000ff	/* user visible flags */

/*
 * Flags for various system call interfaces.
 * 
 * These aren't used for anything in the system and are present only
 * for source code compatibility reasons.
*/
#define	MNT_WAIT	1
#define	MNT_NOWAIT	2

#if defined(KERNEL) && !defined(SUPERVISOR)
struct	mount mount[NMOUNT];
#endif
