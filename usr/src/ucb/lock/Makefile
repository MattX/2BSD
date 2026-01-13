#
# Copyright (c) 1988 Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms are permitted
# provided that the above copyright notice and this paragraph are
# duplicated in all such forms and that any documentation, advertising
# materials, and other materials related to such redistribution and
# use acknowledge that the software was developed by the University
# of California, Berkeley.  The name of the University may not be
# used to endorse or promote products derived from this software
# without specific prior written permission.  THIS SOFTWARE IS PROVIDED
# ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
# WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND
# FITNESS FOR A PARTICULAR PURPOSE.
#
# @(#)Makefile	5.2.3 (2.11BSD) 2025/11/26
#

CFLAGS=	-O
LDFLAGS= -i
SRCS=	lock.c
OBJS=	lock.o
MAN=	lock.0
MANSRC=	lock.1

all: lock ${MAN}

lock: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

lock.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} core lock

cleandir: clean
	rm -f tags .depend

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: lock ${MAN}
	install -c -s -o root -g staff -m 4755 lock /usr/ucb
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1

lint: ${SRCS}
	lint ${CFLAGS} ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
