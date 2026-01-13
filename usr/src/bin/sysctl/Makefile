#
# Public Domain.  1/15/1995 - Steven Schultz
#
#	@(#)Makefile	1.3 (2.11BSD) 2025/11/29
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	sysctl.c
OBJS=	sysctl.o
MAN=	sysctl.0
MANSRC=	sysctl.8

all: sysctl sysctl.0

sysctl: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

sysctl.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} sysctl tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 sysctl /bin/sysctl

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
