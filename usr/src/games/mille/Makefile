#
#       Makefile        2.0 (2.11BSD) 2025/11/25
#
HEADERS=mille.h
CFILES=	comp.c end.c extern.c init.c mille.c misc.c move.c print.c \
	roll.c save.c types.c varpush.c
OBJS=	comp.o end.o extern.o init.o mille.o misc.o move.o print.o \
	roll.o save.o types.o varpush.o
POBJS=	comp.po end.po extern.po init.po mille.po misc.po move.po \
	roll.po print.po save.po types.po varpush.po
LIBS=	-lcurses -ltermlib
DEFS=
CFLAGS=	-O ${DEFS}
LDFLAGS= -i
BINDIR=	/usr/games

mille: ${OBJS}
	${CC} ${LDFLAGS} -o mille ${OBJS} ${LIBS}

install: mille
	install -c -s -m 755 -o root -g staff mille ${BINDIR}

lint:
	lint -hxb ${DEFS} ${CFILES} ${L} > lint.out

clean:
	rm -f ${OBJS} ${POBJS} a.out errs mille lint.out 
