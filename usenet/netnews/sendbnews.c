/*
 * sendbnews: post processing filter used to send mail from a berknet host.
 * Basic purpose is to change the address from
 *	uucphost!xxxxxx
 * to
 *	berkhost:xxxxxx
 *
 * Since we also really should call sed to put N's at the beginning of
 * each line, we do that too, and fork off mail.
 */
#include <stdio.h>
static char *sccsid = "@(#)sendbnews.c	1.5	2/10/81";
char *index();

FILE *popen();
main(argc, argv)
char **argv;
{
	FILE *out;
	char buffer[BUFSIZ];
	char sender[BUFSIZ];
	char *punct;
	int linecount = 0;

#ifdef debug
	printf("mail %s\n", argv[1]);
	sprintf(buffer, "cat");
#else
	sprintf(buffer, "mail %s", argv[1]);
#endif
	out = popen(buffer, "w");

	/* Standard mail prelude to make the formatters happy */
	fprintf(out, "To: %s\n", argv[1]);
	fprintf(out, "Subject: network news article\n");
	fprintf(out, "\n");

	while (fgets(buffer, sizeof buffer, stdin)) {
		if (++linecount == 3) {
			punct = index(buffer, '!');
			if (punct == NULL)
				printf("Bad from line: '%s'\n", buffer);
			else {
				*punct = ':';	/* berknet mail delimeter */
				fiddle(buffer);
			}
		}
		putc('N', out);
		fputs(buffer, out);
	}
	pclose(out);
	exit(0);
}

/*
 * make sure the host name is a correct berknet address, since the
 * internal names are not the berknet host names.
 */
fiddle(buf)
char *buf;
{
	char berkname[10];
	char uucpname[100];
	char *rest;

	strcpy(uucpname, buf);
	rest = index(uucpname, ':');
	*rest++ = 0;
#ifdef debug
	printf("uucpname='%s', buf='%s', rest='%s',...", uucpname, buf, rest);
#endif
	if (eq(uucpname, "ucbvax"))
		strcpy(berkname, "csvax");
	else if (eq(uucpname, "ucbcory"))
		strcpy(berkname, "cory");
	else if (eq(uucpname, "ucbopt"))
		strcpy(berkname, "optvax");
		/*
		 * The uucp names from here down are guesswork.
		 * They may have to be changed later.  But we can't
		 * allow names like a, b, etc to get outside the berknet.
		 */
	else if (eq(uucpname, "ucbing70"))
		strcpy(berkname, "ing70");
	else if (eq(uucpname, "ucbingvax"))
		strcpy(berkname, "ingvax");
	else if (eq(uucpname, "ucbcfo-a"))
		strcpy(berkname, "a");
	else if (eq(uucpname, "ucbcfo-b"))
		strcpy(berkname, "b");
	else if (eq(uucpname, "ucbcfo-c"))
		strcpy(berkname, "c");
	else if (eq(uucpname, "ucbcfo-d"))
		strcpy(berkname, "d");
	else if (eq(uucpname, "ucbcfo-e"))
		strcpy(berkname, "e");
	else if (eq(uucpname, "ucbcfo-f"))
		strcpy(berkname, "f");
	else if (eq(uucpname, "ucbcfo-g"))
		strcpy(berkname, "g");
	else if (eq(uucpname, "ucbeecs40"))
		strcpy(berkname, "eecs40");
	else if (eq(uucpname, "ucbimage"))
		strcpy(berkname, "image");
	else if (eq(uucpname, "ucbsrc"))
		strcpy(berkname, "src");
	else if (eq(uucpname, "ucbarpa"))
		strcpy(berkname, "arpavax");
	else
		strcpy(berkname, "BADBAD");
	sprintf(buf, "%s:%s", berkname, rest);
#ifdef debug
	printf("berkname='%s', buf='%s'\n", berkname, buf);
#endif
}

eq(a, b)
char *a, *b;
{
	return strcmp(a, b) == 0;
}

/*
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */

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
