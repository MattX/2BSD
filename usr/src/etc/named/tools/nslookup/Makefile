#
# Copyright (c) 1987 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that this notice is preserved and that due credit is given
# to the University of California at Berkeley. The name of the University
# may not be used to endorse or promote products derived from this
# software without specific prior written permission. This software
# is provided ``as is'' without express or implied warranty.
#
#	@(#)Makefile	5.16 (Berkeley) 2/20/88
#
CFLAGS=	-O
SEPFLAG=-i
RES=
LIBC=	/lib/libc.a
LIBS=	-ll ${RES}
CSRCS=	main.c getinfo.c debug.c send.c skip.c list.c subr.c
SRCS=	${CSRCS} commands.c
OBJS=	main.o getinfo.o debug.o send.o skip.o list.o subr.o commands.o

nslookup: ${OBJS} ${LIBC}
	${CC} ${SEPFLAG} -o $@ ${OBJS} ${LIBS}

clean: FRC
	rm -f ${OBJS} core nslookup commands.c nslookup.0 lex.yy.c lex.yy.o

depend: ${SRCS} FRC
	mkdep ${CFLAGS} ${SRCS}

install: FRC
	install -s -o bin -m 755 nslookup ${DESTDIR}/etc/nslookup
	install -c -o bin -m 444 nslookup.help ${DESTDIR}/usr/lib/nslookup.help

lint: ${SRCS} FRC
	lint ${CFLAGS} ${SRCS}

tags: ${CSRCS} FRC
	ctags ${CSRCS}

FRC:

# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

main.o: main.c /usr/include/stdio.h /usr/include/strings.h
main.o: /usr/include/sys/param.h /usr/include/sys/localopts.h
main.o: /usr/include/machine/machparam.h /usr/include/signal.h
main.o: /usr/include/sys/types.h /usr/include/netdb.h
main.o: /usr/include/short_names.h /usr/include/sys/socket.h
main.o: /usr/include/short_names.h /usr/include/netinet/in.h
main.o: /usr/include/short_names.h /usr/include/arpa/inet.h
main.o: /usr/include/short_names.h /usr/include/arpa/nameser.h
main.o: /usr/include/resolv.h /usr/include/signal.h /usr/include/setjmp.h res.h
main.o: shortnames.h
getinfo.o: getinfo.c /usr/include/sys/types.h /usr/include/sys/socket.h
getinfo.o: /usr/include/short_names.h /usr/include/netinet/in.h
getinfo.o: /usr/include/short_names.h /usr/include/stdio.h /usr/include/ctype.h
getinfo.o: /usr/include/arpa/inet.h /usr/include/short_names.h
getinfo.o: /usr/include/arpa/nameser.h /usr/include/resolv.h res.h shortnames.h
debug.o: debug.c /usr/include/sys/types.h /usr/include/netinet/in.h
debug.o: /usr/include/short_names.h /usr/include/stdio.h
debug.o: /usr/include/arpa/inet.h /usr/include/short_names.h
debug.o: /usr/include/arpa/nameser.h /usr/include/resolv.h res.h shortnames.h
send.o: send.c /usr/include/sys/types.h /usr/include/sys/time.h
send.o: /usr/include/time.h /usr/include/sys/socket.h
send.o: /usr/include/short_names.h /usr/include/netinet/in.h
send.o: /usr/include/short_names.h /usr/include/stdio.h
send.o: /usr/include/arpa/nameser.h /usr/include/resolv.h res.h shortnames.h
skip.o: skip.c /usr/include/sys/types.h /usr/include/netinet/in.h
skip.o: /usr/include/short_names.h /usr/include/stdio.h
skip.o: /usr/include/arpa/nameser.h res.h shortnames.h
list.o: list.c /usr/include/sys/types.h /usr/include/sys/socket.h
list.o: /usr/include/short_names.h /usr/include/netinet/in.h
list.o: /usr/include/short_names.h /usr/include/netdb.h
list.o: /usr/include/short_names.h /usr/include/stdio.h /usr/include/strings.h
list.o: /usr/include/ctype.h /usr/include/arpa/inet.h
list.o: /usr/include/short_names.h /usr/include/arpa/nameser.h
list.o: /usr/include/resolv.h res.h shortnames.h
subr.o: subr.c /usr/include/stdio.h /usr/include/strings.h
subr.o: /usr/include/sys/types.h /usr/include/netdb.h
subr.o: /usr/include/short_names.h /usr/include/sys/socket.h
subr.o: /usr/include/short_names.h /usr/include/arpa/inet.h
subr.o: /usr/include/short_names.h /usr/include/netinet/in.h
subr.o: /usr/include/short_names.h /usr/include/arpa/nameser.h
subr.o: /usr/include/signal.h /usr/include/setjmp.h res.h shortnames.h
commands.o: commands.c /usr/include/stdio.h res.h shortnames.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
