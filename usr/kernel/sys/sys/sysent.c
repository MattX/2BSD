#include <sys/param.h>
#include <sys/systm.h>
#include <sys/quota.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)sysent.c	3.3";
#endif


/*
 * This table is the switch used to transfer
 * to the appropriate routine for processing a system call.
 * Each row contains the number of arguments expected
 * and a pointer to the routine.
 */
int	alarm();
int	mpxchan();
int	chdir();
int	chmod();
int	chown();
int	chroot();
int	close();
int	creat();
int	dup();
int	exec();
int	exece();
int	fork();
int	fstat();
int	getgid();
int	getpid();
int	getuid();
int	gtime();
int	gtty();
int	ioctl();
int	kill();
int	link();
int	mknod();
int	nice();
int	nosys();
int	nullsys();
int	open();
int	pause();
int	pipe();
int	profil();
int	ptrace();
int	read();
int	rexit();
int	saccess();
int	sbreak();
int	seek();
int	setgid();
int	setuid();
int	smount();
int	ssig();
int	stat();
int	stime();
int	stty();
int	sumount();
int	ftime();
int	sync();
#ifdef ACCT
int	sysacct();
#endif
int	syslock();
int	sysphys();
int	times();
int	umask();
int	unlink();
int	utime();
int	wait();
int	write();
#ifdef CGL_RTP
int	rtp();
#endif

struct sysent sysent[64] =
{
	0, 0, nullsys,			/*  0 = indir */
	1, 1, rexit,			/*  1 = exit */
	0, 0, fork,			/*  2 = fork */
	3, 1, read,			/*  3 = read */
	3, 1, write,			/*  4 = write */
	2, 0, open,			/*  5 = open */
	1, 1, close,			/*  6 = close */
	0, 0, wait,			/*  7 = wait */
	2, 0, creat,			/*  8 = creat */
	2, 0, link,			/*  9 = link */
	1, 0, unlink,			/* 10 = unlink */
	2, 0, exec,			/* 11 = exec */
	1, 0, chdir,			/* 12 = chdir */
	0, 0, gtime,			/* 13 = time */
	3, 0, mknod,			/* 14 = mknod */
	2, 0, chmod,			/* 15 = chmod */
	3, 0, chown,			/* 16 = chown; now 3 args */
	1, 0, sbreak,			/* 17 = break */
	2, 0, stat,			/* 18 = stat */
	4, 1, seek,			/* 19 = seek; now 3 args */
	0, 0, getpid,			/* 20 = getpid */
	3, 0, smount,			/* 21 = mount */
	1, 0, sumount,			/* 22 = umount */
	1, 1, setuid,			/* 23 = setuid */
	0, 0, getuid,			/* 24 = getuid */
	2, 2, stime,			/* 25 = stime */
	4, 1, ptrace,			/* 26 = ptrace */
	1, 1, alarm,			/* 27 = alarm */
	2, 1, fstat,			/* 28 = fstat */
	0, 0, pause,			/* 29 = pause */
	2, 0, utime,			/* 30 = utime */
	2, 1, stty,			/* 31 = stty */
	2, 1, gtty,			/* 32 = gtty */
	2, 0, saccess,			/* 33 = access */
	1, 1, nice,			/* 34 = nice */
	1, 0, ftime,			/* 35 = ftime; formerly sleep */
	0, 0, sync,			/* 36 = sync */
	2, 1, kill,			/* 37 = kill */
	0, 0, nullsys,			/* 38 = switch; inoperative */
	0, 0, nullsys,			/* 39 = setpgrp (not in yet) */
	1, 1, nosys,			/* 40 = tell (obsolete) */
	2, 2, dup,			/* 41 = dup */
	0, 0, pipe,			/* 42 = pipe */
	1, 0, times,			/* 43 = times */
	4, 0, profil,			/* 44 = prof */
	0, 0, nosys,			/* 45 = unused */
	1, 1, setgid,			/* 46 = setgid */
	0, 0, getgid,			/* 47 = getgid */
	2, 0, ssig,			/* 48 = sig */
#ifdef CGL_RTP
	1, 1, rtp,			/* 49 = real time process */
#else
	0, 0, nosys,			/* 49 = reserved for USG */
#endif
	0, 0, nosys,			/* 50 = reserved for USG */
#ifdef ACCT
	1, 0, sysacct,			/* 51 = turn acct off/on */
#else
	0, 0, nosys,			/* 51 = accting NA */
#endif
	3, 0, sysphys,			/* 52 = set user physical addresses */
	1, 0, syslock,			/* 53 = lock user in core */
	3, 0, ioctl,			/* 54 = ioctl */
	0, 0, nosys,			/* 55 = readwrite (in abeyance) */
	4, 0, mpxchan,			/* 56 = creat mpx comm channel */
	0, 0, nosys,			/* 57 = reserved for USG */
	0, 0, nosys,			/* 58 = local system calls */
	3, 0, exece,			/* 59 = exece */
	1, 0, umask,			/* 60 = umask */
	1, 0, chroot,			/* 61 = chroot */
	0, 0, nosys,			/* 62 = x */
	0, 0, nosys			/* 63 = used internally */
};

#ifdef	UCB_LOGIN
int	login();
#endif
int	iwait();
#ifdef UCB_SUBM
int	submit();
int	killbkg();
#endif
int	nostk();
int	killpg();
int	chfile();
int	fetchi();
int	quota();
int	lfstat();
int	lstat();
int	setpgrp();
int	gldav();
int	fperr();

struct	sysent	syslocal[32] = {
	0, 0, nosys,			/*  0 = illegal local call */
#ifdef UCB_LOGIN
	3, 0, login,			/*  1 = login */
#else 	
	0, 0, nosys,			/*  1 = nosys */
#endif
	1, 1, iwait,			/*  2 = iwait (for accounting, bx) */
#ifdef UCB_SUBM
	1, 1, submit,			/*  3 = submit - allow after logout */
#else
	0, 0, nosys,			/*  3 = unused */
#endif
	0, 0, nostk,			/*  4 = nostk */
#ifdef UCB_SUBM
	2, 0, killbkg,			/*  5 = killbkg - kill background */
#else
	0, 0, nosys,			/*  5 = unused */
#endif
	2, 0, killpg,			/*  6 = killpg - kill process group */
	3, 0, chfile,			/*  7 = chfile */
	1, 1, fetchi,			/*  8 = fetchi */
	0, 0, nosys,			/*  9 = syspr - system profiler */
	5, 0, quota,			/* 10 = quota - set quota parameters */
	2, 1, lfstat,			/* 11 = lfstat - long fstat */
	2, 0, lstat,			/* 12 = lstat - long stat */
	0, 0, setpgrp,			/* 13 = setpgrp - set process group */
	1, 1, gldav,			/* 14 = gldav */
	0, 0, fperr,			/* 15 = fperr - get FP error regs */
	0, 0, nosys,			/* 16 = x */
	0, 0, nosys,			/* 17 = x */
	0, 0, nosys,			/* 18 = x */
	0, 0, nosys,			/* 19 = x */
	0, 0, nosys,			/* 20 = x */
	0, 0, nosys,			/* 21 = x */
	0, 0, nosys,			/* 22 = x */
	0, 0, nosys,			/* 23 = x */
	0, 0, nosys,			/* 24 = x */
	0, 0, nosys,			/* 25 = x */
	0, 0, nosys,			/* 26 = x */
	0, 0, nosys,			/* 27 = x */
	0, 0, nosys,			/* 28 = x */
	0, 0, nosys,			/* 29 = x */
	0, 0, nosys,			/* 30 = x */
	0, 0, nosys,			/* 31 = used internally */
};
