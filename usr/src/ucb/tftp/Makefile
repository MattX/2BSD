# Makefile 10/14/82
ALL=	tftp
CFLAGS=	-O
LFLAG=	-n

all: ${ALL}

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

tftp:	main.o tftp.o
	${CC} main.o tftp.o ${LFLAG} -o tftp

clean:
	rm -f ${ALL} *.[os] core a.out

install: ${ALL}
	install -s -o bin -g bin -m 755 tftp ${DESTDIR}/usr/ucb
