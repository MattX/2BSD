/*
	This include file describes the data structures needed for ip
	accounting. ip accounting counts the number of packets coming
	in and going out over the local ethernet interface. Currently,
	"il" is the local interface. Change it to your favorite interface
	name.
*/

#define	IP_ACCT				/* yes */

struct ip_acct {
	char c_d[2];			/* the bottom two bytes of ip addr */
	long pkt_cnt;			/* count of packets */
};

#define	N_IPHOSTS	64		/* number of hosts to count */

#define	INTERFACE	"il0"		/* what interface to account */
