/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)mount.h	7.2.1 (2.11BSD GTE) 11/26/94
 */

/*
 * Mount structure.
 * One allocated on every mount.
 * Used to find the super block.
 */
struct	mount
{
	dev_t	m_dev;		/* device mounted */
	struct	fs m_filsys;	/* superblock data */
#define	m_flags	(m_filsys.fs_flags)
	struct	inode *m_inodp;	/* pointer to mounted on inode */
	struct	inode *m_qinod; /* QUOTA: pointer to quota file */
};

/*
 * Mount flags.
 */
#define	MNT_RDONLY	0x00000001	/* read only filesystem */
#define	MNT_SYNCHRONOUS	0x00000002	/* file system written synchronously */
#define	MNT_NOEXEC	0x00000004	/* can't exec from filesystem */
#define	MNT_NOSUID	0x00000008	/* don't honor setuid bits on fs */
#define	MNT_NODEV	0x00000010	/* don't interpret special files */

#if defined(KERNEL) && !defined(SUPERVISOR)
struct	mount mount[NMOUNT];
#endif
