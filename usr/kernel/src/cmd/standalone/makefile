#
#	Standalone Makefile
#
#	Note that there are limitations on how large a program may
#	be loaded along with all device drivers.  This is especially
#	a problem with restor.
#

DISTR = /ya/distr/usr

LIBSA = libsa/*.c

BOOT = bootstrap/*.[sc]

ADB = ../adb/*.c

CFLAGS = -O -DSTANDALONE -I../../../include

ALL=	restor cat icheck mkfs mtboot tsboot maketape boot 

all:	$(ALL) size

libsa.a:${LIBSA}
	(cd libsa; make ../libsa.a)

restor:	libsa.a srt0.o restor.c c.o
	cc -c ${CFLAGS} restor.c
	ld  -o restor srt0.o restor.o c.o libsa.a -lc
	rm restor.o

cat:	libsa.a srt0.o c.o
	cc -c ${CFLAGS} util/cat.c
	ld -s -o cat srt0.o cat.o c.o libsa.a -lc
	rm cat.o

mkfs:	libsa.a srt0.o ../mkfs.c c.o
	cc+ld-stand mkfs

icheck:	libsa.a srt0.o ../icheck.c c.o
	cc+ld-stand icheck

mtboot:	mtboot.s
	as -o mtboot mtboot.s
	strip mtboot

tsboot:	tsboot.s
	as -o tsboot tsboot.s
	strip tsboot

maketape:	util/maketape.c
	cc -o maketape util/maketape.c

boot:   ${BOOT} libsa.a c.o
	(chdir bootstrap; make ../boot)

distr:	icheck restor cat mkfs boot mtboot maketape 
	./maketape /dev/nrmt0 tapes/tapedir
	dump 0f /dev/nrmt0 /dev/rhp0e
	(chdir ${DISTR} ; tar cfb /dev/rmt0 20 . )

tsdistr:icheck restor cat mkfs boot tsboot maketape 
	./maketape /dev/nrmt1 tapes/tstapedir
	dump 0f /dev/nrmt1 /dev/rhp0e
	(chdir ${DISTR}; tar cfb /dev/rmt1 20 . )

print:
	pr saio.h tsboot.s mtboot.s srt0.s bootstrap/M.s bootstrap/boot.c \
	bootstrap/ubmapset.c libsa/*.c ../mkfs.c ../restor.c  \
	util/cat.c c.c|lpr

size:
	size ${ALL}
	
clean:
	rm -f $(ALL)
	rm -f *.o
