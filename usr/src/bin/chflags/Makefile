#
# Public Domain.  12/3/1994 - Steven Schultz
#
#	@(#)Makefile	1.3 (2.11BSD) 2025/11/26
#
CFLAGS=	 -O
LDFLAGS= -i
VPATH= ../ls
SRCS=	chflags.c stat_flags.c
OBJS=	chflags.o stat_flags.o
MAN=	chflags.0
MANSRC=	chflags.1

all: chflags chflags.0

chflags: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

chflags.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} chflags tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: chflags
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	install -c -s -o root -g staff -m 755 chflags /bin/chflags

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
