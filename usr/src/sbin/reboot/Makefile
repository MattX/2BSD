#
# Public Domain.  1996/11/16 - Steven Schultz
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
CFLAGS=	 -O
LDFLAGS= -i
SRCS=	reboot.c
OBJS=	reboot.o
MAN=	reboot.0
MANSRC=	reboot.8

all: reboot reboot.0

reboot: ${OBJS}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ ${OBJS}

reboot.0: ${MANSRC}
	/usr/man/manroff ${MANSRC} > ${MAN}

clean:
	rm -f ${OBJS} ${MAN} reboot tags 

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}

install: reboot
	install -c -o root -g staff -m 444 ${MAN} /usr/man/cat8
	-rm -f /usr/man/cat8/fastboot.0 /usr/man/cat8/halt.0
	ln /usr/man/cat8/reboot.0 /usr/man/cat8/fastboot.0
	ln /usr/man/cat8/reboot.0 /usr/man/cat8/halt.0
	install -c -s -o root -g staff -m 755 reboot /sbin/reboot
	-rm /sbin/fastboot /sbin/halt
	ln /sbin/reboot /sbin/halt
	ln /sbin/reboot /sbin/fastboot

lint: ${SRCS}
	lint -hax ${SRCS}

tags: ${SRCS}
	ctags ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
