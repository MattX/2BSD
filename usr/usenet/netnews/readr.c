/*
 * Decide what the user wants to read
 * and print it out in the desired format.
 *
 * The user's last read time is updated if the user
 * did not specify any of -n, -t, -l, -r options.
 */

#include "params.h"
#include <sys/timeb.h>
#define FMETA '%'
static char *sccsid = "@(#)readr.c	1.4	12/3/80";

static char *coptfile = "/tmp/newsXXXXXX";

struct artlist {
	char	*a_fname;
	time_t a_date;
};

static struct artlist a[ARTSIZ];	/* article names and dates */
static struct artlist ta;		/* temporary area */
static char artbuf[ARTSIZ*10];		/* buffer to hold filenames */
static char lbuf[BUFLEN*2];

readr(tflag, lflag, pflag, rflag, aflag, nflag, cflag)
int tflag, lflag, pflag, rflag, aflag, nflag, cflag;
{
	register char *bptr;		/* temp pointer.		*/
	register struct artlist *aptr;	/* pointer to artlist		*/
	struct artlist *oaptr;	/* old art pointer.		*/
	register struct artlist *taptr;	/* temp art pointer.		*/
	struct artlist *freep;		/* pointer to next free slot	*/
	struct artlist *squash();
	register int update;	/* true if must update last read time	*/
	int qflag;		/* true if "q" reply given		*/

	time_t rdate;		/* article cutoff date			*/
	time_t maxadate;	/* most recent article selected.	*/
	time_t maxrdate;	/* most recent article read by user.	*/

	struct urec urec;
	struct nrec nrec;
	struct srec srec;

	FILE *ofp;		/* for output of readr			*/
	FILE *fp, *tfp;		/* for use with hread interface		*/
	struct hbuf h;		/* ditto.				*/

	ofp = stdout;
	if (cflag && coptbuf[0] != '\0') {
		umask(022);
		mktemp(coptfile);	/* get "unique" file name */
		ofp = xfopen(coptfile, "w");
		umask(N_UMASK);
		cflag = FALSE;
		pflag = TRUE;
	}

	getdtln();
	rdate = 0L;
	if (aflag && *datebuf != '\0') {
		if ((rdate = getdate(datebuf, (struct timeb *) NULL)) < 0)
			xerror("Cannot parse date string");
		if (rdate > header.ndate)
			xerror("No news of the future yet");
	}
	if (nflag)
		ngfcheck(TRUE);

	/* read uindex for the rest of the default info. */
	u_openr();
	while (u_read(&urec))
		if (urec.u_uid == uid) goto got_it;

	/* new user */
	urec.u_uid = uid;
	urec.u_date = header.ndate;
	ngcat(strcpy(urec.u_nglist, DFLTSUB));

got_it:
	u_close();

	/* if no aflag, use user's last read time */
	if (!aflag)
		rdate = urec.u_date;

	 /* If no nflag, use user's sublist. */
	if (!nflag)
		strcpy(header.nbuf, urec.u_nglist);

	update = !nflag && !tflag && !lflag && !rflag;
	qflag = FALSE;
	maxadate = 0L;
	maxrdate = 0L;

	/* generate the list of articles to read */
one_more_time:
	aptr = &a[0];
	bptr = artbuf;
	n_openr();
	while (n_read(&nrec)) {

		/* check the date. */
		if (nrec.n_date <= rdate)
			break;

		/* check the newsgroups. */
		if (!ngmatch(nrec.n_nglist, header.nbuf))
			continue;

		/* check the title matching. */
		if (tflag && !titmat(nrec.n_file, titlebuf))
			continue;

		/* Found an article. Check if will fit in artbuf */
		if (aptr >= &a[ARTSIZ]
		|| bptr+strlen(nrec.n_file)+1 >= &artbuf[ARTSIZ*10])
			xerror("Too many articles");

		/* Add article to artlist */
		aptr->a_fname = bptr;
		aptr->a_date = nrec.n_date;
		aptr++;
		strcpy(bptr, nrec.n_file);
		while (*bptr++);
		if (nrec.n_date > maxadate)
			maxadate = nrec.n_date;
	}
	freep = aptr;
	n_close();

	if (sigtrap) {
		cdump(ofp);
		return;
	}
	/* if no news, or 'quit' was requested, update the user */
	if (aptr == &a[0] || qflag) {
		if (maxadate == 0L) {
			fprintf(ofp, "No news.\n");
			cdump(ofp);
			return;
		}
		if (update && maxrdate > urec.u_date) {
			urec.u_date = maxrdate;
			settime(&urec, maxrdate == maxadate);
		}
		cout(ofp);
		return;
	}
	rdate = maxadate;

	if (!rflag) {
		aptr = &a[0];
		oaptr = freep;
		while (aptr < oaptr) {
			ta = *aptr;
			*aptr++ = *--oaptr;
			*oaptr = ta;
		}
	}

	/* loop reading articles. */
	fp = NULL;
	oaptr = NULL;
	aptr = &a[0];
	for (;;) {
		if (aptr < &a[0])
			aptr = &a[0];
		if (aptr != oaptr) {
			if (fp != NULL) {
				if (oaptr+1 == aptr && !lflag) {
#ifdef PAGE
					/*
					 * This code filters the tail of long
					 * messages through PAGE.  We only
					 * do it for the msgs interface, since
					 * the mail interface users seem to
					 * want the identical grungy interface.
					 * Should someone feel differently,
					 * the same change should be made to
					 * the tprint call a few lines down.
					 */
					if (linecnt(fp) > 17) {
						FILE *pfp, *popen();
						pfp = popen(PAGE, "w");
						if (pfp == NULL)
							pfp = ofp;
						tprint(fp, pfp);
						pclose(pfp);
					} else
#endif
						tprint(fp, ofp);
					if (oaptr->a_date > maxrdate)
						maxrdate = oaptr->a_date;
				}
				fclose(fp);
				fp = NULL;
			}
			if (aptr >= freep)
				break;
			if ((fp = hread(&h, NEWSD, aptr->a_fname)) == NULL) {
				freep = squash(aptr, freep);
				continue;
			}
			hprint(&h, cflag, lflag, pflag, ofp);
			if (!cflag && !lflag) {
				tprint(fp, ofp);
				if (aptr->a_date > maxrdate)
					maxrdate = aptr->a_date;
				fclose(fp);
				fp = NULL;
			}
			oaptr = aptr;
		}
		if (lflag || pflag) {
			if (sigtrap) {
				qfflush(ofp);
				fprintf(ofp, "\n");
				cdump(ofp);
				_exit(0); /* kludge! drop when qfflush works */
				return;
			}
			aptr++;
			continue;
		}
		for (;;) {
			sigtrap = FALSE;
			if (cflag)
				fprintf(ofp, "(%d lines) More? [ynq] ", linecnt(fp));
			else
				fprintf(ofp, "? ");
			fflush(ofp);
			bptr = lbuf;
			if (fgets(bptr, BUFLEN, stdin) != NULL)
				break;
			if (!sigtrap)
				return;
			fprintf(ofp, "\n");
		}
		nstrip(bptr);
		while (*bptr == ' ' || *bptr == '\t')
			bptr++;
		switch (*bptr++) {

		/*
		 * Get next article, or delete.
		 * This can also be thought of as "no" for the -c case.
		 */
		case 'n':
		case 'd':
			if (*bptr != '\0') goto badropt;
			if (cflag) {
				if (aptr->a_date > maxrdate)
					maxrdate = aptr->a_date;
				fclose(fp);
				fp = NULL;
			}
			fprintf(ofp, "\n");
			aptr++;
			break;

		/* yes, \n: print this article, go on. */
		case 'y':
			if (*bptr != '\0') goto badropt;
		case '\0':
			aptr++;
			break;

		/* reprint the article */
		case 'p':
			if (*bptr != '\0') goto badropt;
			oaptr = NULL;
			break;

		/* write out the article someplace */
		case 'w':
			taptr = aptr;
			if (*bptr == '-') {
				bptr++;
				if (taptr > &a[0])
					taptr--;
			}
			if (*bptr != '\0' && *bptr != ' ') {
				fprintf(ofp, "Bad file name.\n");
				break;
			}
			while (*bptr == ' ')
				bptr++;
			if (*bptr == '\0') {
				fprintf(ofp, "Missing file name.\n");
				break;
			}
			fwait(fsubr(save, taptr->a_fname, bptr));
			break;

		/* back up some distance. */
		case '-':
		/* skip forwards */
		case '+':
			if (*bptr == '\0')
				strcat(bptr, "1");
			aptr += atoi(bptr-1);
			oaptr = NULL;
			break;

		/* exit - time updated to that of most recently read article */
		case 'q':
			if (*bptr != '\0') goto badropt;
			qflag = TRUE;
			if (update)
				goto one_more_time;
			return;

		/* exit - no time update. */
		case 'x':
			if (*bptr != '\0') goto badropt;
			return;

		/* cancel the article. */
		case 'c':
			if (*bptr != '\0') goto badropt;
			if (cancel(aptr->a_fname)) {
				freep = squash(aptr, freep);
				oaptr = NULL;
			}
			break;

		/* escape to shell */
		case '!':
			fwait(fsubr(ushell, bptr, (char *)NULL));
			fprintf(ofp, "!\n");
			break;

		/* mail reply */
		case 'r':
			taptr = aptr;
			if (*bptr == '-') {
				bptr++;
				if (taptr > &a[0])
					taptr--;
			}
			if (*bptr != '\0') goto badropt;
			if ((tfp = hread(&h, NEWSD, taptr->a_fname)) == NULL) {
				printf("Article is gone\n");
				break;
			}
			fclose(tfp);
			sprintf(bfr, "mail %s", h.path);
			fprintf(ofp, "%s\n", bfr);
			fflush(ofp);
			fwait(fsubr(ushell, bfr, (char *)NULL));
			break;

		/* send to some system */
		case 'X':
			if (*bptr != '\0' && *bptr != ' ') {
				fprintf(ofp, "Bad system name.\n");
				break;
			}
			while (*bptr == ' ')
				bptr++;
			if (*bptr == '\0') {
				fprintf(ofp, "Missing system name.\n");
				break;
			}
			if (s_find(&srec, bptr) == NULL) {
				fprintf(ofp, "%s not in SYSFILE\n", bptr);
				break;
			}
			transmit(&srec, NEWSD, aptr->a_fname);
			break;

		/* error */
		case '?':
		default:
		badropt:
			fprintf(ofp, "(n)ext re(p)rint (w)rite (q)uit (r)eply\
 (c)ancel -[n] +[n]\n");
			break;
		}
	}

	/*
	 * Done with this batch of articles.
	 * Now pick up any that have been submitted
	 * while we were reading these.
	 */
	if (update)
		goto one_more_time;
	cout(ofp);
}

/*
 * Set the user's last read time.
 * If flag is TRUE, also set user 'up to date' in BITFILE.
 */
settime(up, flag)
register struct urec *up;
int flag;
{
	register int bitfd;
	struct urec urec;
	static char c;
	off_t lseek();

	lock();
	u_openm();
	u_write(up);
	while (u_read(&urec))
		if (uid != urec.u_uid)
			u_write(&urec);
	u_close();
	if (flag) {
		/*
		 * To improve efficiency,
		 * and since stdio does not (officially)
		 * support both read and write i/o on the same stream,
		 * the UNIX open/lseek/read/write/close system calls
		 * are used here.
		 */
		if ((bitfd = open(BITFILE, 2)) < 0)
			xerror("Cannot open BITFILE for update.");
		lseek(bitfd, (long)(uid >>3), 0);
		read(bitfd, &c, 1);
		c &= ~(1 << (uid & 07));
		lseek(bitfd, (long)(uid >>3), 0);
		write(bitfd, &c, 1);
		close(bitfd);
	}
	unlock();
}

/*
 * Match title.
 */
titmat(file, titlist)
register char *file, *titlist;
{
	struct hbuf h;
	register char *p;
	register FILE *fp;
	register int titlen;

	if ((fp = hread(&h, NEWSD, file)) == NULL)
		return(FALSE);
	fclose(fp);

	while (*titlist != '\0') {
		titlen = strlen(titlist);
		for (p = h.title; *p != '\0'; p++)
			if (strncmp(p, titlist, titlen) == 0)
				return(TRUE);
		titlist += titlen+1;
	}
	return(FALSE);
}

/*
 * Save the news item in the user's file.
 */
save(file, to)
register char *file, *to;
{
	register FILE *ufp, *hfp;
	struct hbuf h;

	if ((hfp = hread(&h, NEWSD, file)) == NULL) {
		printf("Article is gone\n");
		return;
	}
	setgid(gid);
	setuid(uid);
	umask(savmask);
	if ((ufp = fopen(to, "a")) == NULL) {
		printf("Cannot append to %s\n", to);
		return;
	}
	hprint(&h, TRUE, FALSE, FALSE, ufp);
	tprint(hfp, ufp);
	fclose(hfp);
	fclose(ufp);
}

/*
 * Print out the rest of the article.
 */
tprint(ifp, ofp)
register FILE *ifp, *ofp;
{
	register int c;

	while ((c = getc(ifp)) != EOF && !sigtrap)
		putc(c, ofp);
	if (sigtrap)
		qfflush(ofp);
	fflush(ofp);
	fprintf(ofp, (sigtrap? "\n\n" : "\n"));
}

/*
 * Print the file header.
 */
hprint(hp, cflag, lflag, pflag, ofp)
register struct hbuf *hp;
int cflag, lflag, pflag;	/* sigh.. */
register FILE *ofp;
{
	if (cflag || pflag || !lflag)
		fprintf(ofp, "From %s %s\n", hp->path, hp->date);
	ngdel(strcpy(bfr, hp->nbuf));
	if (cflag || pflag)
		fprintf(ofp, "Subject: %s\nNewsgroups: %s\n", hp->title, bfr);
	else
		fprintf(ofp, "%-20s: %s\n", hp->title, bfr);
}

/*
 * Remove artlist item pointed to by "aptr" and compact the artlist.
 * Return the new freepointer.
 */
struct artlist *
squash(aptr, freep)
register struct artlist *aptr, *freep;
{
	register struct artlist *tptr;

	tptr = aptr+1;
	while (tptr < freep)
		*aptr++ = *tptr++;
	return(freep-1);
}

/*
 * If ofp != stdout, close it and run the script in coptbuf.
 */
cout(ofp)
FILE *ofp;
{
	register char *p, *q, *r;

	if (ofp == stdout)
		return;
	fclose(ofp);
	p = coptbuf;
	q = lbuf;
	while ((*q = *p++) != '\0')
		if (*q++ == FMETA) {
			q--;
			r = coptfile;
			while ((*q++ = *r++) != '\0')
				;
			q--;
		}
	fwait(fsubr(ushell, lbuf, (char *)NULL));
	unlink(coptfile);
}

cdump(ofp)
register FILE *ofp;
{
	if (ofp == stdout)
		return;
	fclose(ofp);
	unlink(coptfile);
}

/*
 * Quiet 'flush'.
 * Empty (without fflush()) the buffer for stream fp.
 */
/* ARGSUSED */
qfflush(fp)
FILE *fp;
{
	/* Alas, stdio does not permit this */
}

/*
 * Count the number of remaining lines in file fp.
 * Do not move the file pointer.
 */
linecnt(fp)
FILE *fp;
{
	long curpos;
	register int nlines = 0;
	register int c;

	if (fp == NULL)
		return 0;
	curpos = ftell(fp);
	while ((c = getc(fp)) != EOF)
		if (c == '\n')
			nlines++;
	fseek(fp, curpos, 0);
	return nlines;
}
