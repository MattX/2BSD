/*	syscall.h	4.13	83/08/11	*/

#define	SYS_exit	1
#define SYS_fork_l      2
#define SYS_read_l      3
#define SYS_write_l     4
#define SYS_open_l      5
#define SYS_close_l     6
				/*  7 is old: wait */
#define SYS_creat_l     8
#define SYS_link_l      9
#define SYS_unlink_l    10
#define	SYS_execv	11
#define SYS_chdir_l     12
				/* 13 is old: time */
#define SYS_mknod_l     14
#define SYS_chmod_l     15
#define SYS_chown_l     16
				/* 17 is old: sbreak */
				/* 18 is old: stat */
#define SYS_lseek_l     19
#define	SYS_getpid	20
#define SYS_mount_l     21
#define SYS_umount_l    22
				/* 23 is old: setuid */
#define	SYS_getuid	24
				/* 25 is old: stime */
#define	SYS_ptrace	26
				/* 27 is old: alarm */
				/* 28 is old: fstat */
				/* 29 is old: pause */
				/* 30 is old: utime */
				/* 31 is old: stty */
				/* 32 is old: gtty */
#define SYS_access_l    33
				/* 34 is old: nice */
				/* 35 is old: ftime */
#define	SYS_sync	36
#define	SYS_kill	37
#define SYS_stat_l      38
				/* 39 is old: setpgrp */
#define SYS_lstat_l     40
#define SYS_dup_l       41
#define	SYS_pipe	42
				/* 43 is old: times */
#define	SYS_profil	44
				/* 45 is unused */
				/* 46 is old: setgid */
#define	SYS_getgid	47
				/* 48 is old: sigsys */
				/* 49 is unused */
				/* 50 is unused */
#define	SYS_acct	51
				/* 52 is old: phys */
				/* 53 is old: syslock */
#define	SYS_ioctl	54
#define	SYS_reboot	55
				/* 56 is old: mpxchan */
#define SYS_symlink_l   57
#define SYS_readlink_l  58
#define	SYS_execve	59
#define SYS_umask_l     60
#define SYS_chroot_l    61
#define SYS_fstat_l     62
				/* 63 is unused */
#define	SYS_getpagesize 64
#define	SYS_mremap	65
				/* 65 is old: vfork */
				/* 66 is old: vread */
				/* 67 is old: vwrite */
#define	SYS_sbrk	69
#define	SYS_sstk	70
#define	SYS_mmap	71
				/* 72 is old: vadvise */
#define	SYS_munmap	73
#define	SYS_mprotect	74
#define	SYS_madvise	75
#define	SYS_vhangup	76
				/* 77 is old: vlimit */
#define	SYS_mincore	78
#define	SYS_getgroups	79
#define	SYS_setgroups	80
#define	SYS_getpgrp	81
#define	SYS_setpgrp	82
#define	SYS_setitimer	83
#define	SYS_wait	84
#define	SYS_swapon	85
#define	SYS_getitimer	86
#define	SYS_gethostname	87
#define	SYS_sethostname	88
#define	SYS_getdtablesize 89
#define SYS_dup2_l      90
#define	SYS_getdopt	91
#define SYS_fcntl_l     92
#define	SYS_select	93
#define	SYS_setdopt	94
#define SYS_fsync_l     95
#define	SYS_setpriority	96
#define	SYS_socket	97
#define	SYS_connect	98
#define	SYS_accept	99
#define	SYS_getpriority	100
#define	SYS_send	101
#define	SYS_recv	102
				/* 103 was socketaddr */
#define	SYS_bind	104
#define	SYS_setsockopt	105
#define	SYS_listen	106
				/* 107 was vtimes */
#define	SYS_sigvec	108
#define	SYS_sigblock	109
#define	SYS_sigsetmask	110
#define	SYS_sigpause	111
#define	SYS_sigstack	112
#define	SYS_recvmsg	113
#define	SYS_sendmsg	114
				/* 115 is old vtrace */
#define	SYS_gettimeofday 116
#define	SYS_getrusage	117
#define	SYS_getsockopt	118
				/* 119 is old resuba */
#define	SYS_readv	120
#define	SYS_writev	121
#define	SYS_settimeofday 122
#define SYS_fchown_l    123
#define SYS_fchmod_l    124
#define	SYS_recvfrom	125
#define	SYS_setreuid	126
#define	SYS_setregid	127
#define SYS_rename_l    128
#define SYS_truncate_l  129
#define SYS_ftruncate_l 130
#define SYS_flock_l     131
				/* 132 is unused */
#define	SYS_sendto	133
#define	SYS_shutdown	134
#define	SYS_socketpair	135
#define SYS_mkdir_l     136
#define SYS_rmdir_l     137
#define	SYS_utimes	138
				/* 139 is unused */
#define	SYS_revoke	140
#define	SYS_getpeername	141
#define	SYS_gethostid	142
#define	SYS_sethostid	143
#define	SYS_getrlimit	144
#define	SYS_setrlimit	145
#define	SYS_killpg	146
				/* 147 is unused */
#define	SYS_setquota	148
#define	SYS_quota	149
#define	SYS_getsockname	150
