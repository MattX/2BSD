#       Makefile        2.0 (2.11BSD) 2025/11/25

LIBDIR=/usr/games/lib

all:	words1

words1:	words1.c
	cc -O -n -o words1 words1.c

install: words1 words.sh
	install -c -s -m 755 words1 ${LIBDIR}
	install -c words.sh /usr/games/words

clean:
	rm -f *.o words1
