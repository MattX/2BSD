/*	if_qn.h	4.3	82/08/25	*/

/*
 * Structure of an Ethernet header
 */
struct	qn_header {
	u_char	qn_dhost[6];		/* Destination Host */
	u_char	qn_shost[6];		/* Source Host */
	u_short	qn_type;		/* Type of packet */
};

