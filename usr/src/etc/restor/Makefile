#
# restor makefile - SEPFLAG must be -i, restor will not run unless built -i
#

SEPFLAG= -i
DEFS= 
CFLAGS=	-O -I. ${DEFS}

SRCS=	restor.c
OBJS=	restor.o

restor: ${OBJS}
	${CC} ${SEPFLAG} ${OBJS} -o restor

install: restor
	install -s restor ${DESTDIR}/etc

clean:
	rm -f *.o a.out core errs restor

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f tags $$cwd/$$i; \
	done

lint:
	lint -haxcz restor.c
