#include <sys/param.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/seg.h>
#ifdef UCB_METER
#include <sys/vm.h>
#endif
#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)bio.c	3.2";
#endif



#ifdef	DISKMON
struct {
	int	nbuf;
	long	nread;
	long	nreada;
	long	ncache;
	long	nwrite;
	long	bufcount[NBUF];
} io_info;
#endif

/*
 * swap IO headers.
 * they are filled in to point
 * at the desired IO operation.
 */
struct	buf	swbuf1;
struct	buf	swbuf2;

/*
 * The following several routines allocate and free
 * buffers with various side effects.  In general the
 * arguments to an allocate routine are a device and
 * a block number, and the value is a pointer to
 * to the buffer header; the buffer is marked "busy"
 * so that no one else can touch it.  If the block was
 * already in core, no I/O need be done; if it is
 * already busy, the process waits until it becomes free.
 * The following routines allocate a buffer:
 *	getblk
 *	bread
 *	breada
 * Eventually the buffer must be released, possibly with the
 * side effect of writing it out, by using one of
 *	bwrite
 *	bdwrite
 *	bawrite
 *	brelse
 */

#ifdef UCB_BHASH
#define	BUFHSZ	64	/* must be power of 2 */
#define	BUFHASH(blkno)	(blkno & (BUFHSZ-1))

struct	buf	*bhash[BUFHSZ] = { 0 };

/*
 * initialize hash links for buffers
 */
bhinit()
{
	register int i;

	for (i = 0; i < BUFHSZ; i++)
		bhash[i] = (struct buf *) NULL;
}
#endif

/*
 * Read in (if necessary) the block and return a buffer pointer.
 */
struct buf *
bread(dev, blkno)
dev_t dev;
daddr_t blkno;
{
	register struct buf *bp;

	bp = getblk(dev, blkno);
	if (bp->b_flags&B_DONE) {
#ifdef	DISKMON
		io_info.ncache++;
#endif
		return(bp);
	}
	bp->b_flags |= B_READ;
	bp->b_bcount = BSIZE;
	(*bdevsw[major(dev)].d_strategy)(bp);
#ifdef	DISKMON
	io_info.nread++;
#endif
	iowait(bp);
	return(bp);
}

/*
 * Read in the block, like bread, but also start I/O on the
 * read-ahead block (which is not allocated to the caller)
 */
struct buf *
breada(dev, blkno, rablkno)
dev_t dev;
daddr_t blkno, rablkno;
{
	register struct buf *bp, *rabp;

	bp = NULL;
	if (!incore(dev, blkno)) {
		bp = getblk(dev, blkno);
		if ((bp->b_flags&B_DONE) == 0) {
			bp->b_flags |= B_READ;
			bp->b_bcount = BSIZE;
			(*bdevsw[major(dev)].d_strategy)(bp);
#ifdef	DISKMON
			io_info.nread++;
#endif
		}
	}
	if (rablkno && !incore(dev, rablkno)) {
		rabp = getblk(dev, rablkno);
		if (rabp->b_flags & B_DONE)
			brelse(rabp);
		else {
			rabp->b_flags |= B_READ|B_ASYNC;
			rabp->b_bcount = BSIZE;
			(*bdevsw[major(dev)].d_strategy)(rabp);
#ifdef	DISKMON
			io_info.nreada++;
#endif
		}
	}
	if(bp == NULL)
		return(bread(dev, blkno));
	iowait(bp);
	return(bp);
}

/*
 * Write the buffer, waiting for completion.
 * Then release the buffer.
 */
bwrite(bp)
register struct buf *bp;
{
	register flag;

	flag = bp->b_flags;
	bp->b_flags &= ~(B_READ | B_DONE | B_ERROR | B_DELWRI | B_AGE);
	bp->b_bcount = BSIZE;
#ifdef	DISKMON
	io_info.nwrite++;
#endif
	(*bdevsw[major(bp->b_dev)].d_strategy)(bp);
	if ((flag&B_ASYNC) == 0) {
		iowait(bp);
		brelse(bp);
	} else if (flag & B_DELWRI)
		bp->b_flags |= B_AGE;
	else
		geterror(bp);
}

/*
 * Release the buffer, marking it so that if it is grabbed
 * for another purpose it will be written out before being
 * given up (e.g. when writing a partial block where it is
 * assumed that another write for the same block will soon follow).
 * This can't be done for magtape, since writes must be done
 * in the same order as requested.
 */
bdwrite(bp)
register struct buf *bp;
{
	register struct buf *dp;

	dp = bdevsw[major(bp->b_dev)].d_tab;
	if(dp->b_flags & B_TAPE)
		bawrite(bp);
	else {
		bp->b_flags |= B_DELWRI | B_DONE;
		brelse(bp);
	}
}

/*
 * Release the buffer, start I/O on it, but don't wait for completion.
 */
bawrite(bp)
register struct buf *bp;
{

	bp->b_flags |= B_ASYNC;
	bwrite(bp);
}

/*
 * release the buffer, with no I/O implied.
 */
brelse(bp)
register struct buf *bp;
{
	register struct buf **backp;
	register s;

	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	if (bfreelist.b_flags&B_WANTED) {
		bfreelist.b_flags &= ~B_WANTED;
		wakeup((caddr_t)&bfreelist);
	}
	if (bp->b_flags&B_ERROR) {
#ifdef UCB_BHASH
		bunhash(bp);
#endif
		bp->b_dev = NODEV;   /* no assoc. on error */
	}
	s = spl6();
	if(bp->b_flags & B_AGE) {
		backp = &bfreelist.av_forw;
		(*backp)->av_back = bp;
		bp->av_forw = *backp;
		*backp = bp;
		bp->av_back = &bfreelist;
	} else {
		backp = &bfreelist.av_back;
		(*backp)->av_forw = bp;
		bp->av_back = *backp;
		*backp = bp;
		bp->av_forw = &bfreelist;
	}
	bp->b_flags &= ~(B_WANTED|B_BUSY|B_ASYNC|B_AGE);
	splx(s);
}

/*
 * release an in-space buffer
 */

#ifdef	UCB_BUFOUT
abrelse(bp)
register struct buf *bp;
{
	register struct buf **backp;
	register s;

	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	if (abfreelist.b_flags&B_WANTED) {
		abfreelist.b_flags &= ~B_WANTED;
		wakeup((caddr_t)&abfreelist);
	}
	s = spl6();
	backp = &abfreelist.av_forw;
	(*backp)->av_back = bp;
	bp->av_forw = *backp;
	*backp = bp;
	bp->av_back = &abfreelist;
	bp->b_flags &= ~(B_WANTED|B_BUSY|B_ASYNC|B_AGE);
	splx(s);
}
#endif

/*
 * See if the block is associated with some buffer
 * (mainly to avoid getting hung up on a wait in breada)
 */
incore(dev, blkno)
dev_t dev;
daddr_t blkno;
{
	register struct buf *bp;
	register struct buf *dp;
#ifdef UCB_NKB
	register daddr_t dblkno = fsbtodb(blkno);
#endif

#ifdef UCB_BHASH
	for(bp = bhash[BUFHASH(blkno)]; bp != NULL; bp = bp->b_link)
#else
	dp = bdevsw[major(dev)].d_tab;
	for (bp=dp->b_forw; bp != dp; bp = bp->b_forw)
#endif
#ifndef UCB_NKB
		if (bp->b_blkno==blkno && bp->b_dev==dev)
#else
		if (bp->b_blkno==dblkno && bp->b_dev==dev)
#endif
			return(1);
	return(0);
}

/*
 * Assign a buffer for the given block.  If the appropriate
 * block is already associated, return it; otherwise search
 * for the oldest non-busy buffer and reassign it.
 */
struct buf *
getblk(dev, blkno)
dev_t dev;
daddr_t blkno;
{
	register struct buf *bp;
	register struct buf *dp;
#ifdef UCB_NKB
	register daddr_t dblkno = fsbtodb(blkno);
#endif
#ifdef	DISKMON
	register i;
#endif
#ifdef UCB_BHASH
	register int j;
	register struct buf *ep;
#endif
	if(major(dev) >= nblkdev)
		panic("blkdev");

    loop:
	spl0();
	dp = bdevsw[major(dev)].d_tab;
	if(dp == NULL)
		panic("devtab");
#ifdef UCB_BHASH
	for(bp = bhash[BUFHASH(blkno)]; bp != NULL; bp = bp->b_link)
#else
	for (bp=dp->b_forw; bp != dp; bp = bp->b_forw)
#endif
	{
#ifndef UCB_NKB
		if (bp->b_blkno!=blkno || bp->b_dev!=dev)
#else
		if (bp->b_blkno!=dblkno || bp->b_dev!=dev)
#endif
			continue;
		spl6();
		if (bp->b_flags&B_BUSY) {
			bp->b_flags |= B_WANTED;
			sleep((caddr_t)bp, PRIBIO+1);
			goto loop;
		}
		spl0();
#ifdef	DISKMON
		i = 0;
		dp = bp->av_forw;
		while (dp != &bfreelist) {
			i++;
			dp = dp->av_forw;
		}
		if (i<NBUF)
			io_info.bufcount[i]++;
#endif
		notavail(bp);
#ifdef UCB_BHASH
		bp->b_flags |= B_CACHE;
#endif
		return(bp);
	}
	spl6();
	if (bfreelist.av_forw == &bfreelist) {
		bfreelist.b_flags |= B_WANTED;
		sleep((caddr_t)&bfreelist, PRIBIO+1);
		goto loop;
	}
	spl0();
	notavail(bp = bfreelist.av_forw);
	if (bp->b_flags & B_DELWRI) {
		bp->b_flags |= B_ASYNC;
		bwrite(bp);
		goto loop;
	}
#ifdef UCB_BHASH
	bunhash(bp);
#endif
	bp->b_flags = B_BUSY;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	bp->b_forw = dp->b_forw;
	bp->b_back = dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = dev;
#ifndef UCB_NKB
	bp->b_blkno = blkno;
#else
	bp->b_blkno = dblkno;
#endif
#ifdef UCB_BHASH
	j = BUFHASH(blkno);
#ifdef	DEBUG
	printf("hashing bp=%o bhash[j=%d]=%o old=%o\n", bp, j, bhash[j], bp->b_link);
#endif
	bp->b_link = bhash[j];
	bhash[j] = bp;
#endif
	return(bp);
}

/*
 * get an empty block,
 * not assigned to any particular device
 */

struct buf *
#ifdef	UCB_BUFOUT
getfblk(flist)
register struct buf *flist;
#else
geteblk()
#define	flist	(&bfreelist)
#endif
{
	register struct buf *bp;
	register struct buf *dp;

loop:
	spl6();
	while (flist->av_forw == flist) {
		flist->b_flags |= B_WANTED;
		sleep((caddr_t)flist, PRIBIO+1);
	}
	spl0();
	dp = flist;
	notavail(bp = flist->av_forw);
	if (bp->b_flags & B_DELWRI) {
		bp->b_flags |= B_ASYNC;
		bwrite(bp);
		goto loop;
	}
#ifdef UCB_BHASH
	bunhash(bp);
#endif
	bp->b_flags = B_BUSY;
	bp->b_back->b_forw = bp->b_forw;
	bp->b_forw->b_back = bp->b_back;
	bp->b_forw = dp->b_forw;
	bp->b_back = dp;
	dp->b_forw->b_back = bp;
	dp->b_forw = bp;
	bp->b_dev = (dev_t)NODEV;
#ifdef UCB_BHASH
	bp->b_link = NULL;
#endif
	return(bp);
}

#ifdef	UCB_BUFOUT
struct buf *
geteblk() {
	return(getfblk(&abfreelist));
}

struct buf *
getiblk() {
	return(getfblk(&bfreelist));
}
#endif

#ifdef UCB_BHASH
bunhash(bp)
register struct buf *bp;
{
	register struct buf *ep;
	register int i;

#ifdef	DEBUG
	printf("unhash\n");
#endif
	if (bp->b_dev == NODEV)
		return;
#ifdef UCB_NKB
	i = BUFHASH(dbtofsb(bp->b_blkno));
#else
	i = BUFHASH(bp->b_blkno);
#endif
	ep = bhash[i];
	if(ep == NULL)
		panic("bunhash 1");
#ifdef	DEBUG
	printf("blkno=%D bhash[i=%d]=%o\n", bp->b_blkno, i, ep);
#endif
	if (ep == bp) {
		bhash[i] = bp->b_link;
		return;
	}
	for(; ep != NULL; ep = ep->b_link)
		if (ep->b_link == bp) {
			ep->b_link = bp->b_link;
			return;
		}
	panic("bunhash 2");
}
#endif

/*
 * Wait for I/O completion on the buffer; return errors
 * to the user.
 */
iowait(bp)
register struct buf *bp;
{

	spl6();
	while ((bp->b_flags&B_DONE)==0)
		sleep((caddr_t)bp, PRIBIO);
	spl0();
	geterror(bp);
}

/*
 * Unlink a buffer from the available list and mark it busy.
 * (internal interface)
 */
notavail(bp)
register struct buf *bp;
{
	register s;

	s = spl6();
	bp->av_back->av_forw = bp->av_forw;
	bp->av_forw->av_back = bp->av_back;
	bp->b_flags |= B_BUSY;
	splx(s);
}

/*
 * Mark I/O complete on a buffer, release it if I/O is asynchronous,
 * and wake up anyone waiting for it.
 */
iodone(bp)
register struct buf *bp;
{

#ifdef UNIBUS_MAP
	if(bp->b_flags&B_MAP)
		mapfree(bp);
#endif
	bp->b_flags |= B_DONE;
	if (bp->b_flags&B_ASYNC)
		brelse(bp);
	else {
		bp->b_flags &= ~B_WANTED;
		wakeup((caddr_t)bp);
	}
}

/*
 * Zero the core associated with a buffer.
 */
clrbuf(bp)
struct buf *bp;
{
	register *p;
	register c;

#ifdef	UCB_BUFOUT
	p = (int *) mapin(bp);
#else
	p = bp->b_un.b_words;
#endif
	c = (BSIZE/sizeof(int)) >> 2;
	do {
		*p++ = 0;
		*p++ = 0;
		*p++ = 0;
		*p++ = 0;
	} while (--c);
	bp->b_resid = 0;
#ifdef	UCB_BUFOUT
	mapout(bp);
#endif
}

/*
 * swap I/O
 */
swap(blkno, coreaddr, count, rdflg)
register count;
{
	register struct buf *bp;
	register tcount;

	bp = &swbuf1;
#ifdef UCB_METER
	if (rdflg) {
		cnt.v_pswpin += count;
		cnt.v_swpin++;
	} else {
		cnt.v_pswpout += count;
		cnt.v_swpout++;
	}
#endif
	if(bp->b_flags & B_BUSY)
		if((swbuf2.b_flags&B_WANTED) == 0)
			bp = &swbuf2;
	spl6();
	while (bp->b_flags&B_BUSY) {
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PSWP+1);
	}
	while (count) {
		bp->b_flags = B_BUSY | B_PHYS | rdflg;
		bp->b_dev = swapdev;
		tcount = count;
		if (tcount >= 01700)	/* prevent byte-count wrap */
			tcount = 01700;
		bp->b_bcount = ctob(tcount);
		bp->b_blkno = swplo+blkno;
		bp->b_un.b_addr = (caddr_t)(coreaddr<<6);
		bp->b_xmem = (coreaddr>>10) & 077;
		(*bdevsw[major(swapdev)].d_strategy)(bp);
		spl6();
		while((bp->b_flags&B_DONE)==0)
			sleep((caddr_t)bp, PSWP);
		count -= tcount;
		coreaddr += tcount;
		blkno += ctod(tcount);
	}
	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	spl0();
	bp->b_flags &= ~(B_BUSY|B_WANTED);
	if (bp->b_flags & B_ERROR)
		panic("IO err in swap");
}

/*
 * make sure all write-behind blocks
 * on dev (or NODEV for all)
 * are flushed out.
 * (from umount and update)
 */
bflush(dev)
dev_t dev;
{
	register struct buf *bp;

loop:
	spl6();
	for (bp = bfreelist.av_forw; bp != &bfreelist; bp = bp->av_forw) {
		if (bp->b_flags&B_DELWRI && (dev == NODEV||dev==bp->b_dev)) {
			bp->b_flags |= B_ASYNC;
			notavail(bp);
			bwrite(bp);
			goto loop;
		}
	}
	spl0();
}

/*
 * Raw I/O. The arguments are
 *	The strategy routine for the device
 *	A buffer, which will always be a special buffer
 *	  header owned exclusively by the device for this purpose
 *	The device number
 *	Read/write flag
 * Essentially all the work is computing physical addresses and
 * validating them.
 */
physio(strat, bp, dev, rw)
register struct buf *bp;
int (*strat)();
{
	register unsigned base;
	register int nb;
	int ts;

	base = (unsigned)u.u_base;
	/*
	 * Check odd base, odd count, and address wraparound
	 */
	if (base&01 || u.u_count&01 || base>=base+u.u_count)
		goto bad;
	ts = (u.u_tsize+127) & ~0177;
	if (u.u_sep)
		ts = 0;
	nb = (base>>6) & 01777;
	/*
	 * Check overlap with text. (ts and nb now
	 * in 64-byte clicks)
	 */
	if (nb < ts)
		goto bad;
	/*
	 * Check that transfer is either entirely in the
	 * data or in the stack: that is, either
	 * the end is in the data or the start is in the stack
	 * (remember wraparound was already checked).
	 */
	if ((((base+u.u_count)>>6)&01777) >= ts+u.u_dsize
	    && nb < 1024-u.u_ssize)
		goto bad;
	spl6();
	while (bp->b_flags&B_BUSY) {
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO+1);
	}
	bp->b_flags = B_BUSY | B_PHYS | rw;
	bp->b_dev = dev;
	/*
	 * Compute physical address by simulating
	 * the segmentation hardware.
	 */
	ts = (u.u_sep? UDSA: UISA)->r[nb>>7] + (nb&0177);
	bp->b_un.b_addr = (caddr_t)((ts<<6) + (base&077));
	bp->b_xmem = (ts>>10) & 077;
#ifdef UCB_NKB
	bp->b_blkno = u.u_offset >> PGSHIFT;
#else
	bp->b_blkno = u.u_offset >> BSHIFT;
#endif
	bp->b_bcount = u.u_count;
	bp->b_error = 0;
	u.u_procp->p_flag |= SLOCK;
	(*strat)(bp);
	spl6();
	while ((bp->b_flags&B_DONE) == 0)
		sleep((caddr_t)bp, PRIBIO);
	u.u_procp->p_flag &= ~SLOCK;
	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	spl0();
	bp->b_flags &= ~(B_BUSY|B_WANTED);
	u.u_count = bp->b_resid;
	geterror(bp);
	return;
    bad:
	u.u_error = EFAULT;
}

/*
 * Pick up the device's error number and pass it to the user;
 * if there is an error but the number is 0 set a generalized
 * code.  Actually the latter is always true because devices
 * don't yet return specific errors.
 */
geterror(bp)
register struct buf *bp;
{

	if (bp->b_flags&B_ERROR)
		if ((u.u_error = bp->b_error)==0)
			u.u_error = EIO;
}
