# %W% %G%
ALL=	ruptime rwho rwhod 
CFLAGS= -DUCB_NET -DMENLO_JCL -Dpdp11 -O -I/usr/include
# rwho is always seperate I&D
TYPE=	-n
LIBC=	-lndir -lc

all:	${ALL}

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

ruptime.o rwho.o rwhod.o: rwhod.h

ruptime: ruptime.o
	${CC} ruptime.o ${TYPE} -o ruptime -ltermlib -lnet ${LIBC}

rwho:	rwho.o
	${CC} rwho.o -n -o rwho -lnet ${LIBC}

rwhod:	rwhod.o
	${CC} rwhod.o ${TYPE} -o rwhod -ljobs -lnet ${LIBC}

clean:
	rm -f ${ALL} *.o *.s

install: ${ALL}
	install -s rwho ${DESTDIR}/usr/ucb
	install -s rwhod ${DESTDIR}/etc
	install -s ruptime ${DESTDIR}/usr/ucb
