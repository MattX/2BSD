#define TOMB

/*
** rm - remove files
**
** This program removes files or subtrees from the directory hierarchy.
**
** Files are not actually removed altogether.  They are moved to a directory
** named /usr/tomb/USER where USER is the user name corresponding to the
** current uid of the rm process.  The names in the tomb directory are the
** full pathname of the deleted file with the current date and time appended
** in parentheses.
**
**
** Bob Brown, George Adams
** RIACS, NASA Ames
*/

#include <stdio.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/dir.h>

#ifdef	TOMB
#include <sys/file.h>
#include <sys/time.h>

#ifndef	TRUE
#define TRUE 1
#define FALSE 0
typedef char bool;
#endif	TRUE
#endif	TOMB

int	Errors;
int	Flag_f = 0;	/* force flag */
int	Flag_i = 0;	/* interactive flag */
int	Flag_r = 0;	/* recursive flag */

#ifdef	TOMB
int Flag_n = 0;	/* no-save flag */
#endif	TOMB

main(argc, argv)
char *argv[];
{
	register char *arg;

	if (isatty(0) == 0)
		Flag_f++;
	while(argc>1 && argv[1][0]=='-') {
		arg = *++argv;
		argc--;

		/*
		 *  all files following a null option are considered file names
		 */
		if (*(arg+1) == '\0') break;

		while(*++arg != '\0')
			switch(*arg) {
			case 'f':
				Flag_f++;
				break;
			case 'i':
				Flag_i++;
				break;
			case 'r':
				Flag_r++;
				break;

#ifdef	TOMB
			case 'n':
				/* no-save option */
				Flag_n++;
				break;
#endif	TOMB

			default:
				printf("rm: unknown option %s\n", *argv);
				exit(1);
			}
	}
	if ( argc == 1) {

#ifdef TOMB
		fprintf(stderr, "usage: rm -firn files ..... \n");
#else
		fprintf(stderr, "usage: rm -fir files ..... \n");
#endif TOMB

		exit(1);
	}

	while(--argc > 0) {
		if(!strcmp(*++argv, "..")) {
			fprintf(stderr, "rm: cannot remove `..'\n");
			continue;
		}
		rm(*argv, 0);
	}

	exit(Errors);
}

rm(arg, level)
char arg[];
{
	struct direct *dp;
	DIR *dirp;
	struct	stat statbuf;
	char name[MAXPATHLEN], *p, *index();

	if(lstat(arg, &statbuf)) {
		if (Flag_f==0) {
			printf("rm: %s nonexistent\n", arg);
			++Errors;
		}
		return;
	}
	if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
		if( !Flag_r ) {
			printf("rm: %s directory\n", arg);
			++Errors;
			return;
		} else {
			if (access(arg, W_OK) < 0) {
				if (Flag_f==0)
					printf("%s not changed\n", arg);
				Errors++;
				return;
			}
			if(Flag_i && level!=0) {
				printf("remove directory %s? ", arg);
				if(!yes())
					return;
			}
			if((dirp = opendir(arg)) == NULL) {
				printf("rm: cannot read %s?\n", arg);
				exit(1);
			}
			while((dp = readdir(dirp)) != NULL) {
				if(dp->d_ino != 0 && !dotname(dp->d_name)) {
					if ((p = index(arg, ':')) && (*++p == '\0')) /* changed here */
						sprintf(name, "%s%s", arg, dp->d_name);
					else
						sprintf(name, "%s/%s", arg, dp->d_name);
					rm(name, level+1);
				}
			}
			closedir(dirp);
			if (dotname(arg))
				return;
			if (Flag_i) {
				printf("rm: remove %s? ", arg);
				if (!yes())
					return;
			}
			if (Rmdir(arg) < 0) {
				fprintf(stderr, "rm: ");
				perror(arg);
				Errors++;
			}
			return;
		}
	}

	if(Flag_i) {
		printf("rm: remove %s? ", arg);
		if(!yes())
			return;
	} else if(!Flag_f) {
		if ((statbuf.st_mode&S_IFMT) != S_IFLNK && access(arg, W_OK) < 0) {
			printf("rm: override protection %o for %s? ",
				statbuf.st_mode & 0777, arg);
			if(!yes())
				return;
		}
	}

#ifdef	TOMB
	if ( !Flag_n )
		entomb(arg,&statbuf);
#endif	TOMB

	if(unlink(arg) && (Flag_f==0 || Flag_i)) {
		printf("rm: %s not removed\n", arg);
		++Errors;
	}
}

dotname(s)
char *s;
{
	return strcmp(s,".")==0 || strcmp(s,"..")==0;
}

Rmdir(f)
char *f;
{
	int status, i;

	if(dotname(f))
		return(0);
	if(Flag_i) {
		printf("rm: remove %s? ", f);
		if(!yes())
			return(0);
	}
	while((i=fork()) < 0)
		sleep(3);
	if(i) {
		wait(&status);
		return(status);
	}
	execl(RMDIR, "rmdir", f, 0);
	printf("rm: can't find rmdir\n");
	exit(1);
	/* NOTREACHED */
}

yes()
{
	int i, b;

	i = b = getchar();
	while(b != '\n' && b != EOF)
		b = getchar();
	return(i == 'y');
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
#define NameLeng 24
#define cchMax   256

entomb (name, statbuf)
char *name;
struct stat *statbuf;
{
	static bool first_time = TRUE;
	static char *tombdir;
	char *setup_tomb();
	char Host[NameLeng], Name[cchMax];      /* changed here */
	int  n;

	if (!IsLocal(name, Host, Name) ||       /* changed here */
	    !NoRemoteLink(name, Host, Name)) {
		Host[n = strlen(Host)] = ':';
		Host[++n] = '\0';
	} else
		Host[0] = '\0';

	if ( checkf(name,statbuf) )
		return;
	if (first_time) {
		first_time = FALSE;
		tombdir = setup_tomb(Host);     /* changed here */
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
char    *host;                                  /* changed here */
{
	static char tombdir[MAXPATHLEN];
	register int kid;
	int status;
	struct passwd *pwd, *getpwuid();

	if ( (pwd = getpwuid (getuid())) == NULL )
		return NULL;
	strcpy(tombdir,host);                   /* changed here */
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
 * copy_in_tomb -- make a copy of the %m'ed file in /usr/tomb
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
