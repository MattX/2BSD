#
int	mux_lst;	/* for counting lost chars	*/
int	mux_lsd;	/* for counting lost chars	*/
int	mux_bad;
int	mux_full;
int	mux_over;
int	mux_rbad;
int	mux_tbad;
int	mux_xint;
int	mux_rint;
int	mux_seq;
int	mux_on=3;
/*
 *   CORY MULTIPLEXER DRIVER
 */
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/conf.h>
#include <sys/user.h>
#include <sys/tty.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)mx.c	3.2";
#endif


#define NMUX 2		/* number of mux's attached  */
#define	MUX1ADDR	0160040	/* mpxr base address */
#define	MUX2ADDR	0160100	/* mpxr base address */
#define	NPORTS	64	/* total number of ports */
#define MUXDLY	30	/* delay count for mux buf to drain */
#define NCHAR	10	/* maximum number of sequential outputs for 1 port */
#define DSRDY	02
#define	RDRENB	01

/*
 * multiplexer commands
 */
#define MUXCTRL	0200	/* control bit */
#define MUXRST	0200	/* restart system */
#define MUXRW	0240	/* send data,  receive data header */
#define MUXRBFUL	0300	/* rcvr buff full **/
#define MUXSTAT	0340	/* status */
/*
 * multiplexer returned status flags
 */
#define MUXXRDY	01	/* xmitter ready for new character */
#define MUXRRDY	02	/* rcvr ready for new character */
#define MUXTBE	04	/* xmitter buffer empty */
#define MUXPE	010	/* parity error */
#define MUXOR	020	/* overrun */
#define MUXFE	040	/* framing error */
/*
 * multiplexer set mode bits
 */
#define MUXTXE	01	/* xmitter on */
#define MUXRXE	04	/* rcvr on */
#define MUXER	020	/* reset error flag */
#define MUXIR	0100	/* resert usart to mode */
/*
 * usart mode bits
 */
#define STOP1	0100	/* 1 stop bit */
#define STOP15	0200	/* 1.5 stop bits */
#define STOP2	0300	/* 2 stop bits */
#define MUXEVEN	040	/* even parity */
#define MUXPAR	020	/* parity enable */
#define LEN8	014	/* character length 8 */
#define LEN7	010	/* character length 7 */
#define LEN6	04	/* character length 6 */
#define LEN5	0	/* character length 5 */
#define HIGH	02	/* high speed */
#define LOW	03	/* low speed */
#define CSEND	01	/* send saved character */
#define TAG	02	/* tag ok (next character valid) */

/*
 * the queue variables for mux
 */
struct muxdata {
	int	mux_count, mux_qhead, mux_qtail, mux_entrys, mux_lastr, mux_flag;
	struct	tty *mux_queue[32];
} muxd[NMUX];
/*
 * the mux addresses
 */
int	muxaddr[NMUX] {
	MUX1ADDR,
	MUX2ADDR,
};
/*
 * the mux active flag.  bit 1 = mux, bit 2 = mux2, etc
 */
int	mux_active;

struct	tty mux[NPORTS];

int	nmuxtty	NPORTS;			/* for pstat */

struct klregs {
	int klrcsr;
	int klrbuf;
	int kltcsr;
	int kltbuf;
}

muxopen(dev, flag)
{
	register struct tty *tp;
	extern muxstart();
	static muxrst = 0;	/* initialized to force into data, not bss */

	if(minor(dev) >= NPORTS) {
		u.u_error = ENXIO;
		return;
	}
	if ((mux_on&(1<<(minor(dev)/32))) == 0) {
		u.u_error = ENXIO;
		 return;
	}
	if (muxrst == 0) {
		muxrst = 1;
		muxreset();
	}
	tp = &mux[minor(dev)];
	tp->t_oproc = muxstart;
	tp->t_iproc = NULL;
	if ((tp->t_state&ISOPEN) == 0) {
		tp->t_state = ISOPEN|CARR_ON;
		tp->t_flags = XTABS|LCASE|ECHO|CRMOD|ODDP|EVENP;
		ttychars(tp);
	}
	muxaddr[(tp-mux)/32]->klrcsr =| IENABLE|DSRDY|RDRENB;
	muxaddr[(tp-mux)/32]->kltcsr =| IENABLE;
#ifndef UCB_LDISC
	ttyopen(dev, tp);
#else UCB_LDISC
	(*linesw[tp->t_line].l_open)(dev, tp);
#endif UCB_LDISC
}

muxclose(dev)
{
	register struct tty *tp;

	tp = &mux[minor(dev)];
#ifdef UCB_LDISC
	(*linesw[tp->t_line].l_close)(tp);
#endif UCB_LDISC
	ttyclose(tp);
}

muxread(dev)
{
#ifndef UCB_LDISC
	ttread(&mux[minor(dev)], dev);
#else UCB_LDISC
	register struct tty *tp;

	tp = &mux[minor(dev)];
	(*linesw[tp->t_line].l_read)(tp, dev);
#endif UCB_LDISC
}

muxwrite(dev)
{
#ifndef UCB_LDISC
	ttwrite(&mux[minor(dev)]);
#else UCB_LDISC
	register struct tty *tp;

	tp = &mux[minor(dev)];
	(*linesw[tp->t_line].l_write)(tp);
#endif UCB_LDISC
}

muxreset()
{
	register i;

	for (i=0; i<NMUX; i++)
		if (mux_on&(1<<i))
			muxaddr[i]->kltbuf = MUXRST;
}

muxstart(tp)
struct	tty *tp;
{
	register struct tty *rtp;
	register struct muxdata *muxp;
	register rmux;
	int sps, ttrstrt();

	sps = spl6();
	rtp = tp;
	muxp = &muxd[(rmux = (rtp-mux)>>5)];
	if (rtp->t_state & (TIMEOUT|TTSTOP))
		goto out;
	if ((rtp->t_state & BUSY) == 0)
	{
		if (++muxp->mux_entrys > 32)
		{
			printf("mux queue overflow\n");
			muxp->mux_entrys = 32;
		}
		muxp->mux_queue[muxp->mux_qtail] = rtp;
		muxp->mux_qtail = (muxp->mux_qtail + 1) & 31;
		rtp->t_state =| BUSY;
	}
	if ((mux_active & (1 << rmux)) == 0)
	{
		mux_active =| (1 << rmux);
		muxaddr[rmux]->kltbuf = 
			MUXRW | ((muxp->mux_queue[muxp->mux_qhead] - mux) & 31);
		muxp->mux_count = 0;
	}
out:
	splx(sps);
}

muxxint(dev)
{
	register struct tty *port;
	register struct muxdata *muxp;
	register c;

/*  Monitoring removed for version 7 upgrade.
	tk_busy = 1;
*/
	mux_xint++;
	if ((muxaddr[dev]->kltcsr & DONE) == 0) {
		mux_tbad++;
		return;
	}
	muxp = &muxd[dev];
	port = muxp->mux_queue[muxp->mux_qhead];
	if (++(muxp->mux_count) > NCHAR || port->t_state & (TTSTOP|TIMEOUT))
	{
		muxp->mux_qhead = (muxp->mux_qhead + 1) & 31;
		if (port->t_state & (TTSTOP|TIMEOUT))
		{
			muxp->mux_entrys--;
			port->t_state =& ~BUSY;
		}
		else
		{
			muxp->mux_queue[muxp->mux_qtail] = port;
			muxp->mux_qtail = (muxp->mux_qtail + 1) & 31;
		}
	}
#ifdef UCB_LDISC
	/*
	else if (port->t_line)
		(*linesw[port->t_line].l_start)(port);
		*/
#endif UCB_LDISC
	else
	{
		if (port->t_outq.c_cc<=TTLOWAT(port) && port->t_state&ASLEEP)
		{
			port->t_state =& ~ASLEEP;
			wakeup(&port->t_outq);
		}
		if (port->t_outq.c_cc > 0)
		{
			muxaddr[dev]->kltbuf = getc(&port->t_outq) & 0177;
			return;
		}
		else
		{
			muxp->mux_qhead = (muxp->mux_qhead + 1) & 31;
			port->t_state =& ~BUSY;
			muxp->mux_entrys--;
		}
	}
	muxp->mux_count = 0;
	for (; muxp->mux_entrys != 0; muxp->mux_entrys--)
	{
		port = muxp->mux_queue[muxp->mux_qhead];
		if (port->t_outq.c_cc==0 || (port->t_state&(TTSTOP|TIMEOUT)))
		{
			port->t_state =& ~BUSY;
			muxp->mux_qhead = (muxp->mux_qhead + 1) & 31;
		}
		else
		{
			muxaddr[dev]->kltbuf = MUXRW | ((port - mux) & 31);
			return;
		}
	}
	mux_active =& ~(1 << dev);
/*  Monitoring removed for version 7 upgrade.
	tk_busy = 0;
*/
}

muxrint(dev)
{
	register *muxp;
	register c;
	register struct tty *tp;
	int ttrstrt();
	int temp;

/*  Monitoring removed for version 7 upgrade.
	tk_busy = 1;
*/
	mux_rint++;
	if (((muxp = muxaddr[dev])->klrcsr&DONE) == 0)
		mux_rbad++;
	if (muxp->klrcsr & 040000)
		mux_over++;
	temp = 0;
	while (muxp->klrcsr & DONE) {
		temp++;
		c = muxp->klrbuf;
		tp = &mux[muxd[dev].mux_lastr];
		/*
		 * check for data word
		 */
		if ((c & MUXCTRL) == 0) {
			if ((tp->t_state&ISOPEN) == 0)
				continue;
			if (muxd[dev].mux_flag & TAG) {
				muxd[dev].mux_flag =& ~TAG;
#ifndef UCB_LDISC
				ttyinput(c, tp);
#else UCB_LDISC
				(*linesw[tp->t_line].l_rint)(c, tp);
#endif UCB_LDISC
			} else
				mux_lst++;
			continue;
		}
		/*
		 * decode control code
		 */
		if (muxd[dev].mux_flag & TAG && (c & 0340) != MUXRBFUL)
			mux_lsd++;
		switch (c & 0340) {
			case MUXRW:	/* read/write control word */
				c = (c & 037) | (dev<<5);
				if (c >= NPORTS) {
					mux_bad++;
					break;
				}
				muxd[dev].mux_lastr = c;
				muxd[dev].mux_flag =| TAG;
				break;
			case MUXRBFUL:	/* recv buffer full */
				mux_full++;
				c = (c & 037) | (dev<<5);
				if (c >= NPORTS) {
					mux_bad++;
					break;
				}
				tp = &mux[c];
				if (tp->t_state&TIMEOUT || (tp->t_state&ISOPEN)==0)
					continue;
				timeout(ttrstrt, tp, MUXDLY);
				tp->t_state =| TIMEOUT;
				break;
		}
	}
	muxp->klrcsr =| RDRENB;
	mux_seq = temp;
/*  Monitoring removed for version 7 upgrade.
	tk_busy=0;
*/
}


muxioctl(dev, cmd, addr, flag)
{
	register	struct	tty	*tp;

	dev = minor(dev);
	tp = &mux[dev];
	if (!ttioctl(tp, cmd, addr, flag))
		u.u_error = ENOTTY;
}
