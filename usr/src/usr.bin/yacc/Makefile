#
#	@(#)Makefile	4.3	(2.11BSD) 2025/8/20
#
DESTDIR=
SEPFLAG= -i
CFLAGS=-O
XSTR=xstr
 
.c.o:
	cc -E ${CFLAGS} $*.c | ${XSTR} -c -
	cc ${CFLAGS} -c x.c
	mv x.o $*.o

all: yacc

yacc: y1.o y2.o y3.o y4.o strings.o
	$(CC) ${SEPFLAG} -o yacc y?.o strings.o

y1.o y2.o y3.o y4.o: dextern files

strings.o: strings
	${XSTR}
	cc -c xs.c
	mv xs.o strings.o

install: yacc yaccpar
	install -s -m 755 yacc $(DESTDIR)/usr/bin
	install -c yaccpar $(DESTDIR)/usr/share/misc
clean :
	-rm -f *.o yacc x.c xs.c
