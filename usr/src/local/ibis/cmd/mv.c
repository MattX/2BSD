#define        TOMB

#ifndef lint
static	char *sccsid = "@@(#)mv.c	4.13 (Berkeley) 83/06/30";
#endif

/*
 * mv file1 file2
 */
#include <sys/param.h>
#include <sys/stat.h>

#include <stdio.h>
#include <sys/dir.h>
#include <errno.h>
#include <signal.h>

#define	DELIM	'/'
#define MODEBITS 07777

#define	ISDIR(st)	(((st).st_mode&S_IFMT) == S_IFDIR)
#define	ISLNK(st)	(((st).st_mode&S_IFMT) == S_IFLNK)
#define	ISREG(st)	(((st).st_mode&S_IFMT) == S_IFREG)
#define	ISDEV(st) \
	(((st).st_mode&S_IFMT) == S_IFCHR || ((st).st_mode&S_IFMT) == S_IFBLK)

char	*sprintf();
char    *dname();
struct	stat s1, s2;
int	iflag = 0;	/* interactive mode */
int	fflag = 0;	/* force overwriting */
extern	unsigned errno;

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
	register char *argv[];
{
	register i, r;
	register char *arg;
	char *dest;

	if (argc < 2)
		goto usage;
	while (argc > 1 && *argv[1] == '-') {
		argc--;
		arg = *++argv;

		/*
		 * all files following a null option
		 * are considered file names
		 */
		if (*(arg+1) == '\0')
			break;
		while (*++arg != '\0') switch (*arg) {

		case 'i':
			iflag++;
			break;

		case 'f':
			fflag++;
			break;

#ifdef	TOMB
		case 'n':
			/* no-save option */
			Flag_n++;
			break;
#endif	TOMB

		default:
			goto usage;
		}
	}
	if (argc < 3)
		goto usage;
	dest = argv[argc-1];
	if (stat(dest, &s2) >= 0 && ISDIR(s2)) {
		r = 0;
		for (i = 1; i < argc-1; i++)
			r |= movewithshortname(argv[i], dest);
		exit(r);
	}
	if (argc > 3)
		goto usage;
	r = move(argv[1], argv[2]);
	exit(r);
	/*NOTREACHED*/
usage:

#ifdef TOMB
	fprintf(stderr,
"usage: mv [-inf] f1 f2 or mv [-inf] f1 ... fn d1 (`fn' is a file or directory)\n");
#else
	fprintf(stderr,
"usage: mv [-if] f1 f2 or mv [-if] f1 ... fn d1 (`fn' is a file or directory)\n");
#endif TOMB

	return (1);
}

movewithshortname(src, dest)
	char *src, *dest;
{
	register char *shortname;
	char target[MAXPATHLEN + 1];
	char *index(), *p;                                /* changed here */

	shortname = dname(src);
	if ((shortname == src) && (p = index(src, ':')))  /* changed here */
		shortname = ++p;
	if (strlen(dest) + strlen(shortname) > MAXPATHLEN - 1) {
		error("%s/%s: pathname too long", dest,
			shortname);
		return (1);
	}
	if ((p = index(dest, ':')) && (*++p == '\0'))     /* changed here */
		sprintf(target, "%s%s", dest, shortname);
	else
		sprintf(target, "%s/%s", dest, shortname);
	return (move(src, target));
}

move(source, target)
	char *source, *target;
{
	int targetexists;

	if (lstat(source, &s1) < 0) {
		error("cannot access %s", source);
		return (1);
	}
	/*
	 * First, try to rename source to destination.
	 * The only reason we continue on failure is if
	 * the move is on a nondirectory and not across
	 * file systems.
	 */
	targetexists = lstat(target, &s2) >= 0;
	if (targetexists) {
		if (iflag && !fflag && query("remove %s? ", target) == 0)
			return (1);
		if (s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino) {
			error("%s and %s are identical", source, target);
			return (1);
		}
		if (access(target, 2) < 0 && !fflag && isatty(fileno(stdin))) {
			if (query("override protection %o for %s? ",
			  s2.st_mode & MODEBITS, target) == 0)
				return (1);
		}

#ifdef	TOMB
		if ( !Flag_n )
			entomb(target,&s2);
#endif	TOMB

	}
	if (rename(source, target) >= 0)
		return (0);
	if (errno != EXDEV) {
		Perror2(source, "rename");
		return (1);
	}
	if (ISDIR(s1)) {
		error("can't mv directories across file systems");
		return (1);
	}
	if (targetexists && unlink(target) < 0) {
		error("cannot unlink %s", target);
		return (1);
	}
	/*
	 * File can't be renamed, try to recreate the symbolic
	 * link or special device, or copy the file wholesale
	 * between file systems.
	 */
	if (ISLNK(s1)) {
		register m;
		char symln[MAXPATHLEN];

		if (readlink(source, symln, sizeof (symln)) < 0) {
			Perror(source);
			return (1);
		}
		m = umask(~(s1.st_mode & MODEBITS));
		if (symlink(symln, target) < 0) {
			Perror(target);
			return (1);
		}
		(void) umask(m);
		goto cleanup;
	}
	if (ISDEV(s1)) {
		time_t tv[2];

		if (mknod(target, s1.st_mode, s1.st_rdev) < 0) {
			Perror(target);
			return (1);
		}
		/* kludge prior to utimes */
		tv[0] = s1.st_atime;
		tv[1] = s1.st_mtime;
		(void) utime(target, tv);
		goto cleanup;
	}
	if (ISREG(s1)) {
		int i, c, status;
		time_t tv[2];

		i = fork();
		if (i == -1) {
			error("try again");
			return (1);
		}
		if (i == 0) {

#ifdef	TOMB
			execl(CP, "cp", "-n", source, target, 0);
#else
			execl(CP, "cp", source, target, 0);
#endif	TOMB

			error("cannot exec cp");
			exit(1);
		}
		while ((c = wait(&status)) != i && c != -1)
			;
		if (status != 0)
			return (1);
		/* kludge prior to utimes */
		tv[0] = s1.st_atime;
		tv[1] = s1.st_mtime;
		(void) utime(target, tv);
		goto cleanup;
	}
	error("%s: unknown file type %o", source, s1.st_mode);
	return (1);

cleanup:
	if (unlink(source) < 0) {
		error("cannot unlink %s", source);
		return (1);
	}
	return (0);
}

/*VARARGS*/
query(prompt, a1, a2)
	char *a1;
{
	register char i, c;

	fprintf(stderr, prompt, a1, a2);
	i = c = getchar();
	while (c != '\n' && c != EOF)
		c = getchar();
	return (i == 'y');
}

char *
dname(name)
	register char *name;
{
	register char *p;

	p = name;
	while (*p)
		if (*p++ == DELIM && *p)
			name = p;
	return name;
}

/*VARARGS*/
error(fmt, a1, a2)
	char *fmt;
{

	fprintf(stderr, "mv: ");
	fprintf(stderr, fmt, a1, a2);
	fprintf(stderr, "\n");
}

Perror(s)
	char *s;
{
	char buf[MAXPATHLEN + 10];
	
	sprintf(buf, "mv: %s", s);
	perror(buf);
}

Perror2(s1, s2)
	char *s1, *s2;
{
	char buf[MAXPATHLEN + 20];

	sprintf(buf, "mv: %s: %s", s1, s2);
	perror(buf);
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
#define cchMax   256                                    /* changed here */

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
		tombdir = setup_tomb(Host);             /* changed here */
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
	strcat(tombdir, ARCHDIR);
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
	else if (fname = index(name, ':'))              /* changed here */
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
