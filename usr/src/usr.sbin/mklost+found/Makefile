#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
SRCS=	mklost+found.sh
MAN=	mklost+found.0
MANSRC=	mklost+found.8

all: ${MAN}

mklost+found.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${MAN}

depend:

install:
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -o root -g staff -m 555 ${SRCS} /usr/sbin/mklost+found

lint:

tags:
