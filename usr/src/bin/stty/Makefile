#
# Public Domain.  1997/3/27 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/26
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	stty.c
OBJS=	stty.o
MAN=	stty.0
MANSRC=	stty.1

all: stty stty.0

stty: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

stty.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} stty tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: stty
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	install -c -s -o root -g staff -m 755 stty /bin/stty

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
