static char sccsid[] = "@(#)timeat.c	1.0 (SRI) 6/5/83";
/*
 * User internet time program
 */
#include <stdio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <setjmp.h>

#define	SINCE1970	2208988800L	/* seconds from Jan 1, 1900 -> 1970 */

struct sockaddr_in sin;
int net;
time_t timebuf;
jmp_buf env;

char *ctime();
int reset();

main(argc, argv)
int argc;
char **argv;
{
	int n;
	struct servent *sp;
	struct hostent *hp;

	if (argc-- == 1) {
		fprintf(stderr, "usage: settime host [host] ...\n");
		exit(-1);
	}
	sp = getservbyname("time", "tcp");
	if (sp)
		sin.sin_port = sp->s_port;
	else
		perror("settime: service");
/*
	sin.sin_port = htons(sin.sin_port);
 */
	signal(SIGALRM, reset);
	while (argc--) {
		argv++;
		if ((hp = gethostbyname(*argv)) == NULL) {
			fprintf(stderr, "settime: %s: unknown host\n", *argv);
			continue;
		}
		sin.sin_family = hp->h_addrtype;
		bcopy(hp->h_addr, (caddr_t) &sin.sin_addr, hp->h_length);
		if (setjmp(env) == 1) {
			fprintf(stderr, "settime: %s: time out on connection\n", *argv);
			continue;
		}
		alarm(10);
		if ((net = socket(SOCK_STREAM, 0, 0, 0)) < 0) {
			fprintf(stderr, "settime: %s: ", *argv);
			perror("socket");
			continue;
		}
		if (connect(net, &sin)) {
			fprintf(stderr, "settime: %s: ", *argv);
			perror("connect");
			continue;
		}
			/* read 32 bit entity */
		n = read(net, &timebuf, sizeof(timebuf));
		alarm(0);
		close(net);
		if (n != sizeof(timebuf) || timebuf == 0L) {
			fprintf(stdout, "settime: time not available from %s\n",*argv);
			continue;
		}
		timebuf = ntohl(timebuf);
		timebuf -= SINCE1970;
		printf("settime: %s: %s", *argv, ctime(&timebuf));
		if (stime(&timebuf) < 0) {
			fprintf(stderr, "settime: can't set time; not super-user\n");
			exit(-1);
		}
		exit(0);
	}
	fprintf(stdout, "settime: couldn't set the time; use date command\n");
}

static
reset()
{
	signal(SIGALRM, reset);
	(void) close(net);
	longjmp(env, 1);
}
