#	Makefile	4.2	(2.11BSD) 2025/11/29
#
CFLAGS=-O -w
LDFLAGS=-i

sed:	sed0.o sed1.o
	cc ${LDFLAGS} -o sed *.o

sed0.o: sed0.c sed.h
sed1.o: sed1.c sed.h

install: sed
	install -c -s -m 755 -o root sed /bin
clean:
	rm -f *.o sed
