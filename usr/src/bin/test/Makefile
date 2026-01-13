#       @(#)Makefile    1.3 (2.11BSD) 2025/11/25
#
#	1.1 	Robin Birch	11 march 1995
#	1.2 	Steven Schultz	27 october 1996
#       1.3     Steven Schulz   25 november 2025

LDFLAGS= -i
CFLAGS = -O

SRCS = test.c operators.c
OBJS = test.o operators.o

all:	test test.0

test:	${OBJS}
	${CC} ${LDFLAGS} -o test ${OBJS}

test.0: test.1
	/usr/man/manroff test.1 > test.0

install: all
	install -s -m 755 -o root -g staff test /bin
	rm -f /bin/[
	ln /bin/test /bin/[
	install -c -m 444 -o root -g staff test.0 /usr/man/cat1

clean:	
	rm -f *.o test *.core test.0
