DESTDIR=

DEFS=
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

all: nmcoll.sh

install: nmcoll.sh nmcoll.1
	install -c -m 755 nmcoll.sh ${DESTDIR}/usr/new/nmcoll
	/usr/man/manroff nmcoll.1 >${DESTDIR}/usr/man/catn/nmcoll.0

clean:
depend:
tags:
lint:
