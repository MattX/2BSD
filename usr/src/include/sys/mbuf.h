/*	mbuf.h	4.13	82/06/14	*/

#if !pdp11

/*
 * Constants related to memory allocator.
 */
#define	MSIZE		128			/* size of an mbuf */
#define	MMINOFF		12			/* mbuf header length */
#define	MTAIL		4
#define	MMAXOFF		(MSIZE-MTAIL)		/* offset where data ends */
#define	MLEN		(MSIZE-MMINOFF-MTAIL)	/* mbuf data length */
#define	NMBCLUSTERS	256
#define	NMBPCL		(CLBYTES/MSIZE)		/* # mbufs per cluster */

/*
 * Macros for type conversion
 */

/* network cluster number to virtual address, and back */
#define	cltom(x) ((struct mbuf *)((int)mbutl + ((x) << CLSHIFT)))
#define	mtocl(x) (((int)x - (int)mbutl) >> CLSHIFT)

/* address in mbuf to mbuf head */
#define	dtom(x)		((struct mbuf *)((int)x & ~(MSIZE-1)))

/* mbuf head, to typed data */
#define	mtod(x,t)	((t)((int)(x) + (x)->m_off))

struct mbuf {
	struct	mbuf *m_next;		/* next buffer in chain */
	u_long	m_off;			/* offset of data */
	short	m_len;			/* amount of data in this mbuf */
	short	m_free;			/* is mbuf free? (consistency check) */
	u_char	m_dat[MLEN];		/* data storage */
	struct	mbuf *m_act;		/* link in higher-level mbuf list */
};

/* flags to m_get */
#define	M_DONTWAIT	0
#define	M_WAIT		1

/* flags to m_pgalloc */
#define	MPG_MBUFS	0		/* put new mbufs on free list */
#define	MPG_CLUSTERS	1		/* put new clusters on free list */
#define	MPG_SPACE	2		/* don't free; caller wants space */

/* length to m_copy to copy all */
#define	M_COPYALL	1000000000

#define	MGET(m, i) \
	{ int ms = splimp(); \
	  if ((m)=mfree) \
		{ if ((m)->m_free == 0) panic("mget"); (m)->m_free = 0; \
		  mbstat.m_mbfree--; mfree = (m)->m_next; (m)->m_next = 0; } \
	  else \
		(m) = m_more(i); \
	  splx(ms); }
#define	MCLGET(m, i) \
	{ int ms = splimp(); \
	  if ((m)=mclfree) \
	     {++mclrefcnt[mtocl(m)];mbstat.m_clfree--;mclfree = (m)->m_next;} \
	  splx(ms); }
#define	MFREE(m, n) \
	{ int ms = splimp(); \
	  if ((m)->m_free) panic("mfree"); (m)->m_free = 1; \
	  if ((m)->m_off >= MSIZE) { \
		(n) = (struct mbuf *)(mtod(m, int)&~0x3ff); \
		if (--mclrefcnt[mtocl(n)] == 0) \
		    { (n)->m_next = mclfree;mclfree = (n);mbstat.m_clfree++;} \
	  } \
	  (n) = (m)->m_next; (m)->m_next = mfree; \
	  (m)->m_off = 0; (m)->m_act = 0; mfree = (m); mbstat.m_mbfree++; \
	  splx(ms); }
#define MSGET(sp,s,clr) \
	{ struct mbuf *mb = m_get(M_DONTWAIT); \
	  if (mb) { \
		mb->m_off = MMINOFF;  sp = mtod(mb, s *); \
		if (clr) bzero(sp, sizeof(s)); \
	  } else sp = 0; }
#define MSFREE(sp) (void)m_free(dtom(sp))
#define MBCOPY(from,fromoff,to,tooff,count) \
	bcopy(mtod((from),caddr_t)+fromoff,mtod((to),caddr_t)+tooff,(u_int)count)

#define MAPSAVE()
#define MAPREST()
#define MAPSET(n)

#ifdef KERNEL
extern	struct mbuf mbutl[];		/* virtual address of net free mem */
extern	struct pte Mbmap[];		/* page tables to map Netutl */
extern	int nmbclusters;
struct	mbuf *mfree, *mclfree;
char	mclrefcnt[NMBCLUSTERS];
caddr_t	m_clalloc();
#endif KERNEL

#endif !pdp11

/*
 * Mbuf statistics.
 */
struct mbstat {
	short	m_mbufs;		/* mbufs obtained from page pool */
	short	m_mbfree;		/* mbufs on our free list */
	short	m_clusters;		/* clusters obtained from page pool */
	short	m_clfree;		/* free clusters */
	short	m_drops;		/* times failed to find space */
};

#ifdef	KERNEL
struct	mbstat mbstat;
struct	mbuf *m_get(),*m_getclr(),*m_free(),*m_more(),*m_copy(),*m_pullup();
#endif


#if pdp11

#ifndef	saveseg5
#include <sys/seg.h>
#endif

/*
 * Constants related to memory allocator.
 */
#define MSIZE           256                     /* size of an mbuf */
#define MMINOFF         4                       /* mbuf header length */
#define MTAIL           0
#define	MMAXOFF		(MSIZE-MTAIL)		/* offset where data ends */
#define	MLEN		(MSIZE-MMINOFF-MTAIL)	/* mbuf data length */
#define MBX             ((struct mbuf *)0120000) /* mbufx structure address */
#define CLBYTES         MSIZE                   /* never used */
#define	MBMAPSIZE	(((128-1)<<8)|RW)	/* 8 KB read/write */

/*
 * Macros for type conversion
 */

/* address in mbuf to mbuf head */
#define dtom(x) ( MBX->m_mbuf->m_click == *KDSA5 ? MBX->m_mbuf : panic("dtom") )

/* mbuf head, to typed data */
#define mtod(x,t) ((t)(mtodf(x)))

struct mbuf {                           /* in address space part of mbuf */
	struct	mbuf *m_next;		/* next buffer in chain */
	short   m_off;                  /* offset of data */
	short	m_len;			/* amount of data in this mbuf */
	short   m_click;                /* MMU address */
	struct	mbuf *m_act;		/* link in higher-level mbuf list */
};

struct mbufx {                          /* external part of mbuf */
	struct  mbuf *m_mbuf;           /* back pointer to mbuf */
	short   m_ref;                  /* reference counter */
	u_char	m_dat[MLEN];		/* data storage */
};

/* flags to m_get */
#define	M_DONTWAIT	0
#define	M_WAIT		1

/* length to m_copy to copy all */
#define M_COPYALL       077776

#define MGET(m, i)      (m) = m_bget()
#define MFREE(m, n)     (n) = m_bfree(m)
#define m_get(w)        m_bget()
#define m_free(m)       m_bfree(m)
#define MSGET(sp,s,clr) sp = (s *)m_sget(sizeof(s),clr)
#define	MSFREE(sp)	(void)m_sfree((caddr_t)sp)
#define MBCOPY(from,fromoff,to,tooff,count) \
	copyv(from->m_click,from->m_off+fromoff, \
		to->m_click,to->m_off+tooff,(u_int)count)
#define	MAPSAVE() {segm ZZMAP; saveseg5(ZZMAP)
#define	MAPREST()   restorseg5(ZZMAP); }
#define	MAPSET(n) mapseg5(n, MBMAPSIZE)

#ifdef KERNEL
struct mbuf *m_bget(),*m_bfree();
#endif KERNEL

#endif pdp11
 
