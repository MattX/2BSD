/*	rwhod.h	4.8	83/06/01	*/

/*
 * rwho protocol packet format.
 */
struct	outmp {
	char	out_line[8];		/* tty name */
	char	out_name[8];		/* user id */
	long	out_time;		/* time on */
};

#define int long
struct	whod {
	char	wd_vers;		/* protocol version # */
	char	wd_type;		/* packet type, see below */
	char	wd_pad[2];
	int	wd_sendtime;		/* time stamp by sender */
	int	wd_recvtime;		/* time stamp applied by receiver */
	char	wd_hostname[32];	/* hosts's name */
	int	wd_loadav[3];		/* load average as in uptime */
	int	wd_boottime;		/* time system booted */
	struct	whoent {
		struct	outmp we_utmp;	/* active tty info */
		int	we_idle;	/* tty idle time */
	} wd_we[1024 / sizeof (struct whoent)];
};
#undef int

#define	WHODVERSION	1
#define	WHODTYPE_STATUS	1		/* host status */

#ifndef DEBUG
#define sendto(s,msg,len,flags,to,tolen) send(s,to,msg,len)
#endif
#define recvfrom(s,buf,len,flags,from,fromlen) receive(s,from,buf,len)
