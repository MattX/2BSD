/*	inet_maddr.c	4.2	82/10/07	*/

#include <sys/types.h>
#include <net/in.h>
#include <netdb.h>

/*
 * Formulate an Internet address from network + host.  Used in
 * building addresses stored in the ifnet structure.
 */
struct in_addr
inet_makeaddr(net, lna)
	union {
		long laddr;
		char oct[4];
	} net, lna;
{
	u_long addr;
	struct in_addr result;
	
	if (net.oct[0])			/* class c network */
	    addr = (net.laddr << 8) & 0xffffff00;
	else
	    if (net.oct[3])		/* class b network */
		    addr = (net.laddr << 16) & 0xffff0000;
		else
		    addr = (net.laddr << 24) & 0xff000000;  /* class a */
	addr |= lna.laddr;		/* include lna */
	result.s_addr = htonl(addr);
	return (result);
}
