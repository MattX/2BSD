#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.7.12 (2.11BSD) 2025/11/29
#
# Several routines have been rewritten in assembly language for the VAX and
# the PDP. If you are not running on a VAX or PDP, you should use the
# original C sources which are included in this directory.
#
SRCS=	${STDSRC}
OBJS=	${STDOBJ}

CFLAGS=	-O ${DEFS}
STDSRC=	abort.c alarm.c atoi.c atol.c calloc.c closedir.c crypt.c \
	ctime.c ctype_.c daemon.c devname.c disklabel.c err.c \
	execvp.c fakcu.c \
	fstab.c gcvt.c getenv.c getgrent.c getgrgid.c getgrnam.c getlogin.c \
	getgrouplist.c gethostname.c getpagesize.c \
	getpass.c getpwent.c getloadavg.c getmntinfo.c \
	getttyent.c getttynam.c getusershell.c getwd.c \
	initgroups.c isatty.c malloc.c mktemp.c ndbm.c nlist.c \
	opendir.c perror.c popen.c psignal.c qsort.c random.c readdir.c \
	regex.c scandir.c seekdir.c \
	setmode.c sethostname.c \
	setenv.c siglist.c signal.c siginterrupt.c \
	sigsetops.c \
	sleep.c strcasecmp.c strftime.c swab.c sysctl.c syslog.c system.c \
	syserrlst.c \
	telldir.c time.c timezone.c ttyname.c ttyslot.c ualarm.c usleep.c \
	valloc.c strdup.c uname.c wait.c wait3.c waitpid.c
STDOBJ=	abort.o alarm.o atoi.o atol.o calloc.o closedir.o crypt.o \
	ctime.o ctype_.o daemon.o devname.o disklabel.o err.o \
	execvp.o fakcu.o \
	fstab.o gcvt.o getenv.o getgrent.o getgrgid.o getgrnam.o getlogin.o \
	getgrouplist.o gethostname.o getpagesize.o \
	getpass.o getpwent.o getloadavg.o getmntinfo.o \
	getttyent.o getttynam.o getusershell.o getwd.o \
	initgroups.o isatty.o malloc.o mktemp.o ndbm.o nlist.o \
	opendir.o perror.o popen.o psignal.o qsort.o random.o readdir.o \
	regex.o scandir.o seekdir.o \
	setmode.o sethostname.o \
	setenv.o siglist.o signal.o siginterrupt.o \
	sigsetops.o \
	sleep.o strcasecmp.o strftime.o swab.o sysctl.o syslog.o system.o \
	syserrlst.o \
	telldir.o time.o timezone.o ttyname.o ttyslot.o ualarm.o usleep.o \
	valloc.o strdup.o uname.o wait.o wait3.o waitpid.o

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

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
