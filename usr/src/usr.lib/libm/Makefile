FP =			# use -f if no floating point
CFLAGS = -O ${FP}

OBJS =	asin.o atan.o hypot.o jn.o j0.o j1.o pow.o fabs.o gamma.o \
	log.o sin.o sqrt.o tan.o tanh.o sinh.o exp.o floor.o

.c.o:
	$(CC) -V $(CFLAGS) -c $*.c
	-ld -x -r $*.o
	mv a.out overlay/$*.o
	$(CC) $(CFLAGS) -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

all: libm.a libovm.a

libm.a:	${OBJS}
	ar rc libm.a ${OBJS}
	ranlib libm.a

libovm.a: ${OBJS}
	cd overlay; ar rc ../libovm.a ${OBJS}
	ranlib libovm.a

install:
	install -o bin -g bin -m 644 libm.a ${DESTDIR}/usr/lib
	install -o bin -g bin -m 644 libovm.a ${DESTDIR}/usr/lib

clean:
	-rm -f *.o libm.a libovm.a overlay/*
