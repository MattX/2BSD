#ifndef lint
static char *rcsid = "$Header: /usr/src/net/src/srinetser/ftp/ftp.c,v 1.3 84/07/06 10:14:04 root Exp $";
static char sccsid[] = "@(#)ftp.c	4.11 (Berkeley) 7/26/83";
#endif

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <netinet/in.h>
#include <arpa/ftp.h>

#include "ftp_var.h"

struct	sockaddr_in hisctladdr;
struct	sockaddr_in data_addr;
int	data = -1;
int	conned;
struct	sockaddr_in myctladdr;

#define	SWAITMAX	90	/* wait at most 90 seconds */
#define	SWAITINT	5	/* interval between retries */

int	swaitmax = SWAITMAX;
int	swaitint = SWAITINT;

FILE	*cin, *cout;
FILE	*dataconn();

struct hostent *
hookup(host, port)
	char *host;
	int port;
{
	register struct hostent *hp;
	int s;

	bzero((char *)&hisctladdr, sizeof (hisctladdr));
	hp = gethostbyname(host);
	if (hp == NULL) {
		static struct hostent def;
		static struct in_addr defaddr;
		static char namebuf[128];
		int inet_addr();

		defaddr.s_addr = inet_addr(host);
		if (defaddr.s_addr == -1) {
			fprintf(stderr, "%s: Unknown host.\n", host);
			return (0);
		}
		strcpy(namebuf, host);
		def.h_name = namebuf;
		hostname = namebuf;
		def.h_addr = (char *)&defaddr;
		def.h_length = sizeof (struct in_addr);
		def.h_addrtype = AF_INET;
		def.h_aliases = 0;
		hp = &def;
	}
	hostname = hp->h_name;
	hisctladdr.sin_family = hp->h_addrtype;
	s = socket(SOCK_STREAM, 0, (struct sockaddr *)0, SO_KEEPALIVE);
	if (s < 0) {
		perror("ftp: socket");
		return (0);
	}
	bcopy(hp->h_addr, (char *)&hisctladdr.sin_addr, hp->h_length);
	hisctladdr.sin_port = port;
	if (connect(s, (char *)&hisctladdr) < 0) {
		perror("ftp: connect");
		goto bad;
	}
	if (socketaddr(s, (char *)&myctladdr) < 0) {
		perror("ftp: socketaddr");
		goto bad;
	}
	cin = fdopen(s, "r");
	cout = fdopen(s, "w");
	if (cin == NULL || cout == NULL) {
		fprintf(stderr, "ftp: fdopen failed.\n");
		if (cin)
			fclose(cin);
		if (cout)
			fclose(cout);
		goto bad;
	}
	if (verbose)
		printf("Connected to %s.\n", hp->h_name);
	(void) getreply(0); 		/* read startup message from server */
	return (hp);
bad:
	close(s);
	return ((struct hostent *)0);
}

login(hp)
	struct hostent *hp;
{
	char acct[80];
	char *user, *pass;
	int n;

	user = pass = 0;
	ruserpass(hp->h_name, &user, &pass);
	n = command("USER %s", user);
	if (n == CONTINUE)
		n = command("PASS %s", pass);
	if (n == CONTINUE) {
		printf("Account: "); (void) fflush(stdout);
		(void) fgets(acct, sizeof(acct) - 1, stdin);
		acct[strlen(acct) - 1] = '\0';
		n = command("ACCT %s", acct);
	}
	if (n != COMPLETE) {
		fprintf(stderr, "Login failed.\n");
		return (0);
	}
	return (1);
}

/*VARARGS 1*/
command(fmt, args)
	char *fmt;
{

	if (debug) {
		printf("---> ");
		_doprnt(fmt, &args, stdout);
		printf("\n");
		(void) fflush(stdout);
	}
	if (cout == NULL) {
		perror ("No control connection for command");
		return (0);
	}
	_doprnt(fmt, &args, cout);
	fprintf(cout, "\r\n");
	(void) fflush(cout);
	return (getreply(!strcmp(fmt, "QUIT")));
}

#include <ctype.h>

getreply(expecteof)
	int expecteof;
{
	register int c, n;
	register int code, dig;
	int originalcode = 0, continuation = 0;

	for (;;) {
		dig = n = code = 0;
		while ((c = getc(cin)) != '\n') {
			dig++;
			if (c == EOF) {
				if (expecteof)
					return (0);
				lostpeer();
				exit(1);
			}
			if (verbose && c != '\r' ||
			    (n == '5' && dig > 4))
				putchar(c);
			if (dig < 4 && isdigit(c))
				code = code * 10 + (c - '0');
			if (dig == 4 && c == '-')
				continuation++;
			if (n == 0)
				n = c;
		}
		if (verbose || n == '5') {
			putchar(c);
			(void) fflush (stdout);
		}
		if (continuation && code != originalcode) {
			if (originalcode == 0)
				originalcode = code;
			continue;
		}
		if (expecteof || empty(cin))
			return (n - '0');
	}
}

empty(f)
	FILE *f;
{
	long mask;

	if (f->_cnt > 0)
		return (0);
	mask = (1 << fileno(f));
	(void) select(20, &mask, 0L, 0L);
	return (mask == 0L);
}

jmp_buf	sendabort;

abortsend()
{

	longjmp(sendabort, 1);
}

sendrequest(cmd, local, remote)
	char *cmd, *local, *remote;
{
	FILE *fin, *dout, *popen();
	int (*closefunc)(), pclose(), fclose(), (*oldintr)();
	char buf[BUFSIZ];
	long bytes = 0L, hashbytes = sizeof (buf);
	register int c, d;
	struct stat st;
	struct timeval start, stop;

	closefunc = NULL;
	if (setjmp(sendabort))
		goto bad;
	oldintr = sigset(SIGINT, abortsend);
	if (strcmp(local, "-") == 0)
		fin = stdin;
	else if (*local == '|') {
		fin = popen(local + 1, "r");
		if (fin == NULL) {
			perror(local + 1);
			goto bad;
		}
		closefunc = pclose;
	} else {
		fin = fopen(local, "r");
		if (fin == NULL) {
			perror(local);
			goto bad;
		}
		closefunc = fclose;
		if (fstat(fileno(fin), &st) < 0 ||
		    (st.st_mode&S_IFMT) != S_IFREG) {
			fprintf(stderr, "%s: not a plain file.", local);
			goto bad;
		}
	}
	if (initconn())
		goto bad;
	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM)
			goto bad;
	} else
		if (command("%s", cmd) != PRELIM)
			goto bad;
	dout = dataconn("w");
	if (dout == NULL)
		goto bad;
	gettimeofday(&start, (struct timezone *)0);
	switch (type) {

	case TYPE_I:
	case TYPE_L:
		errno = d = 0;
		while ((c = fread(buf, sizeof (*buf), sizeof buf, fin)) > 0) {
			if ((d = fwrite(buf, sizeof (*buf), c, dout)) < 0)
				break;
			bytes += c;
			if (hash) {
				putchar('#');
				fflush(stdout);
			}
		}
		if (hash && bytes > 0L) {
			putchar('\n');
			fflush(stdout);
		}
		if (c < 0)
			perror(local);
		if (d < 0)
			perror("netout");
		break;

	case TYPE_A:
		while ((c = getc(fin)) != EOF) {
			if (c == '\n') {
				while (hash && (bytes >= hashbytes)) {
					putchar('#');
					fflush(stdout);
					hashbytes += sizeof (buf);
				}
				if (ferror(dout))
					break;
				putc('\r', dout);
				bytes++;
			}
			putc(c, dout);
			bytes++;
			if (c == '\r') {
				putc('\0', dout);
				bytes++;
			}
		}
		if (hash) {
			if (bytes < hashbytes)
				putchar('#');
			putchar('\n');
			fflush(stdout);
		}
		if (ferror(fin))
			perror(local);
		if (ferror(dout))
			perror("netout");
		break;
	}
	gettimeofday(&stop, (struct timezone *)0);
	if (closefunc != NULL)
		(*closefunc)(fin);
	(void) fclose(dout);
	(void) getreply(0);
done:
	sigset(SIGINT, oldintr);
	if (bytes > 0L && verbose)
		ptransfer("sent", bytes, &start, &stop);
	return;
bad:
	if (data >= 0) {
		gettimeofday(&stop, (struct timezone *)0);
		(void) close(data), data = -1;
	}
	if (closefunc != NULL && fin != NULL)
		(*closefunc)(fin);
	if (!fromatty)
		exit(1);
	goto done;
}

jmp_buf	recvabort;

abortrecv()
{

	longjmp(recvabort, 1);
}

recvrequest(cmd, local, remote, mode)
	char *cmd, *local, *remote, *mode;
{
	FILE *fout, *din, *popen();
	int (*closefunc)(), pclose(), fclose(), (*oldintr)();
	char buf[BUFSIZ];
	long bytes = 0L, hashbytes = sizeof (buf);
	register int c, d;
	struct timeval start, stop;

	closefunc = NULL;
	if (setjmp(recvabort))
		goto bad;
	oldintr = sigset(SIGINT, abortrecv);
	if (strcmp(local, "-") && *local != '|')
		if (access(local, 2) < 0) {
			char *dir = rindex(local, '/');

			if (dir != NULL)
				*dir = 0;
			if (access(dir ? dir : ".", 2) < 0) {
				perror(local);
				goto bad;
			}
			if (dir != NULL)
				*dir = '/';
		}
	if (initconn())
		goto bad;
	if (remote) {
		if (command("%s %s", cmd, remote) != PRELIM)
			goto bad;
	} else
		if (command("%s", cmd) != PRELIM)
			goto bad;
	if (strcmp(local, "-") == 0)
		fout = stdout;
	else if (*local == '|') {
		fout = popen(local + 1, "w");
		closefunc = pclose;
	} else {
		fout = fopen(local, mode);
		closefunc = fclose;
	}
	if (fout == NULL) {
		perror(local + 1);
		goto bad;
	}
	din = dataconn("r");
	if (din == NULL)
		goto bad;
	gettimeofday(&start, (struct timezone *)0);
	switch (type) {

	case TYPE_I:
	case TYPE_L:
		errno = d = 0;
		while ((c = fread(buf, sizeof (*buf), sizeof buf, din)) > 0) {
			if ((d = fwrite(buf, sizeof (*buf), c, fout)) < 0)
				break;
			bytes += c;
			if (hash) {
				putchar('#');
				fflush(stdout);
			}
		}
		if (hash && bytes > 0L) {
			putchar('\n');
			fflush(stdout);
		}
		if (c < 0)
			perror("netin");
		if (d < 0)
			perror(local);
		break;

	case TYPE_A:
		while ((c = getc(din)) != EOF) {
			if (c == '\r') {
				while (hash && (bytes >= hashbytes)) {
					putchar('#');
					fflush(stdout);
					hashbytes += sizeof (buf);
				}
				bytes++;
				if ((c = getc(din)) != '\n') {
					if (ferror (fout))
						break;
					putc ('\r', fout);
				}
				if (c == '\0') {
					bytes++;
					continue;
				}
			}
			putc (c, fout);
			bytes++;
		}
		if (hash) {
			if (bytes < hashbytes)
				putchar('#');
			putchar('\n');
			fflush(stdout);
		}
		if (ferror (din))
			perror ("netin");
		if (ferror (fout))
			perror (local);
		break;
	}
	gettimeofday(&stop, (struct timezone *)0);
	(void) fclose(din);
	if (closefunc != NULL)
		(*closefunc)(fout);
	(void) getreply(0);
done:
	sigset(SIGINT, oldintr);
	if (bytes > 0L && verbose)
		ptransfer("received", bytes, &start, &stop);
	return;
bad:
	if (data >= 0) {
		gettimeofday(&stop, (struct timezone *)0);
		(void) close(data), data = -1;
	}
	if (closefunc != NULL && fout != NULL)
		(*closefunc)(fout);
	if (!fromatty)
		exit(1);
	goto done;
}

/*
 * Need to start a listen on the data channel
 * before we send the command, otherwise the
 * server's connect may fail.
 */
static int sendport = -1;

initconn()
{
	register char *p, *a;
	int result;
	int options = SO_KEEPALIVE | SO_ACCEPTCONN;
	int retry;

noport:
	data_addr = myctladdr;
	if (sendport)
		data_addr.sin_port = 0;	/* let system pick one */ 
	if (data != -1)
		(void) close (data);
	for (retry = 0; retry < swaitmax; sleep (swaitint), retry += swaitint)
	{
		data = socket(SOCK_STREAM, 0, &data_addr, options);
		if (data >= 0 || (errno != EADDRINUSE && errno != ENOBUFS))
			break;
	}
	if (data < 0) {
		perror("ftp: socket");
		return (1);
	}
	if (socketaddr(data, (char *)&data_addr) < 0) {
		perror("ftp: socketaddr");
		goto bad;
	}
	if (sendport) {
		a = (char *)&data_addr.sin_addr;
		p = (char *)&data_addr.sin_port;
#define	UC(b)	(((int)b)&0xff)
		result =
		    command("PORT %d,%d,%d,%d,%d,%d",
		      UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]),
		      UC(p[0]), UC(p[1]));
		if (result == ERROR && sendport == -1) {
			sendport = 0;
			goto noport;
		}
		return (result != COMPLETE);
	}
	return (0);
bad:
	(void) close(data), data = -1;
	return (1);
}

FILE *
dataconn(mode)
	char *mode;
{
	struct sockaddr_in from;

	if (accept(data, &from) < 0) {
		perror("ftp: accept");
		(void) close(data), data = -1;
		return (NULL);
	}
	return (fdopen(data, mode));
}

ptransfer(direction, bytes, t0, t1)
	char *direction;
	long bytes;
	struct timeval *t0, *t1;
{
	struct timeval td;
	long ms;
	float bs;

	tvsub(&td, t1, t0);
	ms = (td.tv_sec * 1000L) + (td.tv_usec / 1000L);
#define	nz(x)	((x) == 0L ? 1L : (x))
	bs = ((bytes * NBBY * 1000L) / (float) nz(ms)) / NBBY;
	printf("%ld bytes %s in %ld.%02ld seconds (%.2g Kbytes/s)\n",
		bytes, direction, td.tv_sec, td.tv_usec / 10000L, bs / 1024.);
}

tvadd(tsum, t0)
	struct timeval *tsum, *t0;
{

	tsum->tv_sec += t0->tv_sec;
	tsum->tv_usec += t0->tv_usec;
	if (tsum->tv_usec > 1000000L)
		tsum->tv_sec++, tsum->tv_usec -= 1000000L;
}

tvsub(tdiff, t1, t0)
	struct timeval *tdiff, *t1, *t0;
{

	tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
	tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
	if (tdiff->tv_usec < 0L)
		tdiff->tv_sec--, tdiff->tv_usec += 1000000L;
}
