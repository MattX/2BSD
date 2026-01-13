#
# Copyright (c) 1983 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.5 (2.11BSD) 2025/11/29
#
OBJS=	talk.o get_names.o display.o io.o ctl.o init_disp.o \
	msgs.o get_addrs.o ctl_transact.o invite.o look_up.o
SRCS=	talk.c get_names.c display.c io.c ctl.c init_disp.c \
	msgs.c get_addrs.c ctl_transact.c invite.c look_up.c
INCLUDE=talk.h talk_ctl.h
CFLAGS= -O
LDFLAGS= -i

all:	talk

talk:	${OBJS}
	cc ${LDFLAGS} -o talk ${OBJS} -lcurses -ltermlib

${OBJS}: talk.h talk_ctl.h

install: talk
	install -g tty -m 2755 -s talk /usr/ucb/talk

clean:
	rm -f ${OBJS} a.out errs core talk

tags:	${SRCS} ${INCLUDE}
	ctags ${SRCS} ${INCLUDE}

depend: ${SRCS}
	mkdep ${CFLAGS} ${SRCS}
# DO NOT DELETE THIS LINE -- mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

talk.o: talk.c talk.h /usr/include/curses.h /usr/include/stdio.h
talk.o: /usr/include/sgtty.h /usr/include/sys/ioctl.h
talk.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
talk.o: /usr/include/utmp.h
get_names.o: get_names.c talk.h /usr/include/curses.h /usr/include/stdio.h
get_names.o: /usr/include/sgtty.h /usr/include/sys/ioctl.h
get_names.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
get_names.o: /usr/include/utmp.h /usr/include/sys/param.h
get_names.o: /usr/include/sys/localopts.h /usr/include/sys/stddef.h
get_names.o: /usr/include/machine/machparam.h /usr/include/sys/types.h
get_names.o: /usr/include/sys/select.h /usr/include/sys/types.h
get_names.o: /usr/include/signal.h /usr/include/sys/types.h
get_names.o: /usr/include/protocols/talkd.h /usr/include/sys/types.h
get_names.o: /usr/include/sys/socket.h /usr/include/netinet/in.h
get_names.o: /usr/include/string.h /usr/include/sys/types.h
get_names.o: /usr/include/unistd.h /usr/include/sys/types.h
get_names.o: /usr/include/stdint.h
display.o: display.c talk.h /usr/include/curses.h /usr/include/stdio.h
display.o: /usr/include/sgtty.h /usr/include/sys/ioctl.h
display.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
display.o: /usr/include/utmp.h
io.o: io.c talk.h /usr/include/curses.h /usr/include/stdio.h
io.o: /usr/include/sgtty.h /usr/include/sys/ioctl.h /usr/include/sys/ttychars.h
io.o: /usr/include/sys/ttydev.h /usr/include/utmp.h /usr/include/stdio.h
io.o: /usr/include/errno.h /usr/include/string.h /usr/include/sys/types.h
io.o: /usr/include/sys/select.h /usr/include/sys/types.h
io.o: /usr/include/sys/time.h /usr/include/sys/types.h /usr/include/time.h
io.o: /usr/include/sys/types.h
ctl.o: ctl.c talk_ctl.h /usr/include/sys/types.h /usr/include/sys/select.h
ctl.o: /usr/include/sys/types.h /usr/include/protocols/talkd.h
ctl.o: /usr/include/sys/types.h /usr/include/sys/socket.h
ctl.o: /usr/include/netinet/in.h talk.h /usr/include/curses.h
ctl.o: /usr/include/stdio.h /usr/include/sgtty.h /usr/include/sys/ioctl.h
ctl.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
ctl.o: /usr/include/utmp.h /usr/include/errno.h
init_disp.o: init_disp.c talk.h /usr/include/curses.h /usr/include/stdio.h
init_disp.o: /usr/include/sgtty.h /usr/include/sys/ioctl.h
init_disp.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
init_disp.o: /usr/include/utmp.h /usr/include/signal.h
msgs.o: msgs.c /usr/include/signal.h /usr/include/stdio.h
msgs.o: /usr/include/sys/time.h /usr/include/sys/types.h
msgs.o: /usr/include/sys/select.h /usr/include/sys/types.h /usr/include/time.h
msgs.o: /usr/include/sys/types.h talk.h /usr/include/curses.h
msgs.o: /usr/include/stdio.h /usr/include/sgtty.h /usr/include/sys/ioctl.h
msgs.o: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
msgs.o: /usr/include/utmp.h
get_addrs.o: get_addrs.c talk_ctl.h /usr/include/sys/types.h
get_addrs.o: /usr/include/sys/select.h /usr/include/sys/types.h
get_addrs.o: /usr/include/protocols/talkd.h /usr/include/sys/types.h
get_addrs.o: /usr/include/sys/socket.h /usr/include/netinet/in.h talk.h
get_addrs.o: /usr/include/curses.h /usr/include/stdio.h /usr/include/sgtty.h
get_addrs.o: /usr/include/sys/ioctl.h /usr/include/sys/ttychars.h
get_addrs.o: /usr/include/sys/ttydev.h /usr/include/utmp.h /usr/include/errno.h
get_addrs.o: /usr/include/netdb.h
ctl_transact.o: ctl_transact.c talk_ctl.h /usr/include/sys/types.h
ctl_transact.o: /usr/include/sys/select.h /usr/include/sys/types.h
ctl_transact.o: /usr/include/protocols/talkd.h /usr/include/sys/types.h
ctl_transact.o: /usr/include/sys/socket.h /usr/include/netinet/in.h talk.h
ctl_transact.o: /usr/include/curses.h /usr/include/stdio.h /usr/include/sgtty.h
ctl_transact.o: /usr/include/sys/ioctl.h /usr/include/sys/ttychars.h
ctl_transact.o: /usr/include/sys/ttydev.h /usr/include/utmp.h
ctl_transact.o: /usr/include/errno.h /usr/include/sys/time.h
ctl_transact.o: /usr/include/sys/types.h /usr/include/time.h
ctl_transact.o: /usr/include/sys/types.h
invite.o: invite.c talk_ctl.h /usr/include/sys/types.h
invite.o: /usr/include/sys/select.h /usr/include/sys/types.h
invite.o: /usr/include/protocols/talkd.h /usr/include/sys/types.h
invite.o: /usr/include/sys/socket.h /usr/include/netinet/in.h talk.h
invite.o: /usr/include/curses.h /usr/include/stdio.h /usr/include/sgtty.h
invite.o: /usr/include/sys/ioctl.h /usr/include/sys/ttychars.h
invite.o: /usr/include/sys/ttydev.h /usr/include/utmp.h /usr/include/errno.h
invite.o: /usr/include/sys/time.h /usr/include/sys/types.h /usr/include/time.h
invite.o: /usr/include/sys/types.h /usr/include/signal.h /usr/include/setjmp.h
look_up.o: look_up.c talk_ctl.h /usr/include/sys/types.h
look_up.o: /usr/include/sys/select.h /usr/include/sys/types.h
look_up.o: /usr/include/protocols/talkd.h /usr/include/sys/types.h
look_up.o: /usr/include/sys/socket.h /usr/include/netinet/in.h talk.h
look_up.o: /usr/include/curses.h /usr/include/stdio.h /usr/include/sgtty.h
look_up.o: /usr/include/sys/ioctl.h /usr/include/sys/ttychars.h
look_up.o: /usr/include/sys/ttydev.h /usr/include/utmp.h /usr/include/errno.h

# IF YOU PUT ANYTHING HERE IT WILL GO AWAY
