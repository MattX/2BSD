#
# compat-4.3 makefile
#

CFLAGS=-O
OV=

COMPAT=	gettime.o utimes.o rename.o

libc.a: ${COMPAT}
	ar q ../$@ ${COMPAT}

libovc.a: ${COMPAT}
	ar q ../$@ ${COMPAT}

clean:
	rm -f ${COMPAT}

.c.o:
	${CC} ${CFLAGS} ${FP} ${OV} -c $*.c
