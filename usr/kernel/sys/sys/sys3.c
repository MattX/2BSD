#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/ino.h>
#include <sys/reg.h>
#include <sys/buf.h>
#include <sys/filsys.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/inode.h>
#include <sys/file.h>
#include <sys/conf.h>
#include <sys/stat.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)sys3.c	3.2";
#endif


/*
 * the fstat system call.
 */
fstat()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		struct stat *sb;
	} *uap;

	uap = (struct a *)u.u_ap;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	stat1(fp->f_inode, uap->sb, fp->f_flag&FPIPE? fp->f_un.f_offset: 0);
}

/*
 * the stat system call.
 */
stat()
{
	register struct inode *ip;
	register struct a {
		char	*fname;
		struct stat *sb;
	} *uap;

	uap = (struct a *)u.u_ap;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	stat1(ip, uap->sb, (off_t)0);
	iput(ip);
}

/*
 * The basic routine for fstat and stat:
 * get the inode and pass appropriate parts back.
 */
stat1(ip, ub, pipeadj)
register struct inode *ip;
struct stat *ub;
off_t pipeadj;
{
	register struct dinode *dp;
	register struct buf *bp;
	struct stat ds;

#ifdef UCB_FSFIX
	iupdat(ip, &time, &time, 0);
#else
	iupdat(ip, &time, &time);
#endif
	/*
	 * first copy from inode table
	 */
	ds.st_dev = ip->i_dev;
	ds.st_ino = ip->i_number;
	ds.st_mode = ip->i_mode;
	ds.st_nlink = ip->i_nlink;
	ds.st_uid = ip->i_uid;
	ds.st_gid = ip->i_gid;
	ds.st_rdev = (dev_t)ip->i_un.i_rdev;
	ds.st_size = ip->i_size - pipeadj;
	/*
	 * next the dates in the disk
	 */
	bp = bread(ip->i_dev, itod(ip->i_number));
#ifdef	UCB_BUFOUT
	dp = (struct dinode *) mapin(bp);
#else
	dp = bp->b_un.b_dino;
#endif
	dp += itoo(ip->i_number);
	ds.st_atime = dp->di_atime;
	ds.st_mtime = dp->di_mtime;
	ds.st_ctime = dp->di_ctime;
#ifdef	UCB_BUFOUT
	mapout(bp);
#endif
	brelse(bp);
	if (copyout((caddr_t)&ds, (caddr_t)ub, sizeof(ds)) < 0)
		u.u_error = EFAULT;
}

/*
 * the dup system call.
 */
dup()
{
	register struct file *fp;
	register struct a {
		int	fdes;
		int	fdes2;
	} *uap;
	register i, m;

	uap = (struct a *)u.u_ap;
	m = uap->fdes & ~077;
	uap->fdes &= 077;
	fp = getf(uap->fdes);
	if(fp == NULL)
		return;
	if ((m&0100) == 0) {
		if ((i = ufalloc()) < 0)
			return;
	} else {
		i = uap->fdes2;
		if (i<0 || i>=NOFILE) {
			u.u_error = EBADF;
			return;
		}
		u.u_r.r_val1 = i;
	}
	if (i!=uap->fdes) {
		if (u.u_ofile[i]!=NULL)
			closef(u.u_ofile[i]);
		u.u_ofile[i] = fp;
		fp->f_count++;
	}
}

/*
 * the mount system call.
 */
smount()
{
	dev_t dev;
	register struct inode *ip;
	register struct mount *mp;
	struct mount *smp;
	register struct filsys *fp;
	struct buf *bp;
	register struct a {
		char	*fspec;
		char	*freg;
		int	ronly;
	} *uap;

	uap = (struct a *)u.u_ap;
	dev = getmdev();
	if(u.u_error || !suser())
		return;
	u.u_dirp = (caddr_t)uap->freg;
	ip = namei(uchar, 0);
	if(ip == NULL)
		return;
	if(ip->i_count!=1 || (ip->i_mode&(IFBLK&IFCHR))!=0)
		goto out;
	smp = NULL;
	for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++) {
		if(mp->m_bufp != NULL) {
			if(dev == mp->m_dev)
				goto out;
		} else
		if(smp == NULL)
			smp = mp;
	}
	mp = smp;
	if(mp == NULL)
		goto out;
	(*bdevsw[major(dev)].d_open)(dev, !uap->ronly);
	if(u.u_error)
		goto out;
	bp = bread(dev, SUPERB);
	if(u.u_error) {
		brelse(bp);
		goto out1;
	}
	mp->m_inodp = ip;
	mp->m_dev = dev;
#ifdef UCB_MOUNT
	if (((mp-mount)%NSBPFSB) == 0 && mp->m_caddr == NULL) {
		mp->m_bufp = geteblk();
		mount1(mp);
	} else
		mp->m_bufp = (struct buf *)1;
#else
	mp->m_bufp = geteblk();
#endif
#ifdef	UCB_BUFOUT
#ifdef UCB_MOUNT
	bcopy((caddr_t) mapin(bp), mp->m_caddr, SBSIZE);
#else
	bcopy((caddr_t) mapin(bp), mp->m_bufp->b_un.b_addr, BSIZE);
#endif
	mapout(bp);
#else
#ifdef UCB_MOUNT
	bcopy((caddr_t)bp->b_un.b_addr, mp->m_caddr, SBSIZE);
#else
	bcopy((caddr_t)bp->b_un.b_addr, mp->m_bufp->b_un.b_addr, BSIZE);
#endif
#endif
#ifdef UCB_MOUNT
	fp = mp->m_caddr;
#else
	fp = mp->m_bufp->b_un.b_filsys;
#endif
	fp->s_ilock = 0;
	fp->s_flock = 0;
	fp->s_ronly = uap->ronly & 1;
#ifdef	UCB_IHASH
	fp->s_lasti = 1;
	fp->s_nbehind = 0;
#endif
	brelse(bp);
	ip->i_flag |= IMOUNT;
	prele(ip);
	return;

out:
	u.u_error = EBUSY;
out1:
	iput(ip);
}

/*
 * the umount system call.
 */
sumount()
{
	dev_t dev;
	register struct inode *ip;
	register struct mount *mp;
	struct buf *bp;
	struct buf *dp;		/* Klein & McConnell fix */
	register struct a {
		char	*fspec;
	};

	dev = getmdev();
	if(u.u_error || !suser())
		return;
	xumount(dev);	/* remove unused sticky files from text table */
	update();
	for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
		if(mp->m_bufp != NULL && dev == mp->m_dev)
			goto found;
	u.u_error = EINVAL;
	return;

found:
	for(ip = &inode[0]; ip < &inode[NINODE]; ip++)
		if(ip->i_number != 0 && dev == ip->i_dev) {
			u.u_error = EBUSY;
			return;
		}
	(*bdevsw[major(dev)].d_close)(dev, 0);
	dp = bdevsw[major(dev)].d_tab;
	for (bp=dp->b_forw; bp!=dp; bp=bp->b_forw) {
		spl6();
		if (bp->b_dev == dev) {
#ifdef UCB_BHASH
			bunhash(bp);
#endif
			bp->b_dev = NODEV;
		}
		spl0();
	}
	ip = mp->m_inodp;
	ip->i_flag &= ~IMOUNT;
	plock(ip);
	iput(ip);
	bp = mp->m_bufp;
	mp->m_bufp = NULL;
#ifndef UCB_MOUNT
#ifdef	UCB_BUFOUT
	abrelse(bp);
#else
	brelse(bp);
#endif
#endif
}

/*
 * Common code for mount and umount.
 * Check that the user's argument is a reasonable
 * thing on which to mount, and return the device number if so.
 */
dev_t
getmdev()
{
	dev_t dev;
	register struct inode *ip;

	ip = namei(uchar, 0);
	if(ip == NULL)
		return(NODEV);
	if((ip->i_mode&IFMT) != IFBLK)
		u.u_error = ENOTBLK;
	dev = (dev_t)ip->i_un.i_rdev;
	if(major(dev) >= nblkdev)
		u.u_error = ENXIO;
	iput(ip);
	return(dev);
}

#ifdef UCB_MOUNT
/*
 * distribute a buffer's address through
 * NSBPFSB succeeding mount table entries
 */
mount1(mp)
register struct mount *mp;
{
	register a, i;

	a = mp->m_bufp->b_un.b_addr;
	for (i=0; i<NSBPFSB; i++)
		(mp++)->m_caddr = a + sizeof(struct filsys) * i;
}
#endif
