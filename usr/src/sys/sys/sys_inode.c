/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)sys_inode.c	1.7 (2.11BSD GTE) 1996/3/2
 */

#include "param.h"
#include "../machine/seg.h"

#include "user.h"
#include "proc.h"
#include "inode.h"
#include "buf.h"
#include "fs.h"
#include "file.h"
#include "stat.h"
#include "mount.h"
#include "conf.h"
#include "uio.h"
#include "ioctl.h"
#include "tty.h"
#include "kernel.h"
#include "systm.h"
#include "syslog.h"
#ifdef QUOTA
#include "quota.h"
#endif

int	ino_rw(), ino_ioctl(), ino_select(), ino_close();
struct 	fileops inodeops =
	{ ino_rw, ino_ioctl, ino_select, ino_close };

ino_rw(fp, uio)
	struct file *fp;
register struct uio *uio;
{
	register struct inode *ip = (struct inode *)fp->f_data;
	u_int count, error;
	int ioflag;

	if ((ip->i_mode&IFMT) != IFCHR)
		ILOCK(ip);
	uio->uio_offset = fp->f_offset;
	count = uio->uio_resid;
	if	(uio->uio_rw == UIO_READ)
		{
		error = rwip(ip, uio, fp->f_flag & FNONBLOCK ? IO_NDELAY : 0);
		fp->f_offset += (count - uio->uio_resid);
		}
	else
		{
		ioflag = 0;
		if	((ip->i_mode&IFMT) == IFREG && (fp->f_flag & FAPPEND))
			ioflag |= IO_APPEND;
		if	(fp->f_flag & FNONBLOCK)
			ioflag |= IO_NDELAY;
		error = rwip(ip, uio, ioflag);
		if	(ioflag & IO_APPEND)
			fp->f_offset = uio->uio_offset;
		else
			fp->f_offset += (count - uio->uio_resid);
		}
	if ((ip->i_mode&IFMT) != IFCHR)
		IUNLOCK(ip);
	return (error);
}

rdwri(rw, ip, base, len, offset, segflg, ioflg, aresid)
	enum uio_rw rw;
	struct inode *ip;
	caddr_t base;
	int len;
	off_t offset;
	enum uio_seg segflg;
	int ioflg;
register int *aresid;
{
	struct uio auio;
	struct iovec aiov;
register int error;

	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	aiov.iov_base = base;
	aiov.iov_len = len;
	auio.uio_rw = rw;
	auio.uio_resid = len;
	auio.uio_offset = offset;
	auio.uio_segflg = segflg;
	error = rwip(ip, &auio, ioflg);
	if (aresid)
		*aresid = auio.uio_resid;
	else
		if (auio.uio_resid)
			error = EIO;
	return (error);
}

rwip(ip, uio, ioflag)
	register struct inode *ip;
	register struct uio *uio;
	int ioflag;
{
	dev_t dev = (dev_t)ip->i_rdev;
	register struct buf *bp;
	off_t osize;
	daddr_t lbn, bn;
	int n, on, type, resid;
	int error = 0;

#ifdef	DIAGNOSTIC
	if (uio->uio_rw != UIO_READ && uio->uio_rw != UIO_WRITE)
		panic("rwip");
#endif
	if (uio->uio_offset < 0)
		return (EINVAL);
	type = ip->i_mode&IFMT;
/*
 * The write case below checks that i/o is done synchronously to directories
 * and that i/o to append only files takes place at the end of file.  The
 * 'log()' statements below should be ifdef'd.  Also, we do not panic on 
 * non-sync directory i/o - the sync bit is forced on.
*/
	if (uio->uio_rw == UIO_READ)
		ip->i_flag |= IACC;
	else
	   {
	   switch (type)
		{
		case IFREG:
		    if	(ioflag & IO_APPEND)
			uio->uio_offset = ip->i_size;
		    if	(ip->i_flags & APPEND && uio->uio_offset != ip->i_size)
			return(EPERM);
		    break;
		case IFDIR:
		    if  ((ioflag & IO_SYNC) == 0)
			{
			log(LOG_ERR, "rwip sync\n");
			ioflag |= IO_SYNC;
			}
		    break;
		case IFLNK:
		case IFBLK:
		case IFCHR:
		    break;
		default:
		    log(LOG_ERR, "rwip: %d\n", type);
		    return(EFTYPE);
		}
	   }

	if (type == IFCHR) {
		if (uio->uio_rw == UIO_READ)
			error = (*cdevsw[major(dev)].d_read)(dev, uio, ioflag);
		else {
			ip->i_flag |= IUPD|ICHG;
			error = (*cdevsw[major(dev)].d_write)(dev, uio, ioflag);
		}
		return (error);
	}
	if (uio->uio_resid == 0)
		return (0);
	if (uio->uio_rw == UIO_WRITE && type == IFREG &&
	    uio->uio_offset + uio->uio_resid >
	      u.u_rlimit[RLIMIT_FSIZE].rlim_cur) {
		psignal(u.u_procp, SIGXFSZ);
		return (EFBIG);
	}
#ifdef	QUOTA
	/*
	 * we do bytes, see the comment on 'blocks' in ino_stat().
	 *
	 * the simplfying assumption is made that the entire write will
	 * succeed, otherwise we have to check the quota on each block.
	 * can you say slow?  i knew you could.  SMS
	*/
	if ((type == IFREG || type == IFDIR || type == IFLNK) && 
	    uio->uio_rw == UIO_WRITE && !(ip->i_flag & IPIPE)) {
		if (uio->uio_offset + uio->uio_resid > ip->i_size) {
			QUOTAMAP();
			error = chkdq(ip, 
				uio->uio_offset+uio->uio_resid - ip->i_size,0);
			QUOTAUNMAP();
			if (error)
				return (error);
		}
	}
#endif
	if (type != IFBLK)
		dev = ip->i_dev;
	resid = uio->uio_resid;
	osize = ip->i_size;

	do {
		lbn = lblkno(uio->uio_offset);
		on = blkoff(uio->uio_offset);
		n = MIN((u_int)(DEV_BSIZE - on), uio->uio_resid);
		if (type != IFBLK) {
			if (uio->uio_rw == UIO_READ) {
				off_t diff = ip->i_size - uio->uio_offset;
				if (diff <= 0)
					return (0);
				if (diff < n)
					n = diff;
			bn = bmap(ip, lbn, B_READ, 0);
			}
			else
				bn = bmap(ip,lbn,B_WRITE,n == DEV_BSIZE ? 0: 1);
			if (u.u_error || uio->uio_rw == UIO_WRITE && (long)bn<0)
				return (u.u_error);
			if (uio->uio_rw == UIO_WRITE && uio->uio_offset + n > ip->i_size &&
			   (type == IFDIR || type == IFREG || type == IFLNK))
				ip->i_size = uio->uio_offset + n;
		} else {
			bn = lbn;
			rablock = bn + 1;
		}
		if (uio->uio_rw == UIO_READ) {
			if ((long)bn<0) {
				bp = geteblk();
				clrbuf(bp);
			} else if (ip->i_lastr + 1 == lbn)
				bp = breada(dev, bn, rablock);
			else
				bp = bread(dev, bn);
			ip->i_lastr = lbn;
		} else {
			if (n == DEV_BSIZE) 
				bp = getblk(dev, bn);
			else
				bp = bread(dev, bn);
/*
 * 4.3 didn't do this, but 2.10 did.  not sure why.
 * something about tape drivers don't clear buffers on end-of-tape
 * any longer (clrbuf can't be called from interrupt).
*/
			if (bp->b_resid == DEV_BSIZE) {
				bp->b_resid = 0;
				clrbuf(bp);
			}
		}
		n = MIN(n, DEV_BSIZE - bp->b_resid);
		if (bp->b_flags & B_ERROR) {
			error = EIO;
			brelse(bp);
			break;
		}
		u.u_error = uiomove(mapin(bp)+on, n, uio);
		mapout(bp);
		if (uio->uio_rw == UIO_READ) {
			if (n + on == DEV_BSIZE || uio->uio_offset == ip->i_size) {
				bp->b_flags |= B_AGE;
				if (ip->i_flag & IPIPE)
					bp->b_flags &= ~B_DELWRI;
			}
			brelse(bp);
		} else {
			if (ioflag & IO_SYNC)
				bwrite(bp);
			else if (n + on == DEV_BSIZE && !(ip->i_flag & IPIPE)) {
				bp->b_flags |= B_AGE;
				bawrite(bp);
			} else
				bdwrite(bp);
			ip->i_flag |= IUPD|ICHG;
			if (u.u_ruid != 0)
				ip->i_mode &= ~(ISUID|ISGID);
		}
	} while (u.u_error == 0 && uio->uio_resid && n != 0);
	if (error == 0)				/* XXX */
		error = u.u_error;		/* XXX */
	if (error && (uio->uio_rw == UIO_WRITE) && (ioflag & IO_UNIT) && 
		(type != IFBLK)) {
		itrunc(ip, osize);
		uio->uio_offset -= (resid - uio->uio_resid);
		uio->uio_resid = resid;
/*
 * Should back out the change to the quota here but that would be a lot
 * of work for little benefit.  Besides we've already made the assumption
 * that the entire write would succeed and users can't turn on the IO_UNIT
 * bit for their writes anyways.
*/
	}
#ifdef whybother
	if (!error && (ioflag & IO_SYNC))
		IUPDAT(ip, &time, &time, 1);
#endif
	return (error);
}


ino_ioctl(fp, com, data)
	register struct file *fp;
	register u_int com;
	caddr_t data;
{
	register struct inode *ip = ((struct inode *)fp->f_data);
	dev_t dev;

	switch (ip->i_mode & IFMT) {

	case IFREG:
	case IFDIR:
		if (com == FIONREAD) {
			if (fp->f_type==DTYPE_PIPE && !(fp->f_flag&FREAD))
				*(off_t *)data = 0;
			else
				*(off_t *)data = ip->i_size - fp->f_offset;
			return (0);
		}
		if (com == FIONBIO || com == FIOASYNC)	/* XXX */
			return (0);			/* XXX */
		/* fall into ... */

	default:
		return (ENOTTY);

	case IFCHR:
		dev = ip->i_rdev;
		u.u_r.r_val1 = 0;
		if (setjmp(&u.u_qsave)) {
			if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0)
				return(EINTR);
			u.u_eosys = RESTARTSYS;
			return (0);
		}
		return((*cdevsw[major(dev)].d_ioctl)(dev,com,data,fp->f_flag));
	}
}

ino_select(fp, which)
	struct file *fp;
	int which;
{
	register struct inode *ip = (struct inode *)fp->f_data;
	register dev_t dev;

	switch (ip->i_mode & IFMT) {

	default:
		return (1);		/* XXX */

	case IFCHR:
		dev = ip->i_rdev;
		return (*cdevsw[major(dev)].d_select)(dev, which);
	}
}

ino_stat(ip, sb)
	register struct inode *ip;
	register struct stat *sb;
{
	register struct icommon2 *ic2;

#ifdef	EXTERNALITIMES
	mapseg5(xitimes, xitdesc);
	ic2 = &((struct icommon2 *)SEG5)[ip - inode];
#else
	ic2 = &ip->i_ic2;
#endif

/*
 * inlined ITIMES which takes advantage of the common times pointer.
*/
	if (ip->i_flag & (IUPD|IACC|ICHG)) {
		ip->i_flag |= IMOD;
		if (ip->i_flag & IACC)
			ic2->ic_atime = time.tv_sec;
		if (ip->i_flag & IUPD)
			ic2->ic_mtime = time.tv_sec;
		if (ip->i_flag & ICHG)
			ic2->ic_ctime = time.tv_sec;
		ip->i_flag &= ~(IUPD|IACC|ICHG);
	}
	sb->st_dev = ip->i_dev;
	sb->st_ino = ip->i_number;
	sb->st_mode = ip->i_mode;
	sb->st_nlink = ip->i_nlink;
	sb->st_uid = ip->i_uid;
	sb->st_gid = ip->i_gid;
	sb->st_rdev = (dev_t)ip->i_rdev;
	sb->st_size = ip->i_size;
	sb->st_atime = ic2->ic_atime;
	sb->st_spare1 = 0;
	sb->st_mtime = ic2->ic_mtime;
	sb->st_spare2 = 0;
	sb->st_ctime = ic2->ic_ctime;
	sb->st_spare3 = 0;
	sb->st_blksize = MAXBSIZE;
	/*
	 * blocks are too tough to do; it's not worth the effort.
	 */
	sb->st_blocks = btodb(ip->i_size + MAXBSIZE - 1);
	sb->st_flags = ip->i_flags;
	sb->st_spare4[0] = 0;
	sb->st_spare4[1] = 0;
	sb->st_spare4[2] = 0;
#ifdef	EXTERNALITIMES
	normalseg5();
#endif
	return (0);
}

ino_close(fp)
	register struct file *fp;
{
	register struct inode *ip = (struct inode *)fp->f_data;
	register struct mount *mp;
	int flag, mode;
	dev_t dev;
	int (*cfunc)();

	if (fp->f_flag & (FSHLOCK | FEXLOCK))
		ino_unlock(fp, FSHLOCK | FEXLOCK);
	flag = fp->f_flag;
	dev = (dev_t)ip->i_rdev;
	mode = ip->i_mode & IFMT;
	ilock(ip);
	if (fp->f_type == DTYPE_PIPE) {
		if (ip->i_rsel) {
			selwakeup(ip->i_rsel, (long)(ip->i_flag & IRCOLL));
			ip->i_rsel = 0;
			ip->i_flag &= ~IRCOLL;
		}
		if (ip->i_wsel) {
			selwakeup(ip->i_wsel, (long)(ip->i_flag & IWCOLL));
			ip->i_wsel = 0;
			ip->i_flag &= ~IWCOLL;
		}
		ip->i_mode &= ~(IREAD|IWRITE);
		wakeup((caddr_t)ip+1);
		wakeup((caddr_t)ip+2);
	}
	iput(ip);
	fp->f_data = (caddr_t) 0;		/* XXX */
	switch (mode) {

	case IFCHR:
		cfunc = cdevsw[major(dev)].d_close;
		break;

	case IFBLK:
		/*
		 * We don't want to really close the device if it is mounted
		 */
/* MOUNT TABLE SHOULD HOLD INODE */
		for (mp = mount; mp < &mount[NMOUNT]; mp++)
			if (mp->m_inodp != NULL && mp->m_dev == dev)
				return;
		cfunc = bdevsw[major(dev)].d_close;
		break;

	default:
		return;
	}
	/*
	 * Check that another inode for the same device isn't active.
	 * This is because the same device can be referenced by two
	 * different inodes.
	 */
	for (fp = file; fp < fileNFILE; fp++) {
		if (fp->f_type != DTYPE_INODE)
			continue;
		if (fp->f_count && (ip = (struct inode *)fp->f_data) &&
		    ip->i_rdev == dev && (ip->i_mode&IFMT) == mode)
			return;
	}
	if (mode == IFBLK) {
		/*
		 * On last close of a block device (that isn't mounted)
		 * we must invalidate any in core blocks, so that
		 * we can, for instance, change floppy disks.
		 */
		bflush(dev);
		binval(dev);
	}
	if (setjmp(&u.u_qsave)) {
		/*
		 * If device close routine is interrupted,
		 * must return so closef can clean up.
		 */
		if (u.u_error == 0)
			u.u_error = EINTR;	/* ??? */
		return;
	}
	(*cfunc)(dev, flag, mode);
}

/*
 * Place an advisory lock on an inode.
 */
ino_lock(fp, cmd)
	register struct file *fp;
	int cmd;
{
	register int priority = PLOCK;
	register struct inode *ip = (struct inode *)fp->f_data;

	if ((cmd & LOCK_EX) == 0)
		priority += 4;
	if (setjmp(&u.u_qsave)) {
		if ((u.u_sigintr & sigmask(u.u_procp->p_cursig)) != 0)
			return(EINTR);
		u.u_eosys = RESTARTSYS;
		return (0);
	}
	/*
	 * If there's a exclusive lock currently applied
	 * to the file, then we've gotta wait for the
	 * lock with everyone else.
	 */
again:
	while (ip->i_flag & IEXLOCK) {
		/*
		 * If we're holding an exclusive
		 * lock, then release it.
		 */
		if (fp->f_flag & FEXLOCK) {
			ino_unlock(fp, FEXLOCK);
			continue;
		}
		if (cmd & LOCK_NB)
			return (EWOULDBLOCK);
		ip->i_flag |= ILWAIT;
		sleep((caddr_t)&ip->i_exlockc, priority);
	}
	if ((cmd & LOCK_EX) && (ip->i_flag & ISHLOCK)) {
		/*
		 * Must wait for any shared locks to finish
		 * before we try to apply a exclusive lock.
		 *
		 * If we're holding a shared
		 * lock, then release it.
		 */
		if (fp->f_flag & FSHLOCK) {
			ino_unlock(fp, FSHLOCK);
			goto again;
		}
		if (cmd & LOCK_NB)
			return (EWOULDBLOCK);
		ip->i_flag |= ILWAIT;
		sleep((caddr_t)&ip->i_shlockc, PLOCK);
		goto again;
	}
#ifdef	DIAGNOSTIC
	if (fp->f_flag & FEXLOCK)
		panic("ino_lock");
#endif
	if (cmd & LOCK_EX) {
		cmd &= ~LOCK_SH;
		ip->i_exlockc++;
		ip->i_flag |= IEXLOCK;
		fp->f_flag |= FEXLOCK;
	}
	if ((cmd & LOCK_SH) && (fp->f_flag & FSHLOCK) == 0) {
		ip->i_shlockc++;
		ip->i_flag |= ISHLOCK;
		fp->f_flag |= FSHLOCK;
	}
	return (0);
}

/*
 * Unlock a file.
 */
ino_unlock(fp, kind)
	register struct file *fp;
	int kind;
{
	register struct inode *ip = (struct inode *)fp->f_data;
	register int flags;

	kind &= fp->f_flag;
	if (ip == NULL || kind == 0)
		return;
	flags = ip->i_flag;
	if (kind & FSHLOCK) {
		if ((flags & ISHLOCK) == 0)
			panic("SHLOCK");
		if (--ip->i_shlockc == 0) {
			ip->i_flag &= ~ISHLOCK;
			if (flags & ILWAIT)
				wakeup((caddr_t)&ip->i_shlockc);
		}
		fp->f_flag &= ~FSHLOCK;
	}
	if (kind & FEXLOCK) {
		if ((flags & IEXLOCK) == 0)
			panic("EXLOCK");
		if (--ip->i_exlockc == 0) {
			ip->i_flag &= ~(IEXLOCK|ILWAIT);
			if (flags & ILWAIT)
				wakeup((caddr_t)&ip->i_exlockc);
		}
		fp->f_flag &= ~FEXLOCK;
	}
}

/*
 * Openi called to allow handler of special files to initialize and
 * validate before actual IO.
 */
openi(ip, mode)
	register struct inode *ip;
{
	register dev_t dev = ip->i_rdev;
	register int maj = major(dev);
	dev_t bdev;
	int error;

	switch (ip->i_mode&IFMT) {

	case IFCHR:
		if (ip->i_fs->fs_flags & MNT_NODEV)
			return(ENXIO);
		if ((u_int)maj >= nchrdev)
			return (ENXIO);
		if (mode & FWRITE) {
			/*
			 * When running in very secure mode, do not allow
			 * opens for writing of any disk character devices.
			 */
			if (securelevel >= 2 && isdisk(dev, IFCHR))
				return(EPERM);
			/*
			 * When running in secure mode, do not allow opens
			 * for writing of /dev/mem, /dev/kmem, or character
			 * devices whose corresponding block devices are
			 * currently mounted.
			 */
			if (securelevel >= 1) {
				if ((bdev = chrtoblk(dev)) != NODEV &&
					(error = ufs_mountedon(bdev)))
						return(error);
				if (iskmemdev(dev))
					return(EPERM);
			}
		}
		return ((*cdevsw[maj].d_open)(dev, mode, S_IFCHR));

	case IFBLK:
		if (ip->i_fs->fs_flags & MNT_NODEV)
			return(ENXIO);
		if ((u_int)maj >= nblkdev)
			return (ENXIO);
		/*
		 * When running in very secure mode, do not allow
		 * opens for writing of any disk block devices.
		 */
		if (securelevel >= 2 && (mode & FWRITE) && isdisk(dev, IFBLK))
			return(EPERM);
		/*
		 * Do not allow opens of block devices that are 
		 * currently mounted.
		 *
		 * 2.11BSD must relax this restriction to allow 'fsck' to
 		 * open the root filesystem (which is always mounted) during 
		 * a reboot.  Once in secure or very secure mode the 
		 * above restriction is fully effective.  On the otherhand
		 * fsck should 1) use the raw device, 2) not do sync calls...
		 */
		if (securelevel > 0 && (error = ufs_mountedon(dev)))
			return(error);
		return ((*bdevsw[maj].d_open)(dev, mode, S_IFBLK));
	}
	return (0);
}

/*
 * Revoke access the current tty by all processes.
 * Used only by the super-user in init
 * to give ``clean'' terminals at login.
 */
vhangup()
{

	if (!suser())
		return;
	if (u.u_ttyp == NULL)
		return;
	forceclose(u.u_ttyd);
	if ((u.u_ttyp->t_state) & TS_ISOPEN)
		gsignal(u.u_ttyp->t_pgrp, SIGHUP);
}

forceclose(dev)
	register dev_t dev;
{
	register struct file *fp;
	register struct inode *ip;

	for (fp = file; fp < fileNFILE; fp++) {
		if (fp->f_count == 0)
			continue;
		if (fp->f_type != DTYPE_INODE)
			continue;
		ip = (struct inode *)fp->f_data;
		if (ip == 0)
			continue;
		if ((ip->i_mode & IFMT) != IFCHR)
			continue;
		if (ip->i_rdev != dev)
			continue;
		fp->f_flag &= ~(FREAD|FWRITE);
	}
}
