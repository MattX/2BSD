# %W% %G%
ALL=	named
CFLAGS= -DUCB_NET -DMENLO_JCL -Dpdp11 -O -I/usr/include
TYPE=	-n
LIBC=	-ljobs -lc

all:	${ALL}

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

named: named.o
	${CC} named.o ${TYPE} -o named -lnet ${LIBC}

