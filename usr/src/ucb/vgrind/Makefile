CFLAGS=-O
ALL=	vfontedpr retest
ROBJS=	retest.o regexp.o
VOBJS=	vfontedpr.o vgrindefs.o regexp.o

all:	${ALL}

vfontedpr: ${VOBJS}
	${CC} ${LFLAG} ${FP} -o $@ ${VOBJS}

retest: ${ROBJS}
	${CC} ${LFLAG} -o $@ ${ROBJS}

install:
	install -s -o bin -g bin -m 711 vfontedpr ${DESTDIR}/usr/lib
	install -c -o bin -g bin -m 755 vgrind.sh ${DESTDIR}/usr/ucb/vgrind
	install -c -o bin -g bin -m 644 vgrindefs.src ${DESTDIR}/usr/lib/vgrindefs
	install -c -o bin -g bin -m 644 tmac.vgrind ${DESTDIR}/usr/lib

clean:
	rm -f ${ROBJS} ${VOBJS} ${ALL}
