CFILES=		link.c pass2.c in.c sup.c 
OBJS=		link.o pass2.o in.o sup.o 
CFLAGS=		-O
LFLAG=		-n -s

all: 		l11

install:
	install -s -o bin -g bin -m 711 l11 $(DESTDIR)/usr/ucb

l11:		$(OBJS)
		${CC} -o l11 $(LFLAG) link.o pass2.o in.o sup.o

lint:
		lint link.c pass2.c in.c sup.c > FLUFF

obint:		obint.o in.o sup.o
		${CC} ${LFLAG} -o obint obint.o in.o sup.o

tags:		$(CFILES)
		ctags $(CFILES)

clean:
		rm -f *.o obint l11

list:
		pr link.h link.c pass2.c in.c sup.c obint.c | lpr
