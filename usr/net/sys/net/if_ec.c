/*	if_ec.c	4.22	82/07/21	*/

#include "ec.h"

#if	NEC > 0
/*
 * 3Com Ethernet Controller interface
 * WARNING: This driver is specific to the pdp11 implementation.
 * Be sure to set the parameters in ecreg.h correctly.
 */

#include "param.h"
#include <sys/systm.h>
#include <sys/mbuf.h>
#include <sys/buf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/ubavar.h>
#include <sys/ecreg.h>
#include <sys/ioctl.h>
#include "../net/in.h"
#include "../net/in_systm.h"
#include "../net/if.h"
#include "../net/if_ec.h"
#include "../net/if_uba.h"
#include "../net/ip.h"
#include "../net/ip_var.h"
#include "../net/pup.h"
#include "../net/route.h"
#include <errno.h>

#define	ECMTU	1500
#define	ECMIN	(60-14)
#define	ECMEM	3840	/* The memory start location in clicks */
#define	RW2K	((31<<8)|RW)	/* Read/write 2K segment */

int	ecprobe(), ecattach(), ecrint(), ecxint(), eccol();
struct	uba_device *ecinfo[NEC];
u_short ecstd[] = { 0 };
struct	uba_driver ecdriver =
	{ ecprobe, 0, ecattach, 0, ecstd, "ec", ecinfo };
u_char	ec_iltop[3] = { 0x02, 0x07, 0x01 };
#define	ECUNIT(x)	minor(x)

int	ecinit(),ecoutput(),ecreset(),ecioctl();
struct mbuf *ecget();

extern struct ifnet loif;

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * es_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface, its address, ...
 * We also have, for each interface, a UBA interface structure, which
 * contains information about the UNIBUS resources held by the interface:
 * map registers, buffered data paths, etc.  Information is cached in this
 * structure for use by the if_uba.c routines in running the interface
 * efficiently.
 */
struct	ec_softc {
	struct	arpcom	es_ac;		/* ARP struct. */
#define	es_if	es_ac.ac_if
#define	es_addr	es_ac.ac_enaddr
	struct	ifuba es_ifuba;		/* UNIBUS resources */
	short	es_mask;		/* mask for current output delay */
	short	es_oactive;		/* is output active? */
} ec_softc[NEC];

/*
 * Do output DMA to determine interface presence and
 * interrupt vector.  DMA is too short to disturb other hosts.
 */
ecprobe(reg)
	caddr_t reg;
{
	return (1);
}

/*
 * Interface exists: make available by filling in network interface
 * record.  System will initialize the interface when it is ready
 * to accept packets.
 */
ecattach(ui)
	struct uba_device *ui;
{
	struct ec_softc *es = &ec_softc[ui->ui_unit];
	register struct ifnet *ifp = &es->es_if;
	register struct ecdevice *addr = (struct ecdevice *)ui->ui_addr;
	struct sockaddr_in *sin;
	int i, j;
	u_char *cp;

	ifp->if_unit = ui->ui_unit;
	ifp->if_name = "ec";
	ifp->if_mtu = ECMTU;

	/*
	 * Read the ethernet address off the board, one nibble at a time.
	 */
	addr->ec_xcr = EC_UECLR;
	addr->ec_rcr = EC_AROM;
	cp = es->es_addr;
#define	NEXTBIT	addr->ec_rcr = EC_AROM|EC_ASTEP; addr->ec_rcr = EC_AROM
	for (i=0; i<6; i++) {
		*cp = 0;
		for (j=0; j<=4; j+=4) {
			*cp |= ((addr->ec_rcr>>8)&0xf)<<j;
			NEXTBIT; NEXTBIT; NEXTBIT; NEXTBIT;
		}
		cp++;
	}
	printf("ec%d: addr=%x:%x:%x:%x:%x:%x\n", ui->ui_unit,
		es->es_addr[0]&0xff, es->es_addr[1]&0xff,
		es->es_addr[2]&0xff, es->es_addr[3]&0xff,
		es->es_addr[4]&0xff, es->es_addr[5]&0xff);
	sin = (struct sockaddr_in *)&ifp->if_addr;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = htonl(ui->ui_flags);
	ifp->if_net = in_netof(sin->sin_addr);
	ifp->if_host[0] = in_lnaof(sin->sin_addr);
	sin = (struct sockaddr_in *)&ifp->if_broadaddr;
	sin->sin_family = AF_INET;
	sin->sin_addr = if_makeaddr(ifp->if_net, INADDR_ANY);
	ifp->if_flags |= IFF_BROADCAST;

	ifp->if_init = ecinit;
	ifp->if_output = ecoutput;
	ifp->if_ioctl = ecioctl;
	ifp->if_ubareset = ecreset;
	if_attach(ifp);
}

/*
 * Reset of interface after UNIBUS reset.
 * If interface is on specified uba, reset its state.
 */
ecreset(unit, uban)
	int unit, uban;
{
}

/*
 * Initialization of interface; clear recorded pending
 * operations, and reinitialize UNIBUS usage.
 */
ecinit(unit)
	int unit;
{
	register struct ec_softc *es = &ec_softc[unit];
	register struct ecdevice *addr;
	register struct ifnet *ifp = &es->es_if;
	struct sockaddr_in *sin = (struct sockaddr_in *)&ifp->if_addr;
	int i, s;

	if (sin->sin_addr.s_addr == 0)
		return;
	/*
	 * Hang receive buffers and start any pending writes.
	 * Writing into the rcr also makes sure the memory
	 * is turned on.
	 */
	if ((es->es_if.if_flags & IFF_RUNNING) == 0) {
		addr = (struct ecdevice *)ecinfo[unit]->ui_addr;
		s = splimp();
		for (i=ECRHBF; i>=ECRLBF; i--)
			addr->ec_rcr = EC_READ|i;
		es->es_oactive = 0;
		es->es_mask = ~0;
		es->es_if.if_flags |= IFF_UP|IFF_RUNNING;
		if (es->es_if.if_snd.ifq_head)
			ecstart(unit);
		splx(s);
	}
	if_rtinit(&es->es_if, RTF_UP);
	arpattach(&es->es_ac);
	arpwhohas(&es->es_ac, &sin->sin_addr);
}

/*
 * Start or restart output on interface.
 * If interface is already active, then this is a retransmit
 * after a collision, and just restuff registers.
 * If interface is not already active, get another datagram
 * to send off of the interface queue, and map it to the interface
 * before starting the output.
 */
ecstart(dev)
	dev_t dev;
{
        int unit = ECUNIT(dev);
	struct ec_softc *es = &ec_softc[unit];
	struct ecdevice *addr;
	struct mbuf *m;

	if (es->es_oactive)
		goto restart;

	IF_DEQUEUE(&es->es_if.if_snd, m);
	if (m == 0) {
		es->es_oactive = 0;
		return;
	}
	ecput(ECTBF, m, &es->es_ifuba);

restart:
	addr = (struct ecdevice *)ecinfo[unit]->ui_addr;
	addr->ec_xcr = EC_WRITE|ECTBF;
	es->es_oactive = 1;
}

/*
 * Ethernet interface transmitter interrupt.
 * Start another output if more data to send.
 */
ecxint(unit)
	int unit;
{
	register struct ec_softc *es = &ec_softc[unit];
	register struct ecdevice *addr =
		(struct ecdevice *)ecinfo[unit]->ui_addr;
	mapinfo save;

	if (es->es_oactive == 0)
		return;
	Savemap(save);
	if ((addr->ec_xcr&EC_XDONE) == 0 || (addr->ec_xcr&EC_XBN) != ECTBF) {
		printf("ec%d:str int,xcr==%b\n", unit,
			addr->ec_xcr, EC_XBITS);
		es->es_oactive = 0;
		addr->ec_xcr = EC_XCLR;
		return;
	}
	es->es_if.if_opackets++;
	es->es_oactive = 0;
	es->es_mask = ~0;
	addr->ec_xcr = EC_XCLR;
	if (es->es_if.if_snd.ifq_head)
		ecstart(unit);
	Restormap(save);
}

/*
 * Collision on ethernet interface.  Do exponential
 * backoff, and retransmit.  If have backed off all
 * the way print warning diagnostic, and drop packet.
 */
eccol(unit)
	int unit;
{
	struct ec_softc *es = &ec_softc[unit];
	mapinfo save;

	Savemap(save);
	es->es_if.if_collisions++;
	if (es->es_oactive)
		ecdocoll(unit);
	Restormap(save);
}

ecdocoll(unit)
	int unit;
{
	register struct ec_softc *es = &ec_softc[unit];
	register struct ecdevice *addr =
	    (struct ecdevice *)ecinfo[unit]->ui_addr;
	register i;
	int delay;

	/*
	 * Es_mask is a 16 bit number with n low zero bits, with
	 * n the number of backoffs.  When es_mask is 0 we have
	 * backed off 16 times, and give up.
	 */
	if (es->es_mask == 0) {
		es->es_if.if_oerrors++;
		printf("ec%d:ser\n", unit);
		/*
		 * Reset interface, then requeue rcv buffers.
		 * Some incoming packets may be lost, but that
		 * can't be helped.
		 */
		addr->ec_xcr = EC_UECLR;
		for (i=ECRHBF; i>=ECRLBF; i--)
			addr->ec_rcr = EC_READ|i;
		/*
		 * Reset and transmit next packet (if any).
		 */
		es->es_oactive = 0;
		es->es_mask = ~0;
		if (es->es_if.if_snd.ifq_head)
			ecstart(unit);
		return;
	}
	/*
	 * Do exponential backoff.  Compute delay based on low bits
	 * of the interval timer.  Then delay for that number of
	 * slot times.  A slot time is 51.2 microseconds (rounded to 51).
	 * This does not take into account the time already used to
	 * process the interrupt.
	 */
	es->es_mask <<= 1;
	delay = ((lbolt &~ es->es_mask)*51)/3;
	for (i = delay; i>0; i--)
		;
	/*
	 * Clear the controller's collision flag, thus enabling retransmit.
	 */
	addr->ec_xcr = EC_CLEAR;
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
ecrint(unit)
	int unit;
{
	struct ecdevice *addr = (struct ecdevice *)ecinfo[unit]->ui_addr;
	mapinfo save;

	Savemap(save);
	while (addr->ec_rcr & EC_RDONE)
		ecread(unit);
	Restormap(save);
}

ecread(unit)
	int unit;
{
	register struct ec_softc *es = &ec_softc[unit];
	struct ecdevice *addr = (struct ecdevice *)ecinfo[unit]->ui_addr;
	register struct ec_header *ec;
    	struct mbuf *m;
	int len, off, resid, ecoff, buf;
	register struct ifqueue *inq;
	caddr_t ecbuf;

	es->es_if.if_ipackets++;
	buf = addr->ec_rcr & EC_RBN;
	if (buf < ECRLBF || buf > ECRHBF)
		panic("ecrint");
	mapseg5(ECMEM+(buf-ECFBF)*32, RW2K);
	ecbuf = SEG5;
	ecoff = *(short *)ecbuf;
	if (ecoff <= ECRDOFF || ecoff > 2046) {
		es->es_if.if_ierrors++;
#ifdef notdef
		if (es->es_if.if_ierrors % 100 == 0)
			printf("ec%d: += 100 input errors\n", unit);
#endif
		goto setup;
	}

	/*
	 * Get input data length.
	 * Get pointer to ethernet header (in input buffer).
	 * Deal with trailer protocol: if type is PUP trailer
	 * get true type from first 16-bit word past data.
	 * Remember that type was trailer by setting off.
	 */
	len = ecoff - ECRDOFF - sizeof (struct ec_header);
	ec = (struct ec_header *)(ecbuf + ECRDOFF);
#define	ecdataaddr(ec, off, type)	((type)(((caddr_t)((ec)+1)+(off))))
	ec->ec_type = ntohs(ec->ec_type);
	if (ec->ec_type >= ECPUP_TRAIL &&
	    ec->ec_type < ECPUP_TRAIL+ECPUP_NTRAILER) {
		off = (ec->ec_type - ECPUP_TRAIL) * 512;
		if (off >= ECMTU)
			goto setup;		/* sanity */
		ec->ec_type = ntohs(*ecdataaddr(ec, off, u_short *));
		resid = ntohs(*(ecdataaddr(ec, off+2, u_short *)));
		if (off + resid > len)
			goto setup;		/* sanity */
		len = off + resid;
	} else
		off = 0;
	if (len == 0)
		goto setup;

	/*
	 * Pull packet off interface.  Off is nonzero if packet
	 * has trailing header; ecget will then force this header
	 * information to be at the front, but we still have to drop
	 * the type and length which are at the front of any trailer data.
	 */
	m = ecget(buf, len, off, &es->es_ifuba);
	if (m == 0)
		goto setup;
	if (off) {
		m->m_off += 2 * sizeof (u_short);
		m->m_len -= 2 * sizeof (u_short);
	}
	switch (ec->ec_type) {

#ifdef INET
	case ECPUP_IPTYPE:
		schednetisr(NETISR_IP);
		inq = &ipintrq;
		break;
	case ECPUP_ARPTYPE:
		arpinput(&es->es_ac, m);
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

setup:
	/*
	 * Reset for next packet.
	 */
	addr->ec_rcr = EC_READ|EC_RCLR|buf;
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
ecoutput(ifp, m0, dst)
	struct ifnet *ifp;
	struct mbuf *m0;
	struct sockaddr *dst;
{
	int error, s;
	u_short type;
	struct in_addr dest;
	register struct ec_softc *es = &ec_softc[ifp->if_unit];
	register struct mbuf *m = m0;
	register struct ec_header *ec;
	register int off, i;
	struct mbuf *mcopy = (struct mbuf *) 0;		/* Null */
	u_char edst[6];
	segm save;

	saveseg5(save);
	dest = ((struct sockaddr_in *)dst)->sin_addr;
	switch (dst->sa_family) {

#ifdef INET
	case AF_INET:
		if (!arpresolve(&es->es_ac, m, &dest, edst))
			return(0);
		/* If broadcast, send a copy to ourselves using loopback */
		if (in_lnaof(dest) == INADDR_ANY)
			mcopy = m_copy(m, 0, M_COPYALL);
		off = ntohs((u_short)mtod(m, struct ip *)->ip_len) - m->m_len;
		if ((ifp->if_flags & IFF_NOTRAILERS) == 0)
		if (off > 0 && (off & 0x1ff) == 0 &&
		    m->m_off >= MMINOFF + 2 * sizeof (u_short)) {
			type = ECPUP_TRAIL + (off>>9);
			m->m_off -= 2 * sizeof (u_short);
			m->m_len += 2 * sizeof (u_short);
			*mtod(m, u_short *) = htons(ECPUP_IPTYPE);
			*(mtod(m, u_short *) + 1) = htons(m->m_len);
			goto gottrailertype;
		}
		type = ECPUP_IPTYPE;
		off = 0;
		goto gottype;
#endif

	case AF_UNSPEC:
		ec = (struct ec_header *)dst->sa_data;
		bcopy((caddr_t)ec->ec_dhost, (caddr_t)edst, sizeof (edst));
		type = ec->ec_type;
		goto gottype;
	default:
		printf("ec%d:chd af%d\n", ifp->if_unit,
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
	    MMINOFF + sizeof (struct ec_header) > m->m_off) {
		m = m_get(M_DONTWAIT);
		if (m == 0) {
			error = ENOBUFS;
			goto bad;
		}
		m->m_next = m0;
		m->m_off = MMINOFF;
		m->m_len = sizeof (struct ec_header);
	} else {
		m->m_off -= sizeof (struct ec_header);
		m->m_len += sizeof (struct ec_header);
	}
	ec = mtod(m, struct ec_header *);
	for (i=0; i<6; i++) {
		ec->ec_shost[i] = es->es_addr[i];
		ec->ec_dhost[i] = edst[i];
	}
	ec->ec_type = htons((u_short)type);
	restorseg5(save);

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
	if (es->es_oactive == 0)
		ecstart(ifp->if_unit);
	splx(s);

gotlocal:
	return(mcopy ? looutput(&loif, mcopy, dst) : 0);

qfull:
	m0 = m;
	splx(s);
bad:
	m_freem(m0);
	return(error);
}

/*
 * Routine to copy from mbuf chain to transmitter
 * buffer in UNIBUS memory.
 */
ecput(buf, m, ifu)
	int buf;
	struct mbuf *m;
	struct ifuba *ifu;
{
	register struct mbuf *mp;
	register int off;
	u_char *ecbuf = SEG5;
	segm save;

	for (off = 2048, mp = m; mp; mp = mp->m_next)
		off -= mp->m_len;
	if (2048-off < ECMIN+sizeof(struct ec_header))
		off = 2048-ECMIN-sizeof(struct ec_header);
	saveseg5(save);
	mapseg5(ECMEM+(buf-ECFBF)*32, RW2K);
	*(u_short *)ecbuf = off;
	restorseg5(save);
	ifu->ifu_w.ifrw_click = ECMEM+(buf-ECFBF)*32;
	if_wubaput(ifu, m, off);
}

/*
 * Routine to copy from UNIBUS memory into mbufs.
 * Calls if_rubaget to do most of the work.
 *
 * Warning: This makes the fairly safe assumption that
 * mbufs have even lengths.
 */
struct mbuf *
ecget(buf, totlen, off0, ifu)
	int buf;
	int totlen, off0;
	register struct ifuba *ifu;
{

	ifu->ifu_hlen = ECRDOFF + sizeof (struct ec_header);
	ifu->ifu_r.ifrw_click = ECMEM+(buf-ECFBF)*32;
	return(if_rubaget(ifu, totlen, off0));
}

/*
 * Process an ioctl request.
 */
ecioctl(ifp, cmd, data)
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
		ecsetaddr(ifp, (struct sockaddr_in *)&ifr->ifr_addr);
		ecinit(ifp->if_unit);
		break;

	default:
		error = EINVAL;
	}
	splx(s);
	return (error);
}

ecsetaddr(ifp, sin)
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
#endif
