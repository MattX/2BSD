#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
/*
 * Convert network-format internet address
 * to base 256 d.d.d.d representation.
 */
char *
inet_ntoa(in)
	struct in_addr in;
{
	static char b[18];
	register char *p;

/*	in.s_addr = ntohl(in.s_addr);	should this be here; dgc; %%%% */
	p = (char *)&in;
#define	UC(b)	(((int)b)&0xff)
	sprintf(b, "%d.%d.%d.%d", UC(p[0]), UC(p[1]), UC(p[2]), UC(p[3]));
	return (b);
}
