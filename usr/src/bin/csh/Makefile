#
#	makefile	4.1	10/9/80
#
# C Shell with process control; PDP-11 separate I/D Makefile
# Bill Joy UC Berkeley; Jim Kulp IIASA, Austria
#
CFLAGS=	-O
XSTR=	/usr/ucb/xstr
ED=	-ed
AS=	-as
RM=	rm
CXREF=	/usr/ucb/cref
VGRIND=	csh /usr/ucb/vgrind
CTAGS=	/usr/ucb/ctags

# strings.o must be last in OBJS since it can change when previous files compile
OBJS=	sh.o sh.dol.o sh.err.o sh.exec.o sh.exp.o sh.func.o sh.glob.o \
	sh.hist.o sh.lex.o sh.misc.o sh.parse.o sh.print.o sh.sem.o sh.set.o \
	sh.proc.o sh.dir.o sh.time.o alloc.o sh.init.o printf.o \
	doprnt.o strings.o

all: csh

install:
	install -s -o bin -g bin -m 711 csh ${DESTDIR}/bin/csh

csh: ${OBJS} sh.local.h
	${CC} ${LFLAG} ${OBJS} -o csh

# Special massaging of C files for sharing of strings
.c.o:
	${CC} -E ${CFLAGS} $*.c | ${XSTR} -c -
	${CC} -c ${CFLAGS} x.c 
	mv x.o $*.o

strings.o: strings
	${XSTR}
	${CC} -c xs.c
	mv xs.o strings.o
	${RM} -f xs.s

csh.prof: ${OBJS} sh.prof.o sh.local.h mcrt0.o
	rm -f csh.prof
	ld -X mcrt0.o ${OBJS} -o csh.prof -lc

sh.o.prof:
	cp sh.c sh.prof.c
	${CC} -c ${CFLAGS} -DPROF sh.prof.c

lint:
	lint ${CFLAGS} sh*.c

print:
	@pr READ_ME
	@pr makefile makefile.*
	@(size -l a.out; size *.o) | pr -h SIZES
	@ls -l | pr 
	@pr sh*.h sh*.c alloc.c

vprint:
	@pr -l132 makefile 
	@(size -l a.out; size *.o) | pr -l84 -h SIZES
	@ls -l | pr -l132
	@pr -l132 sh*.h [a-rt-z]*.h sh*.c alloc.c

vgrind:
	@cp /dev/null index
	@for i in *.h; do vgrind -t -h "C Shell" $$i >/crp/bill/csh/$$i.t; done
	@for i in *.c; do vgrind -t -h "C Shell" $$i >/crp/bill/csh/$$i.t; done
	@vgrind -t -x -h Index index >/crp/bill/csh/index.t

clean:
	${RM} -f a.out strings x.c xs.c *.o sh.prof.c

tags:	/tmp
	${CTAGS} sh*.c

sh.o:		sh.c
sh.dol.o:	sh.dol.c
sh.err.o:	sh.err.c
sh.exec.o:	sh.exec.c
sh.exp.o:	sh.exp.c
sh.func.o:	sh.func.c
sh.glob.o:	sh.glob.c
sh.hist.o:	sh.hist.c
sh.lex.o:	sh.lex.c
sh.misc.o:	sh.misc.c
sh.parse.o:	sh.parse.c
sh.print.o:	sh.print.c
sh.sem.o:	sh.sem.c
sh.set.o:	sh.set.c
sh.proc.o:	sh.proc.c
sh.dir.o:	sh.dir.c
sh.time.o:	sh.time.c
alloc.o:	alloc.c
sh.init.o:	sh.init.c
printf.o:	printf.c
doprnt.o:	doprnt.s
