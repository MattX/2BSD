/*	if_dc.c	4.22	82/07/21	*/

#include "dc.h"

#if	NDC > 0
/*
 * DECNA Ethernet Controller interface
 * WARNING: This driver is specific to the pdp11 implementation.
 * Be sure to set the parameters in dcreg.h correctly.
 */

#include "param.h"
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/buf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/ubavar.h>
#include <sys/dcreg.h>
#include <sys/ivecpos.h>
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


int	dcprobe(), dcattach(), dcintr();
struct	uba_device *dcinfo[NDC];
u_short dcstd[] = { 0 };
struct	uba_driver dcdriver =
	{ dcprobe, 0, dcattach, 0, dcstd, "dc", dcinfo };
#define	DCUNIT(x)	minor(x)
#define	RW8K	((127<<8)|RW)
#define	ritoc(i)	(RCVCLIK+(i)*RCLICK)
#define	xitoc(i)	(RCVCLIK+((i)+NRCV)*RCLICK)
#define	rotoi(o)	(((o)-RFDBASE)/RFDSIZE)
#define	otoa(o)		((u_short *)((o)+0120000))
#define	HANGDEL	{int j;for(j=0;j<250;j++)if(*scb==0)break;}

int	dcinit(),dcoutput(),dcreset(),dcioctl();

extern struct ifnet loif;

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * cs_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 * We also have, for each interface, a UBA interface structure, which
 * contains information about the UNIBUS resources held by the interface:
 * map registers, buffered data paths, etc.  Information is cached in this
 * structure for use by the if_uba.c routines in running the interface
 * efficiently.
 */
struct	dc_softc {
	struct	arpcom	cs_ac;		/* ARP struct. */
#define	cs_if	cs_ac.ac_if
#define	cs_addr	cs_ac.ac_enaddr
	struct	ifuba cs_ifuba;		/* UNIBUS resources */
	short	cs_oactive;		/* is output active? */
	u_short	cs_rfdfree;
	u_short	cs_rbdfree;
	short	cs_hang;
} dc_softc[NDC];
int timerst;

/*
 * Do output DMA to determine interface presence and
 * interrupt vector.  DMA is too short to disturb other hosts.
 */
dcprobe(reg)
	caddr_t reg;
{
	return (1);
}

/*
 * Interface exists: make available by filling in network interface
 * record.  System will initialize the interface when it is ready
 * to accept packets.
 */
dcattach(ui)
	struct uba_device *ui;
{
	struct dc_softc *cs = &dc_softc[ui->ui_unit];
	register struct ifnet *ifp = &cs->cs_if;
	register struct dcdevice *addr = (struct dcdevice *)ui->ui_addr;
	struct sockaddr_in *sin;
	int i, j;
	u_char *cp;

	ifp->if_unit = ui->ui_unit;
	ifp->if_name = "dc";
	ifp->if_mtu = ETHERMTU;

	/*
	 * Read the ethernet address off the board, one nibble at a time.
	 */
	addr->csr = EPROM;
	addr->rar = 0;
	for (i = 0; i < 6; i++)
		cs->cs_addr[i] = addr->rdw & 0xff;
	printf("dc%d: addr=%x:%x:%x:%x:%x:%x\n", ui->ui_unit,
		cs->cs_addr[0]&0xff, cs->cs_addr[1]&0xff,
		cs->cs_addr[2]&0xff, cs->cs_addr[3]&0xff,
		cs->cs_addr[4]&0xff, cs->cs_addr[5]&0xff);
	sin = (struct sockaddr_in *)&ifp->if_addr;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(ui->ui_flags);
	ifp->if_net = in_netof(sin->sin_addr);
	ifp->if_host[0] = in_lnaof(sin->sin_addr);
	sin = (struct sockaddr_in *)&ifp->if_broadaddr;
	sin->sin_family = AF_INET;
	sin->sin_addr = if_makeaddr(ifp->if_net, INADDR_ANY);
	ifp->if_flags |= IFF_BROADCAST;

	ifp->if_init = dcinit;
	ifp->if_output = dcoutput;
	ifp->if_ioctl = dcioctl;
	ifp->if_ubareset = dcreset;
	if_attach(ifp);
}

/*
 * Reset of interface after UNIBUS reset.
 * If interface is on specified uba, reset its state.
 */
dcreset(unit, uban)
	int unit, uban;
{
}

/*
 * Initialization of interface; clear recorded pending
 * operations, and reinitialize UNIBUS usage.
 */
dcinit(unit)
	int unit;
{
	register int i;
	register u_short *scb;
	register struct dcdevice *addr;
	struct dc_softc *cs = &dc_softc[unit];
	struct sockaddr_in *sin = (struct sockaddr_in *)&cs->cs_if.if_addr;
	int s, retrycnt;
	long rbase;
	segm save;

	if (sin->sin_addr.s_addr == 0)
		return;
	saveseg5(save);
	/*
	 * Hang receive buffers and start any pending writes.
	 * Setup the scb blocks, receive lists...
	 * Probably should have been parameterized more but..
	 */
	if ((cs->cs_if.if_flags & IFF_RUNNING) == 0) {
		addr = (struct dcdevice *)dcinfo[unit]->ui_addr;
		s = splimp();
		retrycnt = 0;
retry:
		addr->csr = 01;
		addr->csr = RESET;
		DELAY(10l);
		addr->mba = DCMEM;
		/* Set the SCP to point to the ISPC (bottom of 128K) */
		mapseg5(SCP, RW8K);
		scb = SCPPTR;
		*scb = 0;
		scb += 3;
		*scb++ = 0;
		*scb = 0;
		/* set the ISCP to the SCB (immediately following ISCP */
		mapseg5(SCB, RW8K);
		scb = SCBPTR;
		*scb++ = 01;
		*scb++ = CMDBASE;
		*scb++ = 0;
		*scb++ = 0;
		/* Initialize the SCB */
		*scb++ = 0;
		*scb++ = 0;
		*scb++ = CNFBASE;
		*scb++ = 0;
		*scb++ = 0;
		*scb++ = 0;
		*scb++ = 0;
		*scb++ = 0;
		/* Set up the configuration command */
		*scb++ = 0;
		*scb++ = 02;
		*scb++ = SADBASE;
		*scb++ = 0x80c;
		*scb++ = 0x2e40;
		*scb++ = 0x6080;
		*scb++ = 0xf200;
		*scb++ = 0x8;
		*scb++ = 0x40;
		/* And the address setup command */
		*scb++ = 0;
		*scb++ = 0x8001;
		*scb++ = 0;
		*scb++ = (cs->cs_addr[1]<<8)|(cs->cs_addr[0]&0xff);
		*scb++ = (cs->cs_addr[3]<<8)|(cs->cs_addr[2]&0xff);
		*scb = (cs->cs_addr[5]<<8)|(cs->cs_addr[4]&0xff);
		/* Start initialization sequence */
		addr->csr = LOOPBK;
		addr->csr = CALPBK;
		addr->csr = LOOPBK;
		scb = SCBPTR;
		for(i = 0; i < 10000; i++)
			if ((*scb & 01) == 0)
				break;
		if (*scb & 01) {
			if (++retrycnt < 5)
				goto retry;
			else {
				printf("dc%d:Ferini\n",unit);
				restorseg5(save);
				splx(s);
				return;
			}
		}
		scb = CMDPTR;
		*scb = CUSTART;
		addr->csr = LOOPBK;
		addr->csr = CALPBK;
		addr->csr = LOOPBK;
		scb += 16;
		/* and wait for completion */
		for(i = 0; i < 10000; i++)
			if (*scb & 0100000)
				break;
		if ((*scb & 0120000) != 0120000 || (*(scb-9) & 020000) == 0) {
			if (++retrycnt < 5)
				goto retry;
			else {
				printf("dc%d:feri cn=%o su=%o\n",
				unit, *(scb-9),*scb);
				restorseg5(save);
				splx(s);
				return;
			}
		}
		ienable(IVEC(addr, APOS));
		ienable(IVEC(addr, BPOS));
		scb += 6;
		/* Set up the RFD and RBD lists */
		for (i = 0; i < NRCV; i++) {
			*scb++ = 0;
			if (i == (NRCV-1))
				*scb++ = 0100000;
			else
				*scb++ = 0;
			*scb++ = RFDBASE+(i+1)*RFDSIZE;
			if (i == 0)
				*scb++ = RBDBASE;
			else
				*scb++ = 0xffff;
			scb += 7;
			*scb++ = 0;
			*scb++ = RBDBASE+(i+1)*RFDSIZE;
			rbase = RCVBASE;
			rbase += (i*RSIZE);
			*scb++ = rbase & 0xffff;
			*scb++ = (rbase>>16)&01;
			if (i == (NRCV-1))
				*scb++ = EL|RSIZE;
			else
				*scb++ = RSIZE;
		}
		/* and execute the RU start */
		scb = CMDPTR;
		*(scb+2) = RFDBASE;
		HANGDEL;
		if (*scb != 0) {
			if (++retrycnt < 5)
				goto retry;
			else {
				printf("dc%d:hng\n",unit);
				restorseg5(save);
				splx(s);
				return;
			}
		}
		*scb = RUSTART;
		addr->csr = NORMAL;
		addr->csr = CA;
		addr->csr = NORMAL;
		cs->cs_oactive = 0;
		cs->cs_rbdfree = cs->cs_rfdfree = 0;
		cs->cs_if.if_flags |= IFF_UP|IFF_RUNNING;
		if (cs->cs_if.if_snd.ifq_head)
			dcstart(unit);
		splx(s);
	}
	if (timerst == 0) {
		timerst++;
		dctimer();
	}
	if_rtinit(&cs->cs_if, RTF_UP);
	arpattach(&cs->cs_ac);
	arpwhohas(&cs->cs_ac, &sin->sin_addr);
	restorseg5(save);
}

/*
 * Start or restart output on interface.
 */
dcstart(dev)
	dev_t dev;
{
	register int i;
	register u_short *scb;
	register struct dcdevice *addr;
	u_short siz;
        int unit = DCUNIT(dev);
	struct dc_softc *cs = &dc_softc[unit];
	struct mbuf *m;
	long xbase;

	if (cs->cs_oactive)
		return;

	mapseg5(SCB, RW8K);
	scb = XCPTR;
	for (i = 0; i < NXMT; i++) {
		IF_DEQUEUE(&cs->cs_if.if_snd, m);
		if (m == 0)
			break;
		cs->cs_ifuba.ifu_w.ifrw_click = xitoc(i);
		siz = if_wubaput(&cs->cs_ifuba, m, 0);
		if (siz < (ETHERMIN+sizeof(struct eth_header)))
			siz = ETHERMIN+sizeof(struct eth_header);
		siz += 4;		/* Add 4 bytes for crc */
		*scb++ = 0;
		*scb++ = 04;
		*scb++ = XCBASE+(i+1)*XCSIZE;
		*scb = XCDBASE+i*XCSIZE;
		scb += 5;
		*scb++ = 0100000|siz;
		*scb++ = 0;
		xbase = XMTBASE;
		xbase += i*XSIZE;
		*scb++ = xbase&0xffff;
		*scb++ = (xbase>>16)&01;
	}
	if (m == 0) {
		if (i == 0)
			return;
	} else
		i--;
	*(scb-11) |= 0120000;
	scb = CMDPTR;
	HANGDEL;
	if (*scb != 0) {
		cs->cs_if.if_flags &= ~IFF_RUNNING;
		cs->cs_hang = 0;
		dcinit(unit);
		return;
	}
	*(scb+1) = XCBASE;
	*scb = CUSTART;
	addr = (struct dcdevice *)dcinfo[unit]->ui_addr;
	addr->csr = NORMAL;
	addr->csr = CA;
	addr->csr = NORMAL;
	cs->cs_oactive = i;
}

/*
 * Ethernet interface transmitter interrupt.
 * Start another output if more data to send.
 */
dcxint(unit)
	int unit;
{
	register struct dc_softc *cs = &dc_softc[unit];
	register int i;
	register u_short *scb;
	struct dcdevice *addr = (struct dcdevice *)dcinfo[unit]->ui_addr;

	if (cs->cs_oactive == 0)
		return;
	scb = XCPTR;
	for (i = 0; i < cs->cs_oactive; i++) {
		if ((*scb & 020000 == 0) || (addr->mba & 0200)) {
			printf("dc%d: oerrst=%o\n",unit,*scb);
			cs->cs_if.if_oerrors++;
		} else
			cs->cs_if.if_opackets++;
		if (*scb & 0xf)
			cs->cs_if.if_collisions += (*scb&0xf);
		if (*scb & 0x20)
			cs->cs_if.if_collisions += 16;
	}
	cs->cs_oactive = cs->cs_hang = 0;
	if (cs->cs_if.if_snd.ifq_head)
		dcstart(unit);
}

/*
 * Ethernet interface receiver interrupt.
 * If input error just drop packet.
 * Otherwise purge input buffered data path and examine 
 * packet to determine type.  If can't determine length
 * from type, then have to drop packet.  Othewise ddcapsulate
 * packet based on type and pass to type spdcific higher-level
 * input routine.
 */
dcrint(unit)
	int unit;
{
	register u_short *scb, *scp;
	register struct dc_softc *cs = &dc_softc[unit];
	int len, off, off2, i, resid, flg;
	u_short stat, stat2, offset, type;
	struct mbuf *m;
	long rbase;
	struct ifqueue *inq;
	struct dcdevice *addr = (struct dcdevice *)dcinfo[unit]->ui_addr;
	struct eth_header *dc;
	segm s5;

	scb = CMDPTR;
	if ((*(scb-1)&0x70)==0x20 && cs->cs_rfdfree && cs->cs_rbdfree) {
		scb = RFDPTR+rotoi(cs->cs_rfdfree)*RFDSIZE/2;
		*(scb+3) = cs->cs_rbdfree;
		scb = CMDPTR;
		*(scb+2) = cs->cs_rfdfree;
		HANGDEL;
		if (*scb != 0) {
			cs->cs_if.if_flags &= ~IFF_RUNNING;
			cs->cs_hang = cs->cs_oactive = 0;
			dcinit(unit);
			return;
		}
		*scb = RUSTART;
		addr->csr = NORMAL;
		addr->csr = CA;
		addr->csr = NORMAL;
		cs->cs_rbdfree = cs->cs_rfdfree = 0;
		flg++;
	}
	scb = RFDPTR;
	for(i = 0; i < NRCV; i++) {
		stat = *scb;
		if (stat & 0100000) {
			cs->cs_if.if_ipackets++;
			offset = *(scb+3);
			if (offset != 0xffff) {
				scp = otoa(offset);
				if (*scp & 040000)
					len = (*scp & 037777)-
						sizeof(struct eth_header);
				else
					len = 0x7fff;
			} else {
				scp = 0;
				len = 0x7fff;
			}
			if ((stat & 020000) && len <= ETHERMTU) {
				dc = ((struct eth_header *)0120000);
				saveseg5(s5);
				mapseg5(RCVCLIK+i*RCLICK, RW8K);
#define	dcdataaddr(dc, off, type)	((type)(((caddr_t)((dc)+1)+(off))))
				type = ntohs(dc->eth_type);
				if (type >= ETHERPUP_TRAIL &&
				    type < ETHERPUP_TRAIL+ETHERPUP_NTRAILER) {
					off = (type - ETHERPUP_TRAIL) * 512;
					if (off >= ETHERMTU) {
						restorseg5(s5);
						goto setup;		/* sanity */
					}
					type = ntohs(*dcdataaddr(dc, off, u_short *));
					resid = ntohs(*(dcdataaddr(dc, off+2, u_short *)));
					if (off + resid > len) {
						restorseg5(s5);
						goto setup;		/* sanity */
					}
					len = off + resid;
				} else
					off = 0;
				restorseg5(s5);
				if (len == 0)
					goto setup;
			
				/*
				 * Pull packet off interface.  Off is nonzero if packet
				 * has trailing header; dcget will then force this header
				 * information to be at the front, but we still have to drop
				 * the type and length which are at the front of any trailer data.
				 */
				cs->cs_ifuba.ifu_hlen = sizeof(struct eth_header);
				cs->cs_ifuba.ifu_r.ifrw_click = ritoc(i);
				m = if_rubaget(&cs->cs_ifuba, len, off);
				if (m == 0)
					goto setup;
				if (off) {
					m->m_off += 2 * sizeof (u_short);
					m->m_len -= 2 * sizeof (u_short);
				}
				switch (type) {
			
#ifdef INET
				case ETHERPUP_IPTYPE:
					schednetisr(NETISR_IP);
					inq = &ipintrq;
					break;
				case ETHERPUP_ARPTYPE:
					arpinput(&cs->cs_ac, m);
					goto setup;
#endif
				default:
					m_freem(m);
					goto setup;
				}
			
				if (IF_QFULL(inq)) {
					IF_DROP(inq);
					m_freem(m);
					goto setup;
				}
				IF_ENQUEUE(inq, m);
			} else {
				cs->cs_if.if_ierrors++;
			}
setup:
			if (scp) {
				do {
					stat2 = *scp;
					*scp++ = 0;
					off2 = *scp;
					if (cs->cs_rbdfree) {
						*scp++ = cs->cs_rbdfree;
						*(scp+2) = RSIZE;
					} else {
						*scp++ = 0xffff;
						*(scp+2) = EL|RSIZE;
					}
					cs->cs_rbdfree = offset;
					rbase = RCVBASE;
					rbase += (RSIZE*rotoi(offset));
					*scp++ = rbase & 0xffff;
					*scp = (rbase>>16)&01;
					scp = otoa(off2);
				} while ((stat2 & 0100000) == 0);
			}
			*scb++ = 0;
			if (cs->cs_rfdfree) {
				*scb++ = 0;
				*scb++ = cs->cs_rfdfree;
			} else {
				*scb++ = EL;
				*scb++ = 0xffff;
			}
			cs->cs_rfdfree = RFDBASE+i*RFDSIZE;
			*scb++ = 0xffff;
			scb += 12;
		} else {
			scb += 16;
		}
	}
	scb = CMDPTR;
	if (!flg && (*(scb-1)&0x70)==0x20 && cs->cs_rfdfree && cs->cs_rbdfree) {
		scb = RFDPTR+rotoi(cs->cs_rfdfree)*RFDSIZE/2;
		*(scb+3) = cs->cs_rbdfree;
		scb = CMDPTR;
		*(scb+2) = cs->cs_rfdfree;
		HANGDEL;
		if (*scb != 0) {
			cs->cs_if.if_flags &= ~IFF_RUNNING;
			cs->cs_hang = cs->cs_oactive = 0;
			dcinit(unit);
			return;
		}
		*scb = RUSTART;
		addr->csr = NORMAL;
		addr->csr = CA;
		addr->csr = NORMAL;
		cs->cs_rbdfree = cs->cs_rfdfree = 0;
	}
}

/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Use trailer local net encapsulation if enough data in first
 * packet leaves a multiple of 512 bytes of data in remainder.
 * If destination is this address or broadcast, send packet to
 * loop device to kludge around the fact that 3com interfaces can't
 * talk to themselves.
 */
dcoutput(ifp, m0, dst)
	struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
{
	int error, s;
	u_short type;
	struct in_addr dest;
	register struct dc_softc *cs = &dc_softc[ifp->if_unit];
	register struct mbuf *m = m0;
	register struct eth_header *dc;
	register int off, i;
	struct mbuf *mcopy = (struct mbuf *) 0;		/* Null */
	u_char edst[6];
	segm save;

	saveseg5(save);
	dest = ((struct sockaddr_in *)dst)->sin_addr;
	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		if (!arpresolve(&cs->cs_ac, m, &dest, edst)) {
			restorseg5(save);
			return(0);
		}
		/* If broadcast, send a copy to ourselves using loopback */
		if (in_lnaof(dest) == INADDR_ANY)
			mcopy = m_copy(m, 0, M_COPYALL);
		off = ntohs((u_short)mtod(m, struct ip *)->ip_len) - m->m_len;
		if ((ifp->if_flags & IFF_NOTRAILERS) == 0)
		if (off > 0 && (off & 0x1ff) == 0 &&
		    m->m_off >= MMINOFF + 2 * sizeof (u_short)) {
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
		dc = (struct eth_header *)dst->sa_data;
		bcopy((caddr_t)dc->eth_dhost, (caddr_t)edst, sizeof (edst));
		type = dc->eth_type;
		goto gottype;
	default:
		printf("dc%d:chd af%d\n", ifp->if_unit,
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
	    MMINOFF + sizeof (struct eth_header) > m->m_off) {
		m = m_get(M_DONTWAIT);
		if (m == 0) {
			error = ENOBUFS;
			goto bad;
		}
		m->m_next = m0;
		m->m_off = MMINOFF;
		m->m_len = sizeof (struct eth_header);
	} else {
		m->m_off -= sizeof (struct eth_header);
		m->m_len += sizeof (struct eth_header);
	}
	dc = mtod(m, struct eth_header *);
	for (i=0; i<6; i++) {
		dc->eth_shost[i] = cs->cs_addr[i];
		dc->eth_dhost[i] = edst[i];
	}
	dc->eth_type = htons((u_short)type);

	/*
	 * Queue message on interface, and start output if interface
	 * not yet active.
	 */
	s = splimp();
	if (IF_QFULL(&ifp->if_snd)) {
		IF_DROP(&ifp->if_snd);
		error = ENOBUFS;
		goto qfull;
	}
	IF_ENQUEUE(&ifp->if_snd, m);
	if (cs->cs_oactive == 0)
		dcstart(ifp->if_unit);
	splx(s);

gotlocal:
	restorseg5(save);
	return(mcopy ? looutput(&loif, mcopy, dst) : 0);

qfull:
	m0 = m;
	splx(s);
bad:
	m_freem(m0);
	restorseg5(save);
	return(error);
}

/*
 * Process an ioctl request.
 */
dcioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	int cmd;
	caddr_t data;
{
	register struct ifreq *ifr = (struct ifreq *)data;
	int s = splimp(), error = 0;

	switch (cmd) {

	case SIOCSIFADDR:
		if (ifp->if_flags & IFF_RUNNING)
			if_rtinit(ifp, -1);	/* delete previous route */
		dcsetaddr(ifp, (struct sockaddr_in *)&ifr->ifr_addr);
		dcinit(ifp->if_unit);
		break;

	default:
		error = EINVAL;
	}
	splx(s);
	return (error);
}

dcsetaddr(ifp, sin)
	register struct ifnet *ifp;
	register struct sockaddr_in *sin;
{

	ifp->if_addr = *(struct sockaddr *)sin;
	ifp->if_net = in_netof(sin->sin_addr);
	ifp->if_host[0] = in_lnaof(sin->sin_addr);
	sin = (struct sockaddr_in *)&ifp->if_broadaddr;
	sin->sin_family = AF_INET;
	sin->sin_addr = if_makeaddr(ifp->if_net, INADDR_ANY);
	ifp->if_flags |= IFF_BROADCAST;
}

/* This is the actual interrupt service routine that calls dcrint, dcxint */
dcintr(unit)
int unit;
{
	register u_short *scb;
	mapinfo save;

	Savemap(save);
	mapseg5(SCB, RW8K);
	scb = CMDPTR;
	scb--;
	if (*scb & 050000 || ((*scb & 0x70)==0x20))
		dcrint(unit);
	if (*scb & 0120000)
		dcxint(unit);
	Restormap(save);
}

/* This timer routine attempts to recognize a hung cntrl. and if so
 * calls dcinit.
 */
dctimer()
{
	register int i;
	register struct dc_softc *cs;
	int s;

	s = splimp();
	for(i = 0; i < NDC; i++) {
		cs = &dc_softc[i];
		if (cs->cs_oactive) {
			if (cs->cs_hang) {
				cs->cs_if.if_flags &= ~IFF_RUNNING;
				cs->cs_hang = cs->cs_oactive = 0;
				dcinit(i);
			} else
				cs->cs_hang++;
		}
	}
	splx(s);
	timeout(dctimer, (caddr_t)0, 2*hz);
}
#endif
