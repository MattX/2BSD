#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/filsys.h>
#include <sys/fblk.h>
#include <sys/conf.h>
#include <sys/buf.h>
#include <sys/inode.h>
#include <sys/ino.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/quota.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)alloc.c	3.4";
#endif

typedef	struct fblk *FBLKP;

#ifdef UCB_QUOTAS
/*
 *	Book keeping for quota system
 * interfaces to alloc() and free()
 *
 * count one block against the quota of the given inode.
 * if the quota has been exceeded, retrun NULL without allocating block.
 */
qalloc(ip, d)
register struct inode *ip;
dev_t d;
{
	register struct inode *qp, *rp;
	register struct buf *ret;

if (ip->i_quot) qprint(01)("qalloc: ip->i_number = %d\n", ip->i_number);
else qprint(04)("QALLOC: ip->i_quot == NULL\n");
	for (qp = ip->i_quot; qp != NULL; qp = qp->i_quot) {
		if (qp->i_un.i_qused >= qp->i_un.i_qmax) {
			/*
			 * A quota was exceeded. Undo the counts.
			 */
			for (rp = ip->i_quot; rp != qp; rp = rp->i_quot)
				rp->i_un.i_qused -= QCOUNT;
			u.u_error = EQUOT;
			return(NULL);
		}
		qp->i_un.i_qused += QCOUNT;
		qp->i_flag |= IUPD;
	}
	if((ret = alloc(d)) != NULL)
		return(ret);
	/*
	 * The block could not be allocated.
	 * Undo the quota count.
	 */
	for (qp = ip->i_quot; qp != NULL; qp = qp->i_quot)
		if(qp->i_un.i_qused != 0)
			qp->i_un.i_qused -= QCOUNT;
	return(NULL);
}
#endif

/*
 * alloc will obtain the next available
 * free disk block from the free list of
 * the specified device.
 * The super block has up to NICFREE remembered
 * free blocks; the last of these is read to
 * obtain NICFREE more . . .
 *
 * no space on dev x/y -- when
 * the free list is exhausted.
 */
struct buf *
alloc(dev)
dev_t dev;
{
	daddr_t bno;
	register struct filsys *fp;
	register struct buf *bp;
#ifdef	UCB_BUFOUT
	register caddr_t addr;
#endif

	fp = getfs(dev);
	while(fp->s_flock)
		sleep((caddr_t)&fp->s_flock, PINOD);
	do {
		if(fp->s_nfree <= 0)
			goto nospace;
		if (fp->s_nfree > NICFREE) {
			prdev("Bad free count", dev);
			goto nospace;
		}
		bno = fp->s_free[--fp->s_nfree];
		if(bno == 0)
			goto nospace;
	} while (badblock(fp, bno, dev));
	if(fp->s_nfree <= 0) {
		fp->s_flock++;
		bp = bread(dev, bno);
		if ((bp->b_flags&B_ERROR) == 0) {
#ifdef	UCB_BUFOUT
			addr = mapin(bp);
			fp->s_nfree = ((FBLKP) addr)->df_nfree;
			bcopy((caddr_t)((FBLKP) addr)->df_free,
			    (caddr_t)fp->s_free, sizeof(fp->s_free));
			mapout(bp);
#else
			fp->s_nfree = ((FBLKP)(bp->b_un.b_addr))->df_nfree;
			bcopy((caddr_t)((FBLKP)(bp->b_un.b_addr))->df_free,
			    (caddr_t)fp->s_free, sizeof(fp->s_free));
#endif
		}
		brelse(bp);
#ifdef	UCB_FSFIX

		/*
		 * Prevent ``dups in free''
		 */
		bp = getblk(dev, SUPERB);
		fp->s_fmod = 0;
		fp->s_time = time;

#ifdef	UCB_BUFOUT
#ifdef UCB_MOUNT
			bcopy((caddr_t)fp, mapin(bp), SBSIZE);
#else
			bcopy((caddr_t)fp, mapin(bp), BSIZE);
#endif
			mapout(bp);
#else
#ifdef UCB_MOUNT
			bcopy((caddr_t)fp, bp->b_un.b_addr, SBSIZE);
#else
			bcopy((caddr_t)fp, bp->b_un.b_addr, BSIZE);
#endif
#endif
		bwrite(bp);

#endif
		fp->s_flock = 0;
		wakeup((caddr_t)&fp->s_flock);
		if (fp->s_nfree <=0)
			goto nospace;
	}
	bp = getblk(dev, bno);
	clrbuf(bp);
	fp->s_fmod = 1;
	fp->s_tfree--;
	return(bp);

nospace:
	fp->s_nfree = 0;
	fp->s_tfree = 0;
	/* SHOULD PROBABLY HINDER THE ABILITY OF ONE PROCESS TO CAUSE THIS... */
	prdev("no space", dev);
	u.u_error = ENOSPC;
	return(NULL);
}

#ifdef UCB_QUOTAS
/*
 *	decrement the count for quotas
 */
qfree(ip, b)
register struct inode *ip;
daddr_t b;
{
	register struct inode *qp;

if (ip->i_quot) qprint(01)("qfree: ip->i_quot = d\n", ip->i_quot->i_number);
	for (qp = ip->i_quot; qp != NULL; qp = qp->i_quot)
		if (qp->i_un.i_qused) {
			qp->i_un.i_qused -= QCOUNT;
			qp->i_flag |= IUPD;
		}
	free(ip->i_dev, b);
}
#endif

/*
 * place the specified disk block
 * back on the free list of the
 * specified device.
 */
free(dev, bno)
dev_t dev;
daddr_t bno;
{
	register struct filsys *fp;
	register struct buf *bp;
#ifdef	UCB_BUFOUT
	register caddr_t addr;
#endif

	fp = getfs(dev);
	fp->s_fmod = 1;
	while(fp->s_flock)
		sleep((caddr_t)&fp->s_flock, PINOD);
	if (badblock(fp, bno, dev))
		return;
	if(fp->s_nfree <= 0) {
		fp->s_nfree = 1;
		fp->s_free[0] = 0;
	}
	if(fp->s_nfree >= NICFREE) {
		fp->s_flock++;
		bp = getblk(dev, bno);
#ifdef	UCB_BUFOUT
		addr = mapin(bp);
		((FBLKP) addr)->df_nfree = fp->s_nfree;
		bcopy((caddr_t)fp->s_free, (caddr_t)((FBLKP) addr)->df_free,
			sizeof(fp->s_free));
		mapout(bp);
#else
		((FBLKP)(bp->b_un.b_addr))->df_nfree = fp->s_nfree;
		bcopy((caddr_t)fp->s_free,
			(caddr_t)((FBLKP)(bp->b_un.b_addr))->df_free,
			sizeof(fp->s_free));
#endif
		fp->s_nfree = 0;
		bwrite(bp);
		fp->s_flock = 0;
		wakeup((caddr_t)&fp->s_flock);
	}
	fp->s_free[fp->s_nfree++] = bno;
	fp->s_tfree++;
	fp->s_fmod = 1;
}

/*
 * Check that a block number is in the
 * range between the I list and the size
 * of the device.
 * This is used mainly to check that a
 * garbage file system has not been mounted.
 *
 * bad block on dev x/y -- not in range
 */
badblock(fp, bn, dev)
register struct filsys *fp;
daddr_t bn;
dev_t dev;
{

	if (bn < fp->s_isize || bn >= fp->s_fsize) {
		prdev("bad block", dev);
		return(1);
	}
	return(0);
}

/*
 * Allocate an unused I node
 * on the specified device.
 * Used with file creation.
 * The algorithm keeps up to
 * NICINOD spare I nodes in the
 * super block. When this runs out,
 * a linear search through the
 * I list is instituted to pick
 * up NICINOD more.
 */
struct inode *
ialloc(dev)
dev_t dev;
{
	register struct filsys *fp;
	register struct buf *bp;
	register struct inode *ip;
	int i;
	struct dinode *dp;
	ino_t ino;
	daddr_t adr;
#ifdef	UCB_IHASH
	ino_t inobas;
	int first;
#endif

	fp = getfs(dev);
	while(fp->s_ilock)
		sleep((caddr_t)&fp->s_ilock, PINOD);
loop:
	if(fp->s_ninode > 0) {
		ino = fp->s_inode[--fp->s_ninode];
		if (ino < ROOTINO)
			goto loop;
		ip = iget(dev, ino);
		if(ip == NULL)
			return(NULL);
		if(ip->i_mode == 0) {
			for (i=0; i<NADDR; i++)
				ip->i_un.i_addr[i] = 0;
			fp->s_fmod = 1;
			fp->s_tinode--;
			return(ip);
		}
		/*
		 * Inode was allocated after all.
		 * Look some more.
		 */
		iput(ip);
		goto loop;
	}
	fp->s_ilock++;
#ifdef	UCB_IHASH
#ifdef TRACEIA
	printf("s_lasti %d, s_nbehind %d\n", fp->s_lasti, fp->s_nbehind);
#endif
	if (fp->s_nbehind < 4 * NICINOD) {
		first = 1;
		ino = fp->s_lasti;
		if (itoo(ino))
			panic("ialloc");
		adr = itod(ino);
	} else {
fromtop:
		first = 0;
		ino = 1;
		adr = SUPERB+1;
		fp->s_nbehind = 0;
	}
#ifdef TRACEIA
	printf("searching %x/%x from %d\n", major(dev), minor(dev), ino);
#endif
	for(; adr < fp->s_isize; adr++) {
		inobas = ino;
#else
	ino = 1;
	for(adr = SUPERB+1; adr < fp->s_isize; adr++) {
#endif
		bp = bread(dev, adr);
#ifdef	INBLK
		if ((bp->b_flags&B_CACHE) == 0)
			u.u_vm.vm_inblk--;		/* no charge! */
#endif
		if (bp->b_flags & B_ERROR) {
			brelse(bp);
			ino += INOPB;
			continue;
		}
#ifdef	UCB_BUFOUT
		dp = (struct dinode *) mapin(bp);
#else
		dp = bp->b_un.b_dino;
#endif
		for(i=0; i<INOPB; i++) {
			if(dp->di_mode != 0)
				goto cont;
#ifdef	UCB_IHASH
			if(ifind(dev, ino))
#else
			for(ip = &inode[0]; ip < &inode[NINODE]; ip++)
			if(dev==ip->i_dev && ino==ip->i_number)
#endif
				goto cont;
			fp->s_inode[fp->s_ninode++] = ino;
			if(fp->s_ninode >= NICINOD)
				break;
		cont:
			ino++;
			dp++;
		}
#ifdef	UCB_BUFOUT
		mapout(bp);
#endif
		brelse(bp);
		if(fp->s_ninode >= NICINOD)
			break;
	}
#ifdef	UCB_IHASH
	if (fp->s_ninode < NICINOD && first) {
#ifdef TRACEIA
		printf("found only %d inodes on dev %x/%x, looking from top\n",
		    fp->s_ninode, major(dev), minor(dev));
#endif
		goto fromtop;
	}
#ifdef TRACEIA
	printf("search of %x/%x ended at %d\n", major(dev), minor(dev), ino);
#endif
	fp->s_lasti = inobas;
#endif
	fp->s_ilock = 0;
	wakeup((caddr_t)&fp->s_ilock);
	if(fp->s_ninode > 0)
		goto loop;
	prdev("Out of inodes", dev);
	u.u_error = ENOSPC;
	return(NULL);
}

/*
 * Free the specified I node
 * on the specified device.
 * The algorithm stores up
 * to NICINOD I nodes in the super
 * block and throws away any more.
 */
ifree(dev, ino)
dev_t dev;
ino_t ino;
{
	register struct filsys *fp;

	fp = getfs(dev);
	fp->s_tinode++;
	if(fp->s_ilock)
		return;
#ifdef	UCB_IHASH
	if(fp->s_ninode >= NICINOD) {
		if (fp->s_lasti > ino)
			fp->s_nbehind++;
		return;
	}
#else
	if(fp->s_ninode >= NICINOD)
		return;
#endif
	fp->s_inode[fp->s_ninode++] = ino;
	fp->s_fmod = 1;
}

/*
 * getfs maps a device number into
 * a pointer to the incore super
 * block.
 * The algorithm is a linear
 * search through the mount table.
 * A consistency check of the
 * in core free-block and i-node
 * counts.
 *
 * bad count on dev x/y -- the count
 *	check failed. At this point, all
 *	the counts are zeroed which will
 *	almost certainly lead to "no space"
 *	diagnostic
 * panic: no fs -- the device is not mounted.
 *	this "cannot happen"
 */
struct filsys *
getfs(dev)
dev_t dev;
{
	register struct mount *mp;
	register struct filsys *fp;

	for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
	if(mp->m_bufp != NULL && mp->m_dev == dev) {
#ifdef UCB_MOUNT
		fp = mp->m_caddr;
#else
		fp = mp->m_bufp->b_un.b_filsys;
#endif
		if(fp->s_nfree > NICFREE || fp->s_ninode > NICINOD) {
#ifdef UCB_MOUNT
			printf("nfree = %d, ninode = %d, ", fp->s_nfree, fp->s_ninode);
#endif
			prdev("bad count", dev);
			fp->s_nfree = 0;
			fp->s_ninode = 0;
		}
		return(fp);
	}
	panic("no fs");
	return(NULL);
}

/*
 * update is the internal name of
 * 'sync'. It goes through the disk
 * queues to initiate sandbagged IO;
 * goes through the I nodes to write
 * modified nodes; and it goes through
 * the mount table to initiate modified
 * super blocks.
 */
update()
{
	register struct inode *ip;
	register struct mount *mp;
	register struct buf *bp;
	struct filsys *fp;

	if(updlock)
		return;
	updlock++;
	for(mp = &mount[0]; mp < &mount[NMOUNT]; mp++)
		if(mp->m_bufp != NULL) {
#ifdef UCB_MOUNT
			fp = mp->m_caddr;
#else
			fp = mp->m_bufp->b_un.b_filsys;
#endif
			if(fp->s_fmod==0 || fp->s_ilock!=0 ||
			   fp->s_flock!=0 || fp->s_ronly!=0)
				continue;
			bp = getblk(mp->m_dev, SUPERB);
			if (bp->b_flags & B_ERROR)
				continue;
			fp->s_fmod = 0;
			fp->s_time = time;
#ifdef	UCB_BUFOUT
#ifdef UCB_MOUNT
			bcopy((caddr_t)fp, mapin(bp), SBSIZE);
#else
			bcopy((caddr_t)fp, mapin(bp), BSIZE);
#endif
			mapout(bp);
#else
#ifdef UCB_MOUNT
			bcopy((caddr_t)fp, bp->b_un.b_addr, SBSIZE);
#else
			bcopy((caddr_t)fp, bp->b_un.b_addr, BSIZE);
#endif
#endif
			bwrite(bp);
		}
	for(ip = &inode[0]; ip < &inode[NINODE]; ip++)
		if((ip->i_flag&ILOCK)==0 && ip->i_count) {
			ip->i_flag |= ILOCK;
			ip->i_count++;
#ifdef	UCB_FSFIX
			iupdat(ip, &time, &time, 0);
#else
			iupdat(ip, &time, &time);
#endif
			iput(ip);
		}
	updlock = 0;
	bflush(NODEV);
}
