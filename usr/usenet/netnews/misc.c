#include "params.h"
static char *sccsid = "@(#)misc.c	1.5	12/3/80";

#define DEADTIME 60	/* number of seconds to wait on dead lock. */
#define	NEGCHAR	'!'	/* negation character for newsgroups */

/*
 * Print out an error message and exit.
 */
xerror(ptr)
char *ptr;
{
	fflush(stdout);
	fprintf(stderr, "news: %s.\n", ptr);
	xxit(1);
}

/*
 * Newsystem locking.
 */
static	int	lockcount = 0;	/* number of times we've called lock. */

lock()
{
	register int i;

	if (lockcount++ == 0) {
		i = DEADTIME;
		while (link(UINDEX, LOCKFILE)) {
			if (--i < 0)
				xerror("News system locked up");
			sleep((unsigned)1);
		}
	}
}

unlock()
{
	if (--lockcount == 0)
		unlink(LOCKFILE);
}

/*
 * Trap interrupts.
 */
onsig(n)
int n;
{
	/*
	 * Most UNIX systems reset caught signals to SIG_DFL.
	 * This bad design requires that the trap be set again here.
	 * Unfortunately, if the signal recurs before the trap is set,
	 * the program will die, possibly leaving the lock in place.
	 */
	signal(n, onsig);
	sigtrap = TRUE;
}

/*
 * Strip trailing newlines, blanks, and tabs from 's'.
 * Return TRUE if newline was found, else FALSE.
 */
nstrip(s)
register char *s;
{
	register char *p;
	register int rc;

	rc = FALSE;
	p = s;
	while (*p)
		if (*p++ == '\n')
			rc = TRUE;
	while (--p >= s && (*p == '\n' || *p == ' ' || *p == '\t'));
	*++p = '\0';
	return(rc);
}

/*
 * Delete trailing NGDELIM.
 */
ngdel(s)
register char *s;
{
	if (*s++) {
		while (*s++);
		s -= 2;
		if (*s == NGDELIM)
			*s = '\0';
	}
}

/*
 * Append NGDELIM to string.
 */
ngcat(s)
register char *s;
{
	if (*s) {
		while (*s++);
		s -= 2;
		if (*s++ == NGDELIM)
			return;
	}
	*s++ = NGDELIM;
	*s = '\0';
}

/*
 * Check if header.nbuf contains only valid newsgroup names;
 * exit with error if not valid.
 *
 * a == TRUE means header.nbuf is subscription list
 * a == FALSE means header.nbuf is newsgroup list
 */
#define	NGFSIZ	1000

ngfcheck(a)
int a;
{
	char ngcheck[NGFSIZ];	/* Hold NGFILE newsgroups */
	char tbuf[BUFLEN];	/* hold single header.nbuf news group */
	register char *s1, *s2;
	register FILE *f;

	s1 = ngcheck;
	f = xfopen(NGFILE, "r");
	while (fgets(bfr, BUFLEN, f) != NULL) {
		for (s2 = bfr; *s2 != '\0' &&
		    *s2 != ' ' && *s2 != '\t' &&
		    *s2 != ':' && *s2 != '\n';) {
			if (s1 >= &ngcheck[NGFSIZ-2])
				xerror("NGFILE too long");
			*s1++ = *s2++;
		}
		*s1++ = NGDELIM;
	}
	*s1 = '\0';
	fclose(f);
	for (s1 = header.nbuf; *s1 != '\0';) {
		if (*s1 == NEGCHAR)
			s1++;
		s2 = tbuf;
		while ((*s2++ = *s1++) != NGDELIM)
			if (s1[-1] == ':')
				xerror("Newsgroup cannot contain ':'");
		*s2 = '\0';
		s2 = tbuf;
		if (!ngmatch(s2, ngcheck) && (!a || !ngmatch(ngcheck, s2))) {
			ngdel(s2);
			sprintf(bfr, "Bad news group \"%s\"", s2);
			xerror(bfr);
		}
	}
}

/*
 * News group matching.
 *
 * nglist is a list of newsgroups.
 * sublist is a list of subscriptions.
 * sublist may have "meta newsgroups" in it.
 * All fields are NGDELIM separated,
 * and there is an NGDELIM at the end of each argument.
 *
 * Currently implemented glitches:
 * sublist uses 'ALL' like shell uses '*', and '.' like shell '/'.
 * If subscription X matches Y, it also matches Y.anything.
 */
ngmatch(nglist, sublist)
register char *nglist, *sublist;
{
	register char *n, *s;
	register int rc;

	rc = FALSE;
	for (n = nglist; *n != '\0' && rc == FALSE;) {
		for (s = sublist; *s != '\0';) {
			if (*s != NEGCHAR)
				rc |= ptrncmp(s, n);
			else
				rc &= ~ptrncmp(s+1, n);
			while (*s++ != NGDELIM);
		}
		while (*n++ != NGDELIM);
	}
	return(rc);
}

/*
 * Compare two newsgroups for equality.
 * The first one may be a "meta" newsgroup.
 */
ptrncmp(ng1, ng2)
register char *ng1, *ng2;
{
	while (*ng1 != NGDELIM) {
		if (ng1[0]=='A' && ng1[1]=='L' && ng1[2]=='L') {
			ng1 += 3;
			while (*ng2 != NGDELIM && *ng2 != '.')
				if (ptrncmp(ng1, ng2++))
					return(TRUE);
			return (ptrncmp(ng1, ng2));
		} else if (*ng1++ != *ng2++)
			return(FALSE);
	}
	return (*ng2 == '.' || *ng2 == NGDELIM);
}

/*
 * Remove newsgroups in 'a' not subscribed to by 'b'.
 */
ngsquash(ap, bp)
register char *ap, *bp;
{
	register char *tp;
	char tbuf[BUFLEN];

	/* replace NGDELIM by '\0' in a */
	for (tp = ap; *tp != '\0'; tp++)
		if (*tp == NGDELIM)
			*tp = '\0';
	/* ap = building, tp = checking. */
	tp = ap;
	while (*tp != '\0') {
		ngcat(strcpy(tbuf, tp));
		if (ngmatch(tbuf, bp)) {
			while ((*ap++ = *tp++) != '\0')
				;
			ap[-1] = NGDELIM;
		} else
			while (*tp++ != '\0');
	}
	*ap = '\0';
}

/*
 * Exec the shell.
 * This version resets uid, gid, and umask.
 * Called with fsubr(ushell, s, NULL)
 */
/* ARGSUSED */
ushell(s, dummy)
char *s, *dummy;
{
	umask(savmask);
	setgid(gid);
	setuid(uid);
	xshell(s);
}

/*
 * Exec the shell.
 * This version restricts PATH to bin and /usr/bin.
 * Called with fsubr(pshell, s, NULL)
 */
char	**environ;

/* ARGSUSED */
pshell(s, dummy)
char *s, *dummy;
{
	static char *penv[] = { "PATH=/bin:/usr/bin", NULL };
	register char **ep, *p;
	register int found;

	found = FALSE;
	for (ep = environ; p = *ep; ep++) {
		if (strncmp(p, "PATH=", 5) == 0) {
			*ep = penv[0];
			found = TRUE;
		}
	}
	if (!found)
		environ = &penv[0];
	xshell(s);
}

/*
 * Exec the shell.
 */
xshell(s)
char *s;
{
	execl("/bin/sh", "sh", "-c", s, 0);
	xerror("No shell!");
}

/*
 * Fork and call a subroutine with two args.
 * Return pid without waiting.
 */
fsubr(f, s1, s2)
int (*f)();
char *s1, *s2;
{
	register int pid;

	while ((pid = fork()) == -1)
		sleep(1);
	if (pid == 0) {
		(*f)(s1, s2);
		exit(0);
	}
	return(pid);
}

/*
 * Wait on a child process.
 */
fwait(pid)
register int pid;
{
	register int w;
	static int status, (*onhup)(), (*onint)();

	onint = signal(SIGINT, SIG_IGN);
	onhup = signal(SIGHUP, SIG_IGN);
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1)
		status = -1;
	signal(SIGINT, onint);
	signal(SIGHUP, onhup);
	return(status);
}

/*
 * Exit and cleanup.
 */
xxit(status)
int status;
{
	while (lockcount > 0)
		unlock();
	exit(status);
}

/*
 * Get user name and home directory.
 */
getuser()
{
	static int flag = TRUE;
	register struct passwd *p;

	if (flag) {
		if ((p = getpwuid(uid)) == NULL)
			xerror("Cannot get user's name");
		if (username[0] == 0)
			strcpy(username, p->pw_name);
		strcpy(userhome, p->pw_dir);
		flag = FALSE;
	}
	strcpy(header.path, username);
}

/*
 * Put a unique name into header.name.
 */
getname()
{
	long seqn;
	register FILE *fp;

	fp = xfopen(SEQFILE, "r");
	fgets(bfr, BUFLEN, fp);
	fclose(fp);
	seqn = atol(bfr) + 1;
	fp = xfopen(SEQFILE, "w");
	fprintf(fp, "%ld\n", seqn);
	fclose(fp);
	sprintf(header.name, "%s.%ld", SYSNAME, seqn);
}

/*
 * Put the long int date into header.ndate,
 * and the ascii date string into header.date.
 */
getdtln()
{
	time(&header.ndate);
	nstrip(strcpy(header.date, ctime(&header.ndate)));
}

/*
 * Rename f1 to f2, and ensure that we own f2.
 */
rename(f1, f2)
register char *f1, *f2;
{
	unlink(f2);
	link(f1, f2);
	unlink(f1);
#ifdef PWB
	chmod(f2, 0644);
	if (duid != 0)
		chown(f2, ((dgid << 8) | duid));
#else
	if (duid != 0)
		chown(f2, duid, dgid);
#endif
}

/*
 * Put cancelled or otherwise banished news into the CAND directory.
 */
canned(oname, fname)
char *oname, *fname;
{
	sprintf(bfr, "%s/%s", CAND, fname);
	link(oname, bfr);	/* no diagnostic if link fails */
#ifdef PWB
	chmod(bfr, 0644);
	if (duid != 0)
		chown(bfr, ((dgid << 8) | duid));
#else
	if (duid != 0)
		chown(bfr, duid, dgid);
#endif
}
