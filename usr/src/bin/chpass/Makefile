#
# Copyright (c) 1988 The Regents of the University of California.
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
#	@(#)Makefile	5.1.3 (2.11BSD) 2025/11/26
#
CFLAGS=	-I. -O
LDFLAGS= -i
SRCS=	chpass.c field.c util.c
OBJS=	chpass.o field.o util.o
MAN=	chpass.0
MANSRC=	chpass.1

all: chpass ${MAN}

chpass: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

chpass.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} core chpass tags

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: ${MAN} chpass
	install -c -s -o root -g staff -m 4755 chpass /bin/chpass
	rm -f /bin/chfn; ln /bin/chpass /bin/chfn
	rm -f /bin/chsh; ln /bin/chpass /bin/chsh
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	rm -f /usr/man/cat1/chfn.0
	rm -f /usr/man/cat1/chsh.0
	-ln /usr/man/cat1/chpass.0 /usr/man/cat1/chfn.0
	-ln /usr/man/cat1/chpass.0 /usr/man/cat1/chsh.0

lint: ${SRCS}
	lint ${CFLAGS} ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
