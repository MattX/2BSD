#
#       @(#)Makefile    1.1 (2.11BSD) 2025/11/28
#

# DFLAGS = -DTEST -DDEBUG
CFLAGS = -O -DGENERIC ${DFLAGS}
LDFLAGS = -i
LIBS =		# extra libs if necessary
CC = cc
LD = $(CC)	# linker; defaults to unstripped executables
SRCS = netcat.c inet_aton.c
OBJS = netcat.o inet_aton.o
PROG = nc
MAN = nc.0
MANSRC = nc.1

all:	${PROG} ${MAN}

nc:	${OBJS}
	${LD} ${LDFLAGS} -o ${PROG} ${OBJS} ${LIBS}

${MAN}:	${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

install: ${PROG} ${MAN}
	install -c -s -o root -g staff $(PROG) /usr/sbin/${PROG}
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8/${MAN}

clean:
	rm -f ${PROG} ${MAN} ${OBJS}
