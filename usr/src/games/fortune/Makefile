# Define OLD for all systems not having the "random()" call.

CFLAGS= -O -I. -DOLD -Dpdp11
LDFLAGS=
PROGS=	fortune strfile unstr

all: $(PROGS)

fortune: strfile.h fortune.o
	$(CC) $(LDFLAGS) -o $@ fortune.o

strfile: strfile.h strfile.o
	$(CC) $(LDFLAGS) -o $@ strfile.o

unstr: strfile.h unstr.o
	$(CC) $(LDFLAGS) -o $@ unstr.o

install: all
	install -s -m 711 -o bin -g bin fortune /usr/games
	install -m 644 -o bin -g bin fortunes.dat /usr/games/lib

clean:
	rm -f $(PROGS) *.o core a.out
tar:
	tar cvf fort.src.tar Makefile README do_sort fortune.6 fortune.c strfile.c strfile.h unstr.c
