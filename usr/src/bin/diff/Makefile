#	@(#)Makefile	4.1	2/9/83
#
DIFF=	/bin/diff
DIFFH=	/usr/lib/diffh
PR=	/bin/pr
CFLAGS=	-O
DFLAGS=	-DDIFF='"${DIFF}"' -DDIFFH='"${DIFFH}"' -DPR='"${PR}"'
DESTDIR=

SRCS=	diff.c diffdir.c diffreg.c
HDRS=	diff.h
OBJS=	diff.o diffdir.o diffreg.o
ALL=	diff diffh

all: ${ALL}

diff: ${OBJS}
	${CC} ${LFLAG} -o diff ${OBJS}

diffh: diffh.o
	${CC} ${LFLAG} -o diffh diffh.o

clean:
	rm -f ${ALL} ${OBJS} diffh.o

install:
	install -s -g bin -o bin -m 711 diff ${DESTDIR}/bin/diff
	install -s -g bin -o bin -m 711 diffh ${DESTDIR}/usr/lib/diffh

.c.o:
	${CC} ${CFLAGS} ${DFLAGS} -c $*.c

diff.o:		diff.c
diffdir.o:	diffdir.c
diffh.o:	diffh.c
diffreg.o:	diffreg.c
