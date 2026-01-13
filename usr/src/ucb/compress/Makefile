#
#	@(#)Makefile	5.6 (2.11BSD) 2026/1/1
#
CFLAGS=-O
LDFLAGS= -i
BIN=/usr/ucb

compress: compress.o
	cc ${LDFLAGS} -o compress compress.o

install: compress
	install -c -s -m 755 compress $(BIN)
	rm -f $(BIN)/uncompress $(BIN)/zcat
	ln $(BIN)/compress $(BIN)/uncompress
	ln $(BIN)/compress $(BIN)/zcat

clean:
	rm -f compress *.core errs *.o
