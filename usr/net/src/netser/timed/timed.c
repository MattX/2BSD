#ifndef lint
static char sccsid[] = "@(#)timed.c	4.4 82/10/10";
#endif
/*
 * Implements the arpa standard time daemon.
 * Used by setclock on other systems.
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
struct	sockaddr_in sin = { AF_INET };
time_t	time();
extern	errno;
int	s;

main()
{
	struct sockaddr_in from;
	struct servent *sp;

	sp = getservbyname("time", "udp");
	if (sp == 0) {
		fprintf(stderr, "timed: udp/time: unknown service\n");
		exit(1);
	}
	sin.sin_port = sp->s_port;
#ifndef DEBUG
	if (fork())
		exit(0);
	{ int s;
	  for (s = 0; s < 10; s++)
		(void) close(s);
	  (void) open("/", 0);
	  (void) dup2(0, 1);
	  (void) dup2(0, 2);
	  s = open("/dev/tty", 2);
	  if (s >= 0) {
		ioctl(s, TIOCNOTTY, 0);
		(void) close(s);
	  }
	}
#endif
	(void) signal(SIGHUP, SIG_IGN);
again:
	if ((s = socket(SOCK_DGRAM, 0, &sin, SO_ACCEPTCONN)) < 0) {
		perror("timed: socket");
		sleep(5);
		goto again;
	}
	for (;;) {
		time_t ltime;
		int cc;

		cc = receive(s, &from, (char *)&ltime, sizeof (ltime));
		if (cc < 0) {
			if (cc < 0 && errno != EINTR)
				perror("timed: receive");
			continue;
		}
		ltime = htonl(time(0)+ARPTIME);
		if (send(s, &from, (char *)&ltime, sizeof(ltime)) < 0) {
			perror("timed: send");
		}
	}
}
