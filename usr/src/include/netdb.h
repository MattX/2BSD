/*	netdb.h	4.1	82/10/05	*/

/*
 * this is the include file to include for *any*
 * network program -- it makes compilation possible...
 */

#define gethostbyaddr		gethbyaddr
#define gethostbyname		gethbyname
#define gethostent		gethent
#define gethostid		gethstid
#define getnetbyaddr		getnbyaddr
#define getnetbyname		getnbyname
#define getnetent		getnent
#define getprotobyname		getpbyname
#define getprotobynumber	getpbnumber
#define getprotoent		getpent
#define getservbyname		getsbyname
#define getservbyport		getsbyport
#define getservent		getsent
#define hostnamelen		hostnlen
#define if_ifwithaddr		if_iwaddr
#define if_ifwithaf		if_iwaf
#define if_ifwithnet		if_iwnet
#define in_pcbdetach		in_pcbetach
#define inet_netof		inet_nof
#define inet_network		inet_nwk
#define protoswLAST		protoLAST
#define rawintr			rawint
#define sbappendaddr		sbappadd
#define sethostid		sethstid
#define sethostname		sethstnam
#define sockaddr_in		sock_in
#define sockaddr_pup		sock_pup
#define soisconnected		soisced
#define soisconnecting		soiscing
#define soisdisconnected	soisded
#define soisdisconnecting	soisding
#define spup_zero1		spup_z1
#define spup_zero2		spup_z2
#define ssocketaddr		ssockad
#define tcp_debx		tcp_dbx
#define tcp_initopt		tcp_iopt
#define tcp_output		tcp_oput
#define tcp_usrclosed		tcp_uclosed
#define tcps_badoff		tcps_boff
#define tcps_badsegs		tcps_bsegs
#define tcps_badsum		tcps_bsum
#define tcpstates		tcpstas
#define udps_badlen		udps_blen
#define udps_badsum		udps_bsum

#ifndef KERNEL

/*
 * Structures returned by network
 * data base library.  All addresses
 * are supplied in host order, and
 * returned in network order (suitable
 * for use in system calls).
 */
struct	hostent {
	char	*h_name;	/* official name of host */
	char	**h_aliases;	/* alias list */
	int	h_addrtype;	/* host address type */
	int	h_length;	/* length of address */
	char	*h_addr;	/* address */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 *
 * All elements of the netent structure have been changed to have a prefix
 * of "ne" rather than just "n" since the pdp-11 compilers don't like
 * structures with identically named elements.
 */
struct	netent {
	char	*ne_name;	/* official name of net */
	char	**ne_aliases;	/* alias list */
	int	ne_addrtype;	/* net address type */
	long	ne_net;		/* network # */
};

struct	servent {
	char	*s_name;	/* official service name */
	char	**s_aliases;	/* alias list */
	int	s_port;		/* port # */
	char	*s_proto;	/* protocol to use */
};

struct	protoent {
	char	*p_name;	/* official protocol name */
	char	**p_aliases;	/* alias list */
	int	p_proto;	/* protocol # */
};

struct hostent	*gethostbyname(), *gethostbyaddr(), *gethostent();
struct netent	*getnetbyname(), *getnetbyaddr(), *getnetent();
struct servent	*getservbyname(), *getservbyport(), *getservent();
struct protoent	*getprotobyname(), *getprotobynumber(), *getprotoent();

u_long	inet_addr(), inet_netof(), inet_lnaof(),
	inet_network(), inet_makeaddr(), gethostid();

long	rhost();

char	*raddr(),
	*inet_ntoa();

#endif !KERNEL

u_long	ntohl(), htonl();
u_short	ntohs(), htons();
