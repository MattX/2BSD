#define TOMB

#ifndef lint
static char *sccsid = "@@(#)cp.c	4.8 83/07/01";
#endif

/*
 * cp
 */
#include <stdio.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/dir.h>

#define	BSIZE	8192

int	iflag;
int	rflag;
char    *rindex(), *index(), *sprintf();

#ifdef	TOMB
#include <sys/file.h>
#include <sys/time.h>

#ifndef	TRUE
#define TRUE 1
#define FALSE 0
typedef char bool;
#endif	TRUE

int Flag_n = 0;	/* no-save flag */
#endif	TOMB

main(argc, argv)
	int argc;
	char **argv;
{
	struct stat stb;
	int rc, i;

	argc--, argv++;
	while (argc > 0 && **argv == '-') {
		(*argv)++;
		while (**argv) switch (*(*argv)++) {

		case 'i':
			iflag++; break;

		case 'r':
			rflag++; break;

#ifdef	TOMB
		case 'n':
			/* no-save option */
			Flag_n++;
			break;
#endif	TOMB

		default:
			goto usage;
		}
		argc--; argv++;
	}
	if (argc < 2) 
		goto usage;
	if (argc > 2 || rflag) {
		if (stat(argv[argc-1], &stb) < 0)
			goto usage;
		if ((stb.st_mode&S_IFMT) != S_IFDIR) 
			goto usage;
	}
	rc = 0;
	for (i = 0; i < argc-1; i++)
		rc |= copy(argv[i], argv[argc-1]);
	exit(rc);
usage:

#ifdef TOMB
	fprintf(stderr,
	    "Usage: cp [ -in ] f1 f2; or cp [ -inr ] f1 ... fn d2\n");
#else
	fprintf(stderr,
	    "Usage: cp f1 f2; or cp [ -r ] f1 ... fn d2\n");
#endif TOMB

	exit(1);
}

copy(from, to)
	char *from, *to;
{
	int fold, fnew, n;
	char *last, destname[BSIZE], buf[BSIZE], *p;
	struct stat stfrom, stto;

	fold = open(from, 0);
	if (fold < 0) {
		Perror(from);
		return (1);
	}
	if (fstat(fold, &stfrom) < 0) {
		Perror(from);
		(void) close(fold);
		return (1);
	}
	if (stat(to, &stto) >= 0 &&
	   (stto.st_mode&S_IFMT) == S_IFDIR) {
		last = rindex(from, '/');
		if (last) last++;
		else if (last = index(from, ':'))       /* changed here */
			last++;
		else last = from;
		if (strlen(to) + strlen(last) >= BSIZE - 1) {
			fprintf(stderr, "cp: %s/%s: Name too long", to, last);
			(void) close(fold);
			return(1);
		}
							 /* changed here */
		if ((p = index(to, ':')) && (*++p == '\0'))
			(void) sprintf(destname, "%s%s", to, last);
		else
			(void) sprintf(destname, "%s/%s", to, last);
		to = destname;
	}
	if (rflag && (stfrom.st_mode&S_IFMT) == S_IFDIR) {
		(void) close(fold);
		if (stat(to, &stto) < 0) {
			if (mkdir(to, (int)stfrom.st_mode) < 0) {
				Perror(to);
				return (1);
			}
		} else if ((stto.st_mode&S_IFMT) != S_IFDIR) {
			fprintf(stderr, "cp: %s: Not a directory.\n", to);
			return (1);
		}
		return (rcopy(from, to));
	}
	if (stat(to, &stto) >= 0) {
		if (stfrom.st_dev == stto.st_dev &&
		   stfrom.st_ino == stto.st_ino) {
			fprintf(stderr, "cp: Cannot copy file to itself.\n");
			(void) close(fold);
			return (1);
		}
		if (iflag) {
			int i, c;

			fprintf (stderr, "overwrite %s? ", to);
			i = c = getchar();
			while (c != '\n' && c != EOF)
				c = getchar();
			if (i != 'y') {
				(void) close(fold);
				return(1);
			}
		}
	}

#ifdef	TOMB
	if ( !Flag_n )
		entomb(to,&stto);
#endif	TOMB

	fnew = creat(to, (int)stfrom.st_mode);
	if (fnew < 0) {
		Perror(to);
		(void) close(fold); return(1);
	}
	for (;;) {
		n = read(fold, buf, BSIZE);
		if (n == 0)
			break;
		if (n < 0) {
			Perror(from);
			(void) close(fold); (void) close(fnew); return (1);
		}
		if (write(fnew, buf, n) != n) {
			Perror(to);
			(void) close(fold); (void) close(fnew); return (1);
		}
	}
	(void) close(fold); (void) close(fnew); return (0);
}

rcopy(from, to)
	char *from, *to;
{
	DIR *fold = opendir(from);
	struct direct *dp;
	int errs = 0;
	char fromname[BUFSIZ], *p;

	if (fold == 0) {
		Perror(from);
		return (1);
	}
	for (;;) {
		dp = readdir(fold);
		if (dp == 0) {
			closedir(fold);
			return (errs);
		}
		if (dp->d_ino == 0)
			continue;
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
			continue;
		if (strlen(from) + 1 + strlen(dp->d_name) >= BUFSIZ - 1) {
			fprintf(stderr, "cp: %s/%s: Name too long.\n",
			    from, dp->d_name);
			errs++;
			continue;
		}
		if ((p = index(from, ':')) && (*++p == '\0')) /* chaned here */
			(void) sprintf(fromname, "%s%s", from, dp->d_name);
		else
			(void) sprintf(fromname, "%s/%s", from, dp->d_name);
		errs += copy(fromname, to);
	}
}

Perror(s)
	char *s;
{

	fprintf(stderr, "cp: ");
	perror(s);
}

#ifdef	TOMB
/*
** entomb -- place a copy or link of the file in the tomb directory
**
** name - file name from the command argument list
** statbuf - pointer to a stat on the file.
*/
#include <pwd.h>
#define	ARCHDIR	"/usr/tomb/"
#define NameLeng 24                                     /* changed here */
#define cchMax   256

entomb (name, statbuf)
char *name;
struct stat *statbuf;
{
	static bool first_time = TRUE;
	static char *tombdir;
	char *setup_tomb();
	char Host[NameLeng], Name[cchMax];              /* changed here */
	int  n;

	if (!IsLocal(name, Host, Name) ||               /* changed here */
	    !NoRemoteLink(name, Host, Name)) {
		Host[n = strlen(Host)] = ':';
		Host[++n] = '\0';
	} else
		Host[0] = '\0';

	if ( checkf(name,statbuf) )
		return;
	if (first_time) {
		first_time = FALSE;
		tombdir = setup_tomb(Host);
	}
	if (tombdir != NULL)
		copy_in_tomb (tombdir,name,statbuf);
}

/*
** setup_tomb -- get login name and create a directory in /usr/tomb
**
**  returns the name of the directory to use as tomb.  NULL if error.
*/

char *
setup_tomb (host)
char    *host;                                          /* changed here */
{
	static char tombdir[MAXPATHLEN];
	register int kid;
	int status;
	struct passwd *pwd, *getpwuid();

	if ( (pwd = getpwuid (getuid())) == NULL )
		return NULL;
	strcpy(tombdir,host);                           /* changed here */
	strcat(tombdir,ARCHDIR);
	if ( access(tombdir, W_OK|X_OK) != 0)
		return NULL;

	/*
	** Check /usr/tomb area to see if the user has a directory
	*/

	strcat (tombdir, pwd->pw_name);
	if (access (tombdir, F_OK) < 0 ) {
		while ((kid = fork ()) < 0)
			sleep (3);
		if (kid) {
			wait (&status);
			chmod (tombdir, 0700);
		} else {
			execl (MKDIR, "mkdir", tombdir, 0);
			fprintf (stderr, "rm: can't create /usr/tomb directory\n");
			exit (1);
		}
		if ( (status>>8) != 0 )
			return NULL;
	}

	/*
	** assert: /usr/tomb/$user exists and is mode 700
	*/

	strcat(tombdir,"/");

	return tombdir;
}

/*
 * copy_in_tomb -- make a copy of the rm'ed file in /usr/tomb
 */

copy_in_tomb (tombdir,name,statbuf)
char *tombdir, *name;
struct stat *statbuf;
{
	register int len;
	int fdin, fdout;
	char *tombpath, *makename();
	char buffer[BUFSIZ];

	tombpath = makename(tombdir,name);

	/*
	**  Try a fast link into the tomb directory.  If that fails,
	**  copy it in.
	*/

	if (link (name, tombpath) == 0) {
		if (statbuf->st_nlink == 1)
			chmod (tombpath, 0600);
	} else {
		if ((fdin = open (name, 0)) < 0)
			return;

		if ((fdout = creat (tombpath, 0600)) < 0) {
			close (fdin);
			return;
		}
		while ((len = read (fdin, buffer, BUFSIZ)) > 0)
			write (fdout, buffer, len);
		close (fdin);
		close (fdout);
	}
}

/*
** makename - form the name of a file as it will exist in the tomb
**	directory.
**
** tombdir - name of user's tomb directory ended with a slash
** userfile - the file name provided as an argument to this program
**
** returns a pointer to static storage containing the full pathname
** of the place to put the file (e.g. /usr/tomb/rlb/ ...)
*/

char *Month[] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct",
	"Nov","Dec" };

char *
makename(tombdir,userfile)
char *userfile, *tombdir;
{
	static bool donthavewd = TRUE;
	static char wd[MAXPATHLEN];
	static char result[MAXPATHLEN+MAXNAMLEN+1];
	char *cp, *up;
	long clock;
	struct tm *tm;

	/*
	** This version generates names comprising the tomb directory
	** and the full pathname of the removed file with slashes converted
	** to backslashes and then with a time stamp appended to that.
	**
	** e.g. /usr/tomb/rlb/\user\rlb\src\cmd\testrm.c(Oct 4 12:00:00)
	*/

	if ( donthavewd ) {
		if ( getwd(wd)==0 )
			strcpy(wd,"");
		else {
			strcat(wd,"/");
			for ( cp=wd ; *cp ; cp++ )
				if ( *cp == '/' )
					*cp = '\\';
		}
		donthavewd = FALSE;
	}
	strcpy(result,tombdir);
	strcat(result,wd);
	
	cp = &result[strlen(result)];
	for ( up = userfile ; *up ; up++ )
		if ( *up == '/' )
			*cp++ = '\\';
		else
			*cp++ = *up;

	/*
	** Append a unique timestamp
	*/

	time ( &clock );
	do {
		tm = (struct tm *)localtime(&clock);
		sprintf(cp,"(%s %d %d:%02d:%02d)",Month[tm->tm_mon],
		    tm->tm_mday, tm->tm_hour,tm->tm_min,tm->tm_sec);
		clock++;
	} while (access(result,F_OK)==0);

	return result;
}

/*
 * checkf -- checkf to see if file is 'tombable'. return -1 if it isn't
 */

char *NoSaveName[] = {
	 "core",
	 "a.out",
	 "obj",
	 NULL
};

char *NoSaveExt[] = {
	 ".o",
	 ".CKP",
	 NULL
};

checkf (name,statbuf)
char *name;
struct stat *statbuf;
{
	char    *rindex(), *index();
	char    *ext, *fname;
	int	i;

	/*
	**  Test 1:  File must be a regular file
	*/

	if ((statbuf->st_mode & S_IFMT) != S_IFREG )
		return TRUE;

	if (fname = rindex (name, '/'))
		fname++;
	else if (fname = index (name, ':'))             /* changed here */
		fname++;
	else
		fname = name;

	/*
	**  Test 2: do not save files listed in "NoSaveName"
	*/

	for ( i = 0 ; NoSaveName[i] != NULL ; i++ )
		if ( strcmp(fname,NoSaveName[i])==0 )
			return TRUE;

	/*
	**  Test 3: do not save files with extensions listed in "NoSaveExt"
	*/

	if ((ext = rindex (fname, '.')) != NULL ) {
		for ( i = 0 ; NoSaveExt[i] != NULL ; i++ )
			if ( strcmp(ext,NoSaveExt[i])==0 )
				return TRUE;
		
	}

	return FALSE;
}
#endif	TOMB
