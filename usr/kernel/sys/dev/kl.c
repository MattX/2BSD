/*
 *   KL/DL-11 driver
 */
#include <sys/param.h>
#include <sys/conf.h>
#include	<sys/dir.h>
#include <sys/user.h>
#include <sys/tty.h>
#include <sys/systm.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "%W%";
#endif


/* base address */
#define	KLADDR	((struct device *)0177560)	/* console */
#define	KLBASE	((struct device *)0176600)	/* kl and dl11-a */
#define	DLBASE	((struct device *)0176500)	/* dl-e */
#define	NKL11	1
#define	NDL11	0
#define DSRDY	02
#define	RDRENB	01
#define	DLDELAY	4	/* Extra delay for DL's (double buff) */

#define	NL1	000400
#define	NL2	001000
#define	CR2	020000
#define	FF1	040000
#define	TAB1	002000

struct	tty kl11[NKL11+NDL11];
int	klstart();
int	ttrstrt();
char	partab[];

struct device {
	int	rcsr;
	int	rbuf;
	int	tcsr;
	int	tbuf;
};

klopen(dev, flag)
dev_t dev;
{
	register struct device *addr;
	register struct tty *tp;
	register d;

	d = minor(dev);
	if(d >= NKL11+NDL11) {
		u.u_error = ENXIO;
		return;
	}
	tp = &kl11[d];
	/*
	 * set up minor 0 to address KLADDR
	 * set up minor 1 thru NKL11-1 to address from KLBASE
	 * set up minor NKL11 on to address from DLBASE
	 */
	if(d == 0)
		addr = KLADDR;
	else if(d < NKL11)
		addr = KLBASE + (d-1);
	else
		addr = DLBASE + (d-NKL11);
	tp->t_addr = (caddr_t)addr;
	tp->t_oproc = klstart;
	if ((tp->t_state&ISOPEN) == 0) {
		tp->t_state = ISOPEN|CARR_ON;
		tp->t_flags = EVENP|ECHO|XTABS|CRMOD;
		ttychars(tp);
	} else if (tp->t_state&XCLUDE && u.u_uid != 0) {
		u.u_error = EBUSY;
		return;
	}
	addr->rcsr |= IENABLE|DSRDY|RDRENB;
	addr->tcsr |= IENABLE;
#ifndef UCB_LDISC
	ttyopen(dev, tp);
#else UCB_LDISC
	(*linesw[tp->t_line].l_open)(dev, tp);
#endif UCB_LDISC
}

klclose(dev, flag)
dev_t dev;
int flag;
{
	register struct tty *tp;

	tp = &kl11[minor(dev)];
#ifdef UCB_LDISC
	(*linesw[tp->t_line].l_close)(tp);
#endif UCB_LDISC
	ttyclose(tp);
}

klread(dev)
dev_t dev;
{
#ifndef UCB_LDISC
	ttread(&kl11[minor(dev)]);
#else UCB_LDISC
	register struct tty *tp;

	tp = &kl11[minor(dev)];
	(*linesw[tp->t_line].l_read)(tp);
#endif UCB_LDISC
}

klwrite(dev)
dev_t dev;
{
#ifndef UCB_LDISC
	ttwrite(&kl11[minor(dev)]);
#else UCB_LDISC
	register struct tty *tp;

	tp = &kl11[minor(dev)];
	(*linesw[tp->t_line].l_write)(tp);
#endif UCB_LDISC
}

klxint(dev)
dev_t dev;
{
	register struct tty *tp;

	tp = &kl11[minor(dev)];
#ifndef UCB_LDISC
	ttstart(tp);
#else UCB_LDISC
	if (tp->t_line)
		(*linesw[tp->t_line].l_start)(tp);
	else
		ttstart(tp);
#endif UCB_LDISC
	if (tp->t_state&ASLEEP && tp->t_outq.c_cc<=TTLOWAT(tp))
		if (tp->t_chan)
			mcstart(tp->t_chan, (caddr_t)&tp->t_outq);
		else
			wakeup((caddr_t)&tp->t_outq);
}

klrint(dev)
dev_t dev;
{
	register int c;
	register struct device *addr;
	register struct tty *tp;

	tp = &kl11[minor(dev)];
	addr = (struct device *)tp->t_addr;
	c = addr->rbuf;
	addr->rcsr |= RDRENB;
#ifndef UCB_LDISC
	ttyinput(c, tp);
#else UCB_LDISC
	(*linesw[tp->t_line].l_rint)(c, tp);
#endif UCB_LDISC
}

klioctl(dev, cmd, addr, flag)
caddr_t addr;
dev_t dev;
{
#ifndef UCB_LDISC
	if (ttioctl(&kl11[minor(dev)], cmd, addr, flag)) {
#else UCB_LDISC
	register struct tty *tp;

	tp = &kl11[minor(dev)];
	if (ttioctl(tp, cmd, addr, flag) == 0 )
#endif UCB_LDISC
		u.u_error = ENOTTY;
}

klstart(tp)
register struct tty *tp;
{
	register c;
	register struct device *addr;

	addr = (struct device *)tp->t_addr;
	if((addr->tcsr&DONE) == 0)
		return;
	if ((c=getc(&tp->t_outq)) >= 0) {
		if (tp->t_flags&RAW)
			addr->tbuf = c;
		else if (c<=0177)
			addr->tbuf = c | (partab[c]&0200);
		else {
			timeout(ttrstrt, (caddr_t)tp, (c&0177) + DLDELAY);
			tp->t_state |= TIMEOUT;
		}
	}
}

char	*msgbufp = msgbuf;	/* Next saved printf character */
/*
 * Print a character on console.
 * Attempts to save and restore device
 * status.
 * If the last character input from the console was a break
 * (null or del), all printing is inhibited.
 *
 * Whether or not printing is inhibited,
 * the last MSGBUFS characters
 * are saved in msgbuf for inspection later.
 */
putchar(c)
register c;
{
	register s, timo;

	if (c != '\0' && c != '\r' && c != 0177) {
		*msgbufp++ = c;
		if(msgbufp >= &msgbuf[MSGBUFS])
			msgbufp = msgbuf;
	}
	/*
	 *  If last char was a break or null, don't print
	*/
	if ((KLADDR->rbuf&0177) == 0)
		return;
	timo = 30000;
	/*
	 * Try waiting for the console tty to come ready,
	 * otherwise give up after a reasonable time.
	 */
	while((KLADDR->tcsr&0200) == 0)
		if(--timo == 0)
			break;
	if(c == 0)
		return;
	s = KLADDR->tcsr;
	KLADDR->tcsr = 0;
	KLADDR->tbuf = c;
	if(c == '\n') {
		putchar('\r');
		putchar(0177);
		putchar(0177);
	}
	putchar(0);
	KLADDR->tcsr = s;
}
