# @(#)Makefile	4.3 10/14/82
# SYSDIR must be set to the directory for the system in conf
SYSDIR = ../../../sys/GOO
OBJS=	host.o inet.o if.o main.o mbuf.o route.o
CFLAGS= -DUCB_NET -DMENLO_JCL -Dpdp11 -O -I../../../sys -I/usr/include
LIBC= -lc
DESTDIR=/usr/ucb
TYPE=	-n

.c.o:
	${CC} -S ${CFLAGS} -I${SYSDIR} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

netstat: ${OBJS}
	${CC} ${OBJS} ${TYPE} -o netstat -lnet ${LIBC}

install: netstat
	install -s netstat ${DESTDIR}/usr/ucb

clean:
	rm -f *.o core a.out errs
