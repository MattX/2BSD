#

#define	DK_N	5
#define NSECT 07700

/*
 * RS03/04 disk driver
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/seg.h>
#ifdef	UCB_DKEXT
#define	NDK_N	3
#include <sys/dk.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)hs.c	3.1";
#endif

extern	struct	dk	dk;
#endif


struct device {
	int	hscs1;	/* Control and Status register 1 */
	int	hswc;	/* Word count register */
	int	hsba;	/* UNIBUS address register */
	int	hsda;	/* Desired address register */
	int	hscs2;	/* Control and Status register 2 */
	int	hsds;	/* Drive Status */
	int	hser;	/* Error register */
	int	hsas;	/* not used */
	int	hsla;	/* not used */
	int	hsdb;	/* not used */
	int	hsmr;	/* not used */
	int	hsdt;	/* not used */
	int	hsbae;	/* 11/70 bus extension */
};

struct	buf	hstab;
struct	buf	rhsbuf;

#define	HSADDR	((struct device *) 0172040)
#define	MEMADDR	((struct device *)1777740)

#define ERR	040000	/* hscs1 - composite error */

#define GO	01
#define RCLR	010
#define	DRY	0200	/* hsds - Drive Ready */
#define RHWCOM	060
#define RHRCOM	070
#define IENABLE	0100

hsstrategy(bp)
register struct buf *bp;
{
	register mblks;

	mblks = 1024; /* RJS03 */
	if(minor(bp->b_dev) >= 8)
		mblks = 2048; /* RJS04 */
	if(bp->b_blkno >= mblks) {
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return;
	}
	bp->av_forw = 0;
	spl5();
	if (hstab.b_actf==0)
		hstab.b_actf = bp; else
		hstab.b_actl->av_forw = bp;
	hstab.b_actl = bp;
	if (hstab.b_active==0)
		hsstart();
	spl0();
}

hsstart()
{
	register struct buf *bp;
	register com_addr;

	if ((bp = hstab.b_actf) == 0) {
		return;
	}
	hstab.b_active++;
	com_addr = bp->b_blkno;
	if(minor(bp->b_dev) < 8)
		com_addr <<= 1; /* RJS03 */
	HSADDR->hscs2 = minor(bp->b_dev) & 07;
	HSADDR->hsda = com_addr << 1;
	HSADDR->hsbae = bp->b_xmem;
	HSADDR->hsba = bp->b_un.b_addr;
	HSADDR->hswc = -(bp->b_bcount>>1);
	com_addr = IENABLE | GO | ((bp->b_xmem & 03) << 8);
	if(bp->b_flags & B_READ)
		HSADDR->hscs1 = com_addr | RHRCOM;
	else
		HSADDR->hscs1 = com_addr | RHWCOM;
	dk_busy |= 1<<DK_N;
	dk_numb[DK_N] += 1;
	dk_wds[DK_N] += (bp->b_bcount>>6) & 01777;
#ifdef	UCB_DKEXT
	dk.dk_1busy |= 1<<(2*NDK_N);
	dk.dk_wdsn[NDK_N] += (bp->b_bcount>>6) & 01777;
	dk.dk_nnumb[NDK_N] += 1;
#endif
}

hsintr()
{
	register struct buf *bp;
	register i;

	if (hstab.b_active == 0) {
		return;
	}
#ifdef	UCB_DKEXT
	dk.dk_1busy &= ~(1<<(2*NDK_N));
#endif
	dk_busy &= ~(1<<DK_N);
	bp = hstab.b_actf;
	hstab.b_active = 0;
	if(HSADDR->hscs1 & ERR){	/* error bit */
		deverror(bp, HSADDR->hscs1, HSADDR->hscs2);
		for (i=0; i<7; i++)
			printf("%o ", ((physadr)HSADDR)->r[i]);
		printf("\n");
		HSADDR->hscs1 = RCLR|GO;
		if (++hstab.b_errcnt <= 10) {
			hsstart();
			return;
		}
		bp->b_flags |= B_ERROR;
	}
	hstab.b_errcnt = 0;
	hstab.b_actf = bp->av_forw;
	iodone(bp);
	hsstart();
}

hsread(dev)
{

	physio(hsstrategy, &rhsbuf, dev, B_READ);
}

hswrite(dev)
{

	physio(hsstrategy, &rhsbuf, dev, B_WRITE);
}
