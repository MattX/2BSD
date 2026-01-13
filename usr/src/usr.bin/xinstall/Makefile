#
# Public Domain.  1996/2/20 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
VPATH=	/usr/src/bin/ls
SRCS=	xinstall.c stat_flags.c
OBJS=	xinstall.o stat_flags.o
MAN=	install.0
MANSRC=	install.1

all: xinstall ${MAN}

xinstall: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

install.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} xinstall tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	install -c -s -o root -g staff -m 755 xinstall /usr/bin/install

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
