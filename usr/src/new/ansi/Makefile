#	@(#)Makefile	5.3.2	(2.11BSD)	2025/11/25
#
CFLAGS=	-O
LDFLAGS=-i

SRCS=	ansitape.c vmsprep.c
MSRC=	ansitape.1 vmsprep.1
MOBJ=	ansitape.0 vmsprep.0

.SUFFIXES: .1 .0

.1.0:
	/usr/man/manroff $*.1 > $*.0

all:	ansitape vmsprep ${MOBJ}

ansitape: ansitape.o
	cc ${LDFLAGS} -o ansitape ansitape.o

vmsprep: vmsprep.o
	cc ${LDFLAGS} -o vmsprep vmsprep.o

install: vmsprep ansitape ${MOBJ}
	install -c -s ansitape /usr/new/ansitape
	install -c -s vmsprep /usr/new/vmsprep
	install -c -m 444 -o root -g staff ansitape.0 /usr/new/man/cat1
	install -c -m 444 -o root -g staff vmsprep.0 /usr/new/man/cat1

clean:
	rm -f *.o a.out core errs makefile.bak Makefile.bak ansitape vmsprep *.0

depend:
	mkdep ${CFLAGS} ${SRCS}
