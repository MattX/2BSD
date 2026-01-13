#       @(#)Makefile    2.0 (2.11BSD) 2025/11/26

AS=	/bin/as
LDFLAGS= -i -X

all: as

as: as0.o as2.o
	ld ${LDFLAGS} -o $@ /lib/crt0.o as0.o as2.o -lc

install: all
	install -c -s -m 755 -o root as /bin

clean:
	rm -f *.o a.out as core errs
