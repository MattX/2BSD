int	i;
char	doo[100];
char	poo[100];
FILE	*fp;

#define	VAX_780		1
#define	VAX_750		2
#define	VAX_730		3
#define VAX_8600	4
#define VAX_8200	5
#define VAX_8800	6
#define MVAX_I		7
#define MVAX_II		8

/* System defines to be changed-- */

#define	LOAD	/* If you want the load average box, keep this defined. */
/* #define CPU	/* If you want to know what type CPU you are using, keep
		   this defined.  NOTE: This option has only been tested for
		   Ultrix V2.0, nothing else.  Please... Try it and mail
		   me back! */
/* #define UVERS	/* If you want to know the version of Ultrix, keep this
		   defined. This will work for any system with /etc/motd.
#define LIGHT	/* If you want the stuff inside the welcome box inversed,
		   keep this defined. To make it dark, just comment it out. */
#define TTY	/* If you want the tty box, keep this define		*/
