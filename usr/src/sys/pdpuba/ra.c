/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ra.c	3.2 (2.11BSD GTE) 1997/2/14
 */

 /***********************************************************************
 *			Copyright (c) 1983 by				*
 *		Digital Equipment Corporation, Maynard, MA		*
 *			All rights reserved.				*
 ***********************************************************************/

/* 
 * ra.c - MSCP Driver
 * Date:	February 14, 1997
 * Use 'hz' to calculate delays rather than compile time constant.
 *
 * Date:	October 28, 1995
 * Fix multicontroller support (which was badly broken when disklabels were 
 * added).  Accessing drives on the second controller would cause serious 
 * filesystem corruption on the the corresponding drives on the first 
 * controller.
 *
 * Date:	August 1, 1995
 * Fix a bug which prohibited labeling previously disks which were unlabeled 
 * or had a corrupted label.  The default ('a' partition spanning the volume)
 * must be left in place to allow the write of the label.
 *
 * Date:	July 3, 1995
 * Fix a couple bugs and simplify the close protocol.
 *
 * Date:	June 16, 1995
 * Use the common label ioctl routine created today.
 *
 * Date:	June 15, 1995
 * Disklabels work!  A few minor changes made - raopen() needed to always
 * validate the partition number not just when the drive was first brought
 * online.
 *
 * Date:	May 20, 1995
 * Minor changes before beginning testing.
 *
 * Date:	May 03, 1995
 * Resume adding disklabel support.  The past several weeks were spent
 * porting/rewriting 'newfs', 'disklabel', 'getdiskbyname' and so on.
 *
 * Date:	Apr 13, 1995
 * Begin implementing disklabels.  First job was to remove/change references
 * to dkunit() since that macro has moved from buf.h to disk.h and takes a
 * 'dev_t' now instead of 'buf *'.
 *
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
 * sms@wlv.iipo.gtegsc.com (was wlv.imsd.contel.com at the time).
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
#include "ioctl.h"
#include "uba.h"
#include "rareg.h"
#include "dk.h"
#include "disklabel.h"
#include "disk.h"
#include "errno.h"
#include "file.h"
#include "stat.h"
#include <sys/kernel.h>

#define	RACON(x)			((minor(x) >> 6) & 03)
#define	RAUNIT(x)			((minor(x) >> 3) & 07)

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
	struct  dkdevice   ra_dk;	/* General disk info structure */
	daddr_t		ra_nblks;	/* Volume size from online pkt */
	short		ra_unit;	/* controller unit # */
	struct	buf	ra_utab;	/* buffer header for drive */
} ra_infoT;

#define	ra_bopen	ra_dk.dk_bopenmask
#define	ra_copen	ra_dk.dk_copenmask
#define	ra_open		ra_dk.dk_openmask
#define	ra_flags	ra_dk.dk_flags
#define	ra_label	ra_dk.dk_label
#define	ra_parts	ra_dk.dk_parts

typedef	struct	{
	radeviceT 	*RAADDR;	/* Controller bus address */
	short		sc_unit;	/* attach controller # */
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

ra_softcT	ra_sc[NRAC];	/* Controller table */
memaddr		ra_com[NRAC];	/* Communications area table */
ra_infoT	ra_disks[NRAD];	/* Disk table */

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

int	rastrategy();
daddr_t	rasize();

extern	int	wakeup();
extern	ubadr_t	_iomap();
extern	size_t	physmem;	/* used by the crash dump routine */
void	ragetinfo(), radfltlbl();
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

	for	(i = NRAD, p = ra_disks; i--; p++)
		if	((p->ra_flags & DKF_ALIVE) == 0)
			{
			p->ra_flags = DKF_ALIVE;
			return(p);
			}
	return(NULL);
}

/*
 * Open a RA.  Initialize the device and set the unit online.
 */
raopen(dev, flag, mode)
	dev_t 	dev;
	int 	flag;
	int	mode;
{
	register ra_infoT *disk;
	register struct	mscp *mp;
	register ra_softcT *sc = &ra_sc[RACON(dev)];
	int	unit = RAUNIT(dev);
	int	ctlr = RACON(dev);
	int	mask;
	int	s, i;

	PRINTD(("raopen: dev=%x, flags=%d\n", dev, flag));

	/* Check that controller exists */
	if	(ctlr >= NRAC || sc->RAADDR == NULL)
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
		timeout(wakeup, (caddr_t)&sc->sc_ctab, 12 * hz);
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
	disk = sc->sc_drives[unit];
	if (disk == NULL) {
		PRINTD(("raopen: opening new disk %d\n", unit));
		s = splbio();
		/* Allocate disk table entry for disk */
		if ((disk = ragetdd()) != NULL) {
			sc->sc_drives[unit] = disk;
			disk->ra_unit = ctlr;	/* controller number */
		} else {
			printf("ra: !disk structs\n");
			splx(s);
			return(ENXIO);
		}
	}
	/* Try to online disk unit, it might have gone offline */
	if ((disk->ra_flags & DKF_ONLINE) == 0) {
	/* In high kernel, don't saveseg5, just use normalseg5 later on. */
		while ((mp = ragetcp(sc)) == 0) {
			++sc->sc_cp_wait;
			sleep(&sc->sc_cp_wait, PSWP+1);
			--sc->sc_cp_wait;
		}
		mapseg5(ra_com[sc->sc_unit], MAPSEGDESC);
		mp->m_opcode = M_O_ONLIN;
		mp->m_unit = unit;
		mp->m_cmdref = (unsigned)&disk->ra_flags;
		((Trl *)mp->m_dscptr)->hsh |= RA_OWN|RA_INT;
		normalseg5();
		i = sc->RAADDR->raip;
		timeout(wakeup, (caddr_t)&disk->ra_flags, 10 * hz);
		sleep((caddr_t)&disk->ra_flags, PSWP+1);
		splx(s);
	}

	/* Did it go online? */
	if ((disk->ra_flags & DKF_ONLINE) == 0) {
		PRINTD(("raopen: disk didn't go online\n"));
		s = splbio();
		disk->ra_flags = 0;
		sc->sc_drives[unit] = NULL;
		splx(s);
		return(ENXIO);
	}
/*
 * Now we read the label.  Allocate an external label structure if one has
 * not already been assigned to this drive.  First wait for any pending
 * opens/closes to complete.
*/

	while	(disk->ra_flags & (DKF_OPENING | DKF_CLOSING))
		sleep(disk, PRIBIO);

/*
 * Next if an external label buffer has not already been allocated do so 
 * now.  This "can not fail" because if the initial pool of label buffers
 * has been exhausted the allocation takes place from main memory.  The
 * return value is the 'click' address to be used when mapping in the label.
*/

	if	(disk->ra_label == 0)
		disk->ra_label = disklabelalloc();

/*
 * On first open get label and partition info.  We may block reading the
 * label so be careful to stop any other opens.
*/
	if	(disk->ra_open == 0)
		{
		disk->ra_flags |= DKF_OPENING;
		ragetinfo(disk, dev);
		disk->ra_flags &= ~DKF_OPENING;
		wakeup(disk);
		}
/*
 * Need to make sure the partition is not out of bounds.  This requires
 * mapping in the external label.  This only happens when a partition
 * is opened (at mount time) and isn't an efficiency problem.
*/
	mapseg5(disk->ra_label, LABELDESC);
	i = ((struct disklabel *)SEG5)->d_npartitions;
	normalseg5();
	if	(dkpart(dev) >= i)
		return(ENXIO);

	mask = 1 << dkpart(dev);
	dkoverlapchk(disk->ra_open, dev, disk->ra_label, "ra");
	if	(mode == S_IFCHR)
		disk->ra_copen |= mask;
	else if	(mode == S_IFBLK)
		disk->ra_bopen |= mask;
	else
		return(EINVAL);
	disk->ra_open |= mask;
	PRINTD(("raopen: disk online\n"));
	return(0);
}

/*
 * Disk drivers now have to have close entry points in order to keep
 * track of what partitions are still active on a drive.
*/
raclose(dev, flag, mode)
	register dev_t	dev;
	int	flag, mode;
	{
	int	s, unit = RAUNIT(dev);
	register int	mask;
	register ra_infoT *disk;
	ra_softcT *sc = &ra_sc[RACON(dev)];

	disk = sc->sc_drives[unit];
	mask = 1 << dkpart(dev);
	if	(mode == S_IFCHR)
		disk->ra_copen &= ~mask;
	else if	(mode == S_IFBLK)
		disk->ra_bopen &= ~mask;
	else
		return(EINVAL);
	disk->ra_open = disk->ra_bopen | disk->ra_copen;
	if	(disk->ra_open == 0)
		{
		disk->ra_flags |= DKF_CLOSING;
		s = splbio();
		while	(disk->ra_utab.b_actf)
			sleep(&disk->ra_utab, PRIBIO);
		splx(s);
		disk->ra_flags &= ~DKF_CLOSING;
		wakeup(disk);
		}
	return(0);
	}

/*
 * This code was moved from ragetinfo() because it is fairly large and used
 * twice - once to initialize for reading the label and a second time if
 * there is no valid label present on the drive and the default one must be
 * used.
*/

void
radfltlbl(disk, lp)
	ra_infoT *disk;
	register struct	disklabel *lp;
	{
	register struct	partition *pi = &lp->d_partitions[0];

	bzero(lp, sizeof (*lp));
	lp->d_type = DTYPE_MSCP;
	lp->d_secsize = 512;		/* XXX */
	lp->d_nsectors = 32;
	lp->d_ntracks = 1;
	lp->d_secpercyl = 20 * 32;
	lp->d_npartitions = 1;		/* 'a' */
	pi->p_size = disk->ra_nblks;	/* entire volume */
	pi->p_fstype = FS_V71K;
	pi->p_frag = 1;
	pi->p_fsize = 1024;
/*
 * Put where rastrategy() will look.
*/
	bcopy(pi, disk->ra_parts, sizeof (lp->d_partitions));
	}

/*
 * Read disklabel.  It is tempting to generalize this routine so that
 * all disk drivers could share it.  However by the time all of the 
 * necessary parameters are setup and passed the savings vanish.  Also,
 * each driver has a different method of calculating the number of blocks
 * to use if one large partition must cover the disk.
 *
 * This routine used to always return success and callers carefully checked
 * the return status.  Silly.  This routine will fake a label (a single
 * partition spanning the drive) if necessary but will never return an error.
 *
 * It is the caller's responsibility to check the validity of partition 
 * numbers, etc.
*/

void
ragetinfo(disk, dev)
	register ra_infoT *disk;
	dev_t	dev;
	{
	struct	disklabel locallabel;
	char	*msg;
	register struct disklabel *lp = &locallabel;
/*
 * NOTE: partition 0 ('a') is used to read the label.  Therefore 'a' must
 * start at the beginning of the disk!  If there is no label or the label
 * is corrupted then 'a' will span the entire disk
*/

	radfltlbl(disk, lp);		/* set  up default/fake label */
	msg = readdisklabel((dev & ~7) | 0, rastrategy, lp);	/* 'a' */
	if	(msg != 0)
		{
		log(LOG_NOTICE, "ra%da is entire disk: %s\n", dkunit(dev), msg);
		radfltlbl(disk, lp);
		}
	mapseg5(disk->ra_label, LABELDESC);
	bcopy(lp, (struct disklabel *)SEG5, sizeof (struct disklabel));
	normalseg5();
	bcopy(lp->d_partitions, disk->ra_parts, sizeof (lp->d_partitions));
	return;
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
	int	unit = RAUNIT(bp->b_dev);
	int	part = dkpart(bp->b_dev);
	struct	partition *pi;
	daddr_t sz, maxsz;
	int s;

	/* Is disk online */
	if ((disk = sc->sc_drives[unit]) == NULL || 
			!(disk->ra_flags & (DKF_ONLINE | DKF_ALIVE)))
		goto bad;
	pi = &disk->ra_parts[part];

	/* Valid block in device partition */
	sz = (bp->b_bcount + 511) >> 9;
	if	(bp->b_blkno < 0 || bp->b_blkno + sz > pi->p_size)
		{
		sz = pi->p_size - bp->b_blkno;
		/* if exactly at end of disk, return an EOF */
		if	(sz == 0)
			{
			bp->b_resid = bp->b_bcount;
			goto done;	
			}
		/* or truncate if part of it fits */
		if	(sz < 0)
			{
			bp->b_error = EINVAL;
			goto bad;
			}
		bp->b_bcount = dbtob(sz);	/* compute byte count */
		}
/*
 * Check for write to write-protected label area.  This does not include
 * sector 0 which is the boot block.
*/
	if	(bp->b_blkno + pi->p_offset <= LABELSECTOR &&
		 bp->b_blkno + pi->p_offset + sz > LABELSECTOR &&
		 !(bp->b_flags & B_READ) && !(disk->ra_flags & DKF_WLABEL))
		{
		bp->b_error = EROFS;
		goto bad;
		}

	mapalloc(bp);		/* Unibus Map buffer if required */

	/*
	 * Link the buffer onto the drive queue
	 */
	s = splbio();
	dp = &disk->ra_utab;
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
done:
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
	struct partition *pi;
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
		 * The drive may not need anything, in which case it might
		 * be shutting down in raclose() and a wakeup is needed.
		 */
		dp->b_active = 0;
		sc->sc_ctab.b_actf = dp->b_forw;
		i = offsetof(ra_infoT, ra_utab);
		disk = (ra_infoT *)((int)dp - i);
		if	(disk->ra_open == 0)
			wakeup(dp);	/* finish close protocol */
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
	mp->m_unit = RAUNIT(bp->b_dev);
	disk = sc->sc_drives[mp->m_unit];
	pi = &disk->ra_parts[dkpart(bp->b_dev)];
	temp = bp->b_blkno + pi->p_offset;
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
		printf("ra: state %d ign\n", sc->sc_state);
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
	if (sc->sc_com->ra_ca.ca_cmdint)
		sc->sc_com->ra_ca.ca_cmdint = 0;

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
			log(LOG_NOTICE,"ra%d !ONLINE\n", sc->sc_unit * 8 +
				mp->m_unit);
			break;
		}
		dp = &disk->ra_utab;

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
			printf("ra%d: OFFLINE\n", sc->sc_unit * 8 + mp->m_unit);
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
		PRINTD(("ra%d: attention\n", sc->sc_unit * 8 + mp->m_unit));
		if ((disk = sc->sc_drives[mp->m_unit]) != NULL)
			disk->ra_flags &= ~DKF_ONLINE;
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
		dp = &disk->ra_utab;

#ifdef UCB_METER
		if (ra_dkn >= 0) {
			/* Messy, Should come up with a good way to do this */
			if (--dp->b_qsize == 0)
				dk_busy &= ~(1 << (ra_dkn + mp->m_unit));
		}
#endif
		if (st == M_S_OFFLN || st == M_S_AVLBL) {
			/* mark unit offline */
			disk->ra_flags &= ~DKF_ONLINE;

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
		log(LOG_INFO,"ra: op %o\n", mp->m_opcode);
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
	int	nameid, numid;

	nameid = (((loint(mp->m_mediaid) & 0x3f) << 9) | 
		   ((hiint(mp->m_mediaid) >> 7) & 0x1ff));
	numid = hiint(mp->m_mediaid) & 0x7f;

	/* Get unit total block count */
	disk->ra_nblks = mp->m_uslow + ((long)mp->m_ushigh << 16);

	/* spill the beans about what we have brought online */
	log(LOG_NOTICE, "ra%d: %c%c%d%c size=%D\n", 
		disk->ra_unit * 8 + mp->m_unit,
		mx(nameid,2), mx(nameid,1), numid, mx(nameid,0), 
		disk->ra_nblks);
	disk->ra_flags |= DKF_ONLINE;
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

raioctl(dev, cmd, data, flag)
	dev_t	dev;
	int	cmd;
	caddr_t data;
	int	flag;
	{
	ra_softcT *sc = &ra_sc[RACON(dev)];
	ra_infoT  *disk = sc->sc_drives[RAUNIT(dev)];
	int	error;

	error = ioctldisklabel(dev, cmd, data, flag, disk, rastrategy);
	return(error);
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
	printf("ra: %s err, ",
		mp->me_flags & (M_LF_SUCC|M_LF_CONT) ? "soft" : "hard");

	switch (mp->me_format) {
	case M_F_CNTERR:
		printf("ctlr");
		break;
	case M_F_BUSADDR:
		printf("M_F_BUSADDR %o", mp->me_busaddr);
		break;
	case M_F_DISKTRN:
		printf("disk xfr, unit %d grp x%x hdr x%x",
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
	struct	partition *pi;
	daddr_t	bn, dumpsize;
	long paddr, maddr;
	int count, memblks;
	struct ubmap *ubp;
	int 	unit, partition;
	segm	seg5;

	/* paranoia, space hack */
	unit = RAUNIT(dev);
        sc = &ra_sc[RACON(dev)];
	partition = dkpart(dev);
	if	(sc->RAADDR == NULL)
		return(EINVAL);
	disk = sc->sc_drives[unit];
/*
 * The drive to which we dump must be present and alive.
*/
	if	(!disk || !(disk->ra_flags & DKF_ALIVE))
		return(ENXIO);
	pi = &disk->ra_parts[partition];
/*
 * Paranoia - we do not dump to a partition if it has not been declared as
 * a 'swap' type of filesystem.
*/
	if	(pi->p_fstype != FS_SWAP)
		return(EFTYPE);

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
		PRINTB(("radump: failed start controller\n"));
		return(EFAULT);
	}
	PRINTB(("radump: controller up\n"));

	/* Bring disk for dump online */
	if (!(mp = racmd(M_O_ONLIN, unit, sc))) {
		PRINTB(("radump: failed online\n"));
		return(EFAULT);
	}

 	dumpsize = rasize(dev) - dumplo;
	memblks = ctod(physmem);
	PRINTB(("radump: disk up, size=%D, type=%d\n",
		dumpsize, hiint(mp->m_mediaid) & 0xff));

	/* Check if dump ok on this disk */
	if (dumplo < 0 || dumpsize <= 0)
		return(EINVAL);
	if	(memblks > dumpsize)
		memblks = dumpsize;
	bn = dumplo + pi->p_offset;

	/* Save core to dump partition */
	ubp = &UBMAP[1];
	for	(paddr = 0L; memblks > 0; ) {
		count = MIN(memblks, DBSIZE);
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

		paddr += (count << PGSHIFT);
		bn += count;
		memblks -= count;
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
 * Return the number of blocks in a partition.  Call raopen() to online
 * the drive if necessary.  If an open is necessary then a matching close
 * will be done.
*/
daddr_t
rasize(dev)
	register dev_t dev;
	{
	ra_softcT *sc = &ra_sc[RACON(dev)];
	register ra_infoT *disk;
	daddr_t	psize;
	int	didopen = 0;

	disk = sc->sc_drives[RAUNIT(dev)];
/*
 * This should never happen but if we get called early in the kernel's
 * life (before opening the swap or root devices) then we have to do
 * the open here.
*/
	if	(disk->ra_open == 0)
		{
		if	(raopen(dev, FREAD|FWRITE, S_IFBLK))
			return(-1);
		didopen = 1;
		}
	psize = disk->ra_parts[dkpart(dev)].p_size;
	if	(didopen)
		raclose(dev, FREAD|FWRITE, S_IFBLK);
	return(psize);
	}
#endif NRAC > 0 && NRAD > 0
