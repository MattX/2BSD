# @(#)Makefile	4.3 10/14/82
ALL=	telnet telnetd
CFLAGS= -DGOO -DUCB_NET -DMENLO_JCL -Dpdp11 -O -I/usr/include
TYPE=	-n
LIBC=	-lc

all:	${ALL}

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

telnet:	telnet.o
	${CC} telnet.o ${TYPE} -o telnet -ljobs -lnet ${LIBC}

telnetd: telnetd.o
	${CC} telnetd.o ${TYPE} -o telnetd -ljobs -lnet ${LIBC}

clean:
	rm -f ${ALL} *.o *.s errs core a.out t.?

install: ${ALL}
	install -s telnet ${DESTDIR}/usr/ucb
	install -s telnetd ${DESTDIR}/etc
