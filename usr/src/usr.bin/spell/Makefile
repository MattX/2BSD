#
# spell makefile
#
# spell and its hash lists must be reduced in size to run on a non-separate
# I/D machine.  This is done by defining NONSEPARATE (in <sys/localopts.h>)
# instead of using the -i compiler flag.
# FMT should also be set to -n for nonseparate I/D.

CFLAGS=	-O
LFLAG=	-i
ALL=	spell hlist hlista hlistb hstop spellin

all: ${ALL}

install: ${ALL}
	install -s -o bin -g bin -m 711 spell ${DESTDIR}/usr/lib
	install -o bin -g bin -m 711 hlista ${DESTDIR}/usr/dict
	install -o bin -g bin -m 711 hlistb ${DESTDIR}/usr/dict
	install -o bin -g bin -m 711 hstop ${DESTDIR}/usr/dict

spell: spell.c spell.h
	${CC} ${LFLAG} -O spell.c -o $@

spellin: spellin.c spell.h
	${CC} ${LFLAG} -O spellin.c -o $@

spellout: spellout.c spell.h
	${CC} ${LFLAG} -O spellout.c -o $@

hlist: /usr/dict/words spellin
	spellin </usr/dict/words >hlist

hlista: american local hlist spellin
	(cat american local) | spellin hlist >hlista

hlistb: british local hlist spellin
	(cat british local) | spellin hlist >hlistb

hstop: stop spellin
	spellin < stop > hstop

clean:
	rm -f *.o ${ALL}
