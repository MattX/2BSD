#
#	Standalone Makefile
#
#	Note that there are limitations on how large a program may
#	be loaded along with all device drivers.  This is especially
#	a problem with restor. 
#	Programs should be <= 48K to be safe.

ROOT=		
DISK=		rm0b
TAPE800=	rmt0
TAPE1600=	rmt1
SRCDIR=		/usr/src/cmd
INCLUDES=	-I/usr/src/sys/stand -I/usr/include
BOOT=		bootstrap/M.s bootstrap/boot.c bootstrap/ubmapset.c
LIB=		/lib/libnonfpc.a
STRIP=		strip
CFLAGS=		-O -DSTANDALONE ${INCLUDES}
ALL=		boot cat icheck maketape mkfs mtboot restor tsboot

all:		${ALL} size

libsa.a:
		cd libsa ; make INCLUDES="${INCLUDES}" ../libsa.a

restor:		libsa.a c.o srt0.o ${SRCDIR}/restor.c
		cc -c ${CFLAGS} ${SRCDIR}/restor.c
		ld -o restor srt0.o restor.o c.o libsa.a ${LIB}

cat:		libsa.a srt0.o c.o
		cc -c ${CFLAGS} util/cat.c
		ld -o cat srt0.o cat.o c.o libsa.a ${LIB}

mkfs:		libsa.a srt0.o ${SRCDIR}/mkfs.c c.o
		cc -c ${CFLAGS} ${SRCDIR}/mkfs.c 
		ld -o mkfs srt0.o mkfs.o c.o libsa.a ${LIB}

icheck:		libsa.a srt0.o ${SRCDIR}/icheck.c c.o
		cc -c ${CFLAGS} ${SRCDIR}/icheck.c 
		ld -o icheck srt0.o icheck.o c.o libsa.a ${LIB}

mtboot:		mtboot.s
		as -o mtboot mtboot.s
		strip mtboot

tsboot:		tsboot.s
		as -o tsboot tsboot.s
		strip tsboot

maketape:	util/maketape.c
		cc -o maketape util/maketape.c

boot:   	${BOOT} libsa.a c.o
		chdir bootstrap ; make INCLUDES="${INCLUDES}" ../boot

distr800:	distr distr2

distr1600:	boot cat icheck maketape mkfs mtboot restor
		./maketape /dev/n${TAPE1600} tapes/tapedir.1600
		dump 0f /dev/n${TAPE1600} /dev/r${DISK}
		chdir ${ROOT}/usr ; tar cfb /dev/n${TAPE1600} 20 \
		70 adm bin dict doc games include lib local man \
		msgs preserve public spool sys tmp ucb net contrib \
		src ingres

distr:		boot cat icheck maketape mkfs mtboot restor
		./maketape /dev/n${TAPE800} tapes/tapedir
		dump 0f /dev/n${TAPE800} /dev/r${DISK}
		chdir ${ROOT}/usr ; tar cfb /dev/n${TAPE800} 20 \
		70 adm bin contrib dict doc games include lib local man \
		msgs preserve public spool sys tmp ucb

distr2:
		chdir ${ROOT}/usr ; tar cfb /dev/n${TAPE800} 20 net src ingres

tsdistr:	boot cat icheck maketape mkfs restor tsboot
		./maketape /dev/n${TAPE1600} tapes/tstapedir
		dump 0f /dev/n${TAPE1600} /dev/r${DISK}
		chdir ${ROOT}/usr ; tar cfb /dev/n${TAPE1600} 20 \
		70 adm bin dict doc games include lib local man \
		msgs preserve public spool sys tmp ucb net contrib \
		src ingres

size:
		size ${ALL}
	
clean:
		rm -f ${ALL} *.o *.a libsa/*.o libsa/*.a bootstrap/*.o bootstrap/*.a
