#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	announce.c print.c process.c table.c talkd.c
OBJS=	announce.o print.o process.o table.o talkd.o
MAN=	talkd.0
MANSRC=	talkd.8

all: talkd talkd.0

talkd: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

talkd.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} talkd tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: talkd
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 talkd /usr/libexec/ntalkd

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
