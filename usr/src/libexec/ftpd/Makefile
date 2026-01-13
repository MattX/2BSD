#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O -DSETPROCTITLE
LDFLAGS= -i
SRCS=	ftpd.c ftpcmd.c glob.c logwtmp.c popen.c vers.c
OBJS=	ftpd.o ftpcmd.o glob.o logwtmp.o popen.o vers.o
MAN=	ftpd.0
MANSRC=	ftpd.8

all: ftpd ftpd.0

ftpd: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

ftpd.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} ftpd tags ftpcmd.c

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: ftpd
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	install -c -s -o root -g staff -m 755 ftpd /usr/libexec/ftpd

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
