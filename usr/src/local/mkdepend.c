/* mkdepend - create include dependencies by running sources through /lib/cpp
 * Author: Stephen Uitti, PUCC, 1985
 */

#ifndef lint
static char *Rcsid =
"$Id: mkdepend.c,v 1.2 85/05/24 16:35:25 ach Exp $";
#endif	lint

/* $Log:	mkdepend.c,v $
 * Revision 1.2  85/05/24  16:35:25  ach
 * Added explicit rule option. -sku
 * 
 * Revision 1.1  85/03/27  12:48:40  root
 * Added -includesys, -verbose, -target, -destsuffix options.  Removed the
 * unique sorting routines, improved them and made them a library - srtunq.
 * The primary goal was to let it write dependencies for one line makes
 * that go directly from source to executable, without ever creating the
 * relocatable object (intermediate) files. -sku
 */
#include <stdio.h>
#include <sys/param.h>			/* for MAXPATHLEN in 4_2 */
					/* in 2_9, loads types.h for void */
#ifdef BSD2_9
#include <ndir.h>			/* for MAXPATHLEN, MAXNAMLEN */
#else
#include <sys/dir.h>			/* for MAXNAMLEN in 4_2 */
#endif
#include <sys/file.h>			/* for access */
#include <local/cmd.h>			/* for argv parsing */
#include <local/srtunq.h>		/* for uniq sort routines */

/* libc functions */
char   *index();			/* find char in string */
char   *rindex();			/* reverse index */
char   *strcat();			/* append 2 strings */
char   *strcpy();			/* string copy */

/* forward functions */
void	msoio();			/* open output */
void	mkdepend();			/* does the real work */

/* globals */
char   *prgnm;				/* the progs called name */
struct	srtent u;			/* the handle on the uniq tree */
int	alldep = FALSE;			/* -all dependencies (/usr/include) */
int	kernel = FALSE;			/* -kernel for 2_9 bsd stuff */
char   *exprule = NULL;			/* -explicitrule generate rule */
char   *targetname = NULL;		/* target name for next file */
char   *destsuffix = ".o";		/* suffix for targets */
int	makenseen = FALSE;		/* output file has been specified */
char   *makename = "stdout";		/* -makename= default file for edit */
FILE   *makefd;				/* file desc. for output file */
int	ismakeopen = FALSE;		/* if the output file is open */
char	objpath[MAXPATHLEN+1];		/* -objpath= prepended to .o's */
char	cppflags[100];			/* string for flags to pass to cpp */
int	shortincl = FALSE;		/* ${I} instead of /usr/include */
int	sysincl = FALSE;		/* ${S} instead of /usr/include/sys */
int	verbose = FALSE;		/* verbage for the impatient */
char   *usage =
"Usage: mkdepend [-help] [-alldep] [-destsuffix[=suffix]] \n\
\t[-explicitrule[=rule]] [-objpath=dir]\n\
\t[-includesys] [-kernel29] [-makename=file] [-shortinclude]\n\
\t[-target=targetname] [-CPP flags: -I... -D... -U...] file...\n";

char   *deplin = "# DO NOT DELETE THIS LINE - make depend DEPENDS ON IT\n";

char *clist[] = {
    "-Help\t\tPrint this text.\n",
#define HELP 0
    "-AllDepends\tDo /usr/include dependencies, not just local ones.\n",
#define ALLDEP 1
    "-DestSuffix\t=suffix String to append to target.\n",
#define DESTSUFFIX 2
    "-ExplicitRule\t=Compile rule.\n",
#define EXPRULE 3
    "-IncludeSys\tUse ${S} instead of /usr/include/sys.\n",
#define SYSINCL 4
    "-Kernel29\tDo extra stuff required by 2.9 bsd kernel.\n",
#define KERNEL29 5
    "-Makename\t=file Filename for edit.\n",
#define MAKENAME 6
    "-Objpath\t=directory Prepend string for target.\n",
#define OBJPATH 7
    "-ShortInclude\tUse ${I} instead of /usr/include.\n",
#define SHORTINCL 8
    "-Target\t\t=target Name for next target's basename.\n",
#define TARGET 9
    "-Verbose\tExtra text to stderr for the impatient.\n",
#define VERBOSE 10
    NULL
};

/* some init & argv parsing */
main(argc, argv)
register int argc;
register char **argv;
{
    register a;				/* argv subscript */
    register i;				/* tmp */

    /* prgnm = program name */
    if ((prgnm = rindex(argv[0], '/')) == NULL)
	prgnm = argv[0];
    else
	prgnm++;
    cppflags[0] = '\0';			/* terminate cpp flags string */
    objpath[0] = '\0';			/* init object path */
    srtinit(&u);			/* init sorting database tag */
    for (a = 1; a < argc; a++) {
	switch (cmdprs(argv[a], clist)) {
	case HELP:
	    fputs(usage, stdout);
	    cmdhlp(argv[a], clist, stdout);
	    exit(0);			/* life's too tough to go on */
	case ALLDEP:
	    alldep = TRUE;
	    break;
	case DESTSUFFIX:		/* destination suffix */
	    destsuffix = cmddlm(argv[a]);
	    break;
	case EXPRULE:			/* explicit rule */
	    exprule = cmddlm(argv[a]);
	    break;
	case KERNEL29:
	    kernel = TRUE;
	    break;
	case MAKENAME:
	    if (makenseen) {
		fprintf(stderr, "%s: Allowed one output file only.\n", prgnm);
		exit(1);
	    }
	    makenseen = TRUE;
	    makename = cmddlm(argv[a]);
	    if (strlen(makename) == 0) {
		makename = "makefile";
		if (access(makename, F_OK) != 0)
		    makename[0] = 'M';	/* not a real check */
	    }
	    break;
	case OBJPATH:
	    strcpy(objpath, cmddlm(argv[a]));
	    i = strlen(objpath);
	    if (i == 0) {
		fprintf(stderr, "%s: -Objpath requires path string.\n",
			prgnm);
		fputs(usage, stderr);
		exit(1);
	    }
	    if (i >= MAXPATHLEN) {
		fprintf(stderr, "%s: Object path too long: max is %d.\n",
		    prgnm, MAXPATHLEN);
		exit(1);
	    }
	    if (objpath[i - 1] != '/')
		strcat(objpath, "/");
	    break;
	case SHORTINCL:
	    shortincl = TRUE;		/* ${I} instead of /usr/include */
	    break;
	case SYSINCL:
	    sysincl = TRUE;		/* ${S} instead of /usr/include/sys */
	    break;
	case TARGET:
	    targetname = cmddlm(argv[a]);
	    if (strlen(targetname) == 0) {
		fprintf(stderr, "%s: target option requires name.\n", prgnm);
		exit(1);
	    }
	    break;
	case VERBOSE:			/* user wants to hear noise */
	    verbose = TRUE;
	    break;
	case UNRECINP:
	    if (argv[a][0] == '-') {	/* cpp switch ? */
		switch (argv[a][1]) {
		default:
		    fprintf(stderr, "%s: Unknown switch passed to CPP %s\n",
			prgnm, argv[a]);
		case 'D': case 'I': case 'U':
		    strcat(cppflags, argv[a]);
		    strcat(cppflags, " "); /* add a space separator */
		    break;
		}
	    } else {
		if (verbose)
		    fprintf(stderr, "%s: working on %s.\n", prgnm, argv[a]);
		mkdepend(argv[a]);	/* file to process */
		/* Per-file re-initialization:
		 * for options that only affect 1 file */
		targetname = NULL;
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
	    fprintf(stderr, "%s: Internal cmd line parsing error '%s'\n",
		prgnm, argv[a]);
	}
    }					/* for argv */
    if (ismakeopen)
	fputs("\n\n# *** Do not add anything here - It will go away. ***\n",
	    makefd);
    exit(0);				/* exit status - good */
}

/* msoio - Make Sure Output Is Open.  Interacts strongly with main().
 * Also, do the .orig, .old nonsense if it's a makefile edit. */
void
msoio()
{
    register FILE *oldfd;		/* file pointer for .old */
    char buf[MAXPATHLEN+1];		/* temp */
    char buf2[MAXPATHLEN+1];		/* temp */
    char rwbuf[BUFSIZ];			/* for read/write */

    if (ismakeopen)
	return;
    ismakeopen = TRUE;			/* will be: all errs are fatal */
    if (strcmp("stdout", makename) == 0) {
	makefd = stdout;
	fputc('\n', makefd);		/* one blank line */
	fputs(deplin, makefd);		/* the first line */
	return;
    }
    if (access(makename, F_OK) == 0) {	/* if makefile exits */
	strcpy(buf, makename);		/* get rid of .old */
	strcat(buf, ".old");
	if (access(buf, F_OK) == 0) {
	    if (unlink(buf)) {
		fprintf(stderr, "%s: Can't remove %s for edit\n", prgnm, buf);
		exit(1);
	    }
	}
	strcpy(buf2, makename);
	strcat(buf2, ".orig");
	if (access(buf2, F_OK) != 0)	/* if no .orig */
	    strcpy(buf, buf2);		/* mv makefile .orig */
	if (link(makename, buf)) {	/* else mv makefile .old */
	    fprintf(stderr, "%s: Can't link %s to %s\n",
		prgnm, buf, makename);
	    exit(1);
	}
	if (unlink(makename)) {
	    fprintf(stderr, "%s: Can't remove %s\n", prgnm, makename);
	    exit(1);
	}
    } else {
	buf[0] = '\0';			/* no copy */
    }
    if ((makefd = fopen(makename, "w")) == NULL) {
	fprintf(stderr, "%s: Can't open output '%s'.\n", prgnm, makename);
	exit(1);
    }
    if (buf[0] != '\0') {		/* if .old file */
	if ((oldfd = fopen(buf, "r")) == NULL) {
	    fprintf(stderr, "%s Can't open backup copy of %s\n",
		prgnm, makename);
	    exit(1);
	}
	while (fgets(rwbuf, BUFSIZ, oldfd) != NULL) {	/* until EOF */
	    if (strcmp(deplin, rwbuf) == 0)
		break;			/* or the depend line was seen */
	    fputs(rwbuf, makefd);
	}
	(void) fclose(oldfd);		/* close the .old file for gigles */
    }
    fputs(deplin, makefd);		/* make sure the depend line is in */
}

#define MAXCOL 78			/* output width max for makefile */

/* do the work for a file */
void
mkdepend(infile)
char *infile;
{
    register char *p;			/* temp pointer */
    register char *q;			/* temp pointer */
    register FILE *cppfd;		/* file desc for /lib/cpp */
    char buf[BUFSIZ];			/* temp buff */
    register linfile;			/* length of infile */
    char basename[MAXNAMLEN+1];		/* just the file name */
    register oplen;			/* current length of objpath */
    register le;			/* length of current output line */
    register char *targ;		/* target's name */
    register ifg, sfg;			/* shortinclude & sysinclude flags */

    msoio();				/* Make Sure Output Is Open */
    linfile = strlen(infile);
    if ((p = rindex(infile, '/')) == NULL) /* create basename */
	p = infile;
    else
	p++;
    strcpy(basename, p);
    if ((p = rindex(basename, '.')) != NULL)
	*p = '\0';			/* remove trailing ".*" */
    if (targetname != NULL)		/* set up target's name */
	targ = targetname;
    else
	targ = basename;
    if (access(infile, R_OK) != 0) {
	fprintf(stderr, "%s: Can't open input file '%s', skipped.\n",
	    prgnm, infile);
	return;
    }
    (void)strcpy(buf, "/lib/cpp ");		/* build cpp cmd line */
    (void)strcat(buf, cppflags);	/* add command flags */
    (void)strcat(buf, infile);		/* add file name */
    srtfree(&u);			/* init insertion sorter */
    cppfd = (FILE *) popen(buf, "r");
    while (fgets(buf, BUFSIZ, cppfd) != NULL) {
	if (buf[0] != '#')		/* must start with '#' */
	    continue;
	if ((p = index(buf, '"')) == NULL) /* find first double quote */
	    continue;
	p++;
	if (index(p, '"') != NULL)	/* terminate the file name */
	    *index(p, '"') = '\0';
	if (!alldep && strncmp("/usr/include", p, 12) == 0)
	    continue;			/* ignore /usr/include... stuff */
	if ((q = srtin(&u, p, NULL)) != NULL) /* insert into list */
	    fprintf(stderr, "%s: %s - %s\n", prgnm, q, p); /* warning */
    }
    srtgti(&u);				/* init for srtgets */
    oplen = strlen(objpath);		/* set length of prepend */
    fputc('\n', makefd);		/* blank line between files */
    le = MAXCOL;			/* force new line output */
    while ((p = srtgets(&u)) != NULL) {	/* write out the entries */
	ifg = sfg = FALSE;
	if (sysincl && strncmp(p, "/usr/include/sys", 16) == 0) {
	    p += 12;
	    sfg = TRUE;
	} else if (shortincl && strncmp(p, "/usr/include", 12) == 0) {
	    p += 8;			/* right size, anyway */
	    ifg = TRUE;
	}
	if (le + strlen(p) >= MAXCOL) {
	    le = oplen + linfile + 4;
	    fprintf(makefd, "\n%s%s%s:", objpath, targ, destsuffix);
	}
	fputc(SPC, makefd);
	if (ifg) {
	    fputs("${I}", makefd);
	    p += 4;			/* right place */
	    le += 4;
	} else if (sfg) {
	    fputs("${S}", makefd);
	    p += 4;			/* right place */
	    le += 4;
	}
	fputs(p, makefd);
	le += 1 + strlen(p);
    }
    /* explicit rule */
    if (exprule != NULL) {		/* if rule */
        if (*exprule == '\0') {		/* if use default rule */
            fprintf(makefd, "\n\tcc -c ${CFLAGS} %s", infile);
        } else {			/* use explicit rule */
            fprintf(makefd, "\n\t%s %s", exprule, infile);
        }
    }
    /* kernel post-amble, no one thought to "just" fix make. */
    if (kernel && strcmp(&infile[linfile - 2], ".c") == 0) {
	/* compile line .c => .s */
	fprintf(makefd, "\n\t${C} %s\n", infile);
	/* ed line: .s => .s (for spl's) */
	fprintf(makefd, "\t${E} %s.s\n", basename);
	/* as line: .s => .o */
	fprintf(makefd, "\t${A} %s%s %s.s\n", targ, destsuffix, basename);
	/* rm the .s */
	fprintf(makefd, "\t-rm %s.s", basename);
    }
    pclose(cppfd);			/* end of that file */
}
