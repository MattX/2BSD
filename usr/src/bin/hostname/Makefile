#
# Public Domain.  1995/03/13 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/26
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	hostname.c
OBJS=	hostname.o

all: hostname hostname.0

hostname: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

hostname.0: hostname.1
	/usr/man/manroff hostname.1 > hostname.0

clean:
	rm -f ${OBJS} tags hostname hostname.0

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: all
	install -s -o root -g staff -m 755 hostname /bin
	install -c -o root -g staff -m 444 hostname.0 /usr/man/cat1

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
