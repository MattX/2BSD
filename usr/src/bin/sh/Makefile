#	Makefile	4.6	(2.11BSD) 2025/11/25
#
CFLAGS = -O -w
LDFLAGS= -i

all:	sh

sh:	setbrk.o
sh:	builtin.o blok.o stak.o
sh:	cmd.o fault.o main.o
sh:	word.o string.o name.o args.o
sh:	xec.o service.o error.o io.o
sh:	print.o macro.o expand.o
sh:	ctype.o msg.o
blok.o:		brkincr.h
fault.o:	brkincr.h
main.o:		brkincr.h
stak.o:		brkincr.h

sh:
	cc ${LDFLAGS} -o sh *.o

install: all
	install -c -s -m 755 -o root sh /bin
clean:
	rm -f sh *.o
