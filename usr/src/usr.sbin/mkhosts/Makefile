#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/28
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	mkhosts.c
OBJS=	mkhosts.o
MAN=	mkhosts.0
MANSRC=	mkhosts.8

all: mkhosts mkhosts.0

mkhosts: ${OBJS}
	@echo "Mkhosts only works with the dbm version of the hostent routines."
	@echo "If gethostent() is not in libc, mkhosts isn't installed."
	-cc ${LDFLAGS} -o mkhosts ${OBJS}

mkhosts.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} mkhosts tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: mkhosts
	-if  test -x mkhosts;\
	then \
		install -c -s -o root -g staff -m 755 mkhosts /usr/sbin ;\
		install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8 ;\
	else \
		:;\
	fi

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
