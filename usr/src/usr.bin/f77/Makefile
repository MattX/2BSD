#       Makefile        2.1 (2.11BSD) 2025/11/27

# Fortran 77 compiler generating code for the PDP11 using the Ritchie pass 2

#	1996/10/25 - 2.11BSD
#	2020/2/8 - remove unused malloc.c from SRCS
#		   Add -O to CFLAGS.  "Don't use -O here" was a historical relic
#       2025/11/27 - cleanup

AS=		as -V
C2=		/lib/c2
LD=		ld
STRINGS=	f77_strings
CFL=		-DTARGET=PDP11 -DFAMILY=DMR -DHERE=PDP11 -DOUTPUT=BINARY \
		-DPOLISH=POSTFIX -DC_OVERLAY
CFLAGS=		-O -w ${CFL}
LDFLAGS= -i

SRCS= 	data.c driver.c equiv.c error.c exec.c expr.c gram.c init.c \
	intr.c io.c lex.c main.c misc.c pdp11.c pdp11x.c \
	proc.c put.c putdmr.c

PASS1OBJECTS=	data.o equiv.o error.o exec.o expr.o gram.o init.o \
		intr.o io.o lex.o main.o misc.o pdp11.o pdp11x.o proc.o \
		put.o putdmr.o

all:		f77 f77pass1

f77:		driver.o pdp11x.o
		${CC} ${LDFLAGS} driver.o pdp11x.o -o f77
		@size f77

f77pass1:	${PASS1OBJECTS}
		${LD} /lib/crt0.o ${LDFLAGS} -o f77pass1 \
			main.o misc.o gram.o lex.o \
			-Z proc.o init.o \
			-Z equiv.o data.o error.o \
			-Y io.o expr.o intr.o exec.o \
			put.o putdmr.o pdp11.o pdp11x.o -lc
		@size f77pass1

gram.o:		gram.c
		mkstr - f77_strings xx gram.c
		${CC} -S ${CFLAGS} xxgram.c 
		mv xxgram.s gram.s
		rm xxgram.c
		${AS} -u -o gram.o gram.s
		rm gram.s

gram.c:		gram.head gram.dcl gram.expr gram.exec gram.io tokdefs
		( sed <tokdefs "s/#define/%token/" ;\
		cat gram.head gram.dcl gram.expr gram.exec gram.io ) >gram.in
		${YACC} ${YFLAGS} gram.in
		mv y.tab.c gram.c
		rm gram.in

.c.o:
#	Remove error strings
		mkstr - f77_strings xx $*.c
		${CC} -S ${CFLAGS} xx$*.c
		${C2} xx$*.s | ${AS} -u -o $*.o
		rm xx$*.c xx$*.s

driver.o:
pdp11x.o:
		${CC} -c ${CFLAGS} $*.c

defs:		ftypes defines locdefs
		touch defs

tokdefs:	tokens
		grep -n . <tokens | sed "s/\([^:]*\):\(.*\)/#define \2 \1/" >tokdefs

locdefs:	pdp11defs
		cp pdp11defs locdefs

install: all
		install -c -s -m 755 -o root f77 /usr/bin
		install -c -s -m 755 -o root f77pass1 /usr/libexec
		install -c -m 644 ${STRINGS} /usr/share/misc

clean:
		-rm -f f77 f77pass1 *.o gram.c x.c xs.c f77_strings strings

tags:
	cwd=`pwd`; \
	for i in ${SRCS}; do \
		ctags -a -f tags $$cwd/$$i; \
	done


data.o:		defs string_defs data.c
driver.o:	defs drivedefs driver.c
equiv.o:	defs string_defs equiv.c
error.o:	defs string_defs error.c
exec.o:		defs string_defs exec.c
expr.o:		defs string_defs expr.c
gram.o:		defs string_defs gram.c
init.o:		defs string_defs init.c
intr.o:		defs string_defs intr.c
io.o:		defs string_defs io.c
lex.o:		defs string_defs tokdefs lex.c
main.o:		defs string_defs main.c
misc.o:		defs string_defs misc.c
pdp11.o:	defs string_defs pdp11.c
proc.o:		defs string_defs proc.c
put.o:		defs string_defs put.c
putdmr.o:	defs string_defs dmrdefs putdmr.c
