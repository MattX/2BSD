/*
 * unbatchnews: extract news in batched format and process it one article
 * at a time.  The format looks like
 *	#! rnews 1234
 *	article containing 1234 characters
 *	#! rnews 4321
 *	article containing 4321 characters
 *
 *	or
 *
 *	#! cunbatch
 *	decompresses batch of articles in above format
 */

#ifdef SCCSID
static char	*SccsId = "@(#)unbatch.c	1.13	4/16/85";
#endif /* SCCSID */

#include <stdio.h>
#define void int

char buf[BUFSIZ];
char sibuf[BUFSIZ];

main()
{
	register int c;
	register FILE *pfn;
	register long size;
	char *filename;
	int pid, wpid, exstat;
	char *mktemp(), *gets();
	long atol();

	filename = mktemp("/tmp/unbnewsXXXXXX");
	setbuf(stdin, (char *)NULL);	/* only for the first line */
	if (gets(buf) == NULL) {
		(void) unlink(filename);
		exit(0);
	}
	if (strncmp(buf, "#! cunbatch", 11) == 0) {
#ifdef IHCC
		(void) sprintf(buf, "%s/%s/compress -d | %s/%s/rnews" logdir(HOME),
			LIBDIR, logdir(HOME), LIBDIR);
#else
		(void) sprintf(buf, "%s/compress -d | %s/rnews", LIBDIR, BINDIR);
#endif
		execl("/bin/sh", "sh", "-c", buf, (char *)0);
		perror("/bin/sh");
		exit(1);
	}

	setbuf(stdin, sibuf);	/* buffer the rest of the file */

	do {
		while (strncmp(buf, "#! rnews ", 9)) {
			fprintf(stderr, "out of sync, skipping %s\n", buf);
			if (gets(buf) == NULL)
				exit(0);
		}
		size = atol(buf+9);
		if(size <= 0)
			break;
#ifdef VMS
		vmsdelete(filename);
#endif /* VMS */
		pfn = fopen(filename, "w");
		while(--size >= 0 && (c = getc(stdin)) != EOF)
			putc(c, pfn);
		(void) fclose(pfn);

		/*
		 * If we got a truncated batch, don't process the
		 * last article; it will probably be received again.
		 */
		if (size > 0)
			break;

		/*
		 * rnews < filename
		 */
		while ((pid = fork()) == -1) {
			fprintf(stderr, "fork failed, waiting...\r\n");
			sleep(60);
		}
		if (pid == 0) {
			(void) close(0);
			(void) open(filename, 0);
#ifdef IHCC
			(void) sprintf(buf, "%s/%s/rnews", logdir(HOME), LIBDIR);
			execlp(buf, "rnews", (char *)0);
#else
			execlp("rnews", "rnews", (char *)0);
#endif
			perror("rnews");
			exit(1);
		}
		while ((wpid = wait(&exstat)) >= 0 && wpid != pid)
			;
	} while (gets(buf) != NULL);
	(void) unlink(filename);
	exit(0);
}
