#
# 1997/6/25 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/27
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	id.c
OBJS=	id.o
MAN=	id.0 groups.0 whoami.0

.SUFFIXES: .0 .1

.1.0:
	/usr/man/manroff $*.1 > $@

all: id ${MAN}

id: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} ${MAN} id tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: id
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat1
	install -c -s -o root -g staff -m 755 id /usr/bin/id
	install -c -o root -g staff -m 755 groups.sh /usr/bin/groups
	install -c -o root -g staff -m 755 whoami.sh /usr/bin/whoami

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
