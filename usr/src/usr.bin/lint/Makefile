#
# lint makefile
#

CFLAGS=	-O
M=	../../lib/mip
ALL=	lpass1 lpass2
OVALL=	ovlpass1 lpass2
OBJ=	cgram.o xdefs.o scan.o comm1.o pftn.o trees.o optim.o lint.o
OVOBJ=	cgram.o xdefs.o scan.o comm1.o pftn1.o pftn2.o filbuf.o \
	trees.o optim.o lint.o

all: ${ALL}

ovall: ${OVALL}

lpass1: ${OBJ}
	${CC} ${LFLAG} ${OBJ} -o $@

ovlpass1: ${OVOBJ}
	ld -X ${CRT} ${LFLAG} -o lpass1 \
	-Z cgram.o pftn1.o -Z comm1.o lint.o optim.o \
	-Z pftn2.o -Z scan.o filbuf.o -Z trees.o xdefs.o -L -lovc

lpass2: lpass2.o
	${CC} -o $@ ${LFLAG} lpass2.o

clean:
	rm -f ${OBJ} ${OVOBJ} ${ALL} lpass2.o

install: ${ALL}
	install -s -o bin -g bin -m 711 lpass2 ${DESTDIR}/usr/lib/lint2
	install -s -o bin -g bin -m 711 lpass1 ${DESTDIR}/usr/lib/lint1
	install -c -o bin -g bin -m 755 SHELL ${DESTDIR}/usr/bin/lint

trees.o: $M/manifest macdefs $M/mfile1 $M/trees.c
	${CC} -c ${CFLAGS} -I$M -I. $M/trees.c

optim.o: $M/manifest macdefs $M/mfile1 $M/optim.c
	${CC} -c ${CFLAGS} -I$M -I. $M/optim.c

pftn.o: $M/manifest macdefs $M/mfile1 $M/pftn.c
	${CC} -c ${CFLAGS} -I$M -I. $M/pftn.c

pftn1.o: $M/manifest macdefs $M/mfile1 $M/pftn1.c
	${CC} -c ${CFLAGS} -I$M -I. $M/pftn1.c

pftn2.o: $M/manifest macdefs $M/mfile1 $M/pftn2.c
	${CC} -c ${CFLAGS} -I$M -I. $M/pftn2.c

lint.o: $M/manifest macdefs $M/mfile1 lmanifest
	${CC} -c ${CFLAGS} -I$M -I. lint.c

scan.o: $M/manifest macdefs $M/mfile1 $M/scan.c
	${CC} -c ${CFLAGS} -I$M -I. $M/scan.c

xdefs.o: $M/manifest $M/mfile1 macdefs $M/xdefs.c
	${CC} -c ${CFLAGS} -I$M -I. $M/xdefs.c

cgram.o: $M/manifest $M/mfile1 macdefs cgram.c
	${CC} -c ${CFLAGS} -I$M -I. cgram.c

cgram.c: $M/cgram.y
	yacc $M/cgram.y
	mv y.tab.c cgram.c

comm1.o: $M/manifest $M/mfile1 $M/common macdefs $M/comm1.c
	${CC} -c ${CFLAGS} -I. -I$M $M/comm1.c

lpass2.o: $M/manifest lmanifest
	${CC} ${CFLAGS} -c -I$M -I. lpass2.c

filbuf.o: /usr/lib/libovc.a
	@ar x /usr/lib/libovc.a $@
