#
#	Standalone Makefile
#
#	Note that there are limitations on how large a program may
#	be loaded along with all device drivers.  This is especially
#	a problem with restor.  Programs should be <= 48K to be safe.
#
#	If a GENERIC kernel distribution is being created be sure
#	to install /sys/pdpdist/dtab (or /etc/dtab.save) as ${ROOT}/etc/dtab 
#	so that the GENERIC kernel can find the tape device.

# DISK	which disk to take a root dump of for the distribution tape
# TAPE	which tape to write the distribution on
# DUMP	where to store a root system dump if you do a ``make dump''
#
DISK=	xp0a
TAPE=	mt8
DUMP=	/usr/root.dump

ROOT=	
ETCSRC=	${ROOT}/usr/src/etc
MKFS=	${ETCSRC}/mkfs.c
RESTOR=	${ETCSRC}/restor/restor.c
ICHECK=	${ETCSRC}/icheck.c

DEFS=	-DSTANDALONE -I${ROOT}/usr/include
CFLAGS=	-O ${DEFS}

BOOT=	M.o boot.o ubmapset.o
DRIVERS=prf.o sys.o \
	ht.o tm.o ts.o tmscp.o \
	xp.o rk.o rl.o br.o hk.o si.o ra.o

ALL=	mtboot boot mkfs restor icheck maketape toyset

.c.o:
	cc ${CFLAGS} -c $*.c

.s.o:
	/lib/cpp -P ${DEFS} $< | as -u -V -o $@

all: ${ALL}

distribution: tape1 switch_tapes tape2

tmscptape: ${ALL} ${DUMP}
	./maketape /dev/nr${TAPE} maketape.data
	dd if=${DUMP} of=/dev/nr${TAPE} bs=20b
	cd ${ROOT}/usr; tar cfb /dev/nr${TAPE} 20 \
		adm bin dict doc games guest hosts include ingres lib \
		local man msgs new old preserve pub skel spool tmp ucb
	cd ${ROOT}/usr/src; tar cfb /dev/nr${TAPE} 20 \
		sys include
	cd ${ROOT}/usr/src; tar cfb /dev/r${TAPE} 20 \
		Makefile asm.sed asm.sed.pdp asm.sed.vax bin etc games \
		lib local man new old ucb usr.bin usr.lib

tape1: ${ALL} ${DUMP}
	./maketape /dev/nr${TAPE} maketape.data
	dd if=${DUMP} of=/dev/nr${TAPE} bs=20b
	cd ${ROOT}/usr; tar cfb /dev/nr${TAPE} 20 \
		adm bin dict doc games guest hosts include ingres lib \
		local man msgs new old preserve pub skel spool tmp ucb
	cd ${ROOT}/usr/src; tar cfb /dev/r${TAPE} 20 \
		sys include

switch_tapes: FRC
	@echo "Switch tapes.  Hit CR when second tape is mounted."
	@head -1

FRC:

tape2:
	cd ${ROOT}/usr/src; tar cfb /dev/r${TAPE} 20 \
		Makefile asm.sed asm.sed.pdp asm.sed.vax bin etc games \
		lib local man new old ucb usr.bin usr.lib

${DUMP}:
	@echo "You must make a dump of the root file system first."
	exit 1

dump:
	dump 0f ${DUMP} /dev/r${DISK}

install: boot
	install -s boot ${ROOT}/


libsa.a: ${DRIVERS}
	ar rv $@ $?
	ranlib $@


mkfs.o: ${MKFS}
	cc ${CFLAGS} -c ${MKFS}

restor.o: ${RESTOR}
	cc ${CFLAGS} -c ${RESTOR}

icheck.o: ${ICHECK}
	cc ${CFLAGS} -c ${ICHECK}

mtboot: mtboot.o
	strip $@.o
	dd if=$@.o of=mtboot bs=16 skip=1
	rm -f $@.o

boot: M.o conf.o boot.o ubmapset.o libsa.a
	ld -X -o $@ M.o conf.o boot.o ubmapset.o libsa.a -lc

mkfs: srt0.o conf.o libsa.a mkfs.o
	ld -o $@ srt0.o conf.o $@.o libsa.a -lc

restor: srt0.o conf.o libsa.a restor.o
	ld -o $@ srt0.o conf.o $@.o libsa.a -lc

icheck: srt0.o conf.o libsa.a icheck.o
	ld -o $@ srt0.o conf.o $@.o libsa.a -lc

maketape: maketape.c
	cc -o $@ maketape.c

toyset: toyset.o srt0.o conf.o libsa.a
	ld -o $@ srt0.o conf.o $@.o libsa.a -lc

tags: FRC
	rm -f tags
	ctags *.[ch]
	for i in M.s; do \
		${EGREP} '^[_a-zA-Z][_a-zA-Z0-9]*:' $$i \
		| ${SED} "s;\([^:]*\):\(.*\)$$;\1	$$i	/^\1:\2$$/;" \
		>> tags; \
	done
	sort -o tags tags

FRC:

clean:
	rm -f ${ALL} ${AUX} libsa.a *.o tags

boot.o:		boot.c
conf.o:		conf.c
cat.o:		cat.c
hk.o:		hk.c
ht.o:		ht.c
mtboot.o:	mtboot.s
prf.o:		prf.c
rk.o:		rk.c
rl.o:		rl.c
br.o:		br.c
srt0.o:		srt0.s
sys.o:		sys.c
tm.o:		tm.c
tmscp.o:	tmscp.c
ts.o:		ts.c
ubmapset.o:	ubmapset.c
xp.o:		xp.c
