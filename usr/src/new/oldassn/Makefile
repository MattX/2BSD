DESTDIR=

DEFS=
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

all: oldassn.l
	lex oldassn.l
	cc ${CFLAGS} ${SEPFLAG} -w -o oldassn lex.yy.c
	rm lex.yy.c

install: oldassn oldassn.1
	install -s oldassn ${DESTDIR}/usr/new/oldassn
	/usr/man/manroff oldassn.1 >${DESTDIR}/usr/man/catn/oldassn.0

clean:
	rm -f oldassn *.o lex.yy.c MAKELOG

depend:
tags:
lint:
