/*
 * Copyright (c) 2020 Anders Magnusson (ragge@tethuvudet.se).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)wbuf.c      1.0 (2.11BSD) 2025/12/26";
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "local.h"

#define	SZBUF	84	/* Max # of digits (est.) */

/*
 * matchtab[] contains everything that can be matched in a format string.
 * mjump[] corresponds to matchtab to tell which case entry to go to.
 */
char matchtab[] = {
	'-', '+', ' ', '#', '0',			/* flags */
#define	LCBASE	5
	'j', 'z', 't', 'L', 'h', 'l',			/* length/conversion */
#define	NUMBASE	11
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '*', /* prec digits */
#define	IBASE	22
	'o', 'u', 'd', 'i', 'x', 'X', 'p',		/* integer etc */
#define	SPBASE	29
	'c', 'n', 's',					/* specials */
#define	FPBASE	32
	'f', 'F', 'e', 'E', 'g', 'G', 'a', 'A',		/* floating point */
#define	CBASE	40
	'O', 'U', 'D', 0				/* compat */
};

int lmods[] = { FLADJ, FPLUS, FSPC, FALT, FZERO, MLONG, 0, 0, 0, MCHAR, MLONG };
int convtab[] = { 0, CDEC, CSGN+CDEC, CSGN+CDEC, CHEX, CHEX+CUC, FALT };
char _hextab[] = "0123456789ABCDEF";

int gnum(char **, struct prinfo *);
void prstr(char *b, char *e, struct prinfo *pi);
void putwth(register struct prinfo *pi);

int
__svfprintf(FILE *iob, const char *fmt, va_list ap)
{
	struct prinfo pri;
	char *lastw;
	register char *mtab;
	register int ch;
	register char *chp;
	unsigned long l;
	char buf[SZBUF];

	pri.nwrtn = 0;
	pri.iob = iob;
	pri.app = &ap;
	pri.bend = buf + (pri.nallo = SZBUF);

loop:
	chp = fmt;
	lastw = chp;
	while (*chp && *chp != '%')
		chp++;
	if (chp != lastw)
		prstr(lastw, chp, &pri);
	if (*chp++ == 0)
		return pri.nwrtn;
	fmt = chp;
	pri.ndigit = 1;
	pri.width = pri.flags = 0;
	for (;;) {
		for (ch = 0, mtab = matchtab; *mtab; mtab++, ch++)
			if (*mtab == *fmt)
				break;
		fmt++;
		if (*mtab == 0) {
			prstr(fmt-1, fmt, &pri);
			goto loop;
		}

		if (ch < NUMBASE) {
			pri.flags += lmods[ch];
			if (pri.flags & FLADJ)
				pri.flags &= ~FZERO;
			continue;

		} else if (ch < IBASE) {
			fmt--;
			if ((pri.width = gnum(&fmt, &pri)) < 0)
				pri.width = -pri.width, pri.flags |= FLADJ;
			pri.ndigit = 1;
			if (*fmt == '.') {
				fmt++;
				pri.ndigit = gnum(&fmt, &pri);
				pri.flags |= NDFND;
			}
			continue;

		} else if (ch < SPBASE) {
intfmt:			mtab = (char *)&pri.flags;
#define	FLAGP *(int *)mtab
			FLAGP |= convtab[ch - IBASE];

			ch = pri.width;
			if ((FLAGP & NDFND) && ch < pri.ndigit)
				ch = pri.ndigit;
			if (ch > pri.nallo)
				pri.bend = alloca(pri.nallo = ch) + ch;

#ifdef __pdp11__
			if (FLAGP & (MLONG|MLLONG)) {
				l = *(long *)ap; ap += 3;
			} else
				l = *(int *)ap++;
			ap++;
#else
			l = (FLAGP & (MLONG|MLLONG)) ?
			    va_arg(ap, long) : (long)va_arg(ap, int);
#endif

			if (FLAGP & CSGN) {
				FLAGP &= ~FALT;
				if ((long)l < 0)
					l = -(long)l, FLAGP |= ISNEG;
			} else {
				FLAGP &= ~(FSPC|FPLUS);
				if ((FLAGP & (MLONG|MLLONG)) == 0)
					l = (unsigned int)l;
			}

			ch = 8;
			if (FLAGP & CDEC)
				ch = 10;
			if (FLAGP & CHEX) {
				ch <<= 1;
				if (l == 0)
					FLAGP &= ~FALT;
			}
			mtab = chp = pri.bend;

			if (l == 0 && pri.ndigit)
				*--chp = '0';
			while (l) {
				*--chp = _hextab[l % ch];
				if ((pri.flags & CUC) == 0)
					*chp |= ' '; /* make lowercase */
				l /= ch;
			}
		} else if (ch < FPBASE) {
			if (*mtab == 'c') {
#ifdef __pdp11__
				chp = ap++;
				mtab = ap++;
#else
				char *cw = va_arg(ap, int);
				mtab = chp = &cw;
				mtab++;
#endif
			} else if (*mtab == 's') { // s
				mtab = chp = va_arg(ap, char *);
				ch = INT_MAX; /* XXX */
				if (pri.flags & NDFND)
					ch = pri.ndigit;
				while (*mtab != 0 && mtab - chp < ch)
					mtab++;
			} else { /* 'n' */
				/* XXX missing char/short/long check */
				*va_arg(ap, int *) = pri.nwrtn;
				goto loop;
			}
			pri.flags &= ~(FALT|FPLUS|FSPC|NDFND);
		} else if (ch < CBASE) {
			chp = __pfcom(*mtab, &pri);
			mtab = pri.bend;
		} else { // compat
			pri.flags |= MLONG;
			ch -= (CBASE-IBASE);
			goto intfmt;
		}
		/*
		 * Input:
		 * bbeg, bend = beginning and end of buffer.
		 * will do width padding and aligning.
		 *
		 *  R-adj0	Z+ width num
		 *
		 *  L-adj	Z+      num width_
		 *  L-adj.p	Z+ prec num width
		 *
		 *  R-adj+	Width Z+ prec num
		 *  R-adj_.p	width Z+ prec num
		 *  R-adj_	width Z+      num
		 *  R-adj	      Z+      num
		 */
		/* mtab == end buffer, chp == beg buffer */
		ch = pri.flags;
		if (ch & NDFND) {
			pri.ndigit -= (mtab - chp);
			while (pri.ndigit-- > 0)
				*--chp = '0';
		} else if (ch & FZERO) {
			pri.width -= (mtab - chp);
			if (ch & (FALT|ISNEG|FPLUS|FSPC)) {
				pri.width--;
				if (ch & CHEX)
					pri.width--;
			}
			if ((ch & (CHEX|CDEC|FALT)) == FALT && pri.width > 0)
				pri.width++;
				
			while (pri.width-- > 0)
				*--chp = '0';
		}
		if (ch & FALT) {
			if (ch & CHEX) {
				*--chp = 'X';
				if ((ch & CUC) == 0)
					*chp = 'x';
			}
			if (*chp != '0')
				*--chp = '0';
		} else if (ch & ISNEG)
			*--chp = '-';
		else if (ch & FPLUS)
			*--chp = '+';
		else if (ch & FSPC)
			*--chp = ' ';
		pri.width -= (mtab - chp);

		if ((ch &= FLADJ) == 0)
			putwth(&pri);
		prstr(chp, mtab, &pri);
		if (ch)
			putwth(&pri);

		break;
	}

goto loop;
}

/*
 * Get size of a field.
 */
static int
gnum(char **f, struct prinfo *pi)
{
	register int n = 0;
	register char *fmt = *f;

	if (*fmt == '*') {
		n = va_arg(*pi->app, int);
		fmt++;
	} else {
		while (*fmt >= '0' && *fmt <= '9')
			n = n * 10 + *fmt++ - '0';
	}
	*f = fmt;
	return n;
}

static void
prstr(register char *b, char *e, register struct prinfo *pi)
{
	pi->nwrtn += e - b;
	while (b < e)
		fputc(*b++, pi->iob);
}

static void
putwth(register struct prinfo *pi)
{
	register int w = pi->width;

	while (w-- > 0)
		fputc(' ', pi->iob), pi->nwrtn++;
}
