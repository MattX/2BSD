#ifndef lint
static char sccsid[] = "@(#)raddr.c	4.1 82/04/02";
#endif

#include <stdio.h>
#include <sys/types.h>
#include <netdb.h>

char	*index(), *rindex(), *malloc();

char *
raddr(desaddr)
	long desaddr;
{
	FILE *hf = fopen("/etc/hosts", "r");
	char hbuf[BUFSIZ], *cp, *host;
	int first = 1;

	if (hf == NULL) {
		perror("/etc/hosts");
		exit(1);
	}
top:
	while (fgets(hbuf, sizeof (hbuf), hf) && index(hbuf, '\n')) {
		long addr,rnumber();

		*index(hbuf, '\n') = 0;
		if (hbuf[0] == '#')
			continue;
		if ((addr = rnumber(hbuf)) == -1)
			continue;
		if (addr != desaddr)
			continue;
		host = index(hbuf, ' ') + 1;
		cp = index(host, ' ');
		if (cp)
			*cp = 0;
		cp = malloc(strlen(host)+1);
		strcpy(cp, host);
		fclose(hf);
		return (cp);
	}
	if (first == 1) {
		first = 0;
		fclose(hf);
		if (hf = fopen("/etc/hosts.local", "r"))
			goto top;
		fclose (hf);
		return (0);
	}
bad:
	fclose(hf);
	return (0);
}
