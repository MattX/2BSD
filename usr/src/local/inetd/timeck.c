/* timeck.c - poll the localnet for the current time */

/* Author:	Marshall T. Rose	<MRose@UCI>	(MTR)
		Department of Information and Computer Science
		University of California, Irvine
		Irvine, CA  92716
		714/856-7403

   Date:	Fri Jan 27 13:41:31 1984

 */

#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <utmp.h>
#include "tws.h"		/* ZOTnet tws subroutines */


#define	SCPYN(a,b)	strncpy (a, b, sizeof (a))
#define	WTMP		"/usr/adm/wtmp"


static	int	respondents = 0;

static	int	setime = 0;

static	char   *myname = "timeck";

static  char	myhost[BUFSIZ];
static  char	peer[BUFSIZ];

static	struct in_addr myaddr;

static	struct timeval tv;
static	struct timezone tz;

static struct utmp ut[2] = {
    { "|", "", "", 0L },
    { "{", "", "", 0L }
};


void	alrmser ();

/*  */

/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    int     i,
            j,
            result,
            sd;
    long    value;
    char   *cp;
    struct sockaddr_in  sd_address,
                       *sin = &sd_address;
    struct in_addr *addr = &sin -> sin_addr;
    struct hostent *hp;

    arginit (argv);
    sd = sokinit ();

    for (;;) {
	j = sizeof *sin;
	if ((i = recvfrom (sd, (char *) (&result), sizeof result, 0, sin, &j))
		< 0)
	    adios ("failed", "recvfrom");
	alarm (0);

	value = (long) (ntohl (result) - 2208988800);
	cp = (hp = gethostbyaddr (addr, sizeof *addr, sin -> sin_family))
	    ? hp -> h_name : inet_ntoa (*addr);
	printf ("%s/%d: %s\n", cp, ntohs (sin -> sin_port), dtime (&value));
	if (setime && tv.tv_sec == 0 && addr -> s_addr != myaddr.s_addr) {
	    tv.tv_sec = value;
	    strcpy (peer, cp);
	}
	respondents++;

	alarm ((unsigned) 2);
    }
}

/*  */

static  arginit (vec)
char  **vec;
{
    char   *ap;
    struct hostent *hp;

    if (myname = rindex (*vec, '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = *vec;

    for (vec++; ap = *vec; vec++)
	if (*ap == '-')
	    switch (*++ap) {
		case 's': 
		    setime++;
		    break;

		default: 
		    adios (NULL, "unknown switch -%s", ap);
	    }
	else
	    adios (NULL, "usage: %s", myname);

    gettimeofday (&tv, &tz);
    tv.tv_sec = tv.tv_usec = 0;

    gethostname (myhost, sizeof myhost);
    if ((hp = gethostbyname (myhost)) == NULL)
	adios (NULL, "%s: unknown host", myhost);
    strcpy (myhost, hp -> h_name);
    bzero ((char *) &myaddr, sizeof myaddr);
    bcopy (hp -> h_addr, (char *) &myaddr, hp -> h_length);

    signal (SIGALRM, alrmser);
}

/*  */

static int  sokinit () {
    int     i,
            result,
            sd;
    struct netent  *np;
    struct servent *sp;
    struct sockaddr_in  sd_address,
                       *sin = &sd_address;

    if ((sd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	adios ("socket", "unable to create");

    if ((sp = getservbyname ("time", "udp")) == NULL)
	adios (NULL, "udp/timeserver: unknown server");
    if ((np = getnetbyname ("localnet")) == NULL)
	adios (NULL, "localnet: unknown network");

    sin -> sin_family = np -> ne_addrtype;
    sin -> sin_port = sp -> s_port;
    sin -> sin_addr = inet_makeaddr (np -> ne_net, 0);

    result = 0;
    if (sendto (sd, (char *) (&result), sizeof result, 0, sin, sizeof *sin)
	    < 0)
	adios ("failed", "sendto");
    setuid (getuid ());		/* broadcast requires privs */

    printf ("Opening %s...\n", np -> ne_name);

    alarm ((unsigned) 5);
    return sd;
}

/*  */

/* ARGSUSED */

void alrmser (i)
int     i;
{
    int     fd;

    if (setime && tv.tv_sec > 0) {
	time (&ut[0].ut_time);
	SCPYN (ut[0].ut_host, myhost);
	if (settimeofday (&tv, &tz) < 0)
	    adios ("set time", "unable to");
	if ((fd = open (WTMP, O_WRONLY | O_APPEND)) >= 0) {
	    time (&ut[1].ut_time);
	    SCPYN (ut[1].ut_host, peer);
	    lseek (fd, 0L, L_XTND);
	    write (fd, (char *) ut, sizeof ut);
	    close (fd);
	}
	printf ("time was    %s\n", dtime (&ut[0].ut_time));
	printf ("time set to %s\n", dtime (&tv.tv_sec));
    }

    exit (respondents > 0 ? 0 : 1);
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
    fflush (stdout);

    fprintf (stderr, "%s: ", myname);
    fprintf (stderr, fmt, a, b, c);
    if (what)
	putc (' ', stderr), perror (what);
    else
	putc ('\n', stderr);

    exit (1);
}
