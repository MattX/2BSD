#
# roff makefile
#

ALL=	roff

all:	${ALL}

SRC=	roff1.s roff2.s roff3.s roff4.s roff5.s roff7.s roff8.s

install: ${ALL}
	install -o bin -g bin -m 711 roff ${DESTDIR}/bin

roff:	${SRC}
	as -o roff.o /usr/include/sys.s ${SRC}
	ld -s roff.o -o $@ -lc

clean:
	rm -f *.o ${ALL}
