#include <stdio.h>
static char *sccsid = "@(#)uurec.c	1.3	9/14/80";

/*
 * Process a news article which has been shipped via /bin/mail.
 */

#define FROM	01
#define NLIN	02
#define BLANK	03
#define OTHER	04

#define SKIPPING	010
#define READING		020

#define BFSZ 250

#define TRUE	1
#define FALSE	0

#define EOT	'\004'

#ifdef debug
# define RNEWS "cat"
#else
/*
 * rnews MUST be in /usr/bin so uuxqt can find it.
 */
# define RNEWS	"/usr/bin/rnews"
#endif
extern	char	*strcat(), *strcpy();
extern	char	*frombreak();
extern	FILE	*popen();

/* ARGSUSED */
main(argc, argv)
int argc;
char **argv;
{
	char buf[BFSZ], fbuf[BFSZ];
	register char *p;
	register FILE *pipe = stdout;
	register int mode, frmflg, pathcnt;

	mode = SKIPPING;
	frmflg = FALSE;
	while (fgets(buf, BFSZ, stdin) != NULL) {
#ifdef debug
		printf("%o\t%s", mode|type(buf), buf);
#endif
		switch (mode | type(buf)) {

		case FROM | SKIPPING:
			if (frmflg)
				p = frombreak(p, buf);
			else
				p = fbuf;
			frmflg = TRUE;
			break;

		case FROM | READING:
			if (!frmflg) {
				frmflg = TRUE;
				p = fbuf;
				pclose(pipe);
			}
			p = frombreak(p, buf);
			break;

		case NLIN | SKIPPING:
			mode = READING;

		case NLIN | READING:
			if (frmflg) {
				frmflg = FALSE;
				--p;
				while (p >= fbuf && *--p != '!')
					;
				*++p = '\0';
				pathcnt = 0;
				if ((pipe = popen(RNEWS, "w")) == NULL) {
					perror("uurec: popen failed");
					exit(1);
				}
			}
			if (++pathcnt == 3)
				fputs(fbuf, pipe);
			fputs(buf+1, pipe);
			break;

		case OTHER | SKIPPING:
			break;

		case OTHER | READING:
			pclose(pipe);
			mode = SKIPPING;
		}
	}
	if (pipe)
		pclose(pipe);
	exit(0);
}

type(p)
register char *p;
{
	while (*p == ' ' || *p == '?')
		++p;

	if (*p == 'N')
		return (NLIN);

	if (strncmp(p, ">From", 5) == 0)
		return (FROM);

	if (strncmp(p, "From", 4) == 0)
		return (FROM);

	return(OTHER);
}

/*
 * Get the system name out of a from line.
 */
char *
frombreak(buf, fbuf)
register char *buf, *fbuf;
{
	register char *p;

	/* break the line into tokens. */
	p = fbuf;
	while (*++p != '\0')
		switch (*p) {
		case '\n':
		case '\t':
		case ' ':
			*p = '\0';
			break;
		case EOT:
			goto garbled;
		default:;
		}
	*++p = EOT;
	*++p = '\0';

	for (p=fbuf; *p != EOT  || p[1] != '\0'; p += strlen(p)+1) {
		if (strcmp(p, "forwarded") == 0)
			return(buf);
		if (strcmp(p, "remote") == 0) {
			p += strlen(p)+1;
			if (strcmp(p, "from") == 0) {
				p += strlen(p)+1;
				strcpy(buf, p);
				strcat(buf, "!");
				return(buf+strlen(buf));
			}
		}
	}
    garbled:
	strcat(buf, "???!");
	return(buf+4);
}
