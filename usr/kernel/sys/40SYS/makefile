# definition of PDP11 should normally be in whoami.h

DEFS=-DKERNEL -DPDP11=40

INCLUDES= ../../include

CFLAGS=	-O ${DEFS} -I${INCLUDES}

DISTRROOT=/ya/distr/root

CC = cc -V 
AS = ovas
LD = ./covld

CFILES=	c.c /dev/null

OFILE=	ovunix

SYS=	../sys/*.c

DEV=	../dev/*.c

HEAD=	${INCLUDES}/sys/*.h ${INCLUDES}/whoami.h

OV1 =\
	pipe.o\
	sys1.o\
	sys2.o\
	sys4.o\
	prim.o\
	fakemx.o

OV2 =\
	sys3.o\
	ioctl.o\
	kl.o\
	partab.o\
	lp.o\
	ttynew.o

OV3 =\
	machdep.o\
	sig.o\
	bio.o\
	ht.o\
	dsort.o 

OV4 =\
	fio.o\
	alloc.o \
	sys.o\
	mem.o\
	tty.o\
	ttyold.o

OV5 =\
	main.o\
	acct.o\
	malloc.o\
	text.o\
	syslocal.o\
	clock.o \
	ureg.o\
	dkleave.o

OV6 =\
	xp.o\
	rl.o\
	rk.o\
	hk.o\
	dz.o
OV7 =\
	rp.o\
	tm.o\
	ts.o\
	dvhp.o\
	nami.o

BASE =\
	prf.o\
	rdwri.o\
	slp.o\
	subr.o\
	sysent.o\
	trap.o\
	iget.o

CONFOBJ =\
	l40.o\
	m40.o\
	c40.o

OBJ = ${OV1} ${OV2} ${OV3} ${OV4} ${OV5} ${OV6} ${OV7} ${BASE} ${CONFOBJ}


sys:	${OFILE} checkit

${OFILE}: ${OBJ}
	${LD} -X -n -o ${OFILE}\
	${CONFOBJ}\
	-Z ${OV1}\
	-Z ${OV2}\
	-Z ${OV3}\
	-Z ${OV4}\
	-Z ${OV5}\
	-Z ${OV6}\
	-Z ${OV7}\
	-L ${BASE}


m40.o:	../conf/mch0.s ../conf/m40.s
	${AS} -o m40.o ../conf/mch0.s ../conf/m40.s

l40.o: ../conf/l40.s
	${AS} -o l40.o ../conf/l40.s

checkit:getsymb.sh
	size ${OFILE}
	csh ./getsymb.sh

allinstall:genallsys.sh sys
	csh ./genallsys.sh
	cp rlunix ${DISTRROOT}
	cp rpunix ${DISTRROOT}
	cp rkunix ${DISTRROOT}
	cp xpunix ${DISTRROOT}
	cp hkunix ${DISTRROOT}
	cp dvunix ${DISTRROOT}

clean:
	rm -f *.o ??unix

c40.o:		../conf/c40.c
	${CC} ${CFLAGS} -c $?

acct.o:		../sys/acct.c
	${CC} ${CFLAGS} -c $?

alloc.o :	../sys/alloc.c 
	${CC} ${CFLAGS} -c $?

clock.o :	../sys/clock.c
	${CC} ${CFLAGS} -c $?

fakemx.o:	../sys/fakemx.c
	${CC} ${CFLAGS} -c $?

fio.o:		../sys/fio.c
	${CC} ${CFLAGS} -c $?

ioctl.o:	../sys/ioctl.c
	${CC} ${CFLAGS} -c $?

iget.o:		../sys/iget.c
	${CC} ${CFLAGS} -c $?

machdep.o:	../sys/machdep.c
	${CC} ${CFLAGS} -c $?

main.o:		../sys/main.c
	${CC} ${CFLAGS} -c $?

malloc.o:	../sys/malloc.c
	${CC} ${CFLAGS} -c $?

nami.o:		../sys/nami.c
	${CC} ${CFLAGS} -c $?

pipe.o:		../sys/pipe.c
	${CC} ${CFLAGS} -c $?

prf.o:		../sys/prf.c
	${CC} ${CFLAGS} -c $?

prim.o:		../sys/prim.c
	${CC} ${CFLAGS} -c $?

rdwri.o:	../sys/rdwri.c
	${CC} ${CFLAGS} -c $?

sig.o:		../sys/sig.c
	${CC} ${CFLAGS} -c $?

slp.o:		../sys/slp.c
	${CC} ${CFLAGS} -c $?

subr.o:		../sys/subr.c
	${CC} ${CFLAGS} -c $?

sys1.o:		../sys/sys1.c
	${CC} ${CFLAGS} -c $?

sys2.o:		../sys/sys2.c
	${CC} ${CFLAGS} -c $?

sys3.o:		../sys/sys3.c
	${CC} ${CFLAGS} -c $?

sys4.o:		../sys/sys4.c
	${CC} ${CFLAGS} -c $?

sysent.o:	../sys/sysent.c
	${CC} ${CFLAGS} -c $?

syslocal.o:	../sys/syslocal.c
	${CC} ${CFLAGS} -c $?

text.o:		../sys/text.c
	${CC} ${CFLAGS} -c $?

trap.o:		../sys/trap.c
	${CC} ${CFLAGS} -c $?

ureg.o:		../sys/ureg.c
	${CC} ${CFLAGS} -c $?



bio.o:		../dev/bio.c
	${CC} ${CFLAGS} -c $?

dkleave.o:	../dev/dkleave.c
	${CC} ${CFLAGS} -c $?

dsort.o :	../dev/dsort.c 
	${CC} ${CFLAGS} -c $?

dvhp.o:		../dev/dvhp.c
	${CC} ${CFLAGS} -c $?

dz.o:		../dev/dz.c
	${CC} ${CFLAGS} -c $?

hk.o:		../dev/hk.c
	${CC} ${CFLAGS} -c $?

ht.o:		../dev/ht.c
	${CC} ${CFLAGS} -c $?

kl.o:		../dev/kl.c
	${CC} ${CFLAGS} -c $?

lp.o:		../dev/lp.c
	${CC} ${CFLAGS} -c $?

mem.o:		../dev/mem.c
	${CC} ${CFLAGS} -c $?

partab.o:	../dev/partab.c
	${CC} ${CFLAGS} -c $?

rk.o:		../dev/rk.c
	${CC} ${CFLAGS} -c $?

rl.o:		../dev/rl.c
	${CC} ${CFLAGS} -c $?

rp.o:		../dev/rp.c
	${CC} ${CFLAGS} -c $?

sys.o:		../dev/sys.c
	${CC} ${CFLAGS} -c $?

tm.o:		../dev/tm.c
	${CC} ${CFLAGS} -c $?

ts.o:		../dev/ts.c
	${CC} ${CFLAGS} -c $?

tty.o:		../dev/tty.c
	${CC} ${CFLAGS} -c $?

ttynew.o:	../dev/ttynew.c
	${CC} ${CFLAGS} -c $?

ttyold.o:	../dev/ttyold.c
	${CC} ${CFLAGS} -c $?

xp.o:		../dev/xp.c
	${CC} ${CFLAGS} -c $?

	
${INCLUDES}/sys/param.h: ${INCLUDES}/whoami.h
	touch ${INCLUDES}/sys/param.h

