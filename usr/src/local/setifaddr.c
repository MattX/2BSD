#ifndef lint
static char sccsid[] = "@(#)setifaddr.c	4.1 3/15/83";
#endif

#include <stdio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>
#include <ctype.h>

struct	ifreq ifr;
struct	sockaddr_in sin = { AF_INET };

main(argc, argv)
	int argc;
	char *argv[];
{
	int s;

	if (argc < 3) {
		printf("usage: setifaddr interfacename addr ...\n");
		exit(1);
	}
	s = socket(SOCK_RAW, 0, 0, 0);
	if (s < 0) {
		perror("setifaddr: socket");
		exit(1);
	}
	for (argc--, argv++; argc >= 2; argc -= 2, argv += 2) {
		getaddr(argv[1], (struct sockaddr_in *)&ifr.ifr_addr);
		strcpy(ifr.ifr_name, argv[0]);
		if (ioctl(s, SIOCSIFADDR, (caddr_t)ifr.ifr_name)) {
			error("setifaddr");
		}
	}
	exit(0);
}

error(cmd)
	char *cmd;
{
	extern int errno;

	if (errno == ENXIO)
		fprintf(stderr, "bad interface name; good example: ec0\n");
	else
		perror(cmd);
}

getaddr(s, sin)
	char *s;
	struct sockaddr_in *sin;
{
	struct hostent *hp;
	struct netent *np;
	u_long val;

	hp = gethostbyname(s);
	if (hp) {
		sin->sin_family = hp->h_addrtype;
	/* bcopy converts address to host order */
		bcopy(hp->h_addr, (char *)&sin->sin_addr, hp->h_length);
		return;
	}
	np = getnetbyname(s);
	if (np) {
		sin->sin_family = np->ne_addrtype;
		sin->sin_addr.s_addr = inet_makeaddr(np->ne_net, INADDR_ANY);
		return;
	}
	sin->sin_family = AF_INET;
	val = inet_addr(s);
	if (val != -1) {
		sin->sin_addr.s_addr = val;
		return;
	}
	val = inet_network(s);
	if (val != -1) {
		sin->sin_addr.s_addr = inet_makeaddr(val, INADDR_ANY);
		return;
	}
	fprintf(stderr, "%s: bad value\n", s);
	exit(1);
}
