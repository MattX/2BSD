#       Makefile        2.0 (2.11BSD) 2025/11/25
LIBDIR=	/usr/games/lib
CFLAGS=	-O

OBJ=	agen.o att.o bgen.o bheur.o bmove.o book.o bplay.o ctrl.o data.o\
	init.o io.o mater.o play.o qsort.o savres.o setup.o stat.o\
	stdin.o wgen.o wheur.o wmove.o wplay.o

chess: ${OBJ}
	${CC} -n -o $@ ${OBJ}

init.o:
	${CC} -c -O -DBOOK=\"${LIBDIR}/book\" init.c

${OBJ}: old.h

install: chess
	install -c -s -m 755 chess /usr/games
#	install -c -m 644 book ${LIBDIR}/book

clean:
	rm -f *.o a.out chess core
