# Version 2.1 January 19, 1994

SRCS=	ld.c
OBJS=	ld.o
CFLAGS=	-O -I../ar -DNUM_VM_PAGES=20

all:	ld

ld: ${OBJS}
	cc -i -o ld ${OBJS} -lvmf

install:
	install -c -s -o bin -g bin -m 755 ld ${DESTDIR}/bin/ld

lint:
	lint -havx -I../ar ${SRCS}

clean:
	rm -f *.o *.0 ld
