#
# makefile for as
#

ALL=	as as2

all:	${ALL}

install:
	install -o bin -g bin -m 711 as ${DESTDIR}/bin
	install -o bin -g bin -m 711 as2 ${DESTDIR}/lib

as:
	as /usr/include/sys.s as1?.s
	ld -n -s a.out -o as
	rm -f a.out

as2:
	as /usr/include/sys.s as2?.s
	ld -n -s a.out -o as2
	rm -f a.out

clean:
	rm -f *.o a.out ${ALL}
