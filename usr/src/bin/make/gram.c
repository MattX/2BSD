
# line 1 "gram.y"
#include "defs"


# line 7 "gram.y"
typedef union 
	{
	SHBLOCK yshblock;
	DEPBLOCK ydepblock;
	NAMEBLOCK ynameblock;
	CHARSTAR ycharstring;
	} YYSTYPE;
# define NAME 257
# define SHELLINE 258
# define START 259
# define COLON 260
# define DOUBLECOLON 261
# define EQUAL 262
# define A_STRING 263
# define VERSION 264
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern short yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
YYSTYPE yylval, yyval;

# line 24 "gram.y"
DEPBLOCK pp;
FSTATIC SHBLOCK prevshp;

FSTATIC NAMEBLOCK lefts[NLEFTS];
NAMEBLOCK leftp;
FSTATIC int nlefts;

LINEBLOCK lp, lpp;
FSTATIC DEPBLOCK prevdep;
FSTATIC int sepc;
# define YYERRCODE 256

# line 139 "gram.y"



#include <ctype.h>
CHARSTAR zznextc;	/* zero if need another line; otherwise points to next char */
int yylineno;
static char inmacro = NO;

yylex()
{
	register CHARSTAR p;
	register CHARSTAR q;
	static char word[128];
	CHARSTAR pword;

	pword = word;
	if(zznextc == 0)
		return( nextlin() );

	while( isspace(*zznextc) )
		++zznextc;

	if(inmacro == YES)
	{
		inmacro = NO;
		yylval.ycharstring = copys(zznextc);
		zznextc = 0;
		return(A_STRING);
	}

	if(*zznextc == CNULL)
		return( nextlin() );

	if(*zznextc == KOLON)
	{
		if(*++zznextc == KOLON)
		{
			++zznextc;
			return(DOUBLECOLON);
		}
		else
			return(COLON);
	}

	if(*zznextc == EQUALS)
	{
		inmacro = YES;
		++zznextc;
		return(EQUAL);
	}

	if(*zznextc == SKOLON)
		return( retsh(zznextc) );

	p = zznextc;
	q = word;

	while( ! ( funny[*p] & TERMINAL) )
		*q++ = *p++;

	if(p != zznextc)
	{
		*q = CNULL;
		yylval.ycharstring = copys(pword);
		if(*p == RCURLY)
		{
			zznextc = p+1;
			return(VERSION);
		}
		if(*p == LCURLY)
			p++;
		zznextc = p;
		return(NAME);
		}

	else
	{
		(void)fprintf(stderr,"Bad character %c (octal %o), line %d",
			*zznextc,*zznextc,yylineno);
		fatal(Nullstr);
	}
	return(0);	/* never executed */
}


retsh(q)
register CHARSTAR q;
{
	register CHARSTAR p;
	register int c;
	extern CHARSTAR *linesptr;
	SHBLOCK sp;

	for(p=q+1 ; *p==BLANK||*p==TAB ; ++p)  ;

	sp = ALLOC(shblock);
	sp->nextsh = 0;
	sp->shbp = (fin == NULL ? p : copys(p) );
	yylval.yshblock = sp;
	zznextc = 0;
/*
 *	The following if-else "thing" eats up newlines within
 *	shell blocks.
 */
	if(fin == NULL)
	{
		if(linesptr[0])
			while(linesptr[1] && equal(linesptr[1], "\n"))
			{
				yylineno++;
				linesptr++;
			}
	}
	else
	{
		while((c = GETC()) == NEWLINE)
			yylineno++;
		if(c != EOF)
			(void)ungetc(c, fin);
	}
	return(SHELLINE);
}

nextlin()
{
	register char c;
	register CHARSTAR p, t;
	static char yytext[INMAX+1];
	CHARSTAR text;
	char templin[INMAX];
	char lastch;
	CHARSTAR lastchp;
	extern CHARSTAR *linesptr;
	int incom;
	int kc;
	int nflg;
	int poundflg;

again:
	incom = 0;
	zznextc = 0;
	poundflg = 0;

	if(fin == NULL)
	{
		if( (text = *linesptr++) == 0)
			return(0);
		++yylineno;
		(void)copstr(yytext, text);
	}

	else
	{
		yytext[0] = CNULL;
		for(p=yytext ; ; ++p)
		{
			kc = GETC();
			if(kc == EOF)
			{
				*p = CNULL;
				return(0);
			}
			else if(kc == SKOLON)
				++incom;
			else if (kc == TAB && p == yytext)
				++incom;
			else if (kc==POUND && !incom && yytext[0] != TAB)
			{
				poundflg++;
				kc = CNULL;
			}
			else if (kc == NEWLINE)
			{
				++yylineno;
				if(p==yytext || p[-1]!=BACKSLASH)
					break;
				if(incom || yytext[0] == TAB)
				{
					if(p >= &yytext[INMAX])
						fatal("line too long");
					*p++ = NEWLINE;
				}
				else
					p[-1] = BLANK;
				nflg = YES;
				while( kc = GETC())
				{
					if(kc != TAB && kc != BLANK && kc != NEWLINE)
						break;
					if(incom || yytext[0] == TAB)
					{
						if(nflg == YES && kc == TAB)
						{
							nflg = NO;
							continue;
						}
						if(kc == NEWLINE)
						{
							nflg = YES;
						}

						if(p >= &yytext[INMAX])
							fatal("line too long");
						*p++ = kc;
					}
					if(kc == NEWLINE)
						++yylineno;
				}

				if(kc == EOF)
				{
					*p = CNULL;
					return(0);
				}
			}
			if(p >= &yytext[INMAX])
				fatal("line too long");
			*p = kc;
		}
		*p = CNULL;
		text = yytext;
	}

	c = text[0];

	if(c == TAB)  
		return( retsh(text) );

/*
 *	DO include FILES HERE.
 */
	if(sindex(text, "include") == 0 && (text[7] == BLANK || text[7] == TAB))
	{
		CHARSTAR pfile;

		for(p = &text[8]; *p != CNULL; p++)
			if(*p != TAB ||
			   *p != BLANK)
				break;
		pfile = p;
		for(;	*p != CNULL	&&
			*p != NEWLINE	&&
			*p != TAB	&&
			*p != BLANK; p++);
		if(*p != CNULL)
			*p = CNULL;

/*
 *	Start using new file.
 */
		fstack(pfile, &fin, &yylineno);
		goto again;
	}
	if(isalpha(c) || isdigit(c) || c==BLANK || c==DOT)
		for(p=text+1; *p!=CNULL; p++)
			if(*p == KOLON || *p == EQUALS)
				break;

/* substtitute for macros on dependency line up to the semicolon if any */
	if(*p != EQUALS)
	{
		for(t = yytext ; *t!=CNULL && *t!=SKOLON ; ++t);

		lastchp = t;
		lastch = *t;
		*t = CNULL;

		subst(yytext, templin);	/* Substitute for macros on dep lines */

		if(lastch)
		{
			for(t = templin ; *t ; ++t);
			*t = lastch;
			while( *++t = *++lastchp ) ;
		}

		p = templin;
		t = yytext;
		while( *t++ = *p++ );
	}

	if(poundflg == 0 || yytext[0] != CNULL)
	{
		zznextc = text;
		return(START);
	}
	else
		goto again;
}

#include <stdio.h>

/*
 *	GETC automatically unravels stacked include files. That is,
 *	during include file processing, when a new file is encountered
 *	fstack will stack the FILE pointer argument. Subsequent
 *	calls to GETC with the new FILE pointer will get characters
 *	from the new file. When an EOF is encountered, GETC will
 *	check to see if the file pointer has been stacked. If so,
 *	a character from the previous file will be returned.
 *	The external references are "GETC()" and "fstack(fname,stream,lno)".
 *	"Fstack(stfname,ream,lno)" is used to stack an old file pointer before
 *	the new file is assigned to the same variable. Also stacked are the
 *	file name and the old current lineno, generally, yylineno.
 */



static int morefiles;
static struct sfiles
{
	char sfname[64];
	FILE *sfilep;
	int syylno;
} sfiles[20];

GETC()
{
	register int c;

	c = getc(fin);
	while(c == EOF && morefiles)
	{
		(void)fclose(fin);
		yylineno = sfiles[--morefiles].syylno;
		fin = sfiles[morefiles].sfilep;
		c = getc(fin);
	}
	return(c);
}
fstack(newname, oldfp, oldlno)
register char *newname;
register FILE **oldfp;
register int *oldlno;
{
	if(access(newname, 4) != 0)
/*
 *	This get line can be removed if used elsewhere than make.
 */
	if(co(newname, YES) == NO && get(newname, CD, Nullstr) == NO)
		fatal1("Cannot read, checkout, or get %s", newname);
	if(IS_ON(DBUG))
		(void)printf("Include file: \"%s\"\n", newname);
/*
 *	Stack the new file name, the old file pointer and the
 *	old yylineno;
 */
	strcat(sfiles[morefiles].sfname, newname);
	sfiles[morefiles].sfilep = *oldfp;
	sfiles[morefiles++].syylno = *oldlno;
	yylineno = 0;
	if((*oldfp=fopen(newname, "r")) == NULL)
		fatal1("Cannot open %s", newname);
}

short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
# define YYNPROD 22
# define YYLAST 26
short yyact[]={

  16,  17,  17,  22,  12,  12,   4,  14,  15,   3,
  23,  20,   7,   8,  13,   5,   2,   1,  11,   9,
  10,   6,  18,  19,   0,  21 };
short yypact[]={

-1000,-250,-1000,-245,-1000,-1000,-253,-262,-1000,-247,
-1000,-252,-263,-1000,-1000,-1000,-260,-1000,-1000,-248,
-1000,-1000,-1000,-1000 };
short yypgo[]={

   0,  23,  22,  21,  13,  19,  18,  17,  16,  15,
  14 };
short yyr1[]={

   0,   7,   7,   8,   8,   8,   8,   9,   3,   3,
   4,   4,   5,   5,   6,   6,  10,  10,   2,   2,
   1,   1 };
short yyr2[]={

   0,   0,   2,   1,   2,   4,   1,   3,   1,   2,
   1,   2,   0,   1,   1,   2,   1,   1,   0,   1,
   1,   2 };
short yychk[]={

-1000,  -7,  -8, 259, 256,  -9,  -3, 257,  -4,  -5,
  -4,  -6, 257, -10, 260, 261, 262, 264,  -2,  -1,
 258,  -4, 263, 258 };
short yydef[]={

   1,  -2,   2,   3,   6,   4,  12,  10,   8,  18,
   9,  13,  10,  14,  16,  17,   0,  11,   5,  19,
  20,  15,   7,  21 };
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif not lint

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
			
case 5:
# line 43 "gram.y"
 {
	    if(mainname == NULL && IS_OFF(INTRULE))
		if(lefts[0]->namep[0] != DOT || any(lefts[0]->namep, SLASH) )
			mainname = lefts[0];
	    while( --nlefts >= 0)
	    {
		leftp = lefts[nlefts];
		if(leftp->septype == 0)
			leftp->septype = sepc;
		else if(leftp->septype != sepc)
			(void)fprintf(stderr, "Inconsistent rules lines for `%s'\n",
				leftp->namep);
		else if(sepc==ALLDEPS && *(leftp->namep)!=DOT && yypvt[-0].yshblock!=0)
		{
			for(lp=leftp->linep; lp->nextline!=0; lp=lp->nextline)
			    if(lp->shp)
				(void)fprintf(stderr, "Multiple rules lines for `%s'\n",
				    leftp->namep);
		}

		lp = ALLOC(lineblock);
		lp->nextline = 0;
		lp->depp = yypvt[-1].ydepblock;
		lp->shp = yypvt[-0].yshblock;

		if(equal(leftp->namep, ".SUFFIXES") && yypvt[-1].ydepblock==0)
			leftp->linep = 0;
		else if(leftp->linep == 0)
			leftp->linep = lp;
		else
		{
			for(lpp = leftp->linep; lpp->nextline!=0;
				lpp = lpp->nextline) ;
				if(sepc==ALLDEPS && leftp->namep[0]==DOT)
					lpp->shp = 0;
			lpp->nextline = lp;
		}
	    }
	} break;
case 7:
# line 85 "gram.y"

	{
		setvar(yypvt[-2].ynameblock, yypvt[-0].ycharstring);
	} break;
case 8:
# line 91 "gram.y"
 { lefts[0] = yypvt[-0].ynameblock; nlefts = 1; } break;
case 9:
# line 92 "gram.y"
 { if(nlefts>NLEFTS) fatal("Too many lefts");
		lefts[nlefts++] = yypvt[-0].ynameblock; } break;
case 10:
# line 96 "gram.y"

	{
	if((yyval.ynameblock = srchname(yypvt[-0].ynameblock)) == 0)
		yyval.ynameblock = makename(yypvt[-0].ynameblock);
	} break;
case 11:
# line 101 "gram.y"
 
	{
	if((yyval.ynameblock = srchname(yypvt[-1].ynameblock)) == 0)
		yyval.ynameblock = makename(yypvt[-1].ynameblock);
	} break;
case 12:
# line 108 "gram.y"
 { fatal1("Must be a separator on rules line %d", yylineno); } break;
case 14:
# line 112 "gram.y"
 { prevdep = 0;  yyval.ydepblock = 0; } break;
case 15:
# line 113 "gram.y"
 {
			  pp = ALLOC(depblock);
			  pp->nextdep = 0;
			  pp->depname = yypvt[-0].ynameblock;
			  if(prevdep == 0) yyval.ydepblock = pp;
			  else  prevdep->nextdep = pp;
			  prevdep = pp;
			  } break;
case 16:
# line 123 "gram.y"
 { sepc = ALLDEPS; } break;
case 17:
# line 124 "gram.y"
 { sepc = SOMEDEPS; } break;
case 18:
# line 127 "gram.y"
 {yyval.yshblock = 0; } break;
case 19:
# line 128 "gram.y"
 { yyval.yshblock = yypvt[-0].yshblock; } break;
case 20:
# line 131 "gram.y"
 { yyval.yshblock = yypvt[-0].yshblock;  prevshp = yypvt[-0].yshblock; } break;
case 21:
# line 132 "gram.y"
 { yyval.yshblock = yypvt[-1].yshblock;
			prevshp->nextsh = yypvt[-0].yshblock;
			prevshp = yypvt[-0].yshblock;
			} break;
		}
		goto yystack;  /* stack new state and value */

	}
