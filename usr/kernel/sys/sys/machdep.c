#include <sys/param.h>
#include <sys/systm.h>
#include <sys/acct.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/inode.h>
#include <sys/proc.h>
#include <sys/seg.h>
#include <sys/map.h>
#include <sys/reg.h>
#include <sys/buf.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)machdep.c	3.3";
#endif


#ifdef UCB_BUFOUT
extern int bpaddr;
int	kdsa5	= 0;		/* filled in by initialization */
int	kdsd5	= 0;		/* ditto */
#endif
#ifdef UCB_CLIST
extern int clsize;
#ifdef NONSEPARATE
extern int kisa1, kisd1;
#else
extern int kdsa1;
#endif
#endif

/*
 * Icode is the octal bootstrap
 * program executed in user mode
 * to bring up the system.
 */
int	icode[] =
{
	0104413,	/* sys exec; init; initp */
	0000014,
	0000010,
	0000777,	/* br . */
	0000014,	/* initp: init; 0 */
	0000000,
	0062457,	/* init: </etc/init\0> */
	0061564,
	0064457,
	0064556,
	0000164,
};
int	szicode = sizeof(icode);

int	kisa0;		/* location of kernel text (clicks) */


/*
 * Machine-dependent startup code
 */
startup()
{
	register i;
#ifdef	UCB_BUFOUT
#ifdef UNIBUS_MAP
	long paddr;
	register struct ubmap *ubp;
#endif
#endif

	/*
	 * zero and free all of core
	 */

	i = ka6->r[0] + USIZE;
	UISD->r[0] = 077406;
	for(;;) {
		UISA->r[0] = i;
		if(fuibyte((caddr_t)0) < 0)
			break;
		clearseg(i);
		maxmem++;
		mfree(coremap, (unsigned) 1, (unsigned) i);
		i++;
	}
#ifdef	NONSEPARATE
#ifdef UCB_BUFOUT
	if ((bpaddr = malloc(coremap, (unsigned) btoc(NBUF*BSIZE))) == 0)
		panic("buffers");
	maxmem -= btoc(NBUF*BSIZE);
#endif
#ifdef UCB_CLIST
	maxmem -= btoc(clsize);
#endif
#endif
	printf("mem = %D\n", ctob((long)maxmem));
	if(MAXMEM < maxmem)
		maxmem = MAXMEM;
#ifndef UCB_NKB
	mfree(swapmap, (unsigned) nswap, (unsigned) 1);
#else
	mfree(swapmap, (unsigned) nswap - (unsigned) CLSIZE, (unsigned) CLSIZE);
#endif
	swplo--;
#ifdef	UCB_BUFOUT
	kisa0 = KISA0->r[0];
	kdsa5 = KDSA5->r[0];
	kdsd5 = KDSD5->r[0];
#endif
#ifdef	UCB_CLIST
#ifndef NONSEPARATE
	kdsa1 = KDSA1->r[0];
#else
	kisa1 = KISA1->r[0];
	kisd1 = KISD1->r[0];
#endif
#endif

#ifdef	UNIBUS_MAP
	if((cputype == 70) || (cputype == 44)) {
#ifdef	UCB_BUFOUT
		paddr = 0;
		for(ubp = UBMAP; ubp < &UBMAP[23]; ubp++) {
			ubp->ub_lo = loint(paddr);
			ubp->ub_hi = hiint(paddr);
			paddr += UBPAGE;
		}
		if(kisa0 >= (paddr>>6)) {
			printf("I-space at %o00, last fixed ubmap %O\n", kisa0,
								paddr-UBPAGE);
			panic("buffers+clists too big");
		}
#else
		for(i=0; i<62; i+=2) {
			UBMAP->r[i] = i<<12;
			UBMAP->r[i+1] = 0;
		}
#endif
		SSR3->r[0] |= UBMAPON;
	}
#endif

	UISA->r[7] = ka6->r[1]; /* io segment */
	UISD->r[7] = 077406;
}

/*
 * set up a physical address
 * into users virtual address space.
 */
sysphys()
{
	register i, s, d;
	register struct a {
		int	segno;
		int	size;
		int	phys;
	} *uap;

	if(!suser())
		return;
	uap = (struct a *)u.u_ap;
	i = uap->segno;
	if(i < 0 || i >= 8)
		goto bad;
	s = uap->size;
	if(s < 0 || s > 128)
		goto bad;
	d = u.u_uisd[i+8];
	if(d != 0 && (d&ABS) == 0)
		goto bad;
	u.u_uisd[i+8] = 0;
	u.u_uisa[i+8] = 0;
	if(!u.u_sep) {
		u.u_uisd[i] = 0;
		u.u_uisa[i] = 0;
	}
	if(s) {
		u.u_uisd[i+8] = ((s-1)<<8) | RW|ABS;
		u.u_uisa[i+8] = uap->phys;
		if(!u.u_sep) {
			u.u_uisa[i] = u.u_uisa[i+8];
			u.u_uisd[i] = u.u_uisd[i+8];
		}
	}
	sureg();
	return;

bad:
	u.u_error = EINVAL;
}

/*
 * Determine which clock is attached, and start it.
 * panic: no clock found
 */
#define	CLOCK1	((physadr)0177546)
#define	CLOCK2	((physadr)0172540)
clkstart()
{
	lks = CLOCK1;
	if(fuiword((caddr_t)lks) == -1) {
		lks = CLOCK2;
		if(fuiword((caddr_t)lks) == -1)
#ifndef	CHEAP1123
			panic("no clock");
#else
		{
			printf("no clock??\n");
			lks = 0;
		}
#endif
	}
	lks->r[0] = 0115;
}

/*
 * Let a process handle a signal by simulating an interrupt
 */
sendsig(p, signo)
caddr_t p;
{
	register unsigned n;

	n = u.u_ar0[R6] - 4;
	grow(n);
	suword((caddr_t)n+2, u.u_ar0[RPS]);
	suword((caddr_t)n, u.u_ar0[R7]);
	u.u_ar0[R6] = n;
	u.u_ar0[RPS] &= ~TBIT;
	u.u_ar0[R7] = (int)p;
}

#ifdef UNIBUS_MAP
/*
 * 11/70 routine to allocate the
 * UNIBUS map and initialize for
 * a unibus device.
 * The code here and in
 * rhstart assumes that an rh on an 11/70
 * is an rh70 and contains 22 bit addressing.
 */
int	maplock = { 0 };

mapalloc(bp)
register struct buf *bp;
{
#ifdef	UCB_BUFOUT
	register struct ubmap *ubp;
	long paddr;
#else
	register i, a;
#endif

	if((cputype != 70) && (cputype != 44))
		return;
	spl6();
	while(maplock&B_BUSY) {
		maplock |= B_WANTED;
		sleep((caddr_t)&maplock, PSWP+1);
	}
	maplock |= B_BUSY;
	spl0();
	bp->b_flags |= B_MAP;
#ifdef	UCB_BUFOUT
	paddr = (unsigned) bp->b_xmem;
	paddr <<= 16;
	paddr |= (long) ((unsigned)bp->b_un.b_addr);
	for(ubp = &UBMAP[23]; ubp < &UBMAP[31]; ubp++) {
		ubp->ub_lo = loint(paddr);
		ubp->ub_hi = hiint(paddr);
		paddr += UBPAGE;
	}
	bp->b_un.b_addr = 0160000;
	bp->b_xmem = 02;
#else
	a = bp->b_xmem;
	for(i=16; i<32; i+=2)
		UBMAP->r[i+1] = a;
	for(a++; i<48; i+=2)
		UBMAP->r[i+1] = a;
	bp->b_xmem = 1;
#endif
}

mapfree(bp)
struct buf *bp;
{

	bp->b_flags &= ~B_MAP;
	if(maplock&B_WANTED)
		wakeup((caddr_t)&maplock);
	maplock = 0;
}
#endif

#ifdef	UCB_BUFOUT

struct	buf	*hasmap = (struct buf *) 0;

caddr_t
mapin(bp)
register struct buf *bp;
{
	register caddr_t paddr;
	register caddr_t offset;

	spl5();	/* NOTE: ONLY CLOCK AND DU ARE CERTIFIED SAFE. */
	if(hasmap != (struct buf *) 0) {
		printf("mapping %o over %o\n", bp, hasmap);
		panic("mapin");
	}
	hasmap = bp;
	offset = bp->b_un.b_addr & 077;
	paddr = (bp->b_un.b_addr >> 6) & 01777;
	paddr |= bp->b_xmem << 10;
	KDSA5->r[0] = paddr;
	KDSD5->r[0] = (BSIZE << 2) | RW;
	return(SEG5 + offset);
}

mapout(bp)
struct buf *bp;
{
	if(bp != hasmap) {
		printf("unmapping %o, not %o\n", bp, hasmap);
		panic("mapout");
	}
	hasmap = (struct buf *) 0;
	KDSA5->r[0] = kdsa5;
	KDSD5->r[0] = kdsd5;
	spl0();
}

#endif
