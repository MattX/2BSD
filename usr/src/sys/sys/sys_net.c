/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	%W% (Berkeley) %G%
 */

#include "param.h"
#include "../machine/cons.h"
#include "../machine/psl.h"

#include "user.h"
#include "uio.h"
#include "map.h"
#include "mbuf.h"
#include "acct.h"

#include "../pdpuba/ubavar.h"

#include "acc.h"
#if NACC > 0
extern struct uba_driver accdriver;
#endif

#include "css.h"
#if NCSS > 0
extern struct uba_driver cssdriver;
#endif

#include "de.h"
#if NDE > 0
extern struct uba_driver dedriver;
#endif

#include "ec.h"
#if NEC > 0
extern struct uba_driver ecdriver;
#endif

#include "il.h"
#if NIL > 0
extern struct uba_driver ildriver;
#endif

#include "qe.h"
#if NQE > 0
extern struct uba_driver qedriver;
#endif

#include "sri.h"
#if NSRI > 0
extern struct uba_driver sridriver;
#endif

#include "vv.h"
#if NVV > 0
extern struct uba_driver vvdriver;
#endif

static struct uba_device ubdinit[] = {
#if NDE > 0
	{ &dedriver,	0,0, (caddr_t)0174510 },
#endif
#if NIL > 0
	{ &ildriver,	0,0, (caddr_t)0164000 },
#endif
#if NQE > 0
	{ &qedriver,	0,0, (caddr_t)0174440 },
#endif
#if NSRI > 0
	{ &sridriver,	0,0, (caddr_t)0167770 },
#endif
#if NVV > 0
	{ &vvdriver,	0,0, (caddr_t)0161000 },
#endif
#if NEC > 0
	{ &ecdriver,	0,0, (caddr_t)0164330 },
#endif
#if NACC > 0
	{ &accdriver,	0,0, (caddr_t)0000000 },
#endif
#if NCSS > 0
	{ &cssdriver,	0,0, (caddr_t)0000000 },
#endif
	NULL,
};

int hz = LINEHZ;

#ifdef UNIBUS_MAP
bool_t ubmap = 1;		/* assume we have a unibus map */
#endif

long startnet;			/* start of network data space */

netstart()
{
	extern memaddr miobase, netdata;
	extern u_short miosize;
	register struct uba_driver *udp;
	register struct uba_device *ui = ubdinit;
	register int s;

	/*
	 * The networking uses a mapped region as the DMA area for
	 * network interface drivers.  Allocate this latter region.
	 */
	if ((miobase = MALLOC(coremap, btoc(miosize))) == 0)
		panic("miobase");

	startnet = ctob((long)mfkd(&netdata));

	mbinit();
	for (ui = ubdinit; udp = ui->ui_driver; ++ui) {
		if (badaddr(ui->ui_addr, 2))
			continue;
		ui->ui_alive = 1;
		udp->ud_dinfo[ui->ui_unit] = ui;
		printf("attaching %s%d\n", udp->ud_dname, ui->ui_unit);
		(*udp->ud_attach)(ui);
	}
#include "sl.h"
#if NSL > 0
	printf("attaching sl\n");
	slattach();
#endif

#include "loop.h"
#if NLOOP > 0
	printf("attaching lo0\n");
	loattach();
#endif

	s = splimp();
	ifinit();
	domaininit();			/* must follow interfaces */
	splx(s);
}

/*
 * Panic is called on fatal errors.  It prints "net panic: mesg" and
 * then calls the kernel entry netcrash() to bring down the net.
 */
panic(s)
	register char *s;
{
	printf("net panic: %s\n", s);
	NETCRASH();
	/* NOTREACHED */
}

netcopyout(m, to, len)
	struct mbuf *m;
	char *to;
	int *len;
{
	if (*len > m->m_len)
		*len = m->m_len;
	return(copyout(mtod(m, caddr_t), to, *len));
}

/*
 * These routines are copies of various kernel routines that are required in
 * the supervisor space by the networking.  DO NOT MODIFY THESE ROUTINES!
 * Modify the kernel version and bring a copy here.  The "#ifdef notdef"
 * modifications simplify by eliminating portions of routines not required
 * by the networking.
 */

/* copied from kern_descrip.c */
ufavail()
{
	register int i, avail = 0;

	for (i = 0; i < NOFILE; i++)
		if (u.u_ofile[i] == NULL)
			avail++;
	return (avail);
}

/* copied from kern_descrip.c */
ufalloc(i)
	register int i;
{

	for (; i < NOFILE; i++)
		if (u.u_ofile[i] == NULL) {
			u.u_r.r_val1 = i;
			u.u_pofile[i] = 0;
			if (i > u.u_lastfile)
				u.u_lastfile = i;
			return (i);
		}
	u.u_error = EMFILE;
	return (-1);
}

/* copied from ufs_fio.c */
suser()
{

	if (u.u_uid == 0) {
		u.u_acflag |= ASU;
		return (1);
	}
	u.u_error = EPERM;
	return (0);
}

/*
 * An old version of uiomove, as uiofmove() and vcopy{in,out}() don't
 * exist in supervisor space.  Note, we assume that all transfers will
 * be to/from user D space.  Probably safe, until someone decides to
 * put NFS into the kernel.
 */
uiomove(cp, n, rw)
	register caddr_t cp;
	register u_int n;
	enum uio_rw rw;
{
	if (!n)
		return (0);
#ifdef DIAGNOSTIC
	if (u.u_segflg != UIO_USERSPACE)
		panic("segflag != UIO_USERSPACE");
#endif
	if ((n | (int)cp | (int)u.u_base)&01) {
		if (rw == UIO_READ) {
			do {
				if (ureadc(*cp++) < 0)
					return(EFAULT);
			} while (--n);
		}
		else {
			register int ch;

			do {
				if ((ch = uwritec()) < 0)
					return(EFAULT);
				*cp++ = ch;
			} while (--n);
		}
	}
	else {
		register int error;

		if (rw == UIO_READ)
			error = copyout(cp, u.u_base, n);
		else
			error = copyin(u.u_base, cp, n);
		if (error)
			return(error);
		u.u_base += n;
		u.u_count -= n;
		u.u_offset += n;
	}
	return(0);
}

/* copied from kern_subr.c */
ureadc(c)
	register int c;
{
#ifdef notdef
	switch (u.u_segflg) {

	case UIO_USERSPACE:
		if (subyte(u.u_base, c) < 0)
			return (EFAULT);
		break;

	case UIO_SYSSPACE:
		*u.u_base = c;
		break;

	case UIO_USERISPACE:
		if (suibyte(u.u_base, c) < 0)
			return (EFAULT);
		break;
	}
#else
	if (subyte(u.u_base, c) < 0)
		return (EFAULT);
#endif
	u.u_base++;
	u.u_count--;
	u.u_offset++;
	return (0);
}

/* copied from kern_subr.c */
uwritec()
{
	register int c;

	if (!u.u_count)
		return (-1);
#ifdef notdef
	switch (u.u_segflg) {

	case UIO_USERSPACE:
		c = fubyte(u.u_base);
		break;

	case UIO_SYSSPACE:
		c = *u.u_base & 0377;
		break;

	case UIO_USERISPACE:
		c = fuibyte(u.u_base);
		break;
	}
#else
	c = fubyte(u.u_base);
#endif
	if (c < 0)
		return (-1);
	u.u_base++;
	u.u_count--;
	u.u_offset++;
	return (c & 0377);
}

#define TOCONS	0x1
#define TOTTY	0x2
#define TOLOG	0x4

/* copied from subr_prf.c */
/*VARARGS1*/
printf(fmt, x1)
	char *fmt;
	unsigned x1;
{
	prf(fmt, &x1, TOCONS | TOLOG);
}

#define	putchar(c, flags)	cnputc(c)

/* copied from subr_prf.c */
prf(fmt, adx, flags)
	register char *fmt;
	register u_int *adx;
	int flags;
{
	register int c;
	u_int b;
	char *s;
	int	i, any;

loop:
	while ((c = *fmt++) != '%') {
		if (c == '\0')
			return;
		putchar(c, flags);
	}
	c = *fmt++;
	switch (c) {

	case 'l':
		c = *fmt++;
		switch(c) {
			case 'x':
				b = 16;
				goto lnumber;
			case 'd':
				b = 10;
				goto lnumber;
			case 'o':
				b = 8;
				goto lnumber;
			default:
				putchar('%', flags);
				putchar('l', flags);
				putchar(c, flags);
		}
		break;
	case 'X':
		b = 16;
		goto lnumber;
	case 'D':
		b = 10;
		goto lnumber;
	case 'O':
		b = 8;
lnumber:	printn(*(long *)adx, b, flags);
		adx += (sizeof(long) / sizeof(int)) - 1;
		break;
	case 'x':
		b = 16;	
		goto number;
	case 'd':
	case 'u':		/* what a joke */
		b = 10;
		goto number;
	case 'o':
		b = 8;
number:		printn((long)*adx, b, flags);
		break;
	case 'c':
		putchar(*adx, flags);
		break;
	case 'b':
		b = *adx++;
		s = (char *)*adx;
		printn((long)b, *s++, flags);
		any = 0;
		if (b) {
			while (i = *s++) {
				if (b & (1 << (i - 1))) {
					putchar(any? ',' : '<', flags);
					any = 1;
					for (; (c = *s) > 32; s++)
						putchar(c, flags);
				} else
					for (; *s > 32; s++)
						;
			}
			if (any)
				putchar('>', flags);
		}
		break;
	case 's':
		s = (char *)*adx;
		while (c = *s++)
			putchar(c, flags);
		break;
	case '%':
		putchar(c, flags);
		break;
	default:
		putchar('%', flags);
		putchar(c, flags);
		break;
	}
	adx++;
	goto loop;
}

/* copied from subr_prf.c */
printn(n, b, flags)
	long n;
	u_int b;
{
	char prbuf[12];
	register char *cp = prbuf;
	register int offset = 0;

	if (n<0)
		switch(b) {
		case 8:		/* unchecked, but should be like hex case */
		case 16:
			offset = b-1;
			n++;
			break;
		case 10:
			putchar('-', flags);
			n = -n;
			break;
		}
	do {
		*cp++ = "0123456789ABCDEF"[offset + n%b];
	} while (n = n/b);	/* Avoid  n /= b, since that requires alrem */
	do
		putchar(*--cp, flags);
	while (cp > prbuf);
}

/* copied from cons.c */
cnputc(c)
	int c;
{
	register struct dldevice *cnaddr = (struct dldevice *)0177560;
	register int s, timo;

	timo = 30000;
	/*
	 * Try waiting for the console tty to come ready,
	 * otherwise give up after a reasonable time.
	 */
	while ((cnaddr->dlxcsr & DLXCSR_TRDY) == 0)
		if (--timo == 0)
			break;
	if (c == 0)
		return;
	s = cnaddr->dlxcsr;
	cnaddr->dlxcsr = 0;
	cnaddr->dlxbuf = c&0xff;
	if (c == '\n')
		cnputc('\r');
	cnputc(0);
	cnaddr->dlxcsr = s;
}
