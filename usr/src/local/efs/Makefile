ALL=	efsinit efsd
CFLAGS=-O -DDEBUG

all: ${ALL}

.c.o:
	cc -S ${CFLAGS} $*.c
	sed -f asm.sed $*.s | as -o $*.o
	rm -f $*.s

efsd: efsd.o
	cc efsd.o -o efsd ../raddr.o -ljobs

efsinit: efsinit.o
	cc efsinit.o -o efsinit

clean:
	rm -f ${ALL} *.o *.s

install: ${ALL}
	install -s -o root -g su -m 4755 efsd /etc
	install -s -o root -g su -m 4755 efsinit /etc
