/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)dhv.c	2.2 (2.11BSD Berkeley) 1997/2/14
 */

/*
 * ported to 2.11BSD (uio logic added) 12/22/91 - SMS
 * based on	dh.c 6.3	84/03/15
 * and on	dmf.c	6.2	84/02/16
 *
 * REAL(tm) dhv driver derived from dhu.c by
 * Steve Nuchia at Baylor 22 November 1987
 *	steve@eyeball.bcm.tmc.edu
 *
 * Dave Johnson, Brown University Computer Science
 *	ddj%brown@csnet-relay
 */

#include "dhv.h"
#if NDHV > 0
/*
 * DHV-11 driver
 */
#include "param.h"
#include "dhvreg.h"
#include "conf.h"
#include "user.h"
#include "file.h"
#include "proc.h"
#include "ioctl.h"
#include "tty.h"
#include "ttychars.h"
#include "clist.h"
#include "map.h"
#include "uba.h"
#include "ubavar.h"
#include "systm.h"
#include <sys/kernel.h>

struct	uba_device dhvinfo[NDHV];

#define	NDHVLINE	(NDHV*8)

#define	UNIT(x)		(minor(x) & 077)
#define CDWAIT(x)	(!(minor(x) & 0200))
#define HWFLOW(x)	(minor(x) & 0100)

#define ISPEED	B9600
#define IFLAGS	(EVENP|ODDP|ECHO)

/*
 * DHV's don't have a very good interrupt facility - you get an
 * interrupt when the first character is put into the silo
 * and nothing after that.  Previously an attempt was made to
 * delay a couple of clock ticks with receive interrupts disabled.
 *
 * Unfortunately the code was ineffective because the number of ticks
 * to delay was decremented if a full (90%) or overrun silo was encountered.
 * After two such events the driver was back in interrupt per character
 * mode thus wasting/negating the whole effort.
 */

char	dhv_hwxon[NDHVLINE];	/* hardware xon/xoff enabled, per line */

/*
 * Baud rates: no 50, 200, or 38400 baud; all other rates are from "Group B".
 *	EXTA => 19200 baud
 *	EXTB => 2000 baud
 */
char	dhv_speeds[] =
	{ 0, 0, 1, 2, 3, 4, 0, 5, 6, 7, 8, 10, 11, 13, 14, 9 };

struct	tty dhv_tty[NDHVLINE];
int	ndhv = NDHVLINE;
int	dhvact;				/* mask of active dhv's */
int	dhv_rcnt[16];
int	dhvstart(), ttrstrt();
long	dhvmctl(),dmtodhv();

#if defined(UCB_CLIST)
extern	ubadr_t clstaddr;
#define	cpaddr(x)	(clstaddr + (ubadr_t)((x) - (char *)cfree))
#else
#define	cpaddr(x)	((u_short)(x))
#endif

/*
 * Routine called to attach a dhv.
 */
dhvattach(addr,unit)
	register caddr_t addr;
	register u_int unit;
{
    if (addr && unit < NDHV && !dhvinfo[unit].ui_addr)
    {
	dhvinfo[unit].ui_unit = unit;
	dhvinfo[unit].ui_addr = addr;
	dhvinfo[unit].ui_alive = 1;
	return (1);
    }
    return (0);
}

/*
 * Open a DHV11 line, mapping the clist onto the uba if this
 * is the first dhv on this uba.  Turn on this dhv if this is
 * the first use of it.
 */
/*ARGSUSED*/
dhvopen(dev, flag)
	dev_t dev;
{
	register struct tty *tp;
	register int unit, dhv;
	register struct dhvdevice *addr;
	register struct uba_device *ui;
	int i, s;

	unit = UNIT(dev);
	dhv = unit >> 3;
	if (unit >= NDHVLINE || (ui = &dhvinfo[dhv])->ui_alive == 0)
		return (ENXIO);
	tp = &dhv_tty[unit];
	if (tp->t_state & TS_XCLUDE && u.u_uid != 0)
		return (EBUSY);
	addr = (struct dhvdevice *)ui->ui_addr;
	tp->t_addr = (caddr_t)addr;
	tp->t_oproc = dhvstart;

	if ((dhvact&(1<<dhv)) == 0) {
		addr->dhvcsr = DHV_SELECT(0) | DHV_IE;
		dhvact |= (1<<dhv);
		/* anything else to configure whole board */
	}

	s = spltty();
	if ( (tp->t_state&TS_ISOPEN) && tp->t_dev == dev )
	{
	    i = (*linesw[tp->t_line].l_open)(dev, tp);
	    (void) splx(s);
	    return(i);
	}

	/*
	 * Wait for carrier, then process line discipline specific open.
	 */
	while ( (tp->t_state&TS_ISOPEN) == 0 || tp->t_dev != dev )
	{
	    if ( tp->t_state & TS_ISOPEN )
	    {
		sleep ( (caddr_t)&tp->t_state, TTIPRI );
		continue;
	    }
	    tp->t_dev = dev;
	    addr->dhvcsr = DHV_SELECT(dev) | DHV_IE;
	    if ( addr->dhvstat & DHV_ST_DCD ) tp->t_state |= TS_CARR_ON;
	    else			      tp->t_state &= ~TS_CARR_ON;
	    if ( CDWAIT(dev) && (tp->t_state & TS_CARR_ON) == 0 ||
		 HWFLOW(dev) && (addr->dhvstat & DHV_ST_DSR) == 0 )
	    {
		dhvmctl ( dev, (long)DHV_ON, DMSET );
		tp->t_state |= TS_WOPEN;
		sleep ( (caddr_t)&tp->t_rawq, TTIPRI );
		continue;
	    }
	    dhvmctl ( dev, (long)DHV_ON, DMSET );
	    break;
	}
	if ( !(tp->t_state&TS_ISOPEN) )
	{
		ttychars(tp);
		tp->t_state |= TS_HUPCLS;
		tp->t_ispeed = ISPEED;
		tp->t_ospeed = ISPEED;
		tp->t_flags = IFLAGS | (HWFLOW(dev) ? TANDEM : 0);
		tp->t_dev = dev;
	}
	if ( !CDWAIT(dev) ) tp->t_state |= TS_CARR_ON;
	dhvparam(unit);
	i = (*linesw[tp->t_line].l_open)(dev, tp);
	(void) splx(s);
	return ( i );
}

/*
 * Close a DHV11 line, turning off the modem control.
 */
/*ARGSUSED*/
dhvclose(dev, flag)
	dev_t dev;
	int flag;
{
	register struct tty *tp;
	register unit, s;

	unit = UNIT(dev);
	tp = &dhv_tty[unit];
	s = spltty();
	if ( !(tp->t_state & TS_ISOPEN) || tp->t_dev != dev ) return;
	(*linesw[tp->t_line].l_close)(tp, flag);
	(void) dhvmctl(unit, (long)DHV_BRK, DMBIC);
	(void) dhvmctl(unit, (long)DHV_OFF, DMSET);
	if ( CDWAIT(tp->t_dev) ||
	    (tp->t_state&(TS_HUPCLS|TS_WOPEN)) ||
	    (tp->t_state&TS_ISOPEN)==0 )
	{
		extern int wakeup();

		/* Hold DTR low for 0.5 seconds */
		timeout(wakeup, (caddr_t) &tp->t_dev, hz/2);
		sleep((caddr_t) &tp->t_dev, PZERO);
	}
	ttyclose(tp);
	splx(s);
	wakeup ( (caddr_t) &tp->t_state );
}

dhvselect ( dev, rw )	/* filter the minor device number */
	dev_t	dev;
	int	rw;
{
    return ( ttselect ( dev & ~0300, rw ) );
}

dhvread(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	register struct tty *tp = &dhv_tty[UNIT(dev)];

    return((*linesw[tp->t_line].l_read) (tp, uio, flag));
}

dhvwrite(dev, uio, flag)
	dev_t dev;
	struct uio *uio;
	int flag;
{
	register struct tty *tp = &dhv_tty[UNIT(dev)];

    return((*linesw[tp->t_line].l_write) (tp, uio, flag));
}

/*
 * DHV11 receiver interrupt.
 */

int	dhvwait;

dhvrint(dhv)
	int	dhv;
{
	register struct tty *tp;
	register c;
	register struct dhvdevice *addr;
	register struct tty *tp0;
	register struct uba_device *ui;
	int line, rcnt;
	int overrun;

	ui = &dhvinfo[dhv];
	if (ui->ui_alive == 0)
		return;
	addr = (struct dhvdevice *)ui->ui_addr;
	tp0 = &dhv_tty[dhv<<3];
	/*
	 * Loop fetching characters from the silo for this
	 * dhv until there are no more in the silo.
	 */
	rcnt = 0;
	overrun = 0;
	while ((c = addr->dhvrbuf) & DHV_RB_VALID )
	{
		rcnt++;
		line = DHV_RX_LINE(c);
		tp = tp0 + line;
		if ((c & DHV_RB_STAT) == DHV_RB_STAT) {
			/*
			 * modem changed or diag info
			 */
			if (c & DHV_RB_DIAG)
			{
			    if ( (c & 0xff) > 0201 )
				printf ( "dhv%d: diagnostic %o\n",
							dhv, c & 0xff );
			    continue;
			}
			if ( CDWAIT(tp->t_dev) || (tp->t_flags & MDMBUF) )
			    (*linesw[tp->t_line].l_modem)
				( tp, (c & DHV_ST_DCD) != 0 );
			if ( HWFLOW(tp->t_dev) )
			{
			    if ( c & DHV_ST_DSR )
			    {
				tp->t_state &= ~TS_TTSTOP;
				ttstart(tp);
			    }
			    else
			    {
				tp->t_state |= TS_TTSTOP;
				dhvstop (tp, 0);
			    }
			}
			continue;
		}
		if ((tp->t_state&TS_ISOPEN) == 0)
		{
		    wakeup((caddr_t)&tp->t_rawq);
		    continue;
		}
		if ( c & (DHV_RB_PE|DHV_RB_DO|DHV_RB_FE) )
		{
		    if (c & DHV_RB_PE)
			    if ((tp->t_flags&(EVENP|ODDP)) == EVENP ||
				(tp->t_flags&(EVENP|ODDP)) == ODDP)
				    continue;
		    if ( (c & DHV_RB_DO) && !overrun )
		    {
			overrun = 1;
			    /* bit-bucket the que to free the cpu */
			while ( addr->dhvrbuf & DHV_RB_VALID );
			break;
		    }
		    if (c & DHV_RB_FE)
			/*
			 * At framing error (break) generate
			 * an interrupt (in cooked/cbreak mode)
			 * or let the char through in RAW mode
			 * for autobauding getty's.
			 */
			if ( !(tp->t_flags&RAW) )
			    c = tp->t_intrc;
		}
#if NBK > 0
		if (tp->t_line == NETLDISC) {
			c &= 0x7f;
			BKINPUT(c, tp);
		} else
#endif
		{
		    if ( !(c & DHV_RB_PE) && dhv_hwxon[(dhv<<3)+line] &&
			((c & 0x7f) == CSTOP || (c & 0x7f) == CSTART) )
				continue;
		    (*linesw[tp->t_line].l_rint)(c, tp);
		}
	}
	if	(overrun)
		printf("dhv%d: silo overflow\n", dhv);
	else if	(rcnt)
		{
		for	(line = 0; rcnt; line++) rcnt >>= 1;
		dhv_rcnt[line]++;
		}
}

/*
 * Ioctl for DHV11.
 */
/*ARGSUSED*/
dhvioctl(dev, cmd, data, flag)
	register dev_t dev;
	u_int cmd;
	caddr_t data;
{
	register struct tty *tp;
	register int unit = UNIT(dev);
	int error;

	tp = &dhv_tty[unit];
	error = (*linesw[tp->t_line].l_ioctl)(tp, cmd, data, flag);
	if (error >= 0)
		return (error);
	error = ttioctl(tp, cmd, data, flag);
	if (error >= 0) {
		if (cmd == TIOCSETP || cmd == TIOCSETN || cmd == TIOCLSET ||
		    cmd == TIOCLBIC || cmd == TIOCLBIS || cmd == TIOCSETD ||
		    cmd == TIOCSETC)
			dhvparam(unit);
		return (error);
	}

	switch (cmd) {
	case TIOCSBRK:
		(void) dhvmctl(unit, (long)DHV_BRK, DMBIS);
		break;

	case TIOCCBRK:
		(void) dhvmctl(unit, (long)DHV_BRK, DMBIC);
		break;

	case TIOCSDTR:
		(void) dhvmctl(unit, (long)DHV_DTR|DHV_RTS, DMBIS);
		break;

	case TIOCCDTR:
		(void) dhvmctl(unit, (long)DHV_DTR|DHV_RTS, DMBIC);
		break;

	case TIOCMSET:
		(void) dhvmctl(dev, dmtodhv(*(int *)data), DMSET);
		break;

	case TIOCMBIS:
		(void) dhvmctl(dev, dmtodhv(*(int *)data), DMBIS);
		break;

	case TIOCMBIC:
		(void) dhvmctl(dev, dmtodhv(*(int *)data), DMBIC);
		break;

	case TIOCMGET:
		*(int *)data = dhvtodm(dhvmctl(dev, 0L, DMGET));
		break;
	default:
		return (ENOTTY);
	}
	return (0);
}

static long
dmtodhv(bits)
	register int bits;
{
	long b = 0;

	if (bits & DML_RTS) b |= DHV_RTS;
	if (bits & DML_DTR) b |= DHV_DTR;
	if (bits & DML_LE) b |= DHV_LE;
	return(b);
}

static
dhvtodm(bits)
	long bits;
{
	register int b = 0;

	if (bits & DHV_DSR) b |= DML_DSR;
	if (bits & DHV_RNG) b |= DML_RNG;
	if (bits & DHV_CAR) b |= DML_CAR;
	if (bits & DHV_CTS) b |= DML_CTS;
	if (bits & DHV_RTS) b |= DML_RTS;
	if (bits & DHV_DTR) b |= DML_DTR;
	if (bits & DHV_LE) b |= DML_LE;
	return(b);
}

/*
 * Set parameters from open or stty into the DHV hardware
 * registers.
 */
static
dhvparam(unit)
	register int unit;
{
	register struct tty *tp;
	register struct dhvdevice *addr;
	register int lpar;
	int s;

	tp = &dhv_tty[unit];
	addr = (struct dhvdevice *)tp->t_addr;
	/*
	 * Block interrupts so parameters will be set
	 * before the line interrupts.
	 */
	s = spltty();
	if ((tp->t_ispeed) == 0) {
		tp->t_state |= TS_HUPCLS;
		(void)dhvmctl(unit, (long)DHV_OFF, DMSET);
		splx(s);
		return;
	}
	lpar = (dhv_speeds[tp->t_ospeed]<<12) | (dhv_speeds[tp->t_ispeed]<<8);
	if ((tp->t_ispeed) == B134)
		lpar |= DHV_LP_BITS6|DHV_LP_PENABLE;
	else if (tp->t_flags & (RAW|L001000|LITOUT|PASS8))
		lpar |= DHV_LP_BITS8;
	else
		lpar |= DHV_LP_BITS7|DHV_LP_PENABLE;
	if (tp->t_flags&EVENP)
		lpar |= DHV_LP_EPAR;
	if ( (tp->t_flags & EVENP) && (tp->t_flags & ODDP) )
	{
		/* hack alert.  assume "allow both" means don't care */
		/* trying to make xon/xoff work with evenp+oddp */
		lpar |= DHV_LP_BITS8;
		lpar &= ~DHV_LP_PENABLE;
	}
	if ((tp->t_ospeed) == B110)
		lpar |= DHV_LP_TWOSB;
	addr->dhvcsr = DHV_SELECT(unit) | DHV_IE;
	addr->dhvlpr = lpar;
	dhv_hwxon[unit] = !(tp->t_flags & RAW) &&
		(tp->t_line == OTTYDISC || tp->t_line == NTTYDISC) &&
		tp->t_stopc == CSTOP && tp->t_startc == CSTART;
	if ( dhv_hwxon[unit] )
	    addr->dhvlcr |= DHV_LC_OAUTOF;
	else
	{
	    addr->dhvlcr &= ~DHV_LC_OAUTOF;
	    delay(25L); /* see the dhv manual, sec 3.3.6 */
	    addr->dhvlcr2 |= DHV_LC2_TXEN;
	}
	splx(s);
}

/*
 * DHV11 transmitter interrupt.
 * Restart each line which used to be active but has
 * terminated transmission since the last interrupt.
 */
dhvxint(dhv)
	int dhv;
{
	register struct tty *tp;
	register struct dhvdevice *addr;
	register struct tty *tp0;
	register struct uba_device *ui;
	register int line, t;
	u_short cntr;
	ubadr_t base;

	ui = &dhvinfo[dhv];
	tp0 = &dhv_tty[dhv<<4];
	addr = (struct dhvdevice *)ui->ui_addr;
	while ((t = addr->dhvcsrh) & DHV_CSH_TI) {
		line = DHV_TX_LINE(t);
		tp = tp0 + line;
		tp->t_state &= ~TS_BUSY;
		if (t & DHV_CSH_NXM) {
			printf("dhv(%d,%d): NXM fault\n", dhv, line);
			/* SHOULD RESTART OR SOMETHING... */
		}
		if (tp->t_state&TS_FLUSH)
			tp->t_state &= ~TS_FLUSH;
		else {
			addr->dhvcsrl = DHV_SELECT(line) | DHV_IE;
			base = (ubadr_t) addr->dhvbar1;
			/*
			 * Clists are either:
			 *	1)  in kernel virtual space,
			 *	    which in turn lies in the
			 *	    first 64K of physical memory or
			 *	2)  at UNIBUS virtual address 0.
			 *
			 * In either case, the extension bits are 0.
			 */
			if (!ubmap)
			      base |= (ubadr_t)((addr->dhvbar2 & 037) << 16);
		        cntr = base - cpaddr(tp->t_outq.c_cf);
			ndflush(&tp->t_outq,cntr);
		}
		if (tp->t_line)
			(*linesw[tp->t_line].l_start)(tp);
		else
			dhvstart(tp);
	}
}

/*
 * Start (restart) transmission on the given DHV11 line.
 */
dhvstart(tp)
	register struct tty *tp;
{
	register struct dhvdevice *addr;
	register int unit, nch;
	ubadr_t car;
	int s;

	unit = UNIT(tp->t_dev);
	addr = (struct dhvdevice *)tp->t_addr;

	/*
	 * Must hold interrupts in following code to prevent
	 * state of the tp from changing.
	 */
	s = spltty();
	/*
	 * If it's currently active, or delaying, no need to do anything.
	 */
	if (tp->t_state&(TS_TIMEOUT|TS_BUSY|TS_TTSTOP))
		goto out;
	/*
	 * If there are sleepers, and output has drained below low
	 * water mark, wake up the sleepers..
	 */
	if (tp->t_outq.c_cc<=TTLOWAT(tp)) {
		if (tp->t_state&TS_ASLEEP) {
			tp->t_state &= ~TS_ASLEEP;
			wakeup((caddr_t)&tp->t_outq);
		}
		if (tp->t_wsel) {
			selwakeup(tp->t_wsel, tp->t_state & TS_WCOLL);
			tp->t_wsel = 0;
			tp->t_state &= ~TS_WCOLL;
		}
	}
	/*
	 * Now restart transmission unless the output queue is
	 * empty.
	 */
	if (tp->t_outq.c_cc == 0)
		goto out;
	if (tp->t_flags & (RAW|L001000|LITOUT))
		nch = ndqb(&tp->t_outq, 0);
	else {
		nch = ndqb(&tp->t_outq, 0200);
		/*
		 * If first thing on queue is a delay process it.
		 */
		if (nch == 0) {
			nch = getc(&tp->t_outq);
			timeout(ttrstrt, (caddr_t)tp, (nch&0x7f)+6);
			tp->t_state |= TS_TIMEOUT;
			goto out;
		}
	}
	/*
	 * If characters to transmit, restart transmission.
	 */
	if (nch) {
		car = cpaddr(tp->t_outq.c_cf);
		addr->dhvcsrl = DHV_SELECT(unit) | DHV_IE;
		addr->dhvlcr &= ~DHV_LC_TXABORT;
		addr->dhvbcr = nch;
		addr->dhvbar1 = loint(car);
		if (ubmap)
			addr->dhvbar2 = (hiint(car) & DHV_BA2_XBA) | DHV_BA2_DMAGO;
		else
			addr->dhvbar2 = (hiint(car) & 037) | DHV_BA2_DMAGO;
		tp->t_state |= TS_BUSY;
	}
out:
	splx(s);
}

/*
 * Stop output on a line, e.g. for ^S/^Q or output flush.
 */
/*ARGSUSED*/
dhvstop(tp, flag)
	register struct tty *tp;
{
	register struct dhvdevice *addr;
	register int unit, s;

	addr = (struct dhvdevice *)tp->t_addr;
	/*
	 * Block input/output interrupts while messing with state.
	 */
	s = spltty();
	if (tp->t_state & TS_BUSY) {
		/*
		 * Device is transmitting; stop output
		 * by selecting the line and setting the
		 * abort xmit bit.  We will get an xmit interrupt,
		 * where we will figure out where to continue the
		 * next time the transmitter is enabled.  If
		 * TS_FLUSH is set, the outq will be flushed.
		 * In either case, dhvstart will clear the TXABORT bit.
		 */
		unit = UNIT(tp->t_dev);
		addr->dhvcsrl = DHV_SELECT(unit) | DHV_IE;
		addr->dhvlcr |= DHV_LC_TXABORT;
		delay(25L); /* see the dhv manual, sec 3.3.6 */
		addr->dhvlcr2 |= DHV_LC2_TXEN;
		if ((tp->t_state&TS_TTSTOP)==0)
			tp->t_state |= TS_FLUSH;
	}
	(void) splx(s);
}

/*
 * DHV11 modem control
 */
static long
dhvmctl(dev, bits, how)
	dev_t dev;
	long bits;
	int how;
{
	register struct dhvdevice *dhvaddr;
	register int unit;
	register struct tty *tp;
	long mbits;
	int s;

	unit = UNIT(dev);
	tp = dhv_tty + unit;
	dhvaddr = (struct dhvdevice *) tp->t_addr;
	s = spltty();
	dhvaddr->dhvcsr = DHV_SELECT(unit) | DHV_IE;
	/*
	 * combine byte from stat register (read only, bits 16..23)
	 * with lcr register (read write, bits 0..15).
	 */
	mbits = (u_short)dhvaddr->dhvlcr | ((long)dhvaddr->dhvstat << 16);
	switch (how) {

	case DMSET:
		mbits = (mbits & 0xff0000L) | bits;
		break;

	case DMBIS:
		mbits |= bits;
		break;

	case DMBIC:
		mbits &= ~bits;
		break;

	case DMGET:
		(void) splx(s);
		return(mbits);
	}
	dhvaddr->dhvlcr = (mbits & 0xffff) | DHV_LC_RXEN;
	if ( dhv_hwxon[unit] )
	    dhvaddr->dhvlcr |= DHV_LC_OAUTOF;
	dhvaddr->dhvlcr2 = DHV_LC2_TXEN;
	(void) splx(s);
	return(mbits);
}
#endif
