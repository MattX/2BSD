/*
 * RM02 disc driver (hacked from hp.c)-wfj
 * USGS Menlo Park --- call Peter Ward for  further info
 * January 80
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/dir.h>
#include <sys/user.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)rm.c	3.3";
#endif

#define	DK_N	0


struct	device
{
	union {
		int	w;
		char	c[2];
	} rmcs1;		/* Control and Status register 1 */
	int	rmwc;		/* Word count register */
	caddr_t	rmba;		/* UNIBUS address register */
	int	rmda;		/* Desired address register */
	union {
		int	w;
		char	c[2];
	} rmcs2;		/* Control and Status register 2*/
	int	rmds;		/* Drive Status */
	int	rmer1;		/* Error register 1 */
	int	rmas;		/* Attention Summary */
	int	rmla;		/* Look ahead */
	int	rmdb;		/* Data buffer */
	int	rmmr1;		/* Maintenance register 1 */
	int	rmdt;		/* Drive type */
	int	rmsn;		/* Serial number */
	int	rmof;		/* Offset register */
	int	rmdc;		/* Desired Cylinder register*/
	int	rmhr;		/* Holding register - unused*/
	int	rmmr2;		/* Maintenence register 2 */
	int	rmer2;		/* Error register 2 */
	int	rmec1;		/* Burst error bit position */
	int	rmec2;		/* Burst error bit pattern */
	int	rmbae;		/* 11/70 bus extension */
	int	rmcs3;
};

#define	RMADDR	((struct device *)0176700)
#define	NRM	1
#define	NSECT	32
#define	NTRAC	5
#define	SDIST	2
#define	RDIST	6

struct	size
{
	daddr_t	nblocks;
	int	cyloff;
} rm_sizes[8] =
{
	4800,	0,	/* root		cyl 000-029 */
	1280,	30,	/* swap		cyl 030-037 */
	125600,38,	/* usr		cyl 038-822 */
	62720,	38,	/* <alt fs>	cyl 038-429 */
	62880,	430,	/* <alt fs>	cyl 430-822 */
	0,	0,
	0,	0,
	0,	0,
};

#define	P400	020
#define	M400	0220
#define	P800	040
#define	M800	0240
#define	P1200	060
#define	M1200	0260
int	rm_offset[16] =
{
	P400, M400, P400, M400,
	P800, M800, P800, M800,
	P1200, M1200, P1200, M1200,
	0, 0, 0, 0,
};

struct	buf	rmtab;
struct	buf	rrmbuf;
struct	buf	rmutab[NRM];

#define	GO	01
#define	PRESET	020
#define	RTC	016
#define	OFFSET	014
#define	SEARCH	030
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
#define FMT16	010000

#define	b_cylin	b_resid

daddr_t dkblock();
int	rmcc[8]={ -1, -1, -1, -1, -1, -1, -1, -1}; 	/* Current cylinder */

rmstrategy(bp)
register struct buf *bp;
{
	register struct buf *dp;
	register unit;
	long sz, bn;

	unit = minor(bp->b_dev) & 077;
	sz = bp->b_bcount;
	sz = (sz+511) >> 9;
	if (unit >= (NRM<<3) ||
	    bp->b_blkno < 0 ||
	    (bn = dkblock(bp))+sz > rm_sizes[unit&07].nblocks) {
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return;
	}
#ifdef	UNIBUS_MAP
	if (bp->b_flags & B_PHYS)
#ifndef RH11
		if(cputype != 70)
#endif
			mapalloc(bp);
#endif
	bp->b_cylin = bn/(NSECT*NTRAC) + rm_sizes[unit&07].cyloff;
	unit = dkunit(bp);
	dp = &rmutab[unit];
	spl5();
	disksort(dp, bp);
	if (dp->b_active == 0) {
		rmustart(unit);
		if(rmtab.b_active == 0)
			rmstart();
	}
	spl0();
}

rmustart(unit)
register unit;
{
	register struct buf *bp, *dp;
	daddr_t bn;
	int sn, cn, csn;

	RMADDR->rmcs2.w = unit;
	RMADDR->rmcs1.c[0] = IE;
	RMADDR->rmas = 1<<unit;

	if(unit >= NRM)
		return;
	dk_busy &= ~(1<<(unit+DK_N));
	dp = &rmutab[unit];
	if((bp=dp->b_actf) == NULL)
		return;
	if((RMADDR->rmds & VV) == 0) {
		RMADDR->rmcs1.c[0] = IE|PRESET|GO;
		RMADDR->rmof = FMT16;
	}
	if(dp->b_active)
		goto done;
	dp->b_active++;
	if ((RMADDR->rmds & (DPR|MOL)) != (DPR|MOL))
		goto done;

	bn = dkblock(bp);
	cn = bp->b_cylin;
	sn = bn%(NSECT*NTRAC);
	sn = (sn+NSECT-SDIST)%NSECT;

	if(rmcc[unit] <0 || rmcc[unit] != cn)
		goto search;
	csn = (RMADDR->rmla>>6) - sn + SDIST - 1;
	if(csn < 0)
		csn += NSECT;
	if(csn > NSECT-RDIST)
		goto done;

search:
	RMADDR->rmdc = cn;
	RMADDR->rmda = sn;
	RMADDR->rmcs1.c[0] = IE|SEARCH|GO;
	unit += DK_N;
	dk_busy |= 1<<unit;
	dk_numb[unit] += 1;
	return;

done:
	dp->b_forw = NULL;
	if(rmtab.b_actf == NULL)
		rmtab.b_actf = dp; else
		rmtab.b_actl->b_forw = dp;
	rmtab.b_actl = dp;
}

rmstart()
{
	register struct buf *bp, *dp;
	register unit;
	daddr_t bn;
	int dn, sn, tn, cn;

loop:
	if ((dp = rmtab.b_actf) == NULL)
		return;
	if ((bp = dp->b_actf) == NULL) {
		rmtab.b_actf = dp->b_forw;
		goto loop;
	}
	rmtab.b_active++;
	unit = minor(bp->b_dev) & 077;
	dn = dkunit(bp);
	bn = dkblock(bp);
	cn = bn/(NSECT*NTRAC) + rm_sizes[unit&07].cyloff;
	sn = bn%(NSECT*NTRAC);
	tn = sn/NSECT;
	sn = sn%NSECT;

	RMADDR->rmcs2.w = dn;
	if ((RMADDR->rmds & (DPR|MOL)) != (DPR|MOL)) {
		rmtab.b_active = 0;
		rmtab.b_errcnt = 0;
		dp->b_actf = bp->av_forw;
		bp->b_flags |= B_ERROR;
		iodone(bp);
		goto loop;
	}
	if(rmtab.b_errcnt >= 16) {
		RMADDR->rmof = rm_offset[rmtab.b_errcnt & 017] | FMT16;
		RMADDR->rmcs1.w = OFFSET|GO;
		while(RMADDR->rmds & PIP)
			;
	}
	RMADDR->rmdc = cn;
	RMADDR->rmda = (tn << 8) + sn;
	RMADDR->rmba = bp->b_un.b_addr;
	if(cputype == 70)
		RMADDR->rmbae = bp->b_xmem;
	RMADDR->rmwc = -(bp->b_bcount>>1);
	unit = ((bp->b_xmem&3) << 8) | IE | GO;
	if(bp->b_flags & B_READ)
		unit |= RCOM; else
		unit |= WCOM;
	RMADDR->rmcs1.w = unit;

	dk_busy |= 1<<(DK_N+NRM);
	dk_numb[DK_N+NRM] += 1;
	unit = bp->b_bcount>>6;
	dk_wds[DK_N+NRM] += unit;
}

rmintr()
{
	register struct buf *bp, *dp;
	register unit;
	int as, i, j;

	as = RMADDR->rmas & 0377;
	if(rmtab.b_active) {
		dk_busy &= ~(1<<(DK_N+NRM));
		dp = rmtab.b_actf;
		bp = dp->b_actf;
		unit = dkunit(bp);
		RMADDR->rmcs2.c[0] = unit;
		if (RMADDR->rmcs1.w & TRE) {		/* error bit */
			while((RMADDR->rmds & DRY) == 0)
				;
			if(++rmtab.b_errcnt > 28 || RMADDR->rmer1&WLE)
				bp->b_flags |= B_ERROR; else
				rmtab.b_active = 0;
			if(rmtab.b_errcnt > 27)
				deverror(bp, RMADDR->rmcs2.w, RMADDR->rmer1);
#ifndef UCB_BUFOUT
#ifndef UCB_NKB
			if((bp->b_flags&B_PHYS) == 0 &&
			   (RMADDR->rmer1 & (DCK|ECH)) == DCK) {
				i = RMADDR->rmec1 - 1;
				j = i&017;
				i >>= 4;
				if(i >= 0 && i <256) {
					bp->b_un.b_words[i] ^= RMADDR->rmec2 << j;
					bp->b_un.b_words[i+1] ^= RMADDR->rmec2 >> (16-j);
				}
				rmtab.b_active++;
				printf("%D ", bp->b_blkno);
				prdev("ECC", bp->b_dev);
			}
#endif
#endif
			RMADDR->rmcs1.w = TRE|IE|DCLR|GO;
			if((rmtab.b_errcnt&07) == 4) {
				RMADDR->rmcs1.w = RECAL|IE|GO;
				while(RMADDR->rmds & PIP)
					;
			}
			rmcc[unit] = -1;
		}
		if(rmtab.b_active) {
			if(rmtab.b_errcnt) {
				RMADDR->rmcs1.w = RTC|GO;
				while(RMADDR->rmds & PIP)
					;
			}
			rmtab.b_active = 0;
			rmtab.b_errcnt = 0;
			rmtab.b_actf = dp->b_forw;
			dp->b_active = 0;
			dp->b_errcnt = 0;
			dp->b_actf = bp->av_forw;
			bp->b_resid = -(RMADDR->rmwc<<1);
			iodone(bp);
			RMADDR->rmcs1.w = IE;
			if(dp->b_actf)
			{
				if(rmcc[unit] >= 0)
					rmcc[unit] = bp->b_cylin;
				rmustart(unit);
			}
		}
		as &= ~(1<<unit);
	} else {
		if(as == 0)
			RMADDR->rmcs1.w = IE;
		RMADDR->rmcs1.c[1] = TRE>>8;
	}
	for(unit=0; unit<NRM; unit++)
		if(as & (1<<unit))
		{
			rmcc[unit] = -1;
			rmustart(unit);
		}
	rmstart();
}

rmread(dev)
{

	physio(rmstrategy, &rrmbuf, dev, B_READ);
}

rmwrite(dev)
{

	physio(rmstrategy, &rrmbuf, dev, B_WRITE);
}
