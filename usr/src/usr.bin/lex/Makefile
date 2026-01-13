#	Makefile	4.1.2 (2.11BSD)	2025/11/27
#
CFLAGS = -O
LDFLAGS= -i

all: lex

lex: lmain.o y.tab.o sub1.o sub2.o header.o
	cc ${LDFLAGS} lmain.o y.tab.o sub1.o sub2.o header.o -o lex

smallex:
	cc ${LDFLAGS} lmain.o y.tab.o sub1.o sub2.o header.o -o lex

y.tab.c: parser.y
	yacc parser.y
lmain.o:lmain.c ldefs.c once.c
	cc -c -O lmain.c
sub1.o: sub1.c ldefs.c
	cc -c -O sub1.c
sub2.o: sub2.c ldefs.c
	cc -c -O sub2.c
header.o: header.c ldefs.c
	cc -c -O header.c

install: all
	-mkdir -p /usr/libdata/lex
	chmod 755 /usr/libdata/lex
	install -c -s -m 755 lex /usr/bin
	install -c -m 444 ncform /usr/libdata/lex
	install -c -m 444 nrform /usr/libdata/lex

clean:
	rm -f *.o lex y.tab.c
