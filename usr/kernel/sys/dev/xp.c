/*	From chico!teklabs!clemc  Wed May 13 03:15:28 1981
	To: chico!ucbvax!kridle, chico!ucbvax!william
	Cc: markh
	Subject: xp.c for 2BSD
	Status: R

	Here is a new xp.c that has support for the RM02 and the RM05.
	It is a mod between the 4BSD and current 2BSD drivers.  I will
	send support for the Standalone system later.  I will also send
	a working hk.c (RK07) with standalone support, when I get them
	back from the turkeys that are moding the one I wrote about
	a month ago.  I think 2BSD should include a much better
	MDEC when this whole mess is finished.  By the way does any
	one have 1K Boot Block for a RK05.  I can write one, but
	if somebody could save me the effort....

	Clem
*/

/*
 * RP04/RP06 disk driver
 * ECC support hacked from VAX code by wnj---wfj 12/30/80
 * RM02/03/05 Support by Clem Cole Tektronix 04/20/81
 * RM02/3 has been tested, but not the RM05
 */


/* NO_SEARCH Turned ON 6/20/81
 *
 * For the benefit of those who have old non-DEC controllers that
 * don't implement this command. (It hangs the DIVA COMP V !!)
 */

#define	NO_SEARCH

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/conf.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/seg.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "%W%";
#endif

/*
 * MENLO_ECC Turned OFF 6/20/81
 *
 * Clem updated a version of the driver in which ECC was known to work
 * incorrectly. I have hacked in a supposedly fixed version, but have not
 * had time to test it. It is much better not to have this feature than
 * to have it work wrong. Turn it on only if you intend to test it or you will
 * assuredly be screwed next time yo have a hardware problem - Bob Kridle
 */

#undef	MENLO_ECC

#define	DK_N	0

struct	device
{
	union {
		int	w;
		char	c[2];
	} xpcs1;		/* Control and Status register 1 */
	int	xpwc;		/* Word count register */
	caddr_t	xpba;		/* UNIBUS address register */
	int	xpda;		/* Desired address register */
	union {
		int	w;
		char	c[2];
	} xpcs2;		/* Control and Status register 2*/
	int	xpds;		/* Drive Status */
	int	xper1;		/* Error register 1 */
	int	xpas;		/* Attention Summary */
	int	xpla;		/* Look ahead */
	int	xpdb;		/* Data buffer */
	int	xpmr;		/* Maintenance register */
	int	xpdt;		/* Drive type */
	int	xpsn;		/* Serial number */
	int	xpof;		/* Offset register */
	int	xpdc;		/* Desired Cylinder address register*/
	int	xpcc;		/* Current Cylinder */
	int	xper2;		/* Error register 2 */
	int	xper3;		/* Error register 3 */
	int	xpec1;		/* Burst error bit position */
	int	xpec2;		/* Burst error bit pattern */
	int	xpbae;		/* 11/70 bus extension */
	int	xpcs3;
};

#define	XPADDR	((struct device *)0176700)
#define	NXP	1
#define	SDIST	2
#define	RDIST	6

/*
 * Defines for Disk Type Independence
 */
#define	RP	022		/* RP04/5/6 */
#define	RM	024		/* RM02/3 */
#define	RM5	027		/* RM05 */
#define	RM5X	025		/* Ampex 815 cyl. rm05 with Emulex Controller */
#define DV	077		/* Diva Comp VI Cntr; drive ID is a dummy */
				/* controller actually reads 22 and xp_type */
				/* must be patched on the fly */
#define	HP_SECT	22
#define	HP_TRAC	19
#define	RM_SECT	32
#define	RM_TRAC	5
#define DV_SECT 33
#define DV_TRAC	19

char	xp_type[NXP] = { 0 };	/* For Drive Type */

struct	size
{
	daddr_t	nblocks;
	int	cyloff;
}
	hp_sizes[8] =			/* RP04/5/6 */
{
	10450,	0,		/* cyl 0 thru 24 */
	330220,	25,		/* cyl 25 thru 814 (rp06) */
	161348,	25,		/* cyl 25 thru 410 (rp04/05) */
	0,	0,
	0,	0,
	0,	0,
	0,	0,
	0,	0,
},
	rm_sizes[8] =		/* RM02/3 */
{
	10240,	0,		/* cyl 0 thru 63 */
	121440,	64,		/* cyl 64 thru 823 */
	0,	0,
	0,	0,
	0,	0,
	0,	0,
	0,	0,
	0,	0,
},
	rm5_sizes[8] =		/* RM05 */
{
	10336,  0,		/* cyl   0 thru  16 */
	484576, 17,		/* cyl  17 thru  815 */
	0,	0,
	0,	0,
	0,	0,
	0,	0,
	0,	0,
	0,	0,
},
	dv_sizes[8] =		/* Diva Comp V + Ampex 9300 in direct mode */
{
	9405,  0,		/* cyl   0 thru  15 */
	500346, 17,		/* cyl  17 thru  815 */
	0,	0,
	0,	0,
	0,	0,
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
int	xp_offset[16] =
{
	P400, M400, P400, M400,
	P800, M800, P800, M800,
	P1200, M1200, P1200, M1200,
	0, 0, 0, 0,
};

struct	buf	xptab;
struct	buf	rxpbuf;
struct	buf	xputab[NXP];

#define	GO	01
#define	PRESET	020
#define	RTC	016
#define	OFFSET	014
#define SEEK	04
#ifdef	NO_SEARCH
#	define	SEARCH	SEEK
#else
#	define	SEARCH	030
#endif
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

xpstrategy(bp)
register struct buf *bp;
{
	register struct buf *dp;
	register unit;
	register pseudo_unit;
	register nm_sect_per_cyl;
	long sz, bn;
	struct	size	*sizes;

	unit = dkunit(bp);
	pseudo_unit = minor(bp->b_dev) & 077;
/*
 *	This next weirdness handled the look up into the Drive Type
 *	register to tell what type of disk we have here.
 *
 *	Note: No need to look up after the first time.
 */
	if (xp_type[unit] == 0) {	/* On the first time this fails */
		xp_type[unit] = XPADDR->xpdt & 077;
	}
	switch(	xp_type[unit] ) {

	case RM:
		sizes = rm_sizes;
		nm_sect_per_cyl = RM_SECT * RM_TRAC;
		break;

	case RM5:
	case RM5X:
		sizes = rm5_sizes;
		nm_sect_per_cyl = RM_SECT * HP_TRAC;
		break;

	case RP:
		sizes = hp_sizes;
		nm_sect_per_cyl = HP_SECT * HP_TRAC;
		break;

	case DV:
		sizes = dv_sizes;
		nm_sect_per_cyl = DV_SECT * DV_TRAC;
		break;

	default:
	/*
	 * I know this takes space, but it helps when your hardware does not work, ctc
	 */
		printf("xp: unknown device type 0%o\n", xp_type[unit]);
		u.u_error = ENXIO;	/* Have you got a better error? */
		unit = NXP + 1;		/* This forces the next test to error */
		
	}
	sz = bp->b_bcount;
	sz = (sz+511) >> 9;
	if (((unit >= NXP) || (bp->b_blkno < 0)) ||
	    ((bn = dkblock(bp))+sz > sizes[pseudo_unit&07].nblocks)) {
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return;
	}
/*
 * If we are on an 11/44 or any other machine other than an 11/70
 * that has a Unibus Map (11/40 or 45 class processors with the
 * Able Corp ENABLE Board) We had better allocate the Unibus Map.
 *
 * If you are runing disks on a 70 unibus you will have to
 * remove the test for cputype==70 here.
 */
	if ((bp->b_flags & B_PHYS) && (cputype != 70))
#ifdef	UNIBUS_MAP
			mapalloc(bp);
#endif
	bp->b_cylin = bn/nm_sect_cyl + sizes[pseudo_unit&07].cyloff;
	dp = &xputab[unit];
	spl5();
	disksort(dp, bp);
	if (dp->b_active == 0) {
		xpustart(unit);
		if(xptab.b_active == 0)
			xpstart();
	}
	spl0();
}

xpustart(unit)
register unit;
{
	register struct buf *bp, *dp;
	daddr_t bn;
	int sn, cn, csn;
	int nm_sect, nm_trac;

	XPADDR->xpcs2.w = unit;
	XPADDR->xpcs1.c[0] = IE;
	XPADDR->xpas = 1<<unit;

	if(unit >= NXP)
		return;
	dk_busy &= ~(1<<(unit+DK_N));
	dp = &xputab[unit];
	if((bp=dp->b_actf) == NULL)
		return;
	if((XPADDR->xpds & VV) == 0) {
		XPADDR->xpcs1.c[0] = IE|PRESET|GO;
		XPADDR->xpof = FMT22;
	}
	if(dp->b_active)
		goto done;
	dp->b_active++;
	if ((XPADDR->xpds & (DPR|MOL)) != (DPR|MOL))
		goto done;

	bn = dkblock(bp);
	cn = bp->b_cylin;
	switch(xp_type[unit]) {

	case RM:
		nm_sect = RM_SECT;
		nm_trac = RM_TRAC;
		break;

	case RM5:
	case RM5X:
		nm_sect = RM_SECT;
		nm_trac = HP_TRAC;
		break;

	case RP:
		nm_sect = HP_SECT;
		nm_trac = HP_TRAC;
		break;

	case DV:
		nm_sect = DV_SECT;
		nm_trac = DV_TRAC;
		break;

	default:
		panic("xpustart");
	}
	sn = bn%(nm_sect*nm_trac);
	sn = (sn+nm_sect-SDIST)%nm_sect;

	if(XPADDR->xpcc != cn)
		goto search;
#ifndef	NO_SEARCH
	csn = (XPADDR->xpla>>6) - sn + SDIST - 1;
	if(csn < 0)
		csn += nm_sect;
	if(csn > nm_sect-RDIST)
#endif
		goto done;

search:
	XPADDR->xpdc = cn;
	XPADDR->xpda = sn;
	XPADDR->xpcs1.c[0] = IE|SEARCH|GO;
	unit += DK_N;
	dk_busy |= 1<<unit;
	dk_numb[unit] += 1;
	return;

done:
	dp->b_forw = NULL;
	if(xptab.b_actf == NULL)
		xptab.b_actf = dp; else
		xptab.b_actl->b_forw = dp;
	xptab.b_actl = dp;
}

xpstart()
{
	register struct buf *bp, *dp;
	register unit;
	register pseudo_unit;
	daddr_t bn;
	int sn, tn, cn;
	int nm_sect_per_cyl, nm_sect;

loop:
	if ((dp = xptab.b_actf) == NULL)
		return;
	if ((bp = dp->b_actf) == NULL) {
		xptab.b_actf = dp->b_forw;
		goto loop;
	}
	xptab.b_active++;
	pseudo_unit = minor(bp->b_dev) & 077;
	unit = dkunit(bp);
	bn = dkblock(bp);
	switch(xp_type[unit]) {

	case RM:
		nm_sect_cyl = RM_SECT * RM_TRAC;
		nm_sect = RM_SECT;
		cn = rm_sizes[pseudo_unit&07].cyloff;
		break;

	case RM5:
	case RM5X:
		nm_sect_cyl = RM_SECT * HP_TRAC;
		nm_sect = RM_SECT;
		cn = rm5_sizes[pseudo_unit&07].cyloff;
		break;

	case RP:
		nm_sect_cyl = HP_SECT * HP_TRAC;
		nm_sect = HP_SECT;
		cn = hp_sizes[pseudo_unit&07].cyloff;
		break;

	case DV:
		nm_sect_cyl = DV_SECT * DV_TRAC;
		nm_sect = DV_SECT;
		cn = dv_sizes[pseudo_unit&07].cyloff;
		break;

	default:
		panic("xpstart");
	}
	cn += bn/nm_sect_cyl;
	sn = bn%nm_sect_cyl;
	tn = sn/nm_sect;
	sn = sn%nm_sect;

	XPADDR->xpcs2.w = unit;
	if ((XPADDR->xpds & (DPR|MOL)) != (DPR|MOL)) {
		xptab.b_active = 0;
		xptab.b_errcnt = 0;
		dp->b_actf = bp->av_forw;
		bp->b_flags |= B_ERROR;
		iodone(bp);
		goto loop;
	}
	if(xptab.b_errcnt >= 16) {
		XPADDR->xpof = xp_offset[xptab.b_errcnt & 017] | FMT22;
		XPADDR->xpcs1.w = OFFSET|GO;
		while((XPADDR->xpds & (PIP | DRY)) != DRY)
			;
	}
	XPADDR->xpdc = cn;
	XPADDR->xpda = (tn << 8) + sn;
	XPADDR->xpba = bp->b_un.b_addr;
	if(cputype == 70)
		XPADDR->xpbae = bp->b_xmem;
	XPADDR->xpwc = -(bp->b_bcount>>1);
/*
 * Warning, unit is being used as a temporary
 */
	unit = ((bp->b_xmem&3) << 8) | IE | GO;
	if(bp->b_flags & B_READ)
		unit |= RCOM; else
		unit |= WCOM;
	XPADDR->xpcs1.w = unit;

	dk_busy |= 1<<(DK_N+NXP);
	dk_numb[DK_N+NXP] += 1;
	unit = bp->b_bcount>>6;
	dk_wds[DK_N+NXP] += unit;
}

xpintr()
{
	register struct buf *bp, *dp;
	register unit;
	int as, i, j;

	as = XPADDR->xpas & 0377;
	if(xptab.b_active) {
		dk_busy &= ~(1<<(DK_N+NXP));
		dp = xptab.b_actf;
		bp = dp->b_actf;
		unit = dkunit(bp);
		XPADDR->xpcs2.c[0] = unit;
		if (XPADDR->xpcs1.w & TRE) {		/* error bit */
			while((XPADDR->xpds & DRY) == 0)
				;
			if(++xptab.b_errcnt > 28 || XPADDR->xper1&WLE)
				bp->b_flags |= B_ERROR; else
				xptab.b_active = 0;
			if(xptab.b_errcnt > 27) {
				deverror(bp, XPADDR->xpcs2.w, XPADDR->xper1);
				printf("%D ", bp->b_blkno);
				prdev("ECC", bp->b_dev);
			}
#ifdef MENLO_ECC
			if((XPADDR->xper1 & (DCK|ECH)) == DCK) 
				if (xpecc(XPADDR, bp, &xptab))
					return;
#endif
			XPADDR->xpcs1.w = TRE|IE|DCLR|GO;
			if((xptab.b_errcnt&07) == 4) {
				XPADDR->xpcs1.w = RECAL|IE|GO;
				while((XPADDR->xpds & (PIP | DRY)) != DRY)
					;
			}
		}
		if(xptab.b_active) {
			if(xptab.b_errcnt) {
				XPADDR->xpcs1.w = RTC|GO;
				while((XPADDR->xpds & (PIP | DRY)) != DRY)
					;
			}
			xptab.b_active = 0;
			xptab.b_errcnt = 0;
			xptab.b_actf = dp->b_forw;
			dp->b_active = 0;
			dp->b_errcnt = 0;
			dp->b_actf = bp->av_forw;
			bp->b_resid = -(XPADDR->xpwc<<1);
			iodone(bp);
			XPADDR->xpcs1.w = IE;
			if(dp->b_actf)
				xpustart(unit);
		}
		as &= ~(1<<unit);
	} else {
		if(as == 0)
			XPADDR->xpcs1.w = IE;
		XPADDR->xpcs1.c[1] = TRE>>8;
	}
	for(unit=0; unit<NXP; unit++)
		if(as & (1<<unit))
			xpustart(unit);
	xpstart();
}

xpread(dev)
{

	physio(xpstrategy, &rxpbuf, dev, B_READ);
}

xpwrite(dev)
{

	physio(xpstrategy, &rxpbuf, dev, B_WRITE);
}


# ifdef MENLO_ECC
/*!modified 29may81, D. Fong (EDS)*/
int
xpecc(rp,bp,dp)
    register struct device *rp;
    register struct buf *bp;
    struct buf *dp;
{
# define exadr(x,y) (((long)(x)<<16)|(unsigned)(y))
# define bitfld(x) ~(~0L<<(x))
    long bb,addr,wrong;
    int bit,byte,nok,npx,wc;
    int ocmd;
    int cn,tn,sn;
    long bn;
    ubadr_t *ubp;
    int nm_sect_cyl;
    int nm_sect;
    int unit;

    /*
     *nok is #bytes including the error
     *which is assumed to be in the last disk page xferred.
     */
    wc = rp->xpwc;
    nok = (wc+bp->b_bcount/NBPW)*NBPW;
    npx = nok/PGSIZE;
printf("%D ",bp->b_blkno+npx);
prdev("ECC",bp->b_dev);
    wrong = rp->xpec2&(int)bitfld(11);
    if( wrong==0 )
    {
	rp->xpof = FMT22;
	rp->xpcs1.w |= IE;
	return 0;
    }

    /*
     *compute the byte/bit position of the err
     *within the last disk page xferred.
     */
    byte = rp->xpec1-1;
    bit = byte&(int)bitfld(3);
    byte >>= 3;
    /*correct while possible bits remain of mask*/
    bb = exadr(bp->b_xmem,bp->b_un.b_addr);
    if( byte<PGSIZE
     && ((npx>0 && nok%PGSIZE==0) || wc==0) )
    {
	if( npx>0 )
	    byte += nok-PGSIZE;
	while( 0<=byte && byte<bp->b_bcount && wrong!=0 )
	{
	    addr = bb+byte;
	    if( bp->b_flags&B_PHYS && bp->b_flags&B_MAP )
	    {
		/*simulate ubmap if twas unibus xfer*/
		/*untested*/
printf("RAW XFER\n");
		ubp = UBMAP+((addr>>13)&(int)bitfld(5));
		addr = exadr(ubp->ub_hi,ubp->ub_lo)
		    +(addr&bitfld(13));
	    }
	    wrong <<= bit;
printf("MEMADR==%D\n",addr);
	    putmemc(addr,getmemc(addr)^wrong);
	    byte++;
	    bit = -8;
	}
    }

    dp->b_active++;
    if( wc==0 )
	return 0;

    /*have to continue the transfer*/
    /*cross fingers*/
printf("RESTARTING...\n");
    unit = dkunit(bp);
    switch(xp_type[unit]) {

    case RM:
	    nm_sect_cyl = RM_SECT * RM_TRAC;
	    nm_sect = RM_SECT;

    case RM5:
    case RM5X:
	    nm_sect_cyl = RM_SECT * HP_TRAC;
	    nm_sect = RM_SECT;
	    break;

    case RP:
	    nm_sect_cyl = HP_SECT * HP_TRAC;
	    nm_sect = HP_SECT;
	    break;

    case DV:
	    nm_sect_cyl = DV_SECT * DV_TRAC;
	    nm_sect = DV_SECT;
	    break;

    default:
	    panic("xpecc");
    }
    nok = npx*PGSIZE;
/*rp->xpcs1.w=DCLR|GO;*/
    ocmd = (rp->xpcs1.w&~DRY)|IE|GO;
    rp->xpcs2.w = unit;
    rp->xpcs1.w = TRE|DCLR|GO/*~IE*/;

    bn = dkblock(bp);
    cn = bp->b_cylin-bn/(nm_sect_cyl)/*.cyloff*/;
    bn += npx;
    addr = bb+nok;

    cn += bn/(nm_sect_cyl);
    sn = bn%(nm_sect_cyl);
    tn = sn/nm_sect;
    sn %= nm_sect;

    rp->xpdc = cn;
    rp->xpda = (tn<<8)+sn;
    rp->xpwc = nok/NBPW-bp->b_bcount/NBPW;
    rp->xpba = (int)addr;
    if( cputype==70 )
	rp->xpbae = (int)(addr>>16);
    rp->xpcs1.w = ocmd;
    return 1;
}
# endif


