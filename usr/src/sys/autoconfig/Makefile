#
# Makefile for the system autoconfiguration program
#

OBJS=	attach.o do_config.o main.o read_dtab.o read_nlist.o uprobe.o

CFLAGS=	-O -I.
ALL=	autoconfig

all: ${ALL}

autoconfig:	${OBJS}
	${CC} ${OBJS} -o $@

install: ${ALL}
	install -s -o bin -g bin -m 700 autoconfig ${DESTDIR}/etc

clean:
	rm -f ${ALL} ${OBJS}

.c.o:
	${CC} ${CFLAGS} -c $*.c

attach.o:	dtab.h ivec.h attach.c
do_config.o:	ivec.h dtab.h do_config.c
main.o:		main.c
read_dtab.o:	uprobe.h dtab.h read_dtab.c
read_nlist.o:	dtab.h read_nlist.c
uprobe.o:	uprobe.h uprobe.c
