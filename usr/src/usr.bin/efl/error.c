#include "defs"

char *linerr()
{
static char buff[50];
register int i;

for(i = filedepth; i>0 && filenames[i]==NULL ; --i)
	;
if(i > 0)
	sprintf(buff, "on line %d of file %s", yylineno, filenames[i]);
else
	sprintf(buff, "on line %d", yylineno);
return(buff);
}

#ifdef pdp11
/*
** Print error messages.
** Save memory by looking them up in an external file.
** This saves about 2000 data bytes.
*/

/* Save more memory by overlaying the file name, which is only used
** once, with the message buffer.  The memory constaints on a 64K
** system are truly draconian.
*/
#define MAXMESSAGE 128
static char efilename[MAXMESSAGE] = "/usr/share/efl/efl_errorstrings";
#define buff efilename
static int efil = -1;

void error(location, message, type)
int location;
char *message;
int type;
{
	extern int nerrs;

	if (efil < 0) {			/* open the file, if reuired */
		efil = open(efilename, 0);
		if (efil < 0) {		/* if the file open fails exit */
oops:
			perror(efilename);
			exit(1);
		}
	}
	/* find the message string in the file */
	if (lseek(efil, (long)location, 0) < 0 ||
	    read(efil, buff, MAXMESSAGE) <= 0)
		goto oops;		/* if we can't find the message */
	switch (type) {
	case DCLERR:
		fprintf(diagfile, "**Error %s: Declaration for %s: %s\n",
		  linerr(), buff, message);
		++nerrs;
		break;
	case LABERR:
		errmess("Label error", buff, message);
		break;
	case EXPERR:
		errmess("Expression error", buff, message);
		break;
	case EXECERR:
		errmess("Error", buff, message);
		break;
	default:
		errmess("Internal error", "", NULL);
	}
}

#else
laberr(s,t)
char *s;
char *t;
{
errmess("Label error", s, t);
}





exprerr(s,t)
char *s;
ptr t;
{
errmess("Expression error", s, t);
}




execerr(s,t)
char *s, *t;
{
errmess("Error", s, t);
}

#endif

errmess(m,s,t)
char *m, *s, *t;
{
fprintf(diagfile, "**%s %s:  ", m, linerr());
if(s)
	fprintf(diagfile, s, t);
fprintf(diagfile, "\n");
++nerrs;
}



#ifndef pdp11
dclerr(s, n)
char *s, *n;
{
extern int nerrs;

fprintf(diagfile, "**Error %s: Declaration for %s: %s\n",
		linerr(), n, s);
++nerrs;
}
#endif




badtag(routine, tag)
char *routine;
int tag;
{
char buff[100];
sprintf(buff, "impossible tag %d in routine %s", tag, routine);
fatal(buff);
}



fatal1(s,t)
char *s;
int t;
{

sprintf(msg, s, t);
fatal(msg);
}



fatal(s)
char *s;
{
fprintf(diagfile, "\n***Compiler error %s.", linerr());
if(s) fprintf(diagfile, "   %s.", s);
fprintf(diagfile, "\n");
fflush(stdout);

if(dumpcore)
	abort(0);
else	{
	rmiis();
	exit(-1);
	}
}



warn1(s,t)
char *s;
int t;
{
sprintf(msg, s, t);
warn(msg);
}




warn(s)
char *s;
{
++nwarns;
if( !nowarnflag)
	fprintf(diagfile, "*Warning: %s\n", s);
}



yyerror(s)
char *s;
{
errmess(s, CNULL, CNULL);
}
