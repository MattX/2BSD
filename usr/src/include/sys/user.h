#include	<errno.h>
#include	<sys/fperr.h>

/*
 * The user structure.  One allocated per process.  Contains all per process
 * data that doesn't need to be referenced while the process is swapped.  The
 * user block is USIZE*64 bytes long; resides at virtual kernel loc 0140000;
 * contains the system stack per user; is cross referenced with the proc
 * structure for the same process.
 */

#define	EXCLOSE	01
#define	SHSIZE	16

struct	user
{
	label_t	u_rsav;			/* save info when exchanging stacks */
	short	u_dummy;		/* historical dreg; u_fperr is below */
	short	u_fpsaved;		/* FP regs saved for this proc */
	struct {
		short	u_fpsr;		/* FP status register */
		double	u_fpregs[6];	/* FP registers */
	} u_fps;
	char	u_segflg;		/* IO flag: 0:user D; 1:system; 2:user I */
	char	u_error;		/* return error code */
	short	u_uid;			/* effective user id */
	short	u_gid;			/* effective group id */
	short	u_ruid;			/* real user id */
	short	u_rgid;			/* real group id */
	struct	proc	*u_procp;	/* pointer to proc structure */
	short	*u_ap;			/* pointer to arglist */
	union {				/* syscall return values */
		struct	{
			short	r_val1;
			short	r_val2;
		};
		off_t	r_off;
		time_t	r_time;
	} u_r;
	caddr_t	u_base;			/* base address for IO */
	u_short	u_count;		/* bytes remaining for IO */
	off_t	u_offset;		/* offset in file for IO */
	struct	inode	*u_cdir;	/* inode of current directory */
	struct	inode	*u_rdir;	/* root directory of current process */
	char	u_dbuf[MAXNAMLEN];	/* current pathname component */
	caddr_t	u_dirp;			/* pathname pointer */
	struct	direct	u_dent;		/* current directory entry */
	struct	inode	*u_pdir;	/* inode of parent directory of dirp */
	short	u_uisa[16];		/* segmentation address prototypes */
	short	u_uisd[16];		/* segmentation descriptor prototypes */
	struct	file	*u_ofile[NOFILE];/* pointers to file structures of open files */
	char	u_pofile[NOFILE];	/* per-process flags of open files */
	short	u_arg[5];		/* arguments to current system call */
	size_t	u_tsize;		/* text size (clicks) */
	size_t	u_dsize;		/* data size (clicks) */
	size_t	u_ssize;		/* stack size (clicks) */
	label_t	u_qsav;			/* saved regs for interrupted syscall */
	label_t	u_ssav;			/* saved regs for newproc/expand */
	short	(*u_signal[NSIG])();	/* disposition of signals */
	time_t	u_utime;		/* this process's user time */
	time_t	u_stime;		/* this process's system time */
	time_t	u_cutime;		/* sum of children's utimes */
	time_t	u_cstime;		/* sum of children's stimes */
	short	*u_ar0;			/* address of user's saved R0 */
	struct {			/* profile arguments */
		short	*pr_base;	/* buffer base */
		u_short	pr_size;	/* buffer size */
		u_short	pr_off;		/* pc offset */
		u_short	pr_scale;	/* pc scaling */
	} u_prof;
	char	u_intflg;		/* catch intr from sys */
	char	u_sep;			/* flag for I and D separation */
	struct	tty	*u_ttyp;	/* controlling tty pointer */
	dev_t	u_ttyd;			/* controlling tty dev */
	union {
	   struct {			/* header of executable file */
		short	Ux_mag;		/* magic number */
		u_short Ux_tsize;	/* text size */
		u_short Ux_dsize;	/* data size */
		u_short Ux_bsize;	/* bss size */
		u_short Ux_ssize;	/* symbol table size */
		u_short Ux_entloc;	/* entry location */
		u_short Ux_unused;
		u_short Ux_relflg;
	   } Ux_A;
	   char ux_shell[SHSIZE];	/* #! and name of interpreter */
	} u_exdata;
#define	ux_mag		Ux_A.Ux_mag
#define	ux_tsize	Ux_A.Ux_tsize
#define	ux_dsize	Ux_A.Ux_dsize
#define	ux_bsize	Ux_A.Ux_bsize
#define	ux_ssize	Ux_A.Ux_ssize
#define	ux_entloc	Ux_A.Ux_entloc
#define	ux_unused	Ux_A.Ux_unused
#define	ux_relflg	Ux_A.Ux_relflg
	char	u_comm[MAXNAMLEN];
	time_t	u_start;
	char	u_acflag;
	short	u_fpflag;		/* unused now, will be later */
	short	u_cmask;		/* mask for file creation */
#ifdef	UCB_LOGIN
	short	u_login;		/* login flag: 0 or ttyslot */
	char	u_crn[4];
#endif
#ifdef MENLO_OVLY
#include <a.out.h>
	struct	u_ovd	{		/* automatic overlay data */
		short	uo_curov;	/* current overlay */
		short	uo_ovbase;	/* base of overlay area, seg. */
		u_short uo_dbase;	/* start of data, clicks */
		u_short	uo_ov_offst[NOVL+1];	/* overlay offsets in text */
		short	uo_nseg;	/* number of overlay seg. regs. */
	}	u_ovdata;
#endif
	struct	fperr	u_fperr;	/* floating point error save */
	char	u_eosys;		/* action on syscall termination */
	struct	buf *u_sbuf;		/* Buffer cache of symbolic name */
	int	u_slength;		/* Length of symbolic name */
	int	u_soffset;		/* Pointer into buffer */
	int	u_groups[NGROUPS + 1];	/* group list */
	short	u_stack[1];		/* kernel stack per user
					 * extends from u + USIZE*64
					 * backward not to reach here
					 */
};

#ifdef	KERNEL
extern	struct	user	u;
#endif

#define	JUSTRETURN	0		/* u_eosys values */
#define	RESTARTSYS	1
#define	SIMULATERTI	2
