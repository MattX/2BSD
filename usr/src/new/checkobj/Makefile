DESTDIR=

DEFS=
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

all: checkobj.c
	cc ${CFLAGS} ${SEPFLAG} checkobj.c -o checkobj

install: checkobj checkobj.1
	install -s checkobj ${DESTDIR}/usr/new/checkobj
	/usr/man/manroff checkobj.1 >${DESTDIR}/usr/man/catn/checkobj.0

clean:
	rm -f checkobj *.o MAKELOG

depend:
tags:
lint:
