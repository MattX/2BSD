/*	inet_lnaof.c	4.2	82/10/07	*/

#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

/*
 * Return the local network address portion of an
 * internet address; handles class a/b/c network
 * number formats.
 */
u_long
inet_lnaof(in)
	struct in_addr in;
{
	if (IN_CLASSA(in.s_addr))
	   if (IN_SUBNETA(in.s_addr))
		return 0x0000ffffL&(ntohl(in.s_addr & IN_CLASSA_SUBHOST));
	   else
		return 0x00ffffffL&(ntohl(in.s_addr & IN_CLASSA_LNA));
	if (IN_CLASSB(in.s_addr))
	   if (IN_SUBNETB(in.s_addr))
		return 0x000000ffL&(ntohl(in.s_addr & IN_CLASSB_SUBHOST));
	   else
		return 0x0000ffffL&(ntohl(in.s_addr & IN_CLASSB_LNA));
	else
	   return 0x000000ffL&(ntohl(in.s_addr & IN_CLASSC_LNA));
}
