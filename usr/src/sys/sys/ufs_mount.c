/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ufs_mount.c	1.6 (2.11BSD GTE) 1995/12/24
 */

#include "param.h"
#include "../machine/seg.h"

#include "systm.h"
#include "user.h"
#include "inode.h"
#include "fs.h"
#include "buf.h"
#include "mount.h"
#include "file.h"
#include "namei.h"
#include "conf.h"
#include "stat.h"
#include "disklabel.h"
#include "ioctl.h"
#ifdef QUOTA
#include "quota.h"
#endif

smount()
{
	register struct a {
		char	*fspec;
		char	*freg;
		int	flags;
	} *uap = (struct a *)u.u_ap;
	dev_t dev;
	register struct inode *ip;
	register struct fs *fs;
	register struct	nameidata *ndp = &u.u_nd;
	u_int lenon, lenfrom;
	char	mnton[MNAMELEN], mntfrom[MNAMELEN];

	u.u_error = getmdev(&dev, uap->fspec);
	if (u.u_error)
		return;
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = (caddr_t)uap->freg;
	ip = namei(ndp);
	if (ip == NULL)
		return;
/*
 * This is a hack to update the 'from' field for the root filesystem.  When
 * the kernel boots the string 'root_device' placed there as a place holder
 * until the "mount -a" is done from /etc/rc - at that time the name of the
 * root device is known and passed thru to here.  If '/' is the directory
 * then only the 'from' and 'on' fields are updated.
 *
 * The following two copyinstr calls will not fault because getmdev() or
 * namei() would have returned an error for invalid parameters.
*/
	copyinstr(uap->freg, mnton, sizeof (mnton) - 1, &lenon);
	copyinstr(uap->fspec, mntfrom, sizeof (mntfrom) - 1, &lenfrom);
	if	(mnton[0] == '/' && mnton[1] == '\0')
		{
		iput(ip);
		if	(dev != mount[0].m_dev)
			return(u.u_error = EINVAL);
		fs = &mount[0].m_filsys;
		goto updname;
		}
	if (ip->i_count != 1 || (ip->i_number == ROOTINO)) {
		iput(ip);
		u.u_error = EBUSY;
		return;
	}
	if ((ip->i_mode&IFMT) != IFDIR) {
		iput(ip);
		u.u_error = ENOTDIR;
		return;
	}

	fs = mountfs(dev, uap->flags, ip);
	if (fs == 0)
		return;
updname:
	mount_updname(fs, mnton, mntfrom, lenon, lenfrom);
}

mount_updname(fs, on, from, lenon, lenfrom)
	struct	fs	*fs;
	char	*on, *from;
	int	lenon, lenfrom;
	{
	struct	mount	*mp;
	register struct	xmount	*xmp;

	bzero(fs->fs_fsmnt, sizeof (fs->fs_fsmnt));
	bcopy(on, fs->fs_fsmnt, sizeof (fs->fs_fsmnt) - 1);
	mp = (struct mount *)((int)fs - offsetof(struct mount, m_filsys));
	xmp = (struct xmount *)SEG5;
	mapseg5(mp->m_extern, XMOUNTDESC);
	bzero(xmp, sizeof (struct xmount));
	bcopy(on, xmp->xm_mnton, lenon);
	bcopy(from, xmp->xm_mntfrom, lenfrom);
	normalseg5();
	}

/* this routine has races if running twice */
struct fs *
mountfs(dev, flags, ip)
	dev_t dev;
	int flags;
	struct inode *ip;
{
	register struct mount *mp = 0;
	struct buf *tp = 0;
	register struct fs *fs;
	register int error;
	int ronly = flags & MNT_RDONLY;
	int needclose = 0;
	int (*ioctl)();
	struct	partinfo dpart;

	error =
	    (*bdevsw[major(dev)].d_open)(dev, ronly ? FREAD : FREAD|FWRITE, S_IFBLK);
	if (error)
		goto out;
/*
 * Now make a check that the partition is really a filesystem if the 
 * underlying driver supports disklabels (there is an ioctl entry point 
 * and calling it does not return an error).
*/
	ioctl = cdevsw[blktochr(dev)].d_ioctl;
	if	(ioctl && !(*ioctl)(dev, DIOCGPART, &dpart, FREAD))
		{
		if	(dpart.part->p_fstype != FS_V71K)
			{
			error = EINVAL;
			goto out;
			}
		}
	needclose = 1;
	tp = bread(dev, SBLOCK);
	if (tp->b_flags & B_ERROR)
		goto out;
	for (mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
		if (mp->m_inodp != 0 && dev == mp->m_dev) {
			mp = 0;
			error = EBUSY;
			needclose = 0;
			goto out;
		}
	for (mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
		if (mp->m_inodp == 0)
			goto found;
	mp = 0;
	error = EMFILE;		/* needs translation */
	goto out;
found:
	mp->m_inodp = ip;	/* reserve slot */
	mp->m_dev = dev;
	fs = &mp->m_filsys;
	bcopy(mapin(tp), (caddr_t)fs, sizeof(struct fs));
	mapout(tp);
	brelse(tp);
	tp = 0;
	fs->fs_ronly = (ronly != 0);
	if (ronly == 0)
		fs->fs_fmod = 1;
	fs->fs_ilock = 0;
	fs->fs_flock = 0;
	fs->fs_nbehind = 0;
	fs->fs_lasti = 1;
	fs->fs_flags = flags;
	if (ip) {
		ip->i_flag |= IMOUNT;
		cacheinval(ip);
		IUNLOCK(ip);
	}
	return (fs);
out:
	if (error == 0)
		error = EIO;
	if (ip)
		iput(ip);
	if (mp)
		mp->m_inodp = 0;
	if (tp)
		brelse(tp);
	if (needclose) {
		(*bdevsw[major(dev)].d_close)(dev, 
			ronly? FREAD : FREAD|FWRITE, S_IFBLK);
		binval(dev);
	}
	u.u_error = error;
	return (0);
}

umount()
{
	struct a {
		char	*fspec;
	} *uap = (struct a *)u.u_ap;

	u.u_error = unmount1(uap->fspec);
}

unmount1(fname)
	caddr_t fname;
{
	dev_t dev;
	register struct mount *mp;
	register struct inode *ip;
	register int error;

	error = getmdev(&dev, fname);
	if (error)
		return (error);
	for (mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
		if (mp->m_inodp != NULL && dev == mp->m_dev)
			goto found;
	return (EINVAL);
found:
	xumount(dev);	/* remove unused sticky files from text table */
	nchinval(dev);	/* flush the name cache */
	update();
#ifdef QUOTA
	if (iflush(dev, mp->m_qinod) < 0)
#else
	if (iflush(dev) < 0)
#endif
		return (EBUSY);
#ifdef QUOTA
	QUOTAMAP();
	closedq(mp);
	QUOTAUNMAP();
	/*
	 * Here we have to iflush again to get rid of the quota inode.
	 * A drag, but it would be ugly to cheat, & this doesn't happen often
	 */
	(void)iflush(dev, (struct inode *)NULL);
#endif
	ip = mp->m_inodp;
	ip->i_flag &= ~IMOUNT;
	irele(ip);
	mp->m_inodp = 0;
	mp->m_dev = 0;
	(*bdevsw[major(dev)].d_close)(dev, 0, S_IFBLK);
	binval(dev);
	return (0);
}

/*
 * Common code for mount and umount.
 * Check that the user's argument is a reasonable
 * thing on which to mount, otherwise return error.
 */
getmdev(pdev, fname)
	caddr_t fname;
	dev_t *pdev;
{
	register dev_t dev;
	register struct inode *ip;
	register struct	nameidata *ndp = &u.u_nd;

	if (!suser())
		return (u.u_error);
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = fname;
	ip = namei(ndp);
	if (ip == NULL) {
		if (u.u_error == ENOENT)
			return (ENODEV); /* needs translation */
		return (u.u_error);
	}
	if ((ip->i_mode&IFMT) != IFBLK) {
		iput(ip);
		return (ENOTBLK);
	}
	dev = (dev_t)ip->i_rdev;
	iput(ip);
	if (major(dev) >= nblkdev)
		return (ENXIO);
	*pdev = dev;
	return (0);
}
