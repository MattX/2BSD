#ifndef lint
static	char *sccsid = "@(#)comsat.c	4.5 82/12/23";
#endif

#include <stdio.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sgtty.h>
#include <utmp.h>
#include <stat.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>

/*
 * comsat
 */
#define	dprintf	if (0) printf

#define MAXUTMP 100		/* down from init */

struct	sockaddr_in sin = { AF_INET };
struct	sockaddr_in ifrom;
extern	errno;

struct	utmp utmp[100];
int	nutmp;
int	uf;
unsigned utmpmtime;			/* last modification time for utmp */
int	onalrm();
struct	servent *sp;
char	myname[32];

#define NAMLEN (sizeof (uts[0].ut_name) + 1)

main(argc, argv)
char **argv;
{
	register cc;
	char buf[BUFSIZ];
	int s;

	sp = getservbyname("biff", "udp");
	if (sp == 0) {
		fprintf(stderr, "comsat: biff/udp: unknown service\n");
		exit(1);
	}
	sin.sin_port = htons(sp->s_port);
	gethostname(myname, sizeof myname);
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
	chdir("/usr/spool/mail");
	if((uf = open("/etc/utmp",0)) < 0)
		perror("/etc/utmp"), exit(1);
#ifndef DEBUG
	while (fork())
		wait(0);
#endif
	sleep(10);
	onalrm();
	sigset(SIGALRM, onalrm);
	sigignore(SIGTTOU);
	s = socket(SOCK_DGRAM, 0, &sin, SO_ACCEPTCONN);
	if (s < 0) {
		perror("socket");
		exit(1);
	}
	for (;;) {
		char msgbuf[BUFSIZ];
		int cc;

		cc = receive(s, &ifrom, msgbuf, sizeof (msgbuf) - 1);
		if (cc <= 0) {
			if (errno != EINTR)
				sleep(1);
			errno = 0;
			continue;
		}
		msgbuf[cc] = 0;
		mailfor(msgbuf);
	}
}

onalrm()
{
	struct stat statbf;
	struct utmp *utp;

	dprintf("alarm\n");
	alarm(15);
	fstat(uf,&statbf);
	if (statbf.st_mtime > utmpmtime) {
		dprintf(" changed\n");
		utmpmtime = statbf.st_mtime;
		lseek(uf, 0, 0);
		nutmp = read(uf,utmp,sizeof(utmp))/sizeof(struct utmp);
	} else
		dprintf(" ok\n");
}

mailfor(name)
	char *name;
{
	register struct utmp *utp = &utmp[nutmp];
	register char *cp;
	char *rindex();
	char *from, *subject;

	dprintf("mailfor %s\n", name);
	cp = name;
	while (*cp && *cp != '\01')
		cp++;
	if (*cp == 0) {
		dprintf("bad format\n");
		return;
	}
	*cp = 0;
	from = ++cp;
	while (*cp && *cp != '\01')
		cp++;
	if (*cp == 0) {
		dprintf("bad format\n");
		return;
	}
	*cp = 0;
	subject = ++cp;

	while (--utp >= utmp)
		if (!strncmp(utp->ut_name, name, sizeof(utmp[0].ut_name)))
			if (fork() == 0) {
				signal(SIGALRM, SIG_DFL);
				alarm(30);
				notify(utp, from, subject), exit(0);
			} else
				while (wait2(0, WNOHANG, 0) > 0)
					continue;
}


notify(utp, from, subject)
	register struct utmp *utp;
	char *from, *subject;
{
	FILE *tp;
	char tty[20];
	struct stat stb;
	struct hostent *hp;
	char *fhost;

	strcpy(tty, "/dev/");
	strncat(tty, utp->ut_line, sizeof(utp->ut_line));
	dprintf("notify %s on %s\n", utp->ut_name, tty);
	if (stat(tty, &stb) == 0 && (stb.st_mode & 0100) == 0) {
		dprintf("wrong mode\n");
		return;
	}
	if ((tp = fopen(tty,"w")) == 0) {
		dprintf("fopen failed\n");
		return;
	}
	hp = gethostbyaddr(&ifrom.sin_addr.s_addr, 4, AF_INET);
	if (hp)
		fhost = hp->h_name;
	else
		fhost = inet_ntoa(ifrom.sin_addr.s_addr);
	if (!strcmp(fhost, myname))
		fprintf(tp, "\r\n\007[Mail from %s, Subject: %s]\r\n",
		    from, subject);
	else
		fprintf(tp, "\r\n\007[Mail on %s from %s, Subject: %s]\r\n",
		    fhost, from, subject);
	fclose(tp);
}

