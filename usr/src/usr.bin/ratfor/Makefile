#
# ratfor makefile
#
# FP set to -f on machines without floating point
# BLOCKIF set to -DBLOCKIF to emit f77 block if stmts

FP=
BLOCKIF=	-DBLOCKIF
CFLAGS=		-O
LFLAG=		-i
YFLAGS=		-d
SRCS=		r0.c r1.c r2.c rgram.c rio.c rlex.c rlook.c
OBJS=		r0.o r1.o r2.o rgram.o rio.o rlex.o rlook.o
ALL=		ratfor

all:	${ALL}

ratfor:	${OBJS}
	${CC} ${CFLAGS} ${LFLAG} -o $@ ${OBJS}

install: ${ALL}
	install -s -o bin -g bin -m 711 ratfor ${DESTDIR}/usr/bin

clean:
	rm -f y.tab.h rgram.c ${ALL} ${OBJS}

.c.o:
	${CC} ${FP} ${BLOCKIF} ${CFLAGS} -c $*.c

y.tab.h:	rgram.o
r0.o:		r.h y.tab.h r0.c
r1.o:		r.h y.tab.h r1.c
r2.o:		r.h y.tab.h r2.c
rgram.o:	rgram.y
rio.o:		r.h y.tab.h rio.c
rlook.o:	r.h y.tab.h rlook.c
rlex.o:		r.h y.tab.h rlex.c
