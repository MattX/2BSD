/*	rwhod.h	4.1	82/04/02	*/

struct	whod {
	char	wd_vers;
	char	wd_type;
	char	wd_fill[2];
	long	wd_sendtime;
	long	wd_recvtime;
	char	wd_hostname[32];
	long	wd_loadav[3];
	long	wd_bootime;
	struct	whoent {
		struct	utmp we_utmp;
		long	we_idle;
	} wd_we[1024 / sizeof (struct whoent)];
};
