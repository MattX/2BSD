/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if	!defined(lint) && defined(DOSCCS)
static char sccsid[] = "@(#)subr.c	5.5 (2.11BSD) 2023/4/13";
#endif

/*
 * Melbourne getty.
 */
#include <sgtty.h>
#include "gettytab.h"

extern	struct sgttyb tmode;
extern	struct tchars tc;
extern	struct ltchars ltc;

/*
 * Get a table entry.
 */
gettable(name, buf, area)
	char *name, *buf, *area;
{
	register struct gettystrs *sp;
	register struct gettynums *np;
	register struct gettyflags *fp;
	long ln;
	int n;

	hopcount = 0;		/* new lookup, start fresh */
	if (getent(buf, name) != 1)
		return;

	for (sp = gettystrs; sp->field; sp++)
		sp->value = getstr(sp->field, &area);
	for (np = gettynums; np->field; np++) {
		ln = getnum(np->field);
		if (ln == -1)
			np->set = 0;
		else {
			np->set = 1;
			np->value = ln;
		}
	}
	for (fp = gettyflags; fp->field; fp++) {
		n = getflag(fp->field);
		if (n == -1)
			fp->set = 0;
		else {
			fp->set = 1;
			fp->value = n ^ fp->invrt;
		}
	}
}

gendefaults()
{
	register struct gettystrs *sp;
	register struct gettynums *np;
	register struct gettyflags *fp;

	for (sp = gettystrs; sp->field; sp++)
		if (sp->value)
			sp->defalt = sp->value;
	for (np = gettynums; np->field; np++)
		if (np->set)
			np->defalt = np->value;
	for (fp = gettyflags; fp->field; fp++)
		if (fp->set)
			fp->defalt = fp->value;
		else
			fp->defalt = fp->invrt;
}

setdefaults()
{
	register struct gettystrs *sp;
	register struct gettynums *np;
	register struct gettyflags *fp;

	for (sp = gettystrs; sp->field; sp++)
		if (!sp->value)
			sp->value = sp->defalt;
	for (np = gettynums; np->field; np++)
		if (!np->set)
			np->value = np->defalt;
	for (fp = gettyflags; fp->field; fp++)
		if (!fp->set)
			fp->value = fp->defalt;
}

static char **
charnames[] = {
	&ER, &KL, &IN, &QU, &XN, &XF, &ET, &BK,
	&SU, &DS, &RP, &FL, &WE, &LN, 0
};

static char *
charvars[] = {
	&tmode.sg_erase, &tmode.sg_kill, &tc.t_intrc,
	&tc.t_quitc, &tc.t_startc, &tc.t_stopc,
	&tc.t_eofc, &tc.t_brkc, &ltc.t_suspc,
	&ltc.t_dsuspc, &ltc.t_rprntc, &ltc.t_flushc,
	&ltc.t_werasc, &ltc.t_lnextc, 0
};

setchars()
{
	register int i;
	register char *p;

	for (i = 0; charnames[i]; i++) {
		p = *charnames[i];
		if (p && *p)
			*charvars[i] = *p;
		else
			*charvars[i] = '\377';
	}
}

long
setflags(n)
{
	register long f;

	switch (n) {
	case 0:
		if (F0set)
			return(F0);
		break;
	case 1:
		if (F1set)
			return(F1);
		break;
	default:
		if (F2set)
			return(F2);
		break;
	}

	f = 0;

	if (AP)
		f |= ANYP;
	else if (OP)
		f |= ODDP;
	else if (EP)
		f |= EVENP;
	if (HF)
		f |= RTSCTS;
	if (NL)
		f |= CRMOD;

	if (n == 1) {		/* read mode flags */
		if (RW)
			f |= RAW;
		else
			f |= CBREAK;
		return (f);
	}

	if (!HT)
		f |= XTABS;
	if (n == 0)
		return (f);
	if (CB)
		f |= CRTBS;
	if (CE)
		f |= CRTERA;
	if (CK)
		f |= CRTKIL;
	if (PE)
		f |= PRTERA;
	if (GEC)
		f |= ECHO;
	if (XC)
		f |= CTLECH;
	if (DX)
		f |= DECCTQ;
	if (NP)
		f |= PASS8;
	return (f);
}

char	editedhost[32];

edithost(pat)
	register char *pat;
{
	register char *host = HN;
	register char *res = editedhost;

	if (!pat)
		pat = "";
	while (*pat) {
		switch (*pat) {

		case '#':
			if (*host)
				host++;
			break;

		case '@':
			if (*host)
				*res++ = *host++;
			break;

		default:
			*res++ = *pat;
			break;

		}
		if (res == &editedhost[sizeof editedhost - 1]) {
			*res = '\0';
			return;
		}
		pat++;
	}
	if (*host)
		strncpy(res, host, sizeof editedhost - (res - editedhost) - 1);
	else
		*res = '\0';
	editedhost[sizeof editedhost - 1] = '\0';
}

void makeenv(env, term)
	char *env[];
        char *term;
{
	static char termbuf[128] = "TERM=";
	register char *p, *q;
	register char **ep;
	char *index();

	ep = env;
	if (*term) {
		strcat(termbuf, term);
		*ep++ = termbuf;
	}
	if (p = EV) {
		q = p;
		while (q = index(q, ',')) {
			*q++ = '\0';
			*ep++ = p;
			p = q;
		}
		if (*p)
			*ep++ = p;
	}
	*ep = (char *)0;
}
