#include <stdio.h>

main(argc,argv)
int	argc;
char	**argv;
{
	register char *bp;
	char	buf[200],
		*index();

	gets(buf);
	if (bp = index(buf,'\r')) *bp = 0;
	if (buf[0]) execl("/usr/local/finger", "finger", buf, 0);
	else execl("/usr/local/finger", "finger", 0);
	fprintf(stderr,"%s: can't find finger program.\n",*argv);
}
