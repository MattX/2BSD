#
# Copyright (c) 1989 The Regents of the University of California.
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
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
#	@(#)Makefile	5.2.3 (2.11BSD) 2025/11/28
#

CFLAGS=	-O
LDFLAGS= -i
SRCS=	chroot.c
OBJS=	chroot.o
MAN=	chroot.0
MANSRC=	chroot.8

all: chroot ${MAN}

chroot: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${CFLAGS} ${OBJS}

chroot.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} core chroot

cleandir: clean
	rm -f tags .depend

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: ${MAN} chroot
	install -c -s -o root -g staff -m 4751 chroot /usr/sbin
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8

lint: ${SRCS}
	lint ${CFLAGS} ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
