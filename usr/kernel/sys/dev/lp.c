/*
 *  Line printer driver
 *
 *  Notice nonstandard indirection in struct device
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/tty.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)lp.c	3.2";
#endif


struct device *lp_addr[] = {
#ifndef MENLO70
	(struct device *) 0177514
#else
	(struct device *) 0164004 ,
	(struct device *) 0177524
#endif
};

#define	NLP	2		/* number of line printers */
int	lp_cnt	= { NLP };

#define	LPPRI	(PZERO+8)
#define	LPLOWAT	40
#define	LPHIWAT	400
#define	LPMAX	2
#define LPINTEN	0100

struct device {
	int	lpcsr, lpbuf;
};

struct lp {
	struct	clist l_outq;
	char	flag, ind;
	int	ccc, mcc, mlc;
	int	line, col;
} lp_dt[LPMAX];

#define	OPEN	010
#define	CAP	020
#define	NOCR	040
#define	ASLP	0100

#define	FORM	014

lpopen(dev, flag)
{
	register unit;
	register struct lp *lp;

	unit = dev&07;
	if (unit >= lp_cnt || unit >= LPMAX ||
	 (lp = &lp_dt[unit])->flag || lp_addr[unit]->lpcsr <0 ) {
		u.u_error = EIO;
		return;
	}
	lp->flag = (dev&077)|OPEN;
	lp->ind = 0;
	lp->col = 132;
	lp->line = 60;
	lp_addr[unit]->lpcsr |= IENABLE;
	lpoutput(unit, FORM);
}

lpclose(dev)
{
	register unit;

	unit = dev&07;
	lpoutput(unit, FORM);
	lp_dt[unit].flag = 0;
}

lpwrite(dev)
{
	register unit;
	register c;
	register struct lp *lp;

	unit = dev&07;
	lp = &lp_dt[unit];
	while (u.u_count) {
		spl4();
		while(lp->l_outq.c_cc > LPHIWAT) {
			lpintr(unit);
			lp->flag |= ASLP;
			sleep(lp, LPPRI);
		}
		spl0();
		c = fubyte(u.u_base++);
		if (c < 0) {
			u.u_error = EFAULT;
			break;
		}
		u.u_count--;
		lpoutput(unit, c);
	}
	spl4();
	lpintr(unit);
	spl0();
}

lpoutput(dev, c)
register dev, c;
{
	register struct lp *lp;

	lp = &lp_dt[dev];
	if(lp->flag&CAP) {
		if(c>='a' && c<='z')
			c += 'A'-'a'; else
		switch(c) {
		case '{':
			c = '(';
			goto esc;
		case '}':
			c = ')';
			goto esc;
		case '`':
			c = '\'';
			goto esc;
		case '|':
			c = '!';
			goto esc;
		case '~':
			c = '^';
		esc:
			lpoutput(dev, c);
			lp->ccc--;
			c = '-';
		}
	}
	switch(c) {
	case '\t':
		lp->ccc = ((lp->ccc+8-lp->ind) & ~7) + lp->ind;
		return;
	case '\n':
		lp->mlc++;
		if(lp->mlc >= lp->line )
			c = FORM;
	case FORM:
		lp->mcc = 0;
		if (lp->mlc) {
			putc(c, &lp->l_outq);
			if(c == FORM)
				lp->mlc = 0;
		}
	case '\r':
		lp->ccc = lp->ind;
		spl4();
		lpintr(dev);
		spl0();
		return;
	case 010:
		if(lp->ccc > lp->ind)
			lp->ccc--;
		return;
	case ' ':
		lp->ccc++;
		return;
	default:
		if(lp->ccc < lp->mcc) {
			if (lp->flag&NOCR) {
				lp->ccc++;
				return;
			}
			putc('\r', &lp->l_outq);
			lp->mcc = 0;
		}
		if(lp->ccc <= lp->col) {
			while(lp->ccc > lp->mcc) {
				putc(' ', &lp->l_outq);
				lp->mcc++;
			}
			putc(c, &lp->l_outq);
			lp->mcc++;
		}
		lp->ccc++;
	}
}

lpintr(dev)
register dev;
{
	register struct lp *lp;
	register c;

	lp = &lp_dt[dev];
	while (lp_addr[dev]->lpcsr&DONE && (c = getc(&lp->l_outq)) >= 0)
		lp_addr[dev]->lpbuf = c;
	if (lp->l_outq.c_cc <= LPLOWAT && lp->flag&ASLP) {
		lp->flag &= ~ASLP;
		wakeup(lp);
	}
}

lpioctl(dev, cmd, addr, flag)
dev_t	dev;
int	cmd;
int	*addr;
int	flag;
{
	register struct lp *lp;
	union { char lobyte, hibyte;
		 int w;		 }	lp_wd;

	lp = &lp_dt[dev&07];
	switch (cmd)
	{

case TIOCGETP:
		lp_wd.lobyte = (char)lp->flag;
		lp_wd.hibyte = (char)lp->ind;
		suword(addr,lp_wd);
		suword(++addr, lp->line);
		suword(++addr, lp->col);
		break;

case TIOCSETP:
case TIOCSETN:
		lp_wd.w = fuword(addr);
		lp->flag = (lp_wd.lobyte&077)|OPEN;
		lp->ind = (int)lp_wd.hibyte&017;
		lp->line = fuword(++addr);
		lp->col = fuword(++addr);
		break;

default:
		u.u_error = ENOTTY;
	}
}
