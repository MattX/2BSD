#	Makefile	4.5	83/07/01
#
CFLAGS=	-O ${FP}
OBJS=	setbrk.o builtin.o blok.o stak.o cmd.o fault.o main.o \
	word.o string.o name.o args.o xec.o service.o error.o io.o \
	print.o macro.o expand.o ctype.o msg.o

all:	sh

sh: ${OBJS}
	${CC} ${LFLAG} -o $@ ${OBJS}

install:
	install -s -o bin -g bin -m 711 sh ${DESTDIR}/bin

clean:
	rm -f ${OBJS} ${ALL}

setbrk.o:	setbrk.c
builtin.o:	builtin.c
blok.o:		blok.c brkincr.h
stak.o:		stak.c brkincr.h
cmd.o:		cmd.c
fault.o:	fault.c brkincr.h
main.o:		main.c brkincr.h
word.o:		word.c
string.o:	string.c
name.o:		name.c
args.o:		args.c
xec.o:		xec.c
service.o:	service.c
error.o:	error.c
io.o:		io.c
print.o:	print.c
macro.o:	macro.c
expand.o:	expand.c
ctype.o:	ctype.c
msg.o:		msg.c
