#       Makefile        2.1 (2.11BSD) 2025/11/25
CFLAGS=	-O
LDFLAGS= -i
SRCS=	mkovmake.c
OBJS=	mkovmake.o
MANSRC=	mkovmake.1
MAN=	mkovmake.0

all: mkovmake ${MAN}

mkovmake: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${CFLAGS} ${OBJS}

clean:
	rm -f ${OBJS} core mkovmake ${MAN}

install: all ${MAN}
	install -c -s -o root -g staff -m 755 mkovmake /usr/local
	install -c -m 444 -o root -g staff ${MAN} /usr/local/man/cat1

${MAN}: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

lint: ${SRCS}
	lint ${CFLAGS} ${SRCS}
