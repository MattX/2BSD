CFLAGS=-O
LIB=/lib

all:	getdate.o
	:

getdate.o: dateconv.h datedcl.h datelex.h getdate.h getdate.c
	$(CC) $(CFLAGS) -c getdate.c

clean:
	rm -f getdate.o getdate.c testnull.o testnull testft.o testft

install: getdate.o
	ar rv $(LIB)/libc.a getdate.o
	ranlib $(LIB)/libc.a

testnull: testnull.o getdate.o
	cc -n testnull.o getdate.o -o testnull
	testnull

testft: testft.o getdate.o
	cc -n testft.o getdate.o -o testft
	testft

testnull.o: test.c
	$(CC) $(CFLAGS) -c -DTNULL test.c
	mv test.o testnull.o

testft.o: test.c
	$(CC) $(CFLAGS) -c test.c
	mv test.o testft.o

lint: getdate.c
	lint getdate.c
