#
# Copyright (c) 1988 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation,
# advertising materials, and other materials related to such
# distribution and use acknowledge that the software was developed
# by the University of California, Berkeley.  The name of the
# University may not be used to endorse or promote products derived
# from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
#	@(#)Makefile	5.8 (Berkeley) 9/22/88
#
CFLAGS=	-O
SEPFLAG=-i
LIBC=	/lib/libc.a
SRCS=	ftpd.c ftpcmd.c glob.c logwtmp.c popen.c vers.c
OBJS=	ftpd.o ftpcmd.o glob.o logwtmp.o popen.o vers.o
MAN=	ftpd.0

all: ftpd

ftpd: ${OBJS} ${LIBC}
	${CC} ${SEPFLAG} -o $@ ${OBJS}

vers.o: ftpd.c ftpcmd.y
	sh newvers.sh
	${CC} ${CFLAGS} -c vers.c

clean:
	rm -f ${OBJS} ftpd core ftpcmd.c

cleandir: clean
	rm -f ${MAN} tags .depend

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install:
	install -s -o root -g staff -m 751 ftpd ${DESTDIR}/etc/ftpd

lint: ${SRCS}
	lint ${CFLAGS} ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

ftpd.o: ftpd.c /usr/include/sys/param.h /usr/include/sys/localopts.h
ftpd.o: /usr/include/machine/machparam.h /usr/include/signal.h
ftpd.o: /usr/include/sys/types.h /usr/include/sys/stat.h
ftpd.o: /usr/include/sys/ioctl.h /usr/include/sys/ttychars.h
ftpd.o: /usr/include/sys/ttydev.h /usr/include/sys/socket.h
ftpd.o: /usr/include/short_names.h /usr/include/sys/file.h
ftpd.o: /usr/include/sys/wait.h /usr/include/netinet/in.h
ftpd.o: /usr/include/short_names.h /usr/include/arpa/ftp.h
ftpd.o: /usr/include/arpa/inet.h /usr/include/short_names.h
ftpd.o: /usr/include/arpa/telnet.h /usr/include/stdio.h /usr/include/signal.h
ftpd.o: /usr/include/pwd.h /usr/include/setjmp.h /usr/include/netdb.h
ftpd.o: /usr/include/short_names.h /usr/include/errno.h /usr/include/strings.h
ftpd.o: /usr/include/syslog.h
ftpcmd.o: ftpcmd.c /usr/include/sys/types.h /usr/include/sys/socket.h
ftpcmd.o: /usr/include/short_names.h /usr/include/netinet/in.h
ftpcmd.o: /usr/include/short_names.h /usr/include/arpa/ftp.h
ftpcmd.o: /usr/include/stdio.h /usr/include/signal.h /usr/include/ctype.h
ftpcmd.o: /usr/include/pwd.h /usr/include/setjmp.h /usr/include/syslog.h
ftpcmd.o: /usr/include/arpa/telnet.h
glob.o: glob.c /usr/include/sys/param.h /usr/include/sys/localopts.h
glob.o: /usr/include/machine/machparam.h /usr/include/signal.h
glob.o: /usr/include/sys/types.h /usr/include/sys/stat.h /usr/include/sys/dir.h
glob.o: /usr/include/stdio.h /usr/include/errno.h /usr/include/pwd.h
logwtmp.o: logwtmp.c /usr/include/sys/types.h /usr/include/sys/file.h
logwtmp.o: /usr/include/sys/time.h /usr/include/time.h /usr/include/sys/stat.h
logwtmp.o: /usr/include/utmp.h
popen.o: popen.c /usr/include/sys/types.h /usr/include/sys/signal.h
popen.o: /usr/include/stdio.h
vers.o: vers.c

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
