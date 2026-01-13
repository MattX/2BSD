#if     defined(DOSCCS) && !defined(lint)
static char *sccsid = "@(#)pfcom.c 1.0 (2.11BSD) 2025/12/25";
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "local.h"

#undef	NEWORG

/* XXX should be in some header file */
char *ecvt(), *fcvt(), *gcvt(), *acvt();
char *divput(register char *dbeg, register int *decpt);

/*
 * Format floating point numbers.
 * A formatted number is but in a buffer of size pi->nallo, ending at 
 * address pi->bend. Beginning of number buffer is returned, and end
 * of buffer must be pi->bend.
 */
char *
__pfcom(int ch, struct prinfo *pi)
{
	double d;
	char *fbeg;
	register char *fend, *dbeg;
	register int nd, decpt, sign, m, falt;

/* hack to ensure pfcom only get included if floating point used */
asm(".globl  fltused ; fltused:");

	d = va_arg(*pi->app, double);

	dbeg = pi->bend;
	sign = 0;

	nd = 6;
	if (pi->flags & NDFND)
		nd = pi->ndigit;
	falt = pi->flags & FALT;

	switch (ch | ' ') {
	case 'f':
#ifdef NEWORG
		dbeg -= pi->nallo;
		fbeg = dbeg;
		fend = fcvt(d, nd, &decpt, &sign);
		if (decpt <= 0)
			*dbeg++ = '0';
		else
			while (decpt-- > 0)
				*dbeg++ = *fend++;
		*dbeg++ = '.';
		while (++decpt <= 0)
			*dbeg++ = '0';
		while (*fend)
			*dbeg++ = *fend++;
		fend = pi->bend;
		while (dbeg > fbeg)
			*--fend = *--dbeg;
#else
		fend = fbeg = fcvt(d, nd, &decpt, &sign);
		while (*fend++); fend--;

		if (nd | falt) {
			while (fend > fbeg && nd)
				*--dbeg = *--fend, nd--;
			while (nd-- > 0)
				*--dbeg = '0';
			*--dbeg = '.';
		}
		while (fend > fbeg)
			*--dbeg = *--fend;
		if (*dbeg == '.')
			*--dbeg = '0';
#endif
		break;
	case 'e':
#ifdef NEWORG
		dbeg -= pi->nallo;
		fbeg = dbeg;
		fend = ecvt(d, ++nd, &decpt, &sign);
		if (*fend == '0')
			decpt++;
		*dbeg++ = *fend++;
		if (--nd | falt) {
			*dbeg++ = '.';
			while (--nd >= 0)
				*dbeg++ = *fend++;
		}
		*dbeg++ = ch;
		nd = '+';
		if (--decpt < 0)
			nd = '-', decpt = -decpt;
		*dbeg++ = nd;
		*dbeg++ = (decpt/10) + '0';
		*dbeg++ = (decpt%10) + '0';
		fend = pi->bend;
		while (dbeg > fbeg)
			*--fend = *--dbeg;
			
#else
		fend = fbeg = ecvt(d, nd+1, &decpt, &sign);
		while (*fend++); fend--;

		if (*fbeg != '0')
			decpt--;
		dbeg = divput(dbeg, &decpt);
		*--dbeg = ch;
		if (nd | falt) {
			while (nd-- > 0)
				*--dbeg = *--fend;
			*--dbeg = '.';
		}
		*--dbeg = *--fend;
#endif
		break;
	case 'g':
		/*
		 * F style should be used if (prec > exp >= -4),
		 * otherwise e style. Difference is that no trailing
		 * zeroes are added (unless # flag given).
		 */
		if (nd == 0)
			nd = 1;
		fend = fbeg = ecvt(d, nd, &decpt, &sign);
		while (*fend++); fend--;

		if (*fbeg != '0')
			decpt--;
		if (nd > decpt && decpt >= -4) {
			/* f style */
			nd = nd - (decpt + 1);
			if (!falt)
				while (fend[-1] == '0' && nd)
					fend--, nd--;

			while (fend > fbeg && nd)
				*--dbeg = *--fend, nd--;
			while (nd-- > 0)
				*--dbeg = '0';
			if (dbeg != pi->bend || falt)
				*--dbeg = '.';
			while (fend > fbeg)
				*--dbeg = *--fend;
			if (*dbeg == '.')
				*--dbeg = '0';
		} else {
			/* g style */
			dbeg = divput(dbeg, &decpt);
			*--dbeg = ch - ('g' - 'e');
			nd--;
			if (!falt)
				while (fend[-1] == '0' && nd)
					fend--, nd--;

			while (nd-- > 0)
				*--dbeg = *--fend;
			if (dbeg != pi->bend-4 || falt)
				*--dbeg = '.';
			*--dbeg = *--fend;
		}
		break;
	default:
		if ((pi->flags & NDFND) == 0)
			nd = -1; /* full precision */
		fend = fbeg = __acvt(d, nd, pi->bend - pi->nallo, &sign);
		while (*fend++); fend--;

		while (fend > fbeg)
			*--dbeg = *--fend | (ch & ' ');
	}
	if (sign)
		pi->flags |= ISNEG;
	pi->flags &= ~(NDFND|FALT);

	return dbeg;
}

static char *
divput(register char *dbeg, register int *decpt)
{
	register int m = '+';

	if (*decpt < 0)
		m = '-', *decpt = -*decpt;

	*--dbeg = (*decpt % 10) + '0';
	*--dbeg = (*decpt/10) + '0';
	*--dbeg = m;
	return dbeg;
}
