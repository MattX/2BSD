/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if	!defined(lint) && defined(DOSCCS)
char copyright[] =
"@(#) Copyright (c) 1983 Regents of the University of California.\n\
 All rights reserved.\n";
static char sccsid[] = "@(#)hostid.c	1.2 (2.11BSD GTE) 1/1/94";
#endif

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <netdb.h>

extern	char *index();
extern	unsigned long inet_addr();

main(argc, argv)
	int argc;
	char **argv;
{
	register char *id;
	u_long addr;
	long hostid;
	struct hostent *hp;

	if (argc < 2) {
#ifdef pdp11
		{
			long	val;
			if (val = gethostid())
				printf("0x%lx\n",val);
			else
				puts("0");
		}
#else
		printf("%#lx\n", gethostid());
#endif
		exit(0);
	}

	id = argv[1];
	if (hp = gethostbyname(id)) {
		bcopy(hp->h_addr, &addr, sizeof(addr));
		hostid = addr;
	} else if (index(id, '.')) {
		if ((hostid = inet_addr(id)) == -1)
			goto usage;
	} else {
		if (id[0] == '0' && (id[1] == 'x' || id[1] == 'X'))
			id += 2;
		if (sscanf(id, "%lx", &hostid) != 1) {
usage:
			fprintf(stderr,
			    "usage: %s [hexnum or internet address]\n",
			    argv[0]);
			exit(1);
		}
	}

	if (sethostid(hostid) < 0) {
		perror("sethostid");
		exit(1);
	}

	exit(0);
}
