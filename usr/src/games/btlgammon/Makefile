#	@(#)Makefile	4.3	(2.11BSD)	2025/11/25
#
CFLAGS=	-O
LDFLAGS= -i

btlgammon: btlgammon.c
	cc ${LDFLAGS} ${CFLAGS} -o btlgammon btlgammon.c

install: btlgammon backrules
	install -c -s -m 755 btlgammon /usr/games/btlgammon
	install -c backrules /usr/games/lib/backrules

clean:
	rm -f a.out core *.s *.o btlgammon

depend:	btlgammon.c
	mkdep ${CFLAGS} btlgammon.c
