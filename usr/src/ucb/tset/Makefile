DEFS=	-DFULLLOGIN -DUCB_NTTY
CFLAGS=	-O ${DEFS}

all:	tset

install:
	install -s -o bin -g bin -m 711 tset ${DESTDIR}/usr/ucb
	rm -f ${DESTDIR}/usr/ucb/reset
	ln ${DESTDIR}/usr/ucb/tset ${DESTDIR}/usr/ucb/reset

tset:	tset.c
	${CC} ${LFLAG} ${CFLAGS} tset.c -o tset -ltermcap

clean:
	rm -f *.o tset
