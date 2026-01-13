#	@(#)Makefile	4.3.2 (2.11BSD)	2025/11/27

CFLAGS= -O
LDFLAGS= -i

all:	spell hlist hlista hlistb hstop spellin spellout

install:	all
	install -c -s -m 755 spell /usr/libexec
	install -c -s -m 755 spellin /usr/bin
	install -c -s -m 755 spellout /usr/bin
	install -c -m 644 hlista /usr/dict
	install -c -m 644 hlistb /usr/dict
	install -c -m 644 hstop /usr/dict
	install -c -m 755 spell.sh /usr/bin/spell

clean:	
	rm -f *.o spell spellin hlista hlistb hlist hstop spellout

spell: spell.o
	cc ${LDFLAGS} spell.o -o spell
spellin: spellin.o
	cc ${LDFLAGS} spellin.o -o spellin
spellout: spellout.o
	cc ${LDFLAGS} spellout.o -o spellout

hlist: /usr/dict/words spellin
	spellin </usr/dict/words >hlist
hlista: american local hlist spellin
	(cat american local)|spellin hlist >hlista
hlistb: british local hlist spellin
	(cat british local)|spellin hlist >hlistb
hstop: stop spellin
	spellin <stop >hstop
