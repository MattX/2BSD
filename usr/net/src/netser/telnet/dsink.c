#include <whoami.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define	wait3(a,b,c) wait2(&a,b)

#define	INFINITY	((long)10000000)
#define	BELL		'\07'

char buff[1024];
extern	int errno;
char	hostnm[33];

struct	sockaddr_in sin = { AF_INET };
int	options = SO_ACCEPTCONN|SO_KEEPALIVE;

main(argc, argv)
	char *argv[];
{
	int s, pid;
	union wait status;
	struct servent *sp;

	sp = getservbyname("dsink", "tcp");
	if (sp == 0) {
		fprintf(stderr, "telnetd: tcp/telnet: unknown service\n");
		exit(1);
	}
	gethostname(hostnm, sizeof(hostnm));
	sin.sin_port = ntohs(sp->s_port);
	sin.sin_port = htons(sin.sin_port);
	for (;;) {
		errno = 0;
		if ((s = socket(SOCK_STREAM, 0, &sin, options)) < 0) {
			perror("socket");
			sleep(5);
			continue;
		}
		if (accept(s, 0) < 0) {
			perror("accept");
			close(s);
			sleep(1);
			continue;
		}
		while(read(s, buff, 1024) > 0) ;
		close(s);
		while (wait3(status, WNOHANG, 0) > 0)
			continue;
	}
	/*NOTREACHED*/
}
