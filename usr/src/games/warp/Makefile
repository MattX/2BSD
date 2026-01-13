# @(#)Makefile    1.0 (2.11BSD) 2025/3/31
#
# Revision 7.0.1.1a 87/26/08  03:45:03	games
# Fix minor shell continuation bug and put in a pdp-11 overlay scheme.
#
# Revision 7.0.1.1  86/12/12  16:54:03  lwall
# Frozen for net release.
# 
# Revision 7.0  86/10/08  15:11:42  lwall
# Split into separate files.  Added amoebas and pirates.
# 
#
CC = cc
bin = /usr/games
mansrc = /usr/man/cat6
CFLAGS = -O
LDFLAGS = -i
CHOWNER = daemon
privlib = /usr/games/lib/warp

libs =  -ltermlib  /usr/lib/libtermlib.a -lm

util = smap.0 smap.1 smap.2 smap.3 smap.4 smap.5 smap.6 smap.7

INCS = bang.h init.h intrp.h move.h ndir.h patchlevel.h object.h play.h \
       score.h sig.h term.h them.h us.h util.h version.h warp.h weapon.h

SRCS = bang.c init.c intrp.c move.c object.c play.c score.c sig.c term.c \
       them.c us.c util.c version.c warp.c weapon.c

OBJS = bang.o init.o intrp.o move.o object.o play.o score.o sig.o term.o \
       them.o us.o util.o version.o warp.o weapon.o

lintflags = -hbxac

SHELL = /bin/sh

.c.o:
	$(CC) -c $(CFLAGS) $*.c

all: warp $(util) warp.0

warp: $(OBJS)
	ld -X -i /lib/crt0.o play.o score.o version.o warp.o \
		weapon.o  init.o intrp.o \
		-Z sig.o util.o object.o term.o \
		-Z them.o us.o \
		-Z bang.o move.o \
		-Y $(libs) -lc -o warp

warp.0: warp.man
	/usr/man/manroff warp.man > warp.0

install: all
	-mkdir -p $(privlib)
	chown bin $(privlib)
	chmod 755 $(privlib)
	install -s -o bin -m 4711 warp /usr/games/warp
	install -c -o bin -m 644 $(util) warp.doc warp.news $(privlib)
	install -c -o bin -g bin -m 444 warp.0 ${DESTDIR}/usr/man/cat6

clean:
	rm -f warp $(OBJS) $(util) sm warp.0

lint:
	lint $(lintflags) $(SRCS) > warp.fuzz

smap.0: smp.0 sm
	./sm <smp.0 >smap.0
smap.1: smp.1 sm
	./sm <smp.1 >smap.1
smap.2: smp.2 sm
	./sm <smp.2 >smap.2
smap.3: smp.3 sm
	./sm <smp.3 >smap.3
smap.4: smp.4 sm
	./sm <smp.4 >smap.4
smap.5: smp.5 sm
	./sm <smp.5 >smap.5
smap.6: smp.6 sm
	./sm <smp.6 >smap.6
smap.7: smp.7 sm
	./sm <smp.7 >smap.7
sm: sm.c
	cc sm.c -o sm

depend: $(SRCS)
	mkdep ${CFLAGS} $(SRCS)

# AUTOMATICALLY GENERATED MAKE DEPENDENCIES--PUT NOTHING BELOW THIS LINE
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
