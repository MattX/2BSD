/*
 * Exit with value 0 if user has news, else 1.
 * If 'y' option is given, prints message if user has news.
 * If 'n' option is given, prints message if user does not have news.
 */
char	*BITFILE = NEWSDIR/.bitfile";
static char *sccsid = "@(#)nchk.c	1.3	9/22/80";

extern	long	lseek();

main(argc, argv)
int argc;
char **argv;
{
	register int e, y, n, rc;

	y = 0;
	n = 0;
	e = 0;
	if (--argc > 0) {
		for (argv++; **argv; ++*argv) {
			switch(**argv) {
			case 'y':
			case 'q':
				y++;
				break;
			case 'n':
				n++;
				break;
			case 'e':
			case 'f':
				e++;
				break;
			}
		}
	}
#ifdef PWB
	rc = newscheck(getuid() & 0377);
#else
	rc = newscheck(getuid());
#endif
	if (rc) {
		if (y)
			write(1, "You have news.\n", 15);
	}
	else {
		if (n)
			write(1, "No news.\n", 9);
	}
	if (rc && e) {
		*argv = "news";
#ifdef PWB
		execv("netnews", argv);
		execv("news", argv);
#else
		execvp("netnews", argv);
		execvp("news", argv);
#endif
		printf("Cannot exec news\n");
	}
	exit(!rc);
}

/* return 1 if user has news, else 0. */
newscheck(uid)
register int uid;
{
	register int fd;
	static char c;
	fd = open(BITFILE, 0);
	if (fd < 0) {
		printf("can't open BITFILE\n");
		exit(1);
	}
	lseek(fd, (long)(uid>>3), 0);
	read(fd, &c, 1);
	close(fd);
	return((c >> (uid&07)) & 01);
}
