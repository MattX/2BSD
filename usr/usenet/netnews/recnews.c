/*
 * Process a news article which has been mailed to some group like msgs.
 * Such articles are in normal mail format and have never seen the insides
 * of netnews.
 *
 * Recnews is primarily useful in remote places on the usenet which collect
 * mail from mailing lists and funnel them into the network.  It is also
 * useful if you like to send mail to some user instead of invoking
 * news -i .. -n .. when you want to submit an article.  (Many mailers give
 * you nice facilities like editing the message.)  It is not, however,
 * essential to use recnews to be able to join usenet.
 */

#include <stdio.h>
#define NETNEWS	BINDIR/netnews"
static char *sccsid = "@(#)recnews.c	1.4	12/3/80";

/*
 * Note: we assume there are 2 kinds of hosts using recnews:
 * Those that have delivermail (and hence this program will never
 * have to deal with more than one message at a time) and those on the arpanet
 * that do not (and hence all messages end with a sentenel).  It is
 * supposed that regular v7 type systems without delivermail or some
 * other automatic forwarding device will just use rnews.  We do
 * not attempt to tell where a message ends on all systems due to the
 * different conventions in effect.  (This COULD be fixed, I suppose.)
 */

/*
 * Kinds of lines in a message.
 */
#define FROM	01		/* From line */
#define SUBJ	02		/* Subject */
#define TO	03		/* To (newgroup based on this) */
#define BLANK	04		/* blank line */
#define EOM	05		/* End of message (4 ctrl A's) */
#define CC	06		/* Carbon copy, Mail version only */
#define OTHER	07		/* anything else */

/*
 * Possible states program can be in.
 */
#define SKIPPING	010	/* In header of message */
#define READING		020	/* In body of message */

#define BFSZ 250

#define TRUE	1
#define FALSE	0

#define EOT	'\004'

char	from[BFSZ];		/* mailing address of author */
char	to[BFSZ];		/* Destination of mail (msgs, etc) */
char	subject[BFSZ];		/* subject of message */
char	newsgroup[BFSZ];	/* newsgroups of message */
char	cmdbuf[BFSZ];		/* command to popen */

extern	char	*strcat(), *strcpy();
extern	FILE	*popen();
extern	char	*index();

main(argc, argv)
int argc;
char **argv;
{
	char buf[BFSZ];
	register char *p;
	register FILE *pipe;
	register int state, pathcnt;

	if (argc > 1)
		strcpy(to, argv[1]);
#ifdef debug
	printf("argv[0] is <%s>, argv[1] is <%s>\n", argv[0], argv[1]);
#endif
	state = SKIPPING;
	while (fgets(buf, BFSZ, stdin) != NULL) {
#ifdef debug
		printf("%o\t%s", type(buf) | state, buf);
#endif
		switch (type(buf) | state) {

		case FROM | SKIPPING:
			frombreak(buf, from);
			break;

		case FROM | READING:
			fputs(buf, pipe);
			break;

		case SUBJ | SKIPPING:
			p = index(buf, ' ');
			if (p == NULL)
				p = buf+8;
			strcpy(subject, p+1);
			subject[strlen(subject)-1] = 0;	/* trim trailing \n */
			break;

		case SUBJ | READING:
			fputs(buf, pipe);
			break;

		case TO | SKIPPING:
			if (to[0])
				break;		/* already have one */
			p = index(buf, ' ');
			if (p == NULL)
				p = buf;
			strcpy(to, p+1);	/* strip leading blank */
			to[strlen(to)-1] = 0;	/* strip trailing \n */
			break;
			
		case TO | READING:
			fputs(buf, pipe);
			break;
#ifdef CAPMAIL
		/*
		 * Kludge to compensate for bug in Mail where it doesn't
		 * put a blank line after the header of a piped message.
		 * Only needed where Mail is the forwarding device, not
		 * needed for delivermail or presumably things like mh.
		 */
		case CC | SKIPPING:
			break;

		case CC | READING:
			fputs(buf, pipe);
			break;
#endif

		case BLANK | SKIPPING:
			state = READING;
			findgroup(to, newsgroup);
			sprintf(cmdbuf, "%s -i \"%s\" -n %s -f %s", NETNEWS, subject, newsgroup, from);
#ifdef debug
			pipe = stdout;
			printf("%s\n", cmdbuf);
#else
			pipe = popen(cmdbuf, "w");
			if (pipe == NULL) {
				perror("recnews: popen failed");
				exit(1);
			}
#endif
			break;

		case BLANK | READING:
			fputs(buf, pipe);
			break;

		case OTHER | SKIPPING:
#ifdef CAPMAIL
			state = READING;
			findgroup(to, newsgroup);
			sprintf(cmdbuf, "%s -i \"%s\" -n %s", NETNEWS, subject, newsgroup);
#ifdef debug
			pipe = stdout;
			printf("%s\n", cmdbuf);
#else
			pipe = popen(cmdbuf, "w");
			if (pipe == NULL) {
				fprintf(stderr, "pipe failed\n");
				exit(1);
			}
#endif
			fputs(buf, pipe);
#endif
			break;

		case OTHER | READING:
			fputs(buf, pipe);
			break;
		}
	}
	exit(0);
}

type(p)
register char *p;
{
	while (*p == ' ' || *p == '?')
		++p;

	if (*p == '\n' || *p == 0)
		return(BLANK);
	if (strncmp(p, ">From", 5) == 0 || strncmp(p, "From", 4) == 0)
		return (FROM);
	if (strncmp(p, "Subj", 4)==0 || strncmp(p, "Re:", 3)==0)
		return (SUBJ);
	if (strncmp(p, "To", 2)==0)
		return(TO);
	if (strncmp(p, "\1\1\1\1", 4)==0)
		return(EOM);
#ifdef CAPMAIL
	if (strncmp(p, "Cc:", 3) == 0)
		return(CC);
#endif
	return(OTHER);
}

/*
 * Figure out who a message is from.
 */
frombreak(buf, fbuf)
register char *buf, *fbuf;
{
	register char *p;
	char wordfrom[BFSZ], uname[BFSZ], at[BFSZ], site[BFSZ];

	if (fbuf[0])	/* we already know who it's from */
		return;
	/* break the line into tokens. */
	sscanf(buf, "%s %s %s %s", wordfrom, uname, at, site);
	if (isat(at))
		/*
		 * Some arpanet mail comes from "joe at mit-dms"
		 * instead of "joe@mit-dms", so handle it here.
		 */
		sprintf(fbuf, "%s@%s", uname, site);
	else
		strcpy(fbuf, uname);
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 *
 * This routine stolen from v7 library to make this program portable.
 */

#define	NULL	0

char *
index(sp, c)
register char *sp, c;
{
	do {
		if (*sp == c)
			return(sp);
	} while (*sp++);
	return(NULL);
}

isat(str)
char *str;
{
	if (strcmp(str, "@")==0) return TRUE;
	if (strcmp(str, "at")==0) return TRUE;
	if (strcmp(str, "AT")==0) return TRUE;
	return FALSE;
}

findgroup(to, group)
char *to;
char *group;
{
#ifdef debug
	printf("findgroup(%s)\n", to);
#endif
#ifdef fussy
	/*
	 * Default unknown "to" fields to "general".  This gives you
	 * tight control over which newsgroups exist.
	 */
	if (strcmp(to, "msgs")==0)
		strcpy(group, "msgs");
	else if (strcmp(to, "allmsgs")==0)
		strcpy(group, "NET.allmsgs");
	else if (strcmp(to, "csmsgs")==0)
		strcpy(group, "NET.csmsgs");
	else
		strcpy(group, "general");
#else
	/*
	 * Allow any newsgroup.  This way you don't have to recompile
	 * recnews everytime you add a newsgroup.
	 */
	strcpy(group, to);
#endif
}
