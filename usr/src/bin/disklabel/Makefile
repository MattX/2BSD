#	@(#)Makefile	1.1 (2.11BSD) 2025/12/25

CFLAGS=-O
LDFLAGS=-i
MANDIR=/usr/man/cat

all: disklabel disklabel.5.0 disklabel.0

disklabel: disklabel.o
	cc ${CFLAGS} ${LDFLAGS} -o $@ $@.o -lutil

install: all
	install -s -c -o root -g operator -m 2755 disklabel \
			/bin/disklabel
	install -c -o root -g staff -m 444 disklabel.5.0 \
			${MANDIR}5/disklabel.0
	install -c -o root -g staff -m 444 disklabel.0 \
			${MANDIR}8/disklabel.0

clean:
	rm -f *.o *.0 *.core disklabel

disklabel.0: disklabel.8
	/usr/man/manroff disklabel.8 > disklabel.0

disklabel.5.0: disklabel.5.5
	/usr/man/manroff disklabel.5.5 > disklabel.5.0
