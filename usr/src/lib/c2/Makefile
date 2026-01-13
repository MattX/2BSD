#       Makefile        2.0 (2.11BSD) 2025/11/25

LDFLAGS=-i
CFLAGS=	-O

LIB=	/lib

c2: c20.o c21.o
	cc ${CFLAGS} ${LDFLAGS} -o c2 c20.o c21.o

c20.o c21.o: c2.h

install: c2
	-mv ${LIB}/c2 ${LIB}/oc2
	install -s c2 ${LIB}

restore:
	mv ${LIB}/oc2 ${LIB}/c2

clean:
	rm -f *.o c2
