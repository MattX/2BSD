#
# crash makefile
#

CFLAGS=	-O
OBJS=	crash.o crashsubs.o symbol.o dispnet.o

ALL=	crash

all: ${ALL}

crash: ${OBJS}
	${CC} ${LFLAG} ${OBJS} -o $@

install: ${ALL}
	install -s -o bin -g bin -m 711 crash /usr/local

clean:
	rm -f ${OBJS} ${ALL}

crash.o:	crash.c crash.h
crashsubs.o:	crashsubs.c crash.h
dispnet.o:	dispnet.c crash.h
symbol.o:	symbol.c crash.h
