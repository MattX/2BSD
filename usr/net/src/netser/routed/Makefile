# Makefile 10/14/82
ALL=	rip.h routed.c query.c trace.c
CFLAGS= -DUCB_NET -DMENLO_JCL -Dpdp11 -O -I../../../sys -I/usr/include 
LIBC=	-lc
LDFLAGS= -n

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

all:	routed query trace

routed: routed.o af.o
	${CC} -o routed ${LDFLAGS} routed.o af.o -ljobs -lnet ${LIBC}

routed.o query.o: rip.h
routed.o af.o: router.h

query:	query.o
	${CC} -o query ${LDFLAGS} query.o -lnet ${LIBC}

trace:	trace.o
	${CC} -o trace ${LDFLAGS} trace.o -lnet ${LIBC}

install: routed trace query
	install -s routed /etc

lint:
	lint -hbxn routed.c af.c

clean:
	rm -f routed *.o core a.out t.c query linterrs trace typescript errs
