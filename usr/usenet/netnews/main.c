/*
 *	NEWS program - version 1.2.
 *
 *	June 6, 1980
 *
 *	by Stephen Daniel (duke!swd)
 *	and Tom Truscott  (duke!trt).
 *
 */

#include "params.h"
#include <whoami.h>
static char *sccsid = "@(#)main.c	1.4	12/3/80";

/*
 * Main deciphers the argument list.
 */

/* local defines for main */

#define OPTION	0	/* pick up an option string */
#define STRING	1	/* pick up a string of arguments */

#define UNKNOWN 0001	/* possible modes for news program */
#define READ	0002
#define INPUT	0004
#define UIN	0010
#define SUBSCR	0020
#define BIT	0040

#ifndef SYSBUF
char SYSBUF[BUFSIZ];	/* to buffer std out */
#endif

struct {			/* options table. */
	char	optlet;		/* option character. */
	char	filchar;	/* if to pickup string, fill character. */
	int	flag;		/* TRUE if have seen this opt. */
	int	newstate;	/* STRING if takes arg, else OPTION */
	int	oldmode;	/* OR of legal input modes. */
	int	newmode;	/* output mode. */
	char	*buf;		/* string buffer */
} *optpt, options[] = { /*
optlet	filchar	flag	newstate oldmode	newmode	buf	*/
'l',	'\0',	FALSE,	OPTION,	READ,		READ,	(char *)NULL,	
'p',	'\0',	FALSE,	OPTION,	READ,		READ,	(char *)NULL,	
'r',	'\0',	FALSE,	OPTION,	READ,		READ,	(char *)NULL,	
't',	'\0',	FALSE,	STRING,	READ,		READ,	&titlebuf[0],	
'i',	' ',	FALSE,	STRING,	INPUT,		INPUT,	&header.title[0],
'a',	' ',	FALSE,	STRING,	READ,		READ,	&datebuf[0],
'n',   NGDELIM,	FALSE,	STRING,	READ|INPUT,	READ,	&header.nbuf[0],
's',   NGDELIM,	FALSE,	STRING,	SUBSCR,		SUBSCR,	&header.nbuf[0],
'b',	'\0',	FALSE,	OPTION,	UNKNOWN,	BIT,	(char *)NULL,	
'c',	' ',	FALSE,	STRING,	READ,		READ,	&coptbuf[0],	
'f',	' ',	FALSE,	STRING,	INPUT,		INPUT,	&username[0],
'\0',	'\0',	0,	0,	0,		0,	(char *)NULL
};

main(argc, argv)
int argc;
register char **argv;
{
	int state;		/* which type of argument to pick up */
	register int mode;	/* mode of news program */
	int tlen;		/* temp for string processing routine */
	int len;		/* temp for string processing routine */
	register char *ptr;	/* pointer to rest of buffer */
	int filchar;		/* fill character (state = STRING) */
	struct passwd *pw;
	struct group *gp;

	/* set up defaults and initialize. */

	mode = UNKNOWN;
	ptr = rindex(argv[0],'/');
	if (ptr)
		argv[0] = ++ptr;
	if (argv[0][0] == 'r')
		mode = UIN;	/* will make all options illegal. */
	state = OPTION;
	bitup = FALSE;		/* true if bitmap should be re-made */
	header.title[0] = header.nbuf[0] = '\0';
	titlebuf[0] = coptbuf[0] = datebuf[0] = '\0';
	strncpy(SYSNAME, sysname, SNLN);
	SYSNAME[SNLN] = '\0';

	setbuf(stdout, SYSBUF);
	sigtrap = FALSE;	/* true if a signal has been caught */
	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP, onsig);
	signal(SIGINT, onsig);
	savmask = umask(N_UMASK);	/* set up mask */
#ifdef PWB
	if ((uid = (unsigned) (getuid() & 0377)) >= (unsigned) USIZE)
		xerror("login uid too big");
	gid = (unsigned) (getgid() & 0377);
#else

	if ((uid = (unsigned) getuid()) >= (unsigned) USIZE)
		xerror("login uid too big");
	gid = (unsigned) getgid();
#endif
	duid = 0;
	dgid = 0;
	if (uid == ROOTID && geteuid() == ROOTID) {
		/*
		 * Must go through with this kludge since
		 * some systems do not honor the setuid bit
		 * when root invokes a setuid program.
		 */
		if ((pw = getpwnam(NEWSU)) == NULL)
			xerror("Cannot get NEWSU pw entry");
		duid = pw->pw_uid;
		if ((gp = getgrnam(NEWSG)) == NULL)
			xerror("Cannot get NEWSG gr entry");
		dgid = gp->gr_gid;
	}

	/* loop once per arg. */

	++argv;		/* skip first arg, which is prog name. */

	while (--argc) {
	    if (state == OPTION) {
		if (**argv != '-') {
			sprintf(bfr, "Bad option string \"%s\"", *argv);
			xerror(bfr);
		}
		while (*++*argv != '\0') {
			for (optpt = options; optpt->optlet != '\0'; ++optpt) {
				if (optpt->optlet == **argv)
					goto found;
			}
			/* unknown option letter */
			sprintf(bfr, "Bad option '%c'", **argv);
			xerror(bfr);

		    found:;
			if (optpt->flag == TRUE || (mode != UNKNOWN &&
			    (mode&optpt->oldmode) == 0)) {
				sprintf(bfr, "Bad %c option", **argv);
				xerror(bfr);
			}
			if (mode == UNKNOWN)
				mode = optpt->newmode;
			filchar = optpt->filchar;
			optpt->flag = TRUE;
			state = optpt->newstate;
			ptr = optpt->buf;
			len = BUFLEN;
		}

		argv++;		/* done with this option arg. */

	    } else {

		/*
		 * Pick up a piece of a string and put it into
		 * the appropriate buffer.
		 */
		if (**argv == '-') {
			state = OPTION;
			argc++;	/* uncount this arg. */
			continue;
		}

		if ((tlen = strlen(*argv)) >= len)
			xerror("Argument string too long");
		strcpy(ptr, *argv++);
		ptr += tlen;
		if (*(ptr-1) != filchar)
			*ptr++ = filchar;
		len -= tlen + 1;
		*ptr = '\0';
	    }
	}

	/*
	 * ALL of the command line has now been processed. (!)
	 */
	switch (mode) {

		case UNKNOWN:
		case READ:
			readr(options[3].flag, options[0].flag,
				options[1].flag, options[2].flag,
				options[5].flag, options[6].flag,
				options[9].flag);
			break;

		case INPUT:
			iopt(options[6].flag);
			break;

		case UIN:
			uneti();
			break;

		case BIT:
			getdtln();
			users(header.ndate);
			bitup = TRUE;
			break;

		case SUBSCR:
			subscr();
			break;

		default:
			xerror("Bad mode");
			break;
	}

	if (bitup)
		fsubr(makebits, 0, 0);

	xxit(0);
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 *
 * These are the v7 index and rindex routines, stolen for portability.
 * (Some Unix systems call them strchr and strrchr, notably PWB 2.0
 * and its derivitives such as Unix/TS 2.0, Unix 3.0, etc.)  Others,
 * like v6, don't have them at all.
 */

char *
index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
	return(NULL);
}

/*
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
 */

#define NULL 0

char *
rindex(sp, c)
register char *sp, c;
{
	register char *r;

	r = NULL;
	do {
		if (*sp == c)
			r = sp;
	} while (*sp++);
	return(r);
}
