DESTDIR=

CFLAGS=	-O
SEPFLAG=-i

SRCS=	shar.c traverse.c
OBJS=	shar.o traverse.o

.SUFFIXES: .1 .0

.1.0:
	/usr/man/manroff $*.1 > $*.0

shar: ${OBJS} Makefile
	cc ${CFLAGS} ${SEPFLAG} -o shar ${OBJS}

traverse:
	cc -o traverse -DSTANDALONE traverse.c

lint:
	lint -h ${SRCS}

install: shar shar.0
	install -s shar ${DESTDIR}/usr/new/shar
	install -c shar.0 ${DESTDIR}/usr/new/man/cat1/shar.0

tags: ${SRCS}
	ctags $<

clean:
	rm -f *.o shar traverse core tags *.0
