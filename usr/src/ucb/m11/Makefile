.SUFFIXES:	.m11 .obj

CFLAGS=		-O
LFLAG=		-n
LINK=		/usr/ucb/l11
M11=		/usr/ucb/m11
MACXRF=		/usr/ucb/macxrf		# this pathname is wired into exec.m11
MFLAGS=

.m11.obj:
	${M11} ${MFLAGS} $<

OBJS=		exec.obj macro.obj code.obj expr.obj fltg.obj getl.obj \
		lout.obj mac.obj srch.obj xlat.obj misc.obj pst.obj \
		errs.obj debug.obj often.obj xpcor.obj syscalls.obj
PROGS=		exec.m11 macro.m11 code.m11 expr.m11 fltg.m11 getl.m11 \
		lout.m11  mac.m11 srch.m11 xlat.m11 misc.m11 pst.m11 \
		errs.m11 debug.m11 often.m11 xpcor.m11 syscalls.m11 at.sml

all:		m11 macxrf

install:
	install -s -o bin -g bin -m 711 m11 ${DESTDIR}/usr/ucb
	install -s -o bin -g bin -m 711 macxrf ${DESTDIR}/${MACXRF}

m11:		${OBJS}
		${LINK} -ls ${LFLAG} ${OBJS}
		mv exec.out m11

macxrf:		macxrf.c
		${CC} ${LFLAG} -O -o macxrf macxrf.c

clean:
	rm -f *.obj m11 maxcrf

exec.obj:	exec.m11
macro.obj:	macro.m11
code.obj:	code.m11
expr.obj:	expr.m11
fltg.obj:	fltg.m11
getl.obj:	getl.m11
lout.obj:	lout.m11
mac.obj:	mac.m11
srch.obj:	srch.m11
xlat.obj:	xlat.m11
misc.obj:	misc.m11
pst.obj:	pst.m11
errs.obj:	errs.m11
debug.obj:	debug.m11
often.obj:	often.m11
xpcor.obj:	xpcor.m11
syscalls.obj:	syscalls.m11
