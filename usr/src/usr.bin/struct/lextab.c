# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin ={stdin}, *yyout ={stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
#include "y.tab.h"
#include "b.h"
#undef	input
#define input()	ninput()
#undef	unput
#define unput(c)	nunput(c)
extern int yylval;
#define xxbpmax	1700
char xxbuf[xxbpmax + 2];
int xxbp = -1;
#define xxunmax	200
char xxunbuf[xxunmax + 2];
int xxunbp = -1;


int blflag;
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
char *xxtbuff;
int xxj, xxn, xxk;
char *xxp;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
		{
				blflag = 1;
				sscanf(&yytext[1],"%d",&xxn);
				xxtbuff = malloc(2*xxn+3);
				for (xxj = xxk = 1; xxj <= xxn; ++xxj)
					{
					xxtbuff[xxk] = ninput();
					if (xxtbuff[xxk] == '"')
						xxtbuff[++xxk] = '"';
					++xxk;
					}
				xxtbuff[0] = xxtbuff[xxk++] = '"';
				xxtbuff[xxk] = '\0';
				putback(xxtbuff);
				free(xxtbuff);

				backup(yytext[0]);
				blflag = 0;
				xxbp = -1;
				}
break;
case 2:
		{fixval(); xxbp = -1; return(xxif);}
break;
case 3:
		{fixval(); xxbp = -1; return(xxelse);}
break;
case 4:
		{fixval(); xxbp = -1; return(xxrept); }
break;
case 5:
		{fixval(); xxbp = -1; return(xxwhile); }
break;
case 6:
		{ fixval(); xxbp = -1; return(xxuntil); }
break;
case 7:
		{fixval(); xxbp = -1; return(xxdo); }
break;
case 8:
		{fixval(); xxbp = -1; return(xxswitch); }
break;
case 9:
		{fixval(); xxbp = -1; return(xxcase); }
break;
case 10:
		{fixval(); xxbp = -1; return(xxdefault); }
break;
case 11:
		{fixval(); xxbp = -1; return(xxend); }
break;
case 12:
	case 13:
	case 14:
	{fixval(); xxbp = -1; return(xxident); }
break;
case 15:
		{xxbuf[0] = ' '; fixval(); xxbp = -1; return(xxnum); }
break;
case 16:
case 17:
		case 18:
	case 19:
	case 20:
		{fixval(); xxbp = -1; return(xxnum); }
break;
case 21:
		{ putback(">"); xxbp = -1; }
break;
case 22:
		{ putback(">=");xxbp = -1; }
break;
case 23:
		{ putback("<"); xxbp = -1; }
break;
case 24:
		{ putback("<="); xxbp = -1; }
break;
case 25:
		{ putback("=="); xxbp = -1; }
break;
case 26:
		{ putback("!="); xxbp = -1; }
break;
case 27:
		{ putback("!"); xxbp = -1; }
break;
case 28:
		{ putback("||"); xxbp = -1; }
break;
case 29:
		{ putback("&&"); xxbp = -1; }
break;
case 30:
	{fixval(); xxbp = -1;  return(xxge);  }
break;
case 31:
	{fixval(); xxbp = -1;  return(xxle); }
break;
case 32:
		{fixval(); xxbp = -1; return(xxeq); }
break;
case 33:
		{fixval(); xxbp = -1; return(xxne); }
break;
case 34:
		{fixval(); xxbp = -1; return('|'); }
break;
case 35:
		{fixval(); xxbp = -1;  return('&'); }
break;
case 36:
		{fixval(); xxbp = -1; return('^'); }
break;
case 37:
		{fixval(); xxbp = -1; return(xxcom); }
break;
case 38:
	{fixval(); xxbp = -1; return(xxstring); }
break;
case 39:
			{
					fixval();
					xxp = yylval;
					xxn = slength(xxp);
					xxtbuff = malloc(2*xxn+1);
					xxtbuff[0] = '"';
					for (xxj = xxk = 1; xxj < xxn-1; ++xxj)
						{
						if (xxp[xxj] == '\'' && xxp[++xxj] == '\'')
							xxtbuff[xxk++] = '\'';
						else if (xxp[xxj] == '"')
							{
							xxtbuff[xxk++] = '"';
							xxtbuff[xxk++] = '"';
							}
						else
							xxtbuff[xxk++] = xxp[xxj];
						}
					xxtbuff[xxk++] = '"';
					xxtbuff[xxk] = '\0';
					free(xxp);
					yylval = xxtbuff;
					xxbp = -1;
					return(xxstring);
					}
break;
case 40:
	xxbp = -1;
break;
case 41:
	{xxbp = -1; if (newflag) {fixval(); return('\n'); }  }
break;
case 42:
	{fixval(); xxbp = -1; return(yytext[0]); }
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

rdchar()
	{
	int c;
	if (xxunbp >= 0)
		return(xxunbuf[xxunbp--]);
	c = getchar();
	if (c == EOF) return('\0');
	else return((char)c);
	}

backup(c)
char c;
	{
	if (++xxunbp > xxunmax)
		{
		xxunbuf[xxunmax + 1] = '\0';
		error("RATFOR beautifying; input backed up too far during lex:\n",
			xxunbuf,"\n");
		}
	xxunbuf[xxunbp] = c;
	}

nunput(c)
char c;
	{
	backup(c);
	if (xxbp < 0) return;
	if (c != xxbuf[xxbp])
		{
		xxbuf[xxbp + 1] = '\0';
		error("RATFOR beautifying; lex call of nunput with wrong char:\n",
			xxbuf,"\n");
		}
	for ( --xxbp; xxbp >= 0 && (xxbuf[xxbp] == ' ' || xxbuf[xxbp] == '\t'); --xxbp)
		backup(xxbuf[xxbp]);
	xxbuf[xxbp+1] = '\0';
	}

ninput()
	{
	char c,d;
	if (blflag) c = rdchar();
	else
		while ( (c = rdchar()) == ' ' || c == '\t')
		addbuf(c);
	if (c != '\n')
		return(addbuf(c));
	while ( (d = rdchar()) == ' ' || d == '\t');
	if (d == '&')
		return(ninput());
	backup(d);
	return(addbuf('\n'));
	}

addbuf(c)
char c;
	{
	if (++xxbp > xxbpmax)
		{
		xxbuf[xxbpmax +1] = '\0';
		error("RATFOR beautifying; buffer xxbuf too small for token beginning:\n",
			xxbuf,"\n");
		}
	xxbuf[xxbp] = c;
	xxbuf[xxbp + 1] = '\0';
	return(c);
	}


fixval()
	{
	int i, j, k;
	for (j = 0; xxbuf[j] == ' ' || xxbuf[j] == '\t'; ++j);
	for (k = j; xxbuf[k] != '\0'; ++k);
	for (--k; k > j && xxbuf[k] == ' ' || xxbuf[k]  == '\t'; --k);
	xxbuf[k+1] = '\0';
	i = slength(&xxbuf[j]) + 1;
	yylval = malloc(i);
	str_copy(&xxbuf[j],yylval,i);
	}



putback(str)
char *str;
	{
	int i;
	for (i = 0; str[i] != '\0'; ++i);
	for (--i; i >= 0; --i)
		backup(str[i]);
	}

int yyvstop[] ={
0,

42,
0,

41,
42,
0,

42,
0,

42,
0,

37,
42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

17,
-16,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

42,
0,

14,
0,

42,
0,

42,
0,

40,
41,
42,
0,

33,
0,

38,
0,

37,
0,

35,
0,

39,
0,

36,
0,

20,
0,

17,
0,

31,
0,

32,
0,

30,
0,

7,
0,

2,
0,

34,
0,

15,
0,

1,
0,

18,
0,

11,
0,

19,
0,

25,
0,

22,
0,

21,
0,

24,
0,

23,
0,

26,
0,

28,
0,

9,
0,

3,
0,

29,
0,

27,
0,

16,
0,

6,
0,

5,
0,

12,
0,

4,
0,

8,
0,

13,
0,

10,
0,
0};
# define YYTYPE int
struct yywork { YYTYPE verify, advance; } yycrank[] ={
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	1,4,	
0,0,	0,0,	32,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,5,	1,6,	
1,7,	0,0,	31,30,	1,8,	
1,9,	1,10,	8,33,	1,11,	
1,3,	11,37,	35,34,	1,12,	
0,0,	1,13,	0,0,	0,0,	
0,0,	0,0,	0,0,	67,96,	
0,0,	2,28,	69,98,	70,99,	
71,100,	1,14,	1,15,	1,16,	
5,29,	14,49,	15,50,	16,51,	
1,17,	1,18,	1,19,	17,52,	
20,57,	21,58,	1,20,	24,61,	
53,86,	18,53,	56,88,	19,55,	
2,5,	19,56,	2,7,	1,21,	
1,22,	2,8,	1,23,	18,54,	
1,24,	2,11,	22,59,	23,60,	
52,85,	2,12,	55,87,	58,89,	
59,90,	60,91,	1,25,	61,92,	
72,101,	1,25,	6,30,	41,68,	
7,32,	1,25,	42,69,	2,14,	
2,15,	2,16,	36,64,	6,30,	
44,73,	7,0,	2,17,	2,18,	
2,19,	9,34,	39,66,	40,67,	
2,20,	42,70,	44,74,	43,71,	
45,75,	1,26,	9,34,	1,27,	
26,62,	2,21,	2,22,	46,76,	
2,23,	66,95,	2,24,	6,31,	
68,97,	7,32,	43,72,	73,102,	
6,30,	6,30,	7,32,	7,32,	
6,30,	74,103,	7,32,	75,104,	
76,105,	6,30,	9,34,	7,32,	
77,106,	78,107,	47,77,	9,35,	
9,34,	47,48,	47,78,	9,34,	
47,79,	82,107,	81,107,	85,109,	
9,34,	47,80,	86,110,	47,81,	
47,82,	87,111,	89,112,	2,26,	
81,108,	2,27,	10,36,	10,36,	
10,36,	10,36,	10,36,	10,36,	
10,36,	10,36,	10,36,	10,36,	
12,38,	12,38,	12,38,	12,38,	
12,38,	12,38,	12,38,	12,38,	
12,38,	12,38,	79,107,	90,113,	
91,114,	92,115,	6,30,	95,116,	
7,32,	6,30,	97,117,	7,32,	
103,118,	6,30,	105,119,	7,32,	
106,107,	79,107,	107,120,	108,107,	
110,121,	9,34,	112,122,	113,123,	
9,34,	114,124,	115,125,	13,47,	
9,34,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	13,13,	
13,13,	13,13,	13,13,	117,126,	
119,127,	12,39,	121,128,	122,129,	
123,130,	12,40,	12,41,	12,42,	
126,131,	128,132,	0,0,	0,0,	
12,43,	0,0,	12,44,	12,45,	
0,0,	0,0,	0,0,	0,0,	
12,46,	25,25,	25,25,	25,25,	
25,25,	25,25,	25,25,	25,25,	
25,25,	25,25,	25,25,	27,63,	
27,63,	27,63,	27,63,	27,63,	
27,63,	27,63,	27,63,	27,63,	
27,63,	13,48,	13,48,	0,0,	
38,38,	38,38,	38,38,	38,38,	
38,38,	38,38,	38,38,	38,38,	
38,38,	38,38,	48,83,	0,0,	
48,83,	0,0,	0,0,	48,84,	
48,84,	48,84,	48,84,	48,84,	
48,84,	48,84,	48,84,	48,84,	
48,84,	0,0,	25,25,	25,25,	
25,25,	25,25,	25,25,	25,25,	
25,25,	25,25,	25,25,	25,25,	
25,25,	25,25,	25,25,	25,25,	
25,25,	25,25,	25,25,	25,25,	
25,25,	25,25,	25,25,	25,25,	
25,25,	25,25,	25,25,	25,25,	
38,65,	38,65,	65,93,	0,0,	
65,93,	0,0,	0,0,	65,94,	
65,94,	65,94,	65,94,	65,94,	
65,94,	65,94,	65,94,	65,94,	
65,94,	83,84,	83,84,	83,84,	
83,84,	83,84,	83,84,	83,84,	
83,84,	83,84,	83,84,	93,94,	
93,94,	93,94,	93,94,	93,94,	
93,94,	93,94,	93,94,	93,94,	
93,94,	0,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] ={
0,	0,	0,
yycrank+-1,	0,		0,	
yycrank+-47,	yysvec+1,	0,	
yycrank+0,	0,		yyvstop+1,
yycrank+0,	0,		yyvstop+3,
yycrank+3,	0,		yyvstop+6,
yycrank+-101,	0,		yyvstop+8,
yycrank+-103,	0,		yyvstop+10,
yycrank+4,	0,		yyvstop+13,
yycrank+-116,	0,		yyvstop+15,
yycrank+126,	0,		yyvstop+17,
yycrank+3,	0,		yyvstop+19,
yycrank+136,	0,		yyvstop+21,
yycrank+173,	0,		yyvstop+23,
yycrank+4,	0,		yyvstop+26,
yycrank+5,	yysvec+10,	yyvstop+28,
yycrank+6,	0,		yyvstop+30,
yycrank+6,	0,		yyvstop+32,
yycrank+8,	0,		yyvstop+34,
yycrank+3,	0,		yyvstop+36,
yycrank+2,	0,		yyvstop+38,
yycrank+4,	0,		yyvstop+40,
yycrank+3,	0,		yyvstop+42,
yycrank+13,	0,		yyvstop+44,
yycrank+3,	0,		yyvstop+46,
yycrank+205,	0,		yyvstop+48,
yycrank+4,	0,		yyvstop+50,
yycrank+215,	0,		yyvstop+52,
yycrank+0,	0,		yyvstop+54,
yycrank+0,	0,		yyvstop+58,
yycrank+0,	yysvec+6,	0,	
yycrank+4,	0,		yyvstop+60,
yycrank+-4,	yysvec+7,	yyvstop+62,
yycrank+0,	0,		yyvstop+64,
yycrank+0,	yysvec+9,	0,	
yycrank+7,	0,		yyvstop+66,
yycrank+6,	yysvec+10,	0,	
yycrank+0,	0,		yyvstop+68,
yycrank+228,	0,		yyvstop+70,
yycrank+8,	0,		0,	
yycrank+6,	0,		0,	
yycrank+6,	0,		0,	
yycrank+5,	0,		0,	
yycrank+22,	0,		0,	
yycrank+11,	0,		0,	
yycrank+10,	0,		0,	
yycrank+17,	0,		0,	
yycrank+57,	yysvec+38,	yyvstop+72,
yycrank+243,	0,		0,	
yycrank+0,	0,		yyvstop+74,
yycrank+0,	0,		yyvstop+76,
yycrank+0,	0,		yyvstop+78,
yycrank+9,	0,		0,	
yycrank+6,	0,		0,	
yycrank+0,	0,		yyvstop+80,
yycrank+11,	0,		0,	
yycrank+10,	0,		0,	
yycrank+0,	0,		yyvstop+82,
yycrank+15,	0,		0,	
yycrank+23,	0,		0,	
yycrank+13,	0,		0,	
yycrank+26,	0,		0,	
yycrank+0,	0,		yyvstop+84,
yycrank+0,	yysvec+27,	yyvstop+86,
yycrank+0,	0,		yyvstop+88,
yycrank+287,	0,		0,	
yycrank+33,	0,		0,	
yycrank+9,	0,		0,	
yycrank+28,	0,		0,	
yycrank+12,	0,		0,	
yycrank+13,	0,		0,	
yycrank+14,	0,		0,	
yycrank+54,	0,		0,	
yycrank+93,	0,		0,	
yycrank+29,	0,		0,	
yycrank+101,	0,		0,	
yycrank+31,	0,		0,	
yycrank+42,	0,		0,	
yycrank+40,	yysvec+48,	0,	
yycrank+93,	0,		0,	
yycrank+0,	yysvec+79,	0,	
yycrank+61,	0,		0,	
yycrank+47,	0,		0,	
yycrank+297,	0,		0,	
yycrank+0,	yysvec+83,	yyvstop+90,
yycrank+94,	0,		0,	
yycrank+101,	0,		0,	
yycrank+100,	0,		0,	
yycrank+0,	0,		yyvstop+92,
yycrank+101,	0,		0,	
yycrank+111,	0,		0,	
yycrank+123,	0,		0,	
yycrank+121,	0,		0,	
yycrank+307,	0,		0,	
yycrank+0,	yysvec+93,	yyvstop+94,
yycrank+153,	0,		0,	
yycrank+0,	0,		yyvstop+96,
yycrank+87,	0,		0,	
yycrank+0,	0,		yyvstop+98,
yycrank+0,	0,		yyvstop+100,
yycrank+0,	0,		yyvstop+102,
yycrank+0,	0,		yyvstop+104,
yycrank+0,	0,		yyvstop+106,
yycrank+158,	0,		0,	
yycrank+0,	0,		yyvstop+108,
yycrank+105,	0,		0,	
yycrank+108,	0,		0,	
yycrank+164,	0,		0,	
yycrank+95,	0,		0,	
yycrank+0,	0,		yyvstop+110,
yycrank+127,	0,		0,	
yycrank+0,	0,		yyvstop+112,
yycrank+149,	0,		0,	
yycrank+148,	0,		0,	
yycrank+141,	0,		0,	
yycrank+149,	0,		0,	
yycrank+0,	0,		yyvstop+114,
yycrank+130,	0,		0,	
yycrank+0,	0,		yyvstop+116,
yycrank+186,	0,		0,	
yycrank+0,	0,		yyvstop+118,
yycrank+158,	0,		0,	
yycrank+151,	0,		0,	
yycrank+164,	0,		0,	
yycrank+0,	0,		yyvstop+120,
yycrank+0,	0,		yyvstop+122,
yycrank+194,	0,		0,	
yycrank+0,	0,		yyvstop+124,
yycrank+157,	0,		0,	
yycrank+0,	0,		yyvstop+126,
yycrank+0,	0,		yyvstop+128,
yycrank+0,	0,		yyvstop+130,
yycrank+0,	0,		yyvstop+132,
0,	0,	0};
struct yywork *yytop = yycrank+364;
struct yysvf *yybgin = yysvec+1;
char yymatch[] ={
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,'"' ,01  ,01  ,01  ,01  ,047 ,
'(' ,01  ,01  ,'+' ,'(' ,'+' ,01  ,'(' ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,01  ,01  ,'(' ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,'a' ,'a' ,'a' ,'d' ,'d' ,'a' ,'a' ,
'h' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,
'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,'a' ,
'a' ,'a' ,'a' ,01  ,01  ,01  ,01  ,01  ,
0};
char yyextra[] ={
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
1,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank){		/* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
