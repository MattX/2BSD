/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ra.c	2.5 (2.11BSD GTE) 1/11/95
 */

 /***********************************************************************
 *			Copyright (c) 1983 by				*
 *		Digital Equipment Corporation, Maynard, MA		*
 *			All rights reserved.				*
 ***********************************************************************/

/* 
 * ra.c - MSCP Driver
 * Date:	Jan 11, 1995
 * Remove extra parameter to ra_error() call in radump().
 *
 * Date:	Dec 1992, Jan 1993
 * Add the partition size routine.  Remove unibus map ifdefs, the run time
 * check for 'ubmap' is sufficient and does the right thing.
 *
 * Date:	Nov 1992
 * Add raVec() routine.  This is called by autoconfig to set the vector
 * (from /etc/dtab) for controllers other than the root (1st).  The boot/root
 * controller's vector is always set to 0154.
 *
 * Date:	Jul  1992
 * Major rework of the partition tables.  Some old (RA60,80,81,RD53) tables 
 * were retained (the rd54 'c' partition is now 'g' but the sizes stay 
 * the same) for compatibility.  RC25, RD51, RA82 entries were removed (unlikely
 * that the RA82 was used since the disktab entry was wrong).  A _brand new_
 * scheme utilizing 5 "generic" partition tables was created based on the
 * size of the disc.  This was needed due to the rapid proliferation of
 * MSCP drive types, it was simply not feasible to have a 64 byte partition
 * table for each of the (currently 28 to 30) types of MSCP discs.
 *
 * More attention is paid to bits in the media id beyond the 7 bit 
 * numeric id.  These bits can be used to distinquish between a RZ24 and a
 * RZ24L for example.
 *
 * Some of the diagnostic output was trimmed in an attempt to offset the
 * growth of an already large drive.
 *
 * Date:	Dec  18 1991
 * The controller number (bits 6 and 7 of the minor device) were not
 * being masked off after using dkunit().  This caused a crash when 
 * the controller number was other than 0.
 *
 * Date:	Sep  22 1991
 * The read and write entries were removed as part of implementing the 
 * common rawread and rawwrite routines.
 * 
 * Date:	Mar  16 1991
 * The command packets were moved to an external heap which is dynamically
 * allocated in a manner similar to m_ioget in the networking code.
 * MSCP controllers used too much valuable kernel D space.  For UNIBUS
 * machines sufficient UMRs were allocated to map the heap, removing the
 * need to allocate a UMR per controller at open time.  This has the side
 * effect of greatly simplifying the physical (Q22) or virtual (UMR) address 
 * calculation of the command packets.  It also eliminates the need for
 * 'struct buf racomphys', saving 24 bytes of D space.
 *
 * The error message output was rearranged saving another 82 bytes of
 * kernel D space. Also, there was an extraneous buffer header allocated,
 * it was removed, saving a further 24 bytes of D space.
 * sms@wlv.imsd.contel.com
 *
 * Date:        Jan  30 1984
 * This thing has been beaten beyound belief.
 * decvax!rich.
 */

/*
 * MSCP disk device driver
 * Tim Tucker, Gould Electronics, Sep 1985
 * Note:  This driver is based on the UDA50 4.3 BSD source.
 */

#include "ra.h"
#if 	NRAD > 0  &&  NRAC > 0
#include "param.h"
#include "../machine/seg.h"
#include "../machine/mscp.h"

#include "systm.h"
#include "buf.h"
#include "conf.h"
#include "map.h"
#include "syslog.h"
#include "uba.h"
#include "rareg.h"
#include "dk.h"
#include "errno.h"

#define	RACON(x)			((minor(x) >> 6) & 03)
#define	RAUNIT(x)			((minor(x) >> 3) & 07)

struct	rasizes	{
	daddr_t nblocks;
	daddr_t blkoff;
} rd52_sizes[8] = { 		/* Setup for RD52 and RD53 disks */
	9700,	0,		/* A=blk 0 thru 9699 (root for 52 & 53) */
	17300,	9700,		/* B=blk 9700 thru 26999 (52 & 53 only) */
	3100,	27000,		/* C=blk 27000 thru 30099 (swap 52 & 53) */
	-1,	30100,		/* D=blk 30100 thru end (52 & 53 only) */
	0,	0,		/* E=unused */
	0,	0,		/* F=unused */
	-1,	9700,		/* G=blk 9700 thru end (52 & 53) */
	-1,	0,		/* H=blk 0 thru end (52, 53) */
}, ra60_sizes[8] = {
	15884,	0,		/* A=blk 0 thru 15883 */
	33440,	15884,		/* B=blk 15884 thru 49323 */
	193282,	49324,		/* C=blk 49324 thru 242605 */
	15884,	242606,		/* D=blk 242606 thru 258489 */
	-1,	258490,		/* E=blk 258490 thru end */
	0,	0,		/* F=unused */
	-1,	242606,		/* G=blk 242606 thru end */
	-1,	0,		/* H=blk 0 thru end */
}, ra80_sizes[8] = {
	15884,	0,		/* A=blk 0 thru 15883 */
	33440,	15884,		/* B=blk 15884 thru 49323 */
	-1,	49324,		/* C=blk 49324 thru end */
	0,	0,		/* D=unused */
	0,	0,		/* E=unused */
	0,	0,		/* F=unused */
	0,	0,		/* G=unused */
	-1,	0,		/* H=blk 0 thru end */
}, ra81_sizes[8] ={
	15884,	0,		/* A=blk 0 thru 15883 */
	33440,	15884,		/* B=blk 15884 thru 49323 */
	193282,	49324,		/* C=blk 49324 thru 242605 */
	15884,	242606,		/* D=blk 242606 thru 258489 */
	307200,	258490,		/* E=blk 258490 thru 565689 */
	-1,	565690,		/* F=blk 565690 thru end */
	-1,	242606,		/* G=blk 242606 thru end */
	-1,	0,		/* H=blk 0 thru end */
}, ra_gen1[8] ={
	15884,	0,		/* A=blk 0 thru 15883 */
	8360,	15884,		/* B=blk 15884 thru 24243 */
	-1,	0,		/* C=blk 0 thru end */
	0,	0,		/* D=unused */
	0,	0,		/* E=unused */
	0,	0,		/* F=unused */
	-1,	24244,		/* G=blk 24244 thru end */
	0,	0,		/* H=unused */
}, ra_gen2[8] ={
	15884,	0,		/* A=blk 0 thru 15883 */
	16720,	15884,		/* B=blk 15884 thru 32603 */
	-1,	0,		/* C=blk 0 thru end */
	227136,	32604,		/* D=blk 32604 thru 259739 */
	-1,	259740,		/* E=blk 259740 thru end */
	0,	0,		/* F=unused */
	-1,	32604,		/* G=32604 thru end */
	0,	0,		/* H=unused */
}, ra_gen3[8] ={
	-1,	0,		/* A=blk 0 thru end */
	0,	0,		/* B=unused */
	-1,	0,		/* C=blk 0 thru end */
	0,	0,		/* D=unused */
	0,	0,		/* E=unused */
	0,	0,		/* F=unused */
	0,	0,		/* G=unused */
	0,	0,		/* H=unused */
}, ra_gen4[8] ={
	15884,	0,		/* A=blk 0 thru 15883 */
	16720,	15884,		/* B=blk 15884 thru 32603 */
	-1,	0,		/* C=blk 0 thru end */
	227136,	32604,		/* D=blk 32604 thru 259739 */
	409600,	259740,		/* E=blk 259740 thru 669339 */
	-1,	669340,		/* F=blk 669340 thru end */
	-1,	32604,		/* G=blk 32604 thru end */
	0,	0,		/* H=unused */
}, ra_gen5[8] ={
	15884,	0,		/* A=blk 0 thru 15883 */
	16720,	15884,		/* B=blk 15884 thru 32603 */
	-1,	0,		/* C=blk 0 thru end */
	227136,	32604,		/* D=blk 32604 thru 259739 */
	409600,	259740,		/* E=blk 259740 thru 669339 */
	182464,	669340,		/* F=blk 669340 thru 851803 */
	819200,	32604,		/* G=blk 32604 thru 851803 */
	-1,	851804,		/* H=blk 851804 thru end */
};

#define M15(a,b,c) ((((((a - '@') & 0x1f) << 10) | \
                      ((b - '@') & 0x1f) <<  5) | \
                      ((c - '@') & 0x1f) <<  0))

/*
 * Entries are only placed in this table to over-ride the drive size
 * based selection of partition tables in radisksetup().  For example:
 * the RA81 is listed here to keep the "old" partition table rather than
 * assigning 'ra_gen5' (because the RA81 is > 851804 sectors.
*/

	struct MEDIA
		{
		short	id7;
		short	name15;
		struct	rasizes *parts;
		} Mscptbl[] = {
	{ 81, M15('R','A','@'), ra81_sizes },
	{ 60, M15('R','A','@'), ra60_sizes },
	{ 80, M15('R','A','@'), ra80_sizes },
	{ 53, M15('R','D','@'), rd52_sizes },
	{ 52, M15('R','D','@'), rd52_sizes },
	{ 0, 0, 0 }
	};

#define	NRSPL2	3		/* log2 number of response packets */
#define	NCMDL2	3		/* log2 number of command packets */
#define	NRSP	(1<<NRSPL2)
#define	NCMD	(1<<NCMDL2)

typedef	struct	{		/* Swap shorts for MSCP controller! */
	short	lsh;
	short	hsh;
} Trl;

/*
 * RA Communications Area
 */
struct  raca {
	short	ca_xxx1;	/* unused */
	char	ca_xxx2;	/* unused */
	char	ca_bdp;		/* BDP to purge */
	short	ca_cmdint;	/* command queue transition interrupt flag */
	short	ca_rspint;	/* response queue transition interrupt flag */
	Trl	ca_rsp[NRSP];	/* response descriptors */
	Trl	ca_cmd[NCMD];	/* command descriptors */
};

#define	RINGBASE	(4 * sizeof(short))

#define	RA_OWN	0x8000	/* Controller owns this descriptor */
#define	RA_INT	0x4000	/* allow interrupt on ring transition */

typedef	struct {
	struct raca	ra_ca;		/* communications area */
	struct mscp	ra_rsp[NRSP];	/* response packets */
	struct mscp	ra_cmd[NCMD];	/* command packets */
} ra_comT;				/* 1096 bytes per controller */

typedef	struct	ra_info	{
	struct  rasizes	*ra_size;	/* Partion tables for drive */
	daddr_t		ra_dsize;	/* Max user size from online pkt */
	short		ra_unit;	/* controller unit # */
	struct	buf	ra_dtab;	/* I/O disk drive queues */
} ra_infoT;

typedef	struct	{
	radeviceT 	*RAADDR;	/* Controller bus address */
	short		sc_unit;	/* attach unit # */
	short		sc_state;	/* state of controller */
	short		sc_ivec;	/* interrupt vector address */
	short		sc_credits;	/* transfer credits */
	short		sc_lastcmd;	/* pointer into command ring */
	short		sc_lastrsp;	/* pointer into response ring */
	struct	buf	sc_ctab;	/* Controller queue */
	struct	buf	sc_wtab;	/* I/O wait queue, for controller */
	short		sc_cp_wait;	/* Command packet wait flag */
	ra_comT		*sc_com;	/* Communications area pointer */
	ra_infoT	*sc_drives[8];	/* Disk drive info blocks */
} ra_softcT;

ra_softcT		ra_sc[NRAC];	/* Controller table */
memaddr			ra_com[NRAC];	/* Communications area table */
ra_infoT		ra_disks[NRAD];	/* Disk table */

#define	MAPSEGDESC	(((btoc(sizeof (ra_comT))-1)<<8)|RW)

#ifdef UCB_METER
static	int		ra_dkn = -1;	/* number for iostat */
#endif

/*
 * Controller states
 */
#define	S_IDLE	0		/* hasn't been initialized */
#define	S_STEP1	1		/* doing step 1 init */
#define	S_STEP2	2		/* doing step 2 init */
#define	S_STEP3	3		/* doing step 3 init */
#define	S_SCHAR	4		/* doing "set controller characteristics" */
#define	S_RUN	5		/* running */

#ifdef RADEBUG
#define PRINTD(x)	printf x 
#else
#define PRINTD(x)
#endif

#ifdef RABUGDUMP
#define PRINTB(x)	printf x 
#else
#define PRINTB(x)
#endif

int	wakeup();
extern	ubadr_t	_iomap();
struct	mscp 	*ragetcp();

#define	b_qsize	b_resid		/* queue size per drive, in rqdtab */

/*
 * Setup root MSCP device (use bootcsr passed from ROMs).  In the event
 * the system was not booted from a MSCP drive but swapdev is a MSCP drive
 * we fake the old behaviour of attaching the first (172150) controller.  If
 * the system was booted from a MSCP drive then this routine has already been
 * called with the CSR of the booting controller and the attach routine will
 * ignore further calls to attach controller 0.
 *
 * This whole thing is a hack and should go away somehow.
 */
raroot(csr)
	register radeviceT *csr;
{
	if (!csr)		/* XXX */
		csr = (radeviceT *) 0172150;	/* XXX */
	raattach(csr, 0);
	raVec(0, 0154);
}

/*
 * Called from autoconfig and raroot() to set the vector for a controller.
 * It is an error to attempt to set the vector more than once except for
 * the first controller which may have had the vector set from raroot().
 * In this case the error is ignored and the vector left unchanged.
*/

raVec(ctlr, vector)
	register int	ctlr;
	int	vector;
	{
	register ra_softcT *sc = &ra_sc[ctlr];

	if	(ctlr >= NRAC)
		return(-1);
	if	(sc->sc_ivec == 0)
		sc->sc_ivec = vector;
	else if	(ctlr)
		return(-1);
	return(0);
	}
	
/*
 * Attach controller for autoconfig system.
 */
raattach(addr, unit)
	register radeviceT *addr;
	register int	unit;
{
	register ra_softcT *sc = &ra_sc[unit];

#ifdef UCB_METER
	if (ra_dkn < 0)
		dk_alloc(&ra_dkn, NRAD, "ra", 60L * 31L * 256L);
#endif

	/* Check for bad address (no such controller) */
	if (sc->RAADDR == NULL && addr != NULL) {
		sc->RAADDR = addr;
		sc->sc_unit = unit;
		sc->sc_com = (ra_comT *)SEG5;
		ra_com[unit] = (memaddr)_ioget(sizeof (ra_comT));
		return(1);
	}

	/*
	 * Binit and autoconfig both attempt to attach unit zero if ra is
	 * rootdev
	 */
	return(unit ? 0 : 1);
}

/*
 * Return a pointer to a free disk table entry
 */
ra_infoT *
ragetdd()
{
	register	int		i;
	register	ra_infoT	*p;

	for (i = NRAD, p = ra_disks; i--; p++)
		if (p->ra_dsize == 0L)
			return(p);
	return(NULL);
}

/*
 * Open a RA.  Initialize the device and set the unit online.
 */
raopen(dev, flag)
	dev_t 	dev;
	int 	flag;
{
	register ra_infoT *disk;
	register struct	mscp *mp;
	register ra_softcT *sc = &ra_sc[RACON(dev)];
	int unit = RAUNIT(dev);
	int s, i;

	PRINTD(("raopen: dev=%x, flags=%d\n", dev, flag));

	/* Check that controller exists */
	if (RACON(dev) >= NRAC || sc->RAADDR == NULL)
		return(ENXIO);

	/* Open device */
	if (sc->sc_state != S_RUN) {
		s = splbio();

		/* initialize controller if idle */
		if (sc->sc_state == S_IDLE) {
			if (rainit(sc)) {
				splx(s);
				return(ENXIO);
			}
		}

		/* wait for initialization to complete */
		timeout(wakeup, (caddr_t)&sc->sc_ctab, 12 * LINEHZ);
		sleep((caddr_t)&sc->sc_ctab, PSWP+1);
		if (sc->sc_state != S_RUN) {
			splx(s);
			return(EIO);
		}
		splx(s);
	}

	/*
	 * Check to see if the device is really there.  This code was
	 * taken from Fred Canters 11 driver.
	 */
	if ((disk = sc->sc_drives[unit]) == NULL) {
		PRINTD(("raopen: opening new disk %d\n", unit));
		s = splbio();
		/* Allocate disk table entry for disk */
		if ((disk = ragetdd()) != NULL) {
			sc->sc_drives[unit] = disk;
			disk->ra_unit = unit;
			disk->ra_dsize = -1L;
		} else {
			printf("ra%d: no disk structures\n", unit);
			splx(s);
			return(ENXIO);
		}
	}
	/* Try to online disk unit, it might have gone offline */
	if (disk->ra_dsize == -1L) {
	/* In high kernel, don't saveseg5, just use normalseg5 later on. */
		while ((mp = ragetcp(sc)) == 0) {
			++sc->sc_cp_wait;
			sleep(&sc->sc_cp_wait, PSWP+1);
			--sc->sc_cp_wait;
		}
		mapseg5(ra_com[sc->sc_unit], MAPSEGDESC);
		mp->m_opcode = M_O_ONLIN;
		mp->m_unit = unit;
		mp->m_cmdref = (unsigned)&disk->ra_dsize;
		((Trl *)mp->m_dscptr)->hsh |= RA_OWN|RA_INT;
		normalseg5();
		i = sc->RAADDR->raip;
		timeout(wakeup, (caddr_t)&disk->ra_dsize, 10 * LINEHZ);
		sleep((caddr_t)&disk->ra_dsize, PSWP+1);
		splx(s);
	}

	/* Did it go online? */
	if (disk->ra_dsize == -1L) {
		PRINTD(("raopen: disk didn't go online\n"));
		s = splbio();
		disk->ra_dsize = 0L;
		sc->sc_drives[unit] = NULL;
		splx(s);
		return(ENXIO);
	}
	PRINTD(("raopen: disk online\n"));
	return(0);
}

/*
 * Initialize controller, data structures, and start hardware
 * initialization sequence.
 */
rainit(sc)
	register ra_softcT *sc;
{
	long	adr;

	/*
	 * Cold init of controller
	 */
	++sc->sc_ctab.b_active;
	PRINTD(("rainit: unit=%d, vec=%o\n", sc->sc_unit, sc->sc_ivec));

	/*
	 * Get physical address of RINGBASE
	 */

	adr = _iomap(ra_com[sc->sc_unit]) + RINGBASE;

	/*
	 * Get individual controller RINGBASE physical address 
	 */
	sc->sc_ctab.b_un.b_addr = (caddr_t)loint(adr);
	sc->sc_ctab.b_xmem = hiint(adr);
	PRINTD(("rainit: com area addr low=0%o, high=0%o\n",
		sc->sc_ctab.b_un.b_addr, sc->sc_ctab.b_xmem));

	/*
	 * Start the hardware initialization sequence.
	 */
	sc->RAADDR->raip = 0;
	while ((sc->RAADDR->rasa & RA_STEP1) == 0)
		if (sc->RAADDR->rasa & RA_ERR)
			return(1);
	sc->RAADDR->rasa = RA_ERR | (NCMDL2 << 11) | (NRSPL2 << 8) | RA_IE
			| (sc->sc_ivec / 4);

	/*
	 * Initialization continues in interrupt routine.
	 */
	sc->sc_state = S_STEP1;
	sc->sc_credits = 0;
	return(0);
}

rastrategy(bp)
	register struct	buf *bp;
{
	register ra_infoT *disk;
	register struct buf *dp;
	ra_softcT *sc = &ra_sc[RACON(bp->b_dev)];
	int part = minor(bp->b_dev) & 07;
	daddr_t sz, maxsz;
	int s;

	/* Is disk online */
	if ((disk = sc->sc_drives[dkunit(bp) & 7]) == NULL || disk->ra_dsize <= 0L)
		goto bad;

	/* Valid block in device partition */
	sz = (bp->b_bcount + 511) >> 9;
	if ((maxsz = disk->ra_size[part].nblocks) < 0)
		maxsz = disk->ra_dsize - disk->ra_size[part].blkoff;
	if (bp->b_blkno < 0 || bp->b_blkno + sz > maxsz
	    || disk->ra_size[part].blkoff >= disk->ra_dsize)
		goto bad;

	/* Unibus Map buffer if required */
	mapalloc(bp);

	/*
	 * Link the buffer onto the drive queue
	 */
	s = splbio();
	dp = &disk->ra_dtab;
	if (dp->b_actf == 0)
		dp->b_actf = bp;
	else
		dp->b_actl->av_forw = bp;
	dp->b_actl = bp;
	bp->av_forw = 0;

	/*
	 * Link the drive onto the controller queue
	 */
	if (dp->b_active == 0) {
		dp->b_forw = NULL;
		if (sc->sc_ctab.b_actf == NULL)
			sc->sc_ctab.b_actf = dp;
		else
			sc->sc_ctab.b_actl->b_forw = dp;
		sc->sc_ctab.b_actl = dp;
		dp->b_active = 1;
	}

	/*
	 * Start controller if idle.
	 */
	if (sc->sc_ctab.b_active == 0) {
		rastart(sc);
	}
	splx(s);
	return;
bad:
	bp->b_flags |= B_ERROR;
	iodone(bp);
	return;
}

/* Start i/o, must be called at level splbio */
rastart(sc)
	register ra_softcT *sc;
{
	register struct mscp *mp;
	register struct buf *bp;
	struct buf *dp;
	ra_infoT *disk;
	int i;
	long	temp;
	segm	seg5;

	saveseg5(seg5);		/* save it just once */
loop:
	/* 
	 * Anything left to do on this controller?
	 */
	if ((dp = sc->sc_ctab.b_actf) == NULL) {
		sc->sc_ctab.b_active = 0;

		/*
		 * Check for response ring transitions lost in race
		 * condition
		 */
		mapseg5(ra_com[sc->sc_unit], MAPSEGDESC);
		rarspring(sc);
		restorseg5(seg5);
		return(0);
	}

	/* Get first request waiting on queue */
	if ((bp = dp->b_actf) == NULL) {
		/*
		 * No more requests for this drive, remove
		 * from controller queue and look at next drive.
		 * We know we're at the head of the controller queue.
		 */
		dp->b_active = 0;
		sc->sc_ctab.b_actf = dp->b_forw;
		goto loop;
	}

	++sc->sc_ctab.b_active;
	if (sc->RAADDR->rasa & RA_ERR || sc->sc_state != S_RUN) {
		harderr(bp, "ra");
		log(LOG_INFO, "rasa %o state %d\n", sc->RAADDR->rasa,
			sc->sc_state);
		/* Should requeue outstanding requests somehow */
		rainit(sc);
out:
		restorseg5(seg5);
		return(0);
	}

	/* Issue command */
	mapseg5(ra_com[sc->sc_unit], MAPSEGDESC);
	if ((mp = ragetcp(sc)) == NULL)
		goto out;
	mp->m_cmdref = (unsigned)bp;	/* pointer to get back */
	mp->m_opcode = bp->b_flags & B_READ ? M_O_READ : M_O_WRITE;
	mp->m_unit = dkunit(bp) & 7;
	disk = sc->sc_drives[mp->m_unit];
	temp = bp->b_blkno + disk->ra_size[minor(bp->b_dev) & 7].blkoff;
	mp->m_lbn_l = loint(temp);
	mp->m_lbn_h = hiint(temp);
	mp->m_bytecnt = bp->b_bcount;
	mp->m_buf_l = (u_short)bp->b_un.b_addr;
	mp->m_buf_h = bp->b_xmem;
	PRINTD(("ra: unit=%d op=0%o lbn=%d,%d len=%d buf=0%o,0%o\n",
		mp->m_unit, mp->m_opcode, mp->m_lbn_h, mp->m_lbn_l,
		mp->m_bytecnt, mp->m_buf_h, mp->m_buf_l));
	((Trl *)mp->m_dscptr)->hsh |= RA_OWN|RA_INT;
	if (sc->RAADDR->rasa & RA_ERR)
		printf("ra: Err %d\n", sc->RAADDR->rasa);
	i = sc->RAADDR->raip;		/* initiate polling */

#ifdef UCB_METER
	if (ra_dkn >= 0) {
		int dkn = ra_dkn + mp->m_unit;

		/* Messy, should do something better than this.  Ideas? */
		++dp->b_qsize;
		dk_busy |= 1<<dkn;
		dk_xfer[dkn]++;
		dk_wds[dkn] += bp->b_bcount>>6;
	}
#endif

	/*
	 * Move drive to the end of the controller queue
	 */
	if (dp->b_forw != NULL) {
		sc->sc_ctab.b_actf = dp->b_forw;
		sc->sc_ctab.b_actl->b_forw = dp;
		sc->sc_ctab.b_actl = dp;
		dp->b_forw = NULL;
	}

	/*
	 * Move buffer to I/O wait queue
	 */
	dp->b_actf = bp->av_forw;
	dp = &sc->sc_wtab;
	bp->av_forw = dp;
	bp->av_back = dp->av_back;
	dp->av_back->av_forw = bp;
	dp->av_back = bp;
	goto loop;
}

/*
 * RA interrupt routine.
 */
raintr(unit)
	int				unit;
{
	register ra_softcT *sc = &ra_sc[unit];
	register struct	mscp *mp;
	register struct	buf *bp;
	u_int i;
	segm seg5;

	PRINTD(("raintr%d: state %d, rasa %o\n", unit, sc->sc_state,
		sc->RAADDR->rasa));

	saveseg5(seg5);		/* save it just once */

	switch (sc->sc_state) {
	case S_STEP1:
#define	STEP1MASK	0174377
#define	STEP1GOOD	(RA_STEP2|RA_IE|(NCMDL2<<3)|NRSPL2)
		if ((sc->RAADDR->rasa & STEP1MASK) != STEP1GOOD) {
			sc->sc_state = S_IDLE;
			sc->sc_ctab.b_active = 0;
			wakeup((caddr_t)&sc->sc_ctab);
			return;
		}
		sc->RAADDR->rasa = (short)sc->sc_ctab.b_un.b_addr;
		sc->sc_state = S_STEP2;
		return;

	case S_STEP2:
#define	STEP2MASK	0174377
#define	STEP2GOOD	(RA_STEP3|RA_IE|(sc->sc_ivec/4))
		if ((sc->RAADDR->rasa & STEP2MASK) != STEP2GOOD) {
			sc->sc_state = S_IDLE;
			sc->sc_ctab.b_active = 0;
			wakeup((caddr_t)&sc->sc_ctab);
			return;
		}
		sc->RAADDR->rasa = sc->sc_ctab.b_xmem;
		sc->sc_state = S_STEP3;
		return;

	case S_STEP3:
#define	STEP3MASK	0174000
#define	STEP3GOOD	RA_STEP4
		if ((sc->RAADDR->rasa & STEP3MASK) != STEP3GOOD) {
			sc->sc_state = S_IDLE;
			sc->sc_ctab.b_active = 0;
			wakeup((caddr_t)&sc->sc_ctab);
			return;
		}
		i = sc->RAADDR->rasa;
		PRINTD(("ra: Version %d model %d\n",
				i & 0xf, (i >> 4) & 0xf));
		sc->RAADDR->rasa = RA_GO;
		sc->sc_state = S_SCHAR;

		/*
		 * Initialize the data structures.
		 */
		mapseg5(ra_com[sc->sc_unit], MAPSEGDESC);
		ramsginit(sc, sc->sc_com->ra_ca.ca_rsp, sc->sc_com->ra_rsp,
			0, NRSP, RA_INT | RA_OWN);
		ramsginit(sc, sc->sc_com->ra_ca.ca_cmd, sc->sc_com->ra_cmd,
			NRSP, NCMD, RA_INT);
		bp = &sc->sc_wtab;
		bp->av_forw = bp->av_back = bp;
		sc->sc_lastcmd = 1;
		sc->sc_lastrsp = 0;
		mp = sc->sc_com->ra_cmd;
		ramsgclear(mp);
		mp->m_opcode = M_O_STCON;
		mp->m_cntflgs = M_C_ATTN | M_C_MISC | M_C_THIS;	
		((Trl *)mp->m_dscptr)->hsh |= RA_OWN|RA_INT;
		i = sc->RAADDR->raip;
		restorseg5(seg5);
		return;

	case S_SCHAR:
	case S_RUN:
		break;

	default:
		printf("ra: state %d ignored\n", sc->sc_state);
		return;
	}

	/*
	 * If this happens we are in BIG trouble!
	 */
	if (sc->RAADDR->rasa & RA_ERR) {
		printf("ra: fatal err %o\n", sc->RAADDR->rasa);
		sc->sc_state = S_IDLE;
		sc->sc_ctab.b_active = 0;
		wakeup((caddr_t)&sc->sc_ctab);
	}

	mapseg5(ra_com[sc->sc_unit], MAPSEGDESC);

	/*
	 * Check for buffer purge request
	 */
	if (sc->sc_com->ra_ca.ca_bdp) {
		PRINTD(("ra%d: buffer purge request\n", sc->sc_unit));
		sc->sc_com->ra_ca.ca_bdp = 0;
		sc->RAADDR->rasa = 0;
	}

	/*
	 * Check for response ring transition.
	 */
	if (sc->sc_com->ra_ca.ca_rspint) {
		rarspring(sc);
	}

	/*
	 * Check for command ring transition (Should never happen!)
	 */
	if (sc->sc_com->ra_ca.ca_cmdint) {
		PRINTD(("ra: command ring transition\n"));
		sc->sc_com->ra_ca.ca_cmdint = 0;
	}

	restorseg5(seg5);

	/* Waiting for command? */
	if (sc->sc_cp_wait)
		wakeup((caddr_t)&sc->sc_cp_wait);
	rastart(sc);
}

/*
 * Init mscp communications area
 */
ramsginit(sc, com, msgs, offset, length, flags)
	register ra_softcT	*sc;
	register Trl		*com;
	register struct mscp	*msgs;
	int offset, length, flags;
{
	long	vaddr;

	/* 
	 * Figure out Unibus or physical(Q22) address of message 
	 * skip comm area and mscp messages header and previous messages
	 */
	vaddr = _iomap(ra_com[sc->sc_unit]);		/* base adr in heap */
	vaddr += sizeof(struct raca)			/* skip comm area */
		+sizeof(struct mscp_header);		/* m_cmdref disp */
	vaddr += offset * sizeof(struct mscp);		/* skip previous */
	while (length--) {
		com->lsh = loint(vaddr);
		com->hsh = flags | hiint(vaddr);
		msgs->m_dscptr = (long *)com;
		msgs->m_header.ra_msglen = sizeof(struct mscp);
		++com; ++msgs; vaddr += sizeof(struct mscp);
	}
}

/*
 * Try and find an unused command packet
 */
struct mscp *
ragetcp(sc)
	register ra_softcT	*sc;
{
	register struct	mscp	*mp = NULL;
	register int i;
	int s;
	segm seg5;

	s = splbio();
	saveseg5(seg5);
	mapseg5(ra_com[sc->sc_unit], MAPSEGDESC);
	i = sc->sc_lastcmd;
	if ((sc->sc_com->ra_ca.ca_cmd[i].hsh & (RA_OWN|RA_INT)) == RA_INT
	    && sc->sc_credits >= 2) {
		--sc->sc_credits;
		sc->sc_com->ra_ca.ca_cmd[i].hsh &= ~RA_INT;
		mp = &sc->sc_com->ra_cmd[i];
		ramsgclear(mp);
		sc->sc_lastcmd = (i + 1) % NCMD;
	}
	restorseg5(seg5);
	splx(s);
	return(mp);
}

/* Clear a mscp command packet */
ramsgclear(mp)
	register struct	mscp *mp;
{
	mp->m_unit = mp->m_modifier = mp->m_flags = 
		mp->m_bytecnt = mp->m_buf_l = mp->m_buf_h = 
		mp->m_elgfll = mp->m_copyspd = mp->m_elgflh =
		mp->m_opcode = mp->m_cntflgs = 0;	
}

/* Scan for response messages */
rarspring(sc)
	register ra_softcT *sc;
{
	register int i;

	sc->sc_com->ra_ca.ca_rspint = 0;
	i = sc->sc_lastrsp; 
	for (;;) {
		i %= NRSP;
		if (sc->sc_com->ra_ca.ca_rsp[i].hsh & RA_OWN)
			break;
		rarsp(&sc->sc_com->ra_rsp[i], sc);
		sc->sc_com->ra_ca.ca_rsp[i].hsh |= RA_OWN;
		++i;
	}
	sc->sc_lastrsp = i;
}

/*
 * Process a response packet
 */
rarsp(mp, sc)
	register struct	mscp *mp;
	register ra_softcT *sc;
{
	register struct	buf *dp;
	struct buf *bp;
	ra_infoT *disk;
	int st;

	/*
	 * Reset packet length and check controller credits
	 */
	mp->m_header.ra_msglen = sizeof(struct mscp);
	sc->sc_credits += mp->m_header.ra_credits & 0xf;
	if ((mp->m_header.ra_credits & 0xf0) > 0x10)
		return;

	/*
	 * If it's an error log message (datagram),
	 * pass it on for more extensive processing.
	 */
	if ((mp->m_header.ra_credits & 0xf0) == 0x10) {
		ra_error((struct mslg *)mp);
		return;
	}

	/*
	 * The controller interrupts as drive ZERO so check for it first.
	 */
	st = mp->m_status & M_S_MASK;
	if (mp->m_opcode == (M_O_STCON|M_O_END)) {
		if (st == M_S_SUCC)
			sc->sc_state = S_RUN;
		else
			sc->sc_state = S_IDLE;
		sc->sc_ctab.b_active = 0;
		wakeup((caddr_t)&sc->sc_ctab);
		return;
	}

	/*
	 * Check drive and then decode response and take action.
	 */
	switch (mp->m_opcode) {
	case M_O_ONLIN|M_O_END:
		if ((disk = sc->sc_drives[mp->m_unit]) == NULL) {
			printf("ra: couldn't ONLINE disk\n");
			break;
		}
		dp = &disk->ra_dtab;

		if (st == M_S_SUCC) {
			/* Link the drive onto the controller queue */
			dp->b_forw = NULL;
			if (sc->sc_ctab.b_actf == NULL)
				sc->sc_ctab.b_actf = dp;
			else
				sc->sc_ctab.b_actl->b_forw = dp;
			sc->sc_ctab.b_actl = dp;

			/*  mark it online */
			radisksetup(disk, mp);
			dp->b_active = 1;
		} else {
			printf("ra%d,%d: OFFLINE\n", sc->sc_unit, mp->m_unit);
			while (bp = dp->b_actf) {
				dp->b_actf = bp->av_forw;
				bp->b_flags |= B_ERROR;
				iodone(bp);
			}
		}

		/* Wakeup in open if online came from there */
		if (mp->m_cmdref != NULL)
			wakeup((caddr_t)mp->m_cmdref);
		break;

	case M_O_AVATN:
		/* it went offline and we didn't notice */
		PRINTD(("ra%d: unit %d attention\n", sc->sc_unit, mp->m_unit));
		if ((disk = sc->sc_drives[mp->m_unit]) != NULL)
			disk->ra_dsize = -1L;
		break;

	case M_O_END:
		/* controller incorrectly returns code 0200 instead of 0241 */
		PRINTD(("ra: back logical block request\n"));
		bp = (struct buf *)mp->m_cmdref;
		bp->b_flags |= B_ERROR;

	case M_O_READ | M_O_END:
	case M_O_WRITE | M_O_END:
		/* normal termination of read/write request */
		if ((disk = sc->sc_drives[mp->m_unit]) == NULL)
			break;
		bp = (struct buf *)mp->m_cmdref;

		/*
		 * Unlink buffer from I/O wait queue.
		 */
		bp->av_back->av_forw = bp->av_forw;
		bp->av_forw->av_back = bp->av_back;
		dp = &disk->ra_dtab;

#ifdef UCB_METER
		if (ra_dkn >= 0) {
			/* Messy, Should come up with a good way to do this */
			if (--dp->b_qsize == 0)
				dk_busy &= ~(1 << (ra_dkn + mp->m_unit));
		}
#endif
		if (st == M_S_OFFLN || st == M_S_AVLBL) {
			/* mark unit offline */
			disk->ra_dsize = -1L;

			/* Link the buffer onto the front of the drive queue */
			if ((bp->av_forw = dp->b_actf) == 0)
				dp->b_actl = bp;
			dp->b_actf = bp;

			/* Link the drive onto the controller queue */
			if (dp->b_active == 0) {
				dp->b_forw = NULL;
				if (sc->sc_ctab.b_actf == NULL)
					sc->sc_ctab.b_actf = dp;
				else
					sc->sc_ctab.b_actl->b_forw = dp;
				sc->sc_ctab.b_actl = dp;
				dp->b_active = 1;
			}
			return;
		}
		if (st != M_S_SUCC) {
			harderr(bp, "ra");
			log(LOG_INFO, "status %o\n", mp->m_status);
			bp->b_flags |= B_ERROR;
		}
		bp->b_resid = bp->b_bcount - mp->m_bytecnt;
		iodone(bp);
		break;

	case M_O_GTUNT|M_O_END:
		break;

	default:
		log(LOG_INFO,"ra: opcode %o\n", mp->m_opcode);
		ra_error((caddr_t)mp);
	}
}

/*
 * Init disk info structure from data in mscp packet
 */
radisksetup(disk, mp)
	register	ra_infoT	*disk;
	register	struct	mscp	*mp;
{
	register struct MEDIA *tp;
	int	nameid, numid, i;
	daddr_t	blks;

	nameid = (((loint(mp->m_mediaid) & 0x3f) << 9) | 
		   ((hiint(mp->m_mediaid) >> 7) & 0x1ff));
	numid = hiint(mp->m_mediaid) & 0x7f;

	/* Get unit total block count */
	disk->ra_dsize = mp->m_uslow + ((long)mp->m_ushigh << 16);

	/* spill the beans about what we have brought online */
	log(LOG_NOTICE, "ra%d: %c%c%d%c size=%D id: %X\n", mp->m_unit,
		mx(nameid,2), mx(nameid,1), numid, mx(nameid,0), 
		disk->ra_dsize, mp->m_mediaid);

	/* 
	 * Look for override in the table, if found use the partition
	 * table specified.  Else use a size based rule for assigning on
	 * of the 5 general purpose tables.
	 */

	for	(blks = disk->ra_dsize, tp = Mscptbl; tp->id7; tp++)
		{
		if	(tp->id7 == numid && tp->name15 == nameid)
			{
			disk->ra_size = tp->parts;
			log(LOG_NOTICE, "ra%d: old table\n", mp->m_unit);
			return;
			}
		}
	if	(blks < 24244L)
		{
		i = 3;			/* < ~12mb = type 3 */
		disk->ra_size = ra_gen3;
		}
	else if	(blks < 259740L)
		{
		i = 1;			/* < ~112mb = type 1 */
		disk->ra_size = ra_gen1;
		}
	else if	(blks < 669340L)	/* < ~330mb = type 2 */
		{
		i = 2;
		disk->ra_size = ra_gen2;
		}
	else if	(blks < 851804L)	/* < ~420mb = type 4 */
		{
		i = 4;
		disk->ra_size = ra_gen4;
		}
	else
		{
		i = 5;			/* > ~420mb = type 5 */
		disk->ra_size = ra_gen5;
		}
	log(LOG_NOTICE, "ra%d: type %d partitions\n", mp->m_unit, i);
}

/*
 * this is a routine rather than a macro to save space - shifting, etc 
 * generates a lot of code.
*/

mx(l, i)
	int l, i;
	{
	register int c;

	c = (l >> (5 * i)) & 0x1f;
	if	(c == 0)
		c = ' ' - '@';
	return(c + '@');
	}

/*
 * Process an error log message
 *
 * For now, just log the error on the console.  Only minimal decoding is done,
 * only "useful" information is printed.  Eventually should send message to
 * an error logger.  At least 90 bytes of D space were saved without losing
 * functionality.
 */
ra_error(mp)
	register struct mslg *mp;
{
	printf("ra: %s error, ",
		mp->me_flags & (M_LF_SUCC|M_LF_CONT) ? "soft" : "hard");

	switch (mp->me_format) {
	case M_F_CNTERR:
		printf("ctlr");
		break;
	case M_F_BUSADDR:
		printf("M_F_BUSADDR %o", mp->me_busaddr);
		break;
	case M_F_DISKTRN:
		printf("disk xfer, unit %d grp x%x hdr x%x",
			mp->me_unit, mp->me_group, mp->me_hdr);
		break;
	case M_F_SDI:
		printf("SDI unit %d hdr x%x", mp->me_unit, mp->me_hdr);
		break;
	case M_F_SMLDSK:
		printf("small disk unit %d cyl %d", mp->me_unit, mp->me_sdecyl);
		break;
	default:
		printf("?, unit %d format %o",mp->me_unit,mp->me_format);
	}

	printf(", event 0%o\n", mp->me_event);

#ifdef RADEBUG
	/* If error debug on, do hex dump */
	{
		register char *p = (char *)mp;
		register int i;

		for (i = mp->me_header.ra_msglen; i--; /*void*/)
			printf("%x ", *p++ & 0xff);
		printf("\n");
	}
#endif
}

/*
 * RA dump routines (act like stand alone driver)
 */
#ifdef RA_DUMP
#define DBSIZE	16			/* number of blocks to write */

radump(dev)
	dev_t dev;
{
	register ra_softcT *sc;
	register ra_infoT *disk;
	register struct mscp *mp;
	struct mscp *racmd();
	daddr_t	bn, dumpsize;
	long paddr, maddr;
	int count;
	struct ubmap *ubp;
	int 	unit, partition;
	segm	seg5;

	/* paranoia, space hack */
	disk = ra_disks;
	unit = RAUNIT(dev);
        sc = &ra_sc[RACON(dev)];
	partition = minor(dev) & 7;
	if (bdevsw[major(dev)].d_strategy != rastrategy || sc->RAADDR == NULL)
		return(EINVAL);

	/* Init RA controller */
	paddr = _iomap(ra_com[sc->sc_unit]);
	if (ubmap) {
		ubp = UBMAP;
		ubp->ub_lo = loint(paddr);
		ubp->ub_hi = hiint(paddr);
	}

	/* Get communications area and clear out packets */
	paddr += RINGBASE;
	saveseg5(seg5);
	mapseg5(ra_com[sc->sc_unit], MAPSEGDESC);
	mp = sc->sc_com->ra_rsp;
	sc->sc_com->ra_ca.ca_cmdint = sc->sc_com->ra_ca.ca_rspint = 0;
	bzero((caddr_t)mp, 2 * sizeof(*mp));

	/* Init controller */
	sc->RAADDR->raip = 0;
	while ((sc->RAADDR->rasa & RA_STEP1) == 0)
		/*void*/;
	sc->RAADDR->rasa = RA_ERR;
	while ((sc->RAADDR->rasa & RA_STEP2) == 0)
		/*void*/;
	sc->RAADDR->rasa = loint(paddr);
	while ((sc->RAADDR->rasa & RA_STEP3) == 0)
		/*void*/;
	sc->RAADDR->rasa = hiint(paddr);
	while ((sc->RAADDR->rasa & RA_STEP4) == 0)
		/*void*/;
	sc->RAADDR->rasa = RA_GO;
	ramsginit(sc, sc->sc_com->ra_ca.ca_rsp, mp, 0, 2, 0);
	if (!racmd(M_O_STCON, unit, sc)) {
		PRINTB(("radump: failed to start controller\n"));
		return(EFAULT);
	}
	PRINTB(("radump: controller up ok\n"));

	/* Bring disk for dump online */
	if (!(mp = racmd(M_O_ONLIN, unit, sc))) {
		PRINTB(("radump: failed to bring disk online\n"));
		return(EFAULT);
	}
	radisksetup(disk, mp);
 	dumpsize = MIN(disk->ra_dsize - disk->ra_size[partition].blkoff,
 			disk->ra_size[partition].nblocks);
	PRINTB(("radump: disk up ok, size=%D, type=%d\n",
		dumpsize, hiint(mp->m_mediaid) & 0xff));

	/* Check if dump ok on this disk */
	if (dumplo < 0 || dumplo >= dumpsize || dumpsize <= 0)
		return(EINVAL);
	dumpsize -= dumplo;

	/* Save core to dump partition */
	ubp = &UBMAP[1];
	for (paddr = 0L; dumpsize > 0; dumpsize -= count) {
		count = MIN(dumpsize, DBSIZE);
		bn = dumplo + (paddr >> PGSHIFT)
			+ disk->ra_size[partition].blkoff;
		maddr = paddr;

		if (ubmap) {
			ubp->ub_lo = loint(paddr);
			ubp->ub_hi = hiint(paddr);
			maddr = (u_int)(1 << 13);
		}

		/* Write it to the disk */
		mp = &sc->sc_com->ra_rsp[1];
		mp->m_lbn_l = loint(bn);
		mp->m_lbn_h = hiint(bn);
		mp->m_bytecnt = count * NBPG;
		mp->m_buf_l = loint(maddr);
		mp->m_buf_h = hiint(maddr);
		if (racmd(M_O_WRITE, unit, sc) == 0)
			return(EIO);

		paddr += (DBSIZE << PGSHIFT);
	}
	restorseg5(seg5);
	return(0);
}

struct	mscp	*
racmd(op, unit, sc)
	int op, unit;
	register ra_softcT *sc;
{
	register struct mscp *cmp, *rmp;
	Trl *rlp;
	int i;

	cmp = &sc->sc_com->ra_rsp[1];
	rmp = &sc->sc_com->ra_rsp[0];
	rlp = &sc->sc_com->ra_ca.ca_rsp[0];
	cmp->m_opcode = op;
	cmp->m_unit = unit;
	cmp->m_header.ra_msglen = rmp->m_header.ra_msglen = sizeof(struct mscp);
	rlp[0].hsh &= ~RA_INT;
	rlp[1].hsh &= ~RA_INT;
	rlp[0].hsh &= ~RA_INT;
	rlp[1].hsh &= ~RA_INT;
	rlp[0].hsh |= RA_OWN;
	rlp[1].hsh |= RA_OWN;
	i = sc->RAADDR->raip;
	while ((rlp[1].hsh & RA_INT) == 0)
		/*void*/;
	while ((rlp[0].hsh & RA_INT) == 0)
		/*void*/;
	sc->sc_com->ra_ca.ca_rspint = 0;
	sc->sc_com->ra_ca.ca_cmdint = 0;
	if (rmp->m_opcode != (op | M_O_END)
	    || (rmp->m_status & M_S_MASK) != M_S_SUCC) {
		ra_error(rmp);
		return(0);
	}
	return(rmp);
}
#endif RA_DUMP

/*
 * Assumes the 'open' routine has already been called to bring the
 * drive online and initialize the drive size structures.
*/
daddr_t
rasize(dev)
	register dev_t dev;
	{
	ra_softcT *sc = &ra_sc[RACON(dev)];
	register ra_infoT *disk;

	disk = sc->sc_drives[RAUNIT(dev)];
	return(disk->ra_size[dev & 7].nblocks);
	}
#endif NRAC > 0 && NRAD > 0
