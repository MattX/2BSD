#ifndef lint
static char sccsid[] = "@(#)named.c	4.4 82/10/10";
#endif

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

#define	ARPNAM	01
#define	ARPADDR	02
#define	ARPERR	03
struct	sockaddr_in sin = { AF_INET };
extern	errno;
int	s;

main()
{
	struct sockaddr_in from;
	struct servent *sp;

	sp = getservbyname("name", "udp");
	if (sp == 0) {
		fprintf(stderr, "named: udp/name: unknown service\n");
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
		perror("named: socket");
		sleep(5);
		goto again;
	}
	for (;;) {
		struct namevl {
			char n_type;
			char n_len;
			char n_name[280];
		} namevl;
		int len;
		struct hostent *hp;
		int cc;
		char *p;

		cc = receive(s, &from, (char *)&namevl, sizeof (namevl));
		if (cc < 3) {
			if (cc < 0 && errno != EINTR)
				perror("named: receive");
			continue;
		}
		if (namevl.n_type != ARPNAM)
			continue;
		len = namevl.n_len & 0377;
		if (cc != (len+2))
			continue;
		namevl.n_name[len] = '\0';
		if ((p = rindex(namevl.n_name, '!')) != NULL)
			p++;
		else
			p = namevl.n_name;
		if ((hp = gethostbyname(p)) != NULL) {
			namevl.n_name[len] = ARPADDR;
			namevl.n_name[len+1] = (char)(hp->h_length);
			strncpy(&namevl.n_name[len+2], hp->h_addr, hp->h_length);
			len += 4+hp->h_length;
		} else {
			namevl.n_name[len] = ARPERR;
			namevl.n_name[len+1] = (char)(strlen("Not found"));
			namevl.n_name[len+2] = 01;
			strcpy(&namevl.n_name[len+3], "Not found");
			len += 5+strlen("Not found");
		}
		if (send(s, &from, (char *)&namevl, len) < 0) {
			perror("named: send");
		}
	}
}
