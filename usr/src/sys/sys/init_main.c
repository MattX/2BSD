/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)init_main.c	1.1 (2.10BSD Berkeley) 12/1/86
 */

#include "param.h"
#include "../machine/seg.h"

#include "user.h"
#include "fs.h"
#include "mount.h"
#include "map.h"
#include "proc.h"
#include "inode.h"
#include "conf.h"
#include "buf.h"
#include "vm.h"
#include "clist.h"
#include "uba.h"
#include "reboot.h"
#include "systm.h"
#include "kernel.h"
#include "namei.h"
#ifdef QUOTA
#include "quota.h"
#endif

int	netoff = 1;
int	cmask = CMASK;

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
 */
main()
{
	register struct proc *p;
	register int i;

	startup();

	/*
	 * set up system process 0 (swapper)
	 */
	p = &proc[0];
	p->p_addr = *ka6;
	p->p_stat = SRUN;
	p->p_flag |= SLOAD|SSYS;
	p->p_nice = NZERO;

	u.u_procp = p;			/* init user structure */
	u.u_ap = u.u_arg;
	u.u_cmask = cmask;
	u.u_lastfile = -1;
	for (i = 1; i < NGROUPS; i++)
		u.u_groups[i] = NOGROUP;
	for (i = 0; i < sizeof(u.u_rlimit)/sizeof(u.u_rlimit[0]); i++)
		u.u_rlimit[i].rlim_cur = u.u_rlimit[i].rlim_max = 
		    RLIM_INFINITY;
	/*
	 * Initialize tables, protocols, and set up well-known inodes.
	 */
	cinit();
	pqinit();
	xinit();
	ihinit();
	bhinit();
	binit();
#ifdef UNIBUS_MAP
	ubinit();
#endif
#ifdef QUOTA
	QUOTAMAP();
	qtinit();
	u.u_quota = getquota(0, 0, Q_NDQ);
	px_quota[0] = u.u_quota;
	QUOTAUNMAP();
#endif
	clkstart();
	iinit();

/* kick off timeout driven events by calling first time */
	schedcpu();

/* set up the root file system */
	rootdir = iget(rootdev, &mount[0].m_filsys, (ino_t)ROOTINO);
	iunlock(rootdir);
	u.u_cdir = iget(rootdev, &mount[0].m_filsys, (ino_t)ROOTINO);
	iunlock(u.u_cdir);
	u.u_rdir = NULL;

#ifdef UCB_NET
	if (netoff = netinit())
		printf("Network init failed\n");
	else
		NETSTART();
#endif

	/*
	 * make init process
	 */
#ifdef UCB_FRCSWAP
	idleflg = 1;			/* init can't cause swap */
#endif
	if (newproc(0)) {
		expand((int)btoc(szicode), S_DATA);
		expand((int)1, S_STACK);	/* one click of stack */
		estabur((u_int)0, (u_int)btoc(szicode), (u_int)1, 0, RO);
		copyout((caddr_t)icode, (caddr_t)0, szicode);
		/*
		 * return goes to location 0 of user init code
		 * just copied out.
		 */
		return;
	}
	else
		sched();
}

/*
 * Initialize hash links for buffers.
 */
static
bhinit()
{
	register int i;
	register struct bufhd *bp;

	for (bp = bufhash, i = 0; i < BUFHSZ; i++, bp++)
		bp->b_forw = bp->b_back = (struct buf *)bp;
}

memaddr	bpaddr;		/* physical click-address of buffers */
/*
 * Initialize the buffer I/O system by freeing
 * all buffers and setting all device buffer lists to empty.
 */
static
binit()
{
	register struct buf *bp;
	register int i;
	long paddr;

	for (bp = bfreelist; bp < &bfreelist[BQUEUES]; bp++)
		bp->b_forw = bp->b_back = bp->av_forw = bp->av_back = bp;
	paddr = ((long)bpaddr) << 6;
	for (i = 0; i < NBUF; i++, paddr += MAXBSIZE) {
		bp = &buf[i];
		bp->b_dev = NODEV;
		bp->b_bcount = 0;
		bp->b_un.b_addr = (caddr_t)loint(paddr);
		bp->b_xmem = hiint(paddr);
		binshash(bp, &bfreelist[BQ_AGE]);
		bp->b_flags = B_BUSY|B_INVAL;
		brelse(bp);
	}
}

/*
 * Initialize clist by freeing all character blocks, then count
 * number of character devices. (Once-only routine)
 */
static
cinit()
{
	register int ccp;
	register struct cblock *cp;

	ccp = (int)cfree;
#ifdef UCB_CLIST
	mapseg5(clststrt, clstdesc);	/* don't save, we know it's normal */
#else
	ccp = (ccp + CROUND) & ~CROUND;
#endif
	for (cp = (struct cblock *)ccp; cp <= &cfree[nclist - 1]; cp++) {
		cp->c_next = cfreelist;
		cfreelist = cp;
		cfreecount += CBSIZE;
	}
#ifdef UCB_CLIST
	normalseg5();
#endif
}

/*
 * Iinit is called once (from main) very early in initialization.
 * It reads the root's super block and initializes the current date
 * from the last modified date.
 *
 * panic: iinit -- cannot read the super block
 * (usually because of an IO error).
 */
static
iinit()
{
	register struct bdevsw *bdp;
	register struct buf *bp;
	register struct fs *fp;

	for (bdp = bdevsw; bdp < bdevsw + nblkdev; bdp++)
		(void)(*bdp->d_root)();
	(*bdevsw[major(rootdev)].d_open)(rootdev, B_READ);
	(*bdevsw[major(swapdev)].d_open)(swapdev, B_READ);
	bp = bread(rootdev, SUPERB);
	if (u.u_error)
		panic("iinit");
	fp = &mount[0].m_filsys;
	bcopy(mapin(bp), (caddr_t)fp, sizeof(struct fs));
	mapout(bp);
	mount[0].m_inodp = (struct inode *)1;
	brelse(bp);
	mount[0].m_dev = rootdev;
	fp->fs_flock = fp->fs_ilock = fp->fs_ronly = 0;
	fp->fs_lasti = 1;
	fp->fs_nbehind = 0;
	fp->fs_fsmnt[0] = '/';
	fp->fs_fsmnt[1] = '\0';
	fp->fs_ronly = boothowto&RB_RDONLY ? 1 : 0;
	time.tv_sec = fp->fs_time;
	boottime = time;
}

#ifdef UCB_NET
memaddr netdata;		/* click address of start of net data */

/*
 * We are called here after all the other init routines (clist, inode,
 * unibusmap, etc...) have been called.  'init' is probably running, but
 * other than that we can allocate memory without fragmenting.  Open the
 * file NETNIX and read the a.out header, based on that go allocate
 * memory and read the text+data into the memory.  Set up supervisor page
 * registers, SDSA6 and SDSA7 have already been set up in mch_start.s.
 */
#define	NETNIX	"/netnix"
static
netinit()
{
	register u_short *ap, *dp;
	register int i;
	struct exec ex;
	struct inode *ip;
	memaddr nettext;
	long lsize;
	int initdata, netdsize, nettsize, ret, resid;
	char oneclick[ctob(1)];

	ret = 1;
	u.u_segflg = UIO_SYSSPACE;
	u.u_dirp = NETNIX;
	if (!(ip = namei(LOOKUP | FOLLOW))) {
		printf("%s: not found.\n", NETNIX);
		goto leave;
	}
	if ((ip->i_mode & IFMT) != IFREG || !ip->i_size) {
		printf("%s: bad ip format.\n", NETNIX);
		goto leave;
	}
	u.u_base = (caddr_t)&ex;
	u.u_count = sizeof(ex);
	u.u_offset = 0;
	readi(ip);
	if (u.u_error || u.u_count) {
		printf("%s: u_error {%d} u_count {%d}.\n", NETNIX, u.u_error,
		    u.u_count);
		goto leave;
	}
	if (ex.a_magic != A_MAGIC3) {
		printf("%s: bad magic, %o.\n", NETNIX, ex.a_magic);
		goto leave;
	}
	lsize = (long)ex.a_data + (long)ex.a_bss;
	if (lsize > 48L * 1024L) {
		printf("%s: too big, %ld\n", NETNIX, lsize);
		goto leave;
	}
	nettsize = btoc(ex.a_text);
	nettext = (memaddr)malloc(coremap, nettsize);
	netdsize = btoc(ex.a_data + ex.a_bss);
	netdata = (memaddr)malloc(coremap, netdsize);
	initdata = ex.a_data >> 6;
	resid = ex.a_data & 077;
	for (i = 0; i < nettsize; i++) {
		u.u_count = ctob(1);
		u.u_base = oneclick;
		readi(ip);
		if (u.u_error || u.u_count)
			goto release;
		mapseg5(nettext + i, 077406);
		bcopy(oneclick, SEG5, ctob(1));
		normalseg5();
	}
	for (i = 0; i < initdata; i++) {
		u.u_count = ctob(1);
		u.u_base = oneclick;
		readi(ip);
		if (u.u_error || u.u_count)
			goto release;
		mapseg5(netdata + i, 077406);
		bcopy(oneclick, SEG5, ctob(1));
		normalseg5();
	}
	if (resid) {
		u.u_count = resid;
		u.u_base = oneclick;
		readi(ip);
		if (u.u_error || u.u_count) {
release:		printf("%s: u_error {%d} u_count {%d}.\n",
			    NETNIX, u.u_error, u.u_count);
			mfree(coremap, nettsize, nettext);
			mfree(coremap, netdsize, netdata);
			nettsize = netdsize = 0;
			netdata = nettext = 0;
			goto leave;
		}
		mapseg5(netdata + i, 077406);	/* i is set from above loop */
		bcopy(oneclick, SEG5, resid);
		normalseg5();
	}
	for (i = 0, ap = SISA0, dp = SISD0; i < nettsize; i += stoc(1)) {
		*ap++ = nettext + i;
		*dp++ = ((stoc(1) - 1) << 8) | RO;
	}
	/* might have over run the length on the last one, patch it now */
	if (i > nettsize)
		*--dp -= ((i - nettsize) << 8);
	for (i = 0, ap = SDSA0, dp = SDSD0; i < netdsize; i += stoc(1)) {
		*ap++ = netdata + i;
		*dp++ = ((stoc(1) - 1) << 8) | RW;
	}
	if (i > netdsize)
		*--dp -= ((i - netdsize) << 8);
#ifdef DIAGNOSTIC
	printf("%s: size (clicks): text %d data %d bss %d\nloaded at 0%o 0%o clicks\n",
	    NETNIX, nettsize, initdata, btoc(ex.a_bss), nettext, netdata);
#endif
	ret = 0;
leave:	if (ip)
		iput(ip);
	u.u_error = 0;
	u.u_offset = 0;
	u.u_count = 0;
	u.u_dirp = 0;
	u.u_base = 0;
	u.u_segflg = 0;
	u.ni_endoff = 0;
	bzero(&u.u_ncache, sizeof(u.u_ncache));
	bzero(&u.u_dent, sizeof(u.u_dent));
	if (u.u_pdir) {
		iput(u.u_pdir);
		u.u_pdir = 0;
	}
	return(ret);
}
#endif /* UCB_NET */
