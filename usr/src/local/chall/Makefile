CFLAGS= -O
chall: main.o walk.o assert.o
	cc ${CFLAGS} -o chall main.o walk.o assert.o

install: chall
	install -s -o bin -g bin -m 711 chall /usr/local

clean:
	rm -f *.o  chall core m.err
