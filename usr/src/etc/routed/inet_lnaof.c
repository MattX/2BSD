/*	inet_lnaof.c	4.2	82/10/07	*/

#include <sys/types.h>
#include <net/in.h>
#include <netdb.h>

#undef	IN_CLASSB
#define IN_CLASSB 0x00c00000L
#define IN_SUBNETA 0x80800000L
#define IN_CLASSA_SUBHOST 0x0000ffffL
#define IN_SUBNETB 0x00c00080L
#define IN_CLASSB_SUBHOST 0x0000ff00L

/*
 * Return the local network address portion of an
 * internet address; handles class a/b/c network
 * number formats.
 */
u_long
inet_lnaof(in)
	struct in_addr in;
{
	printf("Returning local address portion of 0x%X\n",in.s_addr);
	if ((in.s_addr&IN_CLASSA) == 0L)
	{
		printf("Class A ");
		if ((in.s_addr&IN_SUBNETA) == 0x80000000L)
		{
			printf("subnet: 0x%X\n",in.s_addr&IN_CLASSA_SUBHOST);
			return (ntohl(in.s_addr & IN_CLASSA_SUBHOST));
		}
		else
		{
			printf("0x%X\n",in.s_addr&IN_CLASSA_LNA);
			return (ntohl(in.s_addr & IN_CLASSA_LNA));
		}
	}
	if ((in.s_addr&IN_CLASSB) == 0x00800000L)
	{
		printf("Class B ");
		if ((in.s_addr&IN_SUBNETB) == 0x00800080L)
		{
			printf("subnet: 0x%X\n",in.s_addr&IN_CLASSB_SUBHOST);
			return (ntohl(in.s_addr & IN_CLASSB_SUBHOST));
		}
		else
		{
			printf("0x%X\n",in.s_addr&IN_CLASSB_LNA);
			return (ntohl(in.s_addr & IN_CLASSB_LNA));
		}
	} else
	{
		printf("Class C: 0x%X\n",in.s_addr&IN_CLASSC_LNA);
		return (ntohl(in.s_addr & IN_CLASSC_LNA));
	}
}
