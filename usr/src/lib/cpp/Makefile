#	Makefile	2.1	(2.11BSD) 2025/8/10
#

# if machine has separate I/D
SEPFLAG=-i
VMPGS = 16
# else
# LDFLAGS =
# VMPGS = 4
# endif

CFLAGS=-O -DLIBVMF -I. -DNVMPGS=${VMPGS}

all : cpp cpp.0

cpp :	cpp.o cpc.o token.o
	$(CC) $(CFLAGS) $(SEPFLAG) -o cpp cpp.o cpc.o token.o -lvmf

cpp.0 : cpp.1
	/usr/man/manroff cpp.1 > cpp.0

clean :
	rm -f *.o cpp cpp.0

install : cpp cpp.0
	install -s -m 755 -o root -g wheel cpp /lib
	install -m 444 -o bin -g bin cpp.0 /usr/man/cat1
