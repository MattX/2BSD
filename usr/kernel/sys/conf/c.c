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
int	dvhpstrategy();
struct	buf	dvhptab;
int	htopen(), htclose(), htstrategy();
struct	buf	httab;
int	hsstrategy();
struct	buf	hstab;
struct	bdevsw	bdevsw[] =
{
	nodev, nodev, nodev, 0, /* rk = 0 */
	nodev, nodev, nodev, 0,	/* rp = 1 */
	nodev, nodev, nodev, 0, /* rf = 2 */
	nodev, nodev, nodev, 0, /* tm = 3 */
	nodev, nodev, nodev, 0, /* tc = 4 */
	nulldev, nulldev, hsstrategy, &hstab,  /* hs = 5 */
	nodev, nodev, nodev, 0,	/* hp = 6 */
	htopen, htclose, htstrategy, &httab,	/* ht = 7 */
	nodev, nodev, nodev, 0, /* rl = 8 */
	nodev, nodev, nodev, 0, /* rm = 9 */
	nodev, nodev, nodev, 0, /* xp = 10 */
	nodev, nodev, nodev, 0, /* xm = 11 */
	nulldev, nulldev, dvhpstrategy, &dvhptab,	/* dvhp = 12 */
	0
};

int	klopen(), klclose(), klread(), klwrite(), klioctl();
int	dzopen(), dzclose(), dzread(), dzwrite(), dzioctl();
int	lpopen(), lpclose(), lpwrite();
int	muxopen(), muxclose(), muxread(), muxwrite(), muxioctl();
int	mmread(), mmwrite();
int	dvhpread(), dvhpwrite();
int	htread(), htwrite();
int	hpread(), hpwrite();
int	syopen(), syread(), sywrite(), sysioctl();
int	hsread(), hswrite();
int	rcread(); rcwrite();
/*
int	mxopen(), mxclose(), mxread(), mxwrite(), mxioctl();
int	mcread(), mcwrite();			/* mpx */
int	fast_open(),fast_close(), fast_read(), fast_write(), fast_ioctl();
struct tty	dz11[];


struct	cdevsw	cdevsw[] =
{
	klopen, klclose, klread, klwrite, klioctl, nulldev, 0,	/* console = 0 */
	muxopen, muxclose, muxread, muxwrite, muxioctl, nulldev, 0, /* mux = 1 */
	lpopen, lpclose, nodev, lpwrite, nodev, nulldev, 0, /* lp = 2 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dc = 3 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dh = 4 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dp = 5 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dj = 6 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* dn = 7 */
	nulldev, nulldev, mmread, mmwrite, nodev, nulldev, 0, 	/* mem = 8 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* rk = 9 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* rf = 10 */
	nodev, nodev, nodev, nodev, nodev, nodev, 0,	/* hp = 11 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* tm = 12 */
	nulldev, nulldev, hsread, hswrite, nodev, nulldev, 0, /* hs = 13 */
	nodev, nodev, nodev, nodev, nodev, nodev, 0,	/* rp = 14 */
	htopen, htclose, htread, htwrite, nodev, nulldev, 0,	/* ht = 15 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* du = 16 */
	syopen, nulldev, syread, sywrite, sysioctl, nulldev, 0,	/* tty = 17 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* rl = 18 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* rm = 19 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* xp = 20 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* xm = 21 */
	dzopen, dzclose, dzread, dzwrite, dzioctl, nulldev,dz11, /* dz = 22 */
	nodev, nodev, nodev, nodev, nodev, nulldev, 0, /* bx = 23 */
	nulldev, nulldev, rcread, rcwrite, nulldev, nulldev, 0, /* rc = 24 */
	nulldev, nulldev, dvhpread, dvhpwrite, nodev, nulldev, 0,/* dvhp = 25 */
/*
	mxopen, mxclose, mxread, mxwrite, mxioctl, nulldev, 0, /* mpx = 26 */
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
#ifdef notdef
	ttyopen, nulldev, ttread, ttwrite, nullioctl,
	ttyinput, ttyrend, nulldev, nulldev, nulldev,	/* 0 */
#endif
#ifdef UCB_NTTY
	bkopen, bkclose, bkread, ttwrite, bkioctl,
	bkinput, nodev, nulldev, ttstart, nulldev,	/* 1 */
	ntyopen, ntyclose, ntread, ntwrite, nullioctl,
	ntyinput, ntyrend, nulldev, ttstart, nulldev,	/* 2 */
#endif
/*
	mxopen, mxclose, mcread, mcwrite, mxioctl,
	nulldev, nulldev, nulldev, nulldev, nulldev,	/* 3 */
	0
};
int	rootdev	= makedev(5, 8);
int	swapdev	= makedev(12, 8);
int	pipedev = makedev(5, 8);
int	nldisp = 3;			/* Don't count mpx!!! (see mx1.c) */
daddr_t	swplo	= 2079;
int	nswap	= 7326;

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
#ifdef	ACCT
struct	acct	acctbuf;
struct	inode	*acctp;
#endif	ACCT
#ifdef	UCB_DKEXT
struct	dk	dk;
#endif
char msgbuf[MSGBUFS] = {"\0"};
