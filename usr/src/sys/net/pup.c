/*	pup.c	4.2	82/06/20	*/

#include "pup.h"
#if NPUP > 0

#include "param.h"
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <net/af.h>
#include <netpup/pup.h>

pup_hash(spup, hp)
	struct sockaddr_pup *spup;
	struct afhash *hp;
{
	hp->afh_nethash = spup->spup_addr.pp_net;
	hp->afh_hosthash = spup->spup_addr.pp_host;
	if (hp->afh_hosthash < 0)
		hp->afh_hosthash = -hp->afh_hosthash;
}

pup_netmatch(spup1, spup2)
	struct sockaddr_pup *spup1, *spup2;
{
	return (spup1->spup_addr.pp_net == spup2->spup_addr.pp_net);
}
#endif
