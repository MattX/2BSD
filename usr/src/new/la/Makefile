DESTDIR=

DEFS=
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

all: la

la: la.c
	cc ${CFLAGS} ${SEPFLAG} la.c -o la

install: la la.0
	install -s la ${DESTDIR}/usr/new/la
	install -c -m 444 -o bin -g bin la.0 ${DESTDIR}/usr/new/man/cat1/la.0

la.0: la.1
	/usr/man/manroff la.1 > la.0

clean:
	rm -f la *.o la.0 MAKELOG

depend:
tags:
lint:
