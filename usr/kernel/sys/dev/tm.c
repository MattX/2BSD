/*
 * TM tape driver
 */

#define DDMT	/* dual density capibility */

#include <sys/param.h>
#include <sys/buf.h>
#include <sys/dir.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/user.h>

struct device {
	int	tmer;
	int	tmcs;
	int	tmbc;
	char	*tmba;
	int	tmdb;
	int	tmrd;
};

struct	buf	tmtab;
struct	buf	ctmbuf;
struct	buf	rtmbuf;

char	t_flags[8];
char	t_openf[8];
daddr_t	t_blkno[8];
daddr_t	t_nxrec[8];
#ifdef	CGL_MTOP
struct softc {
	short sc_erreg;
	short sc_dsreg;
	short sc_resid;
} softc[8];
#endif

#define	TMADDR ((struct device *)0172520)

#ifdef	CGL_MTOP
#define INF	32760
#define OFFL	0
#define BOT	040
#endif
#define	GO	01
#define	RCOM	02
#define	WCOM	04
#define	WEOF	06
#define	NOP	0100
#define	SFORW	010
#define	SREV	012
#define	WIRG	014
#define	REW	016
#ifdef	DDMT
#define LODENS	060000		/* 9-channel, 800bpi */
#define HIDENS	020000		/* 9-channel, 1600bpi */
#else
#define	DENS	060000		/* 9-channel */
#endif
#define	IENABLE	0100
#define	CRDY	0200
#define GAPSD	010000
#define	TUR	1
#define	HARD	0102200	/* ILC, EOT, NXM */
#define	RLE	01000	/* CGL BUG FIX...was 0100  16jun79 */
#define	EOF	0040000
#define	WL	04

#define	SSEEK	1
#define	SIO	2
#define	SCOM	3

#define T_WRITTEN 1

tmopen(dev, flag)
{
	register unit, ds;

	unit = minor(dev) & 077;
	if (t_openf[unit]) {
		u.u_error = ENXIO;
		return;
	}
	t_blkno[unit] = 0;
	t_nxrec[unit] = 65535;
	t_flags[unit] = 0;

	tmtab.b_flags |= B_TAPE;
	ds = tcommand(dev, NOP);
	if ((ds&TUR)==0) {
#ifndef	CGL_LOCAL
		printf("mt%d off line\n",unit);
#endif
		u.u_error = EIO;
	}
	if (flag && ds&WL) {
#ifndef	CGL_LOCAL
		printf("mt%d needs write ring\n",unit);
#endif
		u.u_error = EIO;
	}
	if (u.u_error==0)
		t_openf[unit]++;
}

tmclose(dev, flag)
dev_t dev;
int flag;
{

	if ( flag == FWRITE ||
	((flag&FWRITE) && (t_flags[minor(dev)&077]&T_WRITTEN))) {
		tcommand(dev, WEOF);
		tcommand(dev, WEOF);
#ifdef	CGL_MTOP
		tcommand(dev, SREV, 1);
#else
		tcommand(dev, SREV);
#endif
	}
	if ((minor(dev)&0200) == 0)
		tcommand(dev, REW);
	t_openf[minor(dev)&077] = 0;
}

#ifdef	CGL_MTOP
tcommand(dev, com, cnt)
#else
tcommand(dev, com)
#endif
{
	register struct buf *bp;

	bp = &ctmbuf;
	spl5();
	while (bp->b_flags&B_BUSY) {
		bp->b_flags |= B_WANTED;
		sleep((caddr_t)bp, PRIBIO);
	}
	bp->b_flags = B_BUSY|B_READ;
	spl0();
	bp->b_dev = dev;
	bp->b_resid = com;
#ifdef	CGL_MTOP
	if (com==SFORW || com==SREV)
		bp->b_bcount = cnt;
#endif
	bp->b_blkno = 0;
	tmstrategy(bp);
	iowait(bp);
	if (bp->b_flags&B_WANTED)
		wakeup((caddr_t)bp);
	bp->b_flags = 0;
	return(bp->b_resid);
}

tmstrategy(bp)
register struct buf *bp;
{
	register daddr_t *p;

#ifdef UNIBUS_MAP
	if(bp->b_flags&B_PHYS)
		mapalloc(bp);
#endif
	if (bp != &ctmbuf) {
		p = &t_nxrec[minor(bp->b_dev)&077];
#ifdef	UCB_NKB
		if (*p <= dbtofsb(bp->b_blkno)) {
			if (*p < dbtofsb(bp->b_blkno)) {
#else
		if (*p <= bp->b_blkno) {
			if (*p < bp->b_blkno) {
#endif
				bp->b_flags |= B_ERROR;
				iodone(bp);
				return;
			}
			if (bp->b_flags&B_READ) {
				clrbuf(bp);
				bp->b_resid = 0;
				iodone(bp);
				return;
			}
		}
		if ((bp->b_flags&B_READ) == 0) {
			t_flags[minor(bp->b_dev)&077] |= T_WRITTEN;
#ifdef	UCB_NKB
			*p = dbtofsb(bp->b_blkno)+1;
#else
			*p = bp->b_blkno+1;
#endif
		}
	}
	bp->av_forw = 0;
	spl5();
	if (tmtab.b_actf == NULL)
		tmtab.b_actf = bp;
	else
		tmtab.b_actl->av_forw = bp;
	tmtab.b_actl = bp;
	if (tmtab.b_active == NULL)
		tmstart();
	spl0();
}

tmstart()
{
	register struct buf *bp;
	register int com;
	int unit;
	register daddr_t *blkno;
#ifdef	CGL_MTOP
	register struct softc *sc;
#endif

    loop:
	if ((bp = tmtab.b_actf) == 0)
		return;
	unit = minor(bp->b_dev)&077;
	blkno = &t_blkno[unit];
	if (t_openf[unit] < 0 || (TMADDR->tmcs & CRDY) == NULL) {
		bp->b_flags |= B_ERROR;
		goto next;
	}
#ifdef	CGL_MTOP
	sc = &softc[unit&07];
	sc->sc_erreg = TMADDR->tmer;
	sc->sc_dsreg = TMADDR->tmcs;
	sc->sc_resid = TMADDR->tmbc;
#endif
	if (bp == &ctmbuf) {
		if (bp->b_resid == NOP) {
			bp->b_resid = TMADDR->tmer;
			goto next;
		}
		tmtab.b_active = SCOM;
#ifdef	CGL_MTOP
		TMADDR->tmbc = -bp->b_bcount;
#endif
#ifdef	DDMT
		if (minor(bp->b_dev)&0100)
			TMADDR->tmcs = HIDENS|bp->b_resid|GO|(unit<<8)|IENABLE;
		else
			TMADDR->tmcs = LODENS|bp->b_resid|GO|(unit<<8)|IENABLE;
#else
		TMADDR->tmcs = DENS|bp->b_resid|GO| (unit<<8) | IENABLE;
#endif
		return;
	}
#ifdef	DDMT
	if (minor(bp->b_dev)&0100)
		com = (unit<<8) | ((bp->b_xmem & 03) << 4) | IENABLE|HIDENS;
	else
		com = (unit<<8) | ((bp->b_xmem & 03) << 4) | IENABLE|LODENS;
#else
	com = (unit<<8) | ((bp->b_xmem & 03) << 4) | IENABLE|DENS;
#endif
#ifdef	UCB_NKB
	if (*blkno != dbtofsb(bp->b_blkno)) {
		tmtab.b_active = SSEEK;
		if (*blkno < dbtofsb(bp->b_blkno)) {
			com |= SFORW|GO;
			TMADDR->tmbc = *blkno - dbtofsb(bp->b_blkno);
		} else {
			if (dbtofsb(bp->b_blkno) == 0)
				com |= REW|GO;
			else {
				com |= SREV|GO;
				TMADDR->tmbc = dbtofsb(bp->b_blkno) - *blkno;
			}
		}
#else
	if (*blkno != bp->b_blkno) {
		tmtab.b_active = SSEEK;
		if (*blkno < bp->b_blkno) {
			com |= SFORW|GO;
			TMADDR->tmbc = *blkno - bp->b_blkno;
		} else {
			if (bp->b_blkno == 0)
				com |= REW|GO;
			else {
				com |= SREV|GO;
				TMADDR->tmbc = bp->b_blkno - *blkno;
			}
		}
#endif
		TMADDR->tmcs = com;
		return;
	}
	tmtab.b_active = SIO;
	TMADDR->tmbc = -bp->b_bcount;
	TMADDR->tmba = bp->b_un.b_addr;
	TMADDR->tmcs = com | ((bp->b_flags&B_READ)? RCOM|GO:
	    ((tmtab.b_errcnt)? WIRG|GO: WCOM|GO));
	return;

next:
	tmtab.b_actf = bp->av_forw;
	iodone(bp);
	goto loop;
}

tmintr()
{
	register struct buf *bp;
	register int unit;
	int	state;
#ifdef	CGL_MTOP
	register struct softc *sc;
#endif

	if ((bp = tmtab.b_actf) == NULL)
		return;
	unit = minor(bp->b_dev)&077;
	state = tmtab.b_active;
	tmtab.b_active = 0;
#ifdef	CGL_MTOP
	sc = &softc[unit&07];
	sc->sc_erreg = TMADDR->tmer;
	sc->sc_dsreg = TMADDR->tmcs;
	sc->sc_resid = TMADDR->tmbc;
#endif
	if (TMADDR->tmcs < 0) {		/* error bit */
		while(TMADDR->tmrd & GAPSD) ; /* wait for gap shutdown */
		if (TMADDR->tmer&EOF) {
#ifdef	UCB_NKB
			t_nxrec[unit] = dbtofsb(bp->b_blkno);
#else
			t_nxrec[unit] = bp->b_blkno;
#endif
			state = SCOM;
			TMADDR->tmbc = -bp->b_bcount;
			goto out;
		}
		if ((TMADDR->tmer&HARD) == 0 && TMADDR->tmer&RLE) {
			state = SIO;
			goto out;
		}
		if ((TMADDR->tmer&(HARD|EOF)) == NULL && state==SIO) {
			if (++tmtab.b_errcnt < 4) {	/* CGL change */
				t_blkno[unit]++;
				tmtab.b_active = 0;
				tmstart();
				return;
			}
		} else
			if (t_openf[unit]>0 && bp!=&rtmbuf &&
				(TMADDR->tmer&EOF)==0 ) {
				t_openf[unit] = -1;
				deverror(bp, TMADDR->tmer, 0);
			}
		bp->b_flags |= B_ERROR;
		state = SIO;
	}
out:
	switch ( state ) {
	case SIO:
		t_blkno[unit] += (bp->b_bcount>>BSHIFT);
	case SCOM:
		tmtab.b_errcnt = 0;
		tmtab.b_actf = bp->av_forw;
		bp->b_resid = -TMADDR->tmbc;
		iodone(bp);
		break;
	case SSEEK:
#ifdef	UCB_NKB
		t_blkno[unit] = dbtofsb(bp->b_blkno);
#else
		t_blkno[unit] = bp->b_blkno;
#endif
		break;
	default:
		return;
	}
	tmstart();
}

tmread(dev)
{
	tmphys(dev);
	physio(tmstrategy, &rtmbuf, dev, B_READ);
}

tmwrite(dev)
{
	tmphys(dev);
	physio(tmstrategy, &rtmbuf, dev, B_WRITE);
}

tmphys(dev)
{
	register unit;
	daddr_t a;

	unit = minor(dev) & 077;
	if(unit < 8) {
		a = u.u_offset >> BSHIFT; /* CGL BUG FIX...was '9'  27jan81 */
		t_blkno[unit] = a;
		t_nxrec[unit] = a+1;
	}
}

#ifdef	CGL_MTOP
#include <sys/mtio.h>

/*ARGSUSED*/
tmioctl(dev, cmd, addr, flag)
dev_t dev;
caddr_t addr;
{
	register struct buf *bp = &ctmbuf;
	register struct softc *sc = &softc[minor(dev)&07];
	register callcount;
	int fcount;
	struct mtop mtop;
	struct mtget mtget;
	/* we depend on the values and order of the MT codes here */
	static tmops[] = {WEOF,SFORW,SREV,SFORW,SREV,REW,OFFL,NOP};

	switch (cmd) {

	case MTIOCTOP:	/* tape operation */
		if (copyin((caddr_t)addr, (caddr_t)&mtop, sizeof(mtop))) {
			u.u_error = EFAULT;
			return;
		}
		switch(mtop.mt_op) {
		case MTWEOF:
			callcount = mtop.mt_count;
			fcount = 1;
			break;
		case MTFSF: case MTBSF:
			callcount = mtop.mt_count;
			fcount = INF;
			break;
		case MTFSR: case MTBSR:
			callcount = 1;
			fcount = mtop.mt_count;
			break;
		case MTREW: case MTOFFL: case MTNOP:
			callcount = 1;
			fcount = 1;
			break;
		default:
			u.u_error = ENXIO;
			return;
		}
		if (callcount <= 0 || fcount <= 0) {
			u.u_error = ENXIO;
			return;
		}
		while (--callcount >= 0) {
			tcommand(dev, tmops[mtop.mt_op], fcount);
			if ((mtop.mt_op == MTFSR || mtop.mt_op == MTBSR) &&
			    bp->b_resid) {
				u.u_error = EIO;
				break;
			}
			if ((bp->b_flags&B_ERROR) || sc->sc_erreg&BOT)
				break;
		}
		geterror(bp);
		return;
	case MTIOCGET:
		mtget.mt_dsreg = sc->sc_dsreg;
		mtget.mt_erreg = sc->sc_erreg;
		mtget.mt_resid = sc->sc_resid;
		if (copyout((caddr_t)&mtget, addr, sizeof(mtget)))
			u.u_error = EFAULT;
		return;
	default:
		u.u_error = ENXIO;
	}
}
#endif

