

/*******************************************************************
 *
 *  Remote Access Package,  Client Routines
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

static char rcsid[] = "$Header: /a/ruan/src/RCS/client.c,v 1.1 84/06/19 22:32:18 ruan Exp $";

/* $Log:	client.c,v $
 * Revision 1.1  84/06/19  22:32:18  ruan
 * Initial revision
 *  */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pwd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#include "ra.h"
#include "local.h"

char    hostname[][NameLeng] = HOSTINIT;
extern  int errno;

/* if sbHost is already connected, return the descriptor;
   otherwise, make a new connection to sbHost, return the descriptor;
   return -1 if fail to make connection */

getHostConn(sbHost)
char    *sbHost;
{
    int    fd, fdConn, fd0;
    int    iHost, mask;
    struct hostent      *phe;
    struct sockaddr_in  from;
    int    len = sizeof(from);
    int    pid, w, status;
    int    lport = Highest - 1;
    int    rport;
    int    p0[2], p1[2];
    int    (*sighup)(), (*sigint)(), (*sigquit)();

    if ((phe = gethostbyname(sbHost)) == NULL)
	return(-1);
    iHost = hostid(phe);        /* iHost is a shorthand for host address */
    if ((fdConn = ConnOn(iHost)) != -1)
	return(fdConn);

    if (((fd = bindaport(&lport)) < 0) ||
	(listen(fd, 1) < 0))
	return(-1);

    if (pipe(p0) < 0) {
	(void)close_l(fd);
	return(-1);
    }
    if (pipe(p1) < 0) {
	(void)close_l(fd);
	(void)close_l(p0[0]);
	(void)close_l(p0[1]);
	return(-1);
    }
    if ((pid = fork_l()) == 0) {          /* child */
	(void)close_l(p0[1]);
	(void)close_l(p1[0]);
	if (p0[0] != 0) {
	    dup2_l(p0[0], 0);
	    (void)close_l(p0[0]);
	}
	if (p1[1] != 1) {
	    dup2_l(p1[1], 1);
	    (void)close_l(p1[1]);
	}
	execl(START, "start", sbHost, 0);
	_exit(127);
    }
    if (pid == -1) {
	(void)close_l(p0[0]);
	(void)close_l(p0[1]);
	(void)close_l(p1[0]);
	(void)close_l(p1[1]);
	(void)close_l(fd);
	return(-1);
    }
    (void)close_l(p0[0]);
    (void)close_l(p1[1]);
    if (write_l(p0[1], (char *)&lport, sizeof(int)) != sizeof(int)) {
	(void)close_l(p0[1]);
	(void)close_l(p1[0]);
	(void)close_l(fd);
	return(-1);
    }
    (void)close_l(p0[1]);

    if (read_l(p1[0], (char *)&rport, sizeof(int)) != sizeof(int)) {
	(void)close_l(p1[0]);
	(void)close_l(fd);
	return(-1);
    }
    (void)close_l(p1[0]);

    sighup = signal(SIGHUP, SIG_IGN);
    sigint = signal(SIGINT, SIG_IGN);
    sigquit = signal(SIGQUIT, SIG_IGN);

    while ((w = wait(&status)) != pid && w != -1)
	;
    if ((w == -1) || (status)) {
	(void)close_l(fd);
	return(-1);
    }
    (void)alarm(10);
    fdConn = accept(fd, (struct sockaddr *)&from, &len);
    (void)alarm(0);
    (void)close_l(fd);

    (void)signal(SIGHUP, sighup);
    (void)signal(SIGINT, sigint);
    (void)signal(SIGQUIT, sigquit);

    if ((ntohs((u_short)from.sin_port) != rport) ||
	(from.sin_addr.s_addr != (*(struct in_addr *)phe->h_addr).s_addr)) {
	(void)close_l(fdConn);
	return(-1);
    }
    if (Hide(fdConn, iHost) == -1) {
	(void)close_l(fdConn);
	return(-1);
    }
					/* avoid using fd's 0, 1, 2 */
    if ((fdConn < 3) && (fdConn != -1)) {
	fd0 = fdConn;
	fdConn = fcntl_l(fd0, F_DUPFD, 3);
	(void)close_l(fd0);
    }
					/* set server's mask as client's */
    mask = umask_l(0);
    (void)umask_l(mask);
    if ((SendCommand(fdConn, UMASK, 0, "", "", mask, 0, 0, 0) < 0) ||
	(RecvResult(fdConn) < 0)) {
	(void)close_l(fdConn);
	return(-1);
    }
    return(fdConn);
}

/* return the descriptor if iHost is connected; -1 otherwise */

ConnOn(iHost)
int     iHost;
{
    int     j, flag;

    for (j = 0; j < NOFILE; j++)
	if (((flag = fcntl_l(j, F_GETFL, 0)) != -1) &&
	    (flag & F_HIDDEN) && ((flag >> 16) == iHost))
	    goto found;
    return(-1);
found:
    return(j);
}

/* mark fd as hidden and mapped to iHost */

Hide(fd, iHost)
int     fd, iHost;
{
    int flag;

    if ((flag = fcntl_l(fd, F_GETFL, 0)) == -1)
	return(-1);
    return(fcntl_l(fd, F_SETFL, (flag & 017777) | F_HIDDEN | (iHost << 16)));
}

/* return iHost if fd is hidden; -1 otherwise */

GetHst(fd)
int     fd;
{
    int     flag;

    if (((flag = fcntl_l(fd, F_GETFL, 0)) == -1) ||
	((flag & F_HIDDEN) == 0))
	return(-1);
    return(flag >> 16);
}

/* return 0 if sb is a remote name, 1 otherwise */

IsLocal(sb, sbHost, sbName)
char    *sb, *sbHost, *sbName;
{
    int     hostlen = 0, namelen = 0;
    char    *p2, *p3, *index();

    if (((p2 = index(sb, ':')) == 0) ||                 /* local name */
	((p3 = index(sb, '/')) != 0) && (p2 > p3))
	return(1);
							/* remote name */
    while ((sb < p2) && (hostlen < NameLeng - 1)) {
	*sbHost++ = *sb++;
	hostlen++;
    }
    *sbHost = '\0';
    p2++;
    while ((*p2) && (namelen < cchMax - 1)) {
	*sbName++ = *p2++;
	namelen++;
    }
    *sbName = '\0';
    if ((hostlen == NameLeng - 1) || (namelen == cchMax - 1)) {
	errno = ENAMETOOLONG;
	return(1);
    }
    return(0);
}

/* return 0 if sb contains a cross machine symbolic link, 1 otherwise */

NoRemoteLink(sb, sbHost, sbName)
char    *sb, *sbHost, *sbName;
{
    int     hostlen = 0, namelen = 0, cc;
    char    buf[cchMax];
    char    *p0, *p1, *p2, *p3, *index();
    int     err0;

    err0 = errno;               /* save errno */

    for (p0 = sb; p1 = index(p0, '/'); p0 = p1 + 1) {
	*p1 = '\0';
	cc = readlink_l(sb, buf, cchMax - 1);
	*p1 = '/';              /* restore the '/' */
	if (cc < 0)  {
	    if ((errno == ENXIO) || (errno == EINVAL))
		continue;       /* no symbolic link, check next component */
	    goto no;            /* other errors */
	}

	buf[cc] = '\0';
	if (((p2 = index(buf, ':')) == 0) ||
	    ((p3 = index(buf, '/')) != 0) && (p2 > p3))
	    continue;           /* not cross machine, check next component */
	p3 = buf;               /* cross machine link */
	while ((p3 < p2) && (hostlen < NameLeng - 1)) {
	    *sbHost++ = *p3++;
	    hostlen++;
	}
	*sbHost = '\0';
	p2++;
	while ((*p2) && (namelen < cchMax - 1)) {
	    *sbName++ = *p2++;
	    namelen++;
	}
	if (namelen == 0)       /* symbolic link is <host>: only, drop a '/' */
	    p2 = p1 + 1;
	else
	    p2 = p1;
	while ((*p2) && (namelen < cchMax - 1)) {
	    *sbName++ = *p2++;
	    namelen++;
	}
	*sbName = '\0';
	if ((hostlen == NameLeng - 1) || (namelen == cchMax - 1)) {
	    err0 = ENAMETOOLONG;
	    goto no;
	}
	return(0);
    }

    cc = readlink_l(sb, buf, cchMax - 1);         /* last component */
    if (cc < 0)                                 /* no symbolic link */
	goto no;

    buf[cc] = '\0';
    if (((p2 = index(buf, ':')) == 0) ||
	((p3 = index(buf, '/')) != 0) && (p2 > p3))
	goto no;                                /* not cross machine */
    p3 = buf;
    while ((p3 < p2) && (hostlen < NameLeng - 1)) {
	*sbHost++ = *p3++;
	hostlen++;
    }
    *sbHost = '\0';
    p2++;
    while ((*p2) && (namelen < cchMax - 1)) {
	*sbName++ = *p2++;
	namelen++;
    }
    *sbName = '\0';
    if ((hostlen == NameLeng - 1) || (namelen == cchMax - 1)) {
	err0 = ENAMETOOLONG;
	goto no;
    }
    return(0);

no:
    errno = err0;
    return(1);
}

/* mark fd as pseudo and mapped to fdHid, the descriptor of connection */

setmap(fd, fdHid)
int     fd, fdHid;
{
    int flag;
    if ((GetHst(fdHid) == -1) ||
	((flag = fcntl_l(fd, F_GETFL, 0)) == -1) ||
	((fcntl_l(fd, F_SETFL, (flag & 017777) | F_PSEUDO | (fdHid << 16))) == -1)) {
	return(-1);
    }
    return(0);
}

/* return fdHod if fd is psuedo, -1 otherwise */

getmap(fd)
int     fd;
{
    int     flag, fdHid;

    if (((flag = fcntl_l(fd, F_GETFL, 0)) == -1) ||
	((flag & F_PSEUDO) == 0) ||
	(GetHst(fdHid = (flag >> 16)) == -1))
	return(-1);
    return(fdHid);
}

SendCommand (fd, comm, nname, sb1, sb2, arg1, arg2, arg3, arg4)
int     fd, nname, arg1, arg2, arg3, arg4;
char    *sb1, *sb2;
{
    struct  command combuf;
    int     cch;

    if (GetHst(fd) == -1)
	return(-1);
    combuf.comm = comm;
    combuf.nname = nname;
    combuf.arg1 = arg1;
    combuf.arg2 = arg2;
    combuf.arg3 = arg3;
    combuf.arg4 = arg4;

    if (nname > 0) {
	combuf.arg1 = strlen(sb1) + 1;
    }
    if (nname > 1) {
	combuf.arg2 = strlen(sb2) + 1;
    }
    cch = write_l(fd, (char *)&combuf, sizeof(combuf));

    if (cch != sizeof(combuf))
	return(-1);

    if (nname > 0) {
	cch = write_l(fd, sb1, combuf.arg1);
	if (cch != combuf.arg1)
	    return(-1);
    }
    if (nname > 1) {
	cch = write_l(fd, sb2, combuf.arg2);
	if (cch != combuf.arg2)
	    return(-1);
    }
    return(0);
}



RecvResult(fd)
int     fd;
{
    int     cch;
    struct  result  resbuf;

    if (GetHst(fd) == -1)
	return(-1);

    cch = read_l(fd, (char *)&resbuf, sizeof(resbuf));
    if (cch != sizeof(resbuf))
	return(-1);

    if (resbuf.code < 0)
	errno = resbuf.errno;
    return(resbuf.code);
}

RecvStat(fd, pstatbuf)
int     fd;
struct  stat    *pstatbuf;
{
    int     cch;

    if (GetHst(fd) == -1)
	return(-1);

    cch = read_l(fd, (char *)pstatbuf, sizeof(*pstatbuf));
    if (cch != sizeof(*pstatbuf))
	return(-1);
    return(0);
}

RecvLink(fd, rgch, leng)
int     fd, leng;
char    *rgch;
{
    int     i, cch;
    struct  data    databuf;

    if (GetHst(fd) == -1)
	return(-1);

    cch = read_l(fd, (char *)&databuf, sizeof(databuf));
    if (cch != sizeof(databuf))
	return(-1);
    if (databuf.leng == -1) {
	errno = databuf.flag;
	return(-1);
    }
    for (i = 0; i < (leng = MIN(leng, databuf.leng)); i++)
	*rgch++ = databuf.data[i];
    return(leng);
}

RecvData(fd, rgch, leng)
int     fd, leng;
char    *rgch;
{
    int     lengAct = 0, count, i, cch;
    struct  data    databuf;

    if (GetHst(fd) == -1)
	return(-1);

    do {
	cch = read_l(fd, (char *)&databuf, sizeof(databuf));
	if (cch != sizeof(databuf))
	    return(-1);
	if (databuf.leng < 0)
	    errno = databuf.flag;
	else {
	    count = MIN(leng, databuf.leng);
	    for (i = 0; i < count; i++)
		*rgch++ = databuf.data[i];
	    lengAct += count;
	    leng -= count;
	}
    } while ((databuf.leng > 0) && (databuf.flag == 0));
    if (databuf.leng < 0)
	return(-1);
    return(lengAct);
}

SendData(fd, rgch, leng)
int     fd, leng;
char    *rgch;
{
    int     count, i, cch;
    struct  data    databuf;

    if (GetHst(fd) == -1)
	return(-1);

    do {
	count = MIN(leng, cchMax);
	for (i = 0; i < count; i++)
	    databuf.data[i] = *rgch++;
	databuf.leng = count;
	leng -= count;
	databuf.flag = (leng > 0)? 0: ENDDATA;
	cch = write_l(fd, (char *)&databuf, sizeof(databuf));
	if (cch != sizeof(databuf))
	    return(-1);
    } while (databuf.flag == 0);
    return(RecvResult(fd));
}


bindaport(alport)
	int *alport;
{
	struct sockaddr_in sin;
	int s;

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


listenNew(iHost, fdListen, rport)
int     iHost, fdListen, rport;
{
    int     fdConn;
    struct  sockaddr_in  from;
    int     len = sizeof(from);
    struct  hostent *phe;

    if ((phe = gethostbyname(hostname[iHost])) == NULL)
	exit(1);
    if (listen(fdListen, 1) < 0)
	Error("listen");
    (void)alarm(10);
    if ((fdConn = accept(fdListen, (struct sockaddr *)&from, &len)) < 0)
	Error("accept");
    (void)alarm(0);

    if ((ntohs((u_short)from.sin_port) != rport) ||
	(from.sin_addr.s_addr != (*(struct in_addr *)phe->h_addr).s_addr)) {
	exit(1);
    }
    if (Hide(fdConn, iHost) == -1)              /* mark it hidden */
	exit(1);
    return(fdConn);
}


@
