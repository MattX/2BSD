#
# remind makefile
#
# FP set to -f on machines without floating point
#

ROBJ=	main.o globals.o enqueue.o deliver.o send.o time.o misc.o
POBJ=	prfile.o globals.o
DOBJ=	delrem.o
ALL=	remind prfile delrem

all:	${ALL}

remind: ${ROBJ}
	${CC} ${LFLAG} ${ROBJ} -o $@

prfile:	${POBJ}
	${CC} ${LFLAG} ${POBJ} -o $@

delrem: ${DOBJ}
	${CC} ${LFLAG} ${DOBJ} -o $@

install: ${all}
	install -s -o root -g su -m 4755 prfile /usr/bin
	install -s -o root -g su -m 4755 remind /usr/bin
	install -s -o root -g su -m 4755 delrem /usr/bin

clean:
	rm -f core a.out ${ALL} ${ROBJ} ${DOBJ} ${POBJ}

.c.o:
	${CC} ${CFLAGS} -c $*.c

deliver.o:	deliver.c remind.h
delrem.o:	delrem.c remind.h
enqueue.o:	enqueue.c remind.h
globals.o:	globals.c remind.h
main.o:		main.c remind.h
misc.o:		misc.c remind.h
prfile.o:	prfile.c remind.h
send.o:		send.c remind.h
time.o:		time.c remind.h
