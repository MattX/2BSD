#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/tty.h>
#include <sys/conf.h>
#include <sys/proc.h>
#include <sys/text.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/file.h>
#include <sys/inode.h>
#include <sys/acct.h>
#include <sys/dk.h>

#ifdef UCB_SCCSID
static	char sccs_id[] = "%W%";
#endif

int	nulldev();
int	nodev();
int	xpstrategy(); struct	buf	xptab;
int	rpstrategy(); struct	buf	rptab;
int	rlstrategy(); struct	buf	rltab;
int	rkstrategy(); struct	buf	rktab;
int	hkstrategy(); struct	buf	hktab;
int	dvhpstrategy(); struct	buf	dvhptab;
int	htopen(), htclose(), htstrategy(); struct	buf	httab;
int	tmopen(), tmclose(), tmstrategy(); struct	buf	tmtab;
int	tsopen(), tsclose(), tsstrategy(); struct	buf	tstab;
struct	bdevsw	bdevsw[] =
{
	nulldev, nulldev, rkstrategy, &rktab,	/* rk = 0 */
	nulldev, nulldev, rpstrategy, &rptab,	/* rp = 1 */
	nodev, nodev, nodev, 0, /* rf = 2 */
	tmopen, tmclose, tmstrategy, &tmtab,	/* tm = 3 */
	nodev, nodev, nodev, 0, /* tc = 4 */
	nodev, nodev, nodev, 0,  /* hs = 5 */
	nodev, nodev, nodev, 0,	/* hp = 6 */
	htopen, htclose, htstrategy, &httab,	/* ht = 7 */
	nulldev, nulldev, rlstrategy, &rltab,	/* rl = 8 */
	nodev, nodev, nodev, 0, /* rm = 9 */
	nulldev, nulldev, hkstrategy, &hktab,	/* hk = 10 */
	nodev, nodev, nodev, 0, /* xm = 11 */
	nulldev, nulldev, xpstrategy, &xptab,	/* xp = 12 */
	tsopen, tsclose, tsstrategy, &tstab,	/* ts = 13 */
	nulldev, nulldev, dvhpstrategy, &dvhptab,	/* dvhp = 14 */
	0
};

int	klopen(), klclose(), klread(), klwrite(), klioctl();
int	lpopen(), lpclose(), lpwrite();
int	mmread(), mmwrite();
int	xpread(), xpwrite();
int	dvhpread(), dvhpwrite();
int	rpread(), rpwrite();
int	rlread(), rlwrite();
int	rkread(), rkwrite();
int	hkread(), hkwrite();
int	htread(), htwrite();
int	tmread(), tmwrite();
int	tsread(), tswrite();
int	syopen(), syread(), sywrite(), sysioctl();
int	mxopen(), mxclose(), mxread(), mxwrite(), mxioctl();
int	dzopen(), dzclose(), dzread(), dzwrite(), dzioctl();


struct	cdevsw	cdevsw[] =
{
	klopen, klclose, klread, klwrite, klioctl, nulldev, 0,	/* console = 0 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* mux = 1 */
	lpopen, lpclose, nodev, lpwrite, nodev, nulldev, 0, /* lp = 2 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dc = 3 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dh = 4 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dp = 5 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dj = 6 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dn = 7 */
	nulldev, nulldev, mmread, mmwrite, nodev, nulldev, 0, 	/* mem = 8 */
	nulldev, nulldev, rkread, rkwrite, nodev, nulldev, 0,/* rk = 9 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* rf = 10 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0,	/* hp = 11 */
	tmopen, tmclose, tmread, tmwrite, nodev, nulldev, 0,	/* tm = 12 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* hs = 13 */
	nulldev, nulldev, rpread, rpwrite, nodev, nulldev, 0,/* rp = 14 */
	htopen, htclose, htread, htwrite, nodev, nulldev, 0,	/* ht = 15 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* du = 16 */
	syopen, nulldev, syread, sywrite, sysioctl, nulldev, 0,	/* tty = 17 */
	nulldev, nulldev, rlread, rlwrite, nodev, nulldev, 0,/* rl = 18 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* rm = 19 */
	nulldev, nulldev, xpread, xpwrite, nodev, nulldev, 0,/* xp = 20 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* xm = 21 */
	dzopen, dzclose, dzread, dzwrite, dzioctl, nulldev, 0, /* dz = 22 */
	nulldev, nulldev, dvhpread, dvhpwrite, nodev, nulldev, 0,/* dvhp = 23 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* rc = 24 */
	nulldev, nulldev, hkread, hkwrite, nodev, nulldev, 0,/* hk = 25 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* mpx = 26 */
	tsopen, tsclose, tsread, tswrite, nodev, nulldev, 0,	/* ts = 27 */
	0
};

int	ttyopen(), ttyclose(), ttread(), ttwrite(), ttyinput(), ttstart();
int	ttyrend();
int	nullioctl();
#ifdef UCB_NTTY
int	bkopen(),bkclose(),bkread(),bkinput(),bkioctl();
int	ntyopen(),ntyclose(),ntread();
char	*ntwrite();
int	ntyinput(),ntyrend();
#endif

struct	linesw linesw[] =
{
	ttyopen, nulldev, ttread, ttwrite, nodev,
	ttyinput, ttstart, nulldev, nulldev, nulldev,	/* 0 */
#ifdef UCB_NTTY
	ttyopen, nulldev, ttread, ttwrite, nodev,
	ttyinput, ttstart, nulldev, nulldev, nulldev,	/* 1 */
	ntyopen, ntyclose, ntread, ntwrite, nullioctl,
	ntyinput, ntyrend, nulldev, ttstart, nulldev,	/* 2 */
#endif
	0
};
int	rootdev	= makedev(12, 0);
int	swapdev	= makedev(12, 0);
int	pipedev = makedev(12, 0);
int	nldisp = 3;			/* Don't count mpx!!! (see mx1.c) */
daddr_t	swplo	= 8192;
int	nswap	= 2048;

struct	file	file[NFILE];
struct	inode	inode[NINODE];
int	mpxchan();
int	(*ldmpx)() = mpxchan;
struct	proc	proc[NPROC];
struct	text	text[NTEXT];
struct	buf	buf[NBUF];
struct	buf	bfreelist;
#ifdef	UCB_BUFOUT
struct	buf	abfreelist;
struct	buf	abuf[NABUF];
char	abuffers[NABUF][BSIZE];
#endif
struct	acct	acctbuf;
struct	inode	*acctp;
#ifdef	UCB_DKEXT
struct	dk	dk;
#endif
char msgbuf[MSGBUFS] = {"\0"};
