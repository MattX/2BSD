#include <sys/param.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/inode.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/quota.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)subr.c	3.2";
#endif


/*
 * Bmap defines the structure of file system storage
 * by returning the physical block number on a device given the
 * inode and the logical block number in a file.
 * When convenient, it also leaves the physical
 * block number of the next block of the file in rablock
 * for use in read-ahead.
 */
daddr_t
bmap(ip, bn, rwflg)
register struct inode *ip;
daddr_t bn;
{
	register i;
	struct buf *bp, *nbp;
	int j, sh;
	daddr_t nb, *bap, ra;
	dev_t dev;

	if(bn < 0) {
		u.u_error = EFBIG;
		return((daddr_t)0);
	}
	dev = ip->i_dev;
	ra = rablock = 0;

	/*
	 * blocks 0..NADDR-4 are direct blocks
	 */
	if(bn < NADDR-3) {
		i = bn;
		nb = ip->i_un.i_addr[i];
#ifdef UCB_QUOTAS
		if (rwflg == B_WRITE)
		    qprint(02)("BMAP: ip->number = %d, nb = %D\n",ip->i_number,nb);
		if (nb == 0) {
			/*
			 * use qalloc() instead of alloc() to keep track
			 * of quotas
			 */
			if (rwflg == B_READ || (bp = qalloc(ip,dev)) == NULL)
#else
		if(nb == 0) {
			if(rwflg==B_READ || (bp = alloc(dev))==NULL)
#endif
				return((daddr_t)-1);
#ifndef UCB_NKB
			nb = bp->b_blkno;
#else
			nb = dbtofsb(bp->b_blkno);
#endif
#ifdef UCB_FSFIX
			if ((ip->i_mode&IFMT) == IFDIR)
				/*
				 * Write directory blocks synchronously
				 * so they never appear with garbage in
				 * them on the disk.
				 */
				bwrite(bp);
			else
#endif
				bdwrite(bp);
			ip->i_un.i_addr[i] = nb;
			ip->i_flag |= IUPD|ICHG;
		}
		if(i < NADDR-4)
			rablock = ip->i_un.i_addr[i+1];
		return(nb);
	}

	/*
	 * addresses NADDR-3, NADDR-2, and NADDR-1
	 * have single, double, triple indirect blocks.
	 * the first step is to determine
	 * how many levels of indirection.
	 */
	sh = 0;
	nb = 1;
	bn -= NADDR-3;
	for(j=3; j>0; j--) {
		sh += NSHIFT;
		nb <<= NSHIFT;
		if(bn < nb)
			break;
		bn -= nb;
	}
	if(j == 0) {
		u.u_error = EFBIG;
		return((daddr_t)0);
	}

	/*
	 * fetch the first indirect block
	 */
	nb = ip->i_un.i_addr[NADDR-j];
	if(nb == 0) {
#ifdef UCB_QUOTAS
		if (rwflg == B_READ || (bp = qalloc(ip,dev)) == NULL)
#else
		if(rwflg==B_READ || (bp = alloc(dev))==NULL)
#endif
			return((daddr_t)-1);
#ifndef UCB_NKB
		nb = bp->b_blkno;
#else
		nb = dbtofsb(bp->b_blkno);
#endif
#ifdef UCB_FSFIX
		/*
		 * Write synchronously so that indirect blocks
		 * never point at garbage.
		 */
		bwrite(bp);
#else
		bdwrite(bp);
#endif
		ip->i_un.i_addr[NADDR-j] = nb;
		ip->i_flag |= IUPD|ICHG;
	}

	/*
	 * fetch through the indirect blocks
	 */
	for(; j<=3; j++) {
		bp = bread(dev, nb);
		if(bp->b_flags & B_ERROR) {
			brelse(bp);
			return((daddr_t)0);
		}
#ifdef	UCB_BUFOUT
		bap = (daddr_t *) mapin(bp);
#else
		bap = bp->b_un.b_daddr;
#endif
		sh -= NSHIFT;
		i = (bn>>sh) & NMASK;
		nb = bap[i];
		/*
		 * calculate read-ahead
		 */
		if(i < NINDIR-1)
			ra = bap[i+1];
#ifdef	UCB_BUFOUT
		mapout(bp);
#endif
		if(nb == 0) {
#ifdef UCB_QUOTAS
			if (rwflg == B_READ || (nbp = qalloc(ip,dev)) == NULL) {
#else
			if(rwflg==B_READ || (nbp = alloc(dev))==NULL) {
#endif
				brelse(bp);
				return((daddr_t)-1);
			}
#ifndef UCB_NKB
			nb = nbp->b_blkno;
#else
			nb = dbtofsb(nbp->b_blkno);
#endif
#ifdef UCB_FSFIX
			if (j < 3 || (ip->i_mode&IFMT) == IFDIR)
				/*
				 * Write synchronously so indirect blocks
				 * never point at garbage and blocks
				 * in directories never contain garbage.
				 */
				bwrite(nbp);
			else
#endif
				bdwrite(nbp);
#ifdef	UCB_BUFOUT
			bap = (daddr_t) mapin(bp);
#endif
			bap[i] = nb;
#ifdef	UCB_BUFOUT
			mapout(bp);
#endif
			bdwrite(bp);
		} else
			brelse(bp);
	}

	rablock = ra;
	return(nb);
}

#ifdef UCB_QUOTAS
/*
 * check the current directory name to be
 * a given string.  string is ".q" or ".."
 * used to keep track of quotas.
 */
isname(c1)
register char *c1;
{
	register char *c2;

	c2 = u.u_dent.d_name;
	while (*c1++ == *c2)
		if (*c2++ == 0)
			return(1);
	return(0);
}
#endif

/*
 * Pass back  c  to the user at his location u_base;
 * update u_base, u_count, and u_offset.  Return -1
 * on the last character of the user's read.
 * u_base is in the user address space unless u_segflg is set.
 */
passc(c)
register c;
{
	register id;

	if((id = u.u_segflg) == 1)
		*u.u_base = c;
	else
		if(id?suibyte(u.u_base, c):subyte(u.u_base, c) < 0) {
			u.u_error = EFAULT;
			return(-1);
		}
	u.u_count--;
	u.u_offset++;
	u.u_base++;
	return(u.u_count == 0? -1: 0);
}

/*
 * Pick up and return the next character from the user's
 * write call at location u_base;
 * update u_base, u_count, and u_offset.  Return -1
 * when u_count is exhausted.  u_base is in the user's
 * address space unless u_segflg is set.
 */
cpass()
{
	register c, id;

	if(u.u_count == 0)
		return(-1);
	if((id = u.u_segflg) == 1)
		c = *u.u_base;
	else
		if((c = id==0?fubyte(u.u_base):fuibyte(u.u_base)) < 0) {
			u.u_error = EFAULT;
			return(-1);
		}
	u.u_count--;
	u.u_offset++;
	u.u_base++;
	return(c&0377);
}

/*
 * Routine which sets a user error; placed in
 * illegal entries in the bdevsw and cdevsw tables.
 */
nodev()
{

	u.u_error = ENODEV;
}

/*
 * Null routine; placed in insignificant entries
 * in the bdevsw and cdevsw tables.
 */
nulldev()
{
}

/*
 * copy count bytes from from to to.
 */
bcopy(from, to, count)
caddr_t from, to;
register count;
{
	register char *f, *t;

	f = from;
	t = to;
	do
		*t++ = *f++;
	while(--count);
}
