#       Makefile        2.0 (2.11BSD) 2025/11/25
# Driver makefile for zmodem.  Calls makefile.generic.

CC=cc
CFLAGS=-O -DV7 -DMD=2 -DTXBSIZE=8192 -DNFGVMIN
LDFLAGS=-i
ROFF  = /usr/man/manroff
DESTBIN = /usr/local
DESTMAN = /usr/local/man/cat1

INSTALL = /usr/bin/install

OBJ = sz rz

all: $(OBJ) rz.0 sz.0

sz:	sz.o
	$(CC) ${LDFLAGS} sz.o -o sz
	size sz

rz:	rz.o
	$(CC) ${LDFLAGS} rz.o -o rz
	size rz

rz.0:	rz.1
	/usr/man/manroff rz.1 > rz.0

sz.0:	sz.1
	/usr/man/manroff sz.1 > sz.0

install: rz sz rz.0 sz.0
	install -c -s -m 755 rz ${DESTBIN}
	install -c -s -m 755 sz ${DESTBIN}
	install -c -m 444 rz.0 ${DESTMAN}
	install -c -m 444 -o root -g staff sz.0 ${DESTMAN}
	rm -f $(DESTBIN)/sb $(DESTBIN)/sx
	ln $(DESTBIN)/sz $(DESTBIN)/sb
	ln $(DESTBIN)/sz $(DESTBIN)/sx
	rm -f $(DESTBIN)/rb $(DESTBIN)/rx
	ln $(DESTBIN)/rz $(DESTBIN)/rb
	ln $(DESTBIN)/rz $(DESTBIN)/rx

	rm -f $(DESTMAN)/sb.0 $(DESTMAN)/sx.0
	ln $(DESTMAN)/sz.0 $(DESTMAN)/sb.0
	ln $(DESTMAN)/sz.0 $(DESTMAN)/sx.0
	rm -f $(DESTMAN)/rb.0 $(DESTMAN)/rx.0
	ln $(DESTMAN)/rz.0 $(DESTMAN)/rb.0
	ln $(DESTMAN)/rz.0 $(DESTMAN)/rx.0

clean:
	rm -f $(OBJ) sb sx rb rx rz sz *.o *.0
