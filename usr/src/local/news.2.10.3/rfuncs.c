/*
 * rfuncs - functions for readnews.
 */

#ifdef SCCSID
static char	*SccsId = "@(#)rfuncs.c	2.25	4/16/85";
#endif /* SCCSID */

/*LINTLIBRARY*/

#include "rparams.h"

long nngsize;	/* The next upcoming value of ngsize. */
long nminartno;	/* Smallest article number in this group */
int BITMAPSIZE = 0;

nextng()
{
	long	curpos;
#ifdef DEBUG
	fprintf(stderr, "nextng()\n");
#endif
	curpos = ftell(actfp);

next:
#ifdef DEBUG
	fprintf(stderr, "next:\n");
#endif
	if (actdirect == BACKWARD) {
		if (back()) {
			(void) fseek(actfp, curpos, 0);
			return 1;
		}
		if (back()) {
			(void) fseek(actfp, curpos, 0);
			return 1;
		}
	}
	if (fgets(afline, BUFLEN, actfp) == NULL)
		return 1;
	if (sscanf(afline, "%s %ld %ld", bfr, &nngsize, &nminartno) < 3) {
		bfr[0] = '\0';
		nngsize = 0;
		nminartno = 0;
	}
#ifdef DEBUG
	fprintf(stderr, "bfr = '%s'\n", bfr);
#endif

	if (!ngmatch(bfr, header.nbuf))
		goto next;
	if (xflag)
		readmode = SPEC;
	else
		readmode = NEXT;
	if (selectng(bfr, TRUE, FALSE))
		goto next;
	return 0;
}


selectng(name, fastcheck, resubscribe)
char	*name;
{
	register char	*ptr, punct = ',';
	register int	i;
	register char	*p;
	register long	cur;
	long	next = 0;
	FILE *af;
	long s, sm;
	char buf[100], n[100];

#ifdef DEBUG
	fprintf(stderr,"selectng: groupdir = %s\n", groupdir);
#endif /* DEBUG */
	if (*groupdir)
		updaterc();
	last = 1;
	if (strcmp(name, bfr)) {
		af = xfopen(ACTIVE, "r");
		while (fgets(buf, sizeof buf, af) != NULL) {
			if (sscanf(buf, "%s %ld %ld", n, &s, &sm) == 3 &&
			     strcmp(n, name) == 0) {
				ngsize = s;
				minartno = sm;
				break;
			}
		}
		(void) fclose(af);
	} else {
		ngsize = nngsize;
		minartno = nminartno;
	}
#ifdef DEBUG
	fprintf(stderr, "selectng(%s) sets ngsize to %ld, minartno to %ld\n",
		name, ngsize, minartno);
#endif
	(void) strcpy(groupdir, name);
	if (!xflag) {
		i = findrcline(name);
		if (i >= 0) {
			if (p = index(rcline[i], '!')) {
				switch (resubscribe) {
				case FALSE:
					groupdir[0] = 0;
					return 1;
				case TRUE:
					*p = ':';
					break;
				case PERHAPS:
					zapng = TRUE;
					break;
				}
			} else
				p = index(rcline[i], ':');
			if (!p) /* shouldn't happen */
				p = rcline[i];
			while (*++p == ' ')
				;
			(void) sprintf(rcbuf, "%s%s%ld", rcline[i],
				*p == '\0' ? " " : ",", ngsize+1);
		}
		else
			(void) sprintf(rcbuf, "ng: %ld", ngsize+1);
	} else
		(void) sprintf(rcbuf, "ng: %ld", ngsize+1);
#ifdef DEBUG
	fprintf(stderr, "rcbuf set to %s\n", rcbuf);
#endif /* DEBUG */

	/*
	 * Fast check for common case: 1-###
	 */
	if (fastcheck) {
		p = rcbuf;
		while (*p != ' ')
			p++;
		while (*p == ' ')
			p++;
		if (*p++ == '1' && *p++ == '-') {
			cur = 0;
			while (isdigit(*p))
				cur = 10 * cur + *p++ - '0';
			if (*p == ',' && cur == ngsize) {
#ifdef DEBUG
				fprintf(stderr, "Group: %s, all read\n", groupdir);
#endif
				groupdir[0] = 0;
				return 1;
			}
			if (cur > ngsize) {
				/*
				 * Claim to have read articles
				 * which "active" believes have
				 * never existed - we believe "active"
				 */
				fprintf(stderr,
					"%s %s...\n\t%s %ld to %ld\n",
					"Warning: newsgroup", groupdir,
					"last article claimed read reset from",
					cur, ngsize);
			}
		}
	}

/*
 * The key to understanding this piece of code is that a bit is set iff
 * that article has NOT been read.  Thus, we fill in the holes when
 * commas are found (e.g. 1-20,30-35 will result in filling in the 21-29
 * holes), and so we assume the newsrc file is properly ordered, the way
 * we write it out.
 */
	if ((ngsize-minartno) > BITMAPSIZE) {
		/* This should never happen */
		(void) xerror(" Bitmap not large enough for newsgroup %s", groupdir);
	}

	cur = 0;
	/* Zero out the bitmap */
	p = &bitmap[(ngsize-minartno)/8+1];
	for (ptr = bitmap; ptr <= p;)
		*ptr++ = 0;

	/* Decode the .newsrc line indicating what we have read. */
	for (ptr = rcbuf; *ptr && *ptr != ':'; ptr++)
		;
	while (*ptr) {
		while (!isdigit(*ptr) && *ptr)
			ptr++;
		if (!*ptr)
			break;
		(void) sscanf(ptr, "%ld", &next);
		if (punct == ',') {
			while (++cur < next) {
				set(cur);
			}
		}
		cur = next;
		while (!ispunct(*ptr) && *ptr)
			ptr++;
		punct = *ptr;
	}
	if (rflag)
		bit = ngsize+1;
	else
		bit = minartno -1;
	nextbit();
	ngrp = 1;
	return 0;
}

#ifdef TMAIL
catchterm()
{
	(void) unlink(infile);
	(void) unlink(outfile);
	xxit(0);
}


/*
 * The -M (Mail) interface.  This code is a reasonably simple model for
 * writing other interfaces.  We write out all relevant articles to
 * a temp file, then invoke Mail with an option to have it tell us which
 * articles it read.  Finally we count those articles as really read.
 */
Mail()
{
	register FILE *fp = NULL, *ofp;
	struct hbuf h;
	register char	*ptr, *fname;
	int	news = 0;
	register int i;

	for(i=0;i<NUNREC;i++)
		h.unrec[i] = NULL;

	ofp = xfopen(mktemp(outfile), "w");
	if (aflag && *datebuf)
		if ((atime = cgtdate(datebuf)) == -1)
			xerror("Cannot parse date string");
	while (!nextng())
		while (bit <= ngsize) {
			(void) sprintf(filename, "%s/%ld", dirname(groupdir), bit);
			if (access(filename, 4)
			|| ((fp = fopen(filename, "r")) == NULL)
			|| (hread(&h, fp, TRUE) == NULL)
			|| !aselect(&h, FALSE)) {
#ifdef DEBUG
				fprintf(stderr, "Bad article '%s'\n", filename);
#endif
				if (fp != NULL) {
					(void) fclose(fp);
					fp = NULL;
				}
				clear(bit);
				nextbit();
				continue;
			}
			fname = ptr = index(h.from, '(');
			if (fname) {
				while (ptr && ptr[-1] == ' ')
					ptr--;
				if (ptr)
					*ptr = 0;
				fname++;
				ptr = fname + strlen(fname) - 1;
				if (*ptr == ')')
					*ptr = 0;
			}
			h.subtime = cgtdate(h.subdate);
			fprintf(ofp, "From %s %s",
#ifdef INTERNET
			    h.from[0] ? h.from :
#endif
			    h.path, ctime(&h.subtime));
			if (fname)
				fprintf(ofp, "Full-Name: %s\n", fname);
			fprintf(ofp, "Newsgroups: %s\n", h.nbuf);
			fprintf(ofp, "Subject: %s\n", h.title);
			fprintf(ofp, "Article-ID: %s/%ld\n\n", groupdir, bit);
			tprint(fp, ofp, TRUE);
			putc('\n', ofp);
			news = TRUE;
			(void) fclose(fp);
			fp = NULL;
			nextbit();
		}
	updaterc();
	(void) fclose(ofp);
	if (!news) {
		if (!checkngs())
			fprintf(stderr, "No news.\n");
		(void) unlink(outfile);
		return;
	}
	(void) signal(SIGHUP, catchterm);
	(void) signal(SIGTERM, catchterm);
	(void) sprintf(bfr, "%s -f %s -T %s", TMAIL, outfile, mktemp(infile));
	fwait(fsubr(ushell, bfr, (char *)NULL));
	ofp = xfopen(infile, "r");
	(void) fseek(actfp, 0L, 0);
	while (fgets(afline, BUFLEN, actfp) != NULL) {
		last = 0;
		if (sscanf(afline, "%s %ld", bfr, &nngsize) < 2) {
			bfr[0] = '\0';
			nngsize = 0;
		}
		if (!ngmatch(bfr, header.nbuf))
			continue;
		*groupdir = 0;
		if (selectng(bfr, TRUE, FALSE))
			continue;
		(void) fseek(ofp, 0L, 0);
		while (fgets(groupdir, BUFLEN, ofp) != NULL) {
			(void) nstrip(groupdir);
			ptr = index(groupdir, '/');
			*ptr = 0;
			if (strcmp(bfr, groupdir))
				continue;
			(void) sscanf(++ptr, "%ld", &last);
			clear(last);
		}
		if (last) {
			(void) strcpy(groupdir, bfr);
			updaterc();
		}
	}
	(void) unlink(infile);
	(void) unlink(outfile);
}
#endif

updaterc()
{
	register long	cur = 1, next = 1;
	register int i;
	register char	*ptr;
	char	oldptr;

	sprintf(rcbuf, "%s%c ", groupdir, zapng ? '!' : ':');

	zapng = FALSE;
again:
	ptr = &rcbuf[strlen(rcbuf)];
	while (get(next) && next <= ngsize)
		next++;
	cur = next;
	while (!(get(next)) && next <= ngsize)
		next++;
	if (cur == next) {
		next = ngsize + 1;
		goto skip;
	}
	if (ptr[-1] != ' ')
		*ptr++ = ',';
	if (cur + 1 == next)
		(void) sprintf(ptr, "%ld", cur);
	else
		(void) sprintf(ptr, "%ld-%ld", cur, next - 1);
skip:
	if ((long) next > ngsize) {
		if (strpbrk(rcbuf, ":!") == NULL)	/* bad line, huh?? */
			return;
		ptr = index(rcbuf, ' ');
		if (ptr == NULL)			/* impossible */
			return;
		ptr--;
		oldptr = *ptr;
		ptr[0] = ':';
		ptr[1] = '\0';
		i = findrcline(groupdir);
		if (i >= 0) {
			ptr[0] = oldptr;
			ptr[1] = ' ';
			rcline[i] = realloc(rcline[i], (unsigned)(strlen(rcbuf) + 1));
			if (rcline[i] == NULL)
				xerror("Cannot realloc");
			(void) strcpy(rcline[i], rcbuf);
#ifdef DEBUG
			fprintf(stderr," new rcline = %s\n", rcline[i]);
#endif /* DEBUG */
			return;
		}
		if (++line > LINES)
			xerror("Too many newsgroups");
		ptr[0] = oldptr;
		ptr[1] = ' ';
		if ((rcline[line] = malloc((unsigned)(strlen(rcbuf) + 1))) == NULL)
			xerror("Not enough memory");
		(void) strcpy(rcline[line], rcbuf);
#ifdef DEBUG
		fprintf(stderr," new rcline2 = %s\n", rcline[line]);
#endif /* DEBUG */
		return;
	}
	cur = next;
	goto again;
}

newrc(rcname)
char *rcname;
{
	register FILE *fp;

	if (close(creat(rcname, 0666))) {
		(void) sprintf(bfr, "Cannot create %s", newsrc);
		xerror(bfr);
	}

	sprintf(bfr, "%s/users", LIB);
	if ((fp = fopen(bfr, "a")) != NULL) {
		fprintf(fp, "%s\n", username);
		(void) fclose(fp);
		(void) chmod(bfr, 0666);
	}
}

nextbit()
{
#ifdef DEBUG
	fprintf(stderr,"nextbit() bit = %ld\n", bit);
#endif /* DEBUG */
	last = bit;
	if (readmode == SPEC || xflag) {
		if (rflag)
			bit--;
		else
			bit++;
		return;
	}
	if (rflag)
		while (--bit, !get(bit) && bit > minartno)
			;
	else
		while (++bit, !get(bit) && bit <= ngsize)
			;
#ifdef DEBUG
	fprintf(stderr,"nextng leaves bit as %ld\n", bit);
#endif /* DEBUG */
}

/*
 * Return TRUE if the user has not ruled out this article.
 */
aselect(hp, insist)
register struct hbuf *hp;
int	insist;
{
	if (insist)
		return TRUE;
	if (tflag && !titmat(hp, header.title))
		return FALSE;
	if (aflag && cgtdate(hp->recdate) < atime)
		return FALSE;
	if (index(hp->nbuf, ',') && !rightgroup(hp))
		return FALSE;
	if (fflag && isfol(hp))
		return FALSE;
	return TRUE;
}

/*
 * Code to avoid showing multiple articles for news.
 * Works even if you exit news.
 * Returns nonzero if we should show this article.
 */
rightgroup(hp)
struct hbuf *hp;
{
	char ng[BUFLEN];
	register char *p, *g;
	int i, flag;

	strcpy(ng, hp->nbuf);
	g = ng;
	flag = 1;
	while (g != NULL) {
		p = index(g, ',');
		if (p != NULL) {
			*p++ = '\0';
			while (*p == ' ')
				p++;
		}
		if (strcmp(g, groupdir) == 0)
			return flag;
		if (ngmatch(g, header.nbuf)
		    && ((i = findrcline(g)) >= 0
		    && index(rcline[i], '!') == NULL))
			flag = 0;
		g = p;
	}
	/* we must be in "junk" or "control" */
	return TRUE;
}

/*
 * Return TRUE if this article is a followup to something.
 */
isfol(hp)
register struct hbuf *hp;
{
	if (hp->followid[0])
		return TRUE;
	if (strncmp(hp->title, "Re:", 3) == 0)
		return TRUE;
	return FALSE;
}

back()
{
	while (fseek(actfp, -2L, 1) != -1 && ftell(actfp) > 0L) {
		if (getc(actfp) == '\n')
			return 0;
	}
	if (ftell(actfp) == 0L)
		return 0;
	return 1;
}

/*
 * Trap interrupts.
 */
onsig(n)
int	n;
{
	(void) signal(n, onsig);
	SigTrap = n;
	if (rcreadok < 2) {
		fprintf(stderr, "Aborted early\n");
		xxit(0);
	}
}

/*
 * finds the line in your .newsrc file (actually the in-core "rcline"
 * copy of it) and returns the index into the array where it was found.
 * -1 means it didn't find it.
 *
 * We play clever games here to make this faster.  It's inherently
 * quadratic - we spend lots of CPU time here because we search through
 * the whole .newsrc for each line.  The "prev" variable remembers where
 * the last match was found; we start the search there and loop around
 * to the beginning, in the hopes that the calls will be roughly in order.
 */
int
findrcline(name)
register char *name;
{
	register char *	p;
	register int	i;
	register int	top;
	register int	len;
	static int	prev;
	static char	lentab[LINES];
	static int	didthru;

	for ( ; didthru <= line; ++didthru)
		if ((p = index(rcline[didthru], '!')) != 0 ||
			(p = index(rcline[didthru], ':')) != 0) {
				lentab[didthru] = (int)(p - rcline[didthru]);
		}
	len = strlen(name);
	top = line;
	i = prev;
loop:
	for ( ; i <= top; ++i)
		if (lentab[i] == len && strncmp(name, rcline[i], len) == 0)
			return prev = i;
	if (i > line && line > prev - 1) {
		i = 0;
		top = prev - 1;
		goto loop;
	}
	return -1;
}

#ifdef SORTACTIVE
/*
 * sortactive - make a local copy of the active file, sorted according
 *   to the user's preferences, according to his .newsrc file.
 */

struct table_elt {
	int	rcindex;
	long	maxart, minart;
	char	yn;
};

static int
rcsort(a, b)
char *a, *b;
{
	return(((struct table_elt *)a)->rcindex -
	       ((struct table_elt *)b)->rcindex);
}

static char *newactivename = "/tmp/newsaXXXXXX";
#endif /* SORTACTIVE */

sortactive()
{
	register struct table_elt *tp;
	register char *p;
	register FILE *nfp, *afp;
	char aline[BUFLEN], ngname[BUFLEN];
	struct table_elt table[LINES];
	int nlines = 0, i, delta, lastline;

#ifdef SORTACTIVE
	/* make a new sorted copy of ACTIVE */
	nfp = fopen(mktemp(newactivename), "w");
	(void) chmod(newactivename, 0600);
	if (nfp == NULL) {
		perror(newactivename);
		return;
	}

	/* look up all the lines in ACTIVE, finding their positions in .newsrc */
	p = ACTIVE;
	ACTIVE = newactivename;
	afp = xfopen(p, "r");
	tp = table;
#else /* !SORTACTIVE */
	afp = xfopen(ACTIVE, "r");
#endif /* !SORTACTIVE */
	while (fgets(aline, sizeof aline, afp) != NULL) {
		if (sscanf(aline,"%s %ld %ld %c", ngname, &tp->maxart, &tp->minart, &tp->yn) != 4) 
			xerror("Active file corrupt");
		delta = tp->maxart - tp->minart;
		if (delta > BITMAPSIZE)
			BITMAPSIZE = delta;
#ifdef SORTACTIVE
		tp->rcindex = findrcline(ngname);
		if (tp->rcindex < 0) {
			register FILE *f;
			/* it's not in his .newsrc, maybe it's aliased? */
			f = xfopen(ALIASES,"r");
			while (fscanf(f,"%s %s", afline, bfr) == 2
				&& strcmp(ngname, bfr))
				;
			(void) fclose(f);
			if (strcmp(ngname, bfr) == 0) {
				int j;
				j = findrcline(afline);
				if (j >= 0) {
					p = rcline[j];
					while (*p != ':' && *p != '!' && *p)
						p++;
					strcat(bfr, p);
					rcline[j] = realloc(rcline[j], (unsigned)(strlen(bfr)+1));
					if (rcline[j] == NULL)
						xerror("Not enough memory");
					strcpy(rcline[j], bfr);
					tp++->rcindex = j;
					continue;
				}
			}
			if (++line > LINES)
				xerror("Too many newsgroups");
			strcat(ngname, ":");
			rcline[line] = malloc((unsigned)(strlen(ngname) + 1));
			if (rcline[line] == NULL)
				xerror("Not enough memory");
			strcpy(rcline[line], ngname);
			tp->rcindex = line;
		}
		tp++;
#endif /* SORTACTIVE */
	}
	(void) fclose(afp);
	BITMAPSIZE =  8 * ((BITMAPSIZE+7) / 8);
	bitmap = malloc((unsigned)BITMAPSIZE/8);
	if (bitmap == NULL)
		xerror("Can't malloc bitmap");

#ifdef SORTACTIVE
	/* sort by position in user's .newsrc file (new groups come up last) */
	nlines = tp - table;
	qsort((char *)table, nlines, sizeof table[0], rcsort);

	tp = table;
	lastline = tp->rcindex - 1;
	/* copy active to newactive, in the new order */
	for (i = 0; i < nlines; i++) {
		while (++lastline < tp->rcindex) {
			free(rcline[lastline]);
			rcline[lastline] = NULL;
		}
		if (rcline[tp->rcindex] == NULL)
			continue;
		p = rcline[tp->rcindex];
		while (*p != ':' && *p != '!')
			fputc(*p++, nfp);
		(void) fprintf(nfp, " %ld %ld %c\n", tp->maxart, tp->minart,
			tp->yn);
		tp++;
	}
	(void) fclose(nfp);
#endif /* SORTACTIVE */
}

/* ARGSUSED */
checkngs(nbuf, f)
char *nbuf;
FILE *f;
{
	return 0;
}

#if 0
/*
 * Check that nbuf contains only valid newsgroup names;
 * fd refers to the ACTIVE file.  Print messages for any invalid ones.
 */

checkngs(nbuf, f)
	char *nbuf;
	register FILE *f;
{
	register char *s1, *s2;
	char ngcheck[AFSIZ], bfr[BUFLEN];
	int inval = 0;

	rewind(f);
	s1 = ngcheck;
	while (fgets(bfr, BUFLEN, f) != NULL) {
		for(s2 = bfr; *s2 != '\0' && *s2 != ' ';) {
			if (s1 >= &ngcheck[AFSIZ-2])
				xerror("ACTIVE file too long");
			*s1++ = *s2++;
		}
		*s1++ = '\0';
	}
	*s1++ = '\0';
	*s1 = '\0';

	s1 = nbuf;
	s2 = s1;
	while (*s1 == NGDELIM || *s1 == ' ' || *s1 == NEGCHAR)
		s1++;	/* skip leading junk */
	do {
		/* there shouldn't be blanks, but give the jerk a break */
		if (*s1 == NGDELIM || *s1 == ' ') {
			*s2++ = '\0';
			while (*++s1 == NGDELIM || *s1 == ' ' || *s1 == NEGCHAR)
				;	/* remove extra commas */
		} else
			*s2++ = *s1++;
	} while (*s1 != '\0');
	*s2++ = '\0';
	*s2 = '\0';

	/*
	 * here follows a revolting O(n^2) search ...
	 */
	s1 = nbuf;
	while (*s1 != '\0') {	/* for each newsgroup in list */
		s2 = ngcheck;
		while (*s2 != '\0') { /* for each newsgroup in active file */
			if (ngmatch(s2, s1))
				break;
			while (*s2++ != '\0')
				;
		}
		if (*s2 == '\0') {	/* not found. remove it */
			if (!inval)
				fprintf(stderr, "No news group: ");
			else
				fprintf(stderr, ", ");
			fprintf(stderr, "%s", s1);
			inval++;
		}
		while (*s1++ != '\0')
			;
	}
	if (inval)
		fprintf(stderr, "\n");
	return (inval);
}
#endif
