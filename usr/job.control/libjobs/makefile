INCLUDE = ../include
CFLAGS=-O -I${INCLUDE}
OBJS=	getwd.o killpg.o setpgrp.o signal.o sigset.o wait2.o

libjobs.a: ${OBJS}
	ar cr libjobs.a `lorder ${OBJS}`

install:
	install -c libjobs.a $(DESTDIR)/usr/lib
	ranlib $(DESTDIR)/usr/lib/libjobs.a

clean:
	rm libjobs.a *.o
.s.o:
	as -o $*.o ${INCLUDE}/sys.s $*.s
