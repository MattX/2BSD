#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <strfile.h>

/*
 * create the database from the various fortune files
 *
 *	Keith Bostic
 *		ARPA: keith@seismo
 *		UUCP: seismo!keith
 */

#define LSIZE		100		/* max single line length */

main(argc,argv)
int	argc;
char	**argv;
{
	extern char	*optarg;	/* getopts variable */
	static char	del_str[3] = "%%";	/* delimiter string */
	register long	*lp,			/* pointer for seek table */
			*seekpts;		/* seek table */
	register int	len;			/* length of fortune */
	STRF	*spnt;			/* file table */
	long	thold,			/* hold a time */
		time(), ftell();
	int	ch,			/* argument character */
		longest,		/* longest fortune */
		numforts,		/* total number of fortunes */
		shortest;		/* shortest fortune */
	short	match = NO;		/* flag for empty fortunes */
	char	*ffile = OUTFILE,	/* standard output file */
		lbuf[LSIZE],		/* hold each line of fortune */
		*ctime();

	while ((ch = getopt(argc,argv,"c:f:")) != EOF)
		switch((char)ch) {
			case 'c':	/* new delimiting char */
				del_str[0] = del_str[1] = *optarg;
				break;
			case 'f':	/* new output file name */
				ffile = optarg;
				break;
			default:
				fprintf(stderr,"usage: %s [-cC] [-f file]\n",*argv);
				exit(ERR);
		}

	time(&thold);			/* print pretty label */
	fprintf(stderr,"==== FORTUNE FILE ==== %s",ctime(&thold));

	/* pass 1: find out how many strings there are */

	for (numforts = 0,spnt = tbl;*spnt->fname;++spnt) {
		if (access(spnt->fname,F_OK)) {
			fprintf(stderr,"%s: unable to find the file.\n",spnt->fname);
			exit(ERR);
		}
		if (!(freopen(spnt->fname,"r",stdin))) {
			perror(spnt->fname);
			exit(ERR);
		}			/* try to ignore empty ones */
		for (match = YES;gets(lbuf);)
			if (!strcmp(lbuf,del_str)) {
				if (!match) {
					++numforts;
					match = YES;
				}
			}
			else match = NO;
		if (!match) {		/* what if just some empty lines? */
			fprintf(stderr,"there are lines at the end of file %s not preceding a delimiter.\n",spnt->fname);
			exit(ERR);
		}
	}

	/* save space at beginning of file for tables */

	if (!(freopen(ffile,"w",stdout))) {
		perror(ffile);
		exit(ERR);
	}
	MM(long,seekpts,numforts + SECTIONS + 1,long);
	fseek(stdout,(long)((numforts + SECTIONS + 1) * sizeof(*seekpts)),0);

	/* pass 2: write the strings into the file, set offsets */
	/* again, ignore empty fortunes; will go away after "unstr". */

	for (lp = seekpts + SECTIONS,spnt = tbl;*spnt->fname;++spnt)
		if (!(freopen(spnt->fname,"r",stdin))) {
			perror(spnt->fname);
			exit(ERR);
		}
		else {
			seekpts[spnt->entry] = lp - seekpts;
			for (*lp = ftell(stdout);gets(lbuf);)
				if (strcmp(lbuf,del_str)) {
					puts(lbuf);
					match = NO;
				}
				else if (!match) {
					match = YES;
					++lp;
					len = (*lp = ftell(stdout)) - lp[-1] - 1;
					if (lp > seekpts + SECTIONS + 1) {
						if (longest < len) longest = len;
						else if (shortest > len) shortest = len;
					}
					else longest = shortest = len;
					++spnt->number;
				}
			fprintf(stderr,"%ld\tfortunes in section %ld (%s)\n",spnt->number,spnt->entry + 1,spnt->fname);
		}
	seekpts[spnt->entry] = lp - seekpts;
	fprintf(stderr,"%d\tcharacters in the longest fortune.\n%d\tcharacters in the shortest fortune.\n",longest,shortest);

	/* write the tables into the file */

	rewind(stdout);
	fwrite(seekpts,sizeof(*seekpts),numforts + SECTIONS + 1,stdout);
	exit(OK);
}
