#
# sys makefile
#

CFLAGS=-O
OV=

SYS=	access.o acct.o alarm.o chdir.o chmod.o chown.o chroot.o close.o \
	creat.o dup.o execl.o execle.o execv.o execve.o exit.o fork.o \
	fstat.o getgid.o getpid.o getppid.o getuid.o gldav.o ioctl.o kill.o \
	killpg.o link.o lock.o lseek.o lstat.o mknod.o mount.o nice.o open.o \
	pause.o phys.o pipe.o profil.o ptrace.o qfstat.o qstat.o quota.o \
	read.o readlink.o reboot.o renice.o rtp.o sbrk.o setgid.o setpgrp.o \
	setuid.o stat.o stime.o submit.o symlink.o sync.o time.o times.o \
	ucall.o umask.o umount.o unlink.o utime.o vhangup.o wait.o wait2.o \
	write.o getgroups.o setgroups.o getdtab.o

OVER=	vfork.o signal.o

libc.a: ${SYS} ${OVER}
	ar q ../$@ ${SYS} ${OVER}

libovc.a: ${SYS}
	ar q ../$@ ${SYS}

clean:
	rm -f ${SYS} ${OVER}

.s.o:
	as ${OV} - -o $*.o $*.s

access.o:	access.s
acct.o:		acct.s
alarm.o:	alarm.s
chdir.o:	chdir.s
chmod.o:	chmod.s
chown.o:	chown.s
chroot.o:	chroot.s
close.o:	close.s
creat.o:	creat.s
dup.o:		dup.s
execl.o:	execl.s
execle.o:	execle.s
execv.o:	execv.s
execve.o:	execve.s
exit.o:		exit.s
fetchi.o:	fetchi.s
fork.o:		fork.s
fperr.o:	fperr.s
fstat.o:	fstat.s
getgid.o:	getgid.s
getgrp.o:	getgrp.s
getpid.o:	getpid.s
getppid.o:	getppid.s
getuid.o:	getuid.s
gldav.o:	gldav.s
ioctl.o:	ioctl.s
kill.o:		kill.s
killbkg.o:	killbkg.s
killpg.o:	killpg.s
lfstat.o:	lfstat.s
link.o:		link.s
lock.o:		lock.s
login.o:	login.s
lseek.o:	lseek.s
lstat.o:	lstat.s
mknod.o:	mknod.s
mount.o:	mount.s
nice.o:		nice.s
open.o:		open.s
pause.o:	pause.s
phys.o:		phys.s
pipe.o:		pipe.s
profil.o:	profil.s
ptrace.o:	ptrace.s
qfstat.o:	qfstat.s
qstat.o:	qstat.s
quota.o:	quota.s
read.o:		read.s
readlink.o:	readlink.s
reboot.o:	reboot.s
renice.o:	renice.s
rtp.o:		rtp.s
sbrk.o:		sbrk.s
setgid.o:	setgid.s
setgrp.o:	setgrp.s
setpgrp.o:	setpgrp.s
setuid.o:	setuid.s
signal.o:	signal.s
stat.o:		stat.s
stime.o:	stime.s
submit.o:	submit.s
symlink.o:	symlink.s
sync.o:		sync.s
time.o:		time.s
times.o:	times.s
ucall.o:	ucall.s
umask.o:	umask.s
umount.o:	umount.s
unlink.o:	unlink.s
utime.o:	utime.s
vfork.o:	vfork.s
vhangup.o:	vhangup.s
wait.o:		wait.s
wait2.o:	wait2.s
write.o:	write.s
getgroups.o:	getgroups.s
setgroups.o:	setgroups.s
getdtab.o:	getdtab.s
