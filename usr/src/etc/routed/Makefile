# Makefile 10/14/82
ALL=	routed query trace
CFLAGS= -O
LIBC=

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

all:	$(ALL)

routed: routed.o af.o
	${CC} -o routed ${LFLAG} routed.o af.o

query:	query.o
	${CC} -o query ${LFLAG} query.o

trace:	trace.o
	${CC} -o trace ${LFLAG} trace.o

routed.o query.o: rip.h
routed.o af.o: router.h

install: $(ALL)
	install -s -o bin -g bin -m 755 routed ${DESTDIR}/etc
	install -s -o bin -g bin -m 755 query ${DESTDIR}/etc
	install -s -o bin -g bin -m 755 trace ${DESTDIR}/etc

lint:
	lint -hbxn routed.c af.c

clean:
	rm -f $(ALL) *.[os] core a.out
