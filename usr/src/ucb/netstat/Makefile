# @(#)Makefile	4.3 10/14/82
CFLAGS=	-DUCB_NET
OBJS=	host.o inet.o if.o main.o mbuf.o route.o

all: netstat

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

netstat: ${OBJS}
	${CC} ${LFLAG} ${OBJS} -o netstat

install: netstat
	install -s -o root -g su -m 4755 netstat ${DESTDIR}/usr/ucb

clean:
	rm -f *.[os] core a.out errs
