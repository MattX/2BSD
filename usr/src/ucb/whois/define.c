#ifndef lint
static char sccsid[] = "@(#)define.c	4.2 83/06/10";
#endif

#include <stdio.h>
#include <whoami.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#define	NICHOST	"sri-nic"

main(argc, argv)
	int argc;
	char *argv[];
{
	int s;
	register FILE *sfi, *sfo;
	register char c;
	char *host = NICHOST;
	struct sockaddr_in sin;
	struct hostent *hp;
	struct servent *sp;

	argc--, argv++;
	if (argc > 2 && strcmp(*argv, "-h") == 0) {
		argv++, argc--;
		host = *argv++;
		argc--;
	}
	if (argc != 1) {
		fprintf(stderr, "usage: define [ -h host ] word\n");
		exit(1);
	}
	hp = gethostbyname(host);
	if (hp == NULL) {
		fprintf(stderr, "define: %s: host unknown\n", host);
		exit(1);
	}
	host = hp->h_name;
#ifdef	NVMUNIX
	s = socket(hp->h_addrtype, SOCK_STREAM, 0, 0);
	if (s < 0) {
		perror("define: socket");
		exit(2);
	}
	sin.sin_family = hp->h_addrtype;
	if (bind(s, &sin, sizeof (sin), 0) < 0) {
		perror("define: bind");
		exit(3);
	}
#else	NVMUNIX
	s = socket(SOCK_STREAM, 0, (struct sockaddr_in *) 0, 0);
	if (s < 0) {
		perror("define: socket");
		exit(2);
	}
	sin.sin_family = AF_INET;
#endif	NVMUNIX
	bcopy(hp->h_addr, &sin.sin_addr, hp->h_length);
	sp = getservbyname("define", "tcp");
	if (sp == NULL) {
		fprintf(stderr, "define: define/tcp: unknown service\n");
		exit(4);
	}
	sin.sin_port = sp->s_port;
#ifdef	NVMUNIX
	if (connect(s, &sin, sizeof (sin), 0) < 0) {
		perror("define: connect");
		exit(5);
	}
#else	NVMUNIX
	if (connect(s, &sin) < 0) {
		perror("define: connect");
		exit(5);
	}
#endif	NVMUNIX
	sfi = fdopen(s, "r");
	sfo = fdopen(s, "w");
	if (sfi == NULL || sfo == NULL) {
		perror("fdopen");
		close(s);
		exit(1);
	}
	fprintf(sfo, "%s\n", *argv);
	fflush(sfo);
	while ((c = getc(sfi)) != EOF)
		putchar(c);
	putchar('\n');
}
