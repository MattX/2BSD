#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	syslogd.c
OBJS=	syslogd.o
MAN=	syslogd.0
MANSRC=	syslogd.8

all: syslogd syslogd.0

syslogd: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

syslogd.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} syslogd tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: syslogd syslogd.0
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 syslogd /usr/sbin/syslogd

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
