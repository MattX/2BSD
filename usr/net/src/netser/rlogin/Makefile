# %W% %G%
ALL=	rlogin rlogind
CFLAGS=	-DUCB_NET -DMENLO_JCL -Dpdp11 -I/usr/include
LIBC=	-lc
TYPE=	-n

all:	${ALL}

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

rlogin:	rlogin.o
	${CC} rlogin.o ${TYPE} -o rlogin -ljobs -lnet ${LIBC}

rlogind: rlogind.o
	${CC} rlogind.o ${TYPE} -o rlogind -ljobs -lnet ${LIBC}

clean:
	rm -f ${ALL} *.o *.s

install: ${ALL}
	install -s rlogin ${DESTDIR}/usr/ucb
	chown root ${DESTDIR}/usr/ucb/rlogin
	chmod 4755 ${DESTDIR}/usr/ucb/rlogin
	install -s rlogind ${DESTDIR}/etc
