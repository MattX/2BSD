#
# plot makefile
#

CFLAGS=	-O
ALL=	tek t300 t300s t450 vplot

all: ${ALL}

install: ${ALL}
	install -s -o bin -g bin -m 711 tek ${DESTDIR}/bin
	install -s -o bin -g bin -m 711 t300 ${DESTDIR}/bin
	install -s -o bin -g bin -m 711 t300s ${DESTDIR}/bin
	install -s -o bin -g bin -m 711 t450 ${DESTDIR}/bin
	install -s -o bin -g bin -m 711 vplot ${DESTDIR}/bin

tek:	driver.o
	${CC} ${LFLAG} -o $@ driver.o -lt4014 -lm

t300:	driver.o 
	${CC} ${LFLAG} -o $@ driver.o -lt300 -lm
t300s:	driver.o 
	${CC} ${LFLAG} -o $@ driver.o -lt300s -lm
t450:	driver.o 
	${CC} ${LFLAG} -o $@ driver.o -lt450 -lm

vplot:	chrtab.o vplot.o
	${CC} ${LFLAG} -o $@ vplot.o chrtab.o

clean:
	rm -f ${ALL} ${OBJ} core

.c.o:
	${CC} ${CFLAGS} -c $*.c

chrtab.o:	chrtab.c
driver.o:	driver.c
vplot.o:	vplot.c
