/* chall, chown, chgrp, chmod - change owner, group and/or modes of files.
 *
 * Written by Stephen Uitti, PUCC, Dec. 1984.
 * $Log:	chall.c,v $
 * Revision 1.2  85/04/25  17:16:58  ach
 * Fixed parsing bug where chall removed "delimiter" characters from
 * begining of file names (named on the command line) before processing.
 * These characters include ':', '=' and spaces.  Also, filenames that
 * begin with a dash, '-' are allowed, and the error message for a file
 * named with a dash has been upgraded to warn about possible syntax
 * problems. -sku
 * 
 * Revision 1.1	 85/04/09  19:35:11  ach
 * Initial revision
 *
 */
#ifndef lint
static char *RCSID =
"$Header: /usr/src/local/bin/RCS/chall.c,v 1.2 85/04/25 17:16:58 ach Exp $";
#endif	lint

#include <stdio.h>			/* for nothing */
#include <sys/param.h>			/* for MAXPATHLEN in 4.2 */
					/* in 2.9, loads types.h for void */
#include <sys/dir.h>			/* for MAXNAMLEN in 4.2 */
#include <sys/stat.h>			/* for what modes are */
#include <pwd.h>			/* for getpwnam, struct passwd */
#include <grp.h>			/* for getgrnam, struct group */
#include <ctype.h>			/* for isdigit() */
#include <local/cmd.h>			/* for argv parsing */

/* local defines */
#define TODEEP 15			/* recursion with open directories */
#define MAXMODS 20			/* max comma seperated modes */

/* standard (libc) functions, wish these were declared in <stdio.h> */
char   *index();			/* libc, find char in string */
char   *rindex();			/* libc, reverse index */
char   *strcpy();			/* stdio, string copy */

/* forward functions */
void	chall();			/* most of the work */
void	setown();			/* parse owner */
void	setgrp();			/* parse group */
void	setmod();			/* parse modes */
int	atoo();				/* ASCII octal to int */
int	isoctal();			/* is a character octal */
char   *skipoct();			/* find first non-octal */

/* globals */
char   *prgnm;				/* the progs called name */
int	called;				/* how called: values follow */
#define CHOWN 0
#define CHGRP 1
#define CHMOD 2
#define CHALL 3				/* none of the above */
int	fileseen = FALSE;		/* files yet processed */
int	quiet = FALSE;			/* -quiet */
int	recurse = FALSE;		/* -recurse */
int	owner = -1;			/* UID if not -1 */
int	group = -1;			/* GID if not -1 */
int	modset = 0;			/* # of modes specified */
int	modes[MAXMODS];			/* modes if modset */
int	modtyp[MAXMODS];		/* How to do modes: values follow */
#define MODSET 0			/* set them = to modes */
#define MODPLUS 1			/* or modes into what's there */
#define MODMINUS 2			/* and ~modes into what's there */
int	fromod[MAXMODS];		/* for u+go type formats */
int	tomod[MAXMODS];			/* User Group Other, All: */
#define NONE 0
#define USR 1
#define GRP 2
#define OTH 4
#define ALL (USR | GRP | OTH)
int	dironly[MAXMODS];		/* set modes only if directory */
int	fileonly[MAXMODS];		/* set modes only if non-directory */
int	umsk;				/* user's umask */
int	useumask[MAXMODS];		/* to use umsk or not */
int	err = FALSE;			/* any errors seen */
char   *usage;				/* brief synopis: strings follow: */
char   *ownusage =
"Usage: chown [-help] [-quiet] [-recurse] username or userid files...\n";
char   *grpusage =
"Usage: chgrp [-help] [-quiet] [-recurse] groupname or groupid files...\n";
char   *modusage =
"Usage: chmod [-help] [-quiet] [-recurse]\n\
\t[-modes=(mode # or [ugoadf]+-=[rwxstugo])] files...\n";
char   *allusage =
"Usage: chall [-help] [-quiet] [-recurse] [-owner=(username or user id]]\n\
[-group=(groupname or group id)] [-modes=(mode # or [ugoadf]+-=[rwxstugo])]\n\
files...\n";

char *clist[] = {
    "-Help\t Print usage and command list.  Exit.\n",
#define HELP 0
    "-Quiet\t Ignore file errors.\n",
#define QUIET 1
    "-Recurse Traverse named directory trees.\n",
#define RECURSE 2
/* The following options only apply if not called as chown, chgrp or chmod */
    "-Owner\t =User name or user ID: file ownership.\n",
#define OWNER 3
    "-Group\t =Group name or group ID: file group ownership.\n",
#define GROUP 4
    "-Modes\t =Mode number or [ugoadf]+-=[rwxstugo]: file protections.\n",
#define MODE 5
    NULL
};

/* init & argv parsing */
main(argc, argv)
register int argc;
register char **argv;
{
    register a;				/* argv subscript */

    /* prgnm = program name, set "called" & usage */
    if ((prgnm = rindex(argv[0], '/')) == NULL)
	prgnm = argv[0];
    else
	prgnm++;
    if (strcmp("chown", prgnm) == 0) {
	called = CHOWN;
	usage = ownusage;
    } else if (strcmp("chgrp", prgnm) == 0) {
	called = CHGRP;
	usage = grpusage;
    } else if (strcmp("chmod", prgnm) == 0) {
	called = CHMOD;
	usage = modusage;
    } else {
	called = CHALL;
	usage = allusage;
    }
    if (called != CHALL)
	clist[OWNER] = NULL;		/* only chall has the extra opts */
    if (argc <= 1) {			/* must tell it something */
	fprintf(stderr, "%s: Must have something to do.\n", prgnm);
	fputs(usage, stderr);
	exit(1);
    }
    umsk = umask(0);
    for (a = 1; a < argc; a++) {
	if (argv[a][0] != '-') {
	    if (called == CHOWN && owner == -1) {
		setown(argv[a]);
	    } else if (called == CHGRP && group == -1) {
		setgrp(argv[a]);
	    } else if (called == CHMOD && !modset) {
		setmod(argv[a]);
	    } else {
		chall(argv[a]);		/* must be a file or directory */
	    }
	} else switch (cmdprs(argv[a], clist)) {
	case HELP:
	    fputs(usage, stdout);
	    cmdhlp(argv[a], clist, stdout);
	    exit(0);			/* life's too tough to go on */
	case QUIET:
	    quiet = TRUE;		/* ignore file errors */
	    break;
	case RECURSE:
	    recurse = TRUE;
	    break;
	case OWNER:
	    setown(cmddlm(argv[a]));
	    break;
	case GROUP:
	    setgrp(cmddlm(argv[a]));
	    break;
	case MODE:
	    setmod(cmddlm(argv[a]));
	    break;
	case UNRECINP:			/* it's not a clist switch */
	    if (called == CHOWN && owner == -1) { /* try other argv stuff */
		setown(argv[a]);
	    } else if (called == CHGRP && group == -1) {
		setgrp(argv[a]);
	    } else if (called == CHMOD && !modset) {
		setmod(argv[a]);
	    } else {
		chall(argv[a]);		/* must be a file or directory */
	    }
	    break;
	case NULLINP:
	    break;			/* ignore it: it will go away */
	case AMBIGINP:			/* print partial match choices */
	    fprintf(stderr, "%s: Ambiguous input: choose from\n", prgnm);
	    cmdamb(argv[a], clist, stderr);
	    fputs(usage, stderr);
	    exit(1);			/* & stop */
	default:
	    fprintf(stderr, "%s: Command line parsing error '%s'\n",
		prgnm, argv[a]);
	}
    }
    if (!fileseen) {			/* need at least one file */
	fprintf(stderr, "%s: No files seen.\n", prgnm);
	fputs(usage, stderr);
	exit(1);
    } else {
	exit(err);			/* bool: error(s) detected */
    }
}

/* setown - set the owner globals */
void
setown(user)
register char *user;
{
    register struct passwd *p;
    register char *cp;

    cp = NULL;				/* '.' group specified ? */
    if ((cp = index(user, '.')) != NULL)
	*cp++ = '\0';
    if (isdigit(*user)) {
	owner = atoi(user);
	if (owner < 0) {
	    fprintf(stderr, "%s: bad user ID number specified %d.\n",
		prgnm, owner);
	    fputs(usage, stderr);
	    exit(1);
	}
    } else {
	if ((p = getpwnam(user)) == NULL) {
	    fprintf(stderr, "%s: user name '%s' not found.\n", prgnm, user);
	    fputs(usage, stderr);
	    exit(1);
	}
	owner = p->pw_uid;
    }
    if (cp != NULL)
	setgrp(cp);
}

/* setgrp - set the group globals */
void
setgrp(g)
char *g;				/* group # or name */
{
    struct group *p;

    if (isdigit(*g)) {
	group = atoi(g);
	if (group < 0) {
	    fprintf(stderr, "%s: bad group ID number specified %d.\n",
		prgnm, group);
	    fputs(usage, stderr);
	    exit(1);
	}
    } else {
	if ((p = getgrnam(g)) == NULL) {
	    fprintf(stderr, "%s: group name '%s' not found.\n", prgnm, g);
	    fputs(usage, stderr);
	    exit(1);
	}
	group = p->gr_gid;
    }
}

/* setmod - parse modes to change */
void
setmod(arg)				/* set the modes globals */
register char *arg;
{
    register int i;			/* count */
    register char *mstr = arg;		/* scanner pointer */
    register c;				/* tmp character */
    register msk = 0;			/* Read, Write, eXecute */

    for (i = 0; i < MAXMODS && *mstr != '\0'; i++) {
	if (*mstr == ',')
	    mstr++;
	dironly[i] = FALSE;
	fileonly[i] = FALSE;
	useumask[i] = FALSE;
	fromod[i] = tomod[i] = NONE;
	if (isoctal(*mstr)) {		/* absolute numeric parse */
	    modes[i] = atoo(mstr);
	    modtyp[i] = MODSET;
	    mstr = skipoct(mstr);	/* move pntr past octal string */
	} else {			/* symbolic modes parse */
	    modes[i] = 0;
	    tomod[i] = NONE;
	    while ((c = *mstr) != '\0' && c != ',') {
		if (c == 'u' || c == 'U')
		    tomod[i] |= USR;
		else if (c == 'g' || c == 'G')
		    tomod[i] |= GRP;
		else if (c == 'o' || c == 'O')
		    tomod[i] |= OTH;
		else if (c == 'a' || c == 'A')
		    tomod[i] = ALL;
		else if (c == 'd' || c == 'D')
		    dironly[i] = TRUE;
		else if (c == 'f' || c == 'F')
		    fileonly[i] = TRUE;
		else
		    break;
		mstr++;
	    }
	    if (dironly[i] && fileonly[i]) {
		fprintf(stderr, "%s: Can't set both directory & file only.\n",
		    prgnm);
		fputs(usage, stderr);
		exit(1);
	    }
	    if (tomod[i] == NONE)
		tomod[i] = ALL;
	    if ((c = *mstr++) == '=') {
		modtyp[i] = MODSET;
	    } else if (c == '+') {
		modtyp[i] = MODPLUS;
	    } else if (c == '-') {
		modtyp[i] = MODMINUS;
	    } else {
		fprintf(stderr, "%s: bad modes '%s'.\n", prgnm, arg);
		fputs(usage, stderr);
		exit(1);
	    }
	    msk = 0;
	    while ((c = *mstr) != '\0' && c != ',') {
		if (c == 'r' || c == 'R') {
		    msk |= 4;
		} else if (c == 'w' || c == 'W') {
		    msk |= 2;
		} else if (c == 'x' || c == 'X') {
		    msk |= 1;
		} else if (c == 's' || c == 'S') {
		    if (tomod[i] & USR)
			modes[i] |= S_ISUID;
		    if (tomod[i] & GRP)
			modes[i] |= S_ISGID;
		} else if (c == 't' || c == 'T') {
		    modes[i] |= S_ISVTX;
		} else if (c == 'u' || c == 'U') {
		    fromod[i] = USR;
		} else if (c == 'g' || c == 'G') {
		    fromod[i] = GRP;
		} else if (c == 'o' || c == 'O') {
		    fromod[i] = OTH;
		} else {
		    fprintf(stderr, "%s: illegal mode character '%c'.\n",
		    prgnm, c);
		    fputs(usage, stderr);
		    exit(1);
		}
		mstr++;
	    }				/* while good "from" char */
	    if (msk != 0 && fromod[i] != 0) {
		fprintf(stderr, "%s: can't mix [rwxst] with [ugo] modes.\n",
		    prgnm);
		exit(1);
	    }
	    if (tomod[i] & USR)
		modes[i] |= msk << 6;
	    if (tomod[i] & GRP)
		modes[i] |= msk << 3;
	    if (tomod[i] & OTH)
		modes[i] |= msk;
	}				/* if isdigit */
    }					/* for i */
    modset = i;
}

/* chall - do the changes, in a recursively complicated manner. */
void
chall(arg)
register char *arg;			/* file name pointer (recursive) */
{
    register i;				/* for loop var */
    register j;				/* tmp for loop var */
    static struct stat stbuf;		/* not recursive */
    DIR *dirp;				/* malloc'ed area - opendir */
    register struct direct *dp;		/* static area - readdir */
    char name[MAXNAMLEN + 2];		/* file name buffer - on stack */
    register int tmpmod;		/* temp modes - not recursive */
    static level = 0;			/* recursion level - optimization */
    register tmp;			/* temp fromod[i] modes */
    char backto[MAXPATHLEN + 1];	/* how to cd ".." */

    fileseen = TRUE;			/* at least one file seen */
    if (index(arg, '/') != NULL) {
	if (getwd(backto) == NULL) {
	    fprintf(stderr, "%s: exiting due to: %s\n", prgnm, backto);
	    exit(1);
	}
    } else {
	strcpy(backto, "..");		/* ok, if not multiple dir */
    }
    if (stat(arg, &stbuf) < 0) {
	err = TRUE;			/* err seen */
	if (!quiet) {
	    fprintf(stderr, "%s: couldn't stat file '%s'", prgnm, arg);
	    if (arg[0] == '-') {
		fprintf(stderr, " - bad option?\n");
		fputs(usage, stderr);
	    } else {
		fprintf(stderr, "\n");
	    }
	}
	return;
    }
/* change the owner, group for all files/directories if needed */
    if ((owner != -1 && owner != stbuf.st_uid) ||
	(group != -1 && group != stbuf.st_gid)) {
	if (chown(arg, owner, group) < 0) {
	    err = TRUE;
	    if (!quiet)
		fprintf(stderr,
		"%s: Can't change owner/group of '%s' still %d,%d.\n",
		    prgnm, arg, stbuf.st_uid, stbuf.st_gid);
	}
    }
/* Do mode changes before recursion so that one can recover from removing
 * "x" from his(/her) own directory tree by using d+x.	It also makes it
 * harder to get into that position: if it removes "x" from a directory,
 * then it can't recurse down further.
 */
    for (i = 0; i < modset; i++) {
	if ((!dironly[i] && !fileonly[i]) ||
	    (dironly[i] && (stbuf.st_mode & S_IFDIR)) ||
	    (fileonly[i] && (stbuf.st_mode & S_IFREG))) {
	    if (fromod[i] != NONE) {
		tmp = tomod[i];
		tmpmod = 0;
		for (j = 0; j < 3; j++) {
		    tmpmod <<= 3;
		    if (tmp & 1) {
			if (fromod[i] & USR)
			    tmpmod |= (stbuf.st_mode & 0700) >> 6;
			if (fromod[i] & GRP)
			    tmpmod |= (stbuf.st_mode & 070) >> 3;
			if (fromod[i] & OTH)
			    tmpmod |= stbuf.st_mode & 7;
		    }
		    tmp >>= 1;
		}
		switch (modtyp[i]) {
		case MODSET:
		    break;
		case MODPLUS:
		    tmpmod |= stbuf.st_mode;
		    break;
		case MODMINUS:
		    tmpmod = stbuf.st_mode & ~tmpmod;
		    break;
		}
	    } else switch (modtyp[i]) {
	    case MODSET:
		tmpmod = modes[i];
		break;
	    case MODPLUS:
		tmpmod = stbuf.st_mode | modes[i];
		break;
	    case MODMINUS:
		tmpmod = stbuf.st_mode & ~modes[i];
		break;
	    }
	    if (useumask[i])
		tmpmod = tmpmod & ~umsk; /* oddly efficient on 11's & vaxen */
	    if (tmpmod != stbuf.st_mode) {
		if (chmod(arg, tmpmod) < 0) {
		    err = TRUE;
		    if (!quiet)
			fprintf(stderr,
			"%s: Modes not correctly changed for '%s'.\n",
			prgnm, arg);
		}
		stbuf.st_mode = tmpmod;
	    }				/* if from/to format */
	}				/* if mode changing needed */
    }					/* for i */
/* now handle recursion, if needed */
    if (recurse && stbuf.st_mode & S_IFDIR) {
	if (chdir(arg) < 0) {
	    err = TRUE;
	    if (!quiet)
		fprintf(stderr, "%s: couldn't change dir to '%s'.\n",
		    prgnm, arg);
	    return;
	}
	if ((dirp = opendir(".")) == NULL) {
	    err = TRUE;
	    if (!quiet)
		fprintf(stderr, "%s: couldn't open directory '%s'.\n",
		    prgnm, arg);
	    if (chdir(backto) < 0) {
		fprintf(stderr, "%s: can't chdir back - exiting.\n", prgnm);
		exit(TRUE);
	    }
	    return;			/* err - but can continue */
	}
	if (++level < TODEEP) {		/* 1st: the easy (and faster) way */
	    while ((dp = readdir(dirp)) != NULL) {
		if (dp->d_ino == 0)
		    continue;
		if (strcmp(".", dp->d_name) == 0)
		    continue;		/* don't do self, yet */
		if (strcmp("..", dp->d_name) == 0)
		    continue;		/* don't do parent dir */
		strcpy(name, dp->d_name); /* save it on stack */
		chall(name);
	    }
	} else {			/* 2nd: the hard way, if level */
	    name[0] = '\0';
	    while ((dp = readdir(dirp)) != NULL) {
		if (name[0] != '\0') {
		    if (strcmp(name, dp->d_name) != 0) {
			continue;	/* re-sync */
		    } else if ((dp = readdir(dirp)) == NULL) {
			break;
		    }
		}
		if (dp->d_ino == 0)
		    continue;
		if (strcmp(".", dp->d_name) == 0)
		    continue;		/* don't do self, yet */
		if (strcmp("..", dp->d_name) == 0)
		    continue;		/* don't do parent dir */
		strcpy(name, dp->d_name); /* save it on stack */
		closedir(dirp);
		chall(name);
		if ((dirp = opendir(".")) == NULL) {
		    err = TRUE;
		    if (!quiet)
			fprintf(stderr, "%s: couldn't open directory '%s'.\n",
			    prgnm, arg);
		    if (chdir(backto) < 0) {
			fprintf(stderr, "%s: can't chdir back - exiting.\n",
			    prgnm);
			exit(TRUE);
		    }
		}
	    }				/* while dir read */
	}				/* if level */
	closedir(dirp);
	level--;
	if (chdir(backto) < 0) {
	    fprintf(stderr, "%s: can't chdir back - exiting.\n", prgnm);
	    exit(TRUE);
	}
    }					/* if arg was a directory */
}

/* atoo - convert ASCII octal string to integer.
 * the namespace:	atoi - ASCII (decimal) to integer
 *			atol - ASCII (decimal) to long
 * is brain damaged.
 * What name should be used if one needs an octal long (e.g. for a pdp11)?
 * It probably should have been along the lines of:
 * int dtoi(), long dtol(), short dtos()	decimal
 * int otoi(), long otol(), short otos()	octal
 * int htoi(), long htol(), short htos()	hex, etc.
 * Maybe if C had long symbol names everywhere, this wouldn't have happened.
 * After all, an EBCDIC system would want to use atoi() for compatibility
 * as well as for sanity (bury the system dependant stuff in a library).
 */
int
atoo(str)
register char *str;
{
    register i;
    register c;

    i = 0;
    while ((c = *str++) != '\0') {
	if (c < '0' || c > '7')
	    return i;
	i <<= 3;
	i += c - '0';
    }
    return i;
}

/* isoctal - return TRUE if c is an octal (ASCII) digit */
int
isoctal(c)
char c;
{
    switch (c) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
	return TRUE;
    default:
	return FALSE;
    }
}

/* skipoct - parse past an octal number */
char *
skipoct(str)
register char *str;
{
    while (isoctal(*str++))
	;
    return --str;
}
