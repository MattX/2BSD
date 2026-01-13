#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.1.3 (2.11BSD) 2025/11/25
#
CFLAGS=	-O
LDFLAGS= -i

more:	more.o
	${CC} ${LDFLAGS} -o more more.o -ltermcap

install: more more.help
	install -c -s -m 755 -o root -g staff more /usr/ucb/more
	rm -f /usr/ucb/page
	ln /usr/ucb/more /usr/ucb/page
	install -m 444 -c more.help /usr/share/misc/more.help

clean:
	rm -f more more.o core a.out errs
