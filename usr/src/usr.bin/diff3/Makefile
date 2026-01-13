#	Makefile	4.1.2 (2.11BSD)	2025/11/25

CFLAGS=-O
LDFLAGS= -i

diff3: diff3.o
	cc ${LDFLAGS} -o diff3 diff3.o

install: diff3
	install -c -s -m 755 -o root diff3 /usr/libexec
	install -c -m 755 -o root diff3.sh /usr/bin/diff3

clean:
	rm -f diff3.o diff3	
