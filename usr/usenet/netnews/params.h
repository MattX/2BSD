#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>

/* @(#)params.h	1.3	9/22/80 */

#define	NNREC	20	/* number of nindex records to hold locally	*/
#ifdef vax
#define	ARTSIZ	2000	/* max number of articles handled by readr().	*/
#else
#define	ARTSIZ	200	/* max number of articles handled by readr().	*/
#endif

#ifndef USIZE
#ifdef PWB
#define USIZE	256	/* news can handle uids from 0 up to USIZE-1	*/
#else
#define	USIZE	4000	/* news can handle uids from 0 up to USIZE-1	*/
#endif
#endif
#define	BUFLEN	200	/* length of longest buffer needed.		*/
#define	DATELEN	35	/* length of longest allowed date string.	*/
#define	NAMELEN	14	/* length of longest allowed file name		*/

#define	SNLN	8	/* max significant characters in sysname	*/
#define	PROTO	'A'	/* protocol name.				*/
#define NETCHRS	"!:.@^"	/* Punct. chars used for various networks.	*/
#define	NGDELIM	','	/* delimit character in news group line		*/
#define	ROOTID	0	/* uid of person allowed to cancel anything	*/
#define	N_UMASK 000	/* mask for umask call, 022 for secure system	*/

#define	TRUE	1
#define	FALSE	0

struct	hbuf {
	time_t	ndate;			/* date submitted	*/
	char	path[BUFLEN];		/* source string	*/
	char	date[DATELEN];		/* ndate in ascii	*/
	char	title[BUFLEN];		/* title.		*/
	char	name[NAMELEN+1];	/* unique name		*/
	char	nbuf[BUFLEN];		/* newsgroup line	*/
};

struct	nrec {
	char	n_file[NAMELEN+1];	/* unique name		*/
	time_t	n_date;			/* date submitted	*/
	char	n_nglist[BUFLEN];	/* newsgroup line	*/
};

struct	urec {
	unsigned u_uid;			/* user i-d.		*/
	time_t	u_date;			/* last read time	*/
	char	u_nglist[BUFLEN];	/* subscription list	*/
};

struct	srec {
	char	s_name[NAMELEN+1];	/* system name		*/
	char	s_nbuf[BUFLEN];		/* system subscriptions */
	char	s_xmit[BUFLEN];		/* system xmit routine	*/
};

extern	unsigned	uid, gid, duid, dgid;
extern	int	savmask;
extern	int	sigtrap;
extern	int	bitup;

extern	struct	hbuf header;
extern	char	bfr[BUFLEN];
extern	char	username[BUFLEN], userhome[BUFLEN];
extern	char	datebuf[BUFLEN], titlebuf[BUFLEN];
extern	char	coptbuf[BUFLEN];

extern	char	*NEWSD;
extern	char	*CAND;

extern	char	*LOCKFILE;
extern	char	*BITFILE;
extern	char	*SEQFILE;
extern	char	*NGFILE;
extern	char	*UINDEX, *NINDEX;
extern	char	*SYSFILE, *HISTORY;
extern	char	*TUFILE, *TNFILE, *TIFILE, *TOFILE;

extern	char	SYSNAME[];
extern	char	*NEWSU, *NEWSG;

extern	char	*ADMSUB, *ALL, *DFLTSUB, *DFLTNG;
extern	char	*PARTIAL;

extern	FILE	*xfopen(), *hread();
extern	char	*strcpy(), *strncpy(), *strcat(), *index(), *rindex();
extern	char	*ctime(), *mktemp();
extern	struct	passwd *getpwnam(), *getpwuid(), *getpwent();
extern	struct	group *getgrnam();
extern	time_t	time(), getdate();
extern	int	broadcast(), save(), newssave(), ushell(), pshell(), onsig(),
		makebits();
extern	long	atol();
