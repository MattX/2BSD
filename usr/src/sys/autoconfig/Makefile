#       Makefile        2.0 (2.11BSD) 2025/11/25

OBJS=	attach.o do_config.o main.o read_dtab.o read_nlist.o uprobe.o	\
	brauto.o cnauto.o dhauto.o dhuauto.o drauto.o dzauto.o		\
	hkauto.o htauto.o lpauto.o raauto.o rkauto.o rlauto.o siauto.o	\
	tmauto.o tsauto.o xpauto.o rxauto.o dnauto.o tmsauto.o dhvauto.o
SRCS=	attach.c do_config.c main.c read_dtab.c read_nlist.c uprobe.c	\
	brauto.c cnauto.c dhauto.c dhuauto.c	\
	drauto.c dzauto.c hkauto.c htauto.c		\
	lpauto.c raauto.c rkauto.c rlauto.c		\
	siauto.c tmauto.c tsauto.c xpauto.c		\
	rxauto.c dnauto.c tmsauto.c dhvauto.c

CFLAGS=	-O -I. -I../h -I../pdpuba -I../pdp
LFLAG=	-i

autoconfig: ${OBJS}
	${CC} ${OBJS} -o $@

install:
	install -s -m 700 autoconfig /etc

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}

tags: FRC
	rm -f tags
	/usr/ucb/ctags ${SRCS}

clean: FRC
	rm -f autoconfig *.o

FRC:
