/*
 *  RL disk driver
 */

#define DK_N	2
#include <sys/param.h>
#include <sys/buf.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/systm.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)rl.c	3.2";
#endif

#define	BLKRL1	10240		/* Number of UNIX blocks for an RL01 drive */
#define BLKRL2	20480		/* Number of UNIX blocks for an RL02 drive */
#define RLCYLSZ 10240		/* bytes per cylinder */
#define RLSECSZ 256		/* bytes per sector */

#define RESET 013
#define	RL02TYP	0200		/* drive type bit */
#define STAT 03
#define GETSTAT 04
#define WCOM 012
#define RCOM 014
#define SEEK 06
#define SEEKHI 5
#define SEEKLO 1
#define RDHDR 010
#define IENABLE 0100
#define CRDY 0200
#define OPI 02000
#define CRCERR 04000
#define TIMOUT 010000
#define NXM 020000
#define DE  040000

struct device
{
	int rlcs, rlba, rlda, rlmp;
};

#define RLADDR	((struct device *)0174400)
#define RL_CNT 1
struct	buf	rrlbuf;
struct	buf	rltab;

struct 
{
	int	cn[4];		/* location of heads for each drive */
	int	type[4] ;	/* parameter dependent upon drive type  (RL01/02) */
	int	dn;		/* drive number */
	int	com;		/* read or write command word */
	int	chn;		/* cylinder and head number */
	unsigned int	bleft;	/* bytes left to be transferred */
	unsigned int	bpart;	/* number of bytes transferred */
	int	sn;		/* sector number */
	union {
		int	w[2];
		long	l;
	} addr;			/* address of memory for transfer */

}	rl = {-1,-1,-1,-1, -1,-1,-1,-1} ;	/* initialize cn[] and type[] */

rlstrategy(bp)
register struct buf *bp;
{
	register struct device *rp ;
	int nblocks ;	/* number of UNIX blocks for the drive in question */
	int drive ;
	int ctr ;

	rp = RLADDR ;

#ifdef UNIBUS_MAP
	if(bp->b_flags&B_PHYS)
		mapalloc(bp);
#endif

	/*
	 * We must determine what type of drive we are talking to in order 
	 * to determine how many blocks are on the device.  The rl.type[]
	 * array has been initialized with -1's so that we may test first
	 * contact with a particular drive and do this determination only once.
	 *
	 * For some unknown reason the RL02 (seems to be
	 * only dirve 1) does not return a valid drive status
	 * the first time that a GET STATUS request is issued
	 * for the drive, in fact it can take up to three or more
	 * GET STATUS requests to obtain the correct status.
	 * In order to overcome this "HACK" the driver has been
	 * modified to issue a GET STATUS request, validate the
	 * drive status returned, and then use it to determine the
	 * drive type. If a valid status is not returned after eight
	 * attempts, then an error message is printed.
	 */
	if (rl.type[minor(bp->b_dev)] < 0)
		{
		drive = minor(bp->b_dev) ;
		ctr = 0;
		do {
		/* get status and reset when first touching this drive */
			rp->rlda = RESET ;
		rp->rlcs = (drive << 8) | GETSTAT ;	/* set up csr */
		while ((rp->rlcs & CRDY) == 0) ;	/* wait for it */
		} while (((rp->rlmp & 0177477) != 035) && (++ctr < 8)) ;
		if (ctr >= 8)
			printf("\nCan't get status of RL unit %d\n",drive) ;
		if (rp->rlmp & RL02TYP) 
			rl.type[drive] = BLKRL2 ;	/* drive is RL02 */
		else
			rl.type[drive] = BLKRL1 ;	/* drive RL01 */
		}
	/* determine nblocks based upon which drive this is */
	nblocks = rl.type[minor(bp->b_dev)] ;
	if(bp->b_blkno >= nblocks) {
		if(bp->b_blkno == nblocks && bp->b_flags&B_READ)
			bp->b_resid = bp->b_bcount;
		else {
			bp->b_flags |= B_ERROR;
			bp->b_error = ENXIO;
		}
		iodone(bp);
		return;
	}
	bp->av_forw = NULL;
	spl5();
	if(rltab.b_actf == NULL)
		rltab.b_actf = bp;
	else
		rltab.b_actl->av_forw = bp;
	rltab.b_actl = bp;
	if(rltab.b_active == NULL)
		rlstart();
	spl0();
}

rlstart()
{

	register struct buf *bp;

	if ((bp = rltab.b_actf) == NULL)
		return;
	rltab.b_active++;
	rl.dn = minor(bp->b_dev);
	rl.chn = bp->b_blkno/20;
	rl.sn = (bp->b_blkno%20) << 1;
	rl.bleft = bp->b_bcount;
	rl.addr.w[0] = bp->b_xmem & 3;
	rl.addr.w[1] = (int)bp->b_un.b_addr;
	rl.com = (rl.dn << 8) | IENABLE;
	if (bp->b_flags & B_READ)
		rl.com |= RCOM;
	else
		rl.com |= WCOM;
	rlio();
}

rlintr()
{
	register struct buf *bp;
	register struct device *rp;
	register int status;

	rp = RLADDR;
	if (rltab.b_active == NULL) {
		return;
	}
	bp = rltab.b_actf;
	dk_busy &= ~(1<<DK_N);
	if (rp->rlcs < 0) {		/* error bit */
		if (rp->rlcs & 036000) {
			if(rltab.b_errcnt > 2)
			{
				printf("rlcs, rlda  ") ;
				deverror(bp, rp->rlcs, rp->rlda);
			}
		}
		if (rp->rlcs & 040000) {
			rp->rlda = STAT;
			rp->rlcs = (rl.dn << 8) | GETSTAT;
			while ((rp->rlcs & CRDY) == 0)
				;
			status = rp->rlmp;
			if(rltab.b_errcnt > 2)
			{
				printf("rlmp, rlda  ") ;
				deverror(bp, status, rp->rlda);
			}
			rp->rlda = RESET;
			rp->rlcs = (rl.dn << 8) | GETSTAT;
			while ((rp->rlcs & CRDY) == 0)
				;
			if(status & 01000) {
				rlstart();
				return;
			}
		}
		if (++rltab.b_errcnt <= 10) {
			rl.cn[rl.dn] = -1;
			rlstart();
			return;
		}
		else {
			bp->b_flags |= B_ERROR;
			rl.bpart = rl.bleft;
		}
	}

	if ((rl.bleft -= rl.bpart) > 0) {
		rl.addr.l += rl.bpart;
		rl.sn=0;
		rl.chn++;
		rlio();
		return;
	}
	rltab.b_active = NULL;
	rltab.b_errcnt = 0;
	rltab.b_actf = bp->av_forw;
	bp->b_resid = 0;
	iodone(bp);
	rlstart();
}

rlio()
{

	register struct device *rp;
	register dif;
	register int head;

	rp = RLADDR;
	dk_busy |= 1<<DK_N;
	dk_numb[DK_N] += 1;
	head = rl.bpart>>6;
	dk_wds[DK_N] += head;
	/*
	 * When the device is first touched, find where the heads are and
	 * what type of drive it is.
	 * rl.cn[] is initialized as -1 and this indicates that we have not 
	 * touched this drive before.
	 */
	if (rl.cn[rl.dn] < 0) {
		/* find where the heads are */
		rp->rlcs = (rl.dn << 8) | RDHDR;
		while ((rp->rlcs&CRDY) == 0)
			;
		rl.cn[rl.dn] = (rp->rlmp&0177700) >> 6;
	}
	dif =(rl.cn[rl.dn] >> 1) - (rl.chn >>1);
	head = (rl.chn & 1) << 4;
	if (dif < 0)
		rp->rlda = (-dif <<7) | SEEKHI | head;
	else
		rp->rlda = (dif << 7) | SEEKLO | head;
	rp->rlcs = (rl.dn << 8) | SEEK;
	rl.cn[rl.dn] = rl.chn;
	if (rl.bleft < (rl.bpart = RLCYLSZ - (rl.sn * RLSECSZ)))
		rl.bpart = rl.bleft;
	while ((rp->rlcs&CRDY) == 0)
		;
	rp->rlda = (rl.chn << 6) | rl.sn;
	rp->rlba = rl.addr.w[1];
	rp->rlmp = -(rl.bpart >> 1);
	rp->rlcs = rl.com | rl.addr.w[0] << 4;
}

rlread(dev)
{

	physio(rlstrategy, &rrlbuf, dev, B_READ);
}

rlwrite(dev)
{

	physio(rlstrategy, &rrlbuf, dev, B_WRITE);
}
