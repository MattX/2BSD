/* whois.c - access the NICNAME server at SRI-NIC */

/* Author:	Marshall T. Rose	<MRose@UCI>	(MTR)
		Department of Information and Computer Science
		University of California, Irvine
		Irvine, CA  92716
		714/856-7403

   Date:	Fri Jan 27 13:41:31 1984

   Based on the nicname program written by Jeff Mogul, which was hacked up
   from fing.c which was written by Christopher Kent.  Original inspiration
   from [SRI-NIC]PS:<NETINFO>WHOIS.UNIX.

 */

#include <ctype.h>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define	NOTOK	(-1)

/*  */

static int  help = 0;
static int  nouser = 0;
static int  verbose = 0;

static short    port = 0;

static char *host = "sri-nic";
static char *myname = "whois";
static char names[BUFSIZ] = "";

static    struct servent *sp = NULL;

/*  */

/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    int     bytes,
            c,
            eol,
            i,
            n,
            sd;
    char    buffer[BUFSIZ];

    arginit (argv);

    sd = sokinit ();

    for (bytes = 0, eol = 1;
	    (n = read (sd, buffer, sizeof buffer)) > 0;
	    bytes += n)
	for (i = 0; i < n; i++) {
	    if (!iscntrl (c = toascii (buffer[i])) || c == '\007')
		putchar (c);
	    eol = c == '\n';
	}
    if (!eol)
	putchar ('\n');
    (void) close (sd);

    if (n == NOTOK)
	adios ("response", "error reading");
    if (bytes == 0)
	printf ("%s loses, sorry...\n", names);
    if (nouser && bytes != 0)
	printf ("[Remember to quote \"*\" and \"!\" for the shell]\n");

    exit (0);
}

/*  */

static  arginit (vec)
char  **vec;
{
    char    c,
           *ap,
           *pp;
    static struct servent   ss;

    if (myname = rindex (*vec, '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = *vec;

    if (*++vec == NULL)
	goto usage;

    for (; ap = *vec; vec++)
	if (*ap == '-')
	    while (c = *++ap)
		switch (c) {
		    case 'h': 
			help++;
			break;

		    case 'p': 
			if ((pp = *++vec) == NULL || *pp == '-')
			    adios (NULL, "usage: %s -p portno", myname);
			if (sp = getservbyname (pp, "tcp"))
			    port = ntohs (sp -> s_port);
			else
			    if ((port = atoi (pp)) <= 0)
				adios (NULL, "usage: %s -p portno", myname);
			    else {
				sp = &ss;
				sp -> s_name = pp;
				sp -> s_aliases = NULL;
				sp -> s_port = htons (port);
				sp -> s_proto = "udp";
			    }
			break;

		    case 's': 
			if ((host = *++vec) == NULL || *host == '-')
			    adios (NULL, "usage: %s -s hostname", myname);
			break;

		    case 'v': 
			verbose++;
			break;

		    default: 
		usage: 	;
			adios (NULL, "usage: %s [-h] [-v] ident ...", myname);

		}
	else
	    if (names[0])
		(void) sprintf (names + strlen (names), " %s", ap);
	    else
		(void) strcpy (names, ap);

    if (strlen (names) == 0 || help) {
	nouser++;
	(void) strcpy (names, " ");
    }
    else
	verbose++;
}

/*  */

int     sokinit () {
    int     sd;
    struct hostent *hp;
    struct sockaddr_in  sd_address,
                       *sin = &sd_address;

    if (sp == NULL && (sp = getservbyname ("whois", "tcp")) == NULL)
	adios (NULL, "tcp/whois: unknown service");
    if ((hp = gethostbyname (host)) == NULL)
	adios (NULL, "%s: unknown host", host);
    bzero ((char *) sin, sizeof *sin);
    sin -> sin_family = hp -> h_addrtype;
    sin -> sin_port = sp -> s_port;
    bcopy (hp -> h_addr, (char *) &sin -> sin_addr, hp -> h_length);

    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == NOTOK)
	adios ("socket", "unable to create");
    (void) setsockopt (sd, SOL_SOCKET, SO_KEEPALIVE, NULL, 0);

    if (verbose) {
	printf ("Connecting to server on %s", hp -> h_name);
	if (port != 0)
	    printf (" on port %d", port);
	if (!nouser)
	    printf (" for \"%s\"", names);
	printf ("... ");
	(void) fflush (stdout);
    }

    switch (connect (sd, sin, sizeof *sin)) {
	case NOTOK: 
	    if (verbose) {
		perror ("\nfailed");
		exit (1);
	    }
	    else
		adios (host, "unable to connect to server on");

	default: 
	    if (verbose) {
		printf ("open.\n");
		(void) fflush (stdout);
	    }
	    (void) write (sd, names, strlen (names));
	    (void) write (sd, "\r\n", 2);
	    (void) shutdown (sd, 1);
	    return sd;
    }

/* NOTREACHED */
}

/*  */

/* VARARGS */

static  adios (what, fmt, a, b, c)
char   *what,
       *fmt,
       *a,
       *b,
       *c;
{
    (void) fflush (stdout);

    fprintf (stderr, "%s: ", myname);
    fprintf (stderr, fmt, a, b, c);
    if (what)
	putc (' ', stderr), perror (what);
    else
	putc ('\n', stderr);

    exit (1);
}
