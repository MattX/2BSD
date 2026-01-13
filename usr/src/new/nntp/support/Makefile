#       Makefile        2.0 (2.11BSD) 2025/11/25

OBJS	=	mkgrdates.o
SRCS	=	mkgrdates.c
HFILES	=	../common/conf.h

CFLAGS= -O

# Where these support programs will live

BIN = /usr/new/lib/news

all: mkgrdates

mkgrdates: mkgrdates.o
	$(CC) ${CFLAGS} -o mkgrdates mkgrdates.o

install: mkgrdates
	install -c -s -m  755 ${BIN}/mkgrdates

lint:
	lint mkgrdates.c

clean:
	-rm -f *.o mkgrdates

tags:	${SRCS} ${HFILES}
	ctags ${SRCS} ${HFILES}

${OBJS}:	${HFILES}
