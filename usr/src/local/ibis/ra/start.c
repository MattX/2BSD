

/*******************************************************************
 *
 *  Remote Access Package,  The Connection Starter
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

static char rcsid[] = "$Header: /a/ruan/src/RCS/start.c,v 1.1 84/06/19 22:40:47 ruan Exp $";

/* $Log:	start.c,v $
 * Revision 1.1  84/06/19  22:40:47  ruan
 * Initial revision
 *  */

#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pwd.h>
#include <errno.h>

#include "ra.h"

/*
 * main(argc, argv) -- connect to the Remote Access server in host argv[1].
 * Pass the caller's login name and the caller reserved port number
 * to remote server.  Get the server reserved port number and pass it
 * to the caller.  Itself uses 'root-reserved-port-number' so that
 * the server can trust it.
 */

main(argc, argv)
int     argc;
char    *argv[];
{
    struct sockaddr_in  sa;
    struct passwd *pwd;
    int    fd;
    int    lport = IPPORT_RESERVED - 1;
    int    rport;
    unsigned  timo = 1;
    int    userport;
    char   namebuff[NameLeng], *strncpy();
    extern int     errno;
    struct hostent *phe;
    struct servent *pse;

    if ((argc != 2) ||
	((pwd = getpwuid(getuid())) == NULL) ||
	((phe = gethostbyname(argv[1])) == NULL) ||
	((pse = getservbyname("rfile", NULL)) == NULL))
	exit(1);

retry:
    if ((fd = rresvport(&lport)) < 0)       /* reserve port number < 1024 */
	exit(1);

    sa.sin_family = AF_INET;
    sa.sin_port = pse->s_port;
    sa.sin_addr = *((struct in_addr *)phe->h_addr);

    if (connect(fd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
	(void)close(fd);
	if (errno == EADDRINUSE) {
	    lport--;
	    goto retry;
	}
	if ((errno == ECONNREFUSED) && (timo <= 4)) {
	    sleep(timo);
	    timo *= 2;
	    goto retry;
	}
	exit(1);
    }

    (void)strncpy(namebuff, pwd->pw_name, NameLeng);
    if (write(fd, namebuff, NameLeng) != NameLeng) {
	(void)close(fd);
	exit(1);
    }
    if ((read(0, (char *)&userport, sizeof(int)) != sizeof(int)) ||
	(write(fd, (char *)&userport, sizeof(int)) != sizeof(int))) {
	(void)close(fd);
	exit(1);
    }

    if ((read(fd, (char *)&rport, sizeof(int)) != sizeof(int)) ||
	(write(1, (char *)&rport, sizeof(int)) != sizeof(int))) {
	(void)close(fd);
	exit(1);
    }
    exit(0);
}


