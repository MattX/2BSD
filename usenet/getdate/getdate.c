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

# line 3 "getdate.y"
#include "datedcl.h"
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

# line 64 "getdate.y"


#include "getdate.h"
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

#ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
#endif
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

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
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

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
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
# line 10 "getdate.y"

		{timeflag++;} break;
case 4:
# line 12 "getdate.y"

		{zoneflag++;} break;
case 5:
# line 14 "getdate.y"

		{dateflag++;} break;
case 6:
# line 16 "getdate.y"

		{dayflag++;} break;
case 7:
# line 18 "getdate.y"

		{relflag++;} break;
case 8:
# line 21 "getdate.y"

		{hh = yypvt[-1]; mm = 0; ss = 0; merid = yypvt[-0];} break;
case 9:
# line 23 "getdate.y"

		{hh = yypvt[-2]; mm = yypvt[-0]; merid = 24;} break;
case 10:
# line 25 "getdate.y"

		{hh = yypvt[-3]; mm = yypvt[-1]; merid = yypvt[-0];} break;
case 11:
# line 27 "getdate.y"

		{hh = yypvt[-4]; mm = yypvt[-2]; ss = yypvt[-0]; merid = 24;} break;
case 12:
# line 29 "getdate.y"

		{hh = yypvt[-5]; mm = yypvt[-3]; ss = yypvt[-1]; merid = yypvt[-0];} break;
case 13:
# line 32 "getdate.y"

		{ourzone = yypvt[-0]; daylight = STANDARD;} break;
case 14:
# line 34 "getdate.y"

		{ourzone = yypvt[-0]; daylight = DAYLIGHT;} break;
case 15:
# line 37 "getdate.y"

		{dayord = 1; dayreq = yypvt[-0];} break;
case 16:
# line 39 "getdate.y"

		{dayord = yypvt[-1]; dayreq = yypvt[-0];} break;
case 17:
# line 42 "getdate.y"

		{month = yypvt[-2]; day = yypvt[-0];} break;
case 18:
# line 44 "getdate.y"

		{month = yypvt[-4]; day = yypvt[-2]; year = yypvt[-0];} break;
case 19:
# line 46 "getdate.y"

		{month = yypvt[-1]; day = yypvt[-0];} break;
case 20:
# line 48 "getdate.y"

		{month = yypvt[-3]; day = yypvt[-2]; year = yypvt[-0];} break;
case 21:
# line 52 "getdate.y"

		{relsec +=  60L * yypvt[-1] * yypvt[-0];} break;
case 22:
# line 54 "getdate.y"

		{relmonth += yypvt[-1] * yypvt[-0];} break;
case 23:
# line 56 "getdate.y"

		{relsec += yypvt[-1];} break;
case 24:
# line 58 "getdate.y"

		{relsec +=  60L * yypvt[-0];} break;
case 25:
# line 60 "getdate.y"

		{relmonth += yypvt[-0];} break;
case 26:
# line 62 "getdate.y"

		{relsec++;} break;
		}
		goto yystack;  /* stack new state and value */

	}
