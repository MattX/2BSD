# Driver makefile for zmodem.  Calls makefile.generic.

PREFIX = 
CC=cc
OFLAG=-O
MANSUFF = 1
CATSUFF = 0
ROFF  = /usr/man/manroff
DESTBIN = $(PREFIX)/usr/local
DESTMAN = $(PREFIX)/usr/local/man/cat$(MANSUFF)

INSTALL = /usr/bin/install

OBJ = sz rz

all: $(OBJ)

sz:
	$(CC) $(CFLAGS) $(OFLAG) -DV7 -DTXBSIZE=8192 -DNFGVMIN sz.c -o sz
	size sz

rz:
	$(CC) $(CFLAGS) $(OFLAG) -DMD=2 -Dstrchr=index -DV7 rz.c -o rz
	size rz

install: $(OBJ)
	for f in $(OBJ); do\
	    $(INSTALL) -c -m 755 $$f ${DESTDIR}$(DESTBIN); \
	    rm -f ${DESTDIR}$(DESTMAN)/$$f.$(CATSUFF); \
	    $(ROFF) ./$$f.1 >  ${DESTDIR}$(DESTMAN)/$$f.$(CATSUFF); \
	    chmod 444 ${DESTDIR}$(DESTMAN)/$$f.$(CATSUFF); \
	    chown bin.bin ${DESTDIR}$(DESTMAN)/$$f.$(CATSUFF); \

	done
	rm -f ${DESTDIR}$(DESTBIN)/sb ${DESTDIR}$(DESTBIN)/sx
	ln ${DESTDIR}$(DESTBIN)/sz ${DESTDIR}$(DESTBIN)/sb
	ln ${DESTDIR}$(DESTBIN)/sz ${DESTDIR}$(DESTBIN)/sx
	rm -f ${DESTDIR}$(DESTBIN)/rb ${DESTDIR}$(DESTBIN)/rx
	ln ${DESTDIR}$(DESTBIN)/rz ${DESTDIR}$(DESTBIN)/rb
	ln ${DESTDIR}$(DESTBIN)/rz ${DESTDIR}$(DESTBIN)/rx

	rm -f ${DESTDIR}$(DESTMAN)/sb.$(CATSUFF) ${DESTDIR}$(DESTMAN)/sx.$(CATSUFF)
	ln ${DESTDIR}$(DESTMAN)/sz.$(CATSUFF) ${DESTDIR}$(DESTMAN)/sb.$(CATSUFF)
	ln ${DESTDIR}$(DESTMAN)/sz.$(CATSUFF) ${DESTDIR}$(DESTMAN)/sx.$(CATSUFF)
	rm -f ${DESTDIR}$(DESTMAN)/rb.$(CATSUFF) ${DESTDIR}$(DESTMAN)/rx.$(CATSUFF)
	ln ${DESTDIR}$(DESTMAN)/rz.$(CATSUFF) ${DESTDIR}$(DESTMAN)/rb.$(CATSUFF)
	ln ${DESTDIR}$(DESTMAN)/rz.$(CATSUFF) ${DESTDIR}$(DESTMAN)/rx.$(CATSUFF)

clean:
	rm -f $(OBJ)
	rm -f sb sx rb rx rz sz
	rm -f *.o
