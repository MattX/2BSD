#
# v6 makefile
#

all: v6

v6:	v6.c inode.h filsys.h
	${CC} v6.c -o v6

install:
	install -s -o bin -g bin -m 711 v6 /bin

clean:
	rm -f v6 *.o core
