#	@(#)Makefile	4.1	12/25/82
#
CFLAGS=-O
OBJS= pow.o gcd.o msqrt.o mdiv.o mout.o mult.o madd.o util.o

.c.o:
	$(CC) -V $(CFLAGS) -c $*.c
	-ld -x -r $*.o
	mv a.out overlay/$*.o
	$(CC) $(CFLAGS) -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

libmp.a libovmp.a: $(OBJS)
	ar cr libmp.a $(OBJS)
	ranlib libmp.a
	cd overlay; ar cr ../libovmp.a $(OBJS)
	ranlib libovmp.a

install:
	install -o bin -g bin -m 644 libmp.a ${DESTDIR}/usr/lib/libmp.a
	install -o bin -g bin -m 644 libovmp.a ${DESTDIR}/usr/lib/libmp.a

clean:
	rm -f *.o libmp.a libovmp.a overlay/*
