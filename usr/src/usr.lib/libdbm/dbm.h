#define	PBLKSIZ	1024		/* For 1K block filesystems */
#define	DBLKSIZ	8192
#define	BYTESIZ	8
#define	NULL	((char *) 0)

long	bitno;
long	maxbno;
long	blkno;
long	hmask;

char	pagbuf[PBLKSIZ];
char	dirbuf[DBLKSIZ];

int	dirf;
int	pagf;

typedef	struct
{
	char	*dptr;
	int	dsize;
} datum;

datum	fetch();
datum	makdatum();
datum	firstkey();
datum	nextkey();
datum	firsthash();
long	calchash();
long	hashinc();

