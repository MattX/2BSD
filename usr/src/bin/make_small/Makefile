#
# description file for the Make command
#

FILES=	makefile ident.c defs main.c doname.c misc.c files.c dosys.c \
	gram.y gcos.c
OBJS=	ident.o main.o doname.o misc.o files.o dosys.o gram.o
CFLAGS=	-O ${FP}
LFLAG=	-I
ALL=	make_small

all: ${ALL}

make_small: ${OBJS}
	${CC} ${LFLAG} ${OBJS} -o make_small

${OBJS}: defs

clean:
	rm -f *.o gram.c ${ALL}

install:
	install -s -o bin -g bin -m 711 make_small ${DESTDIR}/bin
