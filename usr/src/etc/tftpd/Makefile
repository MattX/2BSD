#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	1.2 (Berkeley) 2/7/86
#

ALL=	tftpd.o tftpsubs.o
DESTDIR=
CFLAGS=	-O
SEPFLAG= -i
LDFLAGS=

tftpd:	${ALL}
	${CC} ${LDFLAGS} ${SEPFLAG} -o tftpd ${ALL}

install:
	install -s tftpd ${DESTDIR}/etc/tftpd

clean:
	rm -f tftpd *.o *.s errs core a.out t.?
