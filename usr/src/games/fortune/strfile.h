/*
 * fortune include file
 *
 *	Keith Bostic
 *		ARPA: keith@seismo
 *		UUCP: seismo!keith
 */

#define OUTFILE		"fortunes.dat"	/* standard output file */
#define ERR		-1		/* general error condition */
#define NO		0		/* general no, false */
#define OK		0		/* general ok condition */
#define YES		1		/* general yes, true */

char	*malloc();			/* memory allocation */
#define MM(cast,store,amount,type)	if (!(store=(cast *)malloc((unsigned)(sizeof(type)*(amount))))){perror("malloc");exit(ERR);}

#define SCENE		0		/* the first is scene fortunes */
#define OBS		1		/* the second is obscene fortunes */
#define OBSLIM		2		/* the third is obscene limericks */
#define END		3		/* end of offsets */
#define SECTIONS	4		/* number of sections */

struct strf {
	char	*fname;			/* file name */
	long	entry,			/* entry into database */
		number;			/* how many fortunes */
};
typedef struct strf STRF;

static STRF	tbl[SECTIONS] = {
	"scene",	SCENE,		0,	/* current table breakup */
	"obscene",	OBS,		0,
	"obs.lim",	OBSLIM,		0,
	"",		END,		0,
};
