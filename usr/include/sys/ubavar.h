/* fake ubavar.h to initialize net device drivers */

struct uba_device {
	struct	uba_driver *ui_driver;
	short	ui_unit;	/* unit number on the system */
	short	ui_ubanum;	/* the uba it is on */
	caddr_t	ui_addr;	/* address of device in i/o space */
	u_long  ui_flags;       /* parameter from system specification */
	short	ui_alive;	/* device exists */
};

struct uba_driver {
	int	(*ud_probe)();		/* see if a driver is really there */
	int	(*ud_slave)();		/* see if a slave is there */
	int	(*ud_attach)();		/* setup driver for a slave */
	int	(*ud_dgo)();		/* fill csr/ba to start transfer */
	u_short	*ud_addr;		/* device csr addresses */
	char	*ud_dname;		/* name of a device */
	struct	uba_device **ud_dinfo;	/* backpointers to ubdinit structs */
	char	*ud_mname;		/* name of a controller */
	short	ud_xclu;		/* want exclusive use of bdp's */
};

/*
 * Flags to UBA map/bdp allocation routines
 */
#define	UBA_NEEDBDP	0x01		/* transfer needs a bdp */
#define	UBA_CANTWAIT	0x02		/* don't block me */
#define	UBA_NEED16	0x04		/* need 16 bit addresses only */
#define	UBA_HAVEBDP	0x08		/* use bdp specified in high bits */

#define UBAPURGE(uba, bdp)
#define	IF_UBAPURGE(uba, bdp)

ubadr_t	uballoc();
#define	ubarelse(a,b)
ubadr_t ubmalloc();
 
