/*
 * RP04/RP06 disk driver
 * Modified for Diva Comp V 7/80 Bob Kridle
 * Names are changed so it can coexist with hp.c
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/dir.h>
#include <sys/user.h>
#ifdef	UCB_DKEXT
#include <sys/dk.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)dvhp.c	3.2";
#endif

extern	struct	dk	dk;
#define	NDK_N	0
#endif

#define	DK_N	0

struct	device
{
	union {
		int	w;
		char	c[2];
	} dvhpcs1;		/* Control and Status register 1 */
	int	dvhpwc;		/* Word count register */
	caddr_t	dvhpba;		/* UNIBUS address register */
	int	dvhpda;		/* Desired address register */
	union {
		int	w;
		char	c[2];
	} dvhpcs2;		/* Control and Status register 2*/
	int	dvhpds;		/* Drive Status */
	int	dvhper1;		/* Error register 1 */
	int	dvhpas;		/* Attention Summary */
	int	dvhpla;		/* Look ahead */
	int	dvhpdb;		/* Data buffer */
	int	dvhpmr;		/* Maintenance register */
	int	dvhpdt;		/* Drive type */
	int	dvhpsn;		/* Serial number */
	int	dvhpof;		/* Offset register */
	int	dvhpdc;		/* Desired Cylinder address register*/
	int	dvhpcc;		/* Current Cylinder */
	int	dvhper2;		/* Error register 2 */
	int	dvhper3;		/* Error register 3 */
	int	dvhpec1;		/* Burst error bit position */
	int	dvhpec2;		/* Burst error bit pattern */
	int	dvhpbae;		/* 11/70 bus extension */
	int	dvhpcs3;
};

#define	DVHPADDR	((struct device *)0176700)
#define	NDVHP	2
#define	NSECT	33
#define	NTRAC	19
#define	SDIST	2
#define	RDIST	6

struct	size
{
	daddr_t	nblocks;
	int	cyloff;
} dvhp_sizes[8] =
{
	9405,	0,		/* cyl   0 thru  14 */
	125400,	15,		/* cyl  15 thru	214 */
	125400,	215,		/* cyl 215 thru 414 */
	125400,	415,		/* cyl 415 thru 614 */
	125400,	615,		/* cyl 615 thru 814 */
	250800,	15,		/* cyl  15 thru 414 */
	250800,	415,		/* cyl 415 thru 814 */
	501600, 15		/* cyl  15 thru 814 */
};

#define	P400	020
#define	M400	0220
#define	P800	040
#define	M800	0240
#define	P1200	060
#define	M1200	0260
int	dvhp_offset[16] =
{
	P400, M400, P400, M400,
	P800, M800, P800, M800,
	P1200, M1200, P1200, M1200,
	0, 0, 0, 0,
};

struct	buf	dvhptab;
struct	buf	rdvhpbuf;
struct	buf	dvhputab[NDVHP];

#define	GO	01
#define	PRESET	020
#define	RTC	016
#define	OFFSET	014
#define	SEARCH	030
#define SEEK	04
#define	RECAL	06
#define DCLR	010
#define	WCOM	060
#define	RCOM	070

#define	IE	0100
#define	PIP	020000
#define	DRY	0200
#define	ERR	040000
#define	TRE	040000
#define	DCK	0100000
#define	WLE	04000
#define	ECH	0100
#define VV	0100
#define	DPR	0400
#define	MOL	010000
#define FMT22	010000

#define	b_cylin	b_resid

daddr_t dkblock();

dvhpstrategy(bp)
register struct buf *bp;
{
	register struct buf *dp;
	register unit;
	long sz, bn;

	unit = minor(bp->b_dev) & 077;
	sz = bp->b_bcount;
	sz = (sz+511) >> 9;
	if (unit >= (NDVHP<<3) ||
	    bp->b_blkno < 0 ||
	    (bn = dkblock(bp))+sz > dvhp_sizes[unit&07].nblocks) {
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return;
	}
	bp->b_cylin = bn/(NSECT*NTRAC) + dvhp_sizes[unit&07].cyloff;
	unit = dkunit(bp);
	dp = &dvhputab[unit];
	spl5();
	disksort(dp, bp);
	if (dp->b_active == 0) {
		dvhpustart(unit);
		if(dvhptab.b_active == 0)
			dvhpstart();
	}
	spl0();
}

dvhpustart(unit)
register unit;
{
	register struct buf *bp, *dp;
	daddr_t bn;
	int sn, cn, csn;

	DVHPADDR->dvhpcs2.w = unit;
	DVHPADDR->dvhpcs1.c[0] = IE;
	DVHPADDR->dvhpas = 1<<unit;

	if(unit >= NDVHP)
		return;
	dk_busy &= ~(1<<(unit+DK_N));
#ifdef	UCB_DKEXT
	dk.dk_0busy &= ~(2<<(2*(unit)));			/* clear seek bit */
	dk.dk_0busy &= ~(1<<(2*(unit)));			/* clear busy bit */
#endif
	dp = &dvhputab[unit];
	if((bp=dp->b_actf) == NULL)
		return;
	if((DVHPADDR->dvhpds & VV) == 0) {
		DVHPADDR->dvhpcs1.c[0] = IE|PRESET|GO;
		DVHPADDR->dvhpof = FMT22;
	}
	if(dp->b_active)
		goto done;
	dp->b_active++;
	if ((DVHPADDR->dvhpds & (DPR|MOL)) != (DPR|MOL))
		goto done;

	bn = dkblock(bp);
	cn = bp->b_cylin;
	sn = bn%(NSECT*NTRAC);
	sn = (sn+NSECT-SDIST)%NSECT;

	if(DVHPADDR->dvhpcc != cn) {
		DVHPADDR->dvhpdc = cn;
		DVHPADDR->dvhpcs1.c[0] = IE|SEEK|GO;
		unit += DK_N;
		dk_busy |= 1<<unit;
#ifdef	UCB_DKEXT
		dk.dk_0busy |= 2<<(2*unit);		/* set the seek bit for disk */
		dk.dk_nnumb[unit] += 1;			/* inc number of transfers made */
#endif
		dk_numb[unit] += 1;
		return;
	}
done:
	dp->b_forw = NULL;
	if(dvhptab.b_actf == NULL)
		dvhptab.b_actf = dp; else
		dvhptab.b_actl->b_forw = dp;
	dvhptab.b_actl = dp;
}

dvhpstart()
{
	register struct buf *bp, *dp;
	register unit;
	daddr_t bn;
	int dn, sn, tn, cn;
#ifdef	UCB_DKEXT
	int	dk_unit;		/* drive number */
#endif

loop:
	if ((dp = dvhptab.b_actf) == NULL)
		return;
	if ((bp = dp->b_actf) == NULL) {
		dvhptab.b_actf = dp->b_forw;
		goto loop;
	}
	dvhptab.b_active++;
	unit = minor(bp->b_dev) & 077;
	dn = dkunit(bp);
	bn = dkblock(bp);
	cn = bn/(NSECT*NTRAC) + dvhp_sizes[unit&07].cyloff;
	sn = bn%(NSECT*NTRAC);
	tn = sn/NSECT;
	sn = sn%NSECT;

	DVHPADDR->dvhpcs2.w = dn;
	if ((DVHPADDR->dvhpds & (DPR|MOL)) != (DPR|MOL)) {
		dvhptab.b_active = 0;
		dvhptab.b_errcnt = 0;
		dp->b_actf = bp->av_forw;
		bp->b_flags |= B_ERROR;
		iodone(bp);
		goto loop;
	}
	if(dvhptab.b_errcnt >= 16) {
		DVHPADDR->dvhpof = dvhp_offset[dvhptab.b_errcnt & 017] | FMT22;
		DVHPADDR->dvhpcs1.w = OFFSET|GO;
		while(DVHPADDR->dvhpds & PIP)
			;
	}
	DVHPADDR->dvhpdc = cn;
	DVHPADDR->dvhpda = (tn << 8) + sn;
	DVHPADDR->dvhpba = bp->b_un.b_addr;
	if(cputype == 70)
		DVHPADDR->dvhpbae = bp->b_xmem;
	DVHPADDR->dvhpwc = -(bp->b_bcount>>1);
	unit = ((bp->b_xmem&3) << 8) | IE | GO;
	if(bp->b_flags & B_READ)
		unit |= RCOM; else
		unit |= WCOM;
	DVHPADDR->dvhpcs1.w = unit;

	dk_busy |= 1<<(DK_N+NDVHP);
#ifdef	UCB_DKEXT
	dk_unit = dkunit(bp);				/* disk number */
	dk.dk_0busy |= 1<<(2*(dk_unit));		/* set busy bit */
	dk.dk_nnumb[NDVHP] += 1;				/* inc # of transfers for controller */
#endif
	dk_numb[DK_N+NDVHP] += 1;
	unit = bp->b_bcount>>6;
	dk_wds[DK_N+NDVHP] += unit;
#ifdef	UCB_DKEXT
	dk.dk_wdsn[NDVHP] += unit;			/* inc # of words for controller */
	dk.dk_wdsn[dk_unit] += unit;			/* inc # of words for controller */
#endif
}

dvhpintr()
{
	register struct buf *bp, *dp;
	register unit;
	int as, i, j;

	as = DVHPADDR->dvhpas & 0377;
	if(dvhptab.b_active) {
		dk_busy &= ~(1<<(DK_N+NDVHP));
#ifdef	UCB_DKEXT
		dk.dk_0busy = 0;			/* clear ALL bits */
#endif
		dp = dvhptab.b_actf;
		bp = dp->b_actf;
		unit = dkunit(bp);
		DVHPADDR->dvhpcs2.c[0] = unit;
		if (DVHPADDR->dvhpcs1.w & TRE) {		/* error bit */
			while((DVHPADDR->dvhpds & DRY) == 0)
				;
			if(++dvhptab.b_errcnt > 28 || DVHPADDR->dvhper1&WLE)
				bp->b_flags |= B_ERROR; else
				dvhptab.b_active = 0;
			if(dvhptab.b_errcnt > 27)
				deverror(bp, DVHPADDR->dvhpcs2.w, DVHPADDR->dvhper1);
			DVHPADDR->dvhpcs1.w = TRE|IE|DCLR|GO;
			if((dvhptab.b_errcnt&07) == 4) {
				DVHPADDR->dvhpcs1.w = RECAL|IE|GO;
				while(DVHPADDR->dvhpds & PIP)
					;
			}
		}
		if(dvhptab.b_active) {
			if(dvhptab.b_errcnt) {
				DVHPADDR->dvhpcs1.w = RTC|GO;
				while(DVHPADDR->dvhpds & PIP)
					;
			}
			dvhptab.b_active = 0;
			dvhptab.b_errcnt = 0;
			dvhptab.b_actf = dp->b_forw;
			dp->b_active = 0;
			dp->b_errcnt = 0;
			dp->b_actf = bp->av_forw;
			bp->b_resid = -(DVHPADDR->dvhpwc<<1);
			iodone(bp);
			DVHPADDR->dvhpcs1.w = IE;
			if(dp->b_actf)
				dvhpustart(unit);
		}
		as &= ~(1<<unit);
	} else {
		if(as == 0)
			DVHPADDR->dvhpcs1.w = IE;
		DVHPADDR->dvhpcs1.c[1] = TRE>>8;
	}
	for(unit=0; unit<NDVHP; unit++)
		if(as & (1<<unit))
			dvhpustart(unit);
	dvhpstart();
}

dvhpread(dev)
{

	physio(dvhpstrategy, &rdvhpbuf, dev, B_READ);
}

dvhpwrite(dev)
{

	physio(dvhpstrategy, &rdvhpbuf, dev, B_WRITE);
}
