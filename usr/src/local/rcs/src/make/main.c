static	char	*rcsid =
	"$Header: main.c,v 1.8 83/11/21 20:06:06 tony Exp $";

/*
 * $Log:	main.c,v $
 * Revision 1.8  83/11/21  20:06:06  tony
 * Added code in the interrupt routine to make sure that the file
 * to be removed is a regular file. This has to be done because
 * root is allowed to unlink directories, which can have disasterous
 * consequences.
 * 
 * Revision 1.7  83/11/03  11:13:57  crl
 * More fixes from Charlie...
 * 
 * Revision 1.7  83/11/03  11:13:57  crl
 * Fixed bug in finding co_cmd due to a transcription error.
 * Half of the printf's under the -p option went to stderr, the rest to stdout,
 * 	so I made them all go to stdout.
 * 
 * Revision 1.6  83/11/01  20:48:54  tony
 * Added crl`s latest changes (11/1/83)
 * 
 * Revision 1.5  83/10/24  14:36:00  tony
 * Initialize co_cmd from the ".CO" target and turn off COFLAG
 * if null.
 * 
 * Revision 1.4  83/10/06  16:16:59  tony
 * Made the default shell (/bin/sh) take precedence over the
 * environment. This was done because so many people use csh
 * as a login shell but use sh in shell scripts and makefiles.
 * The SHELL variable can still be changed inside the makefile.
 * 
 * Revision 1.3  83/10/06  16:09:43  tony
 * Converted to use new directory routines.
 * 
 * Revision 1.2  83/10/06  10:51:55  tony
 * Added code from physics for RCS.
 * 
 */

/*	@(#)/usr/src/cmd/make/main.c	3.4	*/
#include "defs.h"
/*
command make to update programs.
Flags:	'd'  print out debugging comments
	'p'  print out a version of the input graph
	's'  silent mode--don't print out commands
	'f'  the next argument is the name of the description file;
	     makefile is the default
	'i'  ignore error codes from the shell
	'S'  stop after any command fails (normally do parallel work)
	'n'   don't issue, just print, commands
	't'   touch (update time of) files but don't issue command
	'q'   don't do anything, but check if object is up to date;
	      returns exit code 0 if up to date, -1 if not

	'c'  don`t check out files
	'C'  check out RCS files as needed (default)
	'u'  don`t remove checked out files when done
	'U'  remove checked out files (default)
*/

char makefile[] = "makefile";
char Nullstr[]  = "";
char Makefile[] = "Makefile";
char RELEASE[]  = "RELEASE";
CHARSTAR badptr = (CHARSTAR)-1;

NAMEBLOCK mainname ;
NAMEBLOCK firstname;
LINEBLOCK sufflist;
VARBLOCK firstvar;
PATTERN firstpat ;
OPENDIR firstod;
CHAIN	rmchain = NULL;
struct	shblock *co_cmd = NULL;

#include <signal.h>
int sigivalue=0;
int sigqvalue=0;
int sighvalue=0;
int sigtvalue=0;
int waitpid=0;

long Mflags = MH_DEP | COFLAG | RMFLAG;
int  ndocoms=0;
int  okdel=YES;

CHARSTAR prompt="\t";	/* other systems -- pick what you want */
char junkname[20];
char funny[128];

int	nopdir = 0;

#include <sys/stat.h>
char	*RCSdir = RCS;
char	*RCSsuf = RCS_SUF;
int	dotRCS  = NO;


char Makeflags[]="MAKEFLAGS";

main(argc,argv)
int argc;
CHARSTAR argv[];
{
	register NAMEBLOCK p;
	int i, j;
	int descset, nfargs;
	TIMETYPE tjunk;
	char c;
	CHARSTAR s, *e;
	struct	lineblock *lp;
	struct	stat	sbuf;

	struct	chain	*ch;

#ifdef unix
	int intrupt();
#endif

#ifdef METERFILE
	meter(METERFILE);
#endif

	descset = 0;

	for(s = "|=^();&<>*?[]:$`'\"\\\n" ; *s ; ++s)
		funny[*s] |= META;
	for(s = "\n\t :=;{}&>|" ; *s ; ++s)
		funny[*s] |= TERMINAL;
	funny['\0'] |= TERMINAL;

	TURNON(INTRULE);		/* Default internal rules, turned on */

/*
 *	Set command line flags
 */

	getmflgs();				/* Init $(MAKEFLAGS) variable */
	setflags(argc, argv);

	setvar("$","$");


/*
 *	Read command line "=" type args and make them readonly.
 */
	TURNON(INARGS|EXPORT);
	if(IS_ON(DBUG))printf("Reading \"=\" type args on command line.\n");
	for(i=1; i<argc; ++i)
		if(argv[i]!=0 && argv[i][0]!=MINUS && (eqsign(argv[i]) == YES) )
			argv[i] = 0;
	TURNOFF(INARGS|EXPORT);

/*
 *	Read internal definitions and rules.
 */

	if( IS_ON(INTRULE) )
	{
		if(IS_ON(DBUG))printf("Reading internal rules.\n");
		rdd1(NULL);
	}

/*
 *	Done with internal rules, now.
 */
	TURNOFF(INTRULE);

/*
 *	Read environment args.  Let file args which follow override.
 *	unless 'e' in MAKEFLAGS variable is set.
 */
	if( any( (varptr(Makeflags))->varval, 'e') )
		TURNON(ENVOVER);
	if(IS_ON(DBUG))printf("Reading environment.\n");
	TURNON(EXPORT);
	readenv();
	TURNOFF(EXPORT|ENVOVER);

/****	BEWARE - the SHELL is handled as a special case ****/

	setvar("SHELL","/bin/sh");	/* default shell overrides env. */

/*
 *	Read command line "-f" arguments.
 */

	rdmakecomm();

	for(i = 1; i < argc; i++)
		if( argv[i] && argv[i][0] == MINUS && argv[i][1] == 'f' && argv[i][2] == CNULL)
		{
			argv[i] = 0;
			if(i >= argc-1)
				fatal("No description argument after -f flag");
			if( rddescf(argv[++i], YES) )
				fatal1("Cannot open %s", argv[i]);
			argv[i] = 0;
			++descset;
		}


/*
 *	If no command line "-f" args then look for some form of "makefile"
 */
	if( !descset )
#ifdef unix
		if( rddescf(makefile, NO))
		if( rddescf(Makefile, NO))
		if( rddescf(makefile, YES))
			rddescf(Makefile, YES);

#endif
#ifdef gcos
		rddescf(makefile, NO);
#endif


	if(IS_ON(PRTR)) printdesc(NO);

	if (p = srchname(".CO")) {
		for (lp = p->linep; lp ; lp = lp->nextline)
			if ((co_cmd = lp->shp) != NULL)
				break;
		if (co_cmd == NULL)
			TURNOFF(COFLAG);
	} else
		TURNOFF(COFLAG);

	if( srchname(".IGNORE") ) TURNON(IGNERR);
	if( srchname(".SILENT") ) TURNON(SIL);
	if(p=srchname(".SUFFIXES")) sufflist = p->linep;
	if( !sufflist )
		printf("No suffix list.\n");

#ifdef unix
	sigivalue = (int)signal(SIGINT,SIG_IGN) == SIG_IGN;
	sigqvalue = (int)signal(SIGQUIT,SIG_IGN) == SIG_IGN;
	sighvalue = (int)signal(SIGHUP,SIG_IGN) == SIG_IGN;
	sigtvalue = (int)signal(SIGTERM,SIG_IGN) == SIG_IGN;
	enbint(intrupt);
#endif

	if(stat(RCSdir,&sbuf) == 0 && (sbuf.st_mode & S_IFMT) == S_IFDIR)
		dotRCS = YES;	/* fast check for ./RCS */


	nfargs = 0;

	for(i=1; i<argc; ++i)
		if((s=argv[i]) != 0)
		{
			if((p=srchname(s)) == 0)
			{
				p = makename(s);
			}
			++nfargs;
			ch = NULL;
			doname(p, 0, &tjunk,&ch);
			if(IS_ON(DBUG)) printdesc(YES);
		}

/*
	If no file arguments have been encountered, make the first
	name encountered that doesn't start with a dot
	*/

	if(nfargs == 0)
		if(mainname == 0)
			fatal("No arguments or description file");
		else
		{
			ch = NULL;
			doname(mainname, 0, &tjunk,&ch);
			if(IS_ON(DBUG)) printdesc(YES);
		}

	exit(0);
}



#ifdef unix
intrupt()
{
	CHARSTAR p;
	struct	stat sb;

	if(okdel && IS_OFF(NOEX) && IS_OFF(TOUCH) &&
		(p = varptr("@")->varval) && exists(srchname(p))>0 &&
		!isprecious(p) )
	{
		if(getuid() != 0) {
			if(unlink(p) == 0)
				printf("\n***  %s removed.", p);
		}
		else {
			if((stat(p,&sb) == 0) && (sb.st_mode&S_IFMT == S_IFREG))
				if(unlink(p) == 0)
					printf("\n***  %s removed.", p);
		}
	}

	if(junkname[0])
		unlink(junkname);
	printf("\n");
	exit(2);
}




isprecious(p)
CHARSTAR p;
{
	register NAMEBLOCK np;
	register LINEBLOCK lp;
	register DEPBLOCK dp;

	if(np = srchname(".PRECIOUS"))
	    for(lp = np->linep ; lp ; lp = lp->nextline)
		for(dp = lp->depp ; dp; dp = dp->nextdep)
			if(equal(p, dp->depname->namep))
				return(YES);

	return(NO);
}


enbint(k)
int (*k)();
{
	if(sigivalue == 0)
		signal(SIGINT,k);
	if(sigqvalue == 0)
		signal(SIGQUIT,k);
	if(sighvalue == 0)
		signal(SIGHUP,k);
	if(sigtvalue == 0)
		signal(SIGTERM,k);
}
#endif

extern CHARSTAR builtin[];

CHARSTAR *linesptr=builtin;

FILE * fin;
int firstrd=0;

rdmakecomm()
{
#ifdef PWB
	register char *nlog;
	char s[128];

	if(rddescf( concat((nlog=getenv("HOME")),"/makecomm",s), NO))
		rddescf( concat(nlog,"/Makecomm",s), NO);

	if(rddescf("makecomm", NO))
		rddescf("Makecomm", NO);
#endif
}

extern int yylineno;
extern CHARSTAR zznextc;
rddescf(descfile, flg)
CHARSTAR descfile;
int flg;			/* if YES try s.descfile */
{
	FILE * k;

/* read and parse description */

	if(equal(descfile, "-"))
		return( rdd1(stdin) );

retry:
	if( (k = fopen(descfile,"r")) != NULL)
	{
		if(IS_ON(DBUG))printf("Reading %s\n", descfile);
		return( rdd1(k) );
	}

	if(flg == NO)
		return(1);
	if(get(descfile, CD, varptr(RELEASE)->varval) == NO)
		return(1);
	flg = NO;
	goto retry;

}




rdd1(k)
FILE * k;
{
	fin = k;
	yylineno = 0;
	zznextc = 0;

	if( yyparse() )
		fatal("Description file error");

	if(fin != NULL)
		fclose(fin);

	return(0);
}

printdesc(prntflag)
int prntflag;
{
	NAMEBLOCK p;
	DEPBLOCK dp;
	VARBLOCK vp;
	OPENDIR od;
	SHBLOCK sp;
	LINEBLOCK lp;

#ifdef unix
	if(prntflag)
	{
		printf("Open directories:\n");
		for(od=firstod; od!=0; od = od->nextopendir)
			printf("\t%s\n", od->dirn);
	}
#endif

	if(firstvar != 0) printf("Macros:\n");
	for(vp=firstvar; vp!=0; vp=vp->nextvar)
		if(vp->v_aflg == NO)
			printf("\t%s = %s\n" , vp->varname , vp->varval);
		else
		{
			CHAIN pch;

			printf("Lookup chain: %s\n\t", vp->varname);
			for(pch = (CHAIN)vp->varval; pch; pch = pch->nextchain)
				printf(" %s", ((NAMEBLOCK)pch->datap)->namep);
			printf("\n");
		}

	for(p=firstname; p!=0; p = p->nextname)
		prname(p, prntflag);
	printf("\n");
	fflush(stdout);
}

prname(p, prntflag)
register NAMEBLOCK p;
{
	register LINEBLOCK lp;
	register DEPBLOCK dp;
	register SHBLOCK sp;

	if(p->linep != 0)
		printf("\n\n%s:",p->namep);
	else
		printf("\n\n%s", p->namep);
	if(prntflag)
	{
		printf("  done=%d",p->done);
	}
	if(p==mainname)
		printf("  (MAIN NAME)");
	for(lp = p->linep ; lp!=0 ; lp = lp->nextline)
	{
		if( dp = lp->depp )
		{
			printf("\n depends on:");
			for(; dp!=0 ; dp = dp->nextdep)
				if(dp->depname != 0)
				{
					printf(" %s", dp->depname->namep);
					printf(" ");
				}
		}
		if(sp = lp->shp)
		{
			printf("\n");
			printf(" commands:\n");
			for( ; sp!=0 ; sp = sp->nextsh)
				printf("\t%s\n", sp->shbp);
		}
	}
}


setflags(ac, av)
int ac;
CHARSTAR *av;
{
	register int i, j;
	register char c;
	int flflg=0;			/* flag to note `-f' option. */

	for(i=1; i<ac; ++i)
	{
		if(flflg)
		{
			flflg = 0;
			continue;
		}
		if(av[i]!=0 && av[i][0]==MINUS)
		{
			if(any(av[i], 'f'))
				flflg++;
			for(j=1 ; (c=av[i][j])!=CNULL ; ++j)
				optswitch(c);
			if(flflg)
				av[i] = "-f";
			else
				av[i] = 0;
		}
	}
}


/*
 *	Handle a single char option.
 */
optswitch(c)
register char c;
{

	switch(c)
	{

	case 'e':	/* environment override flag */
		setmflgs(c);
		break;

	case 'd':	/* debug flag */
		TURNON(DBUG);
		setmflgs(c);
		break;

	case 'p':	/* print description */
		TURNON(PRTR);
		break;

	case 's':	/* silent flag */
		TURNON(SIL);
		setmflgs(c);
		break;

	case 'i':	/* ignore errors */
		TURNON(IGNERR);
		setmflgs(c);
		break;

	case 'S':
		TURNOFF(KEEPGO);
		setmflgs(c);
		break;

	case 'k':
		TURNON(KEEPGO);
		setmflgs(c);
		break;

	case 'n':	/* do not exec any commands, just print */
		TURNON(NOEX);
		setmflgs(c);
		break;

	case 'r':	/* turn off internal rules */
		TURNOFF(INTRULE);
		break;

	case 't':	/* touch flag */
		TURNON(TOUCH);
		setmflgs(c);
		break;

	case 'q':	/* question flag */
		TURNON(QUEST);
		setmflgs(c);
		break;

	case 'g':	/* turn default $(GET) of files not found */
		TURNON(GET);
		setmflgs(c);
		break;

	case 'm':	/* print memory map */
		TURNON(MEMMAP);
		setmflgs(c);
		break;

	case 'b':	/* use MH version of test for whether a cmd exists */
		TURNON(MH_DEP);
		setmflgs(c);
		break;
	case 'B':	/* turn off -b flag */
		TURNOFF(MH_DEP);
		setmflgs(c);
		break;
	case 'c':
		TURNOFF(COFLAG);
		setmflgs(c);
		break;
	case 'C':
		TURNON(COFLAG);
		setmflgs(c);
		break;
	case 'u':
		TURNOFF(RMFLAG);
		setmflgs(c);
		break;
	case 'U':
		TURNON(RMFLAG);
		setmflgs(c);
		break;

	case 'f':	/* Named makefile; already handled by setflags(). */
		break;

	default:
		fatal1("Unknown flag argument %c", c);
	}
}

/*
 *	getmflgs() set the cmd line flags into an EXPORTED variable
 *	for future invocations of make to read.
 */


getmflgs()
{
	register VARBLOCK vpr;
	register CHARSTAR *pe;
	register CHARSTAR p;

	vpr = varptr(Makeflags);
	setvar(Makeflags, "ZZZZZZZZZZZZZZZZ");
	vpr->varval[0] = CNULL;
	vpr->envflg = YES;
	vpr->noreset = YES;
	optswitch('b');
	for(pe = environ; *pe; pe++)
	{
		if(sindex(*pe, "MAKEFLAGS=") == 0)
		{
			for(p = (*pe)+sizeof Makeflags; *p; p++)
				optswitch(*p);
			return;
		}
	}
}

/*
 *	setmflgs(c) sets up the cmd line input flags for EXPORT.
 */

setmflgs(c)
register char c;
{
	register VARBLOCK vpr;
	register CHARSTAR p;

	vpr = varptr(Makeflags);
	for(p = vpr->varval; *p; p++)
	{
		if(*p == c)
			return;
	}
	*p++ = c;
	*p = CNULL;
}
