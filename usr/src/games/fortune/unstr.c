#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <strfile.h>

/*
 * create the various fortune files from the database file
 *
 *	Keith Bostic
 *		ARPA: keith@seismo
 *		UUCP: seismo!keith
 */

main(argc,argv)
int	argc;
char	**argv;
{
	extern char	*optarg;		/* getopts variable */
	static char	del_str[3] = "%%";	/* delimiter string */
	register long	choff,			/* fortune offset */
			off,			/* travel through table */
			*seekpts;		/* hold table */
	register int	cnt;			/* general counter */
	long	hold[SECTIONS + 1];		/* part of table */
	STRF	*spnt;				/* table structure pointer */
	int	ch;				/* argument character */
	short	force = NO;			/* if overwrite files */
	char	*ffile = OUTFILE;		/* standard fortune file */

	while ((ch = getopt(argc,argv,"c:f:o")) != EOF)
		switch((char)ch) {
			case 'c':	/* new delimiting char */
				del_str[0] = del_str[1] = *optarg;
				break;
			case 'f':	/* new fortune file */
				ffile = optarg;
				break;
			case 'o':	/* overwrite existing files */
				force = YES;
				break;
			default:
				fprintf(stderr,"usage: %s [-o] [-cC] [-f file]\n",*argv);
				exit(ERR);
		}
	if (!freopen(ffile,"r",stdin)) {
		perror(ffile);
		exit(ERR);
	}

	/* read the table */

	fread(hold,sizeof(*hold),SECTIONS + 1,stdin);
	MM(long,seekpts,hold[SECTIONS],char);
	rewind(stdin);
	fread(seekpts,sizeof(*seekpts),(int)((hold[SECTIONS] + 1) / sizeof(*seekpts)),stdin);

	/* read the strings, addresses in the table delimit the	*/
	/* fortunes, not EOS's as original fortune(6) did	*/

	fseek(stdin,seekpts[*seekpts],(long)0);
	for (spnt = tbl;*spnt->fname;++spnt) {
		if (!access(spnt->fname,F_OK) && !force) {
			fprintf(stderr,"%s: %s would get overwritten.\n",*argv,spnt->fname);
			continue;
		}
		if (!freopen(spnt->fname,"w",stdout)) {
			perror(spnt->fname);
			exit(ERR);
		}
		for (cnt = 0,off = seekpts[spnt->entry];off < seekpts[spnt->entry + 1];++off,++cnt) {
			for (choff = seekpts[off];choff < seekpts[off + 1];++choff)
				putchar(getchar());
			puts(del_str);
		}
		fprintf(stderr,"%d\tfortunes placed in %s.\n",cnt,spnt->fname);
	}
	exit(OK);
}
