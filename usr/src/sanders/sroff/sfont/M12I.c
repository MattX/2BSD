/*
 * sroff driving tables
 * M12I (Messenger 12 and Helvesan 10 Italic)
 */
#include "../sdef.h"

/*
 * Only the first two physical fonts (fonts.phys = 0,1) can be proportional
 * and their character widths are in fontab and fontab1 respectively.
 * Special characters (values greater than 0177) are also in fontab.
 * The special characters assume that the scientific font is on position C
 * (phys = 3) and the greek/math font is on position B (phys = 2).
 * All values are in units (mils, since INCH is 1000).
 * fonts, ulfont, fontab and fontab1 are all overlaid by the -F option.
 */

struct fonts {		/* info for all fonts to be mounted */
	char lab;	/* name */
	char phys;	/* sanders logical font #, |= PROPOR if proportional */
	int Char;	/* character width if nonproportional */
	int Em;		/* character size, value of 1m */
			/* (point size) x (1000/72)    */
	char mount[10];	/* sanders font number */
} fonts[NFONTS] = {
	{'R', 0,   83, 139, "X5010608"},	/* 12-pitch (Messenger 12)*/
	{'I', 1|PROPOR, 0, 139, "X5022110"},	/* Helvesan 10 Italics */
	{'B', 0,   83, 139, ""},	/* emboldened Messenger 12 */
	{'S', 3,   100, 139, "S5480210"},	/* Sci-Pi */
	{ 0,  4,   100, 139, ""},
	{ 0,  5,   100, 139, ""},
	{ 0,  6,   100, 139, ""},
	{'G', 2,   100, 139, "S5480310"}	/* Greek */
};

int ulfont = -1;

int fontab[MAXCHAR-040] = {	/* widths for font 0 PROPOR and special */
	/* 040-0177 used only for ULMODE since font 0 not PROPOR */
0,		/*40: space*/
0,		/*!*/
0,		/*"*/
0,		/*#*/
0,		/*$*/
0,		/*%*/
0,		/*&*/
0|ULMODE,	/*' close*/
0,		/*50: (*/
0,		/*)*/
0,		/***/
0,		/*+*/
0,		/*,*/
0,		/*- hyphen*/
0,		/*.*/
0,		/*/*/
0|ULMODE,	/*60: 0*/
0|ULMODE,	/*1*/
0|ULMODE,	/*2*/
0|ULMODE,	/*3*/
0|ULMODE,	/*4*/
0|ULMODE,	/*5*/
0|ULMODE,	/*6*/
0|ULMODE,	/*7*/
0|ULMODE,	/*70: 8*/
0|ULMODE,	/*9*/
0,		/*:*/
0,		/*;*/
0,		/*<*/
0,		/*=*/
0,		/*>*/
0,		/*?*/
0,		/*100: @*/
0|ULMODE,	/*A*/
0|ULMODE,	/*B*/
0|ULMODE,	/*C*/
0|ULMODE,	/*D*/
0|ULMODE,	/*E*/
0|ULMODE,	/*F*/
0|ULMODE,	/*G*/
0|ULMODE,	/*110: H*/
0|ULMODE,	/*I*/
0|ULMODE,	/*J*/
0|ULMODE,	/*K*/
0|ULMODE,	/*L*/
0|ULMODE,	/*M*/
0|ULMODE,	/*N*/
0|ULMODE,	/*O*/
0|ULMODE,	/*120: P*/
0|ULMODE,	/*Q*/
0|ULMODE,	/*R*/
0|ULMODE,	/*S*/
0|ULMODE,	/*T*/
0|ULMODE,	/*U*/
0|ULMODE,	/*V*/
0|ULMODE,	/*W*/
0|ULMODE,	/*130: X*/
0|ULMODE,	/*Y*/
0|ULMODE,	/*Z*/
0,		/*[*/
0,		/*\*/
0,		/*]*/
0,		/*^*/
0,		/*_ underscore*/
0,		/*140: ` open*/
0|ULMODE,	/*a*/
0|ULMODE,	/*b*/
0|ULMODE,	/*c*/
0|ULMODE,	/*d*/
0|ULMODE,	/*e*/
0|ULMODE,	/*f*/
0|ULMODE,	/*g*/
0|ULMODE,	/*150: h*/
0|ULMODE,	/*i*/
0|ULMODE,	/*j*/
0|ULMODE,	/*k*/
0|ULMODE,	/*l*/
0|ULMODE,	/*m*/
0|ULMODE,	/*n*/
0|ULMODE,	/*o*/
0|ULMODE,	/*160: p*/
0|ULMODE,	/*q*/
0|ULMODE,	/*r*/
0|ULMODE,	/*s*/
0|ULMODE,	/*t*/
0|ULMODE,	/*u*/
0|ULMODE,	/*v*/
0|ULMODE,	/*w*/
0|ULMODE,	/*170: x*/
0|ULMODE,	/*y*/
0|ULMODE,	/*z*/
0,		/*{*/
0,		/*|*/
0,		/*}*/
0,		/*~*/
0,		/*delete */
0000,		/* 200: null */
0000,		/* 201: bs */
0000,		/* 202: tab */
0000,		/* 203: lf */
0000,		/* 204: ff */
0000,		/* 205: cr */
0000,		/* 206: esc */
0000,		/* unused */
0000,		/* unused */
0000,		/* unused */
0000,		/* 212: select font */
0000,		/* 213:  repeat character */
0000,		/* 214:  insert sequence of characters */
0000,		/* 215:  insert space */
0000,		/* 216:  half-line feed */
0000,		/* 217:  neqative line feed */
0000,		/* 220:  negative half-line feed */
0000,		/* 221:  select format */
0000,		/* 222:  set line length */
0000,		/* 223:  set temporary indent */
0000,		/* 224:  set indentation */
0000,		/* 225:  set word space */
0000,		/* 226:  set form length */
0000,		/* 227:  toggle draft mode */
	/* greek: */
100|ULMODE,    /* 230: alpha */
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,    /*240*/
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,    /*250*/
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,    /*omega*/
100|ULMODE,    /*260: Alpha */
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,    /*270*/
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,    /*300*/
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,
100|ULMODE,    /* Omega */
100|ULMODE,	 /*310: square root*/
100|ULMODE,	/*root en */
100,		/*>=*/
100,		/*<=*/
100,		/*identically equal*/
100,		/*approx =*/
100,		/*approximates*/
100,		/*not equal*/
100,		/*320: right arrow*/
100,		/*left arrow*/
100,		/*up arrow*/
100,		/*down arrow*/
100,		/*multiply*/
100,		/*divide*/
100,		/*plus-minus*/
100,		/*cup (union)*/
100,		/*330: cap (intersection)*/
100,		/*subset of*/
100,		/*superset of*/
100,		/*improper subset*/
100,		/* improper superset*/
100,		/*infinity*/
100,		/*partial derivative*/
100,		/*gradient*/
100,		/*340: proportional to*/
0,		/* box vert rule */
100,		/*or */
85,		/*bullet*/
100,		/*square*/
100,		/*degree*/
100,		/*dagger*/
100,		/* underrule */
100,		/* 350: 3/4 Em dash */
100,		/* vertical solid bar */
100,		/* left top curly */
100,		/* left bottom curly */
100,		/* right center curly */
100,		/* left top square */
100,		/* left bottom square */
100,		/* right bottom square */
100,		/*360: right top curly */
100,		/* right bottom curly */
100,		/* right top square */
100,		/* left center curly */
100,		/* top of integral */
100,		/* bottom of integral */
100,		/* bracket upper */
100,		/* bracket lower */
100,		/* 370: angled slash for sqrt */
100|ULMODE	/*371: dbl dagger*/
};

int fontab1[0200-040] = {	/* character widths for font 1 PROPOR */
100,	/*40: space*/
58,	/*!*/
78,	/*"*/
100,	/*#*/
85,	/*$*/
130,	/*%*/
90,	/*&*/
60,	/*' close*/
70,	/*50: (*/
70,	/*)*/
90,	/***/
85,	/*+*/
58,	/*,*/
85,	/*- hyphen*/
58,	/*.*/
70,	/*/*/
85,	/*60: 0*/
85,	/*1*/
85,	/*2*/
85,	/*3*/
85,	/*4*/
85,	/*5*/
85,	/*6*/
85,	/*7*/
85,	/*70: 8*/
85,	/*9*/
58,	/*:*/
58,	/*;*/
85,	/*<*/
85,	/*=*/
85,	/*>*/
85,	/*?*/
100,	/*100: @*/
91,	/*A*/
99,	/*B*/
107,	/*C*/
105,	/*D*/
93,	/*E*/
90,	/*F*/
109,	/*G*/
111,	/*110: H*/
50,	/*I*/
82,	/*J*/
93,	/*K*/
78,	/*L*/
128,	/*M*/
112,	/*N*/
109,	/*O*/
90,	/*120: P*/
109,	/*Q*/
100,	/*R*/
89,	/*S*/
90,	/*T*/
115,	/*U*/
95,	/*V*/
130,	/*W*/
86,	/*130: X*/
83,	/*Y*/
87,	/*Z*/
55,	/*[*/
72,	/*\*/
55,	/*]*/
85,	/*^*/
91,	/*_ underscore*/
60,	/*140: ` open*/
84,	/*a*/
87,	/*b*/
82,	/*c*/
87,	/*d*/
84,	/*e*/
50,	/*f*/
87,	/*g*/
83,	/*150: h*/
46,	/*i*/
46,	/*j*/
74,	/*k*/
46,	/*l*/
125,	/*m*/
83,	/*n*/
85,	/*o*/
87,	/*160: p*/
87,	/*q*/
61,	/*r*/
76,	/*s*/
45,	/*t*/
83,	/*u*/
65,	/*v*/
100,	/*w*/
63,	/*170: x*/
65,	/*y*/
65,	/*z*/
55,	/*{*/
50,	/*|*/
55,	/*}*/
85,	/*~*/
100   /*delete */
};
