/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)init_main.c	1.7 (2.11BSD GTE) 1/6/95
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
int	securelevel;

extern	size_t physmem;
extern	struct	mapent _coremap[];

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
	extern dev_t bootdev;
	extern caddr_t bootcsr;
	register struct proc *p;
	register int i;
	register struct fs *fs;
	time_t  toytime, toyclk();
	daddr_t swsize;

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
	u.u_nd.ni_iov = &u.u_nd.ni_iovec;
	u.u_nd.ni_iovcnt = 1;
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
	ubinit();
#ifdef QUOTA
	QUOTAMAP();
	qtinit();
	u.u_quota = getquota(0, 0, Q_NDQ);
	QUOTAUNMAP();
#endif
	nchinit();
	clkstart();

#ifdef	GENERIC
/*
 * If this is the GENERIC kernel we set 'rootdev' to be the same as
 * the device booted from.  'swapdev' is set to the the 'b' partition
 * of 'bootdev'.  Set 'pipedev' to be 'rootdev'.  The 077 in the first
 * statement removes the controller number (bits 6 and 7) - those bits
 * are passed thru from /boot but would only greatly confuse the rest
 * of the kernel.
*/
	rootdev = makedev(major(bootdev), minor(bootdev) & 077);
	swapdev = rootdev | 1;	/* partition 'b' */
	pipedev = rootdev;
	dumpdev = NODEV;	/* paranoia */
#endif

/*
 * Need to attach the root device.  The CSR is passed thru because this
 * may be a 2nd or 3rd controller rather than the 1st.  NOTE: This poses
 * a big problem if 'swapdev' is not on the same controller as 'rootdev'
 * _or_ if 'swapdev' itself is on a 2nd or 3rd controller.  Short of moving
 * autconfigure back in to the kernel it is not known what can be done about
 * this.
 *
 * One solution (for now) is to call swapdev's attach routine with a zero
 * address.  The MSCP driver treats the 0 as a signal to perform the
 * old (fixed address) attach.  Drivers (all the rest at this point) which
 * do not support alternate controller booting always attach the first
 * (primary) CSR and do not expect an argument to be passed.
*/
	(void)(*bdevsw[major(bootdev)].d_root)(bootcsr);
	(void)(*bdevsw[major(swapdev)].d_root)((caddr_t) 0);	/* XXX */

/*
 * Now we find out how much swap space is available.  Since 'nswap' is
 * a "u_int" we have to restrict the amount of swap to 65535 sectors (~32mb).
 * Considering that 4mb is the maximum physical memory capacity of a pdp-11
 * 32mb swap should be enough ;-)
 *
 * The initialization of the swap map was moved here from machdep2.c because
 * 'nswap' was no longer statically defined and this is where the swap dev
 * is opened/initialized.
 *
 * Also, we toss away/ignore .5kb (1 sector) of swap space (because a 0 value
 * can not be placed in a resource map).
 *
 * 'swplo' was a hack which has _finally_ gone away!  It was never anything
 * but 0 and caused a number of double word adds in the kernel.
*/
	(*bdevsw[major(swapdev)].d_open)(swapdev, B_READ|B_WRITE);
	swsize = (*bdevsw[major(swapdev)].d_psize)(swapdev);
	if	(swsize < 0)
		panic("swsize");	/* don't want to panic, but what ? */
	if	(swsize > (daddr_t)65535)
		swsize = 65535;
	nswap = swsize;
	mfree(swapmap, --nswap, 1);

	fs = mountfs(rootdev, boothowto & RB_RDONLY ? MNT_RDONLY : 0,
			(struct inode *)0);
	if (!fs)
		panic("iinit");
	mount[0].m_inodp = (struct inode *)1;	/* XXX */
	fs->fs_fsmnt[0] = '/';
	fs->fs_fsmnt[1] = '\0';
	time.tv_sec = fs->fs_time;
	if	(toytime = toyclk())
		time.tv_sec = toytime;
	boottime = time;

/* kick off timeout driven events by calling first time */
	schedcpu();

/* set up the root file system */
	rootdir = iget(rootdev, &mount[0].m_filsys, (ino_t)ROOTINO);
	iunlock(rootdir);
	u.u_cdir = iget(rootdev, &mount[0].m_filsys, (ino_t)ROOTINO);
	iunlock(u.u_cdir);
	u.u_rdir = NULL;

#ifdef INET
	if (netoff = netinit())
		printf("Network init failed\n");
	else
		NETSTART();
#endif

/*
 * This came from pdp/machdep2.c because the memory available statements
 * were being made _before_ memory for the networking code was allocated.
 * A side effect of moving this code is that network "attach" and MSCP 
 * "online" messages can appear before the memory sizes.  The (currently
 * safe) assumption is made that no 'free' calls are made so that the
 * size in the first entry of the core map is correct.
*/
	printf("\nphys mem  = %D\n", ctob((long)physmem));
	printf("avail mem = %D\n", ctob((long)_coremap[0].m_size));
	maxmem = MAXMEM;
	printf("user mem  = %D\n", ctob((long)MAXMEM));
#if NRAM > 0
	printf("ram disk  = %D\n", ctob((long)ramsize));
#endif
	printf("\n");

	/*
	 * make init process
	 */
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
	for (i = 0; i < nbuf; i++, paddr += MAXBSIZE) {
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

#ifdef INET
memaddr netdata;		/* click address of start of net data */

/*
 * We are called here after all the other init routines (clist, inode,
 * unibusmap, etc...) have been called.  Open the
 * file NETNIX and read the a.out header, based on that go allocate
 * memory and read the text+data into the memory.  Set up supervisor page
 * registers, SDSA6 and SDSA7 have already been set up in mch_start.s.
 */

static char NETNIX[] = "/netnix";

static
netinit()
{
	register u_short *ap, *dp;
	register int i;
	struct exec ex;
	struct inode *ip;
	memaddr nettext;
	long lsize;
	off_t	off;
	int initdata, netdsize, nettsize, ret, err, resid;
	char oneclick[ctob(1)];
	register struct	nameidata *ndp = &u.u_nd;

	ret = 1;
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_SYSSPACE;
	ndp->ni_dirp = NETNIX;
	if (!(ip = namei(ndp))) {
		printf("%s not found\n", NETNIX);
		goto leave;
	}
	if ((ip->i_mode & IFMT) != IFREG || !ip->i_size) {
		printf("%s bad inode\n", NETNIX);
		goto leave;
	}
	err = rdwri(UIO_READ, ip, &ex, sizeof (ex), (off_t)0, UIO_SYSSPACE,
			IO_UNIT, &resid);
	if (err || resid) {
		printf("%s header err %d\n", NETNIX, ret);
		goto leave;
	}
	if (ex.a_magic != A_MAGIC3) {
		printf("%s bad magic %o\n", NETNIX, ex.a_magic);
		goto leave;
	}
	lsize = (long)ex.a_data + (long)ex.a_bss;
	if (lsize > 48L * 1024L) {
		printf("%s too big %ld\n", NETNIX, lsize);
		goto leave;
	}
	nettsize = btoc(ex.a_text);
	nettext = (memaddr)malloc(coremap, nettsize);
	netdsize = btoc(ex.a_data + ex.a_bss);
	netdata = (memaddr)malloc(coremap, netdsize);
	initdata = ex.a_data >> 6;
	off = sizeof (ex);
	for (i = 0; i < nettsize; i++) {
		err = rdwri(UIO_READ, ip, oneclick, ctob(1), off, UIO_SYSSPACE,
				IO_UNIT, &resid);
		if (err || resid)
			goto release;
		mapseg5(nettext + i, 077406);
		bcopy(oneclick, SEG5, ctob(1));
		off += ctob(1);
		normalseg5();
	}
	for (i = 0; i < initdata; i++) {
		err = rdwri(UIO_READ, ip, oneclick, ctob(1), off, UIO_SYSSPACE,
				IO_UNIT, &resid);
		if (err || resid)
			goto release;
		mapseg5(netdata + i, 077406);
		bcopy(oneclick, SEG5, ctob(1));
		normalseg5();
		off += ctob(1);
	}
	if (ex.a_data & 077) {
		err = rdwri(UIO_READ, ip, oneclick, ex.a_data & 077, off,
				UIO_SYSSPACE, IO_UNIT, &resid);
		if (err || resid) {
release:		printf("%s err %d\n", NETNIX, err);
			mfree(coremap, nettsize, nettext);
			mfree(coremap, netdsize, netdata);
			nettsize = netdsize = 0;
			netdata = nettext = 0;
			goto leave;
		}
		mapseg5(netdata + i, 077406);	/* i is set from above loop */
		bcopy(oneclick, SEG5, ex.a_data & 077);
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
	ret = 0;
leave:	if (ip)
		iput(ip);
	u.u_error = 0;
	ndp->ni_dirp = 0;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_endoff = 0;
	bzero(&u.u_ncache, sizeof(u.u_ncache));
	bzero(&ndp->ni_dent, sizeof(ndp->ni_dent));
	if (ndp->ni_pdir) {
		iput(ndp->ni_pdir);
		ndp->ni_pdir = 0;
	}
	return(ret);
}
#endif
