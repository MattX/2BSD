/*
 * Structure of an Ethernet header
 */
struct	eth_header {
	u_char	eth_dhost[6];		/* Destination Host */
	u_char	eth_shost[6];		/* Source Host */
	u_short	eth_type;		/* Type of packet */
};

/* Ethernet packet data size limits */
#define	ETHERMTU 1500
#define	ETHERMIN 46
/* def'n for ethernet packet types */
#define	ETHERPUP_PUPTYPE	0x0400		/* PUP protocol */
#define	ETHERPUP_IPTYPE		0x0800		/* IP protocol */
#define	ETHERPUP_ARPTYPE	0x0806		/* Address resolution protocol */

/*
 * The ETHERPUP_NTRAILER packet types starting at ETHERPUP_TRAIL have
 * (type-ETHERPUP_TRAIL)*512 bytes of data followed
 * by a PUP type (as given above) and then the (variable-length) header.
 */
#define	ETHERPUP_TRAIL		0x1000		/* Trailer PUP */
#define	ETHERPUP_NTRAILER	16
u_char etherbroadcastaddr[6];			/* 6 bytes of 0xFF */

/*
 * Ethernet Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  Structure below is adapted
 * to resolving internet addresses.  Field names used correspond to 
 * RFC 826.
 */
struct	eth_arp {
	u_short	arp_hrd;	/* format of hardware address */
#define ARPHRD_ETHER 	1	/* ethernet hardware address */
	u_short	arp_pro;	/* format of proto. address (ETHERPUP_IPTYPE) */
	u_char	arp_hln;	/* length of hardware address (6) */
	u_char	arp_pln;	/* length of protocol address (4) */
	u_short	arp_op;
#define	ARPOP_REQUEST	1	/* request to resolve address */
#define	ARPOP_REPLY	2	/* response to previous request */
	u_char	arp_sha[6];	/* sender hardware address */
	u_char	arp_spa[4];	/* sender protocol address */
	u_char	arp_tha[6];	/* target hardware address */
	u_char	arp_tpa[4];	/* target protocol address */
};

#ifdef	KERNEL
/*
 * Structure shared between the ethernet driver modules and
 * the address resolution code.  For example, each ec_softc or il_softc
 * begins with this structure.
 */
struct	arpcom {
	struct 	ifnet ac_if;	/* network-visible interface */
	u_char	ac_enaddr[6];	/* ethernet hardware address */
	struct	arpcom *ac_ac;	/* link to next ether driver */
};

struct	in_addr arpmyaddr();
struct	arptab *arptnew();
#endif
