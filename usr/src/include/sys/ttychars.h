/*	ttychars.h	6.1	83/07/29	*/

/*
 * User visible structures and constants
 * related to terminal handling.
 */
#ifndef _TTYCHARS_
#define	_TTYCHARS_
struct ttychars {
	char	tc_erase;	/* erase last character */
	char	tc_kill;	/* erase entire line */
	char	tc_intrc;	/* interrupt */
	char	tc_quitc;	/* quit */
	char	tc_startc;	/* start output */
	char	tc_stopc;	/* stop output */
	char	tc_eofc;	/* end-of-file */
	char	tc_brkc;	/* input delimiter (like nl) */
	char	tc_suspc;	/* stop process signal */
	char	tc_dsuspc;	/* delayed stop process signal */
	char	tc_rprntc;	/* reprint line */
	char	tc_flushc;	/* flush output (toggles) */
	char	tc_werasc;	/* word erase */
	char	tc_lnextc;	/* literal next character */
};

/* begin CCA_PAGE */
struct pagechars {
	char tc_pagec;		/* allow next page of output */
	char tc_linec;		/* allow next line of output */
	char tc_halfc;		/* allow next half page of output */
	char tc_goc;		/* turn off page mode until next char typed */
	char tc_clearc;		/* clear and go a page NYI */
	char tc_sparec;		/* unused */
};
/* end CCA_PAGE */

/* begin _USESTAT */
struct tauxil {
	char	tc_usest;	/* print out user stats */
	short	tc_usemap;	/* bitmap for which stats */
};
/* end _USESTAT */

#define	CTRL(c)	('c'&037)

/* default special characters */
#define	CERASE	0177
#define	CKILL	CTRL(u)
#define	CINTR	CTRL(c)
#define	CQUIT	034		/* FS, ^\ */
#define	CSTART	CTRL(q)
#define	CSTOP	CTRL(s)
#define	CEOF	CTRL(d)
#define	CEOT	CEOF
#define	CBRK	0377
#define	CSUSP	CTRL(z)
#define	CDSUSP	CTRL(y)
#define	CRPRNT	CTRL(r)
#define	CFLUSH	CTRL(o)
#define	CWERASE	CTRL(w)
#define	CLNEXT	CTRL(v)
#endif

/* begin CCA_PAGE */
/*
 * Note: these characters are not special except when stopped at end of
 * page, so space and newline work normally otherwise.
 */
#define CPAGE	' '
#define CLINE	'\n'
#define CHALF	CTRL(d)
#define CGO	CTRL(a)
#define CCLEAR	CTRL(f)
#define CSPARE	'\377'
/* end CCA_PAGE */

/* begin _USESTAT */
#define CUSEST	CTRL(t)
#define CSUMAP	~(Ust_uptime|Ust_flags|Ust_childs)
/* end _USESTAT */
