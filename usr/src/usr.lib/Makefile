#
# usr.lib makefile
#

CFLAGS=	-O

atrun: atrun.o
	${CC} ${LFLAG} $@.o -o $@

calendar: calendar.o
	${CC} ${LFLAG} $@.o -o $@

makekey: makekey.o
	${CC} ${LFLAG} $@.o -o $@

atrun.o:	atrun.c
calendar.o:	calendar.c
makekey.o:	makekey.c
