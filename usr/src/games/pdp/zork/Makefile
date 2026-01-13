#
# @(#)Makefile  1.2 (2.11BSD) 2025/11/25
#

BINDIR = /usr/games
LIBDIR = /usr/games/lib

CFLAGS = -O
LDFLAGS= -i

all:	z zork

z:	z.c
	cc -O ${LDFLAGS} -o z z.c

# All the 1.o stuff is just to do a . = . + 140000
zork:	1.o o.o 
	ld 1.o 1.o 1.o o.o
	nm a.out >syms
	dd if=a.out of=h bs=16 count=1
	dd if=a.out of=b.out bs=26120 skip=2
	cat h d b.out >zork
	rm a.out h b.out
	chmod 755 zork
	./p1

install: z zork
	install -c -s -m 755 z ${BINDIR}/zork
	install -c -s -m 755 zork ${LIBDIR}/zork
	install -c -m 644 doverlay ${LIBDIR}
	install -c -m 644 dindex.dat ${LIBDIR}
	install -c -m 644 dtext.dat ${LIBDIR}

clean:
	rm -f 1.out a.out b.out h *.o core dungeon.dat zork z syms

1.o:	1.s
o.o:	o.s

.s.o:
	/lib/cpp -t -E $< | sed -e 's;^#;/;' | as -o $@
