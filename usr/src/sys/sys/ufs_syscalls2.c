/*
 * 	@(#) 	ufs_syscalls2.c	  1.3 (2.11BSD) 1996/9/13
 *
 * ufs_syscalls was getting too large.  New UFS related system calls are
 * placed in this file.
*/

#include "param.h"
#include "../machine/seg.h"
#include "user.h"
#include "inode.h"
#include "buf.h"
#include "fs.h"
#include "namei.h"
#include "mount.h"
#include "kernel.h"

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

/*
 * 'ufs_sync' is the routine which syncs a single filesystem.  This was
 * created to replace 'update' which 'unmount' called.  It seemed silly to
 * sync _every_ filesystem when unmounting just one filesystem.
*/

ufs_sync(mp)
	register struct mount *mp;
	{
	register struct fs *fs;
	struct	buf *bp;
	int	error = 0;

	fs = &mp->m_filsys;
	if	(fs->fs_fmod && (mp->m_flags & MNT_RDONLY))
		{
		printf("fs = %s\n", fs->fs_fsmnt);
		panic("sync: rofs");
		}
	syncinodes(fs);		/* sync the inodes for this filesystem */
	bflush(mp->m_dev);	/* flush dirty data blocks */
#ifdef	QUOTA
	qsync(mp->m_dev);	/* sync the quotas */
#endif

/*
 * And lastly the superblock, if the filesystem was modified.
 * Write back modified superblocks. Consistency check that the superblock
 * of each file system is still in the buffer cache.
*/
	if	(fs->fs_fmod)
		{
		bp =  getblk(mp->m_dev, SUPERB);
		fs->fs_fmod = 0;
		fs->fs_time = time.tv_sec;
		bcopy(fs, mapin(bp), sizeof (struct fs));
		mapout(bp);
		bwrite(bp);
		error = geterror(bp);
		}
	return(error);
	}

/*
 * This is somewhat inefficient in that the inode table is scanned for each
 * filesystem but it didn't seem worth a page or two of code on something
 * which only happens every 30 seconds.
*/

syncinodes(fs)
	struct	fs *fs;
	{
	register struct inode *ip;

	/*
	 * Write back each (modified) inode.
	 */
	for	(ip = inode; ip < inodeNINODE; ip++)
		{
/*
 * Attempt to reduce the overhead by short circuiting the scan if the
 * inode is not for the filesystem being processed.
*/
		if	(ip->i_fs != fs)
			continue;
		if ((ip->i_flag & ILOCKED) != 0 || ip->i_count == 0 ||
			   (ip->i_flag & (IMOD|IACC|IUPD|ICHG)) == 0)
			continue;
		ip->i_flag |= ILOCKED;
		ip->i_count++;
		iupdat(ip, &time, &time, 0);
		iput(ip);
		}
	}
