#
#	@(#)Makefile	1.13	(Berkeley)	3/29/86
#
M=/usr/src/lib/mip
P=/usr/src/lib/pcc
VPATH=	$M
XSTR=	/usr/ucb/xstr

# turn off the debugging to save string space
CFLAGS=-O -I$M -I. -DBUG1 -DBUG2 -DBUG3 -DBUG4
LFLAGS=-i
SRCS=	$M/cgram.y $M/common.c $M/optim.c $M/pftn.c $M/scan.c $M/trees.c \
	$M/xdefs.c lint.c hash.c $M/pass1.h $M/manifest.h macdefs.h \
	lpass2.c

.c.o:
	${CC} -E ${CFLAGS} $*.c | ${XSTR} -c -
	${CC} -c ${CFLAGS} x.c
	mv -f x.o $*.o
	rm -f x.c

all:	lpass1 lpass2

lpass1: cgram.o comm1.o optim.o pftn.o scan.o trees.o xdefs.o \
	lint.o hash.o strings.o
	${CC} ${LFLAGS} ${CFLAGS} cgram.o comm1.o optim.o pftn.o \
		scan.o trees.o xdefs.o lint.o hash.o strings.o -o lpass1

cgram.o: $M/manifest.h $M/pass1.h pcclocal.h macdefs.h cgram.c

strings.o: strings
	${XSTR}
	${CC} -c xs.c
	mv -f xs.o strings.o
	rm -f xs.c

comm1.o: $M/manifest.h $M/pass1.h pcclocal.h $M/common.c macdefs.h
	ln $M/common.c comm1.c
	${CC} -E ${CFLAGS} -DPASS1COMMON comm1.c | ${XSTR} -c -
	${CC} -c ${CFLAGS} -DPASS1COMMON x.c
	mv -f x.o comm1.o
	rm -f x.c
	rm -f comm1.c

optim.o: $M/manifest.h macdefs.h $M/pass1.h pcclocal.h $M/optim.c

pftn.o: $M/manifest.h macdefs.h $M/pass1.h pcclocal.h $M/pftn.c

scan.o: $M/manifest.h macdefs.h $M/pass1.h pcclocal.h $M/scan.c

trees.o: $M/manifest.h macdefs.h $M/pass1.h pcclocal.h $M/trees.c

xdefs.o: $M/manifest.h $M/pass1.h pcclocal.h macdefs.h $M/xdefs.c

cgram.c: $M/cgram.y pcctokens
	cat pcctokens $M/cgram.y > gram.in
	$(YACC) gram.in
	fgrep -v "static char yaccpar_sccsid" y.tab.c >cgram.c
	rm -f y.tab.c

GREP=	egrep

pcclocal.h: $P/localdefs.h /usr/include/pcc.h
	rm -f pcclocal.h
	cat /usr/include/pcc.h $P/localdefs.h | $(GREP) '^#[ 	]*(define[ 	][ 	]*PCC(F|T|TM|OM)?_|ifdef|ifndef|endif)' | sed -e 's/PCC[A-Z]*_//' > pcclocal.h 

pcctokens: $P/localdefs.h /usr/include/pcc.h
	rm -f pcctokens
	cat /usr/include/pcc.h $P/localdefs.h | $(GREP) '^#[ 	]*define[ 	][ 	]*PCC_' | sed -e 's/^#[ 	]*define[ 	][ 	]*PCC_/%term	/' > pcctokens

lint.o: $M/manifest.h macdefs.h $M/pass1.h lmanifest.h

lpass2: lpass2.o hash.o
	${CC} ${LFLAGS} ${CFLAGS} lpass2.o hash.o -o lpass2

lpass2.o: $M/manifest.h lmanifest.h pcclocal.h macdefs.h
	${CC} ${CFLAGS} -c -I$M -I. lpass2.c

hash.o: hash.c
	${CC} ${CFLAGS} -c -I$M -I. hash.c

lintall:
	lint -hpav -I. -I$M  -DPASS1COMMON cgram.c $M/xdefs.c $M/scan.c \
		$M/common.c $M/pftn.c $M/trees.c $M/optim.c hash.c lint.c

install: all SHELL
	install -s lpass1 ${DESTDIR}/usr/lib/lint/lint1
	install -s lpass2 ${DESTDIR}/usr/lib/lint/lint2
	./libs
	-rm -f ${DESTDIR}/usr/lib/lint/llib-ltermlib*
	ln -s llib-ltermcap ${DESTDIR}/usr/lib/lint/llib-ltermlib
	ln -s llib-ltermcap.ln ${DESTDIR}/usr/lib/lint/llib-ltermlib.ln
	install -c SHELL ${DESTDIR}/usr/bin/lint

shrink:
	rm -f *.o errs pcctokens pcclocal.h x.c xs.c strings

clean: shrink
	rm -f lpass1 lpass2 cgram.c gram.in tags

tags:	${SRCS}
	ctags -w ${SRCS}
