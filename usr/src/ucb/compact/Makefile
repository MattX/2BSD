CFLAGS=-O -I${INCLUDES}
INCLUDES=

all: compact uncompact

install:
	install -s -o bin -g bin -m 711 compact ${DESTDIR}/usr/ucb
	install -s -o bin -g bin -m 711 uncompact ${DESTDIR}/usr/ucb
	install -c -o bin -g bin -m 755 ccat.sh ${DESTDIR}/usr/ucb/ccat

compact: compact.o tree.o
	${CC} ${LFLAG} -O compact.o tree.o -o compact

uncompact: uncompact.o tree.o
	${CC} ${LFLAG} -O uncompact.o tree.o -o uncompact

clean:
	rm -f compact.o uncompact.o tree.o

.c.o:
	${CC} ${CFLAGS} -c $*.c

compact.o:	compact.c
uncompact.o:	uncompact.c
tree.o:		tree.c

