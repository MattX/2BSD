/*
 * One file structure is allocated for each
 * open/creat/pipe call.  Main use is to hold
 * the read/write pointer associated with each open file.
 */
struct	file
{
	char	f_flag;
	char	f_count;	/* reference count */
	struct inode *f_inode;	/* pointer to inode structure */
	union {
		off_t	f_offset;	/* read/write character pointer */
#ifdef  UCB_NET
		struct socket *f_Socket;
#endif
	} f_un;
};
#ifdef  UCB_NET
#define f_socket        f_un.f_Socket
#endif

#ifdef	KERNEL
extern struct file file[];	/* The file table itself */
#endif

/* flags */
#define	FREAD	01
#define	FWRITE	02
#define	FPIPE	04
#ifdef  UCB_NET
#define FSOCKET 040     /* descriptor of a socket */
#endif
#ifdef  RAND_XO
#define FXOPEN 0100     /* exclusive open */
#define FXOPENM 04      /* exclusive open bit in mode argument of open */
#endif

/* flags supplied to access call */
#define	FACCESS_EXISTS	0x0	/* does file exist? */
#define	FACCESS_EXECUTE	0x1	/* is it executable by caller? */
#define	FACCESS_WRITE	0x2	/* is it writable by caller? */
#define	FACCESS_READ	0x4	/* is it readable by caller? */

/* added for 4.2 compatibility */
#define	F_OK		0	/* does file exist */
#define	X_OK		1	/* is it executable by caller */
#define	W_OK		2	/* writable by caller */
#define	R_OK		4	/* readable by caller */

/* flags supplies to lseek call */
#define	FSEEK_ABSOLUTE	0x0	/* absolute offset */
#define	FSEEK_RELATIVE	0x1	/* relative to current offset */
#define	FSEEK_EOF	0x2	/* relative to end of file */

/* added for 4.2 compatibility */
#define	L_SET		0	/* absolute offset */
#define	L_INCR		1	/* relative to current offset */
#define	L_XTND		2	/* relative to end of file */

/* flags supplied to open call */
#define	FATT_RDONLY	0x0	/* open for reading only */
#define	FATT_WRONLY	0x1	/* open for writing only */
#define	FATT_RDWR	0x2	/* open for reading and writing */

/* added for 4.2 compatibility */
#define	O_RDONLY	000	/* open for reading */
#define	O_WRONLY	001	/* open for writing */
#define	O_RDWR		002	/* open for read & write */
