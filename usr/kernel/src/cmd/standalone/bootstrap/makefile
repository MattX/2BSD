CFLAGS = -O -I../../../../include

../boot:	boot.o M.o ubmapset.o ../libsa.a ../c.o
	ld -s -o ../boot M.o ubmapset.o boot.o ../c.o ../libsa.a -lc

