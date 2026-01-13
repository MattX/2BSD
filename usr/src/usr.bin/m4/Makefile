#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25

DEFS= 	-DEXTENDED -I.
CFLAGS=	-O ${DEFS}
LDFLAGS= -i
SRCS=	eval.c expr.c look.c main.c misc.c
OBJS=	eval.o expr.o look.o main.o misc.o

all:	m4 tags

m4:	${OBJS}
	cc ${LDFLAGS} -o m4 ${OBJS}

install: all
	install -c -s -m 755 m4 /usr/bin/m4

lint:
	lint -hax ${DEFS} ${SRCS}

tags:
	ctags -t *.h ${SRCS}

clean:
	-rm -f *.o m4 core

depend: ${SRCS}
	mkdep -p ${CFLAGS} ${SRCS}
