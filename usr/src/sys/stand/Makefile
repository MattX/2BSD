#
#	Standalone Makefile
#
#	Note that there are limitations on how large a program may
#	be loaded along with all device drivers.  This is especially
#	a problem with restor.  Programs should be <= 48K to be safe.

ROOT=		/
DISK=		rm0b
TAPE800=	rmt0
TAPE1600=	rmt1
BINDIR=		${ROOT}/usr/src/bin
ETCDIR=		${ROOT}/usr/src/etc
INCL=		-I${ROOT}/usr/include
BOOT=		M.o boot.o ubmapset.o
AS=		${ROOT}/bin/as
CC=		${ROOT}/bin/cc
LD=		${ROOT}/bin/ld
RM=		${ROOT}/bin/rm
LIB=		${ROOT}/lib/libc.a
STRIP=		${ROOT}/bin/strip
CFLAGS=		-O -DSTANDALONE ${INCL}
DRIV=		prf.o xp.o ht.o rk.o rm.o rl.o rp.o hk.o ts.o tm.o sys.o
ALL=		boot cat icheck maketape mkfs mtboot restor tsboot

all: ${ALL}

libsa.a: ${DRIV}
	ar rv libsa.a $?
	ranlib libsa.a

restor: libsa.a c.o srt0.o ${BINDIR}/restor.c
	${CC} -c ${CFLAGS} ${BINDIR}/restor.c
	${LD} -o restor srt0.o restor.o c.o libsa.a ${LIB}
	${RM} restor.o

cat: libsa.a srt0.o c.o
	${CC} -c ${CFLAGS} cat.c
	${LD} -o cat srt0.o cat.o c.o libsa.a ${LIB}
	${RM} cat.o

mkfs: libsa.a srt0.o ${ETCDIR}/mkfs.c c.o
	${CC} -c ${CFLAGS} ${ETCDIR}/mkfs.c
	${LD} -o mkfs srt0.o mkfs.o c.o libsa.a ${LIB}
	${RM} mkfs.o

icheck: libsa.a srt0.o ${BINDIR}/icheck.c c.o
	${CC} -c ${CFLAGS} ${BINDIR}/icheck.c
	${LD} -o icheck srt0.o icheck.o c.o libsa.a ${LIB}
	${RM} icheck.o

mtboot: mtboot.s
	${AS} -o mtboot mtboot.s
	${STRIP} mtboot

tsboot: tsboot.s
	${AS} -o tsboot tsboot.s
	${STRIP} tsboot

maketape: maketape.c
	${CC} -o $@ maketape.c

boot: ${BOOT} libsa.a c.o
	ld -o $@ ${BOOT} c.o libsa.a -lc

distr800: distr distr2

distr1600: boot cat icheck maketape mkfs mtboot restor
	./maketape /dev/n${TAPE1600} tapes/tapedir.1600
	dump 0f /dev/n${TAPE1600} /dev/r${DISK}
	chdir ${ROOT}/usr ; tar cfb /dev/n${TAPE1600} 20 \
	70 adm bin dict doc games include lib local man \
	msgs preserve public spool sys tmp ucb net contrib \
	src ingres

distr: boot cat icheck maketape mkfs mtboot restor
	./maketape /dev/n${TAPE800} tapes/tapedir
	dump 0f /dev/n${TAPE800} /dev/r${DISK}
	chdir ${ROOT}/usr ; tar cfb /dev/n${TAPE800} 20 \
	70 adm bin contrib dict doc games include lib local man \
	msgs preserve public spool sys tmp ucb

distr2:
	chdir ${ROOT}/usr ; tar cfb /dev/n${TAPE800} 20 net src ingres

tsdistr: boot cat icheck maketape mkfs restor tsboot
	./maketape /dev/n${TAPE1600} tapes/tstapedir
	dump 0f /dev/n${TAPE1600} /dev/r${DISK}
	chdir ${ROOT}/usr ; tar cfb /dev/n${TAPE1600} 20 \
	70 adm bin dict doc games include lib local man \
	msgs preserve public spool sys tmp ucb net contrib \
	src ingres

clean:
	${RM} -f ${ALL} *.o

.c.o:
	${CC} ${CFLAGS} -c $*.c

M.o: M.s
	${AS} - -o M.o M.s

c.o:		c.c
mtboot.o:	mtboot.s
srt0.o:		srt0.s
tsboot.o:	tsboot.s
sys.o:		sys.c
prf.o:		prf.c
xp.o:		xp.c
ht.o:		ht.c
rk.o:		rk.c
rm.o:		rm.c
rl.o:		rl.c
rp.o:		rp.c
hk.o:		hk.c
ts.o:		ts.c
tm.o:		tm.c
boot.o:		boot.c
ubmapset.o:	ubmapset.c
