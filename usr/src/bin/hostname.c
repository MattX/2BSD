#ifndef lint
static char *sccsid = "@@(#)hostname.c	1.4 (Berkeley) 8/11/83"; 
#endif
/*
 * hostname - get (or set) hostname
 */

#include	<stdio.h>
#include	<sys/param.h>

char hostname[32];
extern int errno;

main(argc, argv)
char	*argv[];
{
	int myerrno;

	argc--;
	argv++;
	if (argc) {
#ifdef	UCB_NET
		strcpy(hostname, *argv);
		if (sethostname(hostname, strlen(hostname)+1) < 0)
#else
		if (sethostname(*argv))
#endif	UCB_NET
			perror("sethostname");
		myerrno	= errno;
	}
	else	{
		gethostname(hostname,sizeof(hostname));
		myerrno = errno;
		printf("%s\n", hostname);
	}
	exit(myerrno);
}

#ifndef	UCB_NET
sethostname(s)
char *s;
{
	FILE	*fopen();
	register FILE	*fp;

	if ((fp = fopen("/etc/localhostname", "w")) != (FILE *) NULL) {
		fprintf(fp, "%s\n", s);
		fclose(fp);
		(void) chmod("/etc/localhostname", 0644);
		if (ferror(fp))
			return(-1);
		else
			return(0);
	}
	else
		return(-1);
}
#endif	UCB_NET
