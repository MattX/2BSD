static char *sccsid = "@(#)getdate.c	1.2	9/14/80";
# define ID 257
# define MONTH 258
# define DAY 259
# define MERIDIAN 260
# define NUMBER 261
# define UNIT 262
# define MUNIT 263
# define SUNIT 264
# define ZONE 265
# define DAYZONE 266

	/* 	Steven M. Bellovin (unc!smb)			*/
	/*	Dept. of Computer Science			*/
	/*	University of North Carolina at Chapel Hill	*/

#include <sys/types.h>
#include <sys/timeb.h>
#define	NULL	0
#define daysec (24L*60L*60L)
	static int timeflag, zoneflag, dateflag, dayflag, relflag;
	static time_t relsec, relmonth;
	static int hh, mm, ss, merid, daylight;
	static int dayord, dayreq;
	static int month, day, year;
	static int ourzone;
#define AM 1
#define PM 2
#define DAYLIGHT 1
#define STANDARD 2
#define MAYBE    3
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
# define YYERRCODE 256



static int mdays[12] =
	{31, 0, 31,  30, 31, 30,  31, 31, 30,  31, 30, 31};
#define epoch 1970

#include <time.h>
extern struct tm *localtime();
time_t dateconv(mm, dd, yy, h, m, s, mer, zone, dayflag)
int mm, dd, yy, h, m, s, mer, zone, dayflag;
{
	time_t tod, jdate;
	register int i;
	time_t timeconv();

	if (yy < 100) yy += 1900;
	mdays[1] = 28 + (yy%4 == 0);
	if (yy < epoch || yy > 1999 || mm < 1 || mm > 12 ||
		dd < 1 || dd > mdays[--mm]) return (-1);
	jdate = dd-1;
        for (i=0; i<mm; i++) jdate += mdays[i];
	for (i = epoch; i < yy; i++) jdate += 365 + (i%4 == 0);
	jdate *= daysec;
	jdate += zone * (60L*60L);
	if ((tod = timeconv(h, m, s, mer)) < 0) return (-1);
	jdate += tod;
	if (dayflag==DAYLIGHT || (dayflag==MAYBE&&localtime(&jdate)->tm_isdst))
		jdate += -1*60*60;
	return (jdate);
}

time_t dayconv(ord, day, now) int ord, day; time_t now;
{
	register struct tm *loctime;
	time_t tod;
	time_t daylcorr();

	tod = now;
	loctime = localtime(&tod);
	tod += daysec * ((day - loctime->tm_wday + 7) % 7);
	tod += 7*daysec*(ord<=0?ord:ord-1);
	return daylcorr(tod, now);
}

time_t timeconv(hh, mm, ss, mer) register int hh, mm, ss, mer;
{
	if (mm < 0 || mm > 59 || ss < 0 || ss > 59) return (-1);
	switch (mer) {
		case AM: if (hh < 1 || hh > 12) return(-1);
			 return (60L * ((hh%12)*60L + mm)+ss);
		case PM: if (hh < 1 || hh > 12) return(-1);
			 return (60L * ((hh%12 +12)*60L + mm)+ss);
		case 24: if (hh < 0 || hh > 23) return (-1);
			 return (60L * (hh*60L + mm)+ss);
		default: return (-1);
	}
}
time_t monthadd(sdate, relmonth) time_t sdate, relmonth;
{
	struct tm *ltime;
	time_t dateconv();
	time_t daylcorr();
	int mm, yy;

	if (relmonth == 0) return 0;
	ltime = localtime(&sdate);
	mm = 12*ltime->tm_year + ltime->tm_mon + relmonth;
	yy = mm/12;
	mm = mm%12 + 1;
	return daylcorr(dateconv(mm, ltime->tm_mday, yy, ltime->tm_hour,
		ltime->tm_min, ltime->tm_sec, 24, ourzone, MAYBE), sdate);
}

time_t daylcorr(future, now) time_t future, now;
{
	int fdayl, nowdayl;

	nowdayl = (localtime(&now)->tm_hour+1) % 24;
	fdayl = (localtime(&future)->tm_hour+1) % 24;
	return (future-now) + 60L*60L*(nowdayl-fdayl);
}
#include <ctype.h>
static char *lptr;

yylex()
{
	extern int yylval;
	int sign;
	register char c;
	register char *p;
	char idbuf[20];

	while (isspace(*lptr)) lptr++;

	if (isdigit(c = *lptr) || c == '-' || c == '+') {
		if (c== '-' || c == '+') {
			if (c=='-') sign = -1;
			else sign = 1;
			if (!isdigit(*++lptr)) {
				yylval = sign;
				return (NUMBER);
			}
		} else sign = 1;
		yylval = 0;
		while (isdigit(c = *lptr++)) yylval = 10*yylval + c - '0';
		yylval *= sign;
		lptr--;
		return (NUMBER);

	} else if (isalpha(c)) {
		p = idbuf;
		while (isalpha(c = *lptr++) || c=='.')
			*p++ = c;
		*p = '\0';
		lptr--;
		return (lookup(idbuf));
	}

	else return (*lptr++);
}

struct table {
	char *name;
	int type, value;
};

struct table mdtab[] = {
	{"January", MONTH, 1},
	{"February", MONTH, 2},
	{"March", MONTH, 3},
	{"April", MONTH, 4},
	{"May", MONTH, 5},
	{"June", MONTH, 6},
	{"July", MONTH, 7},
	{"August", MONTH, 8},
	{"September", MONTH, 9},
	{"Sept", MONTH, 9},
	{"October", MONTH, 10},
	{"November", MONTH, 11},
	{"December", MONTH, 12},

	{"Sunday", DAY, 0},
	{"Monday", DAY, 1},
	{"Tuesday", DAY, 2},
	{"Tues", DAY, 2},
	{"Wednesday", DAY, 3},
	{"Wednes", DAY, 3},
	{"Thursday", DAY, 4},
	{"Thur", DAY, 4},
	{"Thurs", DAY, 4},
	{"Friday", DAY, 5},
	{"Saturday", DAY, 6},
	{0, 0, 0}};

struct table mztab[] = {
	{"a.m.", MERIDIAN, AM},
	{"am", MERIDIAN, AM},
	{"p.m.", MERIDIAN, PM},
	{"pm", MERIDIAN, PM},
	{"ast", ZONE, 4},		/* Atlantic */
	{"a.s.t.", ZONE, 4},
	{"adt", DAYZONE, 4},
	{"a.d.t.", DAYZONE, 4},
	{"est", ZONE, 5},		/* Eastern */
	{"e.s.t.", ZONE, 5},
	{"edt", DAYZONE, 5},
	{"e.d.t.", DAYZONE, 5},
	{"cst", ZONE, 6},		/* Central */
	{"c.s.t.", ZONE, 6},
	{"cdt", DAYZONE, 6},
	{"c.d.t.", DAYZONE, 6},
	{"mst", ZONE, 7},		/* Mountain */
	{"m.s.t.", ZONE, 7},
	{"mdt", DAYZONE, 7},
	{"m.d.t.", DAYZONE, 7},
	{"pst", ZONE, 8},		/* Pacific */
	{"p.s.t.", ZONE, 8},
	{"pdt", DAYZONE, 8},
	{"p.d.t.", DAYZONE, 8},
	{"gmt", ZONE, 0},
	{"g.m.t.", ZONE, 0},
	{0, 0, 0}};

struct table unittb[] = {
	{"year", MUNIT, 12},
	{"month", MUNIT, 1},
	{"fortnight", UNIT, 14*24*60},
	{"week", UNIT, 7*24*60},
	{"day", UNIT, 1*24*60},
	{"hour", UNIT, 60},
	{"minute", UNIT, 1},
	{"min", UNIT, 1},
	{"second", SUNIT, 1},
	{"sec", SUNIT, 1},
	{0, 0, 0}};

struct table othertb[] = {
	{"tomorrow", UNIT, 1*24*60},
	{"yesterday", UNIT, -1*24*60},
	{"today", UNIT, 0},
	{"now", UNIT, 0},
	{"last", NUMBER, -1},
	{"next", NUMBER, 2},
	{"first", NUMBER, 1},
	/* {"second", NUMBER, 2}, */
	{"third", NUMBER, 3},
	{"fourth", NUMBER, 4},
	{"fifth", NUMBER, 5},
	{"sixth", NUMBER, 6},
	{"seventh", NUMBER, 7},
	{"eigth", NUMBER, 8},
	{"ninth", NUMBER, 9},
	{"tenth", NUMBER, 10},
	{"eleventh", NUMBER, 11},
	{"twelfth", NUMBER, 12},
	{0, 0, 0}};

lookup(id) char *id;
{
#define gotit (yylval=i->value,  i->type)
#define getid for(j=idvar, k=id; *j++ = *k++; )

	char idvar[20];
	register char *j, *k;
	register struct table *i;
	int abbrev;

	getid;
	if (strlen(idvar) == 3) abbrev = 1;
	else if (strlen(idvar) == 4 && idvar[3] == '.') {
		abbrev = 1;
		idvar[3] = '\0';
	}
	else abbrev = 0;

	if (islower(*idvar)) *idvar = toupper(*idvar);

	for (i = mdtab; i->name; i++) {
		k = idvar;
		for (j = i->name; *j++ == *k++;) {
			if (abbrev && j==i->name+3) return gotit;
			if (j[-1] == 0) return gotit;
		}
	}

	getid;
	for (i = mztab; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	for (j = idvar; *j; j++) if (isupper(*j)) *j = tolower(*j);
	for (i=mztab; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	getid;
	for (i=unittb; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	if (idvar[strlen(idvar)-1] == 's') idvar[strlen(idvar)-1] = '\0';
	for (i=unittb; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	getid;
	for (i = othertb; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	return(ID);
}

time_t getdate(p, now) char *p; struct timeb *now;
{
#define mcheck(f)	if (f>1) err++
	time_t monthadd();
	int err;
	struct tm *lt;
	struct timeb ftz;

	time_t sdate, tod;

	lptr = p;
	if (now == ((struct timeb *) NULL)) {
		now = &ftz;
		ftime(&ftz);
	}
	lt = localtime(&now->time);
	year = lt->tm_year;
	month = lt->tm_mon+1;
	day = lt->tm_mday;
	relsec = 0; relmonth = 0;
	timeflag=zoneflag=dateflag=dayflag=relflag=0;
	ourzone = now->timezone / 60;
	daylight = MAYBE;
	hh = mm = ss = 0;
	merid = 24;

	if (err = yyparse()) return (-1);

	mcheck(timeflag);
	mcheck(zoneflag);
	mcheck(dateflag);
	mcheck(dayflag);

	if (err) return (-1);

	if (dateflag || timeflag || dayflag) {
		sdate = dateconv(month,day,year,hh,mm,ss,merid,ourzone,daylight);
		if (sdate < 0) return -1;
	}
	else {
		sdate = now->time;
		if (relflag == 0)
			sdate -= (lt->tm_sec + lt->tm_min*60 +
				lt->tm_hour*(60L*60L));
	}

	sdate += relsec;
	sdate += monthadd(sdate, relmonth);

	if (dayflag) {
		tod = dayconv(dayord, dayreq, sdate);
		sdate += tod;
	}
	
	return sdate;
}

yyerror(s) char *s;
{}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 27
# define YYLAST 220
short yyact[]={

  11,  12,  19,   8,  13,  14,  15,   9,  10,  32,
  31,  30,  25,  17,  28,  24,  23,  33,  29,  26,
   7,   6,   5,   4,   3,   2,   1,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,  18,  16,  27,  20,  21,  22 };
short yypact[]={

-1000,-258,-1000,-1000,-1000,-1000,-1000,-1000, -45,-1000,
-1000,-245,-1000,-1000,-1000,-1000,-1000,-246,-1000,-249,
-1000,-1000,-1000, -25, -44, -29,-250,-1000,-251,-252,
-1000,-243,-1000,-1000 };
short yypgo[]={

   0,  26,  25,  24,  23,  22,  21,  20 };
short yyr1[]={

   0,   1,   1,   2,   2,   2,   2,   2,   3,   3,
   3,   3,   3,   4,   4,   6,   6,   5,   5,   5,
   5,   7,   7,   7,   7,   7,   7 };
short yyr2[]={

   0,   0,   2,   1,   1,   1,   1,   1,   2,   3,
   4,   5,   6,   1,   1,   1,   2,   3,   5,   2,
   4,   2,   2,   2,   1,   1,   1 };
short yychk[]={

-1000,  -1,  -2,  -3,  -4,  -5,  -6,  -7, 261, 265,
 266, 258, 259, 262, 263, 264, 260,  58, 259,  47,
 262, 263, 264, 261, 261, 261,  44, 260,  58,  47,
 261, 261, 261, 260 };
short yydef[]={

   1,  -2,   2,   3,   4,   5,   6,   7,   0,  13,
  14,   0,  15,  24,  25,  26,   8,   0,  16,   0,
  21,  22,  23,  19,   9,  17,   0,  10,   0,   0,
  20,  11,  18,  12 };
#
# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

int yydebug = 0; /* 1 for debugging */
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */

yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register YYSTYPE *yypvt;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

			if( yydebug ) printf( "error recovery discards char %d\n", yychar );

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

		if( yydebug ) printf("reduce %d\n",yyn);
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
case 3:

		{timeflag++;} break;
case 4:

		{zoneflag++;} break;
case 5:

		{dateflag++;} break;
case 6:

		{dayflag++;} break;
case 7:

		{relflag++;} break;
case 8:

		{hh = yypvt[-1]; mm = 0; ss = 0; merid = yypvt[-0];} break;
case 9:

		{hh = yypvt[-2]; mm = yypvt[-0]; merid = 24;} break;
case 10:

		{hh = yypvt[-3]; mm = yypvt[-1]; merid = yypvt[-0];} break;
case 11:

		{hh = yypvt[-4]; mm = yypvt[-2]; ss = yypvt[-0]; merid = 24;} break;
case 12:

		{hh = yypvt[-5]; mm = yypvt[-3]; ss = yypvt[-1]; merid = yypvt[-0];} break;
case 13:

		{ourzone = yypvt[-0]; daylight = STANDARD;} break;
case 14:

		{ourzone = yypvt[-0]; daylight = DAYLIGHT;} break;
case 15:

		{dayord = 1; dayreq = yypvt[-0];} break;
case 16:

		{dayord = yypvt[-1]; dayreq = yypvt[-0];} break;
case 17:

		{month = yypvt[-2]; day = yypvt[-0];} break;
case 18:

		{month = yypvt[-4]; day = yypvt[-2]; year = yypvt[-0];} break;
case 19:

		{month = yypvt[-1]; day = yypvt[-0];} break;
case 20:

		{month = yypvt[-3]; day = yypvt[-2]; year = yypvt[-0];} break;
case 21:

		{relsec +=  60L * yypvt[-1] * yypvt[-0];} break;
case 22:

		{relmonth += yypvt[-1] * yypvt[-0];} break;
case 23:

		{relsec += yypvt[-1];} break;
case 24:

		{relsec +=  60L * yypvt[-0];} break;
case 25:

		{relmonth += yypvt[-0];} break;
case 26:

		{relsec++;} break;
		}
		goto yystack;  /* stack new state and value */

	}
