# Makefile for tn3270 and friends...
# @(#)Makefile	2.11	2.11

CC	= cc
SEPFLAG	= -i

DEFINES = -DDEBUG
DEFINES =

INCLUDES = -I.
INCLUDES =

OPTIMIZE = -O

CFLAGS	= $(OPTIMIZE) $(INCLUDES) $(DEFINES)

# Lint flags
LINTFLAGS	= -hbxaz
# How to install the bloody thing...

DESTDIR=

BINDIR		= $(DESTDIR)/usr/ucb
ETCDIR		= $(DESTDIR)/etc

# Names for the terminal libraries...
LIBCURSES	= -lcurses
LIBTERM		= -ltermlib

# The source files...
ALLH =	3270.h ascebc.h ectype.h options.h screen.h state.h DEFS.h
M4FILE = termcodes.m4

MOSTC = asctab.c ebctab.c options.c screen.c datastream.c keyboard.c \
	termin.c map3270.c ectype.c
TNMAIN = tn3270.c
MSMAIN = mset.c

#
#	In the vax and pdp environments, we use {vax,pdp}bsubs.s, which give
#	us a fair amount of increased performance.  We have provided
#	genbsubs.c, which perform (more or less) the same function.
#	Don't forget to specify the correct assembly rules if you use them.

#SUBS = genbsubs.c
SUBS = pdpbsubs.s
#SUBS = vaxbsubs.s

#SUBSO = genbsubs.o
SUBSO = pdpbsubs.o
#SUBSO = vaxbsubs.o

# PDP-11 assembly rules:
.s.o:
	/lib/cpp -E $*.s | sed -e 's;^#;/;' > x.s
	as -V -o $*.o x.s
	rm -f x.s

# Vax assembly rules:
#.s.o:
#	/lib/cpp -E $< | as -o $@

ALLC = ${MOSTC} ${TNMAIN} ${MSMAIN} ${SUBS}
ALLC = ${MOSTC} ${TNMAIN} ${MSMAIN}

ALLS =
ALLS = vaxbsubs.s

MOSTO	= tn3270.o ebctab.o asctab.o termin.o keyboard.o \
		datastream.o screen.o map3270.o options.o \
		ectype.o $(SUBSO)

ALLO	= $(MOSTO) mset.o

all:	${ALLH} ${ALLC} ${ALLS} tn3270 mset

tn3270:	$(MOSTO)
	${CC} ${CFLAGS} ${SEPFLAG} -o tn3270 $(MOSTO) $(LIBCURSES) $(LIBTERM)

mset:	mset.o map3270.o
	${CC} ${CFLAGS} ${SEPFLAG} -o mset mset.o map3270.o $(LIBCURSES)

install:
	install -s tn3270 $(BINDIR)
	install -s mset $(BINDIR)
	install -c -m 444 map3270 $(ETCDIR)

clean:
	rm -f $(ALLO) mset tn3270 m4.out errs Makefile.bak

print:
	print ${ALLH} ${M4FILE} ${MOSTC} ${TNMAIN} ${MSMAIN} ${SUBS}
	fold default.map3270 | pr -f -h "` echo default.map3270 - ;date`" | lpr

tags:	${ALLC} ${ALLH}
	ctags -t ${ALLC} ${ALLH}

lint:
	lint ${LINTFLAGS} ${TNMAIN} ${MOSTC} -lcurses
	lint ${LINTFLAGS} ${MSMAIN} map3270.c -lcurses

.DEFAULT:
	sccs get $<

m4.out:		termcodes.m4
	/bin/rm -f m4.out
	m4 termcodes.m4 > m4.out
	/bin/chmod 444 m4.out

depend:
	grep '^#include' ${ALLC} | grep -v '<' | \
	sed -e 's/:[^"]*"\([^"]*\)".*/: \1/' \
	    -e 's/\.c/.o/' \
	    -e 's,../[a-zA-Z]*/,,' | \
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	      END { print rec } ' > makedep
	echo '$$r makedep' >>eddep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep

# DO NOT DELETE THIS LINE

asctab.o: shortnames.h ascebc.h
ebctab.o: shortnames.h ascebc.h
options.o: shortnames.h options.h
screen.o: shortnames.h screen.h 3270.h
datastream.o: shortnames.h ascebc.h 3270.h screen.h
keyboard.o: shortnames.h m4.out ascebc.h 3270.h screen.h options.h ectype.h
termin.o: shortnames.h m4.out state.h
map3270.o: shortnames.h m4.out state.h default.map3270
ectype.o: shortnames.h ectype.h
tn3270.o: shortnames.h
mset.o: shortnames.h state.h m4.out
