

/*******************************************************************
 *
 *  Remote Access Package,  The Server
 *
 *******************************************************************
 *
 *  Copyright (c) 1984 by   Walter F. Tichy and Zuwang Ruan
 *                          Department of Computer Sciences
 *                          Purdue University
 *                          West Lafayette, IN 47907
 *
 *  All rights reserved.  No parts of this software may be sold or
 *  distributed in any form or by any means without the prior written
 *  permission of the authors.
 *
 *******************************************************************/

static char rcsid[] = "$Header: /a/ruan/src/RCS/rfiled.c,v 1.1 84/06/19 22:40:23 ruan Exp $";

/* $Log:	rfiled.c,v $
 * Revision 1.1  84/06/19  22:40:23  ruan
 * Initial revision
 *  */

#include <stdio.h>
#include <sys/wait.h>
#include <assert.h>
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pwd.h>

#include "ra.h"
#include "local.h"

int     mpPsuLoc[NOFILE];
struct  sockaddr_in  from;
int     len = sizeof(from);

/*
 * main () -- fork server process in background; wait
 * for requests; fork a process for each request;
 * after arthentication checking, the dedecate process has the same
 * user id as client process
 */

main ()
{
    int     fdListen, fdConn, fd;
    int     lport = Highest - 1;
    int     rport;
    int     w;

    if (fork () != 0)
	exit (0);	/* parent exits; child remains in background */

    fdListen = CreateListener ();
    for (;;) {
	if ((fdConn = accept (fdListen, (struct sockaddr *)&from, &len)) < 0) {
	    Error ("accept");
	}
	assert (len == sizeof (from));

	if (fork() == 0) {
	    (void)close(fdListen);
	    SecuGuard(fdConn, &from);
	    if ((read(fdConn, (char *)&rport, sizeof(int)) != sizeof(int)) ||
		((fd = bindaport(&lport)) < 0) ||
		(write(fdConn, (char *)&lport, sizeof(int)) != sizeof(int)))
		exit(1);
	    (void)close(fdConn);
	    if (OpenNew(fd, rport, &from) < 0)
		Error("OpenNew");
	    while (Service (fd) == 0)
		;
	    (void)close(fd);
	    exit(0);
	}
	(void)close (fdConn);
	while (wait3(&w, WNOHANG, 0) > 0)
	    ;
    }
}

/*
 * CreateListener () -- create and retuma an fd that will accept
 * connections for service.
 */

CreateListener ()
{
    int fd;
    struct sockaddr_in sa;
    struct servent *pse;

    if (((pse = getservbyname("rfile", NULL)) == NULL) ||
	((fd = socket (AF_INET, SOCK_STREAM, 0)) < 0))
	exit(1);

    sa.sin_family = AF_INET;
    sa.sin_port = pse->s_port;
    sa.sin_addr.s_addr = INADDR_ANY;

    if ((bind (fd, (struct sockaddr *)&sa, sizeof (sa)) == -1) ||
	(listen (fd, 5) == -1)) /* listen for upto 5 connections at a time */
	exit(1);
    return(fd);
}

/*
 * SecuGuard(fd, pfrom) -- authentication checking.  It fails (exits) if
 * remote port >= 1024, remote host not in /etc/hosts.equiv or client's
 * login name not in /etc/passwd.  Otherwise, setuid and setgid to the
 * dedicate process.
 */

SecuGuard(fd, pfrom)
int     fd;
struct  sockaddr_in *pfrom;
{
    int     rport;
    struct  hostent *phe;
    struct  passwd  *pwd;
    char    username[NameLeng];
    extern  int     errno;

    rport = ntohs((u_short)pfrom->sin_port);

    if (((pfrom->sin_family != AF_INET) || (rport >= IPPORT_RESERVED)) ||
	((phe = gethostbyaddr(&pfrom->sin_addr, sizeof(struct in_addr),
	pfrom->sin_family)) == NULL) ||
	(read(fd, username, NameLeng) != NameLeng))
	Error("SecuGuard");
    username[NameLeng - 1] = '\0';

    (void)setpwent();
    if ((pwd = getpwnam(username)) == NULL)
	Error("SecuGuard");
    (void)endpwent();

    if ((chdir(pwd->pw_dir) < 0) ||
	(ruserok(phe->h_name, pwd->pw_uid == 0, username, username) < 0) ||
	(chdir(pwd->pw_dir) < 0) ||
	(initgroups(pwd->pw_name, pwd->pw_gid) < 0) ||
	((setgid(pwd->pw_gid) < 0) || (setuid(pwd->pw_uid) < 0)))
	Error("SecuGuard");

}

/*
 * OpenNew(fd, rport, pfrom) -- open a new connection to port number rport
 * in the same host as pfrom.
 */

OpenNew(fd, rport, pfrom)
int     fd, rport;
struct  sockaddr_in *pfrom;
{
    struct  sockaddr_in sa;

    sa.sin_family = AF_INET;
    sa.sin_port = htons(rport);
    sa.sin_addr = pfrom->sin_addr;

    if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
	Error("Connect");
	(void)close(fd);
	return(-1);
    }
    return(0);
}

/*
 * Service (fd) -- remote system call server.
 */

Service (fd)
int     fd;
{
    struct  command combuf;
    struct  stat    statbuf;
    int     code, fdLocal;
    char    rgch1[cchMax], rgch2[cchMax];
    int     pid, fdNew;
    int     lport = Highest - 1;
    extern  int     errno;

    if (RecvCommand(fd, &combuf, rgch1, rgch2) < 0)
	return(-1);

    switch (combuf.comm) {

	case ACCESS:
	    code = access(rgch1, combuf.arg2);
	    SendResult(fd, code, errno);
	    break;

	case CHMOD:
	    code = chmod(rgch1, combuf.arg2);
	    SendResult(fd, code, errno);
	    break;

	case FCHMOD:
	    code = fchmod(combuf.arg1, combuf.arg2);
	    SendResult(fd, code, errno);
	    break;

	case MKDIR:
	    code = mkdir(rgch1, combuf.arg2);
	    SendResult(fd, code, errno);
	    break;

	case CHDIR:
	    code = chdir(rgch1);
	    SendResult(fd, code, errno);
	    break;

	case RMDIR:
	    code = rmdir(rgch1);
	    SendResult(fd, code, errno);
	    break;

	case TRUNC:
	    code = truncate(rgch1, combuf.arg2);
	    SendResult(fd, code, errno);
	    break;

	case FTRUN:
	    code = ftruncate(combuf.arg1, combuf.arg2);
	    SendResult(fd, code, errno);
	    break;

	case UNLINK:
	    code = unlink(rgch1);
	    SendResult(fd, code, errno);
	    break;

	case LINK:
	    code = link(rgch1, rgch2);
	    SendResult(fd, code, errno);
	    break;

	case SYMLINK:
	    code = symlink(rgch1, rgch2);
	    SendResult(fd, code, errno);
	    break;

	case RENAME:
	    code = rename(rgch1, rgch2);
	    SendResult(fd, code, errno);
	    break;

	case READLINK:
	    SendLink(fd, rgch1, combuf.arg2);
	    break;

	case OPEN:
	    fdLocal = open(rgch1, combuf.arg2, combuf.arg3);
	    mpPsuLoc[combuf.arg4] = fdLocal;
	    SendResult(fd, fdLocal, errno);
	    break;

	case CREAT:
	    fdLocal = creat(rgch1, combuf.arg2);
	    mpPsuLoc[combuf.arg3] = fdLocal;
	    SendResult(fd, fdLocal, errno);
	    break;

	case STAT:
	    code = stat(rgch1, &statbuf);
	    SendResult(fd, code, errno);
	    if (code == 0)
		SendStat(fd, &statbuf);
	    break;

	case FSTAT:
	    fdLocal = mpPsuLoc[combuf.arg1];
	    code = fstat(fdLocal, &statbuf);
	    SendResult(fd, code, errno);
	    if (code == 0)
		SendStat(fd, &statbuf);
	    break;

	case LSTAT:
	    code = lstat(rgch1, &statbuf);
	    SendResult(fd, code, errno);
	    if (code == 0)
		SendStat(fd, &statbuf);
	    break;

	case READ:
	    fdLocal = mpPsuLoc[combuf.arg1];
	    ReadSend(fd, fdLocal, combuf.arg2);
	    break;

	case WRITE:
	    fdLocal = mpPsuLoc[combuf.arg1];
	    RecvWrite(fd, fdLocal, combuf.arg2);
	    break;

	case LSEEK:
	    fdLocal = mpPsuLoc[combuf.arg1];
	    code = lseek(fdLocal, combuf.arg2, combuf.arg3);
	    SendResult(fd, code, errno);
	    break;

	case FLOCK:
	    fdLocal = mpPsuLoc[combuf.arg1];
	    code = flock(fdLocal, combuf.arg2);
	    SendResult(fd, code, errno);
	    break;

	case FSYNC:
	    fdLocal = mpPsuLoc[combuf.arg1];
	    code = fsync(fdLocal);
	    SendResult(fd, code, errno);
	    break;

	case DUP:
	    fdLocal = mpPsuLoc[combuf.arg1];
	    code = dup(fdLocal);
	    if (code >= 0)
		mpPsuLoc[combuf.arg2] = code;
	    SendResult(fd, code, errno);
	    break;

	case CLOSE:
	    fdLocal = mpPsuLoc[combuf.arg1];
	    code = close(fdLocal);
	    SendResult(fd, code, errno);
	    break;

	case UMASK:
	    code = umask(combuf.arg1);
	    SendResult(fd, code, 0);
	    break;

	case FORK:
	    if (((fdNew = bindaport(&lport)) < 0) ||
		((pid = fork()) < 0)) {         /* fail */
		SendResult(fd, -1, errno);
		break;
	    }
	    if (pid > 0) {                      /*parent */
		SendResult(fd, lport, 0);
	    } else {                            /* child */
		(void)close(fd);
		if (OpenNew(fdNew, combuf.arg1, &from) < 0)
		    Error("OpenNew");
		while (Service(fdNew) == 0)
		    ;
		(void)close(fdNew);
		exit(0);
	    }
	    break;

	default:
	    SendResult(fd, -1, ECOMM);
	    break;
    }
    return(0);
}

/*
 * RecvCommand -- receive 'commands' (system calls)
 */

RecvCommand(fd, pcombuf, rgch1, rgch2)
int     fd;
struct  command *pcombuf;
char    rgch1[], rgch2[];
{
    int     cch;

    cch = read(fd, (char *)pcombuf, sizeof(*pcombuf));
    if (cch == 0)
	return(-1);
    assert(cch == sizeof(*pcombuf));
    if (pcombuf->nname > 0) {
	cch = read(fd, rgch1, pcombuf->arg1);
	assert(cch == pcombuf->arg1);
	rgch1[pcombuf->arg1] = '\0';
    } else
	rgch1[0] = '\0';
    if (pcombuf->nname > 1) {
	cch = read(fd, rgch2, pcombuf->arg2);
	assert(cch == pcombuf->arg2);
	rgch2[pcombuf->arg2] = '\0';
    } else
	rgch2[0] = '\0';
    return(0);
}

/*
 * SendResult -- send back return code and error number.
 */

SendResult(fd, code, errno)
int     fd, code, errno;
{
    int     cch;
    struct  result  resbuf;

    resbuf.code = code;
    resbuf.errno = errno;
    cch = write(fd, (char *)&resbuf, sizeof(resbuf));
    assert(cch == sizeof(resbuf));
}

/*
 * SendStat -- send back stat.
 */

SendStat(fd, pstatbuf)
int     fd;
struct  stat    *pstatbuf;
{
    int     cch;

    cch = write(fd, (char *)pstatbuf, sizeof(*pstatbuf));
    assert(cch == sizeof(*pstatbuf));
}

/*
 * SendLink -- send back link (pathname)
 */

SendLink(fd, rgch, leng)
int     fd, leng;
char    *rgch;
{
    int     code, cch;
    struct  data    databuf;

    code = readlink(rgch, databuf.data, MIN(leng, cchMax));
    databuf.leng = code;
    if (code < 0)
	databuf.flag = errno;
    cch = write(fd, (char *)&databuf, sizeof(databuf));
    assert(cch == sizeof(databuf));
}

/*
 * ReadSend -- read data from local file and send to client.
 */

ReadSend(fd, fdLocal, leng)
int     fd, fdLocal, leng;
{
    int     count, cch;
    struct  data    databuf;
    extern  int     errno;

    do {
	count = MIN(leng, cchMax);
	databuf.leng = read(fdLocal, databuf.data, count);
	if (databuf.leng < 0)
	    databuf.flag = errno;
	else {
	    if ((databuf.leng < count) || (leng == count))
		databuf.flag = ENDDATA;
	    else
		databuf.flag = 0;
	    leng -= count;
	}
	cch = write(fd, (char *)&databuf, sizeof(databuf));
	assert(cch == sizeof(databuf));
    } while ((databuf.leng > 0) && (databuf.flag == 0));
}

/*
 * RecvWrite -- receive data from client and write to local file.
 */

RecvWrite(fd, fdLocal, leng)
int     fd, fdLocal, leng;
{
    int     cchAct = 0, count, cch;
    struct  data    databuf;
    extern  int     errno;

    do {
	cch = read(fd, (char *)&databuf, sizeof(databuf));
	assert(cch == sizeof(databuf));
	count = MIN(leng, databuf.leng);
	if (count > 0) {
	    count = write(fdLocal, databuf.data, count);
	    cchAct += count;
	    leng -= count;
	}
    } while (databuf.flag == 0);
    SendResult(fd, cchAct, errno);
}


bindaport(alport)
int     *alport;
{
	struct sockaddr_in sin;
	int s;
	extern int errno;

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		return (-1);
	for (;;) {
		sin.sin_port = htons((u_short)*alport);
		if (bind(s, (struct sockaddr *)&sin, sizeof (sin)) >= 0)
			return (s);
		if (errno != EADDRINUSE && errno != EADDRNOTAVAIL) {
			perror("socket");
			return (-1);
		}
		(*alport)--;
		if (*alport < Lowest) {
			fprintf(stderr, "socket: All ports in use\n");
			return (-1);
		}
	}
}

