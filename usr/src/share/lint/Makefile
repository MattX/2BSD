#
# Public domain - 1996/10/23 - sms
#
#	@(#)Makefile	1.0 (2.11BSD) 1996/10/23
#
#  Makefile for the lint libraries.
#

SRCS= llib-lc llib-lcurses llib-ldbm llib-lm llib-lmp \
	llib-lplot llib-ltermcap llib-port

all:

install:${SRCS}
	-mkdir -p ${DESTDIR}/usr/share/lint
	-chmod a+r,a+x ${DESTDIR}/usr/share/lint
	install -c -m 444 ${SRCS} ${DESTDIR}/usr/share/lint

clean:

tags:

depend:
