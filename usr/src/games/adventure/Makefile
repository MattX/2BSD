#	Makefile	4.5 (2.11BSD) 2025/11/25

CFILES=	done.c init.c io.c main.c save.c subr.c vocab.c wizard.c
OBJS=	done.o init.o io.o main.o save.o subr.o vocab.o wizard.o
HFILES = hdr.h
SRCS = $(CFILES) $(HFILES) glorkz
CFLAGS=	-O
LDFLAGS=-i

advent:	a.out
	rm -f advent
	echo y | ./a.out

a.out: ${OBJS}
	${CC} ${LDFLAGS} ${CFLAGS} ${OBJS}

save.o:	save.c
	cc ${CFLAGS} -c save.c

install: advent
	install -c -s -m 755 advent /usr/games/adventure

lint:
	lint $(CFILES) >& lint.out

clean:
	rm -f ${OBJS} a.out advent core errs
