#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.7 (GTE) 11/28/92
#
# Bug report processor and associated programs
#

BUGS_NAME = sms
BUGS_HOME = @WLV.IIPO.GTEGSC.COM
DEFS = -DBUGS_NAME='"$(BUGS_NAME)"' -DBUGS_HOME='"$(BUGS_HOME)"'
CFLAGS = -O -DUNIXCOMP $(DEFS)
LIBDIR=	/usr/lib
DESTDIR=

all: bugfiler unixtomh

bugfiler: bugfiler.c
	${CC} ${CFLAGS} ${SEPFLAG} -DUNIXTOMH='"${LIBDIR}/unixtomh"' -o bugfiler bugfiler.c

unixtomh: unixtomh.c
	${CC} ${SEPFLAG} ${CFLAGS} -o unixtomh unixtomh.c

install: bugfiler sendbug.sh bugformat unixtomh
	install -s bugfiler ${DESTDIR}${LIBDIR}/bugfiler
	install -c -m 555 sendbug.sh ${DESTDIR}/usr/ucb/sendbug
	install -s unixtomh ${DESTDIR}${LIBDIR}/unixtomh
	install -c -m 644 bugformat ${DESTDIR}/usr/lib/bugformat

clean:
	rm -f *.o bugfiler unixtomh
