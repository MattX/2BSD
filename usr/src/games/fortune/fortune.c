#include <stdio.h>
#include <sys/param.h>
#include <strfile.h>

/*
 * produce a fortune from the database
 *
 *	Keith Bostic
 *		ARPA: keith@seismo
 *		UUCP: seismo!keith
 */

#define CPERS		20		/* # of chars for each sec */
#define MAXTRY		20		/* try 20 times, then fuck it */
#define MINW		6		/* minimum sleep wait */
#define SLEN		80		/* # of chars in short fortune */
#define LLEN		240		/* # of chars in longer fortune */
#define FFILE		"/usr/games/lib/fortunes.dat"

main(argc,argv)
int	argc;
char	**argv;
{
	extern char	*optarg;	/* getopt variable */
	static char	*ffile = FFILE;	/* fortune file */
	register long	len,		/* length of the fortune */
			cnt;		/* general counter */
	long	bottom,			/* low fortune */
		num,			/* number of fortunes to choose from */
		try,			/* random number to try */
		*seekpts,		/* point to all of table */
		hold[SECTIONS + 1];	/* hold first part of table */
	int	ch;			/* argument character */
	short	do_all = NO,		/* all fortunes */
		do_lim = NO,		/* limericks */
		do_long = NO,		/* long fortune */
		do_off = NO,		/* offensive */
		do_shrt = NO,		/* short fortune */
		do_wait = NO;		/* wait afterward */

#ifdef OLD				/* try to handle old versions */
	long	time();			/* of random number generators */
#define srandom	srand
#define random	rand
#else !OLD
	long	random();
#endif OLD

	while ((ch = getopt(argc,argv,"af:lmosw")) != EOF)
		switch((char)ch) {
			case 'a':	/* obscene, scene, limericks  */
				do_all = YES;
				break;
			case 'f':	/* different fortune file */
				ffile = optarg;
				break;
			case 'l':	/* long */
				do_long = YES;
				break;
			case 'm':	/* limericks */
				do_lim = YES;
				break;
			case 'o':	/* offensive */
				do_off = YES;
				break;
			case 's':	/* short */
				do_shrt = YES;
				break;
			case 'w':	/* wait afterward */
				do_wait = YES;
				break;
			default:
				fprintf(stderr,"usage: %s [-a] [-f file] [-l] [-m] [-o] [-s] [-w]\n",*argv);
				exit(ERR);
		}

	if (!(freopen(ffile,"r",stdin))) {
		perror(ffile);
		exit(ERR);
	}

	/* read in the table of addresses */

	fread(hold,sizeof(*hold),SECTIONS + 1,stdin);
	MM(long,seekpts,hold[SECTIONS],char);
	rewind(stdin);
	fread(seekpts,sizeof(*seekpts),(int)((hold[SECTIONS] + 1) / sizeof(*seekpts)),stdin);

	/* decide range of random number to generate */

	if (do_all) num = hold[END] - (bottom = hold[SCENE]);
	else if (do_off)
		if (do_lim) num = hold[END] - (bottom = hold[OBS]);
		else num = hold[OBSLIM] - (bottom = hold[OBS]);
	else if (do_lim) num = hold[END] - (bottom = hold[OBSLIM]);
	else num = hold[OBS] - (bottom = hold[SCENE]);

	/* get entries until MAXTRYs or find one that works */
	/* if can't find one, just give the user last one tested */

	srandom((int)time((long *)NULL));
	for (cnt = 0;cnt < MAXTRY;++cnt) {
		try = random() % num;
		len = seekpts[bottom + try + 1] - seekpts[bottom + try];
		if (do_shrt && len <= SLEN || do_long && len >= LLEN) break;
	}

	/* go to chosen fortune, dump it out */
	/* sleep if user requested it */

	fseek(stdin,seekpts[bottom + try],(int)0);
	for (cnt = len;cnt--;) putchar(getchar());
	if (do_wait) sleep(MAX(len/CPERS,MINW));
	exit(OK);
}
