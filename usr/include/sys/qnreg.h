/*
 * Interlan Ethernet Communications Controller interface
 */
struct qndevice {
	u_short	ad0;
	u_short ad1;
	u_short rbl;
	u_short rbh;
	u_short xbl;
	u_short xbh;
	u_short vec;
	u_short csr;
};

/*
 * Command and status bits
 */
#define	RI	0100000
#define	CA	0020000
#define	FOK	0010000
#define	SE	0002000
#define	EL	0001000
#define	IL	0000400
#define	XI	0000200
#define	IE	0000100
#define	RL	0000040
#define	XL	0000020
#define	BDROM	0000010
#define	NI	0000004
#define	SR	0000002
#define	RE	0000001

/* flag bits */
#define	BINIT	0100000
#define	VALID	0100000
#define	EOM	0020000
#define	SETUP	0010000
#define	LOWT	0000200

/* status bits */
#define	LASTNOT	0100000
#define	XERROR	0040000
#define	LOSS	0010000
#define	NOCAR	0004000
#define	STE16	0002000
#define	ABORT	0001000
#define	FAIL	0000400

#define	RERROR	0040000
#define	RESETUP	0020000
#define	DISCARD	0010000
#define	RUNT	0004000
#define	RBLH	0003400
#define	FRAME	0000004
#define	CRCERR	0000002
#define	OVF	0000001
#define	RBLL	0000377

/* parameters */
#define	RCVSIZ	1
