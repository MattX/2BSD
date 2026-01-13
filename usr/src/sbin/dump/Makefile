# sccsid = "@(#)Makefile	1.7 (2.11BSD) 2025/11/25"
#
#	dump.h			header file
#	dumpitime.c		reads /etc/ddate
#	dumpmain.c		driver
#	dumpoptr.c		operator interface
#	dumptape.c		handles the mag tape and opening/closing
#	dumptraverse.c		traverses the file system
#	unctime.c		undo ctime
#
#	dumpdir.c		program to output tape directories
#
#	DEBUG			use local directory to find ddate and dumpdates
#	TDEBUG			trace out the process forking

CFLAGS=	-O -I.
LDFLAGS=-i

HDRS=	dump.h
SRCS=	dumpitime.c dumpmain.c dumpoptr.c dumptape.c dumptraverse.c unctime.c \
	dumprmt.c
OBJS=	dumpitime.o dumpmain.o dumpoptr.o dumptape.o dumptraverse.o unctime.o
ROBJS=	dumpitime.o dumprmain.o dumpoptr.o dumprtape.o dumptraverse.o \
	unctime.o dumprmt.o

all: dump rdump dumpdir

dump: ${OBJS}
	cc ${CFLAGS} ${LDFLAGS} ${OBJS} -o dump

rdump: ${ROBJS}
	cc ${CFLAGS} ${LDFLAGS} ${ROBJS} -o rdump

dumpdir: dumpdir.o
	cc ${CFLAGS} ${LDFLAGS} dumpdir.o -o dumpdir

install: all
	install -c -s -m 755 dump /sbin/dump
	install -c -s -m 755 rdump /sbin/rdump
	install -c -s -m 755 dumpdir /sbin/dumpdir

clean:
	rm -f *.o dump rdump dumpdir

lint:
	lint -chabx ${CFLAGS} -DRDUMP ${SRCS}

${OBJS}: dump.h

${ROBJS}: dump.h

dumprmain.o: dumpmain.c
	cc -c -DRDUMP ${CFLAGS} dumpmain.c
	mv dumpmain.o dumprmain.o

dumprtape.o: dumptape.c
	cc -c -DRDUMP ${CFLAGS} dumptape.c
	mv dumptape.o dumprtape.o
