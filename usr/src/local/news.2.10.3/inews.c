/*
 * inews - insert, receive, and transmit news articles.
 */

#ifdef SCCSID
static char	*SccsId = "@(#)inews.c	2.54	4/16/85";
#endif /* SCCSID */

#include "iparams.h"
#include <errno.h>

/* local defines for inews */

#define OPTION	0	/* pick up an option string */
#define STRING	1	/* pick up a string of arguments */

#define UNKNOWN 0001	/* possible modes for news program */
#define UNPROC	0002	/* Unprocessed input */
#define PROC	0004	/* Processed input */
#define	CANCEL	0010	/* Cancel an article */
#define	CREATENG 0020	/* Create a new newsgroup */

char	forgedname[NAMELEN];	/* A user specified -f option. */
extern char histline[];

char *Progname = "inews";	/* used by xerror to identify failing program */

struct {			/* options table. */
	char	optlet;		/* option character. */
	char	filchar;	/* if to pickup string, fill character. */
	int	flag;		/* TRUE if have seen this opt. */
	int	oldmode;	/* OR of legal input modes. */
	int	newmode;	/* output mode. */
	char	*buf;		/* string buffer */
} *optpt, options[] = { /*
optlet	filchar		flag	oldmode	newmode		buf	*/
't',	' ',		FALSE,	UNPROC,	UNKNOWN,	header.title,
'n',	NGDELIM,	FALSE,	UNPROC,	UNKNOWN,	header.nbuf,
'd',	'\0',		FALSE,	UNPROC,	UNKNOWN,	header.distribution,
'e',	' ',		FALSE,	UNPROC,	UNKNOWN,	header.expdate,
'p',	'\0',		FALSE,	UNKNOWN,PROC,		filename,
'f',	'\0',		FALSE,	UNPROC,	UNKNOWN,	forgedname,
'F',	' ',		FALSE,	UNPROC,	UNKNOWN,	header.followid,
'c',	'\0',		FALSE,	UNKNOWN,CANCEL,		filename,
'C',	'\0',		FALSE,	UNKNOWN,CREATENG,	header.nbuf,
#define hflag	options[9].flag
'h',	'\0',		FALSE,	UNPROC,	UNKNOWN,	filename,
#define oflag	options[10].flag
'o',	'\0',		FALSE,	UNPROC, UNKNOWN,	header.organization,
'a',	'\0',		FALSE,	UNPROC, UNKNOWN,	header.approved,
'\0',	'\0',		0,	0,	0,		(char *)NULL
};

FILE *mailhdr();
char *genversion();
extern int errno;

/*
 *	Authors:
 *		Matt Glickman	glickman@ucbarpa.Berkeley.ARPA
 *		Mark Horton	mark@cbosgd.UUCP
 *		Stephen Daniels	swd@mcnc.UUCP
 *		Tom Truscott	trt@duke.UUCP
 *	IHCC version adapted by:
 *		Larry Marek	larry@ihuxf.UUCP
 */
main(argc, argv)
int	argc;
register char **argv;
{
	int	state;		/* which type of argument to pick up	*/
	int	tlen, len;	/* temps for string processing routine	*/
	register char *ptr;	/* pointer to rest of buffer		*/
	int	filchar;	/* fill character (state = STRING)	*/
	char	*user = NULL, *home = NULL;	/* environment temps	*/
	struct passwd	*pw;	/* struct for pw lookup			*/
	struct group	*gp;	/* struct for group lookup		*/
	register int	i;
	FILE	*mfd;		/* mail file file-descriptor		*/
	char	cbuf[BUFLEN];	/* command buffer			*/

	/* uuxqt doesn't close all it's files */
	for (i = 3; !close(i); i++)
		;
	/* set up defaults and initialize. */
	mode = UNKNOWN;
	pathinit();
	ptr = rindex(*argv, '/');
	if (!ptr)
		ptr = *argv - 1;
	if (!strncmp(ptr+1, "rnews", 5)) {
		mode = PROC;
#ifdef NICENESS
		nice(NICENESS);
#endif /* NICENESS */
	} else if (argc < 2)
		goto usage;

	state = OPTION;
	header.title[0] = header.nbuf[0] = filename[0] = '\0';

	/* check for existence of special files */
	if (!rwaccess(ARTFILE)) {
		mfd = mailhdr((struct hbuf *)NULL, exists(ARTFILE) ? "Unwritable files!" : "Missing files!");
		if (mfd != NULL) {
#ifdef HIDDENNET
			fprintf(mfd,"System: %s.%s\n\nThere was a problem with %s!!\n", LOCALSYSNAME, FULLSYSNAME, ARTFILE);
#else /* !HIDDENNET */
			fprintf(mfd,"System: %s\n\nThere was a problem with %s!!\n", FULLSYSNAME, ARTFILE);
#endif /* !HIDDENNET */
			(void) sprintf(cbuf, "touch %s;chmod 666 %s", ARTFILE, ARTFILE);
			(void) system(cbuf);
			if (rwaccess(ARTFILE))
				fprintf(mfd, "The problem has been taken care of.\n");
			else
				fprintf(mfd, "Corrective action failed - check suid bits.\n");
			(void) mclose(mfd);
		}
	}
	if (!rwaccess(ACTIVE)) {
		mfd = mailhdr((struct hbuf *)NULL, exists(ACTIVE) ? "Unwritable files!" : "Missing files!");
		if (mfd != NULL) {
#ifdef HIDDENNET
			fprintf(mfd,"System: %s.%s\n\nThere was a problem with %s!!\n", LOCALSYSNAME, FULLSYSNAME, ARTFILE);
#else /* !HIDDENNET */
			fprintf(mfd, "System: %s\n\nThere was a problem with %s!!\n", FULLSYSNAME, ACTIVE);
#endif /* !HIDDENNET */
			(void) sprintf(cbuf, "touch %s;chmod 666 %s", ACTIVE, ACTIVE);
			(void) system(cbuf);
			if (rwaccess(ACTIVE))
				fprintf(mfd, "The problem has been taken care of.\n");
			else
				fprintf(mfd, "Corrective action failed - check suid bits.\n");
			(void) mclose(mfd);
		}
	}
	SigTrap = FALSE;	/* true if a signal has been caught */
	if (mode != PROC) {
		(void) signal(SIGHUP, onsig);
		(void) signal(SIGINT, onsig);
	}
	savmask = umask(N_UMASK);	/* set up mask */
	uid = getuid();
	gid = getgid();
	duid = geteuid();
	dgid = getegid();
	if (uid == 0 && duid == 0) {
		/*
		 * Must go through with this kludge since
		 * some systems do not honor the setuid bit
		 * when root invokes a setuid program.
		 */
		if ((pw = getpwnam(NEWSUSR)) == NULL)
			xerror("Cannot get NEWSU pw entry");

		duid = pw->pw_uid;
		if ((gp = getgrnam(NEWSGRP)) == NULL)
			xerror("Cannot get NEWSG gr entry");
		dgid = gp->gr_gid;
		(void) setgid(dgid);
		(void) setuid(duid);
	}

#ifndef IHCC
	/*
	 * We force the use of 'getuser()' to prevent forgery of articles
	 * by just changing $LOGNAME
	 */
	if (isatty(fileno(stderr))) {
		if ((user = getenv("USER")) == NULL)
			user = getenv("LOGNAME");
		if ((home = getenv("HOME")) == NULL)
			home = getenv("LOGDIR");
	}
#endif
	if (user == NULL || home == NULL)
		getuser();
	else {
		if (username == NULL || username[0] == 0) {
			username = AllocCpy(user);
		}
		userhome = AllocCpy(home);
	}
	getuser();

	/* loop once per arg. */

	++argv;		/* skip first arg, which is prog name. */

	while (--argc) {
	    if (state == OPTION) {
		if (**argv != '-') {
			xerror("Bad option string \"%s\"", *argv);
		}
		while (*++*argv != '\0') {
			for (optpt = options; optpt->optlet != '\0'; ++optpt) {
				if (optpt->optlet == **argv)
					goto found;
			}
			/* unknown option letter */
usage:
			fprintf(stderr, "usage: inews -t title");
			fprintf(stderr, " [ -n newsgroups ]");
			fprintf(stderr, " [ -e expiration date ]\n");
			fprintf(stderr, "\t[ -f sender]\n\n");
			xxit(1);

		    found:;
			if (optpt->flag == TRUE || (mode != UNKNOWN &&
			    (mode&optpt->oldmode) == 0)) {
				xerror("Bad %c option", **argv);
			}
			if (mode == UNKNOWN)
				mode = optpt->newmode;
			filchar = optpt->filchar;
			optpt->flag = TRUE;
			state = STRING;
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
		(void) strcpy(ptr, *argv++);
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

	tty = isatty(fileno(stdin));

	/* This code is really intended to be replaced by the control message. */
	if (mode == CANCEL) {
		char *p; FILE *f;
		f = xfopen(filename, "r");
		(void) hread(&header, f, TRUE);
		p = index(header.path, ' ');
		if (p != NULL)
			*p = 0;
		p = header.path;
		if (strncmp(username, p, strlen(username))
			&& uid != ROOTID && uid != geteuid() && uid)
			xerror("Not contributor");
		cancel();
		xxit(0);
	}

	if (*header.nbuf) {
		lcase(header.nbuf);
		ptr = index(header.nbuf, '\0');
		if (ptr[-1] == NGDELIM)
			*--ptr = '\0';
	}
	(void) nstrip(header.title);
	(void) nstrip(header.expdate);
	(void) nstrip(header.followid);
	if (mode != PROC) {
		getident(&header);
		if (hflag) {
			/* Fill in a few to make frmread return TRUE */
			(void) strcpy(header.subdate, "today");
			(void) strcpy(header.path, "me");
			/* Allow the user to supply some headers. */
			hread(&header, stdin, FALSE);
			/* But there are certain fields we won't let him specify. */
			if (header.from[0])
				(void) strcpy(forgedname, header.from);
			header.from[0] = '\0';
			header.path[0] = '\0';
			header.subdate[0] = '\0';
			header.sender[0] = '\0';
		}
#ifdef MYORG
		if (header.organization[0] == '\0') {
			strncpy(header.organization, MYORG, BUFLEN);
			if (strncmp(header.organization, "Frobozz", 7) == 0)
				header.organization[0] = '\0';
			if (ptr = getenv("ORGANIZATION"))
				strncpy(header.organization, ptr, BUFLEN);
			/*
			 * Note that the organization can also be turned off by
			 * setting it to the null string, either in MYORG or
			 * $ORGANIZATION in the environment.
			 */
			if (header.organization[0] == '/') {
				mfd = fopen(header.organization, "r");
				if (mfd) {
					(void) fgets(header.organization, sizeof header.organization, mfd);
					(void) fclose(mfd);
				}
				ptr = index(header.organization, '\n');
				if (ptr)
					*ptr = 0;
			}
		}
#endif /* MYORG */
		if (forgedname[0]) {
			register char *p1;
			(void) strcpy(header.path, forgedname);
			p1 = strpbrk(header.path, "@ (<");
			if (p1 == NULL)
				(void) sprintf(header.from,"%s@%s%s",
					forgedname, FULLSYSNAME, MYDOMAIN);
			else {
				(void) strncpy(header.from, forgedname, BUFLEN);
				*p1 = '\0';
			}
			(void) sprintf(header.sender, "%s@%s%s",
				username, FULLSYSNAME, MYDOMAIN);
		} else {
			gensender(&header, username);
		}
		(void) strncpy(header.postversion, genversion(), BUFLEN);
	}

	/* Authorize newsgroups. */
	if (mode == PROC) {
#ifdef BATCH
		checkbatch();
#endif /* BATCH */
		(void) signal(SIGHUP, SIG_IGN);
		(void) signal(SIGINT, SIG_IGN);
		(void) signal(SIGQUIT, SIG_IGN);
		header.ident[0] = '\0';
		if (hread(&header, stdin, TRUE) == NULL)
			xerror("Inbound news is garbled");
		input();
		if (history(&header)) {
			log("Duplicate article %s rejected", header.ident);
			xxit(0);
		}
	}

	/* Easy way to make control messages, since all.all.ctl is unblessed */
	if (mode != PROC && prefix(header.title, "cmsg ") && header.ctlmsg[0] == 0)
		(void) strcpy(header.ctlmsg, &header.title[5]);
	is_ctl = mode != CREATENG &&
		(ngmatch(header.nbuf, "all.all.ctl,") || header.ctlmsg[0]);
#ifdef DEBUG
	fprintf(stderr,"is_ctl set to %d\n", is_ctl);
#endif

			/* Must end in comma (NGDELIM) */
#define MODGROUPS	"mod.all,all.mod,all.announce,"
	if (ngmatch(header.nbuf, MODGROUPS) && !header.approved[0]) {
		mfd = mailhdr(&header, "Moderated newsgroup");
		if (mfd) {
			fprintf(mfd, "This newsgroup is moderated, and cannot be posted to directly.\n");
			fprintf(mfd, "Please mail your article to the moderator for posting.\n");
			hwrite(&header, mfd);
			if (infp)
				while ((i = getc(infp)) != EOF)
					putc(i, mfd);
			(void) mclose(mfd);
		}
		xerror("Unapproved moderated newsgroup.");
	}

	if (mode != CREATENG) {
		if (!*header.title)
			xerror("No title, ng %s from %s", header.nbuf,
				header.from);
		if (!*header.nbuf)
			(void) strcpy(header.nbuf, DFLTNG);
	}

	if (mode <= UNPROC)
		ctlcheck();

	if (mode == CREATENG)
		createng();
	/* Determine input. */
	if (mode != PROC)
		input();

	/* Do the actual insertion. */
	insert();
}

/*
 *	Create a newsgroup
 */
createng()
{

	/*
	 * Only certain users are allowed to create newsgroups
	 */
	if (uid != ROOTID && uid != geteuid() && uid) {
		fprintf(stderr, "Please contact one of the local netnews people\n\tto create this group for you");
		xxit(1);
	}

	(void) sprintf(bfr, "%s/inews -n %s.ctl -t cmsg newgroup %s", LIB,
		header.nbuf, header.nbuf);
	printf("Please type in a paragraph describing the new newsgroup.\n");
	printf("End with control D as usual.\n");
	printf("%s\n", bfr);
	(void) fflush(stdout);
	(void) system(bfr);
	exit(0);
	/*NOTREACHED*/
}

char firstbufname[BUFLEN];
/*
 *	Link ARTICLE into dir for ngname and update active file.
 */
long
localize(ngname)
char	*ngname;
{
	char afline[BUFLEN];
	long ngsize;
	long fpos;
	int e;
	char *cp;

	lock();
	actfp = xfopen(ACTIVE, "r+");

	for(;;) {
		fpos = ftell(actfp);
		if (fgets(afline, sizeof afline, actfp) == NULL) {
			unlock();
			(void) fclose(actfp);
			return FALSE;		/* No such newsgroup locally */
		}
		if (prefix(afline, ngname)) {
			(void) sscanf(afline, "%s %ld", bfr, &ngsize);
			if (strcmp(bfr, ngname) == 0) {
				break;
			}
			if (ngsize < 0 || ngsize > 99998) {
				logerr("found bad ngsize %ld ng %s, setting to 1", ngsize, bfr);
				ngsize = 1;
			}
		}
	}
	for (;;) {
		cp = dirname(ngname);
		if (!exists(cp))
			mknewsg(cp, ngname);

		(void) sprintf(bfr, "%s/%ld", cp, ngsize+1);
#ifdef VMS
		if (vmslink(ARTICLE, bfr) == 0)
			break;
#else /* !VMS */
		if (link(ARTICLE, bfr) == 0)
			break;
#endif /* !VMS */
		e = errno;	/* keep log from clobbering it */
		log("Cannot install article as %s: %s", bfr, errmsg(errno));
		if (e != EEXIST) {
			logerr("Link into %s failed (%s); check dir permissions.",
			    bfr, errmsg(e));
			unlock();
			(void) fclose(actfp);
			return FALSE;
		}
		ngsize++;
	}

	/* Next two lines program around a bug in 4.1BSD stdio. */
	(void) fclose(actfp);
#ifdef VMS
	vmstounix(ACTIVE);
#endif /* VMS */
	actfp = fopen(ACTIVE, "r+");

	fseek(actfp, fpos, 0);
	/* Has to be same size as old because of %05d.
	 * This will overflow with 99999 articles.
	 */
	fprintf(actfp, "%s %05ld", ngname, ngsize+1);
	(void) fclose(actfp);
#ifdef VMS
	unixtovms(ACTIVE);
#endif /* VMS */
	unlock();
	if (firstbufname[0] == '\0')
		(void) strcpy(firstbufname, bfr);
	(void) sprintf(bfr, "%s/%ld ", ngname, ngsize+1);
	addhist(bfr);
	return ngsize+1;
}

/*
 *	Localize for each newsgroup and broadcast.
 */
insert()
{
	register char *ptr;
	register FILE *tfp;
	register int c;
	struct srec srec;	/* struct for sys file lookup	*/
	struct tm *tm;
	int is_invalid = FALSE;
	int exitcode = 0;
#ifdef DOXREFS
	register char *nextref = header.xref;
#endif /* DOXREFS */

	/* Fill up the rest of header. */
	if (mode != PROC) {
		history(&header);
	}
	dates(&header);
	tm = localtime(&header.rectime);
	if (header.expdate[0])
		addhist(" ");
#ifdef USG
	sprintf(bfr,"%2.2d/%2.2d/%d %2.2d:%2.2d\t",
#else /* !USG */
	sprintf(bfr,"%02d/%02d/%d %02d:%02d\t",
#endif /* !USG */
		tm->tm_mon+1, tm->tm_mday, tm->tm_year,tm->tm_hour, tm->tm_min);
	addhist(bfr);
	log("%s %s ng %s subj '%s'", mode==PROC ? "received" : "posted", header.ident, header.nbuf, header.title);
	if (mode==PROC)
		log("from %s relay %s", header.from, header.relayversion);

	/* Clean up Newsgroups: line */
	if (!is_ctl && mode != CREATENG)
		is_invalid = ngfcheck(mode == PROC);

	/* Write article to temp file. */
	tfp = xfopen(mktemp(ARTICLE), "w");

	if (is_invalid) {
		logerr("No valid newsgroups found, moved to junk");
		if (localize("junk"))
			savehist(histline);
		exitcode = 1;
		goto writeout;
	}

	if (time((time_t *)0) > (cgtdate(header.subdate) + HISTEXP) ){
		logerr("Article too old, moved to junk");
		if (localize("junk"))
			savehist(histline);
		exitcode = 1;
		goto writeout;
	}

	if (is_ctl) {
		control(&header);
		(void) localize("control");
	} else {
		if (s_find(&srec, FULLSYSNAME) == FALSE)
			xerror("Cannot find my name '%s' in %s", FULLSYSNAME, SUBFILE);
#ifdef DOXREFS
		(void) strncpy(nextref, FULLSYSNAME, BUFLEN);
#endif /* DOXREFS */
		for (ptr = nbuf; *ptr;) {
			if (ngmatch(ptr,srec.s_nbuf) || index(ptr,'.') == NULL){
#ifdef DOXREFS
				while (*nextref++)
					;
				(void) sprintf(--nextref, " %s:%ld", ptr, localize(ptr));
#else /* !DOXREFS */
				(void) localize(ptr);
#endif /* !DOXREFS */
			}
			while (*ptr++)
				;
		}
		if (firstbufname[0] == '\0') {
			logerr("Newsgroups in active, but not sys");
			(void) localize("junk");
		}
	}
#ifdef DOXREFS
	if (index(header.nbuf, NGDELIM) == NULL)
		header.xref[0] = '\0';
#endif /* DOXREFS */

writeout:
	if ( (c=getc(infp)) != EOF && (c == ' ' || c == '\t')) {
		header.intnumlines++;
		(void) sprintf(header.numlines, "%d", header.intnumlines);
	}
	lhwrite(&header, tfp);
	/* Kludge to get around article truncation problem */
	if (c != EOF) {
		if (c == ' ' || c == '\t' )
			putc('\n', tfp);
		putc(c, tfp);
	}
	while (fgets(bfr, BUFLEN, infp) != NULL)
		fputs(bfr, tfp);

	if (bfr[strlen(bfr)-1] != '\n')
		putc('\n',tfp);
	(void) fclose(tfp);
	(void) fclose(infp);

	if(exitcode == 0) {
		broadcast();
		savehist(histline);
	}
	xxit(exitcode);
}

input()
{
	register int empty = TRUE;
	register char *cp;
	int c;
	FILE *tmpfp;
	int consec_newlines = 0;
	int linecount = 0;

	tmpfp = xfopen(mktemp(INFILE), "w");
	if (*filename) {
		tty = FALSE;
		infp = xfopen(filename, "r");
	} else {
		infp = stdin;
	}
	while (!SigTrap && fgets(bfr, BUFLEN, stdin) != NULL) {
 		if (mode == PROC) {	/* zap trailing empty lines */
#ifdef ZAPNOTES
			if (empty && bfr[0] == '#' && bfr[2] == ':') {
				(void) strcpy(header.nf_id, bfr);
				(void) nstrip(header.nf_id);
				(void) fgets(bfr, BUFLEN, stdin);
				(void) strcpy(header.nf_from, bfr);
				(void) nstrip(header.nf_from);
				(void) fgets(bfr, BUFLEN, stdin);

				if (header.numlines[0]) {
					header.intnumlines -= 2;
					(void) sprintf(header.numlines, "%d", header.intnumlines);
				}

				/* Strip trailing " - (nf)" */
				if ((cp = rindex(header.title, '-')) != NULL
				    && !strcmp(--cp, " - (nf)"))
					*cp = '\0';
				log("Stripped notes header on %s", header.ident);
				continue;
			}
#endif /* ZAPNOTES */
 			if (bfr[0] == '\n') {	/* 1 empty line, to go */
 				consec_newlines++;	/* count it, in case */
 				continue;		/* but don't write it*/
 			}
 			/* foo! a non-empty line. write out all saved lines. */
 			while (consec_newlines > 0) {
 				putc('\n', tmpfp);
 				consec_newlines--;
				linecount++;
 			}
 		}
		if (mode != PROC && tty && strcmp(bfr, ".\n") == 0)
			break;
		for (cp = bfr; c = *cp; cp++) {
			if (isascii(c) && (isprint(c) || isspace(c) || c=='\b'))
				putc(c, tmpfp);
			if (c == '\n')
				linecount++;
		}
		empty = FALSE;
	}
	if (*filename)
		(void) fclose(infp);

	if (mode != PROC && !is_ctl && header.sender[0] == '\0') {
		(void) sprintf(bfr, "%s/%s", userhome, ".signature");
		if (infp = fopen(bfr, "r")) {
			fprintf(tmpfp, "-- \n");	/* To separate */
			linecount++;
			while ((c = getc(infp)) != EOF) {
				putc(c, tmpfp);
				if (c == '\n')
					linecount++;
			}
			(void) fclose(infp);
		}
	}

	(void) fclose(tmpfp);
	if (SigTrap) {
		if (tty)
			fprintf(stderr, "Interrupt\n");
		if (tty && !empty)
			fwait(fsubr(newssave, (char *) NULL, (char *) NULL));
		if (!tty)
			log("Blown away by an interrupt %d", SigTrap);
		xxit(1);
	}
	if (tty)
		printf("EOT\n");
	fflush(stdout);
	infp = fopen(INFILE, "r");
	if (header.numlines[0]) {
		/*
		 * Check line count if there's already one attached to
		 * the article.  Could make this a fatal error -
		 * throwing it away if it got chopped, in hopes that
		 * another copy will come in later with a correct
		 * line count.  But that seems a bit much for now.
		 */
		if (linecount != header.intnumlines) {
			if (linecount == 0)
				xerror("%s rejected. linecount expected %d, got 0", header.ident, header.intnumlines);
			if (linecount > header.intnumlines ||
			    linecount+consec_newlines < header.intnumlines)
				log("linecount expected %d, got %d", header.intnumlines, linecount+consec_newlines);
		}
		/* adjust count for blank lines we stripped off */
		if (consec_newlines) {
			header.intnumlines -= consec_newlines;
			if (header.intnumlines < 0 )
				header.intnumlines = 0; /* paranoia */
			(void) sprintf(header.numlines, "%d", header.intnumlines);
		}

	} else {
		/* Attach a line count to the article. */
		header.intnumlines = linecount;
		(void) sprintf(header.numlines, "%d", linecount);
	}
}

/*
 * Make the directory for a new newsgroup.  ngname should be the
 * full pathname of the directory.  Do the other stuff too.
 * The various games with setuid and chown are to try to make sure
 * the directory is owned by NEWSUSR and NEWSGRP, which is tough to
 * do if you aren't root.  This will work on a UCB system (which allows
 * setuid(geteuid()) or a USG system (which allows you to give away files
 * you own with chown), otherwise you have to change your kernel to allow
 * one of these things or run with your dirs 777 so that it doesn't matter
 * who owns them.
 */
mknewsg(fulldir, ngname)
char	*fulldir;
char	*ngname;
{
#ifdef USG
	register char *p;
	char parent[200];
	struct stat sbuf;
#endif /* USG */

	if (ngname == NULL || !isalpha(ngname[0]))
		xerror("Tried to make illegal newsgroup %s", ngname);

#ifdef USG
	/*
	 * If the parent is 755 the setuid(getuid)
	 * will fail, and since mkdir is suid, and our real uid is random,
	 * the mkdir will fail.  So we have to temporarily chmod it to 777.
	 */
	(void) strcpy(parent, fulldir);
	while (p = rindex(parent, '/')) {
		*p = '\0';
		if (stat(parent, &sbuf) == 0) {
			(void) chmod(parent, 0777);
			break;
		}
	}
#endif /* USG */

	/* Create the directory */
	mkparents(fulldir);
	if (mkdir(fulldir, 0777) < 0)
		xerror("Cannot mkdir %s: %s", fulldir, errmsg(errno));

#ifdef USG
	(void) chmod(parent, (int)sbuf.st_mode);	/* put it back */
#endif /* USG */

	log("make newsgroup %s in dir %s", ngname, fulldir);
}

/*
 * If any parent directories of this dir don't exist, create them.
 */
mkparents(dname)
char *dname;
{
	char buf[200];
	register char *p;

	(void) strcpy(buf, dname);
	p = rindex(buf, '/');
	if (p)
		*p = '\0';
	if (exists(buf))
		return;
	mkparents(buf);
	if (mkdir(buf, 0777) < 0)
		xerror("Can not mkdir %s: %s", buf, errmsg(errno));
}

cancel()
{
	register FILE *fp;

	log("cancel article %s", filename);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		log("article %s not found", filename);
		return;
	}
	if (hread(&header, fp, TRUE) == NULL)
		xerror("Article is garbled.");
	(void) fclose(fp);
	(void) unlink(filename);
}
