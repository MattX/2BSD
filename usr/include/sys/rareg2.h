/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)rareg.h	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 * Combined the following files from 2.10BSD into rareg.h for 2.9BSD:
 * rareg.h, mscp.h, errno.h, dk.h
 *
 *	04.11.2011, Peter Klapper
 */

/*
 * RA disk controller device registers
*/
typedef	struct	radevice	{
	short	raip;		/* init and polling */
	short	rasa;		/* status and addresses */
} radeviceT;

#define RA_ERR		0100000
#define RA_STEP4	0040000
#define RA_STEP3	0020000
#define RA_STEP2	0010000
#define RA_STEP1	0004000
#define RA_NV		0002000
#define RA_QB		0001000
#define RA_DI		0000400
#define RA_IE		0000200
#define RA_PI		0000001
#define RA_GO		0000001

#define RA_VECTOR	0154

/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)mscp.h	1.1 (2.10BSD Berkeley) 12/1/86
 */

/*
 * Definitions for the Mass Storage Control Protocol
 *
 *	Unused mscp definitions are commented out because of the
 *	limited symbol table size in the V7 C compiler,
 *	`too many defines'.
 *	Lots of strangeness in mscp packet definitions, i.e.,
 *	dealing with longs as hi & lo words. This is due to the
 *	way the PDP-11 stores a long, i.e., high order word
 *	in low memory word and low order word in high memory word.
 *	The UDA does not like that at all !
 *
 *	Fred Canter 10/22/83
*/

/*
 * Some missing prototypes in 2.9 BSD
 *
 *	Peter Klapper, 14.10.2021
 */
typedef char 			u_char; /* watch out! no unisigned chars! */
typedef unsigned short		ushort;	/* sys III compat */
typedef struct { long val[2]; }	quad;

/*
 * Control message opcodes
 */
#define	M_O_ABORT	0001	/* Abort command */
#define	M_O_GTCMD	0002	/* Get command status command */
#define	M_O_GTUNT	0003	/* Get unit status command */
#define	M_O_STCON	0004	/* Set controller characteristics command */
#define	M_O_SEREX	0007	/* Serious exception end message */
#define	M_O_AVAIL	0010	/* Available command */
#define	M_O_ONLIN	0011	/* Online command */
#define	M_O_STUNT	0012	/* Set unit characteristics command */
#define	M_O_DTACP	0013	/* Determine access paths command */
#define	M_O_ACCES	0020	/* Access command */
#define	M_O_CMPCD	0021	/* Compare controller data command */
#define	M_O_ERASE	0022	/* Erase command */
#define	M_O_FLUSH	0023	/* Flush command */
#define	M_O_REPLC	0024	/* Replace command */
#define	M_O_COMP	0040	/* Compare host data command */
#define	M_O_READ	0041	/* Read command */
#define	M_O_WRITE	0042	/* Write command */
#define	M_O_AVATN	0100	/* Available attention message */
#define	M_O_DUPUN	0101	/* Duplicate unit number attention message */
#define	M_O_ACPTH	0102	/* Access path attention message */
#define	M_O_END		0200	/* End message flag */

/*
 * End message flags
*/
#define	M_E_BBLR	0200	/* Bad block reported */
#define	M_E_BBLU	0100	/* Bad block unreported */
#define	M_E_ERLOG	0040	/* Error log generated */
#define	M_E_SEREX	0020	/* Serious exception */


/*
 * Controller flags
*/
#define	M_C_ATTN	0200	/* Enable attention messages */
#define	M_C_MISC	0100	/* Enable miscellaneous error log messages */
#define	M_C_OTHER	0040	/* Enable other host's error log messages */
#define	M_C_THIS	0020	/* Enable this host's error log messages */
#define	M_C_MLTHS	0004	/* Multi-host */
#define	M_C_SHADW	0002	/* Shadowing */
#define	M_C_576		0001	/* 576 byte sectors */

/*
 * Status codes
*/
#define	M_S_MASK	037		/* Status code mask */
#define	M_S_SUCC	000		/* Success */
#define	M_S_ICMD	001		/* Invalid command */
#define	M_S_ABRTD	002		/* Command aborted */
#define	M_S_OFFLN	003		/* Unit offline */
#define	M_S_AVLBL	004		/* Unit available */
#define	M_S_MFMTE	005		/* Media format error */
#define	M_S_WRTPR	006		/* Write protected */
#define	M_S_COMP	007		/* Compare error */
#define	M_S_DATA	010		/* Data error */
#define	M_S_HSTBF	011		/* Host buffer access error */
#define	M_S_CNTLR	012		/* Controller error */
#define	M_S_DRIVE	013		/* Drive error */
#define	M_S_DIAG	037		/* Message from a internal diagnostic */

/*
 * MSCP packet info
*/

struct mscp_header {
	u_short	ra_msglen;	/* length of MSCP packet */
	char	ra_credits;	/* low 4 bits: credits, high 4 bits: msgtype */
	char	ra_vcid;	/* virtual circuit id */
};

/*
 * An MSCP packet
 */

struct mscp {
	struct	mscp_header m_header;	/* device specific header */
	u_short	m_cmdref;		/* command reference number */
	u_short	m_elref;		/* plus error log reference number */
	u_short	m_unit;			/* unit number */
	u_short	m_xxx1;			/* unused */
	u_char	m_opcode;		/* opcode */
	u_char	m_flags;		/* end message flags */
	u_short	m_modifier;		/* modifiers */
	union {
	struct {
		u_short	Ms_bytecnt;	/* byte count */
		u_short	Ms_zzz2;	/* 64kb max on V7 */
		u_short	Ms_buf_l;	/* buffer descriptor low word */
		u_short	Ms_buf_h;	/* buffer descriptor hi  word */
		long	Ms_xx2[2];	/* unused */
		u_short	Ms_lbn_l;	/* logical block number low word */
		u_short	Ms_lbn_h;	/* logical bhock number hi  word */
		long	Ms_xx4;		/* unused */
		long	*Ms_dscptr;	/* pointer to descriptor (software) */
		long	Ms_sftwds[4];	/* software words, padding */
	} m_generic;
	struct {
		u_short	Ms_version;	/* MSCP version */
		u_short	Ms_cntflgs;	/* controller flags */
		u_short	Ms_hsttmo;	/* host timeout */
		u_short	Ms_usefrac;	/* use fraction */
		long	Ms_time;	/* time and date */
	} m_setcntchar;
	struct {
		u_short	Ms_multunt;	/* multi-unit code */
		u_short	Ms_unitflgs;	/* unit flags */
		long	Ms_hostid;	/* host identifier */
		quad	Ms_unitid;	/* unit identifier */
		long	Ms_mediaid;	/* media type identifier */
		u_short	Ms_shdwunt;	/* shadow unit */
		u_short	Ms_shdwsts;	/* shadow status */
		u_short Ms_track;	/* track size */
		u_short	Ms_group;	/* group size */
		u_short	Ms_cylinder;	/* cylinder size */
		u_short	Ms_xx3;		/* reserved */
		u_short	Ms_rctsize;	/* RCT table size */
		char	Ms_rbns;	/* RBNs / track */
		char	Ms_rctcpys;	/* RCT copies */
	} m_getunitsts;
	} m_un;
	int	m_msgpad[3];		/* pad msg length to 64 bytes */
					/* required by UQ bus port spec */
};

/*
 * generic packet
 */

#define	m_zzz2		m_un.m_generic.Ms_zzz2
#define	m_bytecnt	m_un.m_generic.Ms_bytecnt
#define	m_buf_l		m_un.m_generic.Ms_buf_l
#define	m_buf_h		m_un.m_generic.Ms_buf_h
#define	m_lbn_l		m_un.m_generic.Ms_lbn_l
#define	m_lbn_h		m_un.m_generic.Ms_lbn_h
#define	m_dscptr	m_un.m_generic.Ms_dscptr
#define	m_sftwds	m_un.m_generic.Ms_sftwds
#define	m_status	m_modifier

/*
 * Abort / Get Command Status packet
 */

#define	m_outref	m_bytecnt

/*
 * Online / Set Unit Characteristics packet
 */

#define m_elgfll	m_lbn_l
#define m_elgflh	m_lbn_h
#define	m_copyspd	m_shdwsts

/*
 * Replace packet
 */

#define	m_rbn	m_bytecnt

/*
 * Set Controller Characteristics packet
 */

#define	m_version	m_un.m_setcntchar.Ms_version
#define	m_cntflgs	m_un.m_setcntchar.Ms_cntflgs
#define	m_hsttmo	m_un.m_setcntchar.Ms_hsttmo
#define	m_usefrac	m_un.m_setcntchar.Ms_usefrac
#define	m_time		m_un.m_setcntchar.Ms_time

/*
 * Get Unit Status end packet
 */

#define	m_multunt	m_un.m_getunitsts.Ms_multunt
#define	m_unitflgs	m_un.m_getunitsts.Ms_unitflgs
#define	m_hostid	m_un.m_getunitsts.Ms_hostid
#define	m_unitid	m_un.m_getunitsts.Ms_unitid
#define	m_mediaid	m_un.m_getunitsts.Ms_mediaid
#define	m_shdwunt	m_un.m_getunitsts.Ms_shdwunt
#define	m_shdwsts	m_un.m_getunitsts.Ms_shdwsts
#define	m_track		m_un.m_getunitsts.Ms_track
#define	m_group		m_un.m_getunitsts.Ms_group
#define	m_cylinder	m_un.m_getunitsts.Ms_cylinder
#define	m_rctsize	m_un.m_getunitsts.Ms_rctsize
#define	m_rbns		m_un.m_getunitsts.Ms_rbns
#define	m_rctcpys	m_un.m_getunitsts.Ms_rctcpys

/*
 * Online / Set Unit Characteristics end packet
 */

#define	m_uslow		m_un.m_getunitsts.Ms_track
#define	m_ushigh	m_un.m_getunitsts.Ms_group
#define	m_volser	m_sftwds[0]

/*
 * Set Controller Characteristics end packet
 */

#define	m_cnttmo	m_hsttmo
#define	m_cntcmdl	m_usefrac
#define	m_cntid		m_unitid


/*
 * Error Log message format codes
 */
#define	M_F_CNTERR	0	/* Controller error */
#define	M_F_BUSADDR	1	/* Host memory access error */
#define	M_F_DISKTRN	2	/* Disk transfer error */
#define	M_F_SDI		3	/* SDI error */
#define	M_F_SMLDSK	4	/* Small disk error */

/*
 * Error Log message flags
 */
#define	M_LF_SUCC	0200	/* Operation successful */
#define	M_LF_CONT	0100	/* Operation continuing */
#define	M_LF_SQNRS	0001	/* Sequence number reset */

/*
 * MSCP Error Log packet
 *
 *	NOTE: MSCP packet must be padded to this size.
 */

struct mslg {
	struct	mscp_header me_header;	/* device specific header */
	int	me_cmdref;		/* command reference number */
	int	me_elref;		/* error log reference number */
	short	me_unit;		/* unit number */
	short	me_seqnum;		/* sequence number */
	u_char	me_format;		/* format */
	u_char	me_flags;		/* error log message flags */
	short	me_event;		/* event code */
	quad	me_cntid;		/* controller id */
	u_char	me_cntsvr;		/* controller software version */
	u_char	me_cnthvr;		/* controller hardware version */
	short	me_multunt;		/* multi-unit code */
	quad	me_unitid;		/* unit id */
	u_char	me_unitsvr;		/* unit software version */
	u_char	me_unithvr;		/* unit hardware version */
	short	me_group;		/* group; retry + level */
	int	me_volser[2];		/* volume serial number */
	int	me_hdr[2];		/* header */
	char	me_sdistat[12];		/* SDI status information */
};

#define	me_busaddr	me_unitid
#define	me_sdecyl	me_group

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)errno.h	7.1 (Berkeley) 6/4/86
 */

/*
 * Error codes
 */

#define	EPERM		1		/* Not owner */
#define	ENOENT		2		/* No such file or directory */
#define	ESRCH		3		/* No such process */
#define	EINTR		4		/* Interrupted system call */
#define	EIO		5		/* I/O error */
#define	ENXIO		6		/* No such device or address */
#define	E2BIG		7		/* Arg list too long */
#define	ENOEXEC		8		/* Exec format error */
#define	EBADF		9		/* Bad file number */
#define	ECHILD		10		/* No children */
#define	EAGAIN		11		/* No more processes */
#define	ENOMEM		12		/* Not enough core */
#define	EACCES		13		/* Permission denied */
#define	EFAULT		14		/* Bad address */
#define	ENOTBLK		15		/* Block device required */
#define	EBUSY		16		/* Mount device busy */
#define	EEXIST		17		/* File exists */
#define	EXDEV		18		/* Cross-device link */
#define	ENODEV		19		/* No such device */
#define	ENOTDIR		20		/* Not a directory*/
#define	EISDIR		21		/* Is a directory */
#define	EINVAL		22		/* Invalid argument */
#define	ENFILE		23		/* File table overflow */
#define	EMFILE		24		/* Too many open files */
#define	ENOTTY		25		/* Not a typewriter */
#define	ETXTBSY		26		/* Text file busy */
#define	EFBIG		27		/* File too large */
#define	ENOSPC		28		/* No space left on device */
#define	ESPIPE		29		/* Illegal seek */
#define	EROFS		30		/* Read-only file system */
#define	EMLINK		31		/* Too many links */
#define	EPIPE		32		/* Broken pipe */

/* math software */
#define	EDOM		33		/* Argument too large */
#define	ERANGE		34		/* Result too large */

/* non-blocking and interrupt i/o */
#define	EWOULDBLOCK	35		/* Operation would block */
#define	EDEADLK		EWOULDBLOCK	/* ditto */
#define	EINPROGRESS	36		/* Operation now in progress */
#define	EALREADY	37		/* Operation already in progress */

/* ipc/network software */

	/* argument errors */
#define	ENOTSOCK	38		/* Socket operation on non-socket */
#define	EDESTADDRREQ	39		/* Destination address required */
#define	EMSGSIZE	40		/* Message too long */
#define	EPROTOTYPE	41		/* Protocol wrong type for socket */
#define	ENOPROTOOPT	42		/* Protocol not available */
#define	EPROTONOSUPPORT	43		/* Protocol not supported */
#define	ESOCKTNOSUPPORT	44		/* Socket type not supported */
#define	EOPNOTSUPP	45		/* Operation not supported on socket */
#define	EPFNOSUPPORT	46		/* Protocol family not supported */
#define	EAFNOSUPPORT	47		/* Address family not supported by protocol family */
#define	EADDRINUSE	48		/* Address already in use */
#define	EADDRNOTAVAIL	49		/* Can't assign requested address */

	/* operational errors */
#define	ENETDOWN	50		/* Network is down */
#define	ENETUNREACH	51		/* Network is unreachable */
#define	ENETRESET	52		/* Network dropped connection on reset */
#define	ECONNABORTED	53		/* Software caused connection abort */
#define	ECONNRESET	54		/* Connection reset by peer */
#define	ENOBUFS		55		/* No buffer space available */
#define	EISCONN		56		/* Socket is already connected */
#define	ENOTCONN	57		/* Socket is not connected */
#define	ESHUTDOWN	58		/* Can't send after socket shutdown */
#define	ETOOMANYREFS	59		/* Too many references: can't splice */
#define	ETIMEDOUT	60		/* Connection timed out */
#define	ECONNREFUSED	61		/* Connection refused */

	/* */
#define	ELOOP		62		/* Too many levels of symbolic links */
#define	ENAMETOOLONG	63		/* File name too long */

/* should be rearranged */
#define	EHOSTDOWN	64		/* Host is down */
#define	EHOSTUNREACH	65		/* No route to host */
#define	ENOTEMPTY	66		/* Directory not empty */

/* quotas & mush */
#define	EPROCLIM	67		/* Too many processes */
#define	EUSERS		68		/* Too many users */
#define	EDQUOT		69		/* Disc quota exceeded */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)dk.h	7.1 (Berkeley) 6/4/86
 */

/*
 * Instrumentation
 */
#define	CPUSTATES	4

#define	CP_USER		0
#define	CP_NICE		1
#define	CP_SYS		2
#define	CP_IDLE		3

#define	DK_NDRIVE	5

#if defined(KERNEL) && defined(UCB_METER)
long	cp_time[CPUSTATES];	/* number of ticks spent in each cpu state */
int	dk_ndrive;		/* number of drives being monitored */
int	dk_busy;		/* bit array of drive busy flags */
long	dk_time[DK_NDRIVE];	/* ticks spent with drive busy */
long	dk_seek[DK_NDRIVE];	/* number of seeks */
long	dk_xfer[DK_NDRIVE];	/* number of transfers */
long	dk_wds[DK_NDRIVE];	/* number of clicks transfered */
long	dk_wps[DK_NDRIVE];	/* words per second */
char	*dk_name[DK_NDRIVE];	/* names of monitored drives */
int	dk_unit[DK_NDRIVE];	/* unit numbers of monitored drives */
int	dk_n;			/* number of dk numbers assigned so far */

long	tk_nin;			/* number of tty characters input */
long	tk_nout;		/* number of tty characters output */
#endif

