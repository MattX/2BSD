#
# compat-4.1 makefile
#

CFLAGS=-O
OV=

COMPAT=	rand.o

libc.a: ${COMPAT}
	ar q ../$@ ${COMPAT}

libovc.a: ${COMPAT}
	ar q ../$@ ${COMPAT}

clean:
	rm -f ${COMPAT}

.c.o:
	${CC} ${CFLAGS} ${FP} ${OV} -c $*.c

rand.o:		rand.c
