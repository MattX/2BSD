/*      if_qn.c 4.11    84/10/05        */

#include "qn.h"

#if	NQN > 0
/*
 * DEQNA Ethernet Communications Controller interface
 * Includes scatter/gather io to/from mbufs.
 */
#include "param.h"
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/buf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/ubavar.h>
#include <sys/qnreg.h>
#include <sys/ioctl.h>
#include "../net/in.h"
#include "../net/in_systm.h"
#include "../net/if.h"
#include "../net/if_ether.h"
#include "../net/if_uba.h"
#include "../net/ip.h"
#include "../net/ip_var.h"
#include "../net/pup.h"
#include "../net/route.h"
#include <errno.h>

#define	QNMTU	1500		/* Maximum data size for Ethernet packet */
#define	QNMIN	(60-14)		/* Minimum data size for Ethernet packet */
#define	QNMRU	1582		/* Maximum packet received before trunc. */
#define	QNVEC	0340		/* Interrupt vector */

#define	RCV	01		/* Receive flag */
#define	XMT	02		/* Transmit flag */
#define	FLAG	04		/* Flag for ->flag field */
#define	STAT	010		/* Flag for ->stat field */

int	qnprobe(), qnattach(), qnintr();
u_long	qnaddbdl();
u_short *qnxtbdl();
struct	uba_device *qninfo[NQN];
u_short qnstd[] = { 0 };
struct	uba_driver qndriver =
	{ qnprobe, 0, qnattach, 0, qnstd, "qn", qninfo };
#define	QNUNIT(x)	minor(x)
int	qninit(),qnoutput(),qnreset();

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * qs_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 */
struct	ifscat {
	u_long s_info;
	struct mbuf *s_m;
};

struct	qn_softc {
	struct	arpcom qs_ac;		/* Ethernet common part */
#define	qs_if	qs_ac.ac_if		/* network-vqsible interface */
#define	qs_addr	qs_ac.ac_enaddr		/* hardware Ethernet address */
	struct	ifscat qs_ruba[RCVSIZ];
	struct	ifscat qs_xuba[XMTSIZ];
	int	qs_flags;
	long    qs_ubaddr;              /* mapping registers of qn_cbuf */
	int	qs_rsp;			/* Start pnt. in rcv bdl cyclic q */
	int	qs_xsp;			/* Start pnt. of xmit bdl q */
	int	qs_xep;			/* End pnt. of xmit bdl q */
} qn_softc[NQN];

struct qn_bdl {
	u_short b_flag;
	u_short b_haddr;
	u_short b_laddr;
	u_short b_len;
	u_short b_stat1;
	u_short b_stat2;
};

struct qn_cbuf {
	u_char qn_pea[128];
	struct qn_bdl qn_xbdl[XMTSIZ+1];
	struct qn_bdl qn_rbdl[RCVSIZ+1];
} qn_cbuf[NQN];

qnprobe(reg)
	caddr_t reg;
{
#if	!pdp11
	register int br, cvec;		/* r11, r10 value-result */

#ifdef lint
	br = 0; cvec = br; br = cvec;
	qnrint(0); qncint(0); qnwatch(0);
#endif

	br = 0x15;
	cvec = QNVEC;
	return (1);
#endif
}

/*
 * Interface exists: make available by filling in network interface
 * record.  System will initialize the interface when it is ready
 * to accept packets. 
 */
qnattach(ui)
	struct uba_device *ui;
{
	register u_short *ptr;
	register struct qn_softc *qs = &qn_softc[ui->ui_unit];
	register struct ifnet *ifp = &qs->qs_if;
	register struct qndevice *addr = (struct qndevice *)ui->ui_addr;
	struct qn_cbuf *qp = &qn_cbuf[ui->ui_unit];
	struct sockaddr_in *sin;
	int i;
	u_long taddr;

	ifp->if_unit = ui->ui_unit;
	ifp->if_name = "qn";
	ifp->if_mtu = QNMTU;

	/*
	 * Set the default physical address.
	 */
	qs->qs_addr[0] = (addr->ad0 & 0377);
	qs->qs_addr[1] = (addr->ad1 & 0377);
	qs->qs_addr[2] = (addr->rbl & 0377);
	qs->qs_addr[3] = (addr->rbh & 0377);
	qs->qs_addr[4] = (addr->xbl & 0377);
	qs->qs_addr[5] = (addr->xbh & 0377);
	/* Set up initialization packet */
	for (i = 0; i < 128; i++)
		if ((i & 07) == 0 || (i & 060) == 060)
			qp->qn_pea[i] = 0;
		else
			qp->qn_pea[i] = qs->qs_addr[(i>>3)&07];
	qp->qn_pea[2] = qp->qn_pea[10] = qp->qn_pea[18] = qp->qn_pea[26]
		= qp->qn_pea[34] = qp->qn_pea[42] = 0xff;
	addr->csr = SR;
printf("init csr=%o\n",addr->csr);
	addr->csr = 0;
	addr->vec = QNVEC;
	qs->qs_ubaddr = uballoc(ui->ui_ubanum, (caddr_t)qp,
		sizeof(struct qn_cbuf), 0);
for(i = 1; i < 4; i++) {
	ptr = (u_short *)qp->qn_rbdl;
	*ptr++ = BINIT;
/*
	*ptr++ = VALID|((qs->qs_ubaddr>>16)&077);
	*ptr++ = qs->qs_ubaddr & 0xffff;
*/
	*ptr++ = VALID|010;
	*ptr++ = 0;
	*ptr++ = -(256>>1);			/* 2's comp. of struct size */
	*ptr++ = BINIT;
	*ptr-- = 01;
	ptr = (u_short *)qp->qn_xbdl;
	*ptr++ = BINIT;
	*ptr++ = VALID|EOM|SETUP|((qs->qs_ubaddr>>16)&077);
	*ptr++ = qs->qs_ubaddr & 0xffff;
	*ptr++ = (i<<1)|0200;
	*ptr++ = BINIT;
	*ptr-- = 0;
printf("pea %o %o %o %o %o %o %o %o %o %o\n",qp->qn_pea[0]&0377,qp->qn_pea[1]&0377,qp->qn_pea[2]&0377,qp->qn_pea[3]&0377,qp->qn_pea[4]&0377,qp->qn_pea[5]&0377,
qp->qn_pea[6]&0377,qp->qn_pea[7]&0377,qp->qn_pea[8]&0377,qp->qn_pea[9]&0377);
	taddr = qnaddbdl(ui->ui_unit, 0, RCV);
	addr->rbl = loint(taddr);
	addr->rbh = hiint(taddr);
	taddr = qnaddbdl(ui->ui_unit, 0, XMT);
	addr->xbl = loint(taddr);
	addr->xbh = hiint(taddr);
printf("at while\n");
	while (qp->qn_rbdl[0].b_stat1 & LASTNOT)
printf("st1=%o cs=%o\n",qp->qn_rbdl[0].b_stat1, addr->csr);
printf("st1=%o cs=%o\n",qp->qn_rbdl[0].b_stat1, addr->csr);
printf("rst s1=%o rbl=%d\n",qp->qn_rbdl[0].b_stat1,qp->qn_rbdl[0].b_stat2&0377);
printf("pea %o %o %o %o %o %o %o %o %o %o\n",qp->qn_pea[0]&0377,qp->qn_pea[1]&0377,qp->qn_pea[2]&0377,qp->qn_pea[3]&0377,qp->qn_pea[4]&0377,qp->qn_pea[5]&0377,
qp->qn_pea[6]&0377,qp->qn_pea[7]&0377,qp->qn_pea[8]&0377,qp->qn_pea[9]&0377);
printf("pea %o %o %o %o %o %o %o %o %o %o\n",qp->qn_pea[10]&0377,qp->qn_pea[11]&0377,qp->qn_pea[12]&0377,qp->qn_pea[13]&0377,qp->qn_pea[14]&0377,qp->qn_pea[15]&0377,
qp->qn_pea[16]&0377,qp->qn_pea[17]&0377,qp->qn_pea[18]&0377,qp->qn_pea[19]&0377);
}
	printf("qn%d: addr=%d:%d:%d:%d:%d:%d\n", ui->ui_unit,
		qs->qs_addr[0]&0377, qs->qs_addr[1]&0377, qs->qs_addr[2]&0377,
		qs->qs_addr[3]&0377, qs->qs_addr[4]&0377, qs->qs_addr[5]&0377);
	sin = (struct sockaddr_in *)&ifp->if_addr;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(ui->ui_flags);
	ifp->if_net = in_netof(sin->sin_addr);
	ifp->if_host[0] = in_lnaof(sin->sin_addr);
	sin = (struct sockaddr_in *)&ifp->if_broadaddr;
	sin->sin_family = AF_INET;
	sin->sin_addr = if_makeaddr(ifp->if_net, INADDR_ANY);
	ifp->if_flags |= IFF_BROADCAST;
	ifp->if_init = qninit;
	ifp->if_output = qnoutput;
	ifp->if_ubareset = qnreset;
	if_attach(ifp);
printf("qnattend\n");
}

/*
 * Reset of interface after UNIBUS reset.
 * If interface qs on specified uba, reset its state.
 */
qnreset(unit, uban)
	int unit, uban;
{
#if	!pdp11
	register struct uba_device *ui;

	if (unit >= NQN || (ui = qninfo[unit]) == 0 || ui->ui_alive == 0 ||
	    ui->ui_ubanum != uban)
		return;
	printf(" qn%d", unit);
	qninit(unit);
#endif
}

/*
 * Initialization of interface; clear recorded pending
 * operations, and reinitialize UNIBUS usage.
 */
qninit(unit)
	int unit;
{
	register struct qn_softc *qs = &qn_softc[unit];
	struct uba_device *ui = qninfo[unit];
	struct qndevice *addr;
	struct ifnet *ifp = &qs->qs_if;
	struct sockaddr_in *sin;
	register u_short *ptr = qn_cbuf[unit].qn_rbdl;
	int i, nlen;
	int s;
	register struct mbuf *m;
	u_long taddr;
	u_short ord;

	sin = (struct sockaddr_in *)&ifp->if_addr;
	if (sin->sin_addr.s_addr == 0) /* if address stqnl unknown */
		return;

	if (ifp->if_flags & IFF_RUNNING)
		goto justarp;
	/* Get mbufs and stuff the rbld's to start reception */
	qs->qs_rsp = qs->qs_xsp = 0;
	qs->qs_xep = -1;
	i = 0;
	do {
		if (i == (RCVSIZ-1))
			m = 0;
		else
			MGET(m, 0);
		if (m == 0) {
			taddr = qnaddbdl(unit, i+1, RCV);
			ord = VALID|CHAIN;
			nlen = 0;
		} else {
			m->m_len = MLEN;
			m->m_off = MMINOFF;
			qs->qs_ruba[i].s_info = taddr = ubmalloc(0, m->m_click,
				m->m_len, 0);
			taddr = (taddr & 0x3fffff)+m->m_off;
			qs->qs_ruba[i].s_m = m;
			nlen = -(m->m_len>>1);
			ord = VALID;
		}
		*ptr++ = BINIT;
		*ptr++ = ((taddr>>16)&0x3f)|ord;
		*ptr++ = taddr & 0xffff;
		*ptr++ = nlen;
		*ptr++ = BINIT;
		*ptr = 01;
		ptr = qnxtbdl(unit, &i, RCV|FLAG);
	} while (i != qs->qs_rsp);
	addr = (struct qndevice *)ui->ui_addr;

	taddr = qnaddbdl(unit, 0, RCV);
	s = splimp();
	addr->csr = RI|XI|IE|RE;
	addr->rbl = loint(taddr);
	addr->rbh = hiint(taddr);
	qs->qs_if.if_flags |= IFF_UP|IFF_RUNNING;
	if (qs->qs_if.if_snd.ifq_head)
		qnstart(unit);
	splx(s);
justarp:
	if_rtinit(&qs->qs_if, RTF_UP);
	arpattach(&qs->qs_ac);
	arpwhohas(&qs->qs_ac, &sin->sin_addr);
}

/*
 * Start output on interface.
 * Get another datagram to send off of the interface queue,
 * and map it to the interface before starting the output.
 */
qnstart(dev)
	dev_t dev;
{
        int unit = QNUNIT(dev), len;
	struct uba_device *ui = qninfo[unit];
	register struct qn_softc *qs = &qn_softc[unit];
	struct qndevice *addr = (struct qndevice *)ui->ui_addr;
	struct ifqueue *ifq;
	register u_short *ptr;
	register struct mbuf *m;
	int pos, cnt, tlen, olen;
	u_short ord;
	u_long taddr;

	pos = qs->qs_xsp;
	ptr = &qn_cbuf[unit].qn_xbdl[pos].b_flag;
	ifq = &qs->qs_if.if_snd;
	do {
		m = ifq->ifq_head;
		tlen = 0;
		cnt = 0;
		while (m) {
			cnt++;
			tlen += m->m_len;
			m = m->m_next;
		}
		if((cnt < ((XMTSIZ+qs->qs_xep-pos)%XMTSIZ) || qs->qs_xep == -1)
			&& cnt > 0) {
			IF_DEQUEUE(ifq, m);
			while (m) {
				len = m->m_len;
				qs->qs_xuba[pos].s_info = taddr = ubmalloc(0,
					m->m_click, len, 0);
				qs->qs_xuba[pos].s_m = m;
				taddr = (taddr & 0x3fffff) + m->m_off;
				if (pos != qs->qs_xsp)
					*ptr++ = BINIT;
				else
					*ptr++ = 0;
				ord = VALID;
				if (m->m_next == NULL) {
					ord |= EOM;
					if (tlen < (sizeof(struct ether_header)+
						QNMIN))
						len += QNMIN+sizeof(struct 
						   ether_header)-tlen;
				}
				olen = len;
				if (taddr & 01) {
					ord |= HIGHS;
					olen++;
				}
				if ((taddr+len) & 01) {
					ord |= LOWT;
					olen++;
				}
				*ptr++ = ((taddr>>16)&0x3f)|ord;
				*ptr++ = taddr & 0xffff;
				*ptr++ = -(olen>>1);
				*ptr++ = BINIT;
				*ptr = 0;
				ptr = qnxtbdl(unit, &pos, XMT|FLAG);
				m = m->m_next;
			}
		} else {
			if (pos == qs->qs_xsp)
				return;
			break;
		}
	} while (pos != qs->qs_xep);
	qn_cbuf[unit].qn_xbdl[qs->qs_xsp].b_flag = BINIT;
	if (addr->csr & XL) {
		taddr = qnaddbdl(unit, qs->qs_xsp, XMT);
		addr->csr = XI|IE|RE;
		addr->xbl = loint(taddr);
		addr->xbh = hiint(taddr);
	}
	qs->qs_xsp = pos;
	if (qs->qs_xep == -1 && pos > 0)
		qs->qs_xep = 0;
}

/*
 * Transmit done interrupt.
 */
qnxint(unit)
	int unit;
{
	register struct qn_softc *qs = &qn_softc[unit];
	register struct qndevice *addr=(struct qndevice *)qninfo[unit]->ui_addr;
	register u_short *ptr;
	int pos;
	struct ifnet *ifp = &qs->qs_if;
	mapinfo map;

	Savemap(map);
printf("xint csr=%o ",addr->csr);
	pos = qs->qs_xep;
	ptr = &qn_cbuf[unit].qn_xbdl[pos].b_stat1;
	do {
		if ((*ptr & (LASTNOT|UERR)) == LASTNOT)
			break;
printf("s1=%o\n",*ptr);
		if ((*ptr & LASTNOT) == 0) {
			if (*ptr & ABORT)
				ifp->if_collisions += 16;
			else
				ifp->if_collisions += ((*ptr>>4)&0xf);
			if (*ptr & UERR) {
				ifp->if_oerrors++;
				while (*(ptr+1) == 0)
					;
				printf("qn%d:err csr=%o stat=%o tdr=%d\n",
					unit, addr->csr, *ptr, (*(ptr+1))-5);
			}
			ifp->if_opackets++;
		}
		m_freem(qs->qs_xuba[pos].s_m);
		ptr = qnxtbdl(unit, &pos, XMT|STAT);
		qs->qs_xep = pos;
	} while (pos != qs->qs_xsp);
	if (pos == qs->qs_xsp) {
		qs->qs_xsp = 0;
		qs->qs_xep = -1;
	}
	qnstart(unit);
out:
	Restormap(map);
}

/*
 * Ethernet interface receiver interrupt.
 * If input error just drop packet.
 * Otherwise purge input buffered data path and examine 
 * packet to determine type.  If can't determine length
 * from type, then have to drop packet.  Othewise decapsulate
 * packet based on type and pass to type specific higher-level
 * input routine.
 */
qnrint(unit)
	int unit;
{
	register struct qn_softc *qs = &qn_softc[unit];
	struct qndevice *addr = (struct qndevice *)qninfo[unit]->ui_addr;
	struct ether_header *qn;
	struct ifqueue *inq;
	register u_short *ptr, *ptr2;
	int pos;
    	struct mbuf *m, *top, *top2, *m2, **mp;
	int len, off, resid, nlen, tlen, first, pos2, badflg, i, diff;
	u_short stat1, stat2, ord, type;
	u_long taddr;
	mapinfo map;

	Savemap(map);
printf("rint ");
	mp = &top;
	top = 0;
	first = 1;
	pos = qs->qs_rsp;
	ptr = &qn_cbuf[unit].qn_rbdl[pos].b_stat1;
	/* loop thru all used bdl's */
	do {
		/* not used yet, get out */
		if ((*ptr & (LASTNOT|UERR)) == LASTNOT)
			goto out;
		m = qs->qs_ruba[pos].s_m;
		if (first) {
			stat1 = LASTNOT;
			pos2 = pos;
			ptr2 = ptr;
			/* search ahead to find completion status */
			do {
				if ((*ptr2 & LASTNOT) == 0) {
					qs->qs_if.if_ipackets++;
					stat1 = *ptr2;
					do {
						stat2 = *(ptr2+1);
					} while ((stat2&0377)!=((stat2>>8)&0377));
					break;
				}
				ptr2 = qnxtbdl(unit, &pos2, RCV|STAT);
			} while (pos2 != qs->qs_rsp);
			/* not yet complete, get out */
			if (stat1 == LASTNOT)
				goto out;
			/* calc. recv length and validate */
			tlen = (stat1 & RRLH)|(stat2 & RRLL)-
				sizeof(struct ether_header)+60;
printf("l=%d s1=%o s2=%o\n",tlen,stat1,stat2);
			if (tlen < QNMIN || tlen > QNMTU)
				stat1 |= UERR;
			if ((stat1 & UERR) == 0) {
				/* manipulate a valid first mbuf */
				qn = mtod(m, struct ether_header *);
				type = ntohs(qn->ether_type);
				if (type >= ETHERPUP_TRAIL &&
				    type < ETHERPUP_TRAIL+ETHERPUP_NTRAILER) {
					off = (type-ETHERPUP_TRAIL)*512;
					if (off >= QNMTU) {
						printf("qn%d: off>=QNMTU",unit);
						badflg = 1;
					}
				} else {
					off = 0;
				}
				/* zap off ethernet header */
				m->m_off += sizeof(struct ether_header);
				m->m_len -= sizeof(struct ether_header);
			}
			first = 0;
			len = 0;
		}
		/* free bus resources, bunk for the qbus */
		/*ubarelse(qninfo[unit]->ui_ubanum, qs->qs_ruba[pos].s_info);*/
		if ((stat1 & UERR) == 0) {
			/* If trailer starts in this buffer */
			if (off && off < (len+m->m_len)) {
				diff = off-len;
				if((MMAXOFF-(*mp)->m_off-(*mp)->m_len)>=diff){
					m2 = *mp;
				} else {
					MGET(m2, 0);
					if (m2 == 0) {
						badflg = 1;
						goto bad;
					}
					m2->m_len = 0;
					m2->m_off = MMINOFF;
				}
				/* copy last of data to end mbuf */
				copyv(m->m_click,m->m_off,m2->m_click,
					m2->m_off+m2->m_len,diff);
				m2->m_len += diff;
				m->m_off += diff;
				m->m_len -= diff;
				ptr2 = mtod(m, u_short *);
				type = ntohs(*ptr2++);
				resid = ntohs(*ptr2);
				m->m_off += 2*sizeof(u_short);
				m->m_len -= 2*sizeof(u_short);
				if (m2 != *mp) {
					*mp = m2;
				}
				top2 = top;
				mp = &top;
				len += diff;
				off = 0;
			}
bad:
			if (tlen-len < m->m_len)
				m->m_len = tlen-len;
			/* chain m into mp chain */
			*mp = m;
			mp = &m->m_next;
			len += m->m_len;
			/* if end of packet */
			if (len >= tlen) {
			  /* chain trailer on front of list */
			  if (top2 != 0)
				*mp = top2;
			  /* if packet ok */
			  if (badflg == 0) {
				switch (type) {
		
#ifdef INET
				case ETHERPUP_IPTYPE:
					schednetisr(NETISR_IP);
					inq = &ipintrq;
					break;
			
				case ETHERPUP_ARPTYPE:
					arpinput(&qs->qs_ac, m);
					goto setup;
#endif
				default:
					if (type != 0)
			  		printf("qn%d:l=%d t=%x\n", unit, len, type);
					m_freem(m);
					goto setup;
				}
			
				if (IF_QFULL(inq)) {
					IF_DROP(inq);
					m_freem(m);
					printf("QFL\n");
					goto setup;
				}
				IF_ENQUEUE(inq, m);
			  } else {
				m_freem(m);
				badflg = 0;
			  }
setup:
			  top = 0;
			  mp = &top;
			  first = 1;
			}
			MGET(m, 0);
		}
		if (m != 0) {
			m->m_len = MLEN;
			m->m_off = MMINOFF;
			qs->qs_ruba[pos].s_info=taddr=ubmalloc(0, m->m_click,
				m->m_len, 0);
			taddr = (taddr & 0x3fffff)+m->m_off;
			qs->qs_ruba[i].s_m = m;
			nlen = -(m->m_len>>1);
			ord = VALID;
		} else {
			taddr = qnaddbdl(unit, pos+1, RCV);
			nlen = 0;
			ord = VALID|CHAIN;
		}
		ptr2 = ptr+1;
		*ptr2-- = 01;
		*ptr2-- = BINIT;
		*ptr2-- = nlen;
		*ptr2-- = taddr & 0xffff;
		*ptr2-- = ((taddr>>16)&0x3f)|ord;
		*ptr2 = BINIT;
		ptr = qnxtbdl(unit, &pos, RCV|STAT);
	} while (pos != qs->qs_rsp);
out:
	qs->qs_rsp = pos;
	taddr = qnaddbdl(unit, pos-1, RCV);
	addr->csr = RI|IE|RE;
	if (addr->csr & RL) {
		addr->rbl = loint(taddr);
		addr->rbh = hiint(taddr);
	}
	Restormap(map);
}

/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Use trailer local net encapsulation if enough data in first
 * packet leaves a multiple of 512 bytes of data in remainder.
 * il_trailer_ok qs used to say if trailer protocol qs used on
 * thqs wire. If it qs zero, it defaults to support trailer.
 */
#ifdef	trailers
u_char	qn_trailer_ok = ETHERPUP_NOTRAILER;
#else
u_char	qn_trailer_ok = 0;
#endif
qnoutput(ifp, m0, dst)
	struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
{
	int type, s, error;
	u_char edst[6];
	struct in_addr idst;
	register struct qn_softc *qs = &qn_softc[ifp->if_unit];
	register struct mbuf *m = m0;
	register struct ether_header *qn;
	register int off;
	u_char *ptr;

	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		idst = ((struct sockaddr_in *)dst)->sin_addr;
		if (!arpresolve(&qs->qs_ac, m, &idst, edst))
			return (0);	/* if not yet resolved */
		off = ntohs((u_short)mtod(m, struct ip *)->ip_len) - m->m_len;
		/* need per host negotiation */
		if ((ifp->if_flags & IFF_NOTRAILERS) == 0)
		if ((off > 0 && (off & 0x1ff) == 0 &&
		    m->m_off >= MMINOFF + 2 * sizeof (u_short)) && 
		    (qn_trailer_ok)) {
			type = ETHERPUP_TRAIL + (off>>9);
			m->m_off -= 2 * sizeof (u_short);
			m->m_len += 2 * sizeof (u_short);
			*mtod(m, u_short *) = htons(ETHERPUP_IPTYPE);
			*(mtod(m, u_short *) + 1) = htons(m->m_len);
			goto gottrailertype;
		}
		type = ETHERPUP_IPTYPE;
		off = 0;
		goto gottype;
#endif

	case AF_UNSPEC:
		qn = (struct ether_header *)dst->sa_data;
		bcopy((caddr_t)qn->ether_dhost, (caddr_t)edst, sizeof (edst));
		type = qn->ether_type;
		goto gottype;

	default:
		printf("qn%:dchd af%d\n", ifp->if_unit,
			dst->sa_family);
		error = EAFNOSUPPORT;
		goto bad;
	}

gottrailertype:
	/*
	 * Packet to be sent as trailer: move first packet
	 * (control information) to end of chain.
	 */
	while (m->m_next)
		m = m->m_next;
	m->m_next = m0;
	m = m0->m_next;
	m0->m_next = 0;
	m0 = m;

gottype:
	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */
	if (m->m_off > MMAXOFF ||
	    MMINOFF + sizeof (struct ether_header) > m->m_off) {
		m = m_get(M_DONTWAIT);
		if (m == 0) {
			error = ENOBUFS;
			goto bad;
		}
		m->m_next = m0;
		m->m_off = MMINOFF;
		m->m_len = sizeof (struct ether_header);
	} else {
		m->m_off -= sizeof (struct ether_header);
		m->m_len += sizeof (struct ether_header);
	}
	qn = mtod(m, struct ether_header *);
	qn->ether_type = htons((u_short)type);
	bcopy((caddr_t)edst,(caddr_t)qn->ether_dhost, sizeof (edst));
	bcopy((caddr_t)qs->qs_addr, (caddr_t)qn->ether_shost, 6);

	/*
	 * Queue message on interface, and start output if interface
	 * not yet active.
	 */
	s = splimp();
	if (IF_QFULL(&ifp->if_snd)) {
		IF_DROP(&ifp->if_snd);
		splx(s);
		m_freem(m);
		return (ENOBUFS);
	}
	IF_ENQUEUE(&ifp->if_snd, m);
	qnstart(ifp->if_unit);
	splx(s);
	return (0);

bad:
	m_freem(m0);
	return (error);
}

/* qn interrupt service routine: simply calls qnrint, qnxint as required */
qnintr(unit)
	int unit;
{
	register struct qndevice *addr = (struct qndevice *)qninfo[unit]->ui_addr;

	if (addr->csr & RI)
		qnrint(unit);
	if (addr->csr & XI)
		qnxint(unit);
}

u_short *qnxtbdl(unit, pos, flag)
	int unit;
	int *pos;
	int flag;
{
	register int pos2;
	register u_short *qptr;
	register struct qn_softc *qs = &qn_softc[unit];
	struct qn_cbuf *qn = &qn_cbuf[unit];
	u_short *ptr2;
	u_long taddr;

	pos2 = *pos;
	if (flag & RCV) {
		do {
			pos2++;
			if (pos2 >= RCVSIZ) {
				pos2 = 0;
				ptr2 = qn->qn_rbdl;
				qptr = &qn->qn_rbdl[RCVSIZ].b_laddr;
				taddr = qs->qs_ubaddr & 0x3fffff;
				taddr += ((unsigned)ptr2)-((unsigned)qn);
				*qptr-- = loint(taddr);
				*qptr-- = hiint(taddr)|VALID|CHAIN;
				*qptr = BINIT;
			} else {
				ptr2 = (u_short *)&qn->qn_rbdl[pos2].b_flag;
			}
		} while (*(ptr2+1) & CHAIN);
	} else {
		pos2++;
		if (pos2 >= XMTSIZ) {
			pos2 = 0;
			ptr2 = qn->qn_xbdl;
			qptr = &qn->qn_xbdl[XMTSIZ].b_laddr;
			taddr = qs->qs_ubaddr & 0x3fffff;
			taddr += ((unsigned)ptr2)-((unsigned)qn);
			*qptr-- = loint(taddr);
			*qptr-- = hiint(taddr)|VALID|CHAIN;
			*qptr = BINIT;
		} else {
			ptr2 = (u_short *)&qn->qn_xbdl[pos2].b_flag;
		}
	}
	if (flag & STAT)
		ptr2 += 4;
	*pos = pos2;
	return(ptr2);
}

u_long qnaddbdl(unit, pos, flag)
	int unit, pos, flag;
{
	register struct qn_softc *qs = &qn_softc[unit];
	register struct qn_cbuf *qn = &qn_cbuf[unit];
	u_long taddr;

	taddr = qs->qs_ubaddr & 0x3fffff;
	if (flag & RCV) {
		if (pos < 0)
			pos = RCVSIZ-1;
		if (pos > (RCVSIZ-1))
			pos = 0;
		taddr += ((unsigned)&qn->qn_rbdl[pos])-((unsigned)qn);
	} else {
		if (pos < 0)
			pos = XMTSIZ-1;
		if (pos > (XMTSIZ-1))
			pos = 0;
		taddr += ((unsigned)&qn->qn_xbdl[pos])-((unsigned)qn);
	}
	return(taddr);
}
#endif
