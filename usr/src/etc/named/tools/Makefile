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
#	@(#)Makefile	4.7 (Berkeley) 4/6/88
#
CFLAGS=	-O
SEPFLAG=-i
RES=
LIBC=	/lib/libc.a
SRCS=	nsquery.c nstest.c
OBJS=	nsquery.o nstest.o

all: nsquery nstest

nsquery: ${LIBC}
	${CC} ${CFLAGS} ${SEPFLAG} -o $@ nsquery.c ${RES}

# test programs and utilities
nstest: ${LIBC}
	@echo "The resolver routines in libc must be compiled with the -DDEBUG"
	@echo "option or nstest will not have any output."
	${CC} ${CFLAGS} ${SEPFLAG} -o $@ nstest.c ${RES}

clean: FRC
	rm -f ${OBJS} core nsquery nstest

depend: FRC
	mkdep -p ${CFLAGS} ${SRCS}

install: FRC

lint: FRC
	lint ${CFLAGS} ${SRCS}

tags: FRC
	ctags ${SRCS}

FRC:

# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

nsquery: nsquery.c /usr/include/stdio.h /usr/include/sys/types.h
nsquery: /usr/include/arpa/nameser.h /usr/include/netdb.h
nsquery: /usr/include/short_names.h /usr/include/sys/socket.h
nsquery: /usr/include/short_names.h /usr/include/netinet/in.h
nsquery: /usr/include/short_names.h /usr/include/resolv.h
nstest: nstest.c /usr/include/sys/types.h /usr/include/sys/socket.h
nstest: /usr/include/short_names.h /usr/include/netinet/in.h
nstest: /usr/include/short_names.h /usr/include/stdio.h
nstest: /usr/include/arpa/nameser.h /usr/include/arpa/inet.h
nstest: /usr/include/short_names.h /usr/include/resolv.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
