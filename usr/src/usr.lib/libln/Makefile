#	@(#)Makefile	4.3 (2.11BSD) 2025/11/27
#
CFLAGS = -O
SRCS=	allprint.c main.c reject.c yyless.c yywrap.c
OBJS=	allprint.o main.o reject.o yyless.o yywrap.o
TAGSFILE=tags

all: libln.a

libln.a: ${OBJS}
	ar rc libln.a ${OBJS}

install: all
	install -c -m 644 -o root libln.a /usr/lib
	rm -f /usr/lib/libl.a
	ln /usr/lib/libln.a /usr/lib/libl.a
	ranlib /usr/lib/libln.a

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f ${TAGSFILE} $$cwd/$$i; \
	done

clean:
	rm -f ${OBJS} libln.a tags
