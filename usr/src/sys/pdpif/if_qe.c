/*
 * if_qe.c
 *
 * SCCSID: @(#)if_qe.c	2.9	2/21/86
 *	based on "@(#)if_qe.c	1.1	(decvax!rjl)";
 */

/**********************************************************************
 *   Copyright (c) Digital Equipment Corporation 1984, 1985.	      *
 *   All Rights Reserved. 					      *
 *   Reference "/usr/include/COPYRIGHT" for applicable restrictions.  *
 **********************************************************************/
/* ---------------------------------------------------------------------
 * Modification History 
 *
 *  04 Oct. 84 -- jf
 *	Added support for new ioctls to add and delete multicast addresses
 *	and set the physical address.
 *	Add support for general protocols.
 *
 *  14 Aug. 84 -- rjl
 *	Integrated Shannon changes. (allow arp above 1024 and ? )
 *
 *  13 Feb. 84 -- rjl
 *
 *	Initial version of driver. derived from IL driver.
 *
 * ---------------------------------------------------------------------
 */

#include "qe.h"
#if	NQE > 0
/*
 * Digital Q-BUS to NI Adapter
 */

#include "param.h"
#include "../machine/seg.h"
#include "mbuf.h"
#include "domain.h"
#include "protosw.h"
#include "socket.h"
#include "ioctl.h"
#include "errno.h"
#include "kernel.h"

#include "../net/if.h"
#include "../net/netisr.h"
#include "../net/route.h"

#ifdef INET
#include "../netinet/in.h"
#include "../netinet/in_systm.h"
#include "../netinet/in_var.h"
#include "../netinet/ip.h"
#include "../netinet/if_ether.h"
#endif

#ifdef NS
#include "../netns/ns.h
#include "../netns/ns_if.h"
#endif

#include "if_qereg.h"
#include "../pdpuba/ubavar.h"

#if NQE > 1
#define NRCV	5	 		/* Receive descriptors		*/
#else
#define NRCV	20	 		/* Receive descriptors		*/
#endif
#define NXMT	15	 		/* Transmit descriptors		*/
#define NTOT	(NXMT + NRCV)

#define	QETIMEOUT	2		/* transmit timeout, must be > 1 */

/*
 * This constant should really be 60 because the qna adds 4 bytes of crc.
 * However when set to 60 our packets are ignored by deuna's , 3coms are
 * okay ??????????????????????????????????????????
 */
#define MINDATA 64

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * qe_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 */
/* We allocate enough descriptors to do scatter/gather plus 1 to
 * construct a ring.
 *
 * The MicroVAX-I doesn't have an I/O map, therefore all addresses presented
 * to devices must be physical address. To keep track of mbufs in use the
 * softc for this device has to record the mbufs because  the data addresses
 * in the ring descriptors are physical addresses instead of virtual. With
 * an I/O map this will no longer be necessary and we'll be able to use
 * the mbuf macro's for allocation and deallocation.
 *
 * There must be enough descriptors in the receive ring to map at least 2
 * of the largest size datagrams so that a race condition doesn't occur in
 * the receiver. (~1600/252 bytes)
 *
 */
struct	qe_softc {
	struct	arpcom qe_ac;		/* Ethernet common part 	*/
#define	qe_if	qe_ac.ac_if		/* network-visible interface 	*/
#define	qe_addr	qe_ac.ac_enaddr		/* hardware Ethernet address 	*/
	struct	qe_ring rring[NRCV+1];	/* Receive ring descriptors 	*/
	struct	qe_ring tring[NXMT+1];	/* Transmit ring descriptors 	*/
	struct	mbuf *rmbuf[NRCV+1];	/* Receive mbuf chains		*/
	struct	mbuf *tmbuf[NXMT+1];	/* Transmit mbuf chains		*/
	int	rindex;			/* Receive index		*/
	int	tindex;			/* Transmit index		*/
	int	otindex;		/* Old transmit index		*/
	int	qe_intvec;		/* Interrupt vector 		*/
	struct	qedevice *addr;		/* device address		*/
	int	nxmit;			/* number of transmits		*/
	int	qe_restarts;		/* timeouts			*/
} qe_softc[NQE];

int	qeattach(), qeintr(), qetimeout();
int	qeinit(), qeoutput(), qeioctl();

u_short qestd[] = { 0 };
struct	uba_driver qedriver =
	{ 0, 0, qeattach, 0, qestd, "qe", 0 };

#if NQE > 1
SETUP_PKT NEEDS TO BE MOVED INTO THE QE_SOFTC STRUCTURE
#else
static u_char setup_pkt[16][8];
#endif

#define	QE_TIMEO	(15)
#define	QEUNIT(x)	minor(x)

#define DEBUG
#ifdef DEBUG
int qedebug = 0;
#endif

/*
 * Interface exists: make available by filling in network interface
 * record.  System will initialize the interface when it is ready
 * to accept packets.
 */
qeattach(ui)
	struct uba_device *ui;
{
	register struct qe_softc *sc = &qe_softc[ui->ui_unit];
	register struct ifnet *ifp = &sc->qe_if;
	register struct qedevice *addr = (struct qedevice *)ui->ui_addr;
	register int i;

	ifp->if_unit = ui->ui_unit;
	ifp->if_name = "qe";
	ifp->if_mtu = ETHERMTU;
	ifp->if_flags = IFF_BROADCAST;

	/*
	 * Read the address from the prom and save it.
	 */
	for ( i=0 ; i<6 ; i++ )
		sc->qe_addr[i] = addr->qe_sta_addr[i] & 0xff;  
	printf("qe%d: hardware address %s\n", ui->ui_unit,
		ether_sprintf(sc->qe_addr));

	/*
	 * Save the vector for initialization at reset time.
	 */
	sc->qe_intvec = addr->qe_vector;
	sc->addr = addr;

	ifp->if_init = qeinit;
	ifp->if_output = qeoutput;
	ifp->if_ioctl = qeioctl;
	ifp->if_reset = NULL;
	ifp->if_watchdog = qetimeout;
	if_attach(ifp);
}

/*
 * Initialization of interface and allocation of mbufs for receive ring
 * buffers.
 */
qeinit(unit)
	int unit;
{
	register struct qe_softc *sc = &qe_softc[unit];
	register struct qedevice *addr = sc->addr;
	register struct ifnet *ifp = &sc->qe_if;
	register i;
	int s;
	long add;

	/* address not known */
	if (ifp->if_addrlist == (struct ifaddr *)0)
			return;

	/*
	 * Initialize the transmit and receive rings.
	 */
	initring(sc->rring, sc->rmbuf, NRCV, QEALLOC);
	initring(sc->tring, sc->tmbuf, NXMT, QENOALLOC);

	sc->nxmit = sc->otindex = sc->tindex = sc->rindex = 0;

	/*
	 * Take the interface out of reset, program the vector, 
	 * enable interrupts, and tell the world we are up.
	 */
	s = splimp();
#define	QEVECT	0400				/* arbitrary, for now... */
	addr->qe_vector = QEVECT;
	addr->qe_csr = QE_RCV_ENABLE | QE_INT_ENABLE | QE_XMIT_INT |
	    QE_RCV_INT | QE_ILOOP;
	add = startnet + (long)&sc->rring[sc->rindex];
	addr->qe_rcvlist_lo = loint(add);
	addr->qe_rcvlist_hi = hiint(add) & 077;
	ifp->if_flags |= IFF_UP | IFF_RUNNING;
	qesetup(sc);
	qeissuesetup( sc, "qeinit");
	qestart(unit);
	splx(s);
}

/*
 * Start output on interface.
 *
 */
static
qestart(dev)
	dev_t dev;
{
	int unit = QEUNIT(dev);
	register struct qe_softc *sc = &qe_softc[unit];
	register struct qedevice *addr;
	register struct qe_ring *rp;
	struct mbuf *m;
	long add;
	int buf_addr, len, s, desc_needed, i, j, tlen, diff;

	/*
	 * Check for enough transmit descriptors to map
	 * this datagram onto the interface. If there will never be enough
	 * throw the packet away and complain. If there will be enough but
	 * there aren't right now just return and if there are enough now,
	 * setup one or more descriptors to map the packet onto the interface 
	 * and start it.
	 *
	 * The deqna doesn't look at anything but the valid bit to determine
	 * if it should transmit this packet.  If you fill the ring, the
	 * device will loop indefinately and flood the network with packets
	 * until the ring is broken.  So, we always make sure there is at
	 * least one empty descriptor.
	 */
	s = splimp();
	if( (m = sc->qe_if.if_snd.ifq_head) == 0 ){
		splx( s );
		return;				/* Nothing on the queue	*/
	}

	for( desc_needed = 0 ; m ; m = m->m_next )
		desc_needed++;

	if( desc_needed > NXMT-1 )
		panic("qe: xmit packet too big");

	/*
	 * See if the required descriptors are available.
	 */
	addr = sc->addr;
	i = 0;
	j = sc->tindex;
	while( sc->tring[j].qe_valid == 0 && i <= desc_needed ) {
		i++;
		j = ++j % NXMT;
	}

	if((desc_needed+1) > i) {
		splx( s );
		return;
	}

	/*
	 * Record the chain head, attach each mbuf data area to a 
	 * descriptor and start the QNA if the transmit list is invalid.
	 */
	IF_DEQUEUE(&sc->qe_if.if_snd, m);
	/*
	 * Save the chain head so that we can deallocate it after the
	 * i/o is done. This will not be necessary when we have an i/o map
	 * because we can use virtual addresses ?
	 */
	for(i=sc->tindex, sc->tmbuf[i]=m, tlen=0 ; m ; m=m->m_next, i = ++i % NXMT){
		rp = &sc->tring[i];
		len = m->m_len;

		/*
		 *  Does buffer end on odd byte ? 
		 */
		if( len & 1 ) {
			len++;
			rp->qe_odd_end = 1;
		}
		tlen += len;
		rp->qe_buf_len = -(len/2);
		add = startnet + (long)m + (long)m->m_off;
		rp->qe_addr_lo = loint(add);
		rp->qe_addr_hi = hiint(add) & 077;
		if( m->m_next == NULL ) {
			/*
			 * Make sure we don't send a runt.
			 */
			if( tlen < MINDATA ) {
				diff = MINDATA - tlen;
				if( (len + diff + m->m_off) <= MMAXOFF ) {
					rp->qe_buf_len = -(len + diff)/2;
					tlen += diff;
				} else {
					/*
					 * This packet is too short.  Grab
					 * another descriptor, and point it
					 * to arbitrary data, just so that
					 * we can fill up to the minimum
					 * length.  This should probabaly be
					 * an empty buffer, not low core, but
					 * no one should look at it anyway...
					 */
					i = ++i % NXMT;
					rp = &sc->tring[i];
					if( diff & 1 )
						diff++;
					rp->qe_buf_len = -(diff/2);
					rp->qe_addr_lo = 0;
					rp->qe_addr_hi = 0;
				}
			}
			rp->qe_eomsg = 1;
			rp->qe_valid = 1;
			/*
			 * Last descriptor !! If the QNA is running it could
			 * beat us down the list if we set the valid address 
			 * bits in the forward order, so we do it backwards.
			 */
			for( j = i ; j != sc->tindex ; ) {
				j = --j >= 0 ? j : NXMT;
				sc->tring[j].qe_valid = 1;
			}
			sc->nxmit++;
		}
	}
	sc->qe_if.if_timer = QETIMEOUT;

	/*
	 * See if the xmit list is invalid.
	 */
	if( addr->qe_csr & QE_XL_INVALID ) {
		add = startnet + (long)&sc->tring[sc->tindex];
		addr->qe_xmtlist_lo = loint(add);
		addr->qe_xmtlist_hi = hiint(add) & 077;
	}
	sc->tindex = i;
	splx( s );
}
 
/*
 * Ethernet interface interrupt processor
 */
qeintr(unit)
	int unit;
{
	register struct qe_softc *sc = &qe_softc[unit];
	register struct qedevice *addr = sc->addr;
	int s, csr;

	s = splimp();
	csr = addr->qe_csr;
	addr->qe_csr = QE_RCV_ENABLE | QE_INT_ENABLE | QE_XMIT_INT | QE_RCV_INT | QE_ILOOP;
	if( csr & QE_RCV_INT )
		qerint( unit );
	if( csr & QE_XMIT_INT )
		qetint( unit );
	if( csr & QE_NEX_MEM_INT )
		panic("qe: Non existant memory interrupt");
	
	if( addr->qe_csr & QE_RL_INVALID && sc->rring[sc->rindex].qe_status1 == QE_NOTYET ) {
		long add;
		add = startnet + (long)&sc->rring[sc->rindex];
		addr->qe_rcvlist_lo = loint(add);
		addr->qe_rcvlist_hi = hiint(add) & 077;
	}
	splx( s );
}
 
/*
 * Ethernet interface transmit interrupt.
 */
static
qetint(unit)
	int unit;
{
	register struct qe_softc *sc = &qe_softc[unit];
	register struct mbuf *mp;
	register first, index;
	int i, len, status1;

	while (sc->otindex != sc->tindex &&
	    sc->tring[sc->otindex].qe_status1 != QE_NOTYET) {
		/*
		 * Find the index of the last descriptor in this 
		 * packet. ( LASTNOT will be clear ) If we can't find one
		 * then the QNA is still working on it. This is necessary
		 * for subsequent passes because we can't be sure that the
		 * QNA is through with a descriptor until we find the last
		 * in the chain.
		 */
		first = index = sc->otindex;
		while (sc->tring[index].qe_valid && !sc->tring[index].qe_eomsg)
			index = ++index % NXMT;
		/*
		 * Is the QNA done with this packet ?
		 */
		if (sc->tring[index].qe_status1 == QE_NOTYET)
			break;
		/*
		 * Save the status words from the descriptor so that it can
		 * be released.
		 */
		status1 = sc->tring[index].qe_status1;
		mp = sc->tmbuf[first];

		qeinitdesc(&sc->tring[first], &sc->tmbuf[first], QENOALLOC, NULL);
		if (first == NXMT-1)
			sc->tring[NXMT].qe_flag = QE_NOTYET;
		while (first != index) {
			first = ++first % NXMT;
			qeinitdesc(&sc->tring[first], &sc->tmbuf[first], QENOALLOC, NULL);
			if (first == NXMT-1)
				sc->tring[NXMT].qe_flag = QE_NOTYET;
		}
		sc->otindex = ++index % NXMT;
		if (--sc->nxmit <= 0) {
			sc->qe_if.if_timer = 0;
			sc->nxmit = 0;
		}

		/*
		 * Do some statistics.
		 */
		sc->qe_if.if_opackets++;
		sc->qe_if.if_collisions += ( status1 & QE_CCNT ) >> 4;
		if (status1 & QE_ERROR) { 
			sc->qe_if.if_oerrors++;
			m_freem(mp);
		} else if (mp) {
			/*
			 * The QNA doesn't hear it's own packets. Unfortunately
			 * the code above us expects to hear all broadcast 
			 * traffic including our own. Therefore if this is a
			 * broadcast packet we have to loop it back,
			 * otherwise we simply free the packets.
			 */
			{
				register short *p;

				p = (short *)(mtod(mp, struct ether_header *)
					->ether_dhost);
				i = *p++ == -1 && *p++ == -1 && *p == -1;
			}
			if (i) {
				register struct mbuf *mp0;
				for (mp0 = mp, len=0; mp0; mp0 = mp0->m_next)
					len += mp0->m_len;
				qeread(sc, mp, len);
			} else
				m_freem(mp);
		}
	}
	qestart(unit);
}
 
struct foob {
	unsigned char lo;
	unsigned char hi;
};
#define	lobyte(x)	(((struct foob *)&(x))->lo)
#define	hibyte(x)	(((struct foob *)&(x))->hi)
/*
 * Ethernet interface receiver interrupt.
 * If can't determine length from type, then have to drop packet.  
 * Othewise decapsulate packet based on type and pass to type specific 
 * higher-level input routine.
 *	Fred Canter -- 2/20/86
 *	Serach the entire receive ring for a completed message
 *	instead of depending on the DEQNA to always put the next
 *	message in the descriptor pointed to by sc->rindex.
 *	Looks like this fixed the "winking out" problem.
 */
static
qerint(unit)
	int unit;
{
	register struct qe_softc *sc = &qe_softc[unit];
	struct mbuf *m, *n;
	int len, index, first, status1, status2, resid, drop;
	struct mbuf tmb;
	int orindex, i;

	orindex = sc->rindex;
	i = sc->rindex;
	for (;;) {
		if(sc->rring[i].qe_status1 != QE_NOTYET) {
			sc->rindex = i;
			break;
		}
		i = ++i % NRCV;
		if(i == orindex)
			return;
	}
	/*
	 * Traverse the receive ring looking for packets to pass back.
	 * The search is complete when we find a descriptor not in use.
	 */
	while ( sc->rring[sc->rindex].qe_status1 != QE_NOTYET ) {
		/*
		 * Find the index of the last descriptor in this 
		 * packet. ( LASTNOT will be clear ) If we can't find one
		 * then the QNA is still working on it.
		 */
		first = index = sc->rindex;

		while( (sc->rring[index].qe_status1 & QE_MASK) == QE_MASK )
			index = ++index % NRCV;
		/*
		 * If we found an unused descriptor we've beat the QNA 
		 */
		if( sc->rring[index].qe_status1 == QE_NOTYET )
			break;
		/*
		 * Save the status words from the descriptor so that it can
		 * be released. This thing isn't valid unless the low and
		 * high bytes are the same.
		 */
		status2 = sc->rring[index].qe_status2;
		if (lobyte(status2) != hibyte(status2))
			break;
		status1 = sc->rring[index].qe_status1;
		sc->qe_if.if_ipackets++;
		/*
		 * Link the mbufs together, reinitialize the descriptors and
		 * pass the mbuf chain off to qeread if status is okay.
		 */
		n = &tmb;
		drop = 0;
		for (;;) {
			m = sc->rmbuf[first];
			if (qeinitdesc(&sc->rring[first], &sc->rmbuf[first],
							QEALLOC, m)) {
				n->m_next = m;
				n = m;
			} else
				drop++;
/* see qeinitdesc()	sc->rring[first].qe_status2 = 1;	*/
			if (first == NRCV - 1)
				sc->rring[NRCV].qe_flag = QE_NOTYET;
			if (first == index)
				break;
			first = ++first % NRCV;
		}
		if ((sc->rindex = ++first % NRCV) == NRCV - 1)
			sc->rring[NRCV].qe_flag = QE_NOTYET;
		n->m_next = 0;
		m = tmb.m_next;
		/*
		 * If this was a setup packet, discard it.
		 */
		if (status1 & QE_ESETUP) {
#ifdef DEBUG
			if (qedebug)
				printf("qe%d: setup pkt\n", unit);
#endif
			m_freem( m );
			continue;
		}
		if (drop) {
			sc->qe_if.if_ierrors++;
			m_freem( m );
#ifdef DEBUG
			if (qedebug)
				printf("qe%d: pkt drop\n", unit);
#endif
			continue;
		}
		/*
		 * If there was an error discard it.
		 */
		if (status1 & QE_ERROR) {
			sc->qe_if.if_ierrors++;
			m_freem( m );
#ifdef DEBUG
			if (status1&QE_DISCARD) {
				if (qedebug) {
				    if (status1&QE_SHORT)
					printf("qe%d: short pkt\n", unit);
				    if (status1&QE_RUNT)
					printf("qe%d: runt pkt\n", unit);
				}
			} else if (qedebug) {
				if (status1&QE_RUNT)
					printf("qe%d: runt pkt\n", unit);
				else
					printf("qe%d: long pkt\n", unit);
			}
#endif
			continue;
		}
		/*
		 * Get the actual length and compute the size of data in the
		 * last mbuf. The hardware doesn't have time to count the first
		 * 60 bytes because it is doing address filtering so we add 60.
		 */
		len = ((status1 & QE_RBL_HI) | (status2 & QE_RBL_LO)) + 60;

		/*
		 * The last mbuf may not be full so we have to set the correct 
		 * length in it.
		 */
		if( resid = len % MLEN )
			n->m_len = resid;
		qeread( sc, m, len );
	}
}
 
/*
 * Ethernet output routine.
 * Encapsulate a packet of type family for the local net.
 * Use trailer local net encapsulation if enough data in first
 * packet leaves a multiple of 512 bytes of data in remainder.
 */
qeoutput(ifp, m0, dst)
	struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
{
	int type, s, error;
	u_char edst[6];
	struct in_addr idst;
	register struct qe_softc *is = &qe_softc[ifp->if_unit];
	register struct mbuf *m = m0;
	register struct ether_header *eh;
	register int off;
	int usetrailers;

	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING)) {
		error = ENETDOWN;
		goto bad;
	}

	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		idst = ((struct sockaddr_in *)dst)->sin_addr;
		if (!arpresolve(&is->qe_ac, m, &idst, edst, &usetrailers))
			return (0);	/* if not yet resolved */
#ifdef TRAILER_TYPE_NEVER_SET
		if (usetrailers) {
			off = ntohs((u_short)mtod(m, struct ip *)->ip_len)
			    - m->m_len;
			if (off > 0 && (off & 0x1ff) == 0 &&
			    m->m_off >= MMINOFF + 2 * sizeof (u_short)) {
				u_short *p;

				type = ETHERTYPE_TRAIL + (off>>9);
				m->m_off -= 2 * sizeof (u_short);
				m->m_len += 2 * sizeof (u_short);
				p = mtod(m, u_short *);
				*p++ = htons(ETHERTYPE_IP);
				*p = htons(m->m_len);
				goto gottrailertype;
			}
		}
#endif
		type = ETHERTYPE_IP;
		off = 0;
		goto gottype;
#endif
#ifdef NS
	case AF_NS:
		type = ETHERTYPE_NS;
		bcopy((caddr_t)&(((struct sockaddr_ns *)dst)->sns_addr.x_host),
		    (caddr_t)edst, sizeof (edst));
		off = 0;
		goto gottype;
#endif

	case AF_UNSPEC:
		eh = (struct ether_header *)dst->sa_data;
		bcopy((caddr_t)eh->ether_dhost, (caddr_t)edst, sizeof (edst));
		type = eh->ether_type;
		goto gottype;

	default:
		printf("qe%d: can't handle af%d\n", ifp->if_unit,
			dst->sa_family);
		error = EAFNOSUPPORT;
		goto bad;
	}

#ifdef TRAILER_TYPE_NEVER_SET
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
#endif

gottype:
	/*
	 * Add local net header.  If no space in first mbuf,
	 * allocate another.
	 */
	if (m->m_off > MMAXOFF ||
	    MMINOFF + sizeof (struct ether_header) > m->m_off) {
		m = m_get(M_DONTWAIT, MT_HEADER);
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
	eh = mtod(m, struct ether_header *);
	eh->ether_type = htons((u_short)type);
	bcopy((caddr_t)edst, (caddr_t)eh->ether_dhost, sizeof (edst));
	bcopy((caddr_t)is->qe_addr, (caddr_t)eh->ether_shost, sizeof (is->qe_addr));

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
	qestart(ifp->if_unit);
	splx(s);
	return (0);

bad:
	m_freem(m0);
	return (error);
}


/*
 * Process an ioctl request.
 */
qeioctl(ifp, cmd, data)
	register struct ifnet *ifp;
	int cmd;
	caddr_t data;
{
	struct qe_softc *sc = &qe_softc[ifp->if_unit];
	struct ifaddr *ifa = (struct ifaddr *)data;
	int s = splimp(), error = 0;

	switch (cmd) {

	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;
		qeinit(ifp->if_unit);
		switch(ifa->ifa_addr.sa_family) {
#ifdef INET
		case AF_INET:
			((struct arpcom *)ifp)->ac_ipaddr =
				IA_SIN(ifa)->sin_addr;
			arpwhohas((struct arpcom *)ifp, &IA_SIN(ifa)->sin_addr);
			break;
#endif
#ifdef NS
		case AF_NS:
		    {
			register struct ns_addr *ina = &(IA_SNS(ifa)->sns_addr);

			if (ns_nullhost(*ina)) {
				ina->x_host = * (union ns_host *)
					(qe_softc[ifp->if_unit].qe_addr);
			} else
				qe_setaddr(ina->x_host.c_host, ifp->if_unit);
			break;
		    }
#endif
		}
		break;

	case SIOCSIFFLAGS:		/* this should be here... KB */
	default:
		error = EINVAL;

	}
	splx(s);
	return (error);
}

/*
 * Initialize a ring descriptor with mbuf allocation side effects
 * If we are allocating, and mp2 is non-null, then use it if we
 * can't allocate a new mbuf.  The return value indicates whether
 * or not we used mp2.
 */
static
qeinitdesc(rp, mp, option, mp2)
	register struct qe_ring *rp;
	register struct mbuf **mp;
	int option;
	struct mbuf *mp2;
{
	/*
	 * clear the entire descriptor
	 */
	bzero(rp, sizeof(*rp));

	rp->qe_flag = rp->qe_status1 = QE_NOTYET;
	rp->qe_status2 = 0xff00;

	/*
	 * Perform the necessary allocation/deallocation
	 */
	if (option == QENOALLOC)
		*mp = NULL;
	else {
		register struct mbuf *m;
		long add;

		if ((option == QEALLOC) || ((m = *mp) == NULL))
			MGET(m, M_DONTWAIT, MT_DATA);
		if (m == 0) {
		    if ((m = mp2) == 0)
			panic("qe: no mbufs for desc ring");
		}
		*mp = m;
		rp->qe_buf_len = -(MLEN/2);
		m->m_len = MLEN; 
		m->m_off = MMINOFF;
		add = startnet + (long)m + (long)m->m_off;
		rp->qe_addr_lo = loint(add);
		rp->qe_addr_hi = hiint(add) & 077;
		/*
		 * Must be last. QNA could be listening.
		 */
		rp->qe_valid = 1;
		return(m != mp2);
	}
	return(1);
}

/*
 * Build a setup packet - the physical address will already be present
 * in first column.
 */
static
qesetup(sc)
struct qe_softc *sc;
{
	register i, j;

	/*
	 * Get the addr off of the interface and place it into the setup
	 * packet. This code looks strange due to the fact that the address
	 * is placed in the setup packet in col. major order. 
	 */
	for (i = 0 ; i < 6 ; i++)
		setup_pkt[i][1] = sc->qe_addr[i];

	/* copy the target address to the rest of the entries in this row. */
	for (j = 0; j < 6; j++)
		for (i = 2; i < 8; i++)
			setup_pkt[j][i] = setup_pkt[j][1];

	/* duplicate the first half. */
	bcopy(setup_pkt, setup_pkt[8], 64);

	/* fill in the broadcast address. */
	for (i = 0; i < 6; i++)
		setup_pkt[i][2] = 0xff;
}

/*
 * Process a packet.
 */
static
qeread( sc, m, len )
	struct qe_softc *sc;
	struct mbuf *m;
	int len;
{
	register struct mbuf *mp;
	int resid, index;
	struct ifqueue *inq;
	register int type;

	type = ntohs(mtod(m, struct ether_header *)->ether_type);
	if (len == 0 || type >= ETHERTYPE_TRAIL &&
	    type < ETHERTYPE_TRAIL+ETHERTYPE_NTRAILER) {
		/* we don't do trailers */
		goto done;
	}

	m->m_off += sizeof(struct ether_header) - sizeof(struct ifnet **);
	m->m_len -= sizeof(struct ether_header) - sizeof(struct ifnet **);
	*(mtod(m, struct ifnet **)) = &sc->qe_if;

	switch (type) {
#ifdef INET
	case ETHERTYPE_IP:
		schednetisr(NETISR_IP);
		inq = &ipintrq;
		break;

	case ETHERTYPE_ARP:
		arpinput(&sc->qe_ac, m);
		return;
#endif
#ifdef NS
	case ETHERTYPE_NS:
		schednetisr(NETISR_NS);
		inq = &nsintrq;
		break;
#endif
	default:
		goto done;
	}

	if (IF_QFULL(inq)) {
		IF_DROP(inq);
done:		m_freem(m);
		return;
	}
	IF_ENQUEUE(inq, m);
}
 
/*
 * Watchdog timeout routine. There is a condition in the hardware that
 * causes the board to lock up under heavy load. This routine detects
 * the hang up and restarts the device.
 */
qetimeout(unit)
	int unit;
{
	register struct qe_softc *sc;
	int s;

	s = splimp();
	sc = &qe_softc[unit];
	printf("qe%d: transmit timeout, restarted %d\n",
	    unit, ++sc->qe_restarts);
	qerestart(sc);
	splx(s);
}

/*
 * On a qerestart, if qe_xfree is set free all mbufs on the transmit ring,
 * otherwise re-transmit them.  Freeing is the recommended approach.
 */
int qe_xfree = 1;
qerestart(sc)
	register struct qe_softc *sc;
{
	register struct ifnet *ifp = &sc->qe_if;
	register struct qedevice *addr = sc->addr;
	register i;
	long add;

	/*
	 * 1. Reset the device.
	 * 2. Clean out the transmit ring, and put all the pending
	 *    transmits back on the send queue.  We do this in
	 *    reverse order, and stick them at the front of the queue.
	 * 3. Clean out and re-allocate the receive ring, making
	 *    sure that we free up any mbufs that don't get re-used!
	 * 4. Turn on interrupts, validate the receive ring, and
	 *    issue a setup packet.
	 * 5. Restart the device.
	 */
	addr->qe_csr = QE_RESET;
	addr->qe_csr &= ~QE_RESET;

	if (qe_xfree == 0) {
		i = sc->tindex;
		do {
			register struct mbuf *m;

			if (--i < 0)
				i = NXMT - 1;
			if (m = sc->tmbuf[i])
				IF_PREPEND(&ifp->if_snd, m);
		} while (i != sc->otindex);
	}
	else if (qe_xfree == 1)
		for (i = 0; i < NXMT; i++) {
			register struct mbuf *m;

			if (m = sc->tmbuf[i])
				m_freem(m);
		}

	initring(sc->tring, sc->tmbuf, NXMT, QENOALLOC);
	initring(sc->rring, sc->rmbuf, NRCV, QEREALLOC);

	sc->nxmit = sc->otindex = sc->tindex = sc->rindex = 0;
	addr->qe_csr = QE_RCV_ENABLE | QE_INT_ENABLE | QE_XMIT_INT |
	    QE_RCV_INT | QE_ILOOP;
	add = startnet + (long)&sc->rring[0];
	addr->qe_rcvlist_lo = loint(add);
	addr->qe_rcvlist_hi = hiint(add) & 077;
	qesetup(sc);
	qeissuesetup(sc, "qerestart");
	qestart(ifp->if_unit);
}

/*
 * Initialize a bdl ring, possibly allocating mbufs
 * on the way.
 */
initring(rp, mbp, cnt, option)
	register struct qe_ring *rp;
	register struct mbuf **mbp;
	register int cnt, option;
{
	struct qe_ring *orp = rp;
	long add;

	do {
		qeinitdesc(rp++, mbp++, option, NULL);
	} while (--cnt > 0);
	qeinitdesc(rp, mbp, QENOALLOC, NULL);
	add = startnet + (long)orp;
	rp->qe_addr_lo = loint(add);
	rp->qe_addr_hi = hiint(add) & 077;
	rp->qe_chain = 1;
	rp->qe_valid = 1;
}
 
/*
 * Issue a setup packet to the QNA and wait for it's completion. Report an
 * error if we can't immediately get the transmit ring entry to send the
 * setup packet or if the transmission fails.
 */
qeissuesetup(sc, cmdname)
	struct qe_softc *sc;
	char *cmdname;
{
	register struct qe_ring *rp = (struct qe_ring *)&sc->tring[sc->tindex];
	struct qedevice *addr = sc->addr;
	long add;

	if (rp->qe_valid == 0) {
		sc->tmbuf[sc->tindex] = 0;
		add = startnet + (long)setup_pkt;
		rp->qe_addr_lo = loint(add);
		rp->qe_addr_hi = hiint(add) & 077;
		rp->qe_buf_len = -64;
		rp->qe_setup = 1;
		rp->qe_eomsg = 1;
		rp->qe_valid = 1;

		if (addr->qe_csr & QE_XL_INVALID) {
			add = startnet + (long)rp;
			addr->qe_xmtlist_lo = loint(add);
			addr->qe_xmtlist_hi = hiint(add) & 077;
		}
		sc->tindex = ++sc->tindex % NXMT;
#ifdef OLDWAY
		while (rp->qe_status1 == QE_NOTYET);
#else
{
		long countdown;
		/* POSSIBLE HANGUP -- THIS IS UNTESTED -- KB */
tryagain:	for (countdown = 200000L;
		    countdown && rp->qe_status1 == QE_NOTYET; --countdown);
		if (rp->qe_status1 == QE_NOTYET) {
			printf("qe: HUNG!\n");
			addr->qe_csr = QE_RESET;
			addr->qe_csr = 0;
			addr->qe_csr = QE_RCV_ENABLE | QE_INT_ENABLE | QE_XMIT_INT |
			    QE_RCV_INT | QE_ILOOP;
			goto tryagain;
		}
}
#endif /* OLDWAY */
		/*
		 * Avoid an obscure race condition with the hardware continuing
		 * around the transmit ring and finding this setup packet again.
		 */
		rp->qe_setup = 0;
	}
	else
		printf("qe%d: %s failed: no ring entry\n",
		    sc->qe_if.if_unit, cmdname);
}

/*
 * Convert Ethernet address to printable (loggable) representation.
 */
char *
ether_sprintf(ap)
	register u_char *ap;
{
	register i;
	static char etherbuf[18];
	register char *cp = etherbuf;
	static char digits[] = "0123456789abcdef";

	for (i = 0; i < 6; i++) {
		*cp++ = digits[*ap >> 4];
		*cp++ = digits[*ap++ & 0xf];
		*cp++ = ':';
	}
	*--cp = 0;
	return (etherbuf);
}
#endif /* NQE */
