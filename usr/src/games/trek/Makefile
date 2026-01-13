#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.2 (2.11BSD) 2025/11/29
#
CFLAGS=	-O
LDFLAGS= -i
SRCS=	abandon.c attack.c autover.c capture.c check_out.c checkcond.c \
	compkl.c computer.c damage.c damaged.c dcrept.c destruct.c \
	dock.c dumpgame.c dumpme.c dumpssradio.c events.c externs.c \
	getcodi.c getpar.c help.c impulse.c initquad.c kill.c klmove.c \
	lose.c lrscan.c main.c move.c nova.c out.c phaser.c play.c ram.c \
	ranf.c rest.c schedule.c score.c setup.c setwarp.c shell.c \
	shield.c snova.c srscan.c systemname.c torped.c utility.c \
	visual.c warp.c win.c cgetc.c
OBJS=	abandon.o attack.o autover.o capture.o check_out.o checkcond.o \
	compkl.o computer.o damage.o damaged.o dcrept.o destruct.o \
	dock.o dumpgame.o dumpme.o dumpssradio.o events.o externs.o \
	getcodi.o getpar.o help.o impulse.o initquad.o kill.o klmove.o \
	lose.o lrscan.o main.o move.o nova.o out.o phaser.o play.o ram.o \
	ranf.o rest.o schedule.o score.o setup.o setwarp.o shell.o \
	shield.o snova.o srscan.o systemname.o torped.o utility.o \
	visual.o warp.o win.o cgetc.o
HDRS=	getpar.h trek.h

trek: 	${OBJS} ${HDRS}
	cc ${LDFLAGS} -o trek ${OBJS} -lm

${OBJS}:
	${CC} ${CFLAGS} -c $*.c

tags:	tags $(SRCS)
	ctags $(SRCS)

install: trek
	install -c -s -m 755 -o root trek /usr/games/trek

clean:
	rm -f a.out core ${OBJS} trek

depend:	${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

abandon.o: abandon.c trek.h
attack.o: attack.c trek.h
autover.o: autover.c trek.h
capture.o: capture.c trek.h
check_out.o: check_out.c trek.h
checkcond.o: checkcond.c trek.h
compkl.o: compkl.c trek.h
computer.o: computer.c trek.h getpar.h /usr/include/stdio.h
damage.o: damage.c trek.h
damaged.o: damaged.c trek.h
dcrept.o: dcrept.c trek.h
destruct.o: destruct.c trek.h
dock.o: dock.c trek.h
dumpgame.o: dumpgame.c trek.h
dumpme.o: dumpme.c trek.h
dumpssradio.o: dumpssradio.c trek.h
events.o: events.c trek.h
externs.o: externs.c trek.h
getcodi.o: getcodi.c getpar.h
getpar.o: getpar.c /usr/include/stdio.h getpar.h
help.o: help.c trek.h
impulse.o: impulse.c trek.h
initquad.o: initquad.c trek.h
kill.o: kill.c trek.h
klmove.o: klmove.c trek.h
lose.o: lose.c trek.h /usr/include/setjmp.h
lrscan.o: lrscan.c trek.h
main.o: main.c trek.h /usr/include/stdio.h /usr/include/sgtty.h
main.o: /usr/include/sys/ioctl.h /usr/include/sys/ttychars.h
main.o: /usr/include/sys/ttydev.h /usr/include/setjmp.h
move.o: move.c trek.h
nova.o: nova.c trek.h
out.o: out.c trek.h
phaser.o: phaser.c trek.h getpar.h
play.o: play.c trek.h getpar.h /usr/include/setjmp.h
ram.o: ram.c trek.h
ranf.o: ranf.c /usr/include/stdio.h
rest.o: rest.c trek.h getpar.h
schedule.o: schedule.c trek.h
score.o: score.c trek.h getpar.h
setup.o: setup.c trek.h getpar.h
setwarp.o: setwarp.c trek.h getpar.h
shell.o: shell.c
shield.o: shield.c trek.h getpar.h
snova.o: snova.c trek.h
srscan.o: srscan.c trek.h getpar.h
systemname.o: systemname.c trek.h
torped.o: torped.c /usr/include/stdio.h trek.h
utility.o: utility.c
visual.o: visual.c trek.h
warp.o: warp.c trek.h
win.o: win.c trek.h getpar.h /usr/include/setjmp.h
cgetc.o: cgetc.c /usr/include/stdio.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
