DESTDIR=

DEFS=
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

all: la.c
	cc ${CFLAGS} ${SEPFLAG} la.c -o la

install: la la.1
	install -s la ${DESTDIR}/usr/new/la
	/usr/man/manroff la.1 >${DESTDIR}/usr/man/catn/la.0

clean:
	rm -f la *.o MAKELOG

depend:
tags:
lint:
