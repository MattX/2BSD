#       Makefile        2.0 (2.11BSD) 2025/11/29

SRCS	=	inews.c uname.c ../common/clientlib.c ../common/version.c
OBJS	=	inews.o uname.o ../common/clientlib.o ../common/version.o
HFILES	=	../common/nntp.h ../common/conf.h

CFLAGS	= -O

inews:  $(OBJS) $(HFILES)
	$(CC) $(CFLAGS) $(OBJS) -o inews

uname.o: ../common/conf.h

../common/clientlib.o:
	${CC} ${CFLAGS} -DNNTPSRC -c ../common/clientlib.c
	mv clientlib.o ../common/clientlib.o

../common/version.o:
	${CC} ${CFLAGS} -c ../common/version.c
	mv version.o ../common/version.o

install: inews
	cp inews /usr/new/rn/inews
	chmod 755 /usr/new/lib/rn/inews

clean:
	-rm -f ${OBJS} inews make*.out

lint:
	lint ${SRCS} ${HFILES}

tags:
	ctags $(CFILES) $(HFILES)
