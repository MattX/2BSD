#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.7.2 (2.11BSD GTE) 2/4/95
#
# Several routines have been rewritten in assembly language for the VAX and
# the PDP. If you are not running on a VAX or PDP, you should use the
# original C sources which are included in this directory.
#
SRCS=	${STDSRC}
OBJS=	${STDOBJ}

CFLAGS=	-O ${DEFS}
STDSRC=	abort.c alarm.c atoi.c atol.c calloc.c closedir.c crypt.c \
	ctime.c ctype_.c daemon.c devname.c err.c errlst.c execvp.c fakcu.c \
	fstab.c gcvt.c getenv.c getgrent.c getgrgid.c getgrnam.c getlogin.c \
	getpass.c getpwent.c getloadavg.c \
	getttyent.c getttynam.c getusershell.c getwd.c \
	initgroups.c isatty.c malloc.c mktemp.c ndbm.c nlist.c \
	opendir.c perror.c popen.c psignal.c qsort.c random.c readdir.c \
	regex.c scandir.c seekdir.c setgid.c setegid.c setrgid.c setuid.c \
	setenv.c seteuid.c setruid.c siglist.c signal.c siginterrupt.c \
	sleep.c strcasecmp.c swab.c sysctl.c syslog.c system.c telldir.c \
	time.c timezone.c ttyname.c ttyslot.c ualarm.c usleep.c valloc.c \
	strdup.c strsep.c uname.c wait.c wait3.c waitpid.c
STDOBJ=	abort.o alarm.o atoi.o atol.o calloc.o closedir.o crypt.o \
	ctime.o ctype_.o daemon.o devname.o err.o errlst.o execvp.o fakcu.o \
	fstab.o gcvt.o getenv.o getgrent.o getgrgid.o getgrnam.o getlogin.o \
	getpass.o getpwent.o getloadavg.o \
	getttyent.o getttynam.o getusershell.o getwd.o \
	initgroups.o isatty.o malloc.o mktemp.o ndbm.o nlist.o \
	opendir.o perror.o popen.o psignal.o qsort.o random.o readdir.o \
	regex.o scandir.o seekdir.o setgid.o setegid.o setrgid.o setuid.o \
	setenv.o seteuid.o setruid.o siglist.o signal.o siginterrupt.o \
	sleep.o strcasecmp.o swab.o sysctl.o syslog.o system.o telldir.o \
	time.o timezone.o ttyname.o ttyslot.o ualarm.o usleep.o valloc.o \
	strdup.o strsep.o uname.o wait.o wait3.o waitpid.o

TAGSFILE=tags

.c.o:
	${CC} -p ${CFLAGS} -c $*.c
	-ld -X -r $*.o
	mv a.out profiled/$*.o
	${CC} ${CFLAGS} -c $*.c
	-ld -X -r $*.o
	mv a.out $*.o

genlib genlib_p: ${OBJS}
	@echo "building profiled genlib"
	@cd profiled; ar cru ../genlib_p ${OBJS}
	@echo "buiding normal genlib"
	@ar cru genlib ${OBJS}

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f genlib genlib_p *.o profiled/*.o tags Makefile.bak

depend:
	for i in ${SRCS}; do \
	    cc -M ${CFLAGS} $$i | awk ' { if ($$1 != prev) \
		{ if (rec != "") print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		else rec = rec " " $$2 } } \
		END { print rec } ' >> makedep; done
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile

# DO NOT DELETE THIS LINE -- make depend uses it

abort.o: abort.c /usr/include/signal.h
alarm.o: alarm.c /usr/include/sys/time.h /usr/include/time.h
atoi.o: atoi.c
atol.o: atol.c
abort.o: abort.c /usr/include/signal.h
alarm.o: alarm.c /usr/include/sys/time.h /usr/include/time.h
atoi.o: atoi.c
atol.o: atol.c
calloc.o: calloc.c
closedir.o: closedir.c /usr/include/sys/param.h /usr/include/sys/localopts.h
closedir.o: /usr/include/machine/machparam.h /usr/include/signal.h
closedir.o: /usr/include/sys/types.h /usr/include/sys/dir.h
crypt.o: crypt.c
ctime.o: ctime.c /usr/include/sys/param.h /usr/include/sys/localopts.h
ctime.o: /usr/include/machine/machparam.h /usr/include/signal.h
ctime.o: /usr/include/sys/types.h /usr/include/sys/time.h /usr/include/time.h
ctime.o: /usr/include/tzfile.h
ctype_.o: ctype_.c /usr/include/ctype.h
errlst.o: errlst.c
execvp.o: execvp.c /usr/include/errno.h
fakcu.o: fakcu.c
fstab.o: fstab.c /usr/include/fstab.h /usr/include/stdio.h /usr/include/ctype.h
gcvt.o: gcvt.c
getenv.o: getenv.c /usr/include/stdio.h
getgrent.o: getgrent.c /usr/include/stdio.h /usr/include/grp.h
getgrgid.o: getgrgid.c /usr/include/grp.h
getgrnam.o: getgrnam.c /usr/include/grp.h
getlogin.o: getlogin.c /usr/include/utmp.h
getpass.o: getpass.c /usr/include/stdio.h /usr/include/signal.h
getpass.o: /usr/include/sgtty.h /usr/include/sys/ioctl.h
getpass.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
getpwent.o: getpwent.c /usr/include/sys/types.h /usr/include/sys/file.h
getpwent.o: /usr/include/stdio.h /usr/include/pwd.h /usr/include/ndbm.h
getttyent.o: getttyent.c /usr/include/stdio.h /usr/include/strings.h
getttyent.o: /usr/include/ttyent.h
getttynam.o: getttynam.c /usr/include/ttyent.h
getusershell.o: getusershell.c /usr/include/sys/param.h
getusershell.o: /usr/include/sys/localopts.h /usr/include/machine/machparam.h
getusershell.o: /usr/include/signal.h /usr/include/sys/types.h
getusershell.o: /usr/include/sys/file.h /usr/include/sys/stat.h
getusershell.o: /usr/include/ctype.h /usr/include/stdio.h
getwd.o: getwd.c /usr/include/sys/param.h /usr/include/sys/localopts.h
getwd.o: /usr/include/machine/machparam.h /usr/include/signal.h
getwd.o: /usr/include/sys/types.h /usr/include/sys/stat.h
getwd.o: /usr/include/sys/dir.h
initgroups.o: initgroups.c /usr/include/stdio.h /usr/include/sys/param.h
initgroups.o: /usr/include/sys/localopts.h /usr/include/machine/machparam.h
initgroups.o: /usr/include/signal.h /usr/include/sys/types.h /usr/include/grp.h
isatty.o: isatty.c /usr/include/sgtty.h /usr/include/sys/ioctl.h
isatty.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
malloc.o: malloc.c
mktemp.o: mktemp.c /usr/include/sys/types.h /usr/include/sys/file.h
mktemp.o: /usr/include/sys/stat.h /usr/include/errno.h /usr/include/stdio.h
mktemp.o: /usr/include/ctype.h
ndbm.o: ndbm.c /usr/include/sys/types.h /usr/include/sys/stat.h
ndbm.o: /usr/include/sys/file.h /usr/include/stdio.h /usr/include/errno.h
ndbm.o: /usr/include/ndbm.h
nlist.o: nlist.c /usr/include/sys/types.h /usr/include/sys/file.h
nlist.o: /usr/include/a.out.h /usr/include/sys/exec.h /usr/include/stdio.h
opendir.o: opendir.c /usr/include/sys/param.h /usr/include/sys/localopts.h
opendir.o: /usr/include/machine/machparam.h /usr/include/signal.h
opendir.o: /usr/include/sys/types.h /usr/include/sys/dir.h
perror.o: perror.c /usr/include/sys/types.h /usr/include/sys/uio.h
popen.o: popen.c /usr/include/stdio.h /usr/include/signal.h
psignal.o: psignal.c /usr/include/signal.h
qsort.o: qsort.c
random.o: random.c /usr/include/stdio.h
readdir.o: readdir.c /usr/include/sys/param.h /usr/include/sys/localopts.h
readdir.o: /usr/include/machine/machparam.h /usr/include/signal.h
readdir.o: /usr/include/sys/types.h /usr/include/sys/dir.h
regex.o: regex.c
scandir.o: scandir.c /usr/include/sys/types.h /usr/include/sys/stat.h
scandir.o: /usr/include/sys/dir.h
seekdir.o: seekdir.c /usr/include/sys/param.h /usr/include/sys/localopts.h
seekdir.o: /usr/include/machine/machparam.h /usr/include/signal.h
seekdir.o: /usr/include/sys/types.h /usr/include/sys/dir.h
setgid.o: setgid.c
setegid.o: setegid.c
setrgid.o: setrgid.c
setuid.o: setuid.c
setenv.o: setenv.c /usr/include/sys/types.h /usr/include/stdio.h
seteuid.o: seteuid.c
setruid.o: setruid.c
siglist.o: siglist.c /usr/include/signal.h
signal.o: signal.c /usr/include/signal.h
siginterrupt.o: siginterrupt.c /usr/include/signal.h
sleep.o: sleep.c /usr/include/sys/time.h /usr/include/time.h
sleep.o: /usr/include/signal.h
strcasecmp.o: strcasecmp.c
swab.o: swab.c
syslog.o: syslog.c /usr/include/sys/types.h /usr/include/netdb.h
syslog.o: /usr/include/sys/socket.h
syslog.o: /usr/include/sys/file.h
syslog.o: /usr/include/sys/signal.h /usr/include/sys/syslog.h
syslog.o: /usr/include/strings.h
system.o: system.c /usr/include/signal.h
telldir.o: telldir.c /usr/include/sys/param.h /usr/include/sys/localopts.h
telldir.o: /usr/include/machine/machparam.h /usr/include/signal.h
telldir.o: /usr/include/sys/types.h /usr/include/sys/dir.h
time.o: time.c /usr/include/sys/types.h /usr/include/sys/time.h
time.o: /usr/include/time.h
timezone.o: timezone.c /usr/include/sys/types.h /usr/include/sys/time.h
timezone.o: /usr/include/time.h /usr/include/stdio.h /usr/include/tzfile.h
ttyname.o: ttyname.c /usr/include/sys/param.h /usr/include/sys/localopts.h
ttyname.o: /usr/include/machine/machparam.h /usr/include/signal.h
ttyname.o: /usr/include/sys/types.h /usr/include/sys/dir.h
ttyname.o: /usr/include/sys/stat.h
ttyslot.o: ttyslot.c /usr/include/ttyent.h
ualarm.o: ualarm.c /usr/include/sys/time.h /usr/include/time.h
usleep.o: usleep.c /usr/include/sys/time.h /usr/include/time.h
usleep.o: /usr/include/signal.h
valloc.o: valloc.c
strdup.o: strdup.c /usr/include/sys/types.h /usr/include/stdio.h
strsep.o: strsep.c /usr/include/stdio.h
wait.o: wait.c /usr/include/sys/types.h /usr/include/sys/time.h
wait.o: /usr/include/time.h /usr/include/sys/wait.h /usr/include/sys/resource.h
wait3.o: wait3.c /usr/include/sys/types.h /usr/include/sys/time.h
wait3.o: /usr/include/time.h /usr/include/sys/wait.h
wait3.o: /usr/include/sys/resource.h
waitpid.o: waitpid.c /usr/include/sys/types.h /usr/include/sys/time.h
waitpid.o: /usr/include/time.h /usr/include/sys/wait.h
waitpid.o: /usr/include/sys/resource.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
