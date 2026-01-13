#
# Public Domain.  1995/05/02 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	printf.c
OBJS=	printf.o
MAN=	printf.0
MANSRC=	printf.1

all: printf printf.0

printf: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

printf.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} printf tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	install -c -s -o root -g staff -m 755 printf /usr/bin/printf

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
