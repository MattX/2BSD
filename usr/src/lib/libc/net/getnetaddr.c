/*	getnetaddr.c	4.3	82/10/06	*/

#include <sys/types.h>
#include <netdb.h>

struct netent *
getnetbyaddr(net, type)
	long net;
	register int type;
{
	register struct netent *p;

	setnetent(0);
	while (p = getnetent())
		if (p->ne_addrtype == type && p->ne_net == net)
			break;
	endnetent();
	return (p);
}
