#
# Public Domain.  12/3/1994 - Steven Schultz
#
#	@(#)Makefile	1.0 (2.11BSD GTE) 12/3/94
#
CFLAGS=	 -O
SEPFLAG= -i
SRCS=	ls.c stat_flags.c strsep.c
OBJS=	ls.o stat_flags.o strsep.o

all: ls

ls: ${OBJS}
	${CC} ${CFLAGS} ${SEPFLAG} -o $@ ${OBJS}

clean:
	rm -f ${OBJS} ls tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: ls
	install -s -o root -g bin -m 755 ls ${DESTDIR}/bin/ls

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
