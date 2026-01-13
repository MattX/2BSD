#
# Public Domain.  1996/6/25 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	ctimed.c
OBJS=	ctimed.o

all: ctimed

ctimed: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} ctimed tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: ctimed
	install -c -s -o root -g staff -m 755 ctimed /usr/libexec/ctimed

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
