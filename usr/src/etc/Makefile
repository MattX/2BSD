#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.14.9 (2.11BSD GTE) 1996/11/16
#
DESTDIR=
CFLAGS=	-O
SEPFLAG= -i

# C programs that live in the current directory and do not need
# explicit make lines.
#
STD=	init

all:	${STD}

${STD}:
	cc ${CFLAGS} ${SEPFLAG} -o $@ $@.c

install: ${STD}
	install -s ${STD} ${DESTDIR}/etc
	-chmod 755 ${DESTDIR}/etc/init

clean:
	-rm -f a.out core *.s *.o
	-rm -f ${STD}

FRC:

depend:
