#
#	Makefile	4.3	6/11/83
#
# C Shell with process control; VM/UNIX VAX Makefile
# Bill Joy UC Berkeley; Jim Kulp IIASA, Austria
#
CFLAGS= -O -DTELL -DVMUNIX -Ddebug -DVFORK -DPURDUE
XSTR=	/usr/ucb/xstr

LIB =   # the pathname of lib directory
BIN =   # the pathname of bin directory

# strings.o must be last in OBJS since it can change when previous files compile
OBJS=	alloc.o doprnt.o printf.o sh.o sh.dir.o sh.dol.o sh.err.o sh.exec.o \
	sh.exp.o sh.func.o sh.glob.o sh.hist.o sh.init.o sh.lex.o sh.misc.o \
	sh.parse.o sh.print.o sh.proc.o sh.sem.o sh.set.o sh.sig.o sh.time.o \
	strings.o

# Special massaging of C files for sharing of strings
.c.o:
	${CC} -E ${CFLAGS} $*.c | ${XSTR} -c -
	${CC} -c ${CFLAGS} x.c 
	mv x.o $*.o

csh: ${OBJS} sh.local.h
	cc ${OBJS} $(LIB)/libra.a -o csh
	mv csh $(BIN)/rcsh
	rm -f x.c

.DEFAULT:
	${SCCS} get $<

# need an old doprnt, whose output we can trap
doprnt.o: doprnt.c
	cc -E doprnt.c > doprnt.s
	as -o doprnt.o doprnt.s
	rm -f doprnt.s

# strings.o and sh.init.o are specially processed to be shared
strings.o: strings
	${XSTR}
	${CC} -c -R xs.c
	mv xs.o strings.o

sh.init.o:
	${CC} -E ${CFLAGS} sh.init.c | ${XSTR} -c -
	${CC} ${CFLAGS} -c -R x.c
	mv x.o sh.init.o
	

#  The source codes of above functions are exactly the same as in
#  /usr/src/bin/csh except sh.glob.c sh.misc.c and sh.sem.c,
#  which are listed in this directory.
#



