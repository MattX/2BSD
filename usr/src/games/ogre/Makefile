OBJ = init.o termcap.o map.o main.o move.o initround.o ogrecom.o ogrestat.o \
	  attack.o resolve.o
DOBJ = dinit.o dtermcap.o dmap.o dmain.o dmove.o dinitround.o dogrecom.o dogrestat.o \
	  dattack.o dresolve.o

CFLAGS=	-Dpdp11

ogre: $(OBJ)
	cc -o ogre $(OBJ) -ltermcap

debug: $(DOBJ)
	cc -g -o ogre $(OBJ) -ltermcap

init.o: init.c ogre.h
	cc -c init.c

dinit.o: init.c ogre.h
	cc -g -c init.c

main.o: main.c ogre.h
	cc -c main.c

dmain.o: main.c ogre.h
	cc -g -c main.c

move.o: move.c ogre.h
	cc -c move.c

dmove.o: move.c ogre.h
	cc -g -c move.c

dattack.o: attack.c ogre.h
	cc -g -c attack.c

attack.o: attack.c ogre.h
	cc -c attack.c

dresolve.o: resolve.c ogre.h
	cc -g -c resolve.c

resolve.o: resolve.c ogre.h
	cc -c resolve.c

dinitround.o: initround.c ogre.h
	cc -g -c initround.c

initround.o: initround.c ogre.h
	cc -c initround.c

dtermcap.o: termcap.c
	cc -g -c termcap.c

termcap.o: termcap.c
	cc -c termcap.c

dogrecom.o: ogrecom.c ogre.h
	cc -g -c ogrecom.c

ogrecom.o: ogrecom.c ogre.h
	cc -c ogrecom.c

dogrestat.o: ogrestat.c ogre.h
	cc -g -c ogrestat.c

ogrestat.o: ogrestat.c ogre.h
	cc -c ogrestat.c

map.o: map.c ogre.h
	cc -c map.c

dmap.o: map.c ogre.h
	cc -g -c map.c

backup:
	tar cvf /arch/mike/ogre.tar *

rcs: init.c termcap.c map.c main.c move.c initround.c ogrecom.c ogrestat.c \
	  attack.c resolve.c ogre.6
	ci -l $?
	touch rcs
