DESTDIR=

DEFS=
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

all: shortc.c
	cc ${CFLAGS} ${SEPFLAG} shortc.c -o shortc

install: shortc shortc.1
	install -s shortc ${DESTDIR}/usr/new/shortc
	/usr/man/manroff shortc.1 >${DESTDIR}/usr/man/catn/shortc.0

clean:
	rm -f shortc *.o MAKELOG

depend:
tags:
lint:
