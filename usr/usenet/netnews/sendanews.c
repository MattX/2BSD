/*
 * sendanews: post processing filter specific to ucbvax, because it must
 * serve as a gateway between the uucp net and the arpanet.
 * Basic purpose is to change the address from
 *	ucbvax!xxxxxx
 * to
 *	csvax.xxxxxx@berkeley
 *
 * Since we also really should call sed to put N's at the beginning of
 * each line, we do that too, and fork off mail.
 */
#include <stdio.h>
#define LNLEN	7	/* strlen("ucbvax!") */
static char *sccsid = "@(#)sendanews.c	1.3	9/14/80";

FILE *popen();
main(argc, argv)
char **argv;
{
	FILE *out;
	char buffer[BUFSIZ];
	char sender[BUFSIZ];
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
			if (strncmp(buffer, "ucbvax!", LNLEN))
				printf("Bad from line: '%s'\n", buffer);
			strcpy(sender, buffer+LNLEN);
			sender[strlen(sender)-1] = 0;
			sprintf(buffer, "csvax.%s@berkeley\n", sender);
		}
		putc('N', out);
		fputs(buffer, out);
	}
	pclose(out);
	exit(0);
}
