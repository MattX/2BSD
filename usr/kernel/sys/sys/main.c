#include <sys/param.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/filsys.h>
#include <sys/mount.h>
#include <sys/map.h>
#include <sys/proc.h>
#include <sys/inode.h>
#include <sys/seg.h>
#include <sys/conf.h>
#include <sys/buf.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)main.c	3.1";
#endif


#ifdef UCB_FRCSWAP
int	swpdebug	0;
int	idleflg		0;	/* If set, allow incore forks and expands */
				/* Set before idle(), cleared in clock.c */
#endif
/*
 * Initialization code.
 * Called from cold start routine as
 * soon as a stack and segmentation
 * have been established.
 * Functions:
 *	clear and free user core
 *	turn on clock
 *	hand craft 0th process
 *	call all initialization routines
 *	fork - process 0 to schedule
 *	     - process 1 execute bootstrap
 *
 * loop at low address in user mode -- /etc/init
 *	cannot be executed.
 */
main()
{

	startup();
	/*
	 * set up system process
	 */

	proc[0].p_addr = ka6->r[0];
	proc[0].p_size = USIZE;
	proc[0].p_stat = SRUN;
	proc[0].p_flag |= SLOAD|SSYS;
	proc[0].p_nice = NZERO;
	u.u_procp = &proc[0];
	u.u_cmask = CMASK;

	/*
	 * Initialize devices and
	 * set up 'known' i-nodes
	 */

	clkstart();
#ifdef UCB_IHASH
	ihinit();
#endif
#ifdef UCB_BHASH
	bhinit();
#endif
	cinit();
	binit();
	iinit();
	rootdir = iget(rootdev, (ino_t)ROOTINO);
	rootdir->i_flag &= ~ILOCK;
	u.u_cdir = iget(rootdev, (ino_t)ROOTINO);
	u.u_cdir->i_flag &= ~ILOCK;
	u.u_rdir = NULL;

	/*
	 * make init process
	 * enter scheduling loop
	 * with system process
	 */
#ifdef	UCB_FRCSWAP	/* The init creation must not cause swap */
	idleflg++;
#endif
	if(newproc()) {
		expand(USIZE + (int)btoc(szicode));
		estabur((unsigned)0, btoc(szicode), (unsigned)0, 0, RO);
		copyout((caddr_t)icode, (caddr_t)0, szicode);
		/*
		 * Return goes to loc. 0 of user init
		 * code just copied out.
		 */
		return;
	}
	sched();
}

/*
 * iinit is called once (from main)
 * very early in initialization.
 * It reads the root's super block
 * and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super
 * block. Usually because of an IO error.
 */
iinit()
{
	register struct buf *cp, *bp;
	register struct filsys *fp;

	(*bdevsw[major(rootdev)].d_open)(rootdev, 1);
	bp = bread(rootdev, SUPERB);
	cp = geteblk();
	if(u.u_error)
		panic("iinit");
#ifdef	UCB_BUFOUT
	bcopy(mapin(bp), cp->b_un.b_addr, sizeof(struct filsys));
	mapout(bp);
#else
	bcopy(bp->b_un.b_addr, cp->b_un.b_addr, sizeof(struct filsys));
#endif
	brelse(bp);
	mount[0].m_bufp = cp;
	mount[0].m_dev = rootdev;
#ifdef UCB_MOUNT
	mount1(&mount[0]);
	fp = mount[0].m_caddr;
#else
	fp = cp->b_un.b_filsys;
#endif
	fp->s_flock = 0;
	fp->s_ilock = 0;
	fp->s_ronly = 0;
#ifdef	UCB_IHASH
	fp->s_lasti = 1;
	fp->s_nbehind = 0;
#endif
	time = fp->s_time;
#ifdef UCB_LOAD
	bootime = time;
#endif
}

#ifdef	UCB_BUFOUT
extern	char	abuffers[NABUF][BSIZE];
#else

/*
 * This is the set of buffers proper, whose heads
 * were declared in buf.h.  There can exist buffer
 * headers not pointing here that are used purely
 * as arguments to the I/O routines to describe
 * I/O to be done-- e.g. swbuf for
 * swapping.
 */

char	buffers[NBUF][BSIZE+BSLOP];

#endif

#ifdef	UCB_BUFOUT

/*
 * Constants used by machine-language initialization
 * to allocate the I/O buffer pool
 */

int	nbufs	= NBUF;			/* number of buffers */
int	bsize	= BSIZE + BSLOP;	/* size of each buffer */
int	bpaddr	= 0;			/* physical click-address of buffers,
					 * filled in by initilization */
#endif

/*
 * Initialize the buffer I/O system by freeing
 * all buffers and setting all device buffer lists to empty.
 */
binit()
{
	register struct buf *bp;
	register struct buf *dp;
	register int i;
	struct bdevsw *bdp;
#ifdef	UCB_BUFOUT
	long paddr;
#endif

	bfreelist.b_forw = bfreelist.b_back =
	    bfreelist.av_forw = bfreelist.av_back = &bfreelist;
#ifdef	UCB_BUFOUT
	paddr = ((long) bpaddr) << 6;
#endif
	for (i=0; i<NBUF; i++) {
		bp = &buf[i];
		bp->b_dev = NODEV;
#ifdef	UCB_BUFOUT
		bp->b_un.b_addr = loint(paddr);
		bp->b_xmem = hiint(paddr);
		paddr += bsize;
#else
		bp->b_un.b_addr = buffers[i];
#endif
		bp->b_back = &bfreelist;
		bp->b_forw = bfreelist.b_forw;
		bfreelist.b_forw->b_back = bp;
		bfreelist.b_forw = bp;
		bp->b_flags = B_BUSY;
		brelse(bp);
	}
#ifdef	UCB_BUFOUT
	abfreelist.b_forw = abfreelist.b_back =
	    abfreelist.av_forw = abfreelist.av_back = &abfreelist;
	for (i=0; i<NABUF; i++) {
		bp = &abuf[i];
		bp->b_dev = NODEV;
		bp->b_un.b_addr = abuffers[i];
		bp->b_back = &abfreelist;
		bp->b_forw = abfreelist.b_forw;
		abfreelist.b_forw->b_back = bp;
		abfreelist.b_forw = bp;
		bp->b_flags = B_BUSY;
		abrelse(bp);
	}
#endif
	for (bdp = bdevsw; bdp->d_open; bdp++) {
		dp = bdp->d_tab;
		if(dp) {
			dp->b_forw = dp;
			dp->b_back = dp;
		}
		nblkdev++;
	}
}
