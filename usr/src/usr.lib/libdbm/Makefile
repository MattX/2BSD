CFLAGS= -O

.c.o:
	$(CC) -V $(CFLAGS) -c $*.c
	-ld -x -r $*.o
	mv a.out overlay/$*.o
	$(CC) $(CFLAGS) -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

all: libdbm.a libovdbm.a

libdbm.a: dbm.o
	$(CC) -c ${CFLAGS} dbm.o
	mv dbm.o libdbm.a

libovdbm.a: dbm.o
	mv overlay/dbm.o libovdbm.a

install:
	install -o bin -g bin -m 644 libdbm.a ${DESTDIR}/usr/lib
	install -o bin -g bin -m 644 libovdbm.a ${DESTDIR}/usr/lib

clean:
	rm -f *.o ${ALL} overlay/*.o
