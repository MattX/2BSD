
#include <stdio.h>
#include <sys/param.h>

#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <netinet/in.h>

#define	KERNEL			/* to get routehash and RTHASHSIZ */
#include <net/route.h>
#undef	KERNEL

#include <netinet/in_pcb.h>
#include <net/if.h>

#include <netinet/ip_var.h>
#include <netinet/tcp.h>

#define TCPSTATES		/* to define tcpstates */

#include <netinet/tcp_fsm.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>
#include <netinet/udp.h>
#include <netinet/udp_var.h>
#include <net/raw_cb.h>
#include "crash.h"

#define klseek(fd,base,off)	lseek(fd, (long)((unsigned)base), off)
#define	VALADD(a,type)		((unsigned)(a) > allocs && (unsigned)(a) < alloct-sizeof (type))
#define	VALMBA(a)		VALADD(a,struct mbuf)
#define	VALMBXA(a)		((unsigned)(a) >= mbbase && (unsigned)(a) <= mbend-(sizeof (struct mbufx)/64))

long lseek();
struct arenas	*getars(),
		*putars();
char	*hoststr(),
	*inet_ntoa();

/*
 * Header portion of struct mbufx, defined in mbuf.h
 */
struct mbufxh {				/* external part of mbuf */
	struct	mbuf *mh_mbuf;		/* back pointer to mbuf */
	short	mh_ref;			/* reference counter */
};

/*
 * Arena (Mbuf) cross-reference table entry
 */
struct arenas {
	struct	arenas *as_next;	/* link */
	unsigned as_addr;		/* address in arena*/
	short   as_ref;			/* reference counter */
	short	as_size;
	short	as_flags;
};
#define	AS_SOCK		01
#define	AS_MBUF		02
#define	AS_ARCK		04
#define	AS_BDSZ		010
#define	AS_FREE		020
#define	AS_INPCB	040
#define	AS_TCPCB	0100
#define	AS_RTENT	0200
#define	AS_RAWCB	0400

/*
 * The following defines are taken from mbuf11.c.  They should
 * be in an include file.
 */

#define IOSIZE		8192	/* area for DMA buffers */
#define NMBUFS		60	/* mbuf area = NMBUFS*MSIZE */
#define NMBCACHE	8	/* cache of mbufs */
#define NWORDS		2000	/* init arena size, bytes=(NWORDS-2)*WORD */
struct mbuf	*mbcache[NMBCACHE];	/* mbuf cache for fast allocation */
int		mbicache;	/* # entries in cache and index of next add */
u_int		mbend;
unsigned int	mbbase,
		mbsize,
		miobase,
		miosize,
		mbfree;

/* End of stuff taken from mbuf11.c */

struct mbstat	mbstat;
struct ipstat	ipstat;
unsigned int	allocs,
		alloct;
int in_ckodd;			/* from in_cksum.c */
struct arenas	arenash = {0};	/* list head */

struct	prname {
	int	pr_id;
	char	*pr_name;
};

struct	prname inetprotos[] = {
	{ IPPROTO_ICMP,	"icmp" },
	{ IPPROTO_GGP,	"ggp", },
	{ IPPROTO_TCP,	"tcp", },
	{ IPPROTO_PUP,	"pup", },
	{ IPPROTO_UDP,	"udp", },
	{ IPPROTO_RAW,	"raw", },
	0
};

struct	prname impprotos[] = {
	{ IMPLINK_IP,	"ip" },
	0
};

struct	pfname {
	int	pf_id;
	char	*pf_name;
	struct	prname *pf_protos;
} pfnames[] = {
	{ PF_UNSPEC,	"unspec",	0 },
	{ PF_UNIX,	"unix",		0 },
	{ PF_INET,	"inet",		inetprotos },
	{ PF_IMPLINK,	"implink",	impprotos },
	{ PF_PUP,	"pup",		0 },
	{ PF_CHAOS,	"chaos",	0 },
	{ PF_OISCP,	"oiscp",	0 },
	{ PF_NBS,	"nbs",		0 },
	{ PF_ECMA,	"ecma",		0 },
	{ PF_DATAKIT,	"datakit",	0 },
	{ PF_CCITT,	"ccitt",	0 },
	0
};

struct	portname {
	int	port_id;
	char	*port_name;
} ipports[] = {
	{ IPPORT_ECHO,		"echo" },
	{ IPPORT_DISCARD,	"sink" },
	{ IPPORT_SYSTAT,	"systat" },
	{ IPPORT_DAYTIME,	"daytime" },
	{ IPPORT_NETSTAT,	"netstat" },
	{ IPPORT_FTP,		"ftp" },
	{ IPPORT_TELNET,	"telnet" },
	{ IPPORT_SMTP,		"smtp" },
	{ IPPORT_TIMESERVER,	"time" },
	{ IPPORT_NAMESERVER,	"name" },
	{ IPPORT_WHOIS,		"whois" },
	{ IPPORT_MTP,		"mtp" },
	{ IPPORT_EXECSERVER,	"exec" },	/* host specific functions */
	{ IPPORT_LOGINSERVER,	"login" },
	{ IPPORT_CMDSERVER,	"shell" },
	{ IPPORT_TFTP,		"tftp" },
	{ IPPORT_RJE,		"rje" },
	{ IPPORT_FINGER,	"finger" },
	{ IPPORT_TTYLINK,	"ttylink" },
	{ IPPORT_SUPDUP,	"supdup" },
	0
};

struct	portname udpports[] = {
	{ IPPORT_ECHO,		"echo" },
	{ IPPORT_DISCARD,	"sink" },
	{ IPPORT_DAYTIME,	"daytime" },
	{ IPPORT_TIMESERVER,	"time" },
	{ IPPORT_NAMESERVER,	"name" },
	{ IPPORT_WHOIS,		"whois" },
	{ IPPORT_BIFFUDP,	"biff" },	/* host specific functions */
	{ IPPORT_WHOSERVER,	"who" },
	{ IPPORT_FINGER,	"finger" },
	{ IPPORT_SYSLOG,	"syslog" },
	{ IPPORT_ROUTESERVER,	"route" },
	0
};

int	inetprint(), pupprint();

struct	sockname {
	int	sa_id;
	char	*sa_name;
	int	(*sa_printer)();
} addrnames[] = {
	{ AF_UNSPEC,	"unspec",	0 },
	{ AF_UNIX,	"unix",		0 },
	{ AF_INET,	"inet",		inetprint },
	{ AF_IMPLINK,	"implink",	inetprint },
	{ AF_PUP,	"pup",		pupprint },
	{ AF_CHAOS,	"chaos",	0 },
	{ AF_OISCP,	"oiscp",	0 },
	{ AF_NBS,	"nbs",		0 },
	{ AF_ECMA,	"ecma",		0 },
	{ AF_DATAKIT,	"datakit",	0 },
	{ AF_CCITT,	"ccitt",	0 },
	0
};

int	gcore,	/* Global FD for core file */
	line;
char *arenap;	/* stuffed with addr of arena, an auto variable */

struct fetch fetchnet[] = {
	"_mbstat",	(char *) &mbstat,	sizeof mbstat,
	"_ipstat",	(char *) &ipstat,	sizeof ipstat,
	"_mbbase",	(char *) &mbbase,	sizeof mbbase,
	"_mbend",	(char *) &mbend,	sizeof mbend,
	"_mbsize",	(char *) &mbsize,	sizeof mbsize,
	"_miobase",	(char *) &miobase,	sizeof miobase,
	"_miosize",	(char *) &miosize,	sizeof miosize,
	"_mbfree",	(char *) &mbfree,	sizeof mbfree,
	"_mbicache",	(char *) &mbicache,	sizeof mbicache,
	"_mbcache",	(char *) &mbcache,	sizeof mbcache,
	"_alloct",	(char *) &alloct,	sizeof alloct,
	"_in_ckod",	(char *) &in_ckodd,	sizeof in_ckodd,
	END
};

struct display net_tab[] = {
	"\nMBUFS:\tfree (low water mark)", (char *) &(mbstat.m_mbufs),DEC,0,
	"\tfree (current)",	(char *) &(mbstat.m_mbfree),	DEC,0,
	"\n\tmbufs in mbuf cache",	(char *) &mbicache,	DEC,0,
	"\tdrops",	(char *) &(mbstat.m_drops),	DEC,0,
	"\nMBUFX area:\tStart click",	(char *) &mbbase,	OCT,0,
	"\tEnd click",	(char *) &mbend,	OCT,0,
	"\n\tSize in bytes",	(char *) &mbsize,	OCT,0,
	"\tmbufx free list (mbfree)",	(char *) &mbfree,	OCT,0,
	"\nARENA:\tallocs",	(char *) &allocs,	OCT,0,
	"\talloct",	(char *) &alloct,	OCT,0,
	"\n\tchars free (low water mark)",(char *) &(mbstat.m_clusters),DEC,0,
	"\tchars free (current)",	(char *) &(mbstat.m_clfree),	DEC,0,
	"\nMIO area:\tstart of free area",	(char *) &miobase,	OCT,0,
	"\tchars free",	(char *) &miosize,	OCT,0,
	"\nIP:\tbadsum",	(char *) &(ipstat.ips_badsum),	DEC,0,
	"\ttooshort",	(char *) &(ipstat.ips_tooshort),	DEC,0,
	"\nMISC:\tin_ckodd",	(char *) &in_ckodd,	DEC,0,
	END
};

struct display mbuf_tab[] = {
	"\nnext",	(char *) &((struct mbuf *)0)->m_next,	OCT,0,
	"\toff",	(char *) &((struct mbuf *)0)->m_off,	OCT,0,
	"\tlen",	(char *) &((struct mbuf *)0)->m_len,	OCT,0,
	"\tclick",	(char *) &((struct mbuf *)0)->m_click,	OCT,0,
	"\tact",	(char *) &((struct mbuf *)0)->m_act,	OCT,0,
	END
};

struct display mbufx_tab[] = {
	"\tm_mbuf",	(char *) &((struct mbufxh *)0)->mh_mbuf,OCT,0,
	"\tm_ref",	(char *) &((struct mbufxh *)0)->mh_ref,	DEC,0,
	END
};

/*
 * Display mbuf status info
 */
dispnet()
{
	char arena[NWORDS*2];

	arenap = &arena[0];		/* fill in static variables */
	fetch(fetchnet);
	allocs = findv("_allocs");	/* fill in automatic variables */
	vf("_allocs",(char *)arena, sizeof arena);
	newpage();
	display(net_tab,0);
	putchar('\n');			/* misc checks */
	if (alloct-allocs != (NWORDS-1)*2)
		printf("\tWARNING: allocs/alloct inconsistent\n");
	if (mbfree != 0 && !VALMBXA(mbfree))
		printf("\tWARNING: mbfree pointer is bad\n");
	if (mbicache < 0 || mbicache >= NMBCACHE)
		printf("\tWARNING: mbcache pointer is bad\n");
	arenack();
	mbufpr();
	defpr();
	intpr();
	routepr();
	ifqpr();
	orphans();
	clrars();	/* return memory to pool */
}

/*
 * Print info in interfaces.
 */
intpr()
{
	struct ifnet	ifnet;
	int	ifoff;
	char	name[16];

	if (!vf("_ifnet", (char *) &ifoff, sizeof ifoff)) return;
	printf("\nName\t  mtu  net     addr          ipkts ierrs   opkts oerrs  collis flags\n");
	while (ifoff) {
		struct sockaddr_in *sin;
		struct in_addr *in;	/* ifnet & name are not in arena */

		klseek(gcore, ifoff, 0);
		read(gcore, &ifnet, sizeof ifnet);
		klseek(gcore, (int)ifnet.if_name, 0);
		read(gcore, name, 16);
		sin = (struct sockaddr_in *)&ifnet.if_addr;
		in = (struct in_addr *)&ifnet.if_net;
		printf("%s%d:\t%5d  %-7.7s %11.11s %7D %5D %7D %5D  %6D ",name,ifnet.if_unit,ifnet.if_mtu,getnet(in->s_addr),hoststr(sin->sin_addr.s_addr),ifnet.if_ipackets,ifnet.if_ierrors, ifnet.if_opackets,ifnet.if_oerrors, ifnet.if_collisions);
#define	IF_FLAGS "\010\1UP\2BROADCAST\3DEBUG\4ROUTE\5POINTOPOINT\6NOTRAILERS\7RUNNING\10NOARP"
		printb((u_long) ifnet.if_flags, IF_FLAGS);
		putchar('\n');			/* run through the mbufs */
		if (ifnet.if_snd.ifq_head !=0 || ifnet.if_snd.ifq_len != 0 || ifnet.if_snd.ifq_drops !=0) {
			chasem(ifnet.if_snd.ifq_head,"send",ifnet.if_snd.ifq_len);
			printf("--send queue: head=%o  tail=%o  len=%d  maxlen=%d  drops=%d\n",ifnet.if_snd.ifq_head, ifnet.if_snd.ifq_tail,ifnet.if_snd.ifq_len,ifnet.if_snd.ifq_maxlen, ifnet.if_snd.ifq_drops);
		}
		ifoff = (off_t) ifnet.if_next;
	}
}

ifqpr()
{
	struct	ifqueue	ifqueue;		/* packet input queue */

	putchar('\n');
#ifdef	INET					/* ip packet input queue */
	if(vf("_ipintrq", (char *) &ifqueue, sizeof ifqueue)) {
		chasedm(ifqueue.ifq_head,"input",ifqueue.ifq_len);
		printf("ip packet input queue:\n--head=%o  tail=%o  len=%d  maxlen=%d  drops=%d\n",ifqueue.ifq_head, ifqueue.ifq_tail, ifqueue.ifq_len,ifqueue.ifq_maxlen, ifqueue.ifq_drops);
	}
#endif	INET
	/* raw packet input queue */
	if(vf("_rawintr", (char *) &ifqueue, sizeof ifqueue)) {
		chasedm(ifqueue.ifq_head,"input",ifqueue.ifq_len);
		printf("raw packet input queue:\n--head=%o  tail=%o  len=%d  maxlen=%d  drops=%d\n",ifqueue.ifq_head, ifqueue.ifq_tail, ifqueue.ifq_len,ifqueue.ifq_maxlen, ifqueue.ifq_drops);
	}
}

defpr()
{
	newpage();
	puts("\n                                           RECEIVE  SEND\naddress      local          foreign         cc mb   cc mb   proto  state");
	tcppr();
	udppr();
	rawpr();
}

tcppr()
{
	register struct arenas *asp;
	struct inpcb tcb;
	struct inpcb *inpcbp;
	struct tcpstat tcpstat;
	struct inpcb *prev, *next;
	struct inpcb *atcb;
	struct socket *sockp;
	struct tcpcb *tcpcbp;
	int rcvct;	/* number of mbufs in receive queue */
	int sndct;	/* number of mbufs in send queue */

	printf("tcp:");
	if(vf("_tcpstat", (char *) &tcpstat, sizeof tcpstat))
		printf(" badsum %d badoff %d hdrops %d badsegs %d unack %d\n",tcpstat.tcps_badsum, tcpstat.tcps_badoff,tcpstat.tcps_hdrops, tcpstat.tcps_badsegs,tcpstat.tcps_unack);

	if(!vf("_tcb", (char *) &tcb, sizeof tcb)) return;
	atcb = (struct inpcb *)findv("_tcb");
	inpcbp = &tcb;
	prev = atcb;
	while (inpcbp->inp_next != atcb) {
		next = inpcbp->inp_next;
		if (!VALADD(next, struct inpcb)) {
			printf("bad inpcb address (");
			octout((unsigned)next);
			puts(")");
			break;
		}
		asp = putars((unsigned)next,sizeof(struct inpcb),AS_INPCB);
		asp->as_ref++;
		inpcbp = (struct inpcb *)(arenap + (unsigned)next - allocs);
		if (inpcbp->inp_prev != prev) {
			puts("???");
			break;
		}
		if (!VALADD(inpcbp->inp_socket, struct socket)) {
			printf("bad socket address (");
			octout((unsigned)inpcbp->inp_socket);
			puts(")");
			break;
		}
		asp = putars((unsigned)inpcbp->inp_socket,sizeof(struct socket),AS_SOCK);
		asp->as_ref++;
		sockp = (struct socket *) (arenap + (unsigned)inpcbp->inp_socket - allocs);
		if (!VALADD(inpcbp->inp_ppcb, struct tcpcb)) {
			printf("bad tcpcb address (");
			octout((unsigned)inpcbp->inp_ppcb);
			puts(")");
			break;
		}
		asp = putars((unsigned)inpcbp->inp_ppcb,sizeof(struct tcpcb),AS_TCPCB);
		asp->as_ref++;
		tcpcbp = (struct tcpcb *) (arenap + (unsigned)inpcbp->inp_ppcb - allocs);
		/* chase the chains of mbufs */
		rcvct = chasem(sockp->so_rcv.sb_mb,"receive",sockp->so_rcv.sb_cc);
		sndct = chasem(sockp->so_snd.sb_mb,"send",sockp->so_snd.sb_cc);
		printf("%6o ", inpcbp->inp_ppcb);
		internetprint(&inpcbp->inp_laddr, inpcbp->inp_lport, ipports);
		internetprint(&inpcbp->inp_faddr, inpcbp->inp_fport, ipports);
		printf("%5d%3d%5d%3d    tcp   ",sockp->so_rcv.sb_cc, rcvct, sockp->so_snd.sb_cc, sndct);
		if (tcpcbp->t_state < 0 || tcpcbp->t_state >= TCP_NSTATES)
			printf("%d\n",tcpcbp->t_state);
		else
			printf("%s\n",tcpstates[tcpcbp->t_state]);

		/* dicta */
		if (tcpcbp->t_tcpopt || tcpcbp->t_ipopt || tcpcbp->t_inpcb != next)
		printf("--tcp opts=%o ip opts=%o t_inpcb=%o (actual inpcb=%o)\n",tcpcbp->t_tcpopt, tcpcbp->t_ipopt, tcpcbp->t_inpcb, next);
		prev = next;
	}
}

udppr()
{
	register struct arenas *asp;
	struct inpcb udb;
	struct inpcb *inpcbp;
	struct udpstat udpstat;
	struct inpcb *prev, *next;
	struct inpcb *audb;
	struct socket sock;
	struct socket *sockp;
	int rcvct;	/* number of mbufs in receive queue */
	int sndct;	/* number of mbufs in send queue */

	printf("udp:");
	if (!vf("_udpstat", (char *) &udpstat, sizeof udpstat)) return;
	printf(" hdrops %d badsum %d badlen %d\n",udpstat.udps_hdrops, udpstat.udps_badsum,udpstat.udps_badlen);

	if (!vf("_udb",	(char *) &udb,	sizeof udb)) return;
	audb = (struct inpcb *)findv("_udb");
	inpcbp = &udb;
	prev = audb;
	while (inpcbp->inp_next != audb) {
		next = inpcbp->inp_next;
		if (!VALADD(next, struct inpcb)) {
			printf("bad inpcb address (");
			octout((unsigned)next);
			puts(")");
			break;
		}
		asp = putars((unsigned)next,sizeof(struct inpcb),AS_INPCB);
		asp->as_ref++;
		inpcbp = (struct inpcb *)(arenap + (unsigned)next - allocs);
		if (inpcbp->inp_prev != prev) {
			puts("???");
			break;
		}
		if (!VALADD(inpcbp->inp_socket, struct socket)) {
			printf("bad socket address (");
			octout((unsigned)inpcbp->inp_socket);
			puts(")");
			break;
		}
		asp = putars((unsigned)inpcbp->inp_socket,sizeof(struct socket),AS_SOCK);
		asp->as_ref++;
		sockp = (struct socket *) (arenap + (unsigned)inpcbp->inp_socket - allocs);
		/* chase the chains of mbufs */
		rcvct = chasem(sockp->so_rcv.sb_mb,"receive",sockp->so_rcv.sb_cc);
		sndct = chasem(sockp->so_snd.sb_mb,"send",sockp->so_snd.sb_cc);
		printf("%6o ", next);
		internetprint(&inpcbp->inp_laddr, inpcbp->inp_lport, udpports);
		internetprint(&inpcbp->inp_faddr, inpcbp->inp_fport, udpports);
		printf(" %4d%3d %4d%3d    udp\n",sockp->so_rcv.sb_cc, rcvct, sockp->so_snd.sb_cc, sndct);
		prev = next;
	}
}

rawpr()
{
	register struct arenas *asp;
	struct rawcb *prev, *next;
	struct rawcb *pcbp;
	struct rawcb *arawcb;
	struct socket *sockp;
	struct protosw proto;
	struct rawcb rawcb;
	int rcvct;	/* number of mbufs in receive queue */
	int sndct;	/* number of mbufs in send queue */

	printf("raw:\n");
	if (!vf("_rawcb", (char *) &rawcb, sizeof rawcb)) return;
	arawcb = (struct rawcb *)findv("_rawcb");
	pcbp = &rawcb;
	prev = arawcb;
	while (pcbp->rcb_next != arawcb) {
		next = pcbp->inp_next;
		if (!VALADD(next, struct rawcb)) {
			printf("bad rawcb address (");
			octout((unsigned)next);
			puts(")");
			break;
		}
		asp = putars((unsigned)next,sizeof(struct rawcb),AS_RAWCB);
		asp->as_ref++;
		pcbp = (struct rawcb *)(arenap + (unsigned)next - allocs);
		if (pcbp->rcb_prev != prev) {
			puts("???");
			break;
		}
		printf("%6o ", next);
		if (!VALADD(pcbp->rcb_socket, struct socket)) {
			printf("bad socket address (");
			octout((unsigned)pcbp->rcb_socket);
			puts(")");
			break;
		}
		asp = putars((unsigned)pcbp->rcb_socket,sizeof(struct socket),AS_SOCK);
		asp->as_ref++;
		sockp = (struct socket *) (arenap + (unsigned)pcbp->rcb_socket - allocs);
		klseek(gcore, (off_t)sockp->so_proto, 0);
		read(gcore, &proto, sizeof (proto));
		/* chase the chains of mbufs */
		rcvct = chasem(sockp->so_rcv.sb_mb,"receive",sockp->so_rcv.sb_cc);
		sndct = chasem(sockp->so_snd.sb_mb,"send",sockp->so_snd.sb_cc);
		printaddress(&pcbp->rcb_laddr);
		printaddress(&pcbp->rcb_faddr);
		printf(" %4d%3d %4d%3d    ",sockp->so_rcv.sb_cc, rcvct, sockp->so_snd.sb_cc, sndct);
		printproto(&proto);
		putchar('\n');
		prev = next;
	}
}

routepr()
{
	struct rtentry *rtep;
	register struct rtentry *rt;
	char name[16], *flags;
	struct rtentry *routehash[RTHASHSIZ];
	struct ifnet ifnet;
	register struct arenas *asp;
	int first = 1, i, doinghost = 1;

	printf("\n\t\tRouting Table\n\n");
	if (!vf("_rthost", (char *)routehash, sizeof (routehash))) return;
again:
	for (i = 0; i < RTHASHSIZ; i++) {
		if (!routehash[i]) continue;
		rt = routehash[i];
		while (rt) {
			if (!VALADD(rt, struct rtentry)) {
				printf("bad rtentry address (");
				octout((unsigned)rt);
				puts(")");
				break;
			}
			asp = putars((unsigned)rt,sizeof(struct rtentry),AS_RTENT);
			asp->as_ref++;
			rtep = (struct rtentry *) (arenap + (unsigned)rt - allocs);
			if (first) {
				printf("%11s  %11s  %5s  %6s  %10s   %s\n","dst", "gateway", "flags", "refcnt","use", "interface");
				first = 0;
			}
			printf("%11.11s %11.11s  ",getnet(((struct sockaddr_in *)&rtep->rt_dst)->sin_addr),hoststr(((struct sockaddr_in *)&rtep->rt_gateway)->sin_addr));
			*(flags = name) = '\0';
			if (rtep->rt_flags & RTF_UP)
				*flags++ = 'U';
			if (rtep->rt_flags & RTF_GATEWAY)
				*flags++ = 'G';
			*flags = '\0';
			printf("%4s  %6d  %10ld   ", name,rtep->rt_refcnt, rtep->rt_use);
			if (!rtep->rt_ifp) {
				putchar('\n');
				rt = rtep->rt_next;
				continue;
			}
			/* ifnet & name are not in arena */
			klseek(gcore, rtep->rt_ifp, 0);
			read(gcore, &ifnet, sizeof (ifnet));
			klseek(gcore, (int)ifnet.if_name, 0);
			read(gcore, name, 16);
			printf("%s%d\n", name, ifnet.if_unit);
			rt = rtep->rt_next;
		}
	}
	if (doinghost) {
		if (!vf("_rtnet", (char *)routehash, sizeof (routehash))) return;
		doinghost = 0;
		goto again;
	}
}

mbufpr()
{
	struct mbufxh mbufx;
	register i;
	unsigned base;
	int count;
	int freect;

	line = 22;
	printf("\n\t\t\tMbuf dump\n\nmbcache:");
	for (i=0, count=0; i<NMBCACHE; i++) {
		if ((count++)%4 == 0) {
			putchar('\n');
			line++;
		}
		octout((unsigned)mbcache[i]);
		fputs(VALMBA(mbcache[i]) ? "       " : "(bad)  ",stdout);
	}
	/* print header */
	line += 6;
	puts("\n\n\t\t\tMbufs\n\n click  mbuf   ref  next    off   len    act  click(mbuf)\n\n");

	base = mbbase;
	for(i=0, freect=0; i<NMBUFS ; i++) {
		lseek(gcore, ((long)base<<6), 0);
		read(gcore, &mbufx, sizeof mbufx);
		octout(base);
		putchar(' ');
		octout((unsigned)mbufx.mh_mbuf);
		printf("%5d ",mbufx.mh_ref);
		/* check for validity */
		if (mbufx.mh_ref == 0) {
			/* free */
			freect++;
			if (!mbufx.mh_mbuf || VALMBXA(mbufx.mh_mbuf))
				fputs("---------free",stdout);
			else
				fputs("---------free (bad pointer)",stdout);
		} else if (!VALMBA(mbufx.mh_mbuf)) {
			fputs("---------bad mbuf pointer",stdout);
		} else {
			struct arenas *asp;
			register int j;
			int found = 0;

			asp = putars(mbufx.mh_mbuf,sizeof(struct mbuf),AS_MBUF);
			for (j=0; j<NMBCACHE && j<mbicache; j++) {
				if(mbufx.mh_mbuf == mbcache[j]) {
					found++;
					break;
				}
			}
			if (found) {
				/* in free cache */
				freect++;
				asp->as_ref++;
				fputs("---------in free cache",stdout);
				if (mbufx.mh_ref != 1)
					fputs("--bad ref count",stdout);
			} else {
				struct mbuf *mbptr;

				mbptr = (struct mbuf *)(arenap + (unsigned)mbufx.mh_mbuf - allocs);
				octout(mbptr->m_next);
				printf(" %5d ",mbptr->m_off);
				printf(" %5d ",mbptr->m_len);
				octout(mbptr->m_act);
				if ((mbptr->m_click) != base) {
					putchar(' ');
					octout(mbptr->m_click);
				}
			}
		}
		putchar('\n');
		line++;
		if (line > LINESPERPAGE) {
			newpage();
			puts("\n click  mbuf   ref  next    off   len    act  click(mbuf)\n");
		}
		base += (MSIZE >> 6);
	}
	if (freect != mbstat.m_mbfree)
		printf("\n\tWARNING:  free count discrepancy--\n\n\tmbufx survey gives %d free, mbstat.m_mbfree gives %d\n",freect,mbstat.m_mbfree);
}


/*
 * Dump mbuf information
 * Flag mbufs as free or unaccounted for
 */
orphans()
{
	register i;
	struct arenas *ap;

	puts("\n\t\tMbufs Unaccounted For\n\n\n addr    next    off   len   click   act\n");
	ap = &arenash;
	while (ap = ap->as_next) {
		struct mbuf *mbptr;

		if (ap->as_ref || (!(ap->as_flags & AS_MBUF) && ap->as_size != sizeof(struct mbuf))) continue;
		mbptr = (struct mbuf *)(ap->as_addr - allocs + (unsigned)arenap);
		octout(ap->as_addr);
		putchar(' ');
		octout(mbptr->m_next);
		putchar(' ');
		octout(mbptr->m_off);
		printf(" %5d ",mbptr->m_len);
		octout(mbptr->m_click);
		putchar(' ');
		octout(mbptr->m_act);
		if (!(ap->as_flags & AS_MBUF))
			fputs(" no refs at all",stdout);
		putchar('\n');
	}
	puts("\n\t\tA Trip through the Arena\n");
	ap = &arenash;
	while (ap = ap->as_next) {
		if (ap->as_ref || (ap->as_flags & AS_MBUF) == AS_MBUF) continue;
		printf("addr=%o size=%d flags=%o  ",ap->as_addr, ap->as_size, ap->as_flags);
		switch (ap->as_size) {
			case sizeof(struct protosw):
				puts("protosw");
				break;
			case sizeof(struct socket):
				puts("socket");
				break;
			case sizeof(struct mbuf):
				puts("mbuf");
				break;
			case sizeof(struct sockaddr_in):
				puts("sockaddr_in");
				break;
			case sizeof(struct in_addr):
				puts("in_addr");
				break;
			case sizeof(struct ifnet):
				puts("ifnet");
				break;
			case sizeof(struct inpcb):
				puts("inpcb");
				break;
			case sizeof(struct tcpcb):
				puts("tcpcb");
				break;
			case sizeof(struct rtentry):
				puts("rtentry");
				break;
			default:
				puts("unknown size");
				break;
		}
	}
}

printport(ports, portid)
struct portname *ports;
unsigned portid;
{
	register struct portname *p = ports;

	while (p->port_name) {
		if (p->port_id == portid) {
			printf("%-8s", p->port_name);
			return;
		}
		p++;
	}
	printf("%-8d", portid);
}

printproto(sp)
	register struct protosw *sp;
{
	register struct pfname *pf = pfnames;
	register struct prname *pr;

	while (pf->pf_name) {
		if (pf->pf_id == sp->pr_family) {
			printf("%7s/", pf->pf_name);
			if (pr = pf->pf_protos)
				while (pr->pr_name) {
					if (pr->pr_id == sp->pr_protocol) {
						printf("%-5s", pr->pr_name);
						return;
					}
					pr++;
				}
			printf("%-5d", sp->pr_protocol);
			return;
		}
		pf++;
	}
	printf("%7d/%-5d", sp->pr_family, sp->pr_protocol);
}

printaddress(sa)
register struct sockaddr *sa;
{
	register struct sockname *sn;

	for (sn = addrnames; sn->sa_name; sn++)
		if (sn->sa_id == sa->sa_family) {
			printf("%8s,", sn->sa_name);
			if (!sn->sa_printer) goto generic;
			(*sn->sa_printer)(sa);
			return;
		}
	printf("%8d,", sa->sa_family);
generic:
	printf("%-8s", "?");
}

inetprint(sin)
	register struct sockaddr_in *sin;
{
	internetprint(&sin->sin_addr, sin->sin_port, ipports);
}

pupprint() {
	puts("\nOops! we need pupprint.");
};

#ifdef	pupprint
pupprint(spup)
	register struct sockaddr_pup *spup;
{
	printf("n%-2dh%-2ds%-3d", spup->spup_addr.pp_net,
		spup->spup_addr.pp_host, spup->spup_addr.pp_socket);
}
#endif	pupprint

internetprint(in, p, ports)
struct in_addr *in;
u_short p;
struct portname *ports;
{
	if (!in->s_addr) {
		if (!p) {
			fputs("       */*       ",stdout);
			return;
		}
		fputs("       */",stdout);
	}
	else
		printf("%11.11s/", hoststr(in->s_addr));
	printport(ports, ntohs(p));
}

/*
 * Chase list of mbufs, using m_next and m_act fields.
 */
chasedm(mbufp,str,cc)
register struct mbuf *mbufp;
char *str;
int cc;
{
	register struct arenas *asp;
	int count = 0;
	int chcnt = 0;
	register struct mbuf *mbptr;	/* points into arena */

	while (mbufp) {
		count += chaseit(mbufp,str,&chcnt);
		if (!VALMBA(mbufp)) {
			printf("---------bad mbuf pointer in %s queue (=%o)\n",str,mbufp);
			break;
		}
		if (!(asp=getars(mbufp)) || (asp->as_flags&AS_MBUF) != AS_MBUF) {
			printf("---------mbuf in %s queue not in list (=%o)\n",str,mbufp);
			break;
		}
		mbptr = (struct mbuf *)(arenap + (unsigned)mbufp - allocs);
		mbufp = mbptr->m_act;
	}
	if (cc != chcnt)
		printf("---------char count discrepancy in %s queue; survey gives %d\n",str,chcnt);
	return(count);
}

/*
 * Chase list of mbufs, using only m_next field.
 */
chasem(mbufp,str,cc)
register struct mbuf *mbufp;
char *str;
int cc;
{
	register struct arenas *asp;
	int count = 0;
	int chcnt = 0;
	register struct mbuf *mbptr;	/* points into arena */

	count = chaseit(mbufp,str,&chcnt);
	if (cc != chcnt)
		printf("---------char count discrepancy in %s queue; survey gives %d\n",str,chcnt);
	return(count);
}


chaseit(mbufp,str,achcnt)
register struct mbuf *mbufp;
char *str;
int *achcnt;
{
	register struct arenas *asp;
	int count = 0;
	register struct mbuf *mbptr;	/* points into arena */

	while (mbufp) {
		int err = 0;

		count++;
		if (!VALMBA(mbufp)) {
			printf("---------bad mbuf pointer in %s queue (=%o)\n",str,mbufp);
			break;
		}
		else if (!(asp=getars(mbufp)) || (asp->as_flags&AS_MBUF) != AS_MBUF) {
			printf("---------mbuf in %s queue not in list (=%o)\n",str,mbufp);
			asp = putars(mbufp,sizeof(struct mbuf),AS_MBUF);
			mbptr = (struct mbuf *)(arenap + (unsigned)mbufp - allocs);
			printf("---------off=%d len=%d act=%o click=%o",mbptr->m_off, mbptr->m_len,mbptr->m_act, mbptr->m_click);
			if(!VALMBXA(mbptr->m_click))
				printf("(bad)");
			putchar('\n');
		}
		asp->as_ref++;
		mbptr = (struct mbuf *)(arenap + (unsigned)mbufp - allocs);
		if (mbptr->m_len < 0 || mbptr->m_len > MLEN) {
			printf("---------bad mbuf length in %s queue;\n",str);
			err++;
		}
		if (mbptr->m_off < MMINOFF || mbptr->m_off >MMAXOFF) {
			printf("---------bad mbuf offset in %s queue;\n",str);
			err++;
		}
		if (err)
			printf("---------off=%d len=%d act=%o click=%o\n",mbptr->m_off, mbptr->m_len,mbptr->m_act,mbptr->m_click);
		*achcnt += mbptr->m_len;
		mbufp = mbptr->m_next;
	}
	return(count);
}

clrars()
{
	register struct arenas *asp;
	register struct arenas *next = arenash.as_next;

	while (asp = next) {
		next = asp->as_next;
		free(asp);
	}
	arenash.as_next = (struct arenas *) 0;
}

/*
 * Put entry for arena block in arena statistics list
 * This version allows entries with the same address
 */
struct arenas *putars(addr,size,flags)
register unsigned addr;
int size, flags;
{
	register struct arenas *asp = &arenash;
	register struct arenas *prev;
	unsigned temp;

/* calculate size and get busy flag from info in arena */

	temp = *(int *)(arenap + (unsigned)addr - allocs - 2);
	if (temp&01==0) /* make sure not marked as free */
		flags |= AS_FREE;
	if ((temp&~01) - addr != size)
		flags |= AS_BDSZ;

	while(1) {
		prev = asp;
		asp = asp->as_next;
		if (asp == 0 || addr < asp->as_addr) {
			if (!(prev->as_next = (struct arenas *)malloc(sizeof(struct arenas)))) barf("out of memory");
			prev = prev->as_next;
			prev->as_next = asp;
			prev->as_size = size;
			prev->as_ref = 0;
			prev->as_addr = addr;
			prev->as_flags = flags;
			return(prev);
		}
		if (addr == asp->as_addr && (!(asp->as_flags & ~AS_ARCK)) && asp->as_size == size && !asp->as_ref) {
			asp->as_flags |= flags;
			return(asp);
		}
	}
}

/*
 * Find arena stat structure in list
 * Return zero if not found.
 */
struct arenas *getars(addr)
register unsigned addr;
{
	register struct arenas *asp = &arenash;

	while (asp=asp->as_next) {
		if (asp->as_addr == addr)
			return(asp);
	}
	return((struct arenas *)0);
}

/*
 * Check arena for consistency
 */
arenack()
{
	unsigned ptr = allocs;
	unsigned temp;
	unsigned next;
	int busy;
	int size;
	int fsize = 0;	/* free size, should equal mbstat.clfree */

	fputs("\narena check:",stdout);
	for (;;) {
		temp = *(int *)(ptr - allocs + (unsigned)arenap);
		busy = temp & 01;
		next = temp & ~01;
		if (next < allocs || next > alloct ||
			(ptr > next && (ptr != alloct || next != allocs))){
			fputs("\n\tWARNING:arena clobbered; location: ",stdout);
			octout(ptr);
			fputs(" pointer: ",stdout);
			octout(temp);
			return;
		}
		if (ptr > next) {
			printf("\n\tfree count: %d.\n",fsize);
			return;
		}
		size = next - ptr -2;
		if (!busy)
			fsize += (size+2);
		else
			putars(ptr+2,size,AS_ARCK);
		ptr = next;
	}
}

char *
hoststr(addr)
long addr;
{
	struct hostent *hp;

	hp = gethostbyaddr((char *) &addr, sizeof addr, AF_INET);
	if (hp)
		return(hp->h_name);
	else
		return(inet_ntoa(addr));
}

char *
getnet(addr)
union {
	long a_long;
	char a_octet[4];
} addr;
{
	char buf[32];
	struct netent *np;

	np = getnetbyaddr(inet_netof(addr.a_long), AF_INET);
	if (np)
		return(np->ne_name);
	else {
		sprintf(buf, "%u", (unsigned)addr.a_octet[0]);
		if((unsigned)addr.a_octet[0] >= 128) 
			sprintf(buf+strlen(buf), ".%u", (unsigned)addr.a_octet[1]);
		if((unsigned)addr.a_octet[0] >= 192) 
			sprintf(buf+strlen(buf), ".%u", (unsigned)addr.a_octet[2]);
		return(buf);
	}
}

#ifdef	for_handy_reference
struct tcpcb {
	struct	tcpiphdr *seg_next;	/* sequencing queue */
	struct	tcpiphdr *seg_prev;
[*]	short	t_state;		/* state of this connection */
	short	t_timer[TCPT_NTIMERS];	/* tcp timers */
	short	t_rxtshift;		/* log(2) of rexmt exp. backoff */
[*]	struct	mbuf *t_tcpopt;		/* tcp options */
[*]	struct	mbuf *t_ipopt;		/* ip options */
	short	t_maxseg;		/* maximum segment size */
	char	t_force;		/* 1 if forcing out a byte */
	u_char	t_flags;
#define	TF_ACKNOW	0x01		/* ack peer immediately */
#define	TF_DELACK	0x02		/* ack, but try to delay it */
#define	TF_DONTKEEP	0x04		/* don't use keep-alives */
#define	TF_NOOPT	0x08		/* don't use tcp options */

#ifdef	TCPTRUEOOB
#define	TF_DOOOB	0x10		/* do use out of band data */
#endif	TCPTRUEOOB
	struct	tcpiphdr *t_template;	/* skeletal packet for transmit */
[*]	struct	inpcb *t_inpcb;		/* back pointer to internet pcb */

/*
 * The following fields are used as in the protocol specification.
 * See RFC783, Dec. 1981, page 21.
 */

/* send sequence variables */
	tcp_seq	snd_una;		/* send unacknowledged */
	tcp_seq	snd_nxt;		/* send next */
	tcp_seq	snd_up;			/* send urgent pointer */
	tcp_seq	snd_wl1;		/* window update seg seq number */
	tcp_seq	snd_wl2;		/* window update seg ack number */
	tcp_seq	iss;			/* initial send sequence number */
	u_short	snd_wnd;		/* send window */

/* receive sequence variables */
	short	rcv_wnd;		/* receive window */
	tcp_seq	rcv_nxt;		/* receive next */
	tcp_seq	rcv_up;			/* receive urgent pointer */
	tcp_seq	irs;			/* initial receive sequence number */

/*
 * Additional variables for this implementation.
 */
/* receive variables */
	tcp_seq	rcv_adv;		/* advertised window */
/* retransmit variables */
	tcp_seq	snd_max;		/* highest sequence number sent
					   used to recognize retransmits */
/* transmit timing stuff */
	short	t_idle;			/* inactivity time */
	short	t_rtt;			/* round trip time */
	tcp_seq	t_rtseq;		/* sequence number being timed */
	short   t_srtt;                 /* smoothed round-trip time (*10) */
/* out-of-band data */
	char	t_oobflags;		/* have some */
	char	t_iobc;			/* input character */
#define	TCPOOB_HAVEDATA	0x01

#ifdef	TCPTRUEOOB

#define	TCPOOB_OWEACK	0x02
#define	TCPOOB_NEEDACK	0x04
	u_char	t_iobseq;		/* input receive sequence number */
	tcp_seq	t_oobmark;		/* output mark position */
	char	t_oobc;			/* output character */
	u_char	t_oobseq;		/* output transmit sequence number */

#endif	TCPTRUEOOB

};

#endif	for_handy_reference
