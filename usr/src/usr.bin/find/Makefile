#	@(#)Makefile	4.3.2	(2.11BSD)	2025/11/25
#
CFLAGS=	-O
LDFLAGS= -i

# The programs themselves
#
PROG=	find
LIBS=	bigram code

# Sources
#
SRCS=	find.c bigram.c code.c

all: ${PROG} ${LIBS}

find: find.o
	cc ${LDFLAGS} ${CFLAGS} -o find find.o

bigram: bigram.o
	cc ${LDFLAGS} ${CFLAGS} -o bigram bigram.o

code: code.o
	cc ${LDFLAGS} ${CFLAGS} -o code code.o

install: ${PROG} ${LIBS}
	install -c -s -m 755 ${PROG} /usr/bin/${PROG}
	-mkdir /var/db /usr/libexec/find
	chmod 755 /usr/libexec/find /var/db
	install -c -s -m 755 bigram /usr/libexec/find/bigram
	install -c -s -m 755 code /usr/libexec/find/code
	install -c -m 755 updatedb.csh /usr/libexec/find/updatedb

clean:
	rm -f *.o a.out core ${LIBS} ${PROG}

depend:
	chmod u+w Makefile
	mkdep ${CFLAGS} ${SRCS}
