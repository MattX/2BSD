/* $Header: defs.h,v 1.7 83/11/01 20:44:32 tony Exp $
 *
 * $Log:	defs.h,v $
 * Revision 1.7  83/11/01  20:44:32  tony
 * Added crl`s latest changes (11/1/83)
 * 
 * Revision 1.6  83/10/24  14:39:03  tony
 * Removed CO_CMD and RM_CMD since this is now done via a rule.
 * 
 * Revision 1.5  83/10/11  12:56:07  tony
 * Added definition of standard remove command. (RM_CMD)
 * 
 * Revision 1.4  83/10/07  08:36:31  tony
 * Changed name of macro to determine dir.h location.
 * 
 * Revision 1.3  83/10/06  16:11:50  tony
 * Changed declarations for conversion to new directory
 * routines.
 * 
 * Revision 1.2  83/10/06  11:02:25  tony
 * Added code from physics for RCS.
 * 
 */

/*	@(#)/usr/src/cmd/make/defs	3.2	*/

/*	@(#)defs	3.1	*/

#include <sys/types.h>
#include <stdio.h>

#ifdef	BSD42
#	include <sys/dir.h>
#else
#	include <ndir.h>
#endif

#define NEWLINE	'\n'
#define CNULL	'\0'
#define BLANK	' '
#define TAB	'\t'
#define DOT	'.'
#define WIGGLE	'~'
#define AT	'@'
#define MINUS	'-'
#define EQUALS	'='
#define SLASH	'/'
#define STAR	'*'
#define LCURLY	'{'
#define RCURLY	'}'
#define LPAREN	'('
#define RPAREN	')'
#define LSQUAR	'['
#define RSQUAR	']'
#define QUESTN	'?'
#define KOLON	':'
#define SKOLON	';'
#define DOLLAR	'$'
#define GREATR	'>'
#define POUND	'#'
#define BACKSLASH	'\\'


/*
 *	Flags
 */

#define ALLOC(x) (struct x *) intalloc(sizeof(struct x))


#define TURNON(a)	(Mflags |= (a))
#define TURNOFF(a)	(Mflags &= (~(a)))
#define IS_ON(a)	(Mflags&(a))
#define IS_OFF(a)	(!(IS_ON(a)))

#define DBUG	0000001		/* debug flag */
#define ENVOVER	0000002		/* environ overides file defines */
#define EXPORT	0000004		/* put current variable in environ */
#define PRTR	0000010		/* set `-p' flag */
#define SIL	0000020		/* set `-s' flag */
#define NOEX	0000040		/* set `-n' flag */
#define INTRULE	0000100		/* use internal rules */
#define TOUCH	0000200		/* set `-t' flag */
#define GET	0000400		/* do a $(GET) if file not found */
#define QUEST	0001000		/* set `-q' flag */
#define INARGS	0002000		/* currently reading cmd args */
#define IGNERR	0004000		/* set `-i' flag */
#define KEEPGO	0010000		/* set `-k' flag */
#define GF_KEEP	0020000		/* keep auto get files */
#define MH_DEP	0040000		/* use old question about whether cmd exists */
#define MEMMAP	0100000		/* print memory map */
#define	COFLAG	0200000		/* check out RCS files as necessary */
#define	RMFLAG	0400000		/* remove any checked out files when done */

typedef char *CHARSTAR;
typedef int  *INTSTAR;
typedef long int TIMETYPE;
typedef struct gothead *GOTHEAD;
typedef struct gotf *GOTF;
typedef struct chain *CHAIN;
typedef struct opendir *OPENDIR;
typedef struct pattern *PATTERN;
typedef struct varblock *VARBLOCK;
typedef struct shblock *SHBLOCK;
typedef struct depblock *DEPBLOCK;
typedef struct lineblock *LINEBLOCK;
typedef struct nameblock *NAMEBLOCK;

extern CHARSTAR *environ;
extern long Mflags;
extern FILE *fin;
extern CHARSTAR *linesptr;
/*
 *	flags for get()
 */
#define CD	0
#define NOCD	1


#define max(a,b)	((a)>(b)?(a):(b))
#define SHELLCOM "/bin/sh"

#define	RCS	"RCS"			/* RCS files are in this dir or . */
#define	RCS_SUF	",v"			/* suffix of RCS files */

extern	char	*RCSdir;
extern	struct	shblock	*co_cmd;
extern	char	*RCSsuf;		/* suffix of RCS files */
extern	int	dotRCS;			/* true if ./RCS exists */

#ifdef unix
/*  to install metering, add a statement like */
/*******
#define METERFILE "/usr/sif/make/Meter"
******/
/* to turn metering on, set external variable meteron to 1 */
#endif

/* define FSTATIC to be static on systems with C compilers
   supporting file-static; otherwise define it to be null
*/
#define FSTATIC static

#define NO 0
#define YES 1

#define equal(a,b)	(a[0] == b[0] ? !strcmp((a),(b)) : NO )
#define HASHSIZE 809
#define NLEFTS 100
#define INMAX 2500
#define OUTMAX 2500
#define	MAXDIR	10

#define ALLDEPS  1
#define SOMEDEPS 2

#define META 01
#define TERMINAL 02
extern char funny[128];



extern int sigivalue;
extern int sigqvalue;
extern int sighvalue;
extern int sigtvalue;
extern int waitpid;
extern int ndocoms;
extern int okdel;
extern CHARSTAR prompt;
extern char junkname[ ];
extern char RELEASE[];

extern	int	nopdir;


struct nameblock
{
	NAMEBLOCK nextname;		/* pointer to next nameblock */
	NAMEBLOCK backname;		/* pointer to predecessor */
	CHARSTAR namep;		/* ASCII name string */
	CHARSTAR alias;		/* ASCII alias (when namep translates to another
				 * pathstring.
				 */
	LINEBLOCK linep; /* pointer to dependents */
	int done:3;		/* flag used to tell when finished */
	int septype:3;		/* distinguishes between single and double : */
	int rundep:1;		/* flag indicating runtime translation done */
	char *RCSnamep;		/* name of RCS file, if needed */
	TIMETYPE modtime;	/* set by exists() */
};


extern NAMEBLOCK mainname ;
extern NAMEBLOCK firstname;

struct lineblock
{
	LINEBLOCK nextline;
	DEPBLOCK depp;
	SHBLOCK shp;
};

extern LINEBLOCK sufflist;

struct depblock
{
	DEPBLOCK nextdep;
	NAMEBLOCK depname;
};

struct shblock
{
	SHBLOCK nextsh;
	CHARSTAR shbp;
};

struct varblock
{
	VARBLOCK nextvar;
	CHARSTAR varname;
	CHARSTAR varval;
	int noreset:1;
	int used:1;
	int envflg:1;
	int v_aflg:1;
};

extern VARBLOCK firstvar;

struct pattern
{
	PATTERN nextpattern;
	CHARSTAR patval;
};

extern PATTERN firstpat;

struct	opendir
{
	OPENDIR nextopendir;
	DIR	* dirfc;
	CHARSTAR dirn;
};
extern OPENDIR firstod;


struct chain
{
	CHAIN nextchain;
	CHARSTAR datap;
};

extern	struct	chain	*rmchain;


/*
 *	The following two structures are used to cleanup after
 *	`make' does an automatic get of a file. See get() and
 *	cleanup().
 */
struct gotf			/* list of files */
{
	GOTF gnextp;
	CHARSTAR gnamep;
};

struct gothead			/* first member of list of files */
{
	GOTF gnextp;
	CHARSTAR gnamep;
	GOTF endp;
};

char Nullstr[];
CHARSTAR badptr;
CHARSTAR copys();
CHARSTAR copstr();
CHARSTAR concat();
CHARSTAR colontrans();
CHARSTAR dftrans();
CHARSTAR straightrans();
CHARSTAR mkqlist();
CHARSTAR findfl();
CHARSTAR addstars();
CHARSTAR strshift();
INTSTAR  intalloc();
VARBLOCK varptr();
VARBLOCK srchvar();
TIMETYPE exists();
DEPBLOCK srchdir();
NAMEBLOCK srchname(), makename();
LINEBLOCK runtime();
TIMETYPE getrcs();
char	*rindex();
char	*ncat();
