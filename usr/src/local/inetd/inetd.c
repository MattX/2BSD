/* inetd.c - implement the "little" Internet protocols */

/* Author:	Marshall T. Rose	<MRose@UCI>	(MTR)
		Department of Information and Computer Science
		University of California, Irvine
		Irvine, CA  92716
		714/856-7403

   Date:	Fri Jan 27 13:41:31 1984

   Not like most 4.2BSD UNIX network servers.  Instead of opening one
   socket we open a lot of them to service different protocols/ports.
   The alternative was to write 16 servers(!!).
 */

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <strings.h>
#include <syslog.h>
#include <utmp.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netdb.h>
#include <net/in.h>
#include <net/inet.h>
#include "tws.h"		/* ZOTnet tws subroutines */



#ifndef	RFINGER
#define	RFINGER	"/usr/local/rfinger"
#endif	RFINGER
#ifndef	RFOPTS
#define	RFOPTS	"-m"
#endif	RFOPTS

#ifndef	FORTUNE
#define	FORTUNE	"/usr/games/fortune"
#endif	FORTUNE


#define	NVEC	100

#define	NBYTES	512		/* maximum UDP datagram size */

#define	NOTOK	(-1)
#define	OK	0
#define	DONE	1

#define	TRUE	1
#define	FALSE	0

#define	min(a, b)	((a) < (b) ? (a) : (b))

/*  */

extern int  errno;
extern int  sys_nerr;
extern char *sys_errlist[];
extern char *sys_siglist[];


static int  debug = FALSE;
static int  nbits = ((sizeof (int)) * 8);
static int  options = 0;

static int  ifd_mask;


static char *myname = "inetd";
static char myhost[BUFSIZ];

static char *fortune = FORTUNE;
static char *rfinger = RFINGER;
static char *rfopts = RFOPTS;
static char *utmp = "/etc/utmp";

/*  */

int     Echo (), Sink (), DayTime (), Time (), Users (), Chargen (),
        Qotd (), Finger ();

struct dsp {
    char   *ds_argument;
    char   *ds_service;
    char   *ds_proto;

    unsigned    ds_flags;
#define	DS_NIL	0x00		/* initially */
#define	DS_ENA	0x01		/* host knows about this service */
#define	DS_SEL	0x02		/* user selected this service */

    short   ds_port;
    short   ds_protocol;

    int     ds_sd;

    int     (*ds_vector) ();
};

static struct dsp  dsps[] = {
    "tcp/echo", "echo", "tcp", DS_NIL, 0, 0, NOTOK, Echo,
    "udp/echo", "echo", "udp", DS_NIL, 0, 0, NOTOK, Echo,
    "tcp/sink", "sink", "tcp", DS_NIL, 0, 0, NOTOK, Sink,
    "udp/sink", "sink", "udp", DS_NIL, 0, 0, NOTOK, Sink,
    "tcp/daytime", "daytime", "tcp", DS_NIL, 0, 0, NOTOK, DayTime,
    "udp/daytime", "daytime", "udp", DS_NIL, 0, 0, NOTOK, DayTime,
    "tcp/time", "time", "tcp", DS_NIL, 0, 0, NOTOK, Time,
    "udp/time", "time", "udp", DS_NIL, 0, 0, NOTOK, Time,
    "tcp/users", "users", "tcp", DS_NIL, 0, 0, NOTOK, Users,
    "udp/users", "users", "udp", DS_NIL, 0, 0, NOTOK, Users,
    "tcp/chargen", "chargen", "tcp", DS_NIL, 0, 0, NOTOK, Chargen,
    "udp/chargen", "chargen", "udp", DS_NIL, 0, 0, NOTOK, Chargen,
    "tcp/qotd", "qotd", "tcp", DS_NIL, 0, 0, NOTOK, Qotd,
    "udp/qotd", "qotd", "udp", DS_NIL, 0, 0, NOTOK, Qotd,
    "tcp/finger", "finger", "tcp", DS_NIL, 0, 0, NOTOK, Finger,
    "udp/finger", "finger", "udp", DS_NIL, 0, 0, NOTOK, Finger,
    NULL
};


void    chldser ();
void	adios (), advise (), ds_adios (), ds_advise ();

char   *stradd (), *strdup ();


char   *ctime (), *malloc ();

/*  */

/* ARGSUSED */

main (argc, argv, envp)
int     argc;
char  **argv,
      **envp;
{
    int     fd,
            ifds;
    struct dsp *ds;

    arginit (argv);
    envinit ();

#ifdef	RESTART
    for (;;) {
	char    reason[BUFSIZ];
	union wait status;

	switch (fork ()) {
	    case NOTOK: 
		sleep (5);
		continue;

	    case OK: 
		break;

	    default: 
		sleep (60);
		(void) wait3 (&status, 0, NULL);
		if (WIFEXITED (status))
		    (void) sprintf (reason, "exit=0%o", status.w_retcode);
		else
		    if (WIFSIGNALED (status))
			(void) sprintf (reason, "signal=%s%s",
				status.w_termsig < NSIG
				? sys_siglist[status.w_termsig] : "unknown",
				status.w_coredump ? " (core dumped)" : NULL);
		    else
			(void) strcpy (reason, "stopped(!!)");
		advise (NULL, LOG_WARNING, "server has terminated -- %s",
			reason);
		continue;
	}
	break;
    }

    closelog ();
    openlog (myname, LOG_PID);
    advise (NULL, LOG_INFO, "restart");
#endif	RESTART

/*  */

    ifd_mask = 0;
    for (ds = dsps; ds -> ds_service; ds++)
	if (ds -> ds_flags & DS_SEL)
	    start (ds);

    (void) signal (SIGCHLD, chldser);

    for (;;) {
	ifds = ifd_mask;
	switch (select (nbits, &ifds, NULL, NULL, NULL)) {
	    case NOTOK: 
		if (errno != EINTR)
		    advise ("select", LOG_WARNING, "unable to");
		break;

	    case OK:		/* eh? */
		break;
		
	    default: 
		for (fd = 0; fd < nbits; fd++)
		    if (ifds & (1 << fd))
			service (fd);
		break;
	}
    }
}

/*  */

static  service (sd)
int     sd;
{
    int     cc,
            fd,
            i;
    char    msg[BUFSIZ];
    struct dsp *ds,
               *ds2;
    struct sockaddr_in  sd_address,
                       *sin = &sd_address;

    for (ds = dsps; ds -> ds_service; ds++)
	if (ds -> ds_sd == sd)
	    break;
    if (ds -> ds_service == NULL)
	adios (NULL, "select botch");

    i = sizeof *sin;
    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    if ((fd = accept (ds -> ds_sd, sin, &i)) == NOTOK) {
		if (errno != EINTR && errno != EWOULDBLOCK)
		    ds_advise ("socket", ds, NULL, 
			LOG_WARNING, "unable to accept connection on");
		return;
	    }
	    break;

	case IPPROTO_UDP: 
	    if ((cc = recvfrom (ds -> ds_sd, msg, sizeof msg, 0, sin, &i))
		    == NOTOK) {
		if (errno != EINTR && errno != EWOULDBLOCK)
		    ds_advise ("socket", ds, NULL,
			LOG_WARNING, "unable to receive from");
		return;
	    }
	    break;
    }

    ds_advise (NULL, ds, sin, LOG_INFO, "servicing");
	
/*  */

    switch (fork ()) {
	case OK: 
	    closelog ();
	    openlog (ds -> ds_service, LOG_PID);
	    for (ds2 = dsps; ds2 -> ds_service; ds2++)
		switch (ds2 -> ds_protocol) {
		    case IPPROTO_TCP: 
			(void) close (ds2 -> ds_sd);
			if (ds == ds2)
			    ds2 -> ds_sd = fd;
			break;

		    case IPPROTO_UDP: 
			if (ds != ds2)
			    (void) close (ds2 -> ds_sd);
			break;
		}
	    (void) signal (SIGCHLD, SIG_DFL);

	    (*ds -> ds_vector) (ds, sin, msg, cc);

	    switch (ds -> ds_protocol) {
		case IPPROTO_TCP: 
		    (void) shutdown (ds -> ds_sd, 1);
		    break;

		default: 
		    break;
	    }

	    _exit (0);

	case NOTOK: 
	    ds_advise ("socket", ds, sin,
		LOG_WARNING, "no forks, so rejecting connection on");
	default: 
	    switch (ds -> ds_protocol) {
		case IPPROTO_TCP: 
		    (void) close (fd);
		    break;

		default: 
		    break;
	    }
	    break;
    }
}

/*  */

static  start (ds)
struct dsp *ds;
{
    int     sd;
    struct sockaddr_in  sd_address,
                       *sin = &sd_address;

    if (ds -> ds_sd != NOTOK) {
	(void) close (ds -> ds_sd);
	ifd_mask &= ~(1 << ds -> ds_sd);
    }

    sin -> sin_family = AF_INET;
    sin -> sin_port = ds -> ds_port;
    sin -> sin_addr.s_addr = INADDR_ANY;

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    sd = socket (AF_INET, SOCK_STREAM, 0);
	    break;

	case IPPROTO_UDP: 
	    sd = socket (AF_INET, SOCK_DGRAM, 0);
	    break;

	default: 
	    ds_adios (NULL, ds, "no support");
    }
    if (sd == NOTOK)
	ds_adios ("socket", ds, "unable to create");

/*  */

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    if (options & SO_DEBUG)
		if (setsockopt (sd, SOL_SOCKET, SO_DEBUG, NULL, 0)
			== NOTOK)
		    ds_advise ("SO_DEBUG", ds, NULL,
			LOG_WARNING, "unable to set socket option");
	    if (setsockopt (sd, SOL_SOCKET, SO_KEEPALIVE, NULL, 0)
		    == NOTOK)
		ds_advise ("SO_KEEPALIVE", ds, NULL,
		    LOG_WARNING, "unable to set socket option");
	    break;

	case IPPROTO_UDP: 
	    if (options & SO_DEBUG)
		if (setsockopt (sd, SOL_SOCKET, SO_DEBUG, NULL, 0)
			== NOTOK)
		    ds_advise ("SO_DEBUG", ds, NULL,
			LOG_WARNING, "unable to set socket option");
	    break;
    }

    if (bind (sd, sin, sizeof *sin) == NOTOK)
	ds_adios ("socket", ds, "unable to bind");

    ds -> ds_sd = sd;
    ifd_mask |= 1 << sd;
    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    (void) listen (ds -> ds_sd, SOMAXCONN);
	    break;

	default:
	    break;
    }
}

/*  */

static int  Echo (ds, sin, msg, cc)
struct dsp *ds;
struct sockaddr_in *sin;
char   *msg;
int     cc;
{
    int     i;
    char    buffer[BUFSIZ];

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    while ((i = read (ds -> ds_sd, buffer, sizeof buffer)) > 0)
		if (write (ds -> ds_sd, buffer, i) != i)
		    break;
	    break;

	case IPPROTO_UDP: 
	    (void) sendto (ds -> ds_sd, msg, cc, 0, sin, sizeof *sin);
	    break;
    }
}

/*  */

/* ARGSUSED */

static int  Sink (ds, sin, msg, cc)
struct dsp *ds;
struct sockaddr_in *sin;
char   *msg;
int     cc;
{
    char    buffer[BUFSIZ];

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    while (read (ds -> ds_sd, buffer, sizeof buffer) > 0)
		continue;
	    break;

	case IPPROTO_UDP: 
	    break;
    }
}

/*  */

/* There is no specific syntax for the daytime.  It is recommended that it
   be limited to the ASCII printing characters, space, carriage return, and
   line feed.  The daytime should be just one line.

   We will use the ZOTnet tws subroutine package to give us a standard
   822-style date.

 */

/* ARGSUSED */

static int  DayTime (ds, sin, msg, cc)
struct dsp *ds;
struct sockaddr_in *sin;
char   *msg;
int     cc;
{
    char    buffer[BUFSIZ];

    (void) sprintf (buffer, "%s\r\n", dtimenow ());

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    (void) write (ds -> ds_sd, buffer, strlen (buffer));
	    break;

	case IPPROTO_UDP: 
	    (void) sendto (ds -> ds_sd, buffer, strlen (buffer), 0,
		    sin, sizeof *sin);
	    break;
    }
}

/*  */

/* The time is the number of seconds since 00:00 (midnight) 1 January 1900
   GMT, such that the time 1 is 12:00:01 am on 1 January 1900 GMT;  this
   base will server until the year 2036.

   We will use the gettimeofday() kernel call which tells us (among other
   things) the number of seconds since 00:00 1 Jan 1970 GMT.  If we add 
   2,208,988,800 to this number, then we will get the number of seconds
   since 00:00 1 Jan 1900 GMT.

 */

/* ARGSUSED */

static int  Time (ds, sin, msg, cc)
struct dsp *ds;
struct sockaddr_in *sin;
char   *msg;
int     cc;
{
    int     result;
    struct timeval  tv;
    struct timezone tz;

    if (gettimeofday (&tv, &tz) == NOTOK) {
	ds_advise ("time of day", ds, sin, LOG_WARNING, "unable to get");
	return;
    }
    result = htonl (((int) tv.tv_sec) + 2208988800);

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    (void) write (ds -> ds_sd, (char *) (&result), sizeof result);
	    break;

	case IPPROTO_UDP: 
	    (void) sendto (ds -> ds_sd, (char *) (&result), sizeof result, 0,
		    sin, sizeof *sin);
	    break;
    }
}

/*  */

/* There is no specific syntax for the user list.  It is recommended that
   it be limited to the ASCII printing characters, space, carriage return,
   and line feed.  Each user should be listed on a separate line.

   We will search utmp, and print a listing similar to the output of who.

 */

/* ARGSUSED */

static int  Users (ds, sin, msg, cc)
struct dsp *ds;
struct sockaddr_in *sin;
char   *msg;
int     cc;
{
#define	HMAX	(sizeof (ut -> ut_host))
#define	LMAX	(sizeof (ut -> ut_line))
#define	NMAX	(sizeof (ut -> ut_name))
    int     ud;
    char   *bp,
           *dp,
            buffer[BUFSIZ];
    struct utmp user,
               *ut = &user;

    if ((ud = open (utmp, O_RDONLY)) == NOTOK) {
	ds_advise (utmp, ds, sin, LOG_WARNING, "unable to read");
	return;
    }
    bp = NULL;
    while (read (ud, (char *) ut, sizeof *ut) == sizeof *ut) {
	if (ut -> ut_name[0] == NULL)
	    continue;
	dp = ctime (&ut -> ut_time);
	(void) sprintf (buffer, "%-*.*s %-*.*s %.12s",
		NMAX, NMAX, ut -> ut_name, LMAX, LMAX, ut -> ut_line,
		dp + 4);
	if (ut -> ut_host[0])
	    (void) sprintf (buffer + strlen (buffer), "\t(%.*s)",
		    HMAX, ut -> ut_host);
	(void) sprintf (buffer + strlen (buffer), "\r\n");
	bp = stradd (bp, buffer);
    }
    (void) close (ud);

    if (bp == NULL)
	bp = "";

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    (void) write (ds -> ds_sd, bp, strlen (bp));
	    break;

	case IPPROTO_UDP: 
	    udps (ds, sin, bp);
	    break;
    }

    if (bp != NULL && *bp != NULL)
	free (bp);
}

/*  */

/* The data may be anything.  It is recommended that a recognizable pattern
   be used in the data.

 */

/* ARGSUSED */

static int  Chargen (ds, sin, msg, cc)
struct dsp *ds;
struct sockaddr_in *sin;
char   *msg;
int     cc;
{
#define	LINSIZ	72

    int     i;
    char   *bp,
           *be,
           *dp,
           *de,
           *rs,
           *rp,
           *re,
            buffer[NBYTES],
            line[LINSIZ + 2],
            ring[BUFSIZ];

    bp = buffer;
    be = buffer + sizeof buffer;
    de = line + (sizeof line) - 2;
    rp = re = ring;
    for (i = 0; i <= 0177; i++)
	if (isprint (i))
	    *re++ = i;

    for (rs = ring;; rs++) {
	if (rs >= re)
	    rs = ring;
	rp = rs;
	for (dp = line, rp = rs; dp < de; dp++, rp++) {
	    if (rp >= re)
		rp = ring;
	    *dp = *rp;
	}
	*dp++ = '\r';
	*dp++ = '\n';

	switch (ds -> ds_protocol) {
	    case IPPROTO_TCP: 
		if (write (ds -> ds_sd, line, sizeof line) != sizeof line)
		    break;
		continue;

	    case IPPROTO_UDP: 
		if (bp + sizeof line >= be) {
		    (void) sendto (ds -> ds_sd, buffer, bp - buffer, 0,
			    sin, sizeof *sin);
		    break;
		}
		(void) strncpy (bp, line, sizeof line);
		bp += sizeof line;
		continue;
	}
	break;
    }
}

/*  */

/* There is no specific syntax for the quote.  It is recommended that it
   be limited to the ASCII printing characters, space, carriage return,
   and line feed.  The quote may be just one or up to serveral lines, but
   it should be less than 512 characters.

   We will try to use the humorous fortune program.

 */

/* ARGSUSED */

static int  Qotd (ds, sin, msg, cc)
struct dsp *ds;
struct sockaddr_in *sin;
char   *msg;
int     cc;
{
    int     i,
            pd[2];
    char    c,
           *bp,
           *dp,
           *de,
            buffer[BUFSIZ],
            data[NBYTES];
    union wait status;

    if (access (fortune, X_OK) == NOTOK || pipe (pd) == NOTOK) {
error: 	;
	(void) sprintf (data, "%s: %s\r\n", fortune,
		errno > 0 && errno < sys_nerr ? sys_errlist[errno]
		: "unavailable");
	ds_advise (fortune, ds, sin, LOG_WARNING, "unable to service");
    }
    else
	switch (fork ()) {
	    case NOTOK: 
		(void) close (pd[0]);
		(void) close (pd[1]);
		goto error;

	    case OK: 
		closelog ();
		(void) close (0);
		if ((i = open ("/dev/null", O_RDWR)) != NOTOK) {
		    if (i != 0)
			(void) dup2 (i, 0), (void) close (i);
		    (void) dup2 (0, 2);
		}
		else
		    (void) close (2);
		(void) dup2 (pd[1], 1);
		(void) close (pd[0]);
		(void) close (pd[1]);
		execlp (fortune, fortune, NULL);
		_exit (1);

	    default: 
		(void) close (pd[1]);
		c = NULL;
		dp = data;
		de = data + sizeof data;
		while ((i = read (pd[0], buffer, sizeof buffer)) > 0)
		    for (bp = buffer; i > 0; bp++, i--)
			if (dp >= de)
			    break;
			else
			    if (*bp != NULL) {
				if (c != '\r' && *bp == '\n')
				    *dp++ = '\r';
				*dp++ = c = *bp;
			    }
		(void) close (pd[0]);
		(void) wait3 (&status, 0, NULL);
		if (i < 0 || dp == data) {
		    if (dp == data)
			errno = 0;
		    goto error;
		}
		break;
	}

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    (void) write (ds -> ds_sd, data, strlen (data));
	    break;

	case IPPROTO_UDP: 
	    (void) sendto (ds -> ds_sd, data, strlen (data), 0,
		    sin, sizeof *sin);
	    break;
    }
}

/*  */

/* These programs return a friendly, human-oriented status report on
   either the system at the moment or a particular person in depth.
   ... there is no required format and the protocol consists mostly of
   specifying a single command line.

   We will run the rfinger program.  This function is based on the fing
   server written by CAK.

 */

/* ARGSUSED */

static int  Finger (ds, sin, msg, cc)
struct dsp *ds;
struct sockaddr_in *sin;
char   *msg;
int     cc;
{
    int     i,
            n,
            pd[2];
    char    c,
           *bp,
           *dp,
           *dm,
            buffer[BUFSIZ],
            data[BUFSIZ * 2 + 2],
           *vec[NVEC + 1];
    union wait status;

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    for (n = 0;
		    (i = read (ds -> ds_sd, buffer + n, sizeof buffer - n))
		    > 0;
		    n += i)
		if (index (buffer, '\n'))
		    break;
	    if (i == NOTOK)
		return;
	    break;

	case IPPROTO_UDP: 
	    (void) strncpy (buffer, msg, n = cc);
	    break;
    }

    for (bp = buffer; bp < buffer + n; bp++)
	if (isupper (*bp = toascii (*bp)))
	    *bp = tolower (*bp);
    if (bp = index (buffer, '\n'))
	*bp = NULL;
    if (bp = index (buffer, '\r'))
	*bp = NULL;
    makevec (sprintf (data, "rfinger %s %s", rfopts, buffer), vec);

    if (access (rfinger, X_OK) == NOTOK || pipe (pd) == NOTOK) {
error: 	;
	bp = sprintf (data, "%s: %s\r\n", rfinger,
		errno > 0 && errno < sys_nerr ? sys_errlist[errno]
		: "unavailable");
	ds_advise (rfinger, ds, sin, LOG_WARNING, "unable to service");
    }
    else
	switch (fork ()) {
	    case NOTOK: 
		(void) close (pd[0]);
		(void) close (pd[1]);
		goto error;

	    case OK: 
		closelog ();
		(void) close (0);
		if ((i = open ("/dev/null", O_RDWR)) != NOTOK) {
		    if (i != 0)
			(void) dup2 (i, 0), (void) close (i);
		    (void) dup2 (0, 2);
		}
		else
		    (void) close (2);
		(void) dup2 (pd[1], 1);
		(void) close (pd[0]);
		(void) close (pd[1]);
		execvp (rfinger, vec);
		_exit (1);

	    default: 
		(void) close (pd[1]);
		c = NULL;
		bp = NULL;
		while ((i = read (pd[0], buffer, sizeof buffer)) > 0) {
		    dp = data;
		    for (dm = buffer; i > 0; dm++, i--)
			if (*dm != NULL) {
			    if (c != '\r' && *dm == '\n')
				*dp++ = '\r';
			    *dp++ = c = *dm;
			}
		    *dp = NULL;
		    bp = stradd (bp, data);
		}
		(void) close (pd[0]);
		(void) wait3 (&status, 0, NULL);
		if (i < 0 || bp == NULL) {
		    if (bp == NULL)
			errno = 0;
		    goto error;
		}
		break;
	}

    switch (ds -> ds_protocol) {
	case IPPROTO_TCP: 
	    (void) write (ds -> ds_sd, bp, strlen (bp));
	    break;

	case IPPROTO_UDP: 
	    udps (ds, sin, bp);
	    break;
    }

    if (bp != NULL && bp != data)
	free (bp);
}

/*  */

static  makevec (cmd, vec)
char   *cmd,
      **vec;
{
    int     i;
    char   *s;

    for (i = 0, s = cmd; i <= NVEC;) {
	vec[i] = NULL;
	while (isspace (*s) || *s == ',')
	    *s++ = NULL;
	if (*s == NULL)
	    break;
	vec[i++] = s++;
	while (*s != NULL && !isspace (*s) && *s != ',')
	    s++;
    }
    vec[i] = NULL;
}


static  udps (ds, sin, data)
struct dsp *ds;
struct sockaddr_in *sin;
char   *data;
{
    int     n;
    char   *dp,
           *de,
           *dm;

    for (dp = data, de = data + strlen (data); dp < de; dp += n) {
	n = min (de - dp, NBYTES);
	if (dp + n < de)
	    for (dm = dp + n - 1; dm > dp; dm--)
		if (*dm == '\n') {
		    n = dm - dp + 1;
		    break;
		}
	(void) sendto (ds -> ds_sd, dp, n, 0, sin, sizeof *sin);
    }
}

/*  */

static  arginit (vec)
char  **vec;
{
    char   *ap;
    struct dsp *ds;
    struct hostent *hp;
    struct protoent *pp;
    struct servent *sp;

    if (myname = rindex (*vec, '/'))
	myname++;
    if (myname == NULL || *myname == NULL)
	myname = *vec;

    (void) gethostname (myhost, sizeof myhost);
    if (hp = gethostbyname (myhost))
	(void) strcpy (myhost, hp -> h_name);
    nbits = getdtablesize ();

    for (ds = dsps; ds -> ds_service; ds++)
	if (pp = getprotobyname (ds -> ds_proto)) {
	    ds -> ds_flags |= DS_ENA;
	    ds -> ds_protocol = pp -> p_proto;
	}

    for (ds = dsps; ds -> ds_service; ds++)
	if (sp = getservbyname (ds -> ds_service, ds -> ds_proto)) {
	    ds -> ds_flags |= DS_ENA;
	    ds -> ds_port = sp -> s_port;
	}

/*  */

    if (*++vec == NULL)
	adios (NULL, "usage: %s protocol/service ...", myname);

    for (; ap = *vec; vec++) {
	if (*ap == '-')
	    switch (*++ap) {
		case 'd': 
		    options |= SO_DEBUG;
		    continue;

		case 'f': 
		    if ((rfinger = *++vec) == NULL || *rfinger == '-')
			adios (NULL, "usage: %s -f program", myname);
		    continue;

		case 'o': 
		    if ((rfopts = *++vec) == NULL)
			adios (NULL, "usage: %s -o options", myname);
		    continue;

		case 'q': 
		    if ((fortune = *++vec) == NULL || *fortune == '-')
			adios (NULL, "usage: %s -q program", myname);
		    continue;

		default: 
		    adios (NULL, "-%s: unknown switch", ap);
	    }

	for (ds = dsps; ds -> ds_service; ds++)
	    if (ds -> ds_flags & DS_ENA)
		if (strcmp (ds -> ds_argument, ap) == 0) {
		    ds -> ds_flags |= DS_SEL;
		    break;
		}
	if (ds -> ds_service == NULL)
	    adios (NULL, "%s: unknown service", ap);
    }
}

/*  */

static  envinit () {
    int     i,
            sd;

    if (!(debug = isatty (2))) {
	for (i = 0; i < 5; i++) {
	    switch (fork ()) {
		case NOTOK: 
		    sleep (5);
		    continue;

		case OK: 
		    break;

		default: 
		    _exit (0);
	    }
	    break;
	}

	(void) chdir ("/");

	if ((sd = open ("/dev/null", O_RDWR)) == NOTOK)
	    adios ("/dev/null", "unable to read");
	if (sd != 0)
	    (void) dup2 (sd, 0), (void) close (sd);
	(void) dup2 (0, 1);
	(void) dup2 (0, 2);

	if ((sd = open ("/dev/tty", O_RDWR)) != NOTOK) {
	    (void) ioctl (sd, TIOCNOTTY, NULL);
	    (void) close (sd);
	}
    }

    for (sd = 3; sd < nbits; sd++)
	(void) close (sd);

    (void) signal (SIGPIPE, SIG_IGN);

    openlog (myname, LOG_PID);
    advise (NULL, LOG_INFO, "starting");
}

/*  */

static char *stradd (s1, s2)
char   *s1,
       *s2;
{
    char   *p;

    if (s1 == NULL)
	return strdup (s2);

    if (p = malloc ((unsigned) (strlen (s1) + strlen (s2) + 2)))
	(void) sprintf (p, "%s%s", s1, s2);
    free (s1);
    return p;
}


static char *strdup (s1)
char   *s1;
{
    char   *p;

    if (p = malloc ((unsigned) (strlen (s1) + 1)))
	(void) strcpy (p, s1);
    return p;
}

/*  */

/* ARGSUSED */

static  void chldser (signo, code, scp)
int     signo;
long    code;
struct sigcontext  *scp;
{
    union wait status;

    while (wait3 (&status, WNOHANG, NULL) > 0)
	continue;
}

/*  */

/* VARARGS */

void adios (what, fmt, a, b, c, d)
char   *what,
       *fmt,
       *a,
       *b,
       *c,
       *d;
{
    advise (what, LOG_SALERT, fmt, a, b, c, d);
    _exit (1);
}


/* VARARGS */

void advise (what, code, fmt, a, b, c, d)
char   *what,
       *fmt,
       *a,
       *b,
       *c,
       *d;
int	code;
{
    char    buffer[BUFSIZ];

    if (what) {
	(void) sprintf (buffer, fmt, a, b, c, d);
	syslog (code, "%s %s: %m", buffer, what);
    }
    else
	syslog (code, fmt, a, b, c, d);
}

/*  */

/* VARARGS */

void ds_adios (what, ds, fmt, a, b, c, d)
struct dsp *ds;
char   *what,
       *fmt,
       *a,
       *b,
       *c,
       *d;
{
    ds_advise (what, ds, NULL, LOG_SALERT, fmt, a, b, c, d);
    _exit (1);
}


/* VARARGS */

void ds_advise (what, ds, sin, code, fmt, a, b, c, d)
struct dsp *ds;
char   *what,
       *fmt,
       *a,
       *b,
       *c,
       *d;
struct sockaddr_in *sin;
int	code;
{
    char    buffer[BUFSIZ],
            peer[BUFSIZ];
    struct hostent *hp;
    struct in_addr *addr;

    (void) sprintf (buffer, fmt, a, b, c, d);
    if (sin) {
	addr = &sin -> sin_addr;
	(void) sprintf (peer, "(peer %s/%d) ",
		(hp = gethostbyaddr (addr, sizeof *addr, sin -> sin_family))
		? hp -> h_name : inet_ntoa (*addr),
		ntohs (sin -> sin_port));
    }

    advise (what, code, "%s/%s: %s%s",
	    ds -> ds_proto, ds -> ds_service, sin ? peer : NULL, buffer);
}
