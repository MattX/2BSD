# Makefile for f77 I/O lib, libI77.a
#
# @(#)Makefile	1.1 (Berkeley) 5/27/83

FC=	f77
CFLAGS=	-O ${FP}
DESTDIR=
DEST=	${DESTDIR}/usr/lib
EXTHDR=
FFLAGS=	-O ${FP}
FP=				# -f for machines without floating point
LIB=	libI77.a
OVLIB=	libovI77.a

HDRS=	f_errno.h fio.h fiodefs.h format.h lio.h ioprim.h rat_io.h

OBJS=	backspace.o close.o dballoc.o dfe.o dofio.o dolio.o doprnt.o doscan.o \
	douio.o due.o endfile.o err.o f_errlist.o fmt.o fmtlib.o getlin.o \
	iio.o inquire.o ioinit.o ioprim.o lread.o lwrite.o open.o printn_.o \
	putlin.o rdfe.o rdfmt.o rewind.o riio.o rsfe.o rwiio.o scann_.o sfe.o \
	sue.o util.o wdfe.o wiio.o wrtfmt.o wsfe.o

SRCS=	backspace.c close.c dballoc.c dfe.c dofio.c dolio.c doprnt.c doscan.c \
	douio.c due.c endfile.c err.c f_errlist.c fmt.c fmtlib.c getlin.r \
	iio.c inquire.c ioinit.f ioprim.c lread.c lwrite.c open.c printn_.c \
	putlin.r rdfe.c rdfmt.c rewind.c riio.c rsfe.c rwiio.c scann_.c sfe.c \
	sue.c util.c wdfe.c wiio.c wrtfmt.c wsfe.c

all: Version ${LIB} ${OVLIB}

${LIB}: ${OBJS}
	@echo -n "Loading ${LIB} ... "
	@ar cru ${LIB} ${OBJS} Version ioinit.f
	ranlib ${LIB}

${OVLIB}: ${OBJS}
	cp ioinit.f overlay
	@echo -n "Loading ${OVLIB} ... "
	@cd overlay; ar cru ../${OVLIB} ${OBJS} Version ioinit.f
	ranlib ${OVLIB}

Version: ${SRCS} mkvers
	@rm -f Version.c
	./mkvers ${OVLIB} ${SRCS} > Version.c
	${CC} -c Version.c
	mv Version.o overlay/Version
	@rm -f Version.c
	./mkvers ${LIB} ${SRCS} > Version.c
	${CC} -c Version.c
	mv Version.o Version

mkvers: mkvers.c
	@${CC} mkvers.c -o mkvers

clean:
	@rm -f ${OBJS} ${LIB} overlay/*

extract:
	@ar xo ${DEST}/${LIB}; rm -f __.SYMDEF

index:
	@ctags -wx ${HDRS} ${SRCS}

install: ${LIB} libI66.o
	install -o bin -g bin -m 644 ${LIB} ${DEST}
	install -o bin -g bin -m 644 ${OVLIB} ${DEST}
	install -o bin -g bin -m 644 libI66.o ${DEST}

tags: ${HDRS} ${SRCS}
	@ctags ${HDRS} ${SRCS}

.c.o:
	${CC} -V ${CFLAGS} -c $*.c
	-ld -x -r $*.o
	mv a.out overlay/$*.o
	${CC} ${CFLAGS} -c $*.c
	-ld -x -r $*.o
	mv a.out $*.o

.f.o:
	${FC} -V ${FFLAGS} -c $*.f
	-ld -x -r $*.o
	mv a.out overlay/$*.o
	${FC} ${FFLAGS} -c $*.f
	-ld -x -r $*.o
	mv a.out $*.o

.r.o:
	${FC} -V ${FFLAGS} -c $*.r
	-ld -x -r $*.o
	mv a.out overlay/$*.o
	${FC} ${FFLAGS} -c $*.r
	-ld -x -r $*.o
	mv a.out $*.o

backspace.o:	fio.h backspace.c
close.o:	fio.h close.c
dballoc.o:	f_errno.h dballoc.c
dfe.o:		fio.h dfe.c
dofio.o:	fio.h format.h dofio.c
dolio.o:	fio.h lio.h dolio.c
doscan.o:	ioprim.h doscan.c
douio.o:	fio.h douio.c
due.o:		fio.h due.c
endfile.o:	fio.h endfile.c
err.o:		fiodefs.h err.c
fmt.o:		fio.h format.h fmt.c
fmtlib.o:	fio.h fmtlib.c
getlin.o:	rat_io.h getlin.r
iio.o:		fio.h iio.c
inquire.o:	fio.h inquire.c
ioprim.o:	ioprim.h ioprim.c
lread.o:	fio.h lio.h lread.c
lwrite.o:	fio.h lio.h lwrite.c
open.o:		fio.h open.c
printn_.o:	ioprim.h printn_.c
rdfe.o:		fio.h rdfe.c
rdfmt.o:	fio.h format.h rdfmt.c
rewind.o:	fio.h rewind.c
riio.o:		fio.h lio.h riio.c
rsfe.o:		fio.h rsfe.c
rwiio.o:	fio.h lio.h rwiio.c
scann_.o:	ioprim.h scann_.c
sfe.o:		fio.h sfe.c
sue.o:		fio.h sue.c
util.o:		fio.h util.c
wdfe.o:		fio.h wdfe.c
wiio.o:		fio.h lio.h wiio.c
wrtfmt.o:	fio.h format.h wrtfmt.c
wsfe.o:		fio.h wsfe.c
