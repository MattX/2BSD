/*
 *	SCCS id	%W% (Berkeley)	%G%
 */

#include	"param.h"
#include	<sys/systm.h>
#include	<sys/buf.h>
#include	<sys/tty.h>
#include	<sys/conf.h>
#include	<sys/proc.h>
#include	<sys/text.h>
#include	<sys/dir.h>
#include	<sys/user.h>
#include	<sys/file.h>
#include	<sys/inode.h>
#include	<sys/acct.h>
#include	<sys/map.h>
#include	<sys/filsys.h>
#include	<sys/mount.h>


int	nulldev();
int	nodev();
int	nullioctl();

#include "bk.h"
#if	NBK > 0
int	bkopen(), bkclose(), bkread(), bkioctl(), bkinput();
#else
#define	bkopen		nodev
#define	bkclose		nodev
#define	bkread		nodev
#define	bkioctl		nodev
#define	bkinput		nodev
#endif	NBK

#include "dh.h"
#if	NDH > 0
int	dhopen(), dhclose(), dhread(), dhwrite(), dhioctl(), dhstop();
extern struct tty	dh11[];
#else
#define	dhopen		nodev
#define	dhclose		nodev
#define	dhread		nodev
#define	dhwrite		nodev
#define	dhioctl		nodev
#define	dhstop		nodev
#define	dh11		((struct tty *) NULL)
#endif	NDH

#include "dn.h"
#if	NDN > 0
int	dnopen(), dnclose(), dnwrite();
#else
#define	dnopen		nodev
#define	dnclose		nodev
#define	dnwrite		nodev
#endif	NDN

#include "dz.h"
#if	NDZ > 0
int	dzopen(), dzclose(), dzread(), dzwrite(), dzioctl();
#ifdef	DZ_PDMA
int	dzstop();
#else
#define	dzstop		nulldev
#endif
extern	struct	tty	dz11[];
#else
#define	dzopen		nodev
#define	dzclose		nodev
#define	dzread		nodev
#define	dzwrite		nodev
#define	dzioctl		nodev
#define dzstop		nodev
#define	dz11		((struct tty *) NULL)
#endif	NDZ

#include "hk.h"
#if	NHK > 0
int	hkstrategy(), hkread(), hkwrite(), hkroot();
extern	struct	buf	hktab;
#define	hkopen		nulldev
#define	hkclose		nulldev
#define	_hktab		&hktab
#else
#define	hkopen		nodev
#define	hkclose		nodev
#define hkroot		nulldev
#define	hkstrategy	nodev
#define	hkread		nodev
#define	hkwrite		nodev
#define	_hktab		((struct buf *) NULL)
#endif	NHK

#include "hp.h"
#if	NHP > 0
int	hpstrategy(), hpread(), hpwrite(), hproot();
extern	struct	buf	hptab;
#define	hpopen		nulldev
#define	hpclose		nulldev
#define	_hptab		&hptab
#else
#define	hpopen		nodev
#define	hpclose		nodev
#define	hproot		nulldev
#define	hpstrategy	nodev
#define	hpread		nodev
#define	hpwrite		nodev
#define	_hptab		((struct buf *) NULL)
#endif	NHP

#include "hs.h"
#if	NHS > 0
int	hsstrategy(), hsread(), hswrite(), hsroot();
extern	struct	buf	hstab;
#define	_hstab		&hstab
#define	hsopen		nulldev
#define	hsclose		nulldev
#else
#define	hsopen		nodev
#define	hsclose		nodev
#define	hsstrategy	nodev
#define	hsread		nodev
#define	hswrite		nodev
#define	hsroot		nulldev
#define	_hstab		((struct buf *) NULL)
#endif	NHS

#include "ht.h"
#if	NHT > 0
int	htopen(), htclose(), htread(), htwrite(), htstrategy();
#ifdef	HT_IOCTL
int	htioctl();
#else
#define	htioctl		nodev
#endif
extern	struct	buf	httab;
#define	_httab		&httab
#else
#define	htopen		nodev
#define	htclose		nodev
#define	htread		nodev
#define	htwrite		nodev
#define	htioctl		nodev
#define	htstrategy	nodev
#define	_httab		((struct buf *) NULL)
#endif	NHT

#include "lp.h"
#if	NLP > 0
int	lpopen(), lpclose(), lpwrite();
#else
#define	lpopen		nodev
#define	lpclose		nodev
#define	lpwrite		nodev
#endif	NLP

#ifdef	OLDTTY
int	ttread(), ttyinput(), ttyoutput();
caddr_t	ttwrite();
#define	ttopen		nulldev
#define	ttclose		nulldev
#define	ttioctl		nullioctl
#define	ttmodem		nulldev
#else
#define	ttopen		nodev
#define	ttclose		nodev
#define	ttread		nodev
#define	ttwrite		nodev
#define	ttioctl		nodev
#define	ttyinput	nodev
#define	ttyoutput	nodev
#define	ttmodem		nodev
#endif

#include "pty.h"
#if	NPTY > 0
int	ptsopen(), ptsclose(), ptsread(), ptswrite(), ptsstop();
int	ptcopen(), ptcclose(), ptcread(), ptcwrite(), ptyioctl();
#ifdef	UCB_NET
int	ptcselect();
#endif
extern	struct tty pt_tty[];
#else
#define	ptsopen		nodev
#define	ptsclose	nodev
#define	ptsread		nodev
#define	ptswrite	nodev
#define	ptsstop		nodev
#define	ptcopen		nodev
#define	ptcclose	nodev
#define	ptcread		nodev
#define	ptcwrite	nodev
#define	ptyioctl	nodev
#define	ptcselect	nodev
#define	pt_tty		((struct tty *)NULL)
#endif	NPTY

#include "rk.h"
#if	NRK > 0
int	rkstrategy(), rkread(), rkwrite();
extern	struct	buf	rktab;
#define	rkopen		nulldev
#define	rkclose		nulldev
#define	_rktab		&rktab
#else
#define	rkopen		nodev
#define	rkclose		nodev
#define	rkstrategy	nodev
#define	rkread		nodev
#define	rkwrite		nodev
#define	_rktab		((struct buf *) NULL)
#endif	NRK

#include "rl.h"
#if	NRL > 0
int	rlstrategy(), rlread(), rlwrite();
extern	struct	buf	rltab;
#define	rlopen		nulldev
#define	rlclose		nulldev
#define	_rltab		&rltab
#else
#define	rlopen		nodev
#define	rlclose		nodev
#define	rlstrategy	nodev
#define	rlread		nodev
#define	rlwrite		nodev
#define	_rltab		((struct buf *) NULL)
#endif	NRL

#include "rm.h"
#if	NRM > 0
int	rmstrategy(), rmread(), rmwrite(), rmroot();
extern	struct	buf	rmtab;
#define	rmopen		nulldev
#define	rmclose		nulldev
#define	_rmtab		&rmtab
#else
#define	rmopen		nodev
#define	rmclose		nodev
#define	rmroot		nulldev
#define	rmstrategy	nodev
#define	rmread		nodev
#define	rmwrite		nodev
#define	_rmtab		((struct buf *) NULL)
#endif	NRM

#include "dr.h"
#if	NDR > 0
int	dropen(), drclose(), drread(), drwrite(), drioctl();
#else
#define	dropen		nodev
#define	drclose		nodev
#define	drread		nodev
#define	drwrite		nodev
#define	drioctl		nodev
#endif	NDR

#include "rp.h"
#if	NRP > 0
int	rpstrategy(), rpread(), rpwrite();
extern	struct	buf	rptab;
#define	rpopen		nulldev
#define	rpclose		nulldev
#define	_rptab		&rptab
#else
#define	rpopen		nodev
#define	rpclose		nodev
#define	rpstrategy	nodev
#define	rpread		nodev
#define	rpwrite		nodev
#define	_rptab		((struct buf *) NULL)
#endif	NRP

#include "tm.h"
#if	NTM > 0
int	tmopen(), tmclose(), tmread(), tmwrite(), tmstrategy();
#ifdef	TM_IOCTL
int	tmioctl();
#else
#define	tmioctl		nodev
#endif
extern	struct	buf	tmtab;
#define	_tmtab		&tmtab
#else
#define	tmopen		nodev
#define	tmclose		nodev
#define	tmread		nodev
#define	tmwrite		nodev
#define	tmioctl		nodev
#define	tmstrategy	nodev
#define	_tmtab		((struct buf *) NULL)
#endif	NTM

#include "ts.h"
#if	NTS > 0
int	tsopen(), tsclose(), tsread(), tswrite(), tsstrategy();
#ifdef	TS_IOCTL
int	tsioctl();
#else
#define	tsioctl		nodev
#endif
extern	struct	buf	tstab;
#define	_tstab		&tstab
#else
#define	tsopen		nodev
#define	tsclose		nodev
#define	tsread		nodev
#define	tswrite		nodev
#define	tsioctl		nodev
#define	tsstrategy	nodev
#define	_tstab		((struct buf *) NULL)
#endif	NTS

#ifdef	UCB_NTTY
int	ntyopen(), ntyclose(), ntread(), ntyinput(), ntyoutput();
caddr_t	ntwrite();
#define	ntyioctl	nullioctl
#define	ntymodem	nulldev
#else
#define	ntyopen		nodev
#define	ntyclose	nodev
#define	ntread		nodev
#define	ntwrite		nodev
#define	ntyioctl	nodev
#define	ntyinput	nodev
#define	ntyoutput	nodev
#define	ntymodem	nodev
#endif

#include "vp.h"
#if	NVP > 0
int	vpopen(), vpclose(), vpwrite(), vpioctl();
#else
#define	vpopen		nodev
#define	vpclose		nodev
#define	vpwrite		nodev
#define vpioctl		nodev
#endif	NVP

#include "xp.h"
#if	NXP > 0
int	xpstrategy(), xpread(), xpwrite(), xproot();
extern	struct	buf	xptab;
#define	xpopen		nulldev
#define	xpclose		nulldev
#define	_xptab		&xptab
#else
#define	xpopen		nodev
#define	xpclose		nodev
#define	xproot		nulldev
#define	xpstrategy	nodev
#define	xpread		nodev
#define	xpwrite		nodev
#define	_xptab		((struct buf *) NULL)
#endif	NXP

struct	bdevsw	bdevsw[] =
{
/* rk = 0 */
	rkopen,		rkclose,	rkstrategy,	nulldev,	_rktab,
/* rp = 1 */
	rpopen,		rpclose,	rpstrategy,	nulldev,	_rptab,
/* rf = 2 */
	nodev,		nodev,		nodev,		nulldev,	0,
/* tm = 3 */
	tmopen,		tmclose,	tmstrategy,	nulldev,	_tmtab,
/* hk = 4 */
	hkopen,		hkclose,	hkstrategy,	hkroot,		_hktab,
/* hs = 5 */
	hsopen,		hsclose,	hsstrategy,	hsroot,		_hstab,
/* xp = 6 */
	xpopen,		xpclose,	xpstrategy,	xproot,		_xptab,
/* hp = 7 */
	hpopen,		hpclose,	hpstrategy,	hproot,		_hptab,
/* rm = 8 */
	rmopen,		rmclose,	rmstrategy,	rmroot,		_rmtab,
/* ht = 9 */
	htopen,		htclose,	htstrategy,	nulldev,	_httab,
/* rl = 10 */
	rlopen,		rlclose,	rlstrategy,	nulldev,	_rltab,
/* ts = 11 */
	tsopen,		tsclose,	tsstrategy,	nulldev,	_tstab,
};

int	nblkdev = sizeof(bdevsw) / sizeof(bdevsw[0]);

int	klopen(), klclose(), klread(), klwrite(), klioctl();
extern	struct	tty	kl11[];
int	mmread(), mmwrite();
int	syopen(), syread(), sywrite(), sysioctl();

#ifdef UCB_NET
int	syselect();
int	ttselect();
int	seltrue();
#define	SELECT(r)	r,
#else
#define	SELECT(r)
#endif UCB_NET

struct	cdevsw	cdevsw[] =
{
/* kl = 0 */
	klopen,		klclose,	klread,		klwrite,
	klioctl,	nulldev,	kl11,		SELECT(ttselect)
/* pc = 1 */
	nodev,		nodev,		nodev,		nodev,
	nodev,		nodev,		0,		SELECT(nodev)
/* vp = 2 */
	vpopen,		vpclose,	nodev,		vpwrite,
	vpioctl,	nulldev,	0,		SELECT(nodev)
/* lp = 3 */
	lpopen,		lpclose,	nodev,		lpwrite,
	nodev,		nulldev,	0,		SELECT(nodev)
/* dh = 4 */
	dhopen,		dhclose,	dhread,		dhwrite,
	dhioctl,	dhstop,		dh11,		SELECT(ttselect)
/* dp = 5 */
	nodev,		nodev,		nodev,		nodev,
	nodev,		nodev,		0,		SELECT(nodev)
/* dj = 6 */
	nodev,		nodev,		nodev,		nodev,
	nodev,		nodev,		0,		SELECT(nodev)
/* dn = 7 */
	dnopen,		dnclose,	nodev,		dnwrite,
	nodev,		nulldev,	0,		SELECT(nodev)
/* mem = 8 */
	nulldev,	nulldev,	mmread,		mmwrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* rk = 9 */
	rkopen,		rkclose,	rkread,		rkwrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* rf = 10 */
	nodev,		nodev,		nodev,		nodev,
	nodev,		nodev,		0,		SELECT(seltrue)
/* rp = 11 */
	rpopen,		rpclose,	rpread,		rpwrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* tm = 12 */
	tmopen,		tmclose,	tmread,		tmwrite,
	tmioctl,	nulldev,	0,		SELECT(seltrue)
/* hs = 13 */
	hsopen,		hsclose,	hsread,		hswrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* xp = 14 */
	xpopen,		xpclose,	xpread,		xpwrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* hp = 15 */
	hpopen,		hpclose,	hpread,		hpwrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* rm = 16 */
	rmopen,		rmclose,	rmread,		rmwrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* ht = 17 */
	htopen,		htclose,	htread,		htwrite,
	htioctl,	nulldev,	0,		SELECT(seltrue)
/* du = 18 */
	nodev,		nodev,		nodev,		nodev,
	nodev,		nodev,		0,		SELECT(nodev)
/* tty = 19 */
	syopen,		nulldev,	syread,		sywrite,
	sysioctl,	nulldev,	0,		SELECT(syselect)
/* rl = 20 */
	rlopen,		rlclose,	rlread,		rlwrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* hk = 21 */
	hkopen,		hkclose,	hkread,		hkwrite,
	nodev,		nulldev,	0,		SELECT(seltrue)
/* ts = 22 */
	tsopen,		tsclose,	tsread,		tswrite,
	tsioctl,	nulldev,	0,		SELECT(seltrue)
/* dz = 23 */
	dzopen,		dzclose,	dzread,		dzwrite,
	dzioctl,	dzstop,		dz11,		SELECT(ttselect)
/* ptc = 24 */
	ptcopen,	ptcclose,	ptcread,	ptcwrite,
	ptyioctl,	nulldev,	pt_tty,		SELECT(ptcselect)
/* pts = 25 */
	ptsopen,	ptsclose,	ptsread,	ptswrite,
	ptyioctl,	ptsstop,	pt_tty,		SELECT(ttselect)
/* dr = 26 */
	dropen,		drclose,	drread,		drwrite,
	drioctl,	nulldev,	0,		SELECT(seltrue)
};

int	nchrdev = sizeof(cdevsw) / sizeof(cdevsw[0]);

struct	linesw linesw[] =
{
/* oldtty = 0 */
	ttopen,		ttclose,	ttread,		ttwrite,
	ttioctl,	ttyinput,	ttyoutput,	ttmodem,
/* newtty = 1 */
	ntyopen,	ntyclose,	ntread,		ntwrite,
	ntyioctl,	ntyinput,	ntyoutput,	ntymodem,
/* oldtty = bk */
	bkopen,		bkclose,	bkread,		ttwrite,
	bkioctl,	bkinput,	nodev,		nulldev,
};

int	nldisp	= sizeof(linesw) / sizeof(linesw[0]);
