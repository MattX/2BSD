/*	inet_netof.c	4.2	82/10/07	*/

#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

/*
 * Return the network number from an internet
 * address; handles class a/b/c network #'s.
 */
u_long
inet_netof(in)
	struct in_addr in;
{
	if (IN_CLASSA(in.s_addr))
	   if(IN_SUBNETA(in.s_addr))
		return 0x0000ffffL&(ntohl(in.s_addr & IN_CLASSA_SUBNET) >> 16);
	   else
		return 0x000000ffL&(ntohl(in.s_addr & IN_CLASSA_NET) >> 24);
	if (IN_CLASSB(in.s_addr))
	   if(IN_SUBNETB(in.s_addr))
		return 0x00ffffffL&(ntohl(in.s_addr & IN_CLASSB_SUBNET) >> 8);
	   else
		return 0x0000ffffL&(ntohl(in.s_addr & IN_CLASSB_NET) >> 16);
	else
		return 0x00ffffffL&(ntohl(in.s_addr & IN_CLASSC_NET) >> 8);
}
