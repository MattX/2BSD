# @(#)Makefile	4.5 10/14/82
ALL=	rcp rexecd rmt route trpt
CFLAGS= -DTCP4_1b -DUCB_NET -DMENLO_JCL -Dpdp11 -O -I../../../sys  -I/usr/include
LIBC=	-lndir -lc
TYPE=	-n

all:	${ALL}

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

rcp:	rcp.o
	${CC} rcp.o ${TYPE} -o rcp -lnet -ljobs ${LIBC}

rexecd:	rexecd.o
	${CC} rexecd.o ${TYPE} -o rexecd -lnet -ljobs ${LIBC}

rmt:	rmt.o
	${CC} ${TYPE} -o rmt rmt.o -lnet -ljobs ${LIBC}

route:	route.o
	${CC} route.o -lnet ${TYPE} -o route ${LIBC}

trpt:	trpt.o
	${CC} trpt.o -lnet ${TYPE} -o trpt ${LIBC}

clean:
	rm -f ${ALL} *.o *.s

install: ${ALL}
	install -s rcp ${DESTDIR}/usr/ucb
	chown root ${DESTDIR}/usr/ucb/rcp
	chmod 4755 ${DESTDIR}/usr/ucb/rcp
	install -s rexecd ${DESTDIR}/etc
	install -s rmt ${DESTDIR}/etc
	install -s route ${DESTDIR}/etc
	install -s trpt ${DESTDIR}/usr/ucb
