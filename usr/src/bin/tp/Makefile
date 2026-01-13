#	@(#)Makefile	4.2 (2.11BSD) 2025/11/29

CFLAGS=-O
LDFLAGS= -i

h = tp.h tp_defs.h
o  = tp0.o tp1.o tp2.o tp3.o

tp :	$o
	$(CC) ${LDFLAGS} -o tp $o

$o :	$h

install : tp
	install -c -s -m 755 -o root tp /bin

clean : 
	rm -f *.o tp
