/*
 *	DZ11 driver
 *
 * appears to work - contains magic numbers!
 */

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/tty.h>
#include <sys/file.h>
#ifdef UCB_LDISC
#include <sys/conf.h>
#endif UCB_LDISC

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)dz.c	3.5";
#endif

struct device *dz_addr[] = {
	(struct device *) 0160000 };
#define	NDZ11	8
int	ndztty	= NDZ11;		/* Only for pstat */
struct	tty	dz11[NDZ11];
char	dz_stat;

char	dz_speeds[] = {
	0, 020, 021, 022, 023, 024, 0, 025,
	026, 027, 030, 032, 034, 036, 0, 0,
};

#define	BITS7	020
#define	BITS8	030
#define	TWOSB	040
#define	PENABLE	0100
#define	OPAR	0200
#define	RCVENA	010000

#define	IE	040140
#define	PERROR	010000
#define	FRERROR	020000
#define	SSPEED	7	/* standard speed: 300 baud */

struct device {
	int	dzcsr;
	int	dzrbuf;
	char	dztcr;
	char	dzdtr;
	char	dztbuf;
	char	dzbrk;
};

#define	dzlpr	dzrbuf
#define	dzmsr	dzbrk

#define	ON	1
#define	OFF	0

dzopen(dev, flag) {
	register struct tty *tp;
	int x;
	extern dzstart(), dzscan();

	x = dev;
	dev = minor(dev);
	if (dev >= NDZ11) {
		u.u_error = ENXIO;
		return;
	}
	tp = &dz11[dev];
	if ((tp->t_state&(ISOPEN|WOPEN)) == 0) {
		tp->t_oproc = dzstart;
		tp->t_iproc = NULL;
		ttychars(tp);
		tp->t_ispeed = SSPEED;
		tp->t_ospeed = SSPEED;
		tp->t_flags = ODDP|EVENP|ECHO;
		dzparam(dev);
	} else if (tp->t_state&XCLUDE && u.u_uid != 0) {
		u.u_error = EBUSY;
		return;
	}
	dzmodem(dev, ON);
	spl6();
	while ((tp->t_state&CARR_ON)==0) {
		tp->t_state |= WOPEN;
		sleep((caddr_t)&tp->t_rawq, TTIPRI);
	}
	spl0();
#ifndef UCB_LDISC
	ttyopen(x,tp);
#else UCB_LDISC
	(*linesw[tp->t_line].l_open)(x,tp);
#endif UCB_LDISC
}

dzclose(d)
{
	register struct tty *tp;
	register dev;

	dev = minor(d);
	tp = &dz11[dev];
#ifdef UCB_LDISC
	(*linesw[tp->t_line].l_close)(tp);
#endif UCB_LDISC
	if (tp->t_state&HUPCLS)
		dzmodem(dev, OFF);
	ttyclose(tp);
}

dzread(dev)
{
#ifdef UCB_LDISC
	register struct tty *tp;
#endif UCB_LDISC

	dev = minor(dev);
#ifndef UCB_LDISC
	ttread(&dz11[dev]);
#else UCB_LDISC
	tp = &dz11[dev];
	(*linesw[tp->t_line].l_read)(tp);
#endif UCB_LDISC
}

dzwrite(dev)
{
#ifdef UCB_LDISC
	register struct tty *tp;
#endif UCB_LDISC

	dev = minor(dev);
#ifndef UCB_LDISC
	ttwrite(&dz11[dev]);
#else UCB_LDISC
	tp = &dz11[dev];
	(*linesw[tp->t_line].l_write)(tp);
#endif UCB_LDISC
}

dzioctl(dev, cmd, addr, flag)
dev_t	dev;
int	cmd;
caddr_t	addr;
int	flag;
{
	register struct tty *tp;

	dev = minor(dev);
	tp = &dz11[dev];
	if (ttioctl(tp, cmd, addr, flag)) {
		if (cmd==TIOCSETP||cmd==TIOCSETN)
			dzparam(dev);
	} else {
		u.u_error = ENOTTY;
	}
}

dzparam(dev)
{
	register struct tty *tp;
	register struct device *dzaddr;
	register lpr;

	tp = &dz11[dev];
	dzaddr= dz_addr[dev>>3];
	dzaddr->dzcsr = IE;
	if (dz_stat==0) {
		dzscan();
		dz_stat++;
	}
	if (tp->t_ispeed==0) {	/* Hang up line */
		dzmodem(dev, OFF);
		return;
	}
	lpr = (dz_speeds[tp->t_ispeed]<<8)|(dev&07);
	if (tp->t_flags&RAW)
		lpr |= BITS8;
	else
		lpr |= BITS7|PENABLE;
	if ((tp->t_flags&EVENP)==0)
		lpr |= OPAR;
	if (tp->t_ispeed == 3)	/* 110 baud */
		lpr |= TWOSB;
	dzaddr->dzlpr = lpr;
}

dzrint(dev)
{
	register struct tty *tp;
	register c;
	register struct device *dzaddr;

	dzaddr = dz_addr[dev];
	while ((c = dzaddr->dzrbuf) < 0) {	/* char. present */
		tp = &dz11[((c>>8)&07)|(dev<<3)];
		if (tp >= &dz11[NDZ11])
			continue;
		if((tp->t_state&ISOPEN)==0) {
			wakeup((caddr_t)&tp->t_rawq);
			continue;
		}
		if (c&FRERROR)		/* break */
			if (tp->t_flags&RAW)
				c = 0;		/* null (for getty) */
			else
				c = tun.t_intrc;
		if (c&PERROR)
			if ((tp->t_flags&(EVENP|ODDP))==EVENP
			 || (tp->t_flags&(EVENP|ODDP))==ODDP )
				continue;
#ifndef UCB_LDISC
		ttyinput(c, tp);
#else UCB_LDISC
		(*linesw[tp->t_line].l_rint)(c, tp);
#endif UCB_LDISC
	}
}

dzxint(dev)
{
	register struct tty *tp;
	register struct device *dzaddr;

	dzaddr = dz_addr[dev];
	while(dzaddr->dzcsr<0) {	/* TX rdy */
		tp = &dz11[((dev<<3)|(dzaddr->dzcsr>>8)&07)];
		dzaddr->dztbuf = tp->t_char;
		tp->t_state &= ~BUSY;
		dzstart(tp);
	}
}

dzstart(tp)
register struct tty *tp;
{
	register unit, c;
	int s;
	struct device *dzaddr;
	extern ttrstrt();

	unit = tp - dz11;
	dzaddr = dz_addr[unit>>3];
	unit = 1<<(unit&07);
	s = spl5();
	if (tp->t_state&(TIMEOUT|BUSY)) {
		splx(s);
		return;
	}
	if (tp->t_state&TTSTOP) {
		dzaddr->dztcr &= ~unit;
		splx(s);
		return;
	}
	if ((c=getc(&tp->t_outq)) >= 0) {
		if (c>=0200 && (tp->t_flags&RAW)==0) {
			dzaddr->dztcr &= ~unit;
			tp->t_state |= TIMEOUT;
			timeout(ttrstrt, (caddr_t)tp, (c&0177)+6);
		} else {
			tp->t_char = c;
			tp->t_state |= BUSY;
			dzaddr->dztcr |= unit;
		}
		if (tp->t_outq.c_cc<=TTLOWAT(tp) && tp->t_state&ASLEEP) {
			tp->t_state &= ~ASLEEP;
			wakeup((caddr_t)&tp->t_outq);
		}
	} else
		dzaddr->dztcr &= ~unit;
	splx(s);
}

dzmodem(dev, flag)
{
	register struct device *dzaddr;
	register bit;

	dzaddr = dz_addr[dev>>3];
	bit = 1<<(dev&07);
	if (flag==OFF)
		dzaddr->dzdtr &= ~bit;
	else	dzaddr->dzdtr |= bit;
}

dzscan()
{
	register i;
	register struct device *dzaddr;
	register struct tty *tp;
	char	bit;

	for (i=0; i<NDZ11; i++) {
		dzaddr = dz_addr[i>>3];
		tp = &dz11[i];
		bit = 1<<(i&07);
		if (dzaddr->dzmsr&bit) {
			if ((tp->t_state&CARR_ON)==0) {
				wakeup((caddr_t)&tp->t_rawq);
				tp->t_state |= CARR_ON;
			}
		} else {
			if ((tp->t_state&CARR_ON) &&
			    (tp->t_local&LNOHANG)==0) {
				if (tp->t_state&ISOPEN) {
					signal(tp->t_pgrp, SIGHUP);
					dzaddr->dzdtr &= ~bit;
					flushtty(tp, FREAD|FWRITE);
				}
				tp->t_state &= ~CARR_ON;
			}
		}
	}
	timeout(dzscan, (caddr_t)0, 120);
}
