#ifndef lint
static char sccsid[] = "@(#)rstat.c	4.1 82/04/02";
#endif

#include <sys/param.h>
#if !pdp11
#include <sys/pte.h>
#include <nlist.h>
#else
#include <a.out.h>
#define sockaddrname saddrname
#define klseek(fd,base,off) lseek(fd, (long)((unsigned)base), off)
#endif
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/mbuf.h>
#define PRUREQUESTS
#include <sys/protosw.h>
#include <net/in.h>
#define	KERNEL		/* to get routehash and RTHASHSIZ */
#include <net/route.h>
#include <net/in_pcb.h>
#include <net/in_systm.h>
#include <net/if.h>
#include <net/ip.h>
#include <net/ip_var.h>
#include <net/tcp.h>
#define TCPSTATES
#include <net/tcp_fsm.h>
#include <net/tcp_seq.h>
#define	TCPTIMERS
#include <net/tcp_timer.h>
#include <net/tcp_var.h>
#include <net/tcpip.h>
#include <net/udp.h>
#include <net/udp_var.h>
#include <net/raw_cb.h>
#define	TANAMES
#include <net/tcp_debug.h>
#include <net/pup.h>
#include <errno.h>

#include <stdio.h>
#include <ctype.h>

struct nlist nl[] = {
#define	N_MBSTAT	0
	{ "_mbstat" },
#define	N_IPSTAT	1
	{ "_ipstat" },
#define	N_TCB		2
	{ "_tcb" },
#define	N_TCPSTAT	3
	{ "_tcpstat" },
#define	N_UDB		4
	{ "_udb" },
#define	N_UDPSTAT	5
	{ "_udpstat" },
#define	N_RAWCB		6
	{ "_rawcb" },
#define	N_SYSMAP	7
	{ "_Sysmap" },
#define	N_SYSSIZE	8
	{ "_Syssize" },
#define	N_IFNET		9
	{ "_ifnet" },
#define	N_HOSTS		10
	{ "_hosts" },
#define	N_RTHOST	11
	{ "_rthost" },
#define	N_RTNET		12
	{ "_rtnet" },
	0,
};

#define	PRTCP	0
#define	PRUDP	1
#define	PRRAW	2
#define	PRIP	3

#define	PRCOUNT	4
char	wantproto[PRCOUNT];
char	*protonames[PRCOUNT] = {
	"tcp",
	"udp",
	"raw",
	"ip ",
};
int	tcppr(), udppr(), rawpr(), ippr();
int	(*protopr[PRCOUNT])() = {
	tcppr,
	udppr,
	rawpr,
	ippr,
};

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
	/* host specific functions */
	{ IPPORT_EXECSERVER,	"exec" },
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
	/* host specific functions */
	{ IPPORT_BIFFUDP,	"biff" },
	{ IPPORT_WHOSERVER,	"who" },
	{ IPPORT_FINGER,	"finger" },
	{ IPPORT_SYSLOG,	"syslog" },
	0
};

int	inetprint(), pupprint();

struct	sockaddrname {
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

int	mflag;
int	iflag;
int	rflag;
int	interval;
struct	mbstat mbstat;
struct	inpcb inpcb;
struct	tcpcb tcpcb;
struct	rawcb rawcb;
struct  socket sock;
struct	protosw proto;

#if vax
struct	pte *Sysmap;
char	*system = "/vmunix";
#else
char    *system = "/unix";
#endif
char	*kmemf = "/dev/kmem";
int	kmem;
int	kflg;
int	Aflg;
int	hflg;
int	sflg;
int     vflg;
int	qflg;
#define HOST_CACHE_SIZE 20
int host_cache_next = -1;
char  *raddr(), *hoststr(), *getnet();

main(argc, argv)
	int argc;
	char *argv[];
{
	int i;
	char *cp;
	long conptr;

	argc--, argv++;
  	while (argc > 0 && **argv == '-') {
		for (cp = &argv[0][1]; *cp; cp++){
		switch(*cp) {

		case 'm':
			mflag++;
			break;

		case 'p':
			if (argc < 2)
				goto usage;
			if (protox(argv[1]) < 0)
				goto usage;
			wantproto[protox(argv[1])]++;
			argc--, argv++;
			break;

		case 'a':
			{ int i;
			  for (i = 0; i < PRCOUNT; i++) wantproto[i]++;
			}
			break;

		case 'i':
			iflag++;
			break;

		case 'A':
			Aflg++;
			break;

		case 'h':
			hflg++;
			break;

		case 's':
			sflg++;
			break;

		case 'r':
			rflag++;
			break;

		case 'q':
			qflg++;
			break;
		case 'v':
			vflg++; mflag++; iflag++; Aflg++; hflg++; sflg++; rflag++;
			break;

		default:
usage:
	printf("usage: rstat [ -Amisrqv ] [ -p protocol | -a ] [ interval ] [ system ] [ core ]\n");
			exit(1);
		}			/* end switch */
		}			/* end for */
		argv++, argc--;
	}
	if (argc > 0 && isdigit(argv[0][0])) {
		interval = atoi(argv[0]);
		if (interval <= 0)
			goto usage;
		argv++, argc--;
	}
	if (argc > 0) {
		system = *argv;
		argv++, argc--;
	}
	nlist(system, nl);
	if (nl[0].n_type == 0) {
		fprintf(stderr, "%s: no namelist\n", system);
		exit(1);
	}
	if (argc > 0) {
		kmemf = *argv;
		kflg++;
	}
	kmem = open(kmemf, 0);
	if (kmem < 0) {
		fprintf(stderr, "cannot open ");
		perror(kmemf);
		exit(1);
	}
#if vax
	if (kflg) {
		nl[N_SYSSIZE].n_value *= 4;
		klseek(kmem, (off_t)nl[N_SYSMAP].n_value, 0);
		Sysmap = (struct pte *)malloc(nl[N_SYSSIZE].n_value);
		read(kmem, Sysmap, nl[N_SYSSIZE].n_value);
	}
#endif
	/*
	 * Set up default -- show all protocol connections
	 */
	if (vflg || (!mflag && !iflag && !hflg && !rflag)) {
		for (i = 0; i < PRCOUNT; i++)
			if (wantproto[i])
				break;
		if (i >= PRCOUNT)
			for (i = 0; i < PRCOUNT; i++)
				wantproto[i]++;
	}
again:
	if (mflag)
		mbpr();
	defpr();
	if (iflag)
		intpr();
	if (hflg)
		hostpr();
	if (rflag)
		routepr();
	if (interval) {
		sleep(interval);
		goto again;
	}
	exit(0);
}

mbpr()
{
	if (nl[N_MBSTAT].n_value == 0) {
		printf("mbstat: symbol not in namelist\n");
		return;
	}
	printf("mbufs:");
	klseek(kmem, nl[N_MBSTAT].n_value, 0);
	if (read(kmem, &mbstat, sizeof (mbstat)) == sizeof (mbstat))
		printf(" mbufs %d mbfree %d clusters %d clfree %d drops %d\n",
			mbstat.m_mbufs, mbstat.m_mbfree,
			mbstat.m_clusters, mbstat.m_clfree, mbstat.m_drops);
		/* printf(" bufs %d hiwat %d lowat %d clusters %d drops %d\n",
			mbstat.m_bufs, mbstat.m_hiwat, mbstat.m_lowat,
			mbstat.m_clusters, mbstat.m_drops);     */
}

defpr()
{
	int i;
	int first = 1;

	for (i = 0; i < PRCOUNT; i++)
		if (wantproto[i]) {
			if ((!sflg || vflg) && first) {
				banner();
				first = 0;
			}
			(*protopr[i])();
		}
}

banner()
{
	if (Aflg) {
		printf("address");
#if !pdp11
		printf("  ");
#endif
	}
	printf("      local             foreign            rcc  scc  proto  state\n");
}
tcppr()
{
	struct inpcb tcb;
	struct tcpstat tcpstat;
	struct inpcb *prev, *next;

	if (nl[N_TCB].n_value == 0) {
		printf("tcb: symbol not in namelist\n");
		return;
	}
	if (sflg) {
		printf("tcp:");
		klseek(kmem, nl[N_TCPSTAT].n_value, 0);
		read(kmem, &tcpstat, sizeof (tcpstat));
		printf(" badsum %d badoff %d hdrops %d badsegs %d unack %d\n",
		   tcpstat.tcps_badsum, tcpstat.tcps_badoff,
		   tcpstat.tcps_hdrops, tcpstat.tcps_badsegs,
		   tcpstat.tcps_unack);
		if (!vflg)
			return;
	}
	klseek(kmem, nl[N_TCB].n_value, 0);
	read(kmem, &tcb, sizeof (tcb));
	inpcb = tcb;
	prev = (struct inpcb *)nl[N_TCB].n_value;
	while (inpcb.inp_next != (struct inpcb *)nl[N_TCB].n_value) {
		char *cp;
		next = inpcb.inp_next;
		klseek(kmem, (off_t)next, 0);
		read(kmem, &inpcb, sizeof (inpcb));
		if (inpcb.inp_prev != prev) {
			printf("???\n");
			break;
		}
		klseek(kmem, (off_t)inpcb.inp_socket, 0);
		read(kmem, &sock, sizeof (struct socket));
		klseek(kmem, (off_t)inpcb.inp_ppcb, 0);
		read(kmem, &tcpcb, sizeof (tcpcb));
		if (Aflg)
#if !pdp11
			printf("%8x ", inpcb.inp_ppcb);
#else
			printf("%6o ", inpcb.inp_ppcb);
#endif
		internetprint(&inpcb.inp_laddr, inpcb.inp_lport, ipports);
		internetprint(&inpcb.inp_faddr, inpcb.inp_fport, ipports);
		printf("%5d%5d    tcp   ",
		    sock.so_rcv.sb_cc, sock.so_snd.sb_cc);
		if (tcpcb.t_state < 0 || tcpcb.t_state >= TCP_NSTATES)
			printf("%d\n", tcpcb.t_state);
		else
			printf("%s\n", tcpstates[tcpcb.t_state]);
		prev = next;
	}
}

udppr()
{
	struct inpcb udb;
	struct udpstat udpstat;
	struct inpcb *prev, *next;

	if (nl[N_UDB].n_value == 0) {
		printf("udb: symbol not in namelist\n");
		return;
	}
	if (sflg) {
		printf("udp:");
		klseek(kmem, nl[N_UDPSTAT].n_value, 0);
		read(kmem, &udpstat, sizeof (udpstat));
		printf(" hdrops %d badsum %d badlen %d\n",
		   udpstat.udps_hdrops, udpstat.udps_badsum,
		   udpstat.udps_badlen);
		if (!vflg)
			return;
	}
	klseek(kmem, nl[N_UDB].n_value, 0);
	read(kmem, &udb, sizeof (udb));
	inpcb = udb;
	prev = (struct inpcb *)nl[N_UDB].n_value;
	while (inpcb.inp_next != (struct inpcb *)nl[N_UDB].n_value) {
		next = inpcb.inp_next;
		klseek(kmem, (off_t)next, 0);
		read(kmem, &inpcb, sizeof (inpcb));
		if (inpcb.inp_prev != prev) {
			printf("???\n");
			break;
		}
		if (Aflg)
#if !pdp11
			printf("%8x ", next);
#else
			printf("%6o ", next);
#endif
		klseek(kmem, (off_t)inpcb.inp_socket, 0);
		read(kmem, &sock, sizeof (struct socket));
		internetprint(&inpcb.inp_laddr, inpcb.inp_lport, udpports);
		internetprint(&inpcb.inp_faddr, inpcb.inp_fport, udpports);
		printf(" %4d %4d    udp\n",
		    sock.so_rcv.sb_cc, sock.so_snd.sb_cc);
		prev = next;
	}
}

rawpr()
{
	struct rawcb *prev, *next, pcb;

	if (nl[N_RAWCB].n_value == 0) {
		printf("rawcb: symbol not in namelist\n");
		return;
	}
	if (sflg && !vflg)
		return;
	klseek(kmem, nl[N_RAWCB].n_value, 0);
	read(kmem, &rawcb, sizeof (rawcb));
	pcb = rawcb;
	prev = (struct rawcb *)nl[N_RAWCB].n_value;
	while (pcb.rcb_next != (struct rawcb *)nl[N_RAWCB].n_value) {
		next = pcb.rcb_next;
		klseek(kmem, (off_t)next, 0);
		read(kmem, &pcb, sizeof (pcb));
		if (pcb.rcb_prev != prev) {
			printf("???\n");
			break;
		}
		if (Aflg)
#if !pdp11
			printf("%8x ", next);
#else
			printf("%6o ", next);
#endif
		klseek(kmem, (off_t)pcb.rcb_socket, 0);
		read(kmem, &sock, sizeof (struct socket));
		klseek(kmem, (off_t)sock.so_proto, 0);
		read(kmem, &proto, sizeof (proto));
		printaddress(&pcb.rcb_laddr);
		printaddress(&pcb.rcb_faddr);
		printf(" %4d %4d    ", sock.so_rcv.sb_cc,
			sock.so_snd.sb_cc);
		printproto(&proto);
		putchar('\n');
		prev = next;
	}
}

ippr()
{
	struct ipstat ipstat;

	if (nl[N_IPSTAT].n_value == 0) {
		printf("ipstat: symbol not in namelist\n");
		return;
	}
	if (!sflg)
		return;
	printf("ip:");
	klseek(kmem, nl[N_IPSTAT].n_value, 0);
	read(kmem, &ipstat, sizeof (ipstat));
	printf(" badlen %d badsum %d tooshort %d\n", ipstat.ips_badlen,
		ipstat.ips_badsum, ipstat.ips_tooshort);
}

#if !pdp11
hostpr()
{
	struct mbuf *hosts, mb;
	register struct mbuf *m;
	register struct hmbuf *mh;
	register struct host *hp;
	int first = 1;

	if (nl[N_HOSTS].n_value == 0) {
		printf("hosts: symbol not in namelist\n");
		return;
	}
	klseek(kmem, nl[N_HOSTS].n_value, 0);
	read(kmem, &hosts, sizeof (hosts));
	m = hosts;
	while (m) {
		klseek(kmem, m, 0);
		read(kmem, &mb, sizeof (mb));
		m = &mb;
		mh = mtod(m, struct hmbuf *);
		if (mh->hm_count == 0) {
			m = m->m_next;
			continue;
		}
		for (hp = mh->hm_hosts; hp < mh->hm_hosts + HPMBUF; hp++) {
			if (hp->h_flags == 0 && hp->h_q == 0 &&
			    hp->h_qcnt == 0 && hp->h_rfnm == 0)
				continue;
			if (first) {
				printf("use\t address\tqcnt\t\tq\trnfm\n");
				first = 0;
			}
			printf("%c\t%8x\t %3d\t%8x\t %3d\n", hp->h_flags ?
				'*' : '.', hp->h_addr.s_addr,
				hp->h_qcnt, hp->h_q, hp->h_rfnm);
		}
		m = m->m_next;
	}
}
#else
hostpr() {}
#endif

routepr()
{
	struct rtentry rte;
	register struct rtentry *rt;
	char name[16], *flags;
	struct rtentry *routehash[RTHASHSIZ];
	struct ifnet ifnet;
	int first = 1, i, doinghost = 1;

	if (nl[N_RTHOST].n_value == 0) {
		printf("rthost: symbol not in namelist\n");
		return;
	}
	if (nl[N_RTNET].n_value == 0) {
		printf("rtnet: symbol not in namelist\n");
		return;
	}
	klseek(kmem, nl[N_RTHOST].n_value, 0);
	read(kmem, routehash, sizeof (routehash));
again:
	for (i = 0; i < RTHASHSIZ; i++) {
		if (routehash[i] == 0)
			continue;
		rt = routehash[i];
		while (rt) {
			klseek(kmem, rt, 0);
			read(kmem, &rte, sizeof (rte));
			if (first) {
				printf("%11s %11s  %5s  %6s  %10s   %s\n",
					"dst", "gateway", "flags", "refcnt",
					"use", "interface");
				first = 0;
			}
			rt = &rte;
			printf("%11.11s %11.11s   ",
			  getnet(((struct sockaddr_in *)&rt->rt_dst)->sin_addr),
			  hoststr(((struct sockaddr_in *)&rt->rt_gateway)->sin_addr));
			*(flags = name) = '\0';
			if (rt->rt_flags & RTF_UP)
				*flags++ = 'U';
			if (rt->rt_flags & RTF_GATEWAY)
				*flags++ = 'G';
			*flags = '\0';
			printf("%4s  %6d  %10ld   ", name,
				rt->rt_refcnt, rt->rt_use);
			if (rt->rt_ifp == 0) {
				putchar('\n');
				rt = rt->rt_next;
				continue;
			}
			klseek(kmem, rt->rt_ifp, 0);
			read(kmem, &ifnet, sizeof (ifnet));
			klseek(kmem, (int)ifnet.if_name, 0);
			read(kmem, name, 16);
			printf("%s%d\n", name, ifnet.if_unit);
			rt = rt->rt_next;
		}
	}
	if (doinghost) {
		klseek(kmem, nl[N_RTNET].n_value, 0);
		read(kmem, routehash, sizeof (routehash));
		doinghost = 0;
		goto again;
	}
}

printport(ports, portid)
	struct portname *ports;
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
	register struct sockaddrname *sn;

	for (sn = addrnames; sn->sa_name; sn++)
		if (sn->sa_id == sa->sa_family) {
			printf("%8s,", sn->sa_name);
			if (sn->sa_printer == 0)
				goto generic;
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

pupprint(spup)
	register struct sockaddr_pup *spup;
{
	printf("n%-2dh%-2ds%-3d", spup->spup_addr.pp_net,
		spup->spup_addr.pp_host, spup->spup_addr.pp_socket);
}

internetprint(in, p, ports)
	struct in_addr *in;
	struct portname *ports;
{
	if (in->s_addr == 0) {
		if (p == 0) {
			printf("          */*       ");
			return;
		}
		printf("          */");
	} else 	printf("%11.11s/", hoststr(in->s_addr));
	printport(ports, ntohs(p));
}

protox(cp)
	char *cp;
{
	int i;

	for (i = 0; i < PRCOUNT; i++)
		if (!strcmp(protonames[i], cp))
			return (i);
	return (-1);
}

intpr()
{
	int ifoff;
	struct ifnet ifnet;
	char name[16];

	ifoff = nl[N_IFNET].n_value;
	klseek(kmem, ifoff, 0);
	read(kmem, &ifoff, sizeof ifoff);
	printf("Name\t  mtu  net       addr        ipkts ierrs   opkts oerrs  collis drops\n");
	while (ifoff) {
		struct sockaddr_in *sin;
		struct in_addr *in;

		klseek(kmem, ifoff, 0);
		read(kmem, &ifnet, sizeof ifnet);
		klseek(kmem, (int)ifnet.if_name, 0);
		read(kmem, name, 16);
		sin = (struct sockaddr_in *)&ifnet.if_addr;
		in = (struct in_addr *)&ifnet.if_net;
		printf("%s%d%c:\t%5d  %-7.7s %11.11s %7D %5D %7D %5D  %6D %5d\n",
		    name, ifnet.if_unit, ifnet.if_flags & IFF_UP ? '\0' : '*',
		    ifnet.if_mtu, getnet(in->s_addr),
		    hoststr(sin->sin_addr.s_addr), ifnet.if_ipackets,
		    ifnet.if_ierrors, ifnet.if_opackets,
		    ifnet.if_oerrors, ifnet.if_collisions,
		    ifnet.if_snd.ifq_drops);
		ifoff = (off_t) ifnet.if_next;
	}
}

#if vax
klseek(fd, base, off)
	int fd, base, off;
{

	if (kflg) {
		/* get kernel pte */
		base &= 0x7fffffff;
		base = Sysmap[base >> 9].pg_pfnum * 512 + (base & 0x1ff);
	}
	lseek(fd, base, off);
}
#endif

char *
hoststr(addr)
union {
	long a_long;
	char a_octet[4];
} addr;
{	static struct {
		char c_buf[100];
		long c_host;
	} hcache[HOST_CACHE_SIZE];
	register char *bp;
	register int i;

	for(i = 0; i < HOST_CACHE_SIZE; i++)
		if (hcache[i].c_host == addr.a_long) return(hcache[i].c_buf);
	host_cache_next = (host_cache_next+1) % HOST_CACHE_SIZE;
	hcache[host_cache_next].c_host = addr.a_long;
	if(qflg)	/* don't want to spend time scanning host table */
		bp = 0;
	else
		bp = raddr(addr.a_long);
	if (bp && *bp)
		strcpy(hcache[host_cache_next].c_buf, bp);
	else {
/*	    addr.a_long = ntohl(addr.a_long); */
	    sprintf(hcache[host_cache_next].c_buf, "%u.%u.%u.%u",
		(unsigned)addr.a_octet[0], (unsigned)addr.a_octet[1],
		(unsigned)addr.a_octet[2], (unsigned)addr.a_octet[3]);
	}
	return(hcache[host_cache_next].c_buf);
}

#include <netdb.h>

char *raddr(addr)
	union {
		long a_long;
		char a_octet[4];
	} addr;
{
	struct hostent *ptr;
	struct netent *nptr;

	ptr = gethostbyaddr(addr.a_octet, sizeof addr.a_long, AF_INET);
	if (ptr != (struct hostent *) NULL)
		return(ptr->h_name);
	else {
		nptr = getnetbyaddr(inet_netof(addr.a_long), AF_INET);
		if (nptr != (struct netent *) NULL)
			return(nptr->ne_name);
		else
			return((char *) NULL);
	}
}

long htonl();

char *
getnet(addr)
union {
	long a_long;
	char a_octet[4];
} addr;
{	static char buf[100];
	struct netent *ptr = (struct netent *)NULL;

	if (!qflg)
		ptr = getnetbyaddr(inet_netof(addr.a_long), AF_INET);
	if (ptr != (struct netent *) NULL)
		return(ptr->ne_name);
	else {
		sprintf(buf, "%u", (unsigned)addr.a_octet[0]);
		if((unsigned)addr.a_octet[0] >= 128) 
		   sprintf(buf+strlen(buf), ".%u", (unsigned)addr.a_octet[1]);
		if((unsigned)addr.a_octet[0] >= 192) 
		   sprintf(buf+strlen(buf), ".%u", (unsigned)addr.a_octet[2]);
		return(buf);
	}
}
