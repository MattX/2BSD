#ifndef lint
static char sccsid[] = "@(#)route.c	4.3 82/10/07";
#endif

#include <stdio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/route.h>
#include <netinet/in.h>
#include <errno.h>
#include <ctype.h>

struct	rtentry route;
int	options;
int	s;
struct	sockaddr_in sin = { AF_INET };

u_long	inet_makeaddr();

main(argc, argv)
	int argc;
	char *argv[];
{

	if (argc < 2)
		printf("usage: route [ cmd ] [ args ]\n"), exit(1);
	s = socket(SOCK_RAW, 0, 0, options);
	if (s < 0) {
		perror("socket");
		exit(1);
	}
	argc--, argv++;
	if (strcmp(*argv, "add") == 0)
		newroute(argc, argv);
	else if (strcmp(*argv, "delete") == 0)
		newroute(argc, argv);
	else if (strcmp(*argv, "change") == 0)
		changeroute(argc-1, argv+1);
	else
		printf("%s: huh?\n", *argv);
}

char *
routename(in)
	struct in_addr in;
{
	char *cp = 0;
	static char line[50];
	long lna, net;

	net = inet_netof(in.s_addr);
	lna = inet_lnaof(in.s_addr);
	if (net == INADDR_ANY && lna == INADDR_ANY)
			cp = "default";
	else
		if (lna == INADDR_ANY) {
			struct netent *np = getnetbyaddr(net, AF_INET);

			if (np)
				cp = np->ne_name;
		} else {
			struct hostent *hp;

			hp = gethostbyaddr(&in, sizeof (struct in_addr),
				AF_INET);
			if (hp)
				cp = hp->h_name;
			}
	if (cp)
		strcpy(line, cp);
	else {
		u_char *ucp = (u_char *)&in;
		if (lna == INADDR_ANY)
			sprintf(line, "%u.%u.%u",ucp[0]&0377,ucp[1]&0377,
				ucp[2]&0377);
		else
			sprintf(line, "%u.%u.%u.%u",ucp[0]&0377,ucp[1]&0377,
				ucp[2]&0377,ucp[3]&0377);
	}
	return (line);
}

newroute(argc, argv)
	int argc;
	char *argv[];
{
	struct sockaddr_in *sin;
	char *cmd;

	if (argc < 3 || argc > 4) {
		printf("usage: %s destination gateway [ metric ]\n", argv[0]);
		return;
	}
	cmd = argv[0];
	ngetaddr(argv[1], &route.rt_dst);
	hgetaddr(argv[2], &route.rt_gateway);
	sin = (struct sockaddr_in *)&route.rt_dst;
	route.rt_flags = RTF_UP;
	if (inet_lnaof(sin->sin_addr) != 0L)
		route.rt_flags |= RTF_HOST;
	if (argc > 3 && atoi(argv[3]) > 0)
		route.rt_flags |= RTF_GATEWAY;

	printf("%s %s(%u): gateway ", 
		cmd, routename(sin->sin_addr),sin->sin_family);
	sin = (struct sockaddr_in *)&route.rt_gateway;
	printf("%s(%u), flags %x\n",
		routename(sin->sin_addr),sin->sin_family, route.rt_flags);
/*
	route.rt_gateway.sin_addr.s_addr = 
		ntohl(route.rt_gateway.sin_addr.s_addr);
	route.rt_dst.sin_addr.s_addr = 
		ntohl(route.rt_dst.sin_addr.s_addr);
*/
	if (ioctl(s, *cmd == 'a' ? SIOCADDRT : SIOCDELRT, (caddr_t)&route))
		error(cmd);
}

changeroute(argc, argv)
	int argc;
	char *argv[];
{
	printf("not supported\n");
}

error(cmd)
	char *cmd;
{
	extern int errno;

	if (errno == ESRCH)
		fprintf(stderr, "not in table\n");
	else if (errno == EBUSY)
		fprintf(stderr, "entry in use\n");
	else if (errno == ENOBUFS)
		fprintf(stderr, "routing table overflow\n");
	else
		perror(cmd);
}

ngetaddr(s, sin)
	char *s;
	struct sockaddr_in *sin;
{
	struct hostent *hp;
	struct netent *np;
	u_long val;

	if (strcmp(s, "default") == 0) {
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = inet_makeaddr((u_long) 0, INADDR_ANY);
		sin->sin_addr.s_addr = htonl(sin->sin_addr.s_addr);
		return;
	}

	np = getnetbyname(s);
	if (np) {
		sin->sin_family = AF_INET;
		sin->sin_addr.s_addr = inet_makeaddr((u_long) np->ne_net, INADDR_ANY);
/*		sin->sin_addr.s_addr = htonl(sin->sin_addr.s_addr);*/
		return;
	}
	sin->sin_family = AF_INET;
	val = inet_network(s);
	if (val != -1) {
		sin->sin_addr.s_addr = inet_makeaddr((u_long) val, INADDR_ANY);
		return;
	}
	fprintf(stderr, "%s: bad value\n", s);
	exit(1);
}

hgetaddr(s, sin)
	char *s;
	struct sockaddr_in *sin;
{
	struct hostent *hp;
	u_long val;

	hp = gethostbyname(s);
	if (hp) {
		sin->sin_family = AF_INET;
		bcopy(hp->h_addr, &sin->sin_addr, hp->h_length);
		return;
	}
	sin->sin_family = AF_INET;
	val = inet_addr(s);
	if (val != -1) {
		sin->sin_addr.s_addr = val;
		return;
	}
}
