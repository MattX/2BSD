/*	tcp_debug.h	4.2	82/03/24	*/

struct	tcp_debug {
	n_time	td_time;
	short	td_act;
	short	td_ostate;
	caddr_t	td_tcb;
	struct	tcpiphdr td_ti;
	short	td_req;
	struct	tcpcb td_cb;
};
#define	TCP_DSIZ	142		/* Size of tcp_debug in bytes */

#define	TA_INPUT 	0
#define	TA_OUTPUT	1
#define	TA_USER		2
#define	TA_RESPOND	3
#define	TA_DROP		4

#ifdef TANAMES
char	*tanames[] =
    { "input", "output", "user", "respond", "drop" };
#endif

#if !pdp11
#define	TCP_NDEBUG 100
#endif
#if pdp11
#define TCP_NDEBUG 5
#endif
#if !pdp11
struct	tcp_debug tcp_debug[TCP_NDEBUG];
#endif
int	tcp_debx;
