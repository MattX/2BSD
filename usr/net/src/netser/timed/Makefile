# %W% %G%
ALL=	timed setclock
CFLAGS= -DUCB_NET -DMENLO_JCL -Dpdp11 -O -I/usr/include
TYPE=	-n
LIBC=	-ljobs -lc

all:	${ALL}

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

timed: timed.o
	${CC} timed.o ${TYPE} -o timed -lnet ${LIBC}

setclock: setclock.o
	${CC} setclock.o ${TYPE} -o setclock -lnet ${LIBC}

