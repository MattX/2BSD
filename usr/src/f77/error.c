#include "defs"
#include "string_defs"
char efilname[] = "/lib/f77_strings";
int  efil = -1;
error(index,t,u,type)
int index;
char *t, *u;
register int type;
{
	char buf[100];
	long lseek();

	if (efil < 0) {
		efil = open(efilname, 0);
		if (efil < 0) {
oops:
			perror(efilname);
			exit(1);
		}
	}
	if (lseek(efil, (long) index, 0) < 0 || read(efil, buf, 100) <= 0)
		goto oops;
	switch (type) {
	    case WARN1:
	    case WARN:
		if(nowarnflag)
			return;
		fprintf(diagfile, "Warning on line %d of %s: ",
			lineno, infname);
		++nwarn;
		break;

	    case YYERR:
	    case ERR2:
	    case ERR1:
	    case ERR:
		fprintf(diagfile, "Error on line %d of %s: ",
			lineno, infname);
		++nerr;
		break;

	    case FATAL1:
	    case FATAL:
		fprintf(diagfile,"f77 compiler error line %d of %s: ",
			lineno, infname);
		if(debugflag)
			abort();
		done(3);
		exit(3);
	    case EXECERR:
		fprintf(diagfile, "Error on line %d of %s: ",
			lineno, infname);
		fprintf(diagfile,"Execution error %s");
		++nerr;
		break;
	    default:
		fprintf(diagfile,"unrecognizable error switch\n");
		exit(1);
	}
	fprintf(diagfile,buf,t,u);
	fprintf(diagfile,"\n");
}

dclerr(s, v)
char *s;
struct nameblock *v;
{

if(v)
	error("Declaration error for %s: %s", varstr(VL, v->varname), s,ERR2);
else
	error("Declaration error %s", s,0,ERR1);
}
