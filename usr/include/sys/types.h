typedef	char		bool_t;		/* Boolean */
typedef	char *     	caddr_t;  	/* virtual core address */
typedef	unsigned short	comp_t;		/* "floating pt": 3 bits base 8 exp, 13 bits fraction */
typedef	long       	daddr_t;  	/* disk address */
typedef	short        	dev_t;    	/* device code */
typedef	unsigned short	ino_t;     	/* i-node number */
#ifdef	MENLO_KOV
typedef	short		label_t[7];	/* regs 2-7 and __ovno */
#else
typedef	short        	label_t[6]; 	/* program status */
#endif
typedef unsigned	memaddr;	/* core or swap address */
typedef	long       	off_t;    	/* offset in file */
typedef	struct {short r[1];}*	physadr;
typedef unsigned	size_t;		/* size of process segments */
typedef	long       	time_t;   	/* a time */
typedef	long		ubadr_t;	/* unibus address */
typedef	unsigned short	u_short;
typedef	unsigned short	u_int;
typedef	long		u_long;		/* watch out! */
typedef	char		u_char;		/* watch out! */
typedef	short		void;		/* Embarassing crock for Ritchie C compiler */
#ifdef	UCB_NET
typedef struct  fd_set { long fds_bits[1]; } fd_set;
#endif

	/* selectors and constructor for device code */
#define	major(x)  	(short)(((unsigned)(x)>>8))
#define	minor(x)  	(short)((x)&0377)
#define	makedev(x,y)	(dev_t)((x)<<8|(y))

#ifdef  UCB_NET
/* stuff below will be moved to param.h */

/* misc net related data */

int     selwait;        /* wchan for select waits */
u_long  ntohl(),htonl();
u_short ntohs(),htons();
#define	insque(q,p)	_insque((caddr_t)q,(caddr_t)p)
#define	remque(q)	_remque((caddr_t)q)
#ifdef	KERNEL
#define DELAY(n) delay((long)n)
#endif
#define useracc(a,c,m) (1)
#define UBAPURGE(a,b)

/* conversion from VAX long identifiers to PDP (unique within 7 chars) */

#define INET 1
#define tcpstates tcpstas
#define sockaddr_in sock_in
#define sockaddr_pup sock_pup
#define spup_zero1 spup_z1
#define spup_zero2 spup_z2
#define if_ifwithaddr if_iwaddr
#define if_ifwithnet if_iwnet
#define if_ifwithaf if_iwaf
#define NBBY 8
#define NBPG 512
#define in_pcbdetach in_pcbetach
#define	SECDAY		((unsigned)(24*60*60))		/* seconds per day */
#define tcps_badsum tcps_bsum
#define tcps_badoff tcps_boff
#define tcps_badsegs tcps_bsegs
#define tcp_output tcp_oput
#define tcp_usrclosed tcp_uclosed
#define udps_badsum udps_bsum
#define udps_badlen udps_blen
#define ssocketaddr ssockad
#define soisconnecting soiscing
#define soisconnected soisced
#define soisdisconnecting soisding
#define soisdisconnected soisded
#define sbappendaddr sbappadd
#define protoswLAST protoLAST
#define rawintr rawint
#define tcp_initopt tcp_iopt
#define hostnamelen hostnlen
#define tcp_debx tcp_dbx
#define	gethostid gethstid
#ifndef	KERNEL
u_long	gethostid();
#endif
#define	sethostid sethstid
#endif
 
