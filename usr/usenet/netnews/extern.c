#include "params.h"
static char *sccsid = "@(#)extern.c	1.3	9/14/80";

#ifndef	NEWSDIR
/* NOTE: following line does not have a trailing quote. */
#define	NEWSDIR	"/usr/spool/news
#endif

#ifndef	NEWSUSR
#define	NEWSUSR	"daemon"
#endif

#ifndef	NEWSGRP
#define	NEWSGRP	"daemon"
#endif

unsigned uid, gid;	/* real user/group id */
unsigned duid, dgid;	/* user/group of who would like to be. */
int	savmask;	/* old umask. */
int	sigtrap;	/* true if a signal has been caught */
int	bitup;		/* true if must re-create bitmap. */

struct	hbuf	header;
char	bfr[BUFLEN];	/* scratch area. use with extreme care! */
char	username[BUFLEN];	/* user login name */
char	userhome[BUFLEN];	/* user home directory */
char	datebuf[BUFLEN];	/* buffer for 'a' option */
char	titlebuf[BUFLEN];	/* title buffer */
char	coptbuf[BUFLEN];	/* for 'c' option. */

char	*NEWSD = NEWSDIR";		/* news directory. */
char	*CAND = NEWSDIR/.canned";	/* Directory for cancelled news */

char	*LOCKFILE = NEWSDIR/.LOCK";	/* lock file. (linked to uindex). */
char	*BITFILE = NEWSDIR/.bitfile";	/* bit map file. */
char	*SEQFILE = NEWSDIR/.seq";	/* current sequence number. */
char	*NGFILE = NEWSDIR/.ngfile";	/* list of legal news groups. */
char	*UINDEX = NEWSDIR/.uindex";	/* user index file. */
char	*NINDEX = NEWSDIR/.nindex";	/* news index file. */
char	*SYSFILE = NEWSDIR/.sys";	/* system subscriptions. */
char	*HISTORY = NEWSDIR/.history";	/* contains all items received */
char	*TUFILE = NEWSDIR/.TMP1";	/* temp files. */
char	*TNFILE = NEWSDIR/.TMP2";
char	*TIFILE = NEWSDIR/.TMP3";
char	*TOFILE = NEWSDIR/.TMP4";

char	SYSNAME[SNLN+1];		/* local system name */
char	*NEWSU = NEWSUSR;
char	*NEWSG = NEWSGRP;

char	*ADMSUB = "general";		/* news group all must belong to */
char	*DFLTSUB = "general";		/* default subscription list */
char	*DFLTNG = "general";		/* default news group */
char	*ALL = "ALL";			/* subscription list keyword. */
char	*PARTIAL = "partial.news";	/* place to save partial news. */
