#
# Public Domain.  2/4/1995 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	uname.c
OBJS=	uname.o
MAN=	uname.0
MANSRC=	uname.1

all: uname uname.0

uname: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

uname.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} uname tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	install -c -s -o root -g staff -m 755 uname /usr/bin/uname

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
