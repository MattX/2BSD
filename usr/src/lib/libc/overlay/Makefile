#
# overlay makefile
#

CFLAGS=-O
OV=

OVER=	setjmp.o doprnt.o signal.o vfork.o csv.o

libovc.a: ${OVER}
	ar q ../$@ ${OVER}

clean:
	rm -f ${OVER}

.s.o:
	as ${OV} - -o $*.o $*.s

csv.o:		csv.s
doprnt.o:	doprnt.s
ovprofcsv.o:	ovprofcsv.s
setjmp.o:	setjmp.s
signal.o:	signal.s
vfork.o:	vfork.s
