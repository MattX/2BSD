#
#	@{#}Makefile	1.4 {Berkeley} 5/4/82
#
#	error.h			error header file
#	errorinput.c		does all of the input, and canonicalizes errors
#	errorpi.c		canonicalizes pi errors
#	errormain.c		main
#	errorsubr.c		does grotty subroutine work
#	errorfilter.c		"greps" out unwanted messages
#	errortouch.c		touches all mentioned files
#

HDRS = error.h
PHDRS = ${HDRS}
SRCS = errormain.c errorinput.c errorpi.c errorsubr.c errorfilter.c errortouch.c
PSRCS = ${SRCS}
OBJS = errormain.o errorinput.o errorpi.o errorsubr.o errorfilter.o errortouch.o

CFLAGS=	-O

all: error

error: ${OBJS}
	${CC} ${LFLAG} ${CFLAGS} ${OBJS} -o error

${OBJS}: ${HDRS}

install:
	install -s -o bin -g bin -m 711 error ${DESTDIR}/usr/ucb

clean:
	rm -f error ${OBJS}

lint:
	lint ${LINTFLAGS} ${SRCS}

psrcs:
	echo ${PHDRS} ${PSRCS}

print:
	print ${PHDRS} ${PSRCS}

errormain.o:	errormain.c
errorinput.o:	errorinput.c
errorpi.o:	errorpi.c
errorsubr.o:	errorsubr.c
errorfilter.o:	errorfilter.c
errortouch.o:	errortouch.c
