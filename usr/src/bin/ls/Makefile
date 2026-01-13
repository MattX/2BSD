#
# Public Domain.  12/3/1994 - Steven Schultz
#
#	@(#)Makefile	1.2 (2.11BSD) 2025/11/26
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	ls.c stat_flags.c
OBJS=	ls.o stat_flags.o

all: ls

ls: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} ls tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: ls
	install -c -s -o root -g staff -m 755 ls /bin/ls

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
