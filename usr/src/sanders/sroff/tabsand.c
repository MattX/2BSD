/*
 * sroff driving tables
 */
#include "sdef.h"

/*
 * struct t is not overlaid by -F options.
 */

struct {
#ifndef TTY
	int bflags;
	int bspeed;
#else
	int bset;
	int breset;
#endif
	int Newline;
	int Halfline;
	char *twinit;
	char *twrest;
	char *twnl;
#ifdef ADJ
	char *twfp;
#endif
	char *hlr;
	char *hlf;
	char *flr;
	char *bdon;
	char *bdoff;
	char *ploton;
	char *plotoff;
	char *up;
	char *down;
	char *right;
	char *left;
	char *ulon;     /* underlining enables */
	char *uloff;    /* underlining disabled */
	int zzz;
	} t {

#ifndef TTY
#ifdef ADJ
/*bflags*/	CR1|NL1|FF1|EVENP|ODDP,
#else
/*bflags*/	CR1|NL1|FF1|EVENP|ODDP|CRMOD,
#endif
/*bspeed*/	BSPEED,

#else

/*bset*/	CR1|NL1|FF1|EVENP|ODDP,
#ifdef ADJ
/*breset*/	ECHO|CRMOD,
#else
/*breset*/	ECHO,
#endif
#endif	TTY

/*Newline*/	INCH/6,
/*Halfline*/	INCH/12,
/*twinit*/  "\033I\033rA\033B@@A", /* reset, ribbon usage, bottom margin */
/*twrest*/  "",
#ifdef ADJ
/*twnl*/    "\r\n",
/*twfp*/    "\033P\n",	/* force print */
#else
/*twnl*/    "\n",
#endif
/*hlr*/     "\033D",
/*hlf*/     "\033U",
/*flr*/     "\033\012",    /*reverse line feed for col*/
/*bdon*/	"\033b",	/* bold on - next char is offset */
/*bdoff*/	"\033b@",	/* try to turn it back off!! */
/*ploton*/	"",
/*plotoff*/	"",
/*up*/		"",
/*down*/	"",
/*right*/	"",
/*left*/	"",
/*ulon*/    "\033uM",
/*uloff*/   "\033uM"
};

/*
 * code tables
 * In the first byte, the
 * 0100 bit specifies that special fonts are used and the previous font
 * should be restored.  The remaining bits specify the font to change to
 * if an initial font transition is desired.
 * leftmost bit of each of the remaining bytes turns on plot mode (see n10.c)
*/

char *codetab[MAXCHAR-0200] = {
"\000\000",	/* 200: null; not output */
"\000\010",	/* 201: bs */
"\000\011",	/* 202: tab */
"\000\012",	/* 203: lf */
"\000\014",	/* 204: ff */
"\000\015",	/* 205: cr */
"\000\033",	/* 206: esc */
"\000\134",	/* backslash (never escaped) */
"\000\000",	/* unused */
"\000\000",	/* unused */
		/* sanders control sequences */
"\000\033a",	/* 212: select font */
"\000\033s",	/* 213:  repeat character */
"\000\033x",	/* 214:  insert sequence of characters */
"\000\033y",	/* 215:  insert space */
"\000\033U",	/* 216:  half-line feed */
"\000\033\012",	/* 217:  neqative line feed */
"\000\033D",	/* 220:  negative half-line feed */
"\000\033n",	/* 221:  select format */
"\000\033e",	/* 222:  set line length */
"\000\033I",	/* 223:  set temporary indent */
"\000\033g",	/* 224:  set indentation */
#ifdef ADJ
"\000\033w@@,@@,FP.",	/* 225:  set word space */
			/* allow up to 400 mils between words */
#else
"\000\033w",	/* 225:  set word space */
#endif
"\000\033i",	/* 226:  set form length */
"\000\033t",	/* 227:  toggle draft mode */
	/* greek: */
"\102a",    /* 230: alpha */
"\102b",    /* beta */
"\102c",    /* gamma */
"\102d",    /* delta */
"\102e",    /* epsilon */
"\102g",    /* zeta */
"\102h",    /* eta */
"\102i",    /* theta */
"\102j",    /*240: iota */
"\102k",    /* kappa */
"\102l",    /* lambda */
"\102m",    /* mu */
"\102n",    /* nu */
"\102o",    /* xi */
"\102p",    /* omicron */
"\102q",    /* pi */
"\102r",    /*250: rho */
"\102s",    /* sigma */
"\102t",    /* tau */
"\102u",    /* upsilon */
"\102v",    /* phi */
"\102x",    /* chi */
"\102y",    /* psi */
"\102z",    /*omega*/
"\102A",    /*260: Alpha */
"\102B",    /* Beta */
"\102C",    /* Gamma */
"\102D",    /* Delta */
"\102E",    /* Epsilon */
"\102G",    /* Zeta */
"\102H",    /* Eta */
"\102I",    /* Theta */
"\102J",    /*270: Iota */
"\102K",    /* Kappa */
"\102L",    /* Lambda */
"\102M",    /* Mu */
"\102N",    /* Nu */
"\102O",    /* Xi */
"\102P",    /* Omicron */
"\102Q",    /* Pi */
"\102R",    /*300: Rho */
"\102S",    /* Sigma */
"\102T",    /* Tau */
"\102U",    /* Upsilon */
"\102V",    /* Phi */
"\102X",    /* Chi */
"\102Y",    /* Psi */
"\102Z",    /* Omega */
"\102`",	 /*310: square root*/
"\102_",	/*root en */
"\103h",	 /*>=*/
"\103H",	 /*<=*/
"\103N",	 /*identically equal*/
"\103k",	 /*approx =*/
"\103J",	 /*approximates*/
"\103j",	 /*not equal*/
"\103W",	 /*320: right arrow*/
"\103w",	 /*left arrow*/
"\103^",	 /*up arrow*/
"\103\177",	 /*down arrow*/
"\103x",	 /*multiply*/
"\103f",	 /*divide*/
"\103L",	 /*plus-minus*/
"\103T",	 /*cup (union)*/
"\103t",	 /*330: cap (intersection)*/
"\103U",	 /*subset of*/
"\103u",	 /*superset of*/
"\103\033uDU\033uD",	 /*improper subset*/
"\103\033uDu\033uD",	 /* improper superset*/
"\103z",	 /*infinity*/
"\103X",	/*partial derivative*/
"\103V", 	/*gradient*/
"\103Z",	/*340: proportional to*/
"\103\033l\177Nd\033o\177p\033l~\134d\033o@P\033l\177N",   /* box vert rule */
"\102|",	 /*or */
"\0000\033N+",	/*bullet*/
"\103[",	/*square*/
"\102;",	/*degree*/
"\103_",	/*dagger*/
"\000\033o@K\_\033o\177u",	/* underrule */
"\102-",    /* 350: 3/4 Em dash */
        /* bracket building symbols */
"\103\033o@Dd\033o\177l\033l~\134d\033o@P",	/* vertical solid bar */
"\103A\033o@N\033l~\134d\033o\177r",       /* left top curly */
"\103B\033o\177t\033l~\134d\033o@L",       /* left bottom curly */
"\103C\033o\177r\033l~\134d\033o@N",       /* right center curly */
"\103D\033o@N\033l~\134d\033o\177r",       /* left top square */
"\103E\033o\177t\033l~\134d\033o@L",       /* left bottom square */
"\103F\033o\177t\033l~\134d\033o@L",       /* right bottom square */
"\103b\033o@N\033l~\134d\033o\177r", /*360: right top curly */
"\103c\033o\177t\033l~\134d\033o@L",       /* right bottom curly */
"\103e\033o@N\033l~\134d\033o\177r",       /* right top square */
"\103/\033o\177r\033l~\134d\033o@N",       /* left center curly */
"\102!\033aC\033o@N\033l~\134d\033o\177r", /* top of integral */
"\103\033o\177rd\033o@N\033l~\134\033aB*", /* bottom of integral */
"\103d\033o\177r\033l~\134d\033o@N",       /* bracket upper */
"\103d\033o\177r\033l~\134d\033o@N",		/* bracket lower */
"\000\033lAF\\\033l~z",		/* 370: angled slash for sqrt */
	/* more constructed characters: */
"\102|\033N=",	 /*371: dbl dagger*/
};
