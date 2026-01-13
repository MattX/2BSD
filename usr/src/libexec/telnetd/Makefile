#
#	@(#)Makefile	2.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	telnetd.c gettytab.c subr.c init.c
OBJS=	telnetd.o gettytab.o subr.o init.o
MAN=	telnetd.0
MANSRC=	telnetd.8
VPATH=	../getty

all: telnetd telnetd.0

telnetd: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

${OBJS}: ../getty/gettytab.h

telnetd.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} telnetd tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: telnetd ${MAN}
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 telnetd /usr/libexec/telnetd

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
