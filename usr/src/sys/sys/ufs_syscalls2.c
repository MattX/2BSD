/*
 * 	@(#) 	ufs_syscalls2.c	  1.2 (2.11BSD) 1995/12/31
 *
 * ufs_syscalls was getting too large.  New UFS related system calls are
 * placed in this file.
*/

#include "param.h"
#include "../machine/seg.h"
#include "user.h"
#include "inode.h"
#include "fs.h"
#include "namei.h"
#include "mount.h"

statfs()
	{
	register struct a
		{
		char	*path;
		struct	statfs	*buf;
		} *uap = (struct a *)u.u_ap;
	register struct	inode	*ip;
	register struct nameidata *ndp = &u.u_nd;
	struct	mount	*mp;

	ndp->ni_nameiop = LOOKUP|FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = uap->path;
	ip = namei(ndp);
	if	(!ip)
		return(u.u_error);
	mp = (struct mount *)((int)ip->i_fs - offsetof(struct mount, m_filsys));
	iput(ip);
	u.u_error = statfs1(mp, uap->buf);
	return(u.u_error);
	}

fstatfs()
	{
	register struct a
		{
		int	fd;
		struct	statfs *buf;
		} *uap = (struct a *)u.u_ap;
	register struct inode *ip;
	struct	mount *mp;

	ip = getinode(uap->fd);
	if	(!ip)
		return(u.u_error);
	mp = (struct mount *)((int)ip->i_fs - offsetof(struct mount, m_filsys));
	u.u_error = statfs1(mp, uap->buf);
	return(u.u_error);
	}

statfs1(mp, sbp)
	struct	mount	*mp;
	struct	statfs	*sbp;
	{
	struct	statfs	sfs;
	register struct	statfs	*sfsp;
	struct	xmount	*xmp = (struct xmount *)SEG5;
	struct	fs	*fs = &mp->m_filsys;

	sfsp = &sfs;
	sfsp->f_type = MOUNT_UFS;
	sfsp->f_bsize = MAXBSIZE;
	sfsp->f_iosize = MAXBSIZE;
	sfsp->f_blocks = fs->fs_fsize - fs->fs_isize;
	sfsp->f_bfree = fs->fs_tfree;
	sfsp->f_bavail = fs->fs_tfree;
	sfsp->f_files = (fs->fs_isize - 2) * INOPB;
	sfsp->f_ffree = fs->fs_tinode;

	mapseg5(mp->m_extern, XMOUNTDESC);
	bcopy(xmp->xm_mnton, sfsp->f_mntonname, MNAMELEN);
	bcopy(xmp->xm_mntfrom, sfsp->f_mntfromname, MNAMELEN);
	normalseg5();
	sfsp->f_flags = mp->m_flags & MNT_VISFLAGMASK;
	return(copyout(sfsp, sbp, sizeof (struct statfs)));
	}

getfsstat()
	{
	register struct a
		{
		struct	statfs	*buf;
		int	bufsize;
		u_int	flags;
		} *uap = (struct a *)u.u_ap;
	register struct	mount *mp;
	caddr_t	sfsp;
	int	count, maxcount, error;

	maxcount = uap->bufsize / sizeof (struct statfs);
	sfsp = (caddr_t)uap->buf;
	count = 0;
	for	(mp = mount; mp < &mount[NMOUNT]; mp++)
		{
		if	(mp->m_inodp == NULL)
			continue;
		if	(count < maxcount)
			{
			if	(error = statfs1(mp, sfsp))
				return(u.u_error = error);
			sfsp += sizeof (struct statfs);
			}
		count++;
		}
	if	(sfsp && count > maxcount)
		u.u_r.r_val1 = maxcount;
	else
		u.u_r.r_val1 = count;
	return(0);
	}
