#
#	makefile for screen package documentation
#
NROFF=	/usr/bin/nroff
TROFF=	/usr/ucb/vtroff
NFLAGS=	-Tcrt
TFLAGS=
BINDIR=	/usr/doc/curses
RAW=	macros appen.B appen.A intro.0 intro.1 intro.2 intro.3 intro.4 \
	intro.5 doc.I doc.II doc.III doc.IV curses.3
SOURCE=	macros appen.B appen.A.tbl intro.0 intro.1 intro.2.tbl intro.3 \
	intro.4 intro.5 doc.I doc.II doc.III doc.IV

Curses: Master macros ${SOURCE}
	nroff -me ${NFLAGS} Master > Curses

troff: Master macros ${SOURCE}
	${TROFF} -me ${TFLAGS} Master ;
	
intro.5: ${DOCS}

intro.2.tbl: intro.2
	tbl intro.2 > intro.2.tbl

appen.A.tbl: appen.A
	tbl appen.A > appen.A.tbl

spell: ${SOURCE}
	/bin/csh ./spellit ${SOURCE} ${DOCS}

install:
	cp Makefile ${RAW} ${BINDIR}

lpr: Curses
	-lpr Curses ; lpq
	-lpq
