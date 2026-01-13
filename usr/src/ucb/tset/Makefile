#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.4.2 (2.11BSD) 2025/11/25
#
CFLAGS=-O
LDFLAGS= -i

all: tset tset.0

tset:	tset.o
	${CC} ${LDFLAGS} ${CFLAGS} tset.o -o tset -ltermlib

tset.0:	tset.1
	/usr/man/manroff tset.1 > tset.0

install: all
	install -c -s -m 755 tset /usr/ucb
	rm -f /usr/ucb/reset
	ln /usr/ucb/tset /usr/ucb/reset
	rm -f /usr/man/cat1/reset.0
	install -c -o root -g staff -m 444 tset.0 /usr/man/cat1/tset.0
	ln /usr/man/cat1/tset.0 /usr/man/cat1/reset.0

clean:
	rm -f *.o tset tset.o tset.0 tags

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
