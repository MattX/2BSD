/*	inet_netof.c	4.2	82/10/07	*/

#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 * Return the network number from an internet
 * address; handles class a/b/c network #'s.
 */
u_long
inet_netof(in)
	struct in_addr in;
{
	register u_long net;

	net = ntohl(IN_NETOF(in));
	if ((in.s_addr&IN_CLASSA) == 0)
		return((net>>24)&0xff);
	if ((in.s_addr&IN_CLASSB) == 0)
		return ((net>>16)&0xffff);
	return ((net>>8)&0xffffff);
}
