#
# non-floating pointer crt makefile
#

CFLAGS=-O
OV=

CRT=	aldiv.o almul.o alrem.o cerror.o ldiv.o lmul.o lrem.o mcount.o

OVER=	csv.o

libc.a: ${CRT} ${OVER}
	ar q ../$@ ${CRT} ${OVER}

libovc.a: ${CRT}
	ar q ../$@ ${CRT}

clean:
	rm -f ${CRT} ${OVER}

.s.o:
	as ${OV} - -o $*.o $*.s

aldiv.o:	aldiv.s
almul.o:	almul.s
alrem.o:	alrem.s
cerror.o:	cerror.s
csv.o:		csv.s
ldiv.o:		ldiv.s
lmul.o:		lmul.s
lrem.o:		lrem.s
mcount.o:	mcount.s
