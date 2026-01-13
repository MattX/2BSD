#	Makefile	4.8	(2.11BSD) 2025/11/27

CFLAGS=	-O
LDFLAGS= -i

ALL=	tek t4013 t300 t300s t450 aedplot bgplot crtplot dumbplot gigiplot \
	hpplot hp7221plot implot atoplot plottoa

all:	${ALL} debug

tek:	driver.o
	cc ${LDFLAGS} -o tek  driver.o -l4014 -lm

t4013:	driver.o
	cc ${LDFLAGS} -o t4013  driver.o -l4013 -lm

t300:	driver.o 
	cc ${LDFLAGS} -o t300 driver.o -l300 -lm

t300s:	driver.o 
	cc ${LDFLAGS} -o t300s driver.o -l300s -lm

t450:	driver.o 
	cc ${LDFLAGS} -o t450 driver.o -l450 -lm

aedplot: driver.o
	cc ${LDFLAGS} -o aedplot driver.o -lplotaed

bgplot: driver.o
	cc ${LDFLAGS} -o bgplot driver.o -lplotbg -lm

crtplot: crtdriver.o crtplot.o
	cc ${LDFLAGS} -o crtplot crtdriver.o crtplot.o -lcurses -ltermcap -lm

dumbplot: driver.o
	cc ${LDFLAGS} -o dumbplot driver.o -lplotdumb -ltermcap -lm

gigiplot: driver.o
	cc ${LDFLAGS} -o gigiplot driver.o -lplotgigi -lm

hpplot: driver.o
	cc ${LDFLAGS} -o hpplot driver.o -lplot2648 -lm

hp7221plot: driver.o
	cc ${LDFLAGS} -o hp7221plot driver.o -lplot7221 -lm

implot: driver.o
	cc ${LDFLAGS} -o implot driver.o -lplotimagen -lm

atoplot: atoplot.o
	cc ${LDFLAGS} -o atoplot atoplot.o -lplot -lm

plottoa: plottoa.o
	cc ${LDFLAGS} -o plottoa plottoa.o

debug: debug.o
	cc ${LDFLAGS} -o debug debug.o

install: all
	-for i in ${ALL}; do \
		(install -c -s -m 755 $$i /usr/bin/$$i); done
	install -c -m 755 plot.sh /usr/bin/plot

clean:
	rm -f *.o ${ALL} a.out core errs debug
