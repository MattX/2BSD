#
#	@(#)Makefile	4.2	(Berkeley)	83/02/11
#
DESTDIR=
CFLAGS=-O
SRCS = Makefile dextern files yaccpar \
	y1.c y2.c y3.c y4.c yaccdiffs yaccnews
LFLAG=-i

all: yacc

yacc: y1.o y2.o y3.o y4.o
	$(CC) ${LFLAG} -o yacc y?.o

y1.o y2.o y3.o y4.o: dextern files

install:
	install -s -o bin -g bin -m 711 yacc $(DESTDIR)/usr/bin
	install -c -o bin -g bin -m 644 yaccpar $(DESTDIR)/usr/lib

clean :
	rm -f *.o yacc

srcs: $(SRCS)
$(SRCS):
	sccs get $@

y1.o:	y1.c
y2.o:	y2.c
y3.o:	y3.c
y4.o:	y4.c
