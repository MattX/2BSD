#	@(#)Makefile	4.1	12/24/82
#
DESTDIR=
CC = /bin/cc
CFLAGS = -O -Dpdp11

all:	boggle bogdict

bogdict: comp sfile
	sed -f sfile /usr/local/lib/w2a | uniq | comp >bogdict

install: boggle bogdict
	install boggle ${DESTDIR}/usr/games/boggle
	install bogdict ${DESTDIR}/usr/games/bogdict

boggle: boggle.c
	cc -o boggle ${CFLAGS} boggle.c

comp: comp.c
	cc -o comp ${CFLAGS} comp.c

clean:
	rm -f bogdict boggle comp
