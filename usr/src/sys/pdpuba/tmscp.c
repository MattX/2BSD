/*	@(#)tmscp.c	1.4 (2.11BSD GTE) 12/9/94 */

#if	!defined(lint) && defined(DOSCCS)
static	char	*sccsid = "@(#)tmscp.c	1.24	(ULTRIX)	1/21/86";
#endif

/************************************************************************
 *									*
 *        Licensed from Digital Equipment Corporation 			*
 *                       Copyright (c) 					*
 *               Digital Equipment Corporation				*
 *                   Maynard, Massachusetts 				*
 *                         1985, 1986 					*
 *                    All rights reserved. 				*
 *									*
 *        The Information in this software is subject to change 	*
 *   without notice and should not be construed as a commitment 	*
 *   by  Digital  Equipment  Corporation.   Digital   makes  no 	*
 *   representations about the suitability of this software for 	*
 *   any purpose.  It is supplied "As Is" without expressed  or 	*
 *   implied  warranty. 						*
 *									*
 *        If the Regents of the University of California or its 	*
 *   licensees modify the software in a manner creating  		*
 *   diriviative copyright rights, appropriate copyright  		*
 *   legends may be placed on  the drivative work in addition  		*
 *   to that set forth above. 						*
 *									*
 ************************************************************************
 * 
 * tmscp.c - TMSCP (TK50/TU81) tape device driver
 * 
 * Modification History:
 * 02-Jan-93 - sms@wlv. [2.11BSD]
 *	Remove unibus map ifdefs and rely on run time testing of 'ubmap' which
 * 	does the right thing and makes kernels easier to move between machines.
 *
 * 20-Nov-92 - sms@wlv.iipo.gtegsc.com [2.11BSD]
 *	Add tmsVec() for autoconfig to use in passing the vector from /etc/dtab
 *	to this driver.  The previous scheme of having a fixed vector with
 *	all TMSCP vectors contiguous has been removed.
 *
 * 23-May-91 - sms@wlv.iipo.gtegsc.com [2.11BSD]
 *	Minor typo in the multicontroller support fixed.  Major overhaul
 *	of the density selection, apparently some TMSCP controllers
 *	do not treat 0 in the mscp_format word as meaning the highest
 *	density, instead leaving the drive in the current/last-used
 *	density.
 *
 * 29-Mar-91 - sms@wlv.iipo.gtegsc.com [2.11BSD]
 *	Major changes to 1) support multiple drives per controller
 *	(the maximum number of drives per controller is 4 for now in order
 *	to maximize compatibility with existing minor device numbers - the
 *	norewind and density bit stay in the same place) and 2) more
 *	importantly reduce the bloat of this driver.  The command
 *	packet area is now allocated from an external heap setup at boot
 *	time (the MSCP, ra.c driver also does this).  Allocating from an
 * 	external arena save approximately 2kb of kernel D space PER CONTROLLER
 *	and costs little in speed because the amount of remapping involved is
 *	quite small.
 *
 *	The 'errinfo' routine was removed in the interest of space savings,
 *	the error code was already being printed out in hex and it's not
 *	worth eating up another 250 or so bytes of kernel D space to pretty
 *	print messages for which tmscp.h provides the cross reference (I space
 *	is also saved by not printing the messages).  Besides, the ra.c (MSCP)
 *	driver doesn't do it and it is worth a degree of non 4.3BSD verisimility
 *	to save a significant amount of space.
 *
 *	The tms_type field should have been a 'long' (mediatype).  Since the
 *	drives are not probed at autoconfigure time a GTUNT (TMS_SENSE) command
 *	is done at open() time to fetch the format/density menu and unit flags.
 *	iodone() proccessing was missing for the GTUNT function in tmscprsp()
 *	causing hangs in both open and ioctl functions.
 *
 *	Multiple controller support made to work, the top 2 bits of the minor
 *	device number are used to designate the controller, thus there is
 *	a maximum of 4 TMSCP controllers per system.
 *
 * 17-Jun-90,14Aug90 - sms@wlv.iipo.gtegsc.com
 *	Began porting to 2.10.1BSD/2.11BSD.  Multiple drives per controller
 *	NOT supported, although multiple controllers are (maybe).  Programmable
 *	vectors don't work very well with the autoconfigure scheme in use.
 *	the define TMSCP_VEC will have to be adjusted in tms.h (see
 *	conf/config and the sample config files).  For patching purposes
 *	the global "TMSvec" is declared and initialized with TMSCP_VEC.
 *
 * 06-Jan-86 - afd
 *	Changed the probe routine to use DELAY (not TODR).  This now
 *	works for MicroVAXen as well.  This eliminates the busy-wait
 *	for MicroVAXen so a dead TK50 controller will not hang autoconf.
 *
 * 06-Dec-85 - afd
 *	Fixed a bug in density selection.  The "set unit characteristics"
 *	command to select density, was clearing the "unit flags" field
 *	where the CACHE bit was for TU81-E.  Now the unit's "format" and
 *	"unitflgs" are saved in tms_info struct.  And are used on STUNT
 *	commands.
 *
 * 19-Oct-85 - afd
 *	Added support to the open routine to allow drives to be opened
 *	for low density (800 or 1600 bpi) use.  When the slave routine
 *	initiates a "get-unit-char" cmd, the format menu for the unit
 *	is saved in the tms_info structure. The format menu is used in the
 *	start routine to select the proper low density.
 *
 * 02-Oct-85 - afd
 *	When a tmscp-type controller is initializing, it is possible for
 *	the sa reg to become 0 between states.  Thus the init code in
 *	the interrupt routine had to be modified to reflect this.
 *
 * 21-Sep-85 - afd
 *	The TK50 declares a serious exception when a tape mark is encountered.
 *	This causes problems to dd (& other UN*X utilities).  So a flag
 *	is set in the rsp() routine when a tape mark is encountered.  If
 *	this flag is set, the start() routine appends the Clear Serious
 *	Exception modifier to the next command.
 *
 * 03-Sep-85 -- jaw
 *	messed up previous edit..
 *
 * 29-Aug-85 - jaw
 *	fixed bugs in 8200 and 750 buffered datapath handling.
 *
 * 06-Aug-85 - afd
 *   1. When repositioning records or files, the count of items skipped
 *	does NOT HAVE to be returned by controllers (& the TU81 doesn't).
 *	So tmscprsp() had to be modified to stop reporting
 *	residual count errors on reposition commands.
 *
 *   2. Fixed bug in the open routine which allowed multiple opens.
 *
 * 18-Jul-85 - afd
 *   1. Need to return status when mt status (or corresponding ioctl) is done.
 *	Save resid, flags, endcode & status in tmscprsp() routine (except on
 *	clear serious exception no-op).  Return these fields when status
 *	ioctl is done (in tmscpcommand()).  How they are returned:
 *		mt_resid = resid
 *		mt_dsreg = flags|endcode
 *		mt_erreg = status
 *
 *   2. Added latent support for enabling/disabling caching.  This is
 *	handled along with all other ioctl commands.
 *
 *   3. Need to issue a no-op on unrecognized ioctl in tmsstart(), since
 *	we have already commited to issuing a command at that point.
 *
 *   4. In tmscprsp() routine if encode is 0200 (invalid command issued);
 *	We need to: Unlink the buffer from the I/O wait queue,
 *	and signal iodone, so the higher level command can exit!
 *	Just as if it were a valid command.
 *
 * 11-jul-85 -- jaw
 *	fix bua/bda map registers.
 *
 * 19-Jun-85 -- jaw
 *	VAX8200 name change.
 *
 * 06-Jun-85 - jaw
 *	fixes for 8200.
 *
 * 9-Apr-85 - afd
 *	Added timeout code to the probe routine, so if the controller
 *	fails to init in 10 seconds we return failed status.
 *
 * 13-Mar-85 -jaw
 *	Changes for support of the VAX8200 were merged in.
 *
 * 27-Feb-85 -tresvik
 *	Changes for support of the VAX8600 were merged in.
 *
 */

#include "tms.h"
#if NTMSCP > 0

#include "param.h"
#include "systm.h"
#include "buf.h"
#include "conf.h"
#include "user.h"
#include "file.h"
#include "map.h"
#include "ioctl.h"
#include "syslog.h"
#include "mtio.h"
#include "uio.h"
#include "tty.h"
#include "uba.h"

#define	TMS_PRI	LOG_INFO

#define NRSPL2  3               /* log2 number of response packets */
#define NCMDL2  3               /* log2 number of command packets */
#define NRSP    (1<<NRSPL2)
#define NCMD    (1<<NCMDL2)

#include "tmscpreg.h"
#include "../pdp/tmscp.h"
#include "../machine/seg.h"

/*
 * The density array is indexed by the density bits (bits 3 and 4) of the
 * minor device number AND the format menu returned for the drive.  Since
 * only 3 densities are apparently supported by TMSCP (no DPE/3200bpi), the 
 * second MEDium table is a copy of the first MEDium entry.
*/

	char	Dmatrix[4][8] =
		{
/* LOW */   {0,M_TF_800,M_TF_PE,M_TF_800,M_TF_GCR,M_TF_800,M_TF_PE,M_TF_800},
/* MED1 */  {0,M_TF_800,M_TF_PE,M_TF_800,M_TF_GCR,M_TF_800,M_TF_PE,M_TF_PE },
/* MED2 */  {0,M_TF_800,M_TF_PE,M_TF_800,M_TF_GCR,M_TF_800,M_TF_PE,M_TF_PE },
/* HI */    {0,M_TF_800,M_TF_PE,M_TF_PE,M_TF_GCR,M_TF_GCR,M_TF_GCR,M_TF_GCR}
		};

/* Software state per controller */

struct tmscp_softc {
	struct	tmscpdevice *sc_addr;	/* controller CSR address */
	short   sc_state;       /* state of controller */
	short	sc_ivec;        /* interrupt vector address */
	short	sc_unit;	/* CONTROLLER number - NOT drive unit # */
	short   sc_credits;     /* transfer credits */
	short   sc_lastcmd;     /* pointer into command ring */
	short   sc_lastrsp;     /* pointer into response ring */
	struct	buf sc_cmdbuf;	/* internal command buffer */
	struct	buf sc_ctab;	/* controller queue */
	struct	buf sc_wtab;	/* I/O wait queue for controller */
	struct	tmscp *sc_com;	/* communications area pointer */
	struct	tms_info *sc_drives[4];	/* pointers to per drive info */
} tmscp_softc[NTMSCP];

#define	RINGBASE	(4 * sizeof (short))

struct tmscp {
	struct tmscpca	tmscp_ca;         /* communications area */
	struct mscp	tmscp_rsp[NRSP];  /* response packets */
	struct mscp	tmscp_cmd[NCMD];  /* command packets */
};					  /* 1896 bytes per controller! */

	memaddr	tmscp[NTMSCP];		/* click addresses of ctrl comm area */

/*
 * Per drive-unit info
 */
struct tms_info {
	daddr_t		tms_dsize;	/* Max user size from online pkt */
	long		tms_type;	/* Drive type field  */
	int		tms_resid;	/* residual from last xfer */
	u_char		tms_endcode;	/* last command endcode */
	u_char		tms_flags;	/* last command end flags */
	unsigned	tms_status;	/* Command status from last command */
	char		tms_openf;	/* lock against multiple opens */
	char		tms_lastiow;	/* last op was a write */
	char		tms_serex;	/* set when serious exception occurs */
	char		tms_clserex;	/* set when serex being cleared by no-op */
	short		tms_fmtmenu;	/* the unit's format (density) menu */
	short		tms_unitflgs;	/* unit flag parameters */
	short		tms_format;	/* the unit's current format (density) */
	struct tty	*tms_ttyp;	/* record user's tty for errors */
	struct	buf	tms_dtab;	/* I/O tape drive queues */
	short		tms_online;	/* 0=available, 1=online, -1=offline */
} tms_info[NTMS];

/* Bits in minor device */
#define	TMSUNIT(dev)	(minor(dev)&03)
#define	TMSCTLR(dev)	((minor(dev) >> 6) & 3)
#define	TMSDENS(dev)	((minor(dev) >> 3) & 3)
#define	FMTMASK		(M_TF_800|M_TF_PE|M_TF_GCR)	/* = 7 */

#define	T_NOREWIND	04

/* Size to map in when mapping a controller's command packet area */
#define	MAPBUFDESC	(((btoc(sizeof (struct tmscp)) - 1) << 8) | RW)

/*
 * Internal (ioctl) command codes (these must also be declared in the
 * tmscpioctl routine).  These correspond to ioctls in mtio.h
 */
#define TMS_WRITM	0		/* write tape mark */
#define TMS_FSF		1		/* forward space file */
#define TMS_BSF		2		/* backward space file */
#define TMS_FSR		3		/* forward space record */
#define TMS_BSR		4		/* backward space record */
#define TMS_REW		5		/* rewind tape */
#define TMS_OFFL	6		/* rewind tape & mark unit offline */
#define TMS_SENSE	7		/* noop - do a get unit status */
#define TMS_CACHE	8		/* enable cache */
#define TMS_NOCACHE	9		/* disable cache */
/* These go last: after all real mt cmds, just bump the numbers up */
#define TMS_CSE		10		/* clear serious exception */
#define TMS_SETDENSITY	11		/* set unit density */

/*
 * Controller states
 */
#define S_IDLE  0               /* hasn't been initialized */
#define S_STEP1 1               /* doing step 1 init */
#define S_STEP2 2               /* doing step 2 init */
#define S_STEP3 3               /* doing step 3 init */
#define S_SCHAR 4               /* doing "set controller characteristics" */
#define S_RUN   5               /* running */

static	char *tmscpstepfailed = "step%d init failed: sa 0x%x\n";
static	char *tmscpfatalerr = "tms%d,%d: fatal error 0%o\n";

int     tmscperror = 0;		/* causes hex dump of packets */
int	tmscp_cp_wait = 0;	/* Something to wait on for command */
				/* packets and or credits. */
int	wakeup();
extern	int	hz;		/* Should find the right include */
extern	long	_iomap();

#ifdef	TMSCP_DEBUG
#define printd if (tmscpdebug) printf
int tmscpdebug = 1;
#define	printd10 if(tmscpdebug >= 10) printf
#endif 

struct  mscp *tmscpgetcp();

#define b_qsize         b_resid         /* queue size per drive, in tmsutab */

tmsVec(ctlr, vector)
	int	ctlr, vector;
	{
	register struct tmscp_softc *sc = &tmscp_softc[ctlr];

	if	(ctlr >= NTMSCP || sc->sc_ivec)
		return(-1);
	sc->sc_ivec = vector;
	return(0);
	}

/* 
 * Open routine will issue the online command, later.
 *
 * This routine attaches controllers, not drives - sc_unit and 'unit' are 
 * the controller number not a drive unit number.  sc_com is initialized
 * to SEG5 because all communication areas are mapped to the same virtual
 * address now.
 */
tmsattach(addr, unit)
	struct tmscpdevice *addr;
	register int unit;
{
	register struct tmscp_softc *sc = &tmscp_softc[unit];

	if (unit >= NTMSCP)
		return(0);
	if (sc->sc_addr == NULL && addr != NULL) {
		tmscp[unit] = (memaddr)_ioget(sizeof (struct tmscp));
		if (tmscp[unit] == NULL)
			return(0);
		sc->sc_addr = addr;
		sc->sc_unit = unit;
		sc->sc_com = (struct tmscp *)SEG5;
		return (1);
	}
	return(0);
}

struct tms_info *
getdd()
{
	register int i;
	register struct tms_info *p;

	for (i = NTMS, p = tms_info; i--; p++) {
		if (p->tms_online == 0)
			return(p);
	}
	log(TMS_PRI, "tms: no drive descriptors\n");
	return(NULL);
}

/*
 * TMSCP interrupt routine.
 */
tmsintr(dev)
	dev_t dev;
{
	register struct tmscpdevice *tmscpaddr;
	struct buf *bp;
	register int i;
	register struct tmscp_softc *sc = &tmscp_softc[dev];
	register struct tmscp *tm = sc->sc_com;
	struct mscp *mp;
	segm seg5;

	tmscpaddr = sc->sc_addr;
#ifdef TMSCP_DEBUG
	printd10("tmscpintr: state %d, tmscpsa %o\n", sc->sc_state, tmscpaddr->tmscpsa);
#endif	

	/*
	 * How the interrupt is handled depends on the state of the controller.
	 */
	switch (sc->sc_state) {

	case S_IDLE:
		printf("tms%d: random intr ignored\n", dev);
		return;

	/* Controller was in step 1 last, see if its gone to step 2 */
	case S_STEP1:
#		define STEP1MASK 0174377
#		define STEP1GOOD (TMSCP_STEP2|TMSCP_IE|(NCMDL2<<3)|NRSPL2)
		for (i = 0; i < 150; i++)
			{
			if ((tmscpaddr->tmscpsa&STEP1MASK) != STEP1GOOD)
				{ /* still in step 1 (wait 1/100 sec) */
				delay(10000L);
#ifdef TMSCP_DEBUG
				printd("still in step 1, delaying\n");
#endif
				}
			else
				break;
			}
		if (i > 149)
			{
			sc->sc_state = S_IDLE;
			sc->sc_ctab.b_active = 0;
			printf(tmscpstepfailed,sc->sc_state,tmscpaddr->tmscpsa);
			wakeup((caddr_t)&sc->sc_ctab);
			return;
			}
		tmscpaddr->tmscpsa = (short)sc->sc_ctab.b_un.b_addr;
		sc->sc_state = S_STEP2;
		return;

	/* Controller was in step 2 last, see if its gone to step 3 */
	case S_STEP2:
#		define STEP2MASK 0174377
#		define STEP2GOOD (TMSCP_STEP3|TMSCP_IE|(sc->sc_ivec/4))
		for (i = 0; i < 150; i++)
			{
			if ((tmscpaddr->tmscpsa&STEP2MASK) != STEP2GOOD)
				{ /* still in step 2 (wait 1/100 sec) */
				delay(10000L);
#ifdef TMSCP_DEBUG
				printd("still in step 2, delaying\n");
#endif
				}
			else
				break;
			}
		if (i > 149)
			{
			sc->sc_state = S_IDLE;
			sc->sc_ctab.b_active = 0;
			printf(tmscpstepfailed,sc->sc_state,tmscpaddr->tmscpsa);
			wakeup((caddr_t)&sc->sc_ctab);
			return;
			}
		tmscpaddr->tmscpsa = sc->sc_ctab.b_xmem;
		sc->sc_state = S_STEP3;
		return;

	/* Controller was in step 3 last, see if its gone to step 4 */
	case S_STEP3:
#		define STEP3MASK 0174000
#		define STEP3GOOD TMSCP_STEP4
		for (i = 0; i < 150; i++)
			{
			if ((tmscpaddr->tmscpsa&STEP3MASK) != STEP3GOOD)
				{ /* still in step 3 (wait 1/100 sec) */
				delay(10000L);
#ifdef TMSCP_DEBUG
				printd("still in step 3, delaying\n");
#endif
				}
			else
				break;
			}
		if (i > 149)
			{
			sc->sc_state = S_IDLE;
			sc->sc_ctab.b_active = 0;
			printf(tmscpstepfailed,sc->sc_state,tmscpaddr->tmscpsa);
			wakeup((caddr_t)&sc->sc_ctab);
			return;
			}
		/*
		 * Get microcode version and model number of controller;
		 * Signal initialization complete (_GO) (to the controller);
		 *    ask for Last Fail response if tmscperror is set;
		 * Set state to "set controller characteristics".
		 */
		i = tmscpaddr->tmscpsa;
		tmscpaddr->tmscpsa = TMSCP_GO | (tmscperror? TMSCP_LF : 0);
		sc->sc_state = S_SCHAR;
#ifdef TMSCP_DEBUG
		printd("tmscpintr: completed state %d \n", sc->sc_state);
		printd("tmscp%d Version: %d model %d\n", dev, i & 0xf, (i >> 4) & 0xf);
#endif

	    /*
	     * Initialize the data structures (response and command queues).
	     */
	    saveseg5(seg5);
	    mapseg5(tmscp[sc->sc_unit], MAPBUFDESC);
	    tmsginit(sc, sc->sc_com->tmscp_ca.ca_rspdsc, sc->sc_com->tmscp_rsp,
			0, NRSP, TMSCP_OWN|TMSCP_INT);
	    tmsginit(sc, sc->sc_com->tmscp_ca.ca_cmddsc, sc->sc_com->tmscp_cmd,
			NRSP, NCMD, TMSCP_INT);
	    bp = &sc->sc_wtab;
	    bp->av_forw = bp->av_back = bp;
	    sc->sc_lastcmd = 1;
	    sc->sc_lastrsp = 0;
	    mp = sc->sc_com->tmscp_cmd;
	    mp->mscp_unit = mp->mscp_modifier = 0;
	    mp->mscp_flags = 0;
	    mp->mscp_version = 0;
	    mp->mscp_cntflgs = M_CF_ATTN|M_CF_MISC|M_CF_THIS;
	    /*
	     * A host time out value of 0 means that the controller will not
	     * time out.  This is ok for the TK50.
	     */
	    mp->mscp_hsttmo = 0;
	    bzero(&mp->mscp_time, sizeof (mp->mscp_time));
	    mp->mscp_cntdep = 0;
	    mp->mscp_opcode = M_OP_STCON;
	    ((Trl *)mp->mscp_dscptr)->hsh |= TMSCP_OWN|TMSCP_INT;
	    i = tmscpaddr->tmscpip;      /* initiate polling */
	    restorseg5(seg5);
	    return;

	case S_SCHAR:
	case S_RUN:
		break;

	default:
	    printf("tms%d: intr unknown state %d ignored\n",dev,sc->sc_state);
	    return;
	}	/* end switch */

	/*
	 * The controller state is S_SCHAR or S_RUN
	 */

	/*
	 * If the error bit is set in the SA register then print an error
	 * message and reinitialize the controller.
	 */
	if (tmscpaddr->tmscpsa&TMSCP_ERR)
		{
		printf(tmscpfatalerr, dev, sc->sc_unit, tmscpaddr->tmscpsa);
		tmscpaddr->tmscpip = 0;
		sc->sc_state = S_IDLE;
		sc->sc_ctab.b_active = 0;
		wakeup((caddr_t)&sc->sc_ctab);
		}
	/*
	 * Check for a buffer purge request. (Won't happen w/ TK50 on Q22 bus)
	 */
	saveseg5(seg5);
	mapseg5(tmscp[sc->sc_unit], MAPBUFDESC);
	if (tm->tmscp_ca.ca_bdp)
		{
		tm->tmscp_ca.ca_bdp = 0;
		tmscpaddr->tmscpsa = 0;      /* signal purge complete */
		}

	/*
	 * Check for response ring transition.
	 */
	if (tm->tmscp_ca.ca_rspint)
		{
		tm->tmscp_ca.ca_rspint = 0;
		for (i = sc->sc_lastrsp;; i++)
			{
			i %= NRSP;
			if (tm->tmscp_ca.ca_rspdsc[i].hsh&TMSCP_OWN)
			    break;
			tmscprsp(sc, i);
			tm->tmscp_ca.ca_rspdsc[i].hsh |= TMSCP_OWN;
			}
		sc->sc_lastrsp = i;
		}

	/*
	 * Check for command ring transition. (Should never happen!)
	 */
	if (tm->tmscp_ca.ca_cmdint)
		{
#ifdef TMSCP_DEBUG
		printd("tmscpintr: command ring transition\n");
#endif		
		tm->tmscp_ca.ca_cmdint = 0;
		}
	restorseg5(seg5);
    	if (tmscp_cp_wait)
		wakeup((caddr_t)&tmscp_cp_wait);
    	(void) tmsstart(sc);
}


/*
 * Open a tmscp device and set the unit online.  If the controller is not 
 * in the run state, call init to initialize the tmscp controller first.
 */

/* ARGSUSED */
tmscpopen(dev, flag)
	dev_t dev;
	int flag;
{
	register int unit = TMSUNIT(dev);
	int ctlr = TMSCTLR(dev);
	register struct tmscp_softc *sc;
	register struct tms_info *tms;
	register struct mscp *mp;
	struct tmscpdevice *tmscpaddr;
	int s,i;
	
#ifdef TMSCP_DEBUG
	printd("tmscpopen %d,%d\n", ctlr, unit);
	if (tmscpdebug) delay(10000L);
#endif
	if (ctlr >= NTMSCP)
		return (ENXIO);
	sc = &tmscp_softc[ctlr];
	if (sc->sc_addr == NULL)
		return (ENXIO);
	if ((tms = sc->sc_drives[unit]) == NULL) {
		tms = getdd();
		if (!tms)
			return(ENXIO);
		tms->tms_online = -1;
		sc->sc_drives[unit] = tms;
	}
	if (tms->tms_openf)
		return(EBUSY);
	tms->tms_openf = 1;
	tms->tms_ttyp = u.u_ttyp;
	s = spl5();
	if (sc->sc_state != S_RUN)
		{
		if (sc->sc_state == S_IDLE)
			if(!tkini(sc))
				{
				printf("tms%d init failed\n", ctlr);
				(void) splx(s);
				return(ENXIO);
				}
		/* 
		 * Wait for initialization to complete
		 */
		timeout(wakeup,(caddr_t)&sc->sc_ctab,11*hz);	/* to be sure*/
		sleep((caddr_t)&sc->sc_ctab, PSWP+1);
		if (sc->sc_state != S_RUN)
			{
			sc->sc_drives[unit] = NULL;
			tms->tms_online = tms->tms_openf = 0;
			(void) splx(s);
			return (EIO);
			}
		}
	/*
	 * Check to see if the device is really there.
	 * this code was taken from Fred Canters 11 driver
	 */
	tmscpaddr = (struct tmscpdevice *) sc->sc_addr;
	(void) splx(s);
	if(tms->tms_online == -1)
		{
		s = spl5();
		while ((mp = tmscpgetcp(sc)) == 0)
			{
			tmscp_cp_wait++;
			sleep((caddr_t)&tmscp_cp_wait,PSWP+1);
			tmscp_cp_wait--;
			}
		(void) splx(s);
		mapseg5(tmscp[sc->sc_unit], MAPBUFDESC);
		mp->mscp_opcode = M_OP_ONLIN;
		mp->mscp_unit = unit;		/* unit? */
		mp->mscp_cmdref = (u_short)&tms->tms_type;
					    /* need to sleep on something */
#ifdef TMSCP_DEBUG
		printd("tmscpopen: bring unit %d,%d online\n",ctlr, unit);
#endif	
		((Trl *)mp->mscp_dscptr)->hsh |= TMSCP_OWN | TMSCP_INT;
		normalseg5();
		i = tmscpaddr->tmscpip;
		/* 
		 * To make sure we wake up, timeout in 240 seconds.
		 * Wakeup in tmscprsp routine.
		 * 240 seconds (4 minutes) is necessary since a rewind
		 * can take a few minutes.
		 */
		timeout(wakeup,(caddr_t) &tms->tms_type,240 * hz);
		sleep((caddr_t) &tms->tms_type,PSWP+1);
		}
	if	(tms->tms_online == -1)
		{
oops:		tms->tms_online = tms->tms_openf = 0;
		sc->sc_drives[unit] = NULL;
		return(ENXIO);  /* Didn't go online */
		}
	/*
	 * Get the unit characteristics (GTUNT).  Done here because we
	 * do not check for slave units at autoconfigure time.  This really
	 * only need be done once, but it's easier to do it on each open.
	 * tmscpcommand() is used since the iodone() handling for GTUNT has
	 * been fixed.
	*/
	tms->tms_type = 0;
	tmscpcommand(dev, TMS_SENSE, 1);
	if	(tms->tms_type == 0)
		goto oops;
	tms->tms_lastiow = 0;
	/*
	 * Set unit density using the density matrix.
	 * This is done as an "internal" ioctl command so
	 * that the command setup and response handling
	 * is done thru "regular" command routines.
	 */
	tmscpcommand(dev, TMS_SETDENSITY, 1);
	return (0);
}

/*
 * Close tape device.
 *
 * If tape was open for writing or last operation was
 * a write, then write two EOF's and backspace over the last one.
 * Unless this is a non-rewinding special file, rewind the tape.
 *
 * NOTE:
 *	We want to be sure that any serious exception is cleared on the
 *	close. A Clear Serious Exception (CSE) modifier is always done on
 *	the rewind command.  For the non-rewind case we check to see if the
 *	"serex" field is set in the softc struct; if it is then issue a noop
 *	command with the CSE modifier.
 * Make the tape available to others, by clearing openf flag.
 */
tmscpclose(dev, flag)
	register dev_t dev;
	register flag;
{
	struct tmscp_softc *sc;
	register struct tms_info *tms;
	int unit = TMSUNIT(dev);

#ifdef TMSCP_DEBUG
	printd("tmscpclose: %d,%d\n", TMSCTLR(dev), unit);
	if(tmscpdebug)delay(10000L);
#endif
	sc = &tmscp_softc[TMSCTLR(dev)];
	tms = sc->sc_drives[unit];
	if (flag == FWRITE || (flag&FWRITE) && tms->tms_lastiow)
		{
		/*	   device, command, count */
		tmscpcommand (dev, TMS_WRITM, 1);
		tmscpcommand (dev, TMS_WRITM, 1);
		tmscpcommand (dev, TMS_BSR, 1);
		}
	if ((minor(dev)&T_NOREWIND) == 0)
		/*
		 * Don't hang waiting for rewind complete.
		 */
		tmscpcommand(dev, TMS_REW, 0);
	else
		if (tms->tms_serex)
			{
#ifdef TMSCP_DEBUG
			printd("tmscpclose: clearing serex\n");
			if (tmscpdebug)delay(10000L);
#endif
			tmscpcommand(dev, TMS_CSE, 1);
			}
	tms->tms_openf = 0;
}

/*
 * Execute a command on the tape drive a specified number of times.
 * This routine sets up a buffer and calls the strategy routine which
 * links the buffer onto the drive's buffer queue.
 * The start routine will take care of creating a tmscp command packet
 * with the command.  The start routine is called by the strategy or the
 * interrupt routine.
 */

tmscpcommand(dev, com, count)
	register dev_t dev;
	int com, count;
{
	register struct buf *bp;
	register int s;
	int unit = TMSUNIT(dev);

	bp = &tmscp_softc[TMSCTLR(dev)].sc_cmdbuf;

	s = spl5();
	while (bp->b_flags&B_BUSY)
		{
		/*
		 * This special check is because B_BUSY never
		 * gets cleared in the non-waiting rewind case.
		 */
		if (bp->b_bcount == 0 && (bp->b_flags&B_DONE))
			break;
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO);
		}
	bp->b_flags = B_BUSY|B_READ;
	splx(s);
	/*
	 * Load the buffer.  The b_count field gets used to hold the command
	 * count.  the b_resid field gets used to hold the command mneumonic.
	 * These 2 fields are "known" to be "safe" to use for this purpose.
	 * (Most other drivers also use these fields in this way.)
	 */
	bp->b_dev = dev;
	bp->b_bcount = count;
	bp->b_resid = com;
	bp->b_blkno = 0;
	tmscpstrategy(bp);
	/*
	 * In case of rewind from close, don't wait.
	 * This is the only case where count can be 0.
	 */
	if (count == 0)
		return;
	iowait(bp);
	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	bp->b_flags &= B_ERROR;
}

/*
 * Init mscp communications area
 */
tmsginit(sc, com, msgs, offset, length, flags)
	register struct tmscp_softc	*sc;
	register	Trl		*com;
	register struct mscp		*msgs;
	int				offset;
	int				length;
	int				flags;
{
	long				vaddr;

	/* 
	 * Figure out virtual address of message 
	 * skip comm area and mscp messages header and previous messages
	 *
	 * N.B. Assumes SEG5 has been remapped to the comm area for this
	 * controller.
	 */
	vaddr = _iomap(tmscp[sc->sc_unit]);
	vaddr += sizeof(struct tmscpca)			/* skip comm area */
		+sizeof(struct mscp_header);		/* m_cmdref disp */
	vaddr += offset * sizeof(struct mscp);		/* skip previous */
	while (length--) {
		com->lsh = loint(vaddr);
		com->hsh = flags | hiint(vaddr);
		msgs->mscp_dscptr = (long *)com;
		msgs->mscp_header.tmscp_msglen = sizeof(struct mscp);
		msgs->mscp_header.tmscp_vcid = 1; /* tape VCID = 1 */
		++com; ++msgs; vaddr += sizeof(struct mscp);
	}
}

/*
 * Find an unused command packet
 */
struct mscp *
tmscpgetcp(sc)
	register struct tmscp_softc *sc;
{
	register struct mscp *mp = NULL;
	struct tmscpca *cp;
	register int i;
	int	s;
	segm seg5;

	s = spl5();
	saveseg5(seg5);
	mapseg5(tmscp[sc->sc_unit], MAPBUFDESC);
	cp = &sc->sc_com->tmscp_ca;
	/*
	 * If no credits, can't issue any commands
	 * until some outstanding commands complete.
	 */
	i = sc->sc_lastcmd;
#ifdef TMSCP_DEBUG
	printd10("tmscpgetcp: %d credits remain\n", sc->sc_credits);
#endif
	if(((cp->ca_cmddsc[i].hsh&(TMSCP_OWN|TMSCP_INT))==TMSCP_INT) &&
	  (sc->sc_credits >= 2))
		{
		sc->sc_credits--;       /* This commits to issuing a command */
		cp->ca_cmddsc[i].hsh &= ~TMSCP_INT;
		mp = &sc->sc_com->tmscp_cmd[i];
		mp->mscp_unit = mp->mscp_modifier = 0;
		mp->mscp_opcode = mp->mscp_flags = 0;
		mp->mscp_bytecnt = 0;
		mp->mscp_buffer_h = mp->mscp_buffer_l = 0;
		mp->mscp_zzz2 = 0;
		sc->sc_lastcmd = (i + 1) % NCMD;
		}
	restorseg5(seg5);
	(void) splx(s);
	return(mp);
}

/*
 * Initialize a TMSCP device.  Set up UBA mapping registers,
 * initialize data structures, and start hardware
 * initialization sequence.
 */
tkini(sc)
	register struct tmscp_softc *sc;
{
	register struct tmscpdevice *tmscpaddr;
	long adr;

	sc->sc_ctab.b_active++;
	adr = _iomap(tmscp[sc->sc_unit]) + (u_int)RINGBASE;
	sc->sc_ctab.b_un.b_addr = (caddr_t)loint(adr);
	sc->sc_ctab.b_xmem = hiint(adr);
	tmscpaddr = sc->sc_addr;

	/*
	 * Start the hardware initialization sequence.
	 */
	tmscpaddr->tmscpip = 0;              /* start initialization */

	while((tmscpaddr->tmscpsa & TMSCP_STEP1) == 0)
		{
#ifdef TMSCP_DEBUG
		printd("tkini: tmscpsa = 0%o\n",tmscpaddr->tmscpsa);
		delay(100000L);
#endif
		if(tmscpaddr->tmscpsa & TMSCP_ERR)
			return(0);	/* CHECK */
		}
	tmscpaddr->tmscpsa=TMSCP_ERR|(NCMDL2<<11)|(NRSPL2<<8)|TMSCP_IE|(sc->sc_ivec/4);
	/*
	 * Initialization continues in the interrupt routine.
	 */
	sc->sc_state = S_STEP1;
	sc->sc_credits = 0;
	return(1);
}

/*
 * Start I/O operation
 * This code is convoluted.  The majority of it was copied from the uda driver.
 */

tmsstart(sc)
	register struct tmscp_softc *sc;
{
	register struct mscp *mp;
	register struct buf *bp, *dp;
	register struct tms_info *tms;
	struct   tmscpdevice *tmscpaddr;
	int i, unit;
	segm seg5;

     saveseg5(seg5);		/* save just once at top */
     for(;;)
	{
	if ((dp = sc->sc_ctab.b_actf) == NULL)
		{
		/*
		 * Release unneeded UBA resources and return
		 * (drive was inactive)
		 */
		sc->sc_ctab.b_active = 0;
		break;
		}
	if ((bp = dp->b_actf) == NULL)
		{
		/*
		 * No more requests for this drive, remove
		 * from controller queue and look at next drive.
		 * We know we're at the head of the controller queue.
		 */
		dp->b_active = 0;
		sc->sc_ctab.b_actf = dp->b_forw;
		continue;		/* Need to check for loop */
		}
	sc->sc_ctab.b_active++;
	unit = TMSUNIT(bp->b_dev);
	tmscpaddr = (struct tmscpdevice *)sc->sc_addr;
	tms = sc->sc_drives[unit];
	if ((tmscpaddr->tmscpsa&TMSCP_ERR) || sc->sc_state != S_RUN)
		{
		tprintf(tms->tms_ttyp,
		    "tms%d,%d: hard error bn%ld\n", sc->sc_unit, unit,
		    bp->b_blkno);
		log(TMS_PRI, "tms%d,%d: sa 0%o state %d\n", sc->sc_unit,
			unit, tmscpaddr->tmscpsa, sc->sc_state);
		(void)tkini(sc);
		/* SHOULD REQUEUE OUTSTANDING REQUESTS, LIKE TMSCPRESET */
		break;
		}
	/*
	 * Default is that last command was NOT a write command;
	 * if a write command is done it will be detected in tmscprsp.
	 */
	tms->tms_lastiow = 0;
	if (tms->tms_online == -1)
		{        /* not online */
		if ((mp = tmscpgetcp(sc)) == NULL)
			break;
		mapseg5(tmscp[sc->sc_unit], MAPBUFDESC);
		mp->mscp_opcode = M_OP_ONLIN;
		mp->mscp_unit = unit;
		dp->b_active = 2;
		sc->sc_ctab.b_actf = dp->b_forw; /* remove from controller q */
		((Trl *)mp->mscp_dscptr)->hsh |= TMSCP_OWN|TMSCP_INT;
		if (tmscpaddr->tmscpsa&TMSCP_ERR)
			printf(tmscpfatalerr, sc->sc_unit,
					TMSUNIT(bp->b_dev), tmscpaddr->tmscpsa);
		restorseg5(seg5);
		i = tmscpaddr->tmscpip;
		continue;
		}
	if ((mp = tmscpgetcp(sc)) == NULL)
		break;
	mapseg5(tmscp[sc->sc_unit], MAPBUFDESC);
	mp->mscp_cmdref = (u_short)bp;		/* pointer to get back */
	mp->mscp_unit = unit;
	/*
	 * If its an ioctl-type command then set up the appropriate
	 * tmscp command;  by doing a switch on the "b_resid" field where
	 * the command mneumonic is stored.
	 */
	if (bp == &sc->sc_cmdbuf)
		{
#ifdef TMSCP_DEBUG
		printd("tmsstart: doing ioctl cmd %d\n", bp->b_resid);
#endif
		/*
		 * The reccnt and tmkcnt fields are set to zero by the getcp
		 * routine (as bytecnt and buffer fields).  Thus reccnt and
		 * tmkcnt are only modified here if they need to be set to
		 * a non-zero value.
		 */
		switch ((int)bp->b_resid) {

		case TMS_WRITM:
			mp->mscp_opcode = M_OP_WRITM;
			break;
		case TMS_FSF:
			mp->mscp_opcode = M_OP_REPOS;
			mp->mscp_tmkcnt = bp->b_bcount;
			break;
		case TMS_BSF:
			mp->mscp_opcode = M_OP_REPOS;
			mp->mscp_modifier = M_MD_REVRS;
			mp->mscp_tmkcnt = bp->b_bcount;
			break;
		case TMS_FSR:
			mp->mscp_opcode = M_OP_REPOS;
			mp->mscp_modifier = M_MD_OBJCT;
			mp->mscp_reccnt = bp->b_bcount;
			break;
		case TMS_BSR:
			mp->mscp_opcode = M_OP_REPOS;
			mp->mscp_modifier = M_MD_REVRS | M_MD_OBJCT;
			mp->mscp_reccnt = bp->b_bcount;
			break;
		/*
		 * Clear serious exception is done for Rewind & Available cmds
		 */
		case TMS_REW:
			mp->mscp_opcode = M_OP_REPOS;
			mp->mscp_modifier = M_MD_REWND | M_MD_CLSEX;
			if (bp->b_bcount == 0)
				mp->mscp_modifier |= M_MD_IMMED;
			tms->tms_serex = 0;
			break;
		case TMS_OFFL:
			mp->mscp_opcode = M_OP_AVAIL;
			mp->mscp_modifier = M_MD_UNLOD | M_MD_CLSEX;
			tms->tms_serex = 0;
			break;
		case TMS_SENSE:
			mp->mscp_opcode = M_OP_GTUNT;
			break;
		case TMS_CACHE:
			mp->mscp_opcode = M_OP_STUNT;
			tms->tms_unitflgs |= M_UF_WBKNV;
			mp->mscp_unitflgs = tms->tms_unitflgs;
			mp->mscp_format = tms->tms_format;
			/* default device dependant parameters */
			mp->mscp_mediaid = 0;
			break;
		case TMS_NOCACHE:
			mp->mscp_opcode = M_OP_STUNT;
			tms->tms_unitflgs &= ~(M_UF_WBKNV);
			mp->mscp_unitflgs = tms->tms_unitflgs;
			mp->mscp_format = tms->tms_format;
			/* default device dependant parameters */
			mp->mscp_mediaid = 0;
			break;
		case TMS_CSE:
			/*
			 * This is a no-op command. It performs a 
			 * clear serious exception only.  (Done on a
			 * non-rewinding close after a serious exception.)
			 */
			mp->mscp_opcode = M_OP_REPOS;
			mp->mscp_modifier = M_MD_CLSEX;
			tms->tms_serex = 0;
			tms->tms_clserex = 1;
			break;
		case TMS_SETDENSITY:
			/*
			 * Set the unit density
			 */
			mp->mscp_opcode = M_OP_STUNT;
			mp->mscp_unitflgs = tms->tms_unitflgs;
			mp->mscp_mediaid = 0;	/* default device dependant parameters */
			mp->mscp_format = Dmatrix[TMSDENS(bp->b_dev)][tms->tms_fmtmenu & FMTMASK];
#ifdef	TMSCP_DEBUG
			printd("b_dev: 0%o fmtmenu: 0%o format: 0%o\n", bp->b_dev, tms->tms_fmtmenu, mp->mscp_format);
			delay(4000000L);
#endif
			tms->tms_format = mp->mscp_format;
			break;
		default:
			printf("tms%d,%d bad ioctl\n",sc->sc_unit,mp->mscp_unit);
			/* Need a no-op. Reposition no amount */
			mp->mscp_opcode = M_OP_REPOS;
			break;
		}   /* end switch (bp->b_resid) */
		}
	else    /* Its a read/write command (not an ioctl) */
		{
		mp->mscp_opcode = bp->b_flags&B_READ ? M_OP_READ : M_OP_WRITE;
		mp->mscp_bytecnt = bp->b_bcount;
		mp->mscp_buffer_l = (u_short) bp->b_un.b_addr;
		mp->mscp_buffer_h = bp->b_xmem;
		}
	if (tms->tms_serex == 2)			/* if tape mark read */
		{
		mp->mscp_modifier |= M_MD_CLSEX;	/*  clear serious exc */
		tms->tms_serex = 0;
		}
	((Trl *)mp->mscp_dscptr)->hsh |= TMSCP_OWN|TMSCP_INT;
#ifdef TMSCP_DEBUG
	printd("tmsstart: opcode 0%o mod %o unit %d cnt %d\n",mp->mscp_opcode,mp->mscp_modifier,mp->mscp_unit,mp->mscp_bytecnt);
	if(tmscpdebug)delay(100000L);
#endif
	i = tmscpaddr->tmscpip;              /* initiate polling */
	dp->b_qsize++;
	/*
	 * Move drive to the end of the controller queue
	 */
	if (dp->b_forw != NULL)
		{
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
	if (tmscpaddr->tmscpsa&TMSCP_ERR)
		{
		printf(tmscpfatalerr,sc->sc_unit, 
			mp->mscp_unit, tmscpaddr->tmscpsa);
		(void)tkini(sc);
		break;
		}
    }   /* end for */
    /*
     * Check for response ring transitions lost in the
     * Race condition.  Map SEG5 in case we escaped early from the for().
     */
    mapseg5(tmscp[sc->sc_unit], MAPBUFDESC);
    for (i = sc->sc_lastrsp;; i++)
	    {
	    i %= NRSP;
	    if (sc->sc_com->tmscp_ca.ca_rspdsc[i].hsh&TMSCP_OWN)
		    break;
	    tmscprsp(sc, i);
	    sc->sc_com->tmscp_ca.ca_rspdsc[i].hsh |= TMSCP_OWN;
	    }
    sc->sc_lastrsp = i;
    restorseg5(seg5);
}


/*
 * Process a response packet.  N.B.  Assumes SEG5 maps comm area for controller
 */
tmscprsp(sc, i)
	register struct tmscp_softc *sc;
	int i;
{
	register struct mscp *mp;
	register struct tms_info *tms;
	struct buf *dp, *bp;
	int st;

	mp = &sc->sc_com->tmscp_rsp[i];
	mp->mscp_header.tmscp_msglen = mscp_msglen;
	sc->sc_credits += mp->mscp_header.tmscp_credits & 0xf;  /* low 4 bits */
	if ((mp->mscp_header.tmscp_credits & 0xf0) > 0x10)	/* Check */
		return;
#ifdef TMSCP_DEBUG
	printd("tmscprsp, opcode 0%o status 0%o\n",mp->mscp_opcode,mp->mscp_status&M_ST_MASK);
	printd("  SEG5: 0%o sc: 0%o mp: 0%o i: %d\n",*KDSA5, sc, mp, i);
#endif	
	/*
	 * If it's an error log message (datagram),
	 * pass it on for more extensive processing.
	 */
	if ((mp->mscp_header.tmscp_credits & 0xf0) == 0x10)
		{	/* check */
		tmserror(sc->sc_unit, (struct mslg *)mp);
		return;
		}
	st = mp->mscp_status&M_ST_MASK;
	/*
	 * The controller interrupts as drive 0.
	 * This means that you must check for controller interrupts
	 * before you check to see if there is a drive 0.
	 */
	if((M_OP_STCON|M_OP_END) == mp->mscp_opcode)
		{
		if (st == M_ST_SUCC)
			{
#ifdef TMSCP_DEBUG
			printd("ctlr has %d credits\n", mp->mscp_header.tmscp_credits & 0xf);
			printd("ctlr timeout = %d\n", mp->mscp_cnttmo);
#endif
			sc->sc_state = S_RUN;
			}
		else
			sc->sc_state = S_IDLE;
		sc->sc_ctab.b_active = 0;
		wakeup((caddr_t)&sc->sc_ctab);
		return;
		}
	if (mp->mscp_unit >= 4)
		return;
	tms = sc->sc_drives[mp->mscp_unit];
	if (!tms)		/* unopened unit coming online - ignore it */
		return;
	/*
	 * Save endcode, endflags, and status for mtioctl get unit status.
	 * NOTE: Don't do this on Clear serious exception (reposition no-op);
	 *    which is done on close since this would
	 *    overwrite the real status we want.
	 */
	if (tms->tms_clserex != 1)
		{
		tms->tms_endcode = mp->mscp_opcode;
		tms->tms_flags = mp->mscp_flags;
		tms->tms_status = st;
		}
	else tms->tms_clserex = 0;

	switch (mp->mscp_opcode) {
	case M_OP_ONLIN|M_OP_END:
		tms->tms_type = mp->mscp_mediaid;
		dp = &tms->tms_dtab;
		if (st == M_ST_SUCC)
			{
			/*
			 * Link the drive onto the controller queue
			 */
			dp->b_forw = NULL;
			if (sc->sc_ctab.b_actf == NULL)
				sc->sc_ctab.b_actf = dp;
			else
				sc->sc_ctab.b_actl->b_forw = dp;
			sc->sc_ctab.b_actl = dp;
			tms->tms_online = 1;       /* mark it online */
			tms->tms_dsize=(daddr_t)mp->mscp_maxwrt;
			/* 
			 * This define decodes the Media type identifier
			 */
#ifdef TMSCP_DEBUG
			printd("tmscprsp: %d,%d online mediaid 0x%lx format 0x%x\n",
				sc->sc_unit, mp->mscp_unit, mp->mscp_mediaid,
				mp->mscp_format);
#endif				
			dp->b_active = 1;
			}	/* end if st == M_ST_SUCC */
		else 
			{
			if (bp = dp->b_actf)
				tprintf(tms->tms_ttyp,
				    "tms%d,%d: hard error bn%ld: OFFLINE\n",
				    sc->sc_unit, mp->mscp_unit, bp->b_blkno);
			else
				tprintf(tms->tms_ttyp,
				    "tms%d,%d: hard error: OFFLINE\n", 
				    sc->sc_unit, mp->mscp_unit);
			while (bp = dp->b_actf)
				{
				dp->b_actf = bp->av_forw;
				bp->b_flags |= B_ERROR;
				iodone(bp);
				}
			}
		if(mp->mscp_cmdref!=NULL)
			/* Seems to get lost sometimes in uda */
			wakeup((caddr_t)mp->mscp_cmdref);
		break;
	/*
	 * The AVAILABLE ATTENTION message occurs when the
	 * unit becomes available after loading,
	 * marking the unit offline (tms_online = -1) will force an
	 * online command prior to using the unit.
	 */
	case M_OP_AVATN:
		tms->tms_online = -1;
		tms->tms_type = mp->mscp_mediaid;
		break;
	case M_OP_END:
		/*
		 * An endcode without an opcode (0200) is an invalid command.
		 * The mscp specification states that this would be a protocol
		 * type error, such as illegal opcodes. The mscp spec. also
		 * states that parameter error type of invalid commands should
		 * return the normal end message for the command. This does not appear
		 * to be the case. An invalid logical block number returned an endcode
		 * of 0200 instead of the 0241 (read) that was expected.
		 */
			
		printf("tms%d,%d: invalid cmd endcode = %o status=%o\n",
			sc->sc_unit, mp->mscp_unit, mp->mscp_opcode, st);
common:	/* GTUNT finishes up thru here too */
		bp = (struct buf *)mp->mscp_cmdref;
		/*
		 * Unlink buffer from I/O wait queue.
		 * And signal iodone, so the higher level command can exit!
		 *
		 */
		bp->av_back->av_forw = bp->av_forw;
		bp->av_forw->av_back = bp->av_back;
		dp = &tms->tms_dtab;
		dp->b_qsize--;
		iodone(bp);
		break;
	case M_OP_WRITE|M_OP_END:
		/* mark the last io op as a write */
		tms->tms_lastiow = 1;
	case M_OP_READ|M_OP_END:
	case M_OP_WRITM|M_OP_END:
	case M_OP_REPOS|M_OP_END:
	case M_OP_STUNT|M_OP_END:
	/*
	 * The AVAILABLE message occurs when the mt ioctl "rewoffl" is
	 * issued.  For the ioctl, "rewoffl", a tmscp AVAILABLE command is
	 * done with the UNLOAD modifier.  This performs a rewind, followed
	 * by marking the unit offline.  So mark the unit offline
	 * software wise as well (tms->tms_online = -1 and tms_open = 0).
	 */
	case M_OP_AVAIL|M_OP_END:
#ifdef TMSCP_DEBUG
		printd("tmscprsp: position = %u,%u\n", mp->mscp_lbn_h,mp->mscp_lbn_l);
#endif
		bp = (struct buf *)mp->mscp_cmdref;
		/*
		 * Unlink buffer from I/O wait queue.
		 */
		bp->av_back->av_forw = bp->av_forw;
		bp->av_forw->av_back = bp->av_back;
		dp = &tms->tms_dtab;
		dp->b_qsize--;
		if (st == M_ST_OFFLN || st == M_ST_AVLBL)
			{
			tms->tms_online = -1;       /* mark unit offline */
			tms->tms_openf = 0;
			tms->tms_type = mp->mscp_mediaid;
			/*
			 * Link the buffer onto the front of the drive queue
			 */
			if ((bp->av_forw = dp->b_actf) == 0)
				dp->b_actl = bp;
			dp->b_actf = bp;
			/*
			 * Link the drive onto the controller queue
			 */
			if (dp->b_active == 0)
				{
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
		if (st != M_ST_SUCC)
			{
			if (mp->mscp_flags & M_EF_SEREX)
				tms->tms_serex = 1;
			if (st != M_ST_TAPEM)
				{
				tprintf(tms->tms_ttyp,
				"tms%d,%d: hard err bn%ld status:0%o flags:0%o\n",
				    sc->sc_unit, mp->mscp_unit, bp->b_blkno,
				    mp->mscp_status, mp->mscp_flags);
				bp->b_flags |= B_ERROR;
				}
			else
				/* Hit a tape mark - Set serex flag to
				 * a special value so we can clear the
				 * serious exception on the next command.
				 */
				tms->tms_serex = 2;
			}
		/*
		 * The tmscp spec states that controllers do not have to
		 * report the number of records or files skipped.  So on
		 * reposition commands we go strictly by cmd status.
		 */
		if (mp->mscp_opcode != (M_OP_REPOS|M_OP_END))
			bp->b_resid = bp->b_bcount - mp->mscp_bytecnt;
		else
			bp->b_resid = 0;
		tms->tms_resid = bp->b_resid;
		iodone(bp);
		break;

	case M_OP_GTUNT|M_OP_END:
#ifdef TMSCP_DEBUG
		printd("tmscprsp: GTUNT end packet status = 0%o\n",st);
		printd("tmscprsp: %d,%d mediaid 0x%lx 0x%lx 0x%lx format=%d speed = %d\n",
		    sc->sc_unit,mp->mscp_unit, mp->mscp_mediaid
		    ,mp->mscp_unitid.val[0]
		    ,mp->mscp_unitid.val[1]
		    ,mp->mscp_format, mp->mscp_speed);
#endif
		tms->tms_type = mp->mscp_mediaid;
		tms->tms_fmtmenu = mp->mscp_fmtmenu;
		tms->tms_unitflgs = mp->mscp_unitflgs;
		goto common;	/* need to dequeue buffer and do iodone() */

	default:
		printf("tms%d,%d unknown packet\n", sc->sc_unit,mp->mscp_unit);
		tmserror(sc->sc_unit, (struct mslg *)mp);
	}	/* end switch mp->mscp_opcode */
}

/*
 * Manage buffers and perform block mode read and write operations.
 */

tmscpstrategy (bp)
	register struct buf *bp;
{
	register struct buf *dp;
	register struct tmscp_softc *sc;
	struct	tms_info *tms;
	int ctlr = TMSCTLR(bp->b_dev);
	int s;

	sc = &tmscp_softc[ctlr];
	tms = sc->sc_drives[TMSUNIT(bp->b_dev)];
	if (!tms || tms->tms_online != 1)
		{
		bp->b_flags |= B_ERROR;
		iodone(bp);
		return;
		}
	mapalloc(bp);
	s = spl5();
	/*
	 * Link the buffer onto the drive queue
	 */
	dp = &tms->tms_dtab;
	if (dp->b_actf == 0)
		dp->b_actf = bp;
	else
		dp->b_actl->av_forw = bp;
	dp->b_actl = bp;
	bp->av_forw = 0;
	/*
	 * Link the drive onto the controller queue
	 */
	if (dp->b_active == 0)
		{
		dp->b_forw = NULL;
		if (sc->sc_ctab.b_actf == NULL)
			sc->sc_ctab.b_actf = dp;
		else
			sc->sc_ctab.b_actl->b_forw = dp;
		sc->sc_ctab.b_actl = dp;
		dp->b_active = 1;
		}
	/*
	 * If the controller is not active, start it.
	 */
	if (sc->sc_ctab.b_active == 0)
		{
#ifdef TMSCP_DEBUG
		printd10("tmscpstrategy: Controller not active, starting it\n");
#endif
		(void) tmsstart(sc);
		}
	splx(s);
	return;
}

#ifdef	TMSCP_DUMP
#define DBSIZE 16

tmsdump(dev)
	dev_t dev;
{
	register struct tmscpdevice *tmscpaddr;
	register struct	tmscp_softc *sc;
	register struct	mscp	*mp;
	daddr_t bn, dumpsize;
	long paddr, maddr;
	int unit = TMSUNIT(dev), count, ctlr = TMSCTLR(dev);
	struct	ubmap *ubp;
	segm seg5;

	if (ctlr >= NTMSCP)
		return (ENXIO);
	sc = &tmscp_softc[ctlr];
	tmscpaddr = sc->sc_addr;
	if (tmscpaddr == NULL)
		return(ENXIO);

	paddr = _iomap(tmscp[sc->sc_unit]);
	if (ubmap) {
		ubp = UBMAP;
		ubp->ub_lo = loint(paddr);
		ubp->ub_hi = hiint(paddr);
	}
	paddr += RINGBASE;
	saveseg5(seg5);
	mapseg5(tmscp[sc->sc_unit], MAPBUFDESC);
	mp = sc->sc_com->tmscp_rsp;
	sc->sc_com->tmscp_ca.ca_cmdint = sc->sc_com->tmscp_ca.ca_rspint = 0;
	bzero(mp, 2 * sizeof (*mp));

	tmscpaddr->tmscpip = 0;
	while ((tmscpaddr->tmscpsa & TMSCP_STEP1) == 0)
		if(tmscpaddr->tmscpsa & TMSCP_ERR) return(EFAULT);
	tmscpaddr->tmscpsa = TMSCP_ERR;
	while ((tmscpaddr->tmscpsa & TMSCP_STEP2) == 0)
		if(tmscpaddr->tmscpsa & TMSCP_ERR) return(EFAULT);
	tmscpaddr->tmscpsa = loint(paddr);
	while ((tmscpaddr->tmscpsa & TMSCP_STEP3) == 0)
		if(tmscpaddr->tmscpsa & TMSCP_ERR) return(EFAULT);
	tmscpaddr->tmscpsa = hiint(paddr);
	while ((tmscpaddr->tmscpsa & TMSCP_STEP4) == 0)
		if(tmscpaddr->tmscpsa & TMSCP_ERR) return(EFAULT);
	tmscpaddr->tmscpsa = TMSCP_GO;
	
	tmsginit(sc, sc->sc_com->tmscp_ca.ca_rspdsc, mp, 0, 2, 0);

	if (tmscpcmd(M_OP_STCON, unit, sc) == 0) {
		return(EFAULT);
	}
	sc->sc_com->tmscp_cmd[0].mscp_unit = unit;
	if (tmscpcmd(M_OP_ONLIN, unit, sc) == 0) {
		return(EFAULT);
	}

	dumpsize = 8 * 1024L;	 /* XXX */
	ubp = &UBMAP[1];
	for (paddr = 0; dumpsize > 0; dumpsize -= count) {
		count = MIN(dumpsize, DBSIZE);
		bn = paddr >> PGSHIFT;
		maddr = paddr;
		if (ubmap) {
			ubp->ub_lo = loint(paddr);
			ubp->ub_hi = hiint(paddr);
			maddr = (u_int)(1 << 13);
		}
	/* write it to the tape */
	mp = &sc->sc_com->tmscp_rsp[1];
	mp->mscp_lbn_l = loint(bn);
	mp->mscp_lbn_h = hiint(bn);
	mp->mscp_bytecnt = count * NBPG;
	mp->mscp_buffer_l = loint(maddr);
	mp->mscp_buffer_h = hiint(maddr);
	if (tmscpcmd(M_OP_WRITE, unit, sc) == 0)
		return(EIO);
	paddr += (DBSIZE << PGSHIFT);
	}
	restorseg5(seg5);
	return (0);
}

/*
 * Perform a standalone tmscp command.  This routine is only used by tmscpdump.
 */

tmscpcmd(op, unit, sc)
	int op;
	int unit;
	register struct tmscp_softc *sc;
{
	int i;
	register struct mscp *cmp, *rmp;
	Trl *rlp;

	cmp = &sc->sc_com->tmscp_rsp[1];
	rmp = &sc->sc_com->tmscp_rsp[0];
	rlp = &sc->sc_com->tmscp_ca.ca_rspdsc[0];

	cmp->mscp_opcode = op;
	cmp->mscp_unit = unit;
	cmp->mscp_header.tmscp_msglen = mscp_msglen;
	rmp->mscp_header.tmscp_msglen = mscp_msglen;
	rlp[0].hsh |= TMSCP_OWN|TMSCP_INT;
	rlp[1].hsh |= TMSCP_OWN|TMSCP_INT;
	if (sc->sc_addr->tmscpsa&TMSCP_ERR)
		printf(tmscpfatalerr, sc->sc_unit, unit, sc->sc_addr->tmscpsa);
	i = sc->sc_addr->tmscpip;

	while ((rlp[1].hsh & TMSCP_INT) == 0)
		;
	while ((rlp[0].hsh & TMSCP_INT) == 0)
		;

	sc->sc_com->tmscp_ca.ca_rspint = 0;
	sc->sc_com->tmscp_ca.ca_cmdint = 0;
	if (rmp->mscp_opcode != (op|M_OP_END) ||
	    (rmp->mscp_status&M_ST_MASK) != M_ST_SUCC)
		{
		printf("err: com %d opc 0x%x stat 0x%x\ndump ", op,
			rmp->mscp_opcode, rmp->mscp_status);
		return(0);
		}
	return(1);
}
#endif TMSCP_DUMP

/*
 * Catch ioctl commands, and call the "command" routine to do them.
 */

/* ARGSUSED */
tmscpioctl(dev, cmd, data, flag)
	dev_t dev;
	int cmd;
	caddr_t data;
	int flag;
{
	struct tmscp_softc *sc = &tmscp_softc[TMSCTLR(dev)];
	register struct buf *bp = &sc->sc_cmdbuf;
	register callcount;	/* number of times to call cmd routine */
	register struct tms_info *tms;
	int fcount;		/* number of files (or records) to space */
	register struct mtop *mtop;	/* mag tape cmd op to perform */
	register struct mtget *mtget;	/* mag tape struct to get info in */

	/* we depend of the values and order of the TMS ioctl codes here */
	static u_char tmsops[] =
	 {TMS_WRITM,TMS_FSF,TMS_BSF,TMS_FSR,TMS_BSR,TMS_REW,TMS_OFFL,TMS_SENSE,
	  TMS_CACHE,TMS_NOCACHE};

	switch (cmd) {
	case MTIOCTOP:	/* tape operation */
		mtop = (struct mtop *)data;
		switch (mtop->mt_op) {

		case MTWEOF:
			callcount = mtop->mt_count;
			fcount = 1;
			break;
		case MTFSF: case MTBSF:
		case MTFSR: case MTBSR:
			callcount = 1;
			fcount = mtop->mt_count;
			break;
		case MTREW: case MTOFFL: case MTNOP:
		case MTCACHE: case MTNOCACHE:
			callcount = 1;
			fcount = 1;		/* wait for this rewind */
			break;
		default:
			return (ENXIO);
		}	/* end switch mtop->mt_op */

		if (callcount <= 0 || fcount <= 0)
			return (EINVAL);
		while (--callcount >= 0)
			{
			tmscpcommand(dev, tmsops[mtop->mt_op], fcount);
			if ((mtop->mt_op == MTFSR || mtop->mt_op == MTBSR) &&
			    bp->b_resid)
				return (EIO);
			if (bp->b_flags & B_ERROR)	/* like hitting BOT */
				break;
			}
		return (geterror(bp));

	case MTIOCGET:
		/*
		 * Return status info associated with the particular UNIT.
		 */
		tms = sc->sc_drives[TMSUNIT(dev)];
		if (!tms)
			return(ENXIO);
		mtget = (struct mtget *)data;
		mtget->mt_type = MT_ISTMSCP;
		mtget->mt_dsreg = tms->tms_flags << 8;
		mtget->mt_dsreg |= tms->tms_endcode;
		mtget->mt_erreg = tms->tms_status;
		mtget->mt_resid = tms->tms_resid;
		break;

	default:
		return (ENXIO);
	}
	return (0);
}

/*
 * Process an error log message
 *
 * Only minimal decoding is done, only "useful"
 * information is printed.  Eventually should
 * send message to an error logger.
 */

tmserror(ctlr, mp)
	register int ctlr;
	register struct mslg *mp;
{
	register i;

#ifdef TMSCP_DEBUG
	printd("tmserror:\n");
#endif
	if(!(mp->mslg_flags & (M_LF_SUCC | M_LF_CONT)))
		log(TMS_PRI, "tms%d,%d: %s err ", ctlr, mp->mslg_unit,
		mp->mslg_flags & ( M_LF_SUCC | M_LF_CONT ) ? "soft" : "hard");

	switch (mp->mslg_format) {

	case M_FM_CNTERR:
		log(TMS_PRI, "ctlr err event 0%o\n", mp->mslg_event);
		break;
	case M_FM_BUSADDR:
		log(TMS_PRI, "host memory access err, event 0%o, addr 0%o\n",
			mp->mslg_event, mp->mslg_busaddr);
		break;
	case M_FM_TAPETRN:
		log(TMS_PRI, "tape transfer err unit %d grp 0x%x event 0%o\n",
			mp->mslg_unit, mp->mslg_group, mp->mslg_event);
		break;
	case M_FM_STIERR:
		log(TMS_PRI, "STI err unit %d event 0%o\n",
			mp->mslg_unit, mp->mslg_event);
#ifdef notdef
		/* too painful to do with log() */
		for(i = 0; i < 62;i++)
			mprintf("\t0x%x",mp->mslg_stiunsucc[i] & 0xff);
		mprintf("\n");
#endif
		break;
	case M_FM_STIDEL:
		log(TMS_PRI, "STI Drive ErrorLog unit %d event 0%o\n",
			mp->mslg_unit, mp->mslg_event);
		break;
	case M_FM_STIFEL:
		log(TMS_PRI, "STI Formatter ErrorLog unit %d event 0%o\n",
			mp->mslg_unit, mp->mslg_event);
		break;
	default:
		log(TMS_PRI, "unknown err %d,%d format 0%o event 0%o\n",
			ctlr,mp->mslg_unit, mp->mslg_format, mp->mslg_event);
	}

	if (tmscperror)
		{
		register long *p = (long *)mp;

		for (i = 0; i < mp->mslg_header.tmscp_msglen; i += sizeof(*p))
			printf("%x ", *p++);
		printf("\n");
		}
}
#endif NTMSCP
