#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	1.2 (Berkeley) 2/7/86
#

OBJ=	tftpd.o tftpsubs.o
MAN=	tftpd.0
ALL=	${OBJ} ${MAN}
DESTDIR=
CFLAGS=	-O
SEPFLAG= -i
LDFLAGS=

tftpd:	${ALL}
	${CC} ${LDFLAGS} ${SEPFLAG} -o tftpd ${OBJ}

tftpd.0:	tftpd.8
	/usr/man/manroff tftpd.8 > tftpd.0

install: tftpd tftpd.0
	install -s tftpd ${DESTDIR}/etc/tftpd
	install -c -m 444 tftpd.0 ${DESTDIR}/usr/man/cat8/tftpd.0

clean:
	rm -f tftpd *.o *.s errs core a.out t.? tftpd.0
