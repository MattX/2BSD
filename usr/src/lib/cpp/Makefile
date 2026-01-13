#	Makefile	2.3	(2.11BSD) 2025/11/25
#

# if machine has separate I/D
LDFLAGS=-i
VMPGS = 12
# else
# LDFLAGS =
# VMPGS = 4
# endif

CFLAGS=-O -DLIBVMF -I. -DNVMPGS=${VMPGS}

all : cpp cpp.0

cpp :	cpp.o cpc.o token.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o cpp cpp.o cpc.o token.o -lvmf -lstubs

cpp.0 : cpp.1
	/usr/man/manroff cpp.1 > cpp.0

clean :
	rm -f *.o cpp cpp.0

install : cpp cpp.0
	install -s -m 755 -o root -g wheel cpp /lib
	install -m 444 -o root -g staff cpp.0 /usr/man/cat1
