# Makefile 10/14/82
ALL=	tftp tftpd
CFLAGS= -DGOO -DUCB_NET -DMENLO_JCL -Dpdp11 -O -I/usr/include
TYPE=	-n
LIBC=	-lc

all: ${ALL}

.c.o:
	${CC} -S ${CFLAGS} $*.c
	sed -f ../asm.sed $*.s > $*.S
	as - -o $*.o $*.S
	rm -f $*.s $*.S

tftp:	main.o tftp.o
	${CC} main.o tftp.o ${TYPE} -o tftp -ljobs -lnet ${LIBC}

tftpd:	tftpd.o
	${CC} tftpd.o ${TYPE} -o tftpd -ljobs -lnet ${LIBC}

clean:
	rm -f ${ALL} *.o *.s errs core a.out t.?

install: tftp tftpd
	install -s tftp ${DESTDIR}/usr/ucb
	install -s tftpd ${DESTDIR}/etc
