#       Makefile        2.0 (2.11BSD) 2025/11/25
CFLAGS=	-O
LDFLAGS=-i

SRCS=	shar.c traverse.c
OBJS=	shar.o traverse.o

.SUFFIXES: .3 .1 .0

.1.0:
	/usr/man/manroff $*.1 > $*.0

.3.0:
	/usr/man/manroff $*.3 > $*.0

all: shar traverse shar.0 traverse.0

shar: ${OBJS}
	cc ${LDFLAGS} -o shar ${OBJS}

traverse: traverse.c
	cc ${CFLAGS} -o traverse -DSTANDALONE traverse.c

lint:
	lint -h ${SRCS}

install: shar shar.0
	install -c -s -m 755 -o root shar /usr/bin/shar
	install -c -s -m 755 -o root traverse /usr/bin/traverse
	install -c -m 444 -o root -g staff shar.0 /usr/man/cat1
	install -c -m 444 -o root -g staff traverse.0 /usr/man/cat3

tags: ${SRCS}
	ctags $<

clean:
	rm -f *.o shar traverse core tags *.0
