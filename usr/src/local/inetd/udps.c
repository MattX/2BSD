/* udps.c - test out some udp servers */

/* Author:	Marshall T. Rose	<MRose@UCI>	(MTR)
		Department of Information and Computer Science
		University of California, Irvine
		Irvine, CA  92716
		714/856-7403

   Date:	Fri Jan 27 13:41:31 1984

 */

#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

/*  */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    int     null,
            i,
            j,
            sd;
    char   *host,
           *port,
            buffer[BUFSIZ];
    struct hostent *hp;
    struct servent *sp,
                    ss;
    struct sockaddr_in  sd_address,
                       *sin = &sd_address;
    struct in_addr *addr = &sin -> sin_addr;

    if ((sd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	perror ("socket"), exit (1);

    null = 0;
    host = port = NULL;
again: ;
    argc--, argv++;
    if (argc > 0) {
	if (strcmp (*argv, "-n") == 0) {
	    null++;
	    goto again;
	}
	if (host)
	    port = *argv;
	else
	    host = *argv;
	goto again;
    }
    if (host == NULL)
	gethostname (host = buffer, sizeof buffer);
    if (port == NULL)
	port = "qotd";

/*  */

    if ((sp = getservbyname (port, "udp")) == NULL) {
	if (!isdigit (*port) || (i = atoi (port)) <= 0) {
	    fprintf (stderr, "udp/%s: unknown service\n", port);
	    exit (1);
	}
	sp = &ss;
	sp -> s_name = port;
	sp -> s_aliases = NULL;
	sp -> s_port = i;
	sp -> s_proto = "udp";
    }
    if ((hp = gethostbyname (host)) == NULL) {
	fprintf (stderr, "%s: unknown host\n", host);
	exit (1);
    }
    bzero ((char *) sin, sizeof *sin);
    sin -> sin_family = hp -> h_addrtype;
    sin -> sin_port = sp -> s_port;
    bcopy (hp -> h_addr, (char *) addr, hp -> h_length);

    i = 0;
    if (!null) {
	printf ("reading from tty\n");
	while (fgets (buffer, sizeof buffer, stdin))
	    if (sendto (sd, buffer, strlen (buffer), 0, sin, sizeof *sin) < 0)
		perror ("sendto"), exit (1);
	    else
		i++;
    }
    if (i == 0) {
	buffer[0] = 0;
	if (i == 0 && sendto (sd, buffer, 1, 0, sin, sizeof *sin) < 0)
	    perror ("sendto"), exit (1);
    }

    printf ("reading from udp\n");
    for (;;) {
	j = sizeof *sin;
	if ((i = recvfrom (sd, buffer, sizeof buffer, 0, sin, &j)) < 0)
	    perror ("recvfrom"), exit (1);
	printf (" -- Datagram from %s/%d (%d bytes) --\n",
		(hp = gethostbyaddr (addr, sizeof *addr, sin -> sin_family))
		? hp -> h_name : inet_ntoa (*addr),
		ntohs (sin -> sin_port), i);
	fwrite (buffer, sizeof (char), i, stdout);
    }
}
