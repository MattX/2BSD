#
# C library makefile
#

LIBS=		stdio pdp net gen compat-4.1 compat-4.3 crt
OVLIBS=		stdio pdp net gen compat-4.1 compat-4.3 crt overlay
FLIBS=		stdio pdp net gen compat-4.1 compat-4.3 nonfpcrt
OVFLIBS=	stdio pdp net gen compat-4.1 compat-4.3 nonfpcrt overlay

libc.a:
	rm -f libc.a
	for i in ${LIBS}; do (cd $$i; make libc.a); done
	ar ma findiop.o libc.a exit.o
	ar ma exit.o libc.a flsbuf.o
	ar ma flsbuf.o libc.a fakcu.o
	ranlib libc.a

libovc.a:
	rm -f libovc.a
	for i in ${OVLIBS}; do (cd $$i; make OV=-V libovc.a); done
	ar ma findiop.o libovc.a exit.o
	ar ma exit.o libovc.a flsbuf.o
	ar ma flsbuf.o libovc.a fakcu.o
	ranlib libovc.a

flibc.a:
	rm -f libc.a
	for i in ${FLIBS}; do (cd $$i; make FP=-f libc.a); done
	ar ma findiop.o libc.a exit.o
	ar ma exit.o libc.a flsbuf.o
	ar ma flsbuf.o libc.a fakcu.o
	ranlib libc.a

flibovc.a:
	rm -f libovc.a
	for i in ${OVFLIBS}; do (cd $$i; make FP=-f OV=-V libovc.a); done
	ar ma findiop.o libovc.a exit.o
	ar ma exit.o libovc.a flsbuf.o
	ar ma flsbuf.o libovc.a fakcu.o
	ranlib libovc.a

install:
	install -o bin -g bin -m 644 libc.a /lib
	install -o bin -g bin -m 644 libovc.a /usr/lib
clean:
	for i in ${LIBS} nonfpcrt overlay; do (cd $$i; make clean); done
