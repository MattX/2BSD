
# line 2 "m4y.y"
extern long	evalval;
#define	YYSTYPE	long
# define DIGITS 257
# define GT 258
# define GE 259
# define LT 260
# define LE 261
# define NE 262
# define EQ 263
# define POWER 264
# define UMINUS 265
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

# line 42 "m4y.y"


yylex() {
	extern char *pe;

	while (*pe==' ' || *pe=='\t' || *pe=='\n')
		pe++;
	switch(*pe) {
	case '\0':
	case '+':
	case '-':
	case '/':
	case '%':
	case '(':
	case ')':
		return(*pe++);
	case '^':
		pe++;
		return(POWER);
	case '*':
		return(peek('*', POWER, '*'));
	case '>':
		return(peek('=', GE, GT));
	case '<':
		return(peek('=', LE, LT));
	case '=':
		return(peek('=', EQ, EQ));
	case '|':
		return(peek('|', '|', '|'));
	case '&':
		return(peek('&', '&', '&'));
	case '!':
		return(peek('=', NE, '!'));
	default:
		evalval = 0;
		while (*pe >= '0' && *pe <= '9')
			evalval = evalval*10 + *pe++ - '0';
		return(DIGITS);
	}
}

peek(c, r1, r2)
{
	if (*++pe != c)
		return(r2);
	++pe;
	return(r1);
}

yyerror(s)
char *s;
{
}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 28,
	258, 0,
	259, 0,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	-2, 6,
-1, 29,
	258, 0,
	259, 0,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	-2, 7,
-1, 30,
	258, 0,
	259, 0,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	-2, 8,
-1, 31,
	258, 0,
	259, 0,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	-2, 9,
-1, 32,
	258, 0,
	259, 0,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	-2, 10,
-1, 33,
	258, 0,
	259, 0,
	260, 0,
	261, 0,
	262, 0,
	263, 0,
	-2, 11,
	};
# define YYNPROD 22
# define YYLAST 264
short yyact[]={

  20,   9,  21,   1,  40,  18,  16,   0,  17,   0,
  19,  20,   9,   0,   0,  20,  18,  16,   0,  17,
  18,  19,  20,   9,   0,  19,   0,  18,  16,  20,
  17,   0,  19,   3,  18,  16,  20,  17,   0,  19,
   4,  18,  16,   6,  17,   5,  19,   2,   0,   0,
   0,  22,  23,  24,  25,   0,  26,  27,  28,  29,
  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   8,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   8,   0,
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
   0,  12,  13,  14,  15,  11,  10,  21,   0,   0,
   0,   0,  12,  13,  14,  15,  11,  10,  21,   0,
   0,   0,  21,  12,  13,  14,  15,  11,  10,  21,
  12,  13,  14,  15,  11,  10,  21,   7,   0,   0,
   0,   0,   0,  21 };
short yypact[]={

   0,-1000, -26,   0,   0,   0,   0,-1000,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  -8, -37,-1000,-1000, -15,  -8,  -1,  -1,
  -1,  -1,  -1,  -1, -22, -22,-262,-262,-262,-262,
-1000 };
short yypgo[]={

   0,   3,  47 };
short yyr1[]={

   0,   1,   1,   2,   2,   2,   2,   2,   2,   2,
   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
   2,   2 };
short yyr2[]={

   0,   1,   0,   3,   3,   2,   3,   3,   3,   3,
   3,   3,   3,   3,   3,   3,   3,   3,   3,   2,
   2,   1 };
short yychk[]={

-1000,  -1,  -2,  33,  40,  45,  43, 257, 124,  38,
 263, 262, 258, 259, 260, 261,  43,  45,  42,  47,
  37, 264,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,
  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,  -2,
  41 };
short yydef[]={

   2,  -2,   1,   0,   0,   0,   0,  21,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   5,   0,  19,  20,   3,   4,  -2,  -2,
  -2,  -2,  -2,  -2,  12,  13,  14,  15,  16,  18,
  17 };
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
			
case 1:
# line 17 "m4y.y"
{ evalval = yypvt[-0]; } break;
case 2:
# line 18 "m4y.y"
{ evalval = 0; } break;
case 3:
# line 21 "m4y.y"
{ yyval = (yypvt[-2]!=0 || yypvt[-0]!=0) ? 1 : 0; } break;
case 4:
# line 22 "m4y.y"
{ yyval = (yypvt[-2]!=0 && yypvt[-0]!=0) ? 1 : 0; } break;
case 5:
# line 23 "m4y.y"
{ yyval = yypvt[-0] == 0; } break;
case 6:
# line 24 "m4y.y"
{ yyval = yypvt[-2] == yypvt[-0]; } break;
case 7:
# line 25 "m4y.y"
{ yyval = yypvt[-2] != yypvt[-0]; } break;
case 8:
# line 26 "m4y.y"
{ yyval = yypvt[-2] > yypvt[-0]; } break;
case 9:
# line 27 "m4y.y"
{ yyval = yypvt[-2] >= yypvt[-0]; } break;
case 10:
# line 28 "m4y.y"
{ yyval = yypvt[-2] < yypvt[-0]; } break;
case 11:
# line 29 "m4y.y"
{ yyval = yypvt[-2] <= yypvt[-0]; } break;
case 12:
# line 30 "m4y.y"
{ yyval = (yypvt[-2]+yypvt[-0]); } break;
case 13:
# line 31 "m4y.y"
{ yyval = (yypvt[-2]-yypvt[-0]); } break;
case 14:
# line 32 "m4y.y"
{ yyval = (yypvt[-2]*yypvt[-0]); } break;
case 15:
# line 33 "m4y.y"
{ yyval = (yypvt[-2]/yypvt[-0]); } break;
case 16:
# line 34 "m4y.y"
{ yyval = (yypvt[-2]%yypvt[-0]); } break;
case 17:
# line 35 "m4y.y"
{ yyval = (yypvt[-1]); } break;
case 18:
# line 36 "m4y.y"
{ for (yyval=1; yypvt[-0]-->0; yyval *= yypvt[-2]); } break;
case 19:
# line 37 "m4y.y"
{ yyval = yypvt[-0]-1; yyval = -yypvt[-0]; } break;
case 20:
# line 38 "m4y.y"
{ yyval = yypvt[-0]-1; yyval = yypvt[-0]; } break;
case 21:
# line 39 "m4y.y"
{ yyval = evalval; } break;
		}
		goto yystack;  /* stack new state and value */

	}
