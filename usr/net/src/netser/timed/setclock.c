#ifndef lint
static char sccsid[] = "@(#)setclock.c	4.4 82/10/10";
#endif
/*
 * This utility uses an arpa time server on a remote host to set
 * the system's time. It is intended to be called from /etc/rc on
 * network hosts that don't have battery backed up time of day clocks
 * so that the system will reboot with a valid idea of the time.
 * It waits a maximum of MAXRETRY*TIMEOUT seconds before giving up
 */

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <netdb.h>
#ifdef	pdp11
#define	wait3(a,b,c) wait2(&a,b)
#endif

#define	ARPTIME	2208988800l
#define	MAXRETRY 15
#define TIMEOUT  60
struct	sockaddr_in sin = { AF_INET };
time_t	time();
extern	errno;
int	s, timeout = 0, timer();

main(argc, argv)
	int argc;
	char *argv[];
{
	struct sockaddr_in from;
	struct servent *sp;
	struct hostent *hp;

	if (argc != 2) {
		fprintf(stderr, "Usage: setclock [host...]\n");
		exit(3);
	}
	sp = getservbyname("time", "udp");
	if (sp == 0) {
		fprintf(stderr, "setclock: udp/time: unknown service\n");
		exit(1);
	}
	hp = gethostbyname(argv[1]);
	if (hp == 0) {
		fprintf(stderr, "Can't find host\n");
		exit(2);
	}
	(void) sigset(SIGHUP, SIG_IGN);
	(void) sigset(SIGALRM, timer);
	sin.sin_port = sp->s_port;
	bcopy(hp->h_addr, &sin.sin_addr, hp->h_length);
again:
	if ((s = socket(SOCK_DGRAM, 0, 0, 0)) < 0) {
		perror("setclock: socket");
		sleep(5);
		goto again;
	}
	for (;;) {
		time_t ltime;
		int cc;

		if (send(s, &sin, (char *)&ltime, sizeof(ltime)) < 0) {
			perror("setclock: send");
		}
		alarm(TIMEOUT);
		cc = receive(s, &from, (char *)&ltime, sizeof (ltime));
		if (cc < sizeof(ltime)) {
			if (cc < 0 && errno != EINTR)
				perror("setclock: receive");
			alarm(0);
			continue;
		}
		alarm(0);
		ltime = ntohl(ltime)-ARPTIME;
		stime(&ltime);
		exit();
	}
}
timer()
{
	if (++timeout > MAXRETRY)
		exit(5);
}
