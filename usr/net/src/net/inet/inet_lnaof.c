/*	inet_lnaof.c	4.2	82/10/07	*/

#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

/*
 * Return the local network address portion of an
 * internet address; handles class a/b/c network
 * number formats.
 */
u_long
inet_lnaof(in)
	struct in_addr in;
{
	u_long lna;

	lna = ntohl(IN_LNAOF(in));
	return(lna);
}
