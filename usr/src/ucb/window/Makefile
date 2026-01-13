#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	3.3 (2.11BSD) 2025/11/26

LDFLAGS       = -i
CFLAGS        = -O

EXTHDRS	      = /usr/include/fcntl.h \
		/usr/include/setjmp.h \
		/usr/include/sgtty.h \
		/usr/include/signal.h \
		/usr/include/stdio.h \
		/usr/include/sys/errno.h \
		/usr/include/sys/ioctl.h \
		/usr/include/sys/resource.h \
		/usr/include/sys/signal.h \
		/usr/include/sys/socket.h \
		/usr/include/sys/time.h \
		/usr/include/sys/ttychars.h \
		/usr/include/sys/ttydev.h \
		/usr/include/sys/types.h \
		/usr/include/sys/wait.h

HDRS	      = alias.h \
		char.h \
		context.h \
		defs.h \
		lcmd.h \
		local.h \
		parser.h \
		string.h \
		token.h \
		tt.h \
		value.h \
		var.h \
		ww.h

LIBS	      = /usr/lib/libtermcap.a

LINKER	      = cc

OBJS	      = char.o \
		cmd.o \
		cmd1.o \
		cmd2.o \
		cmd3.o \
		cmd4.o \
		cmd5.o \
		cmd6.o \
		cmd7.o \
		context.o \
		error.o \
		lcmd.o \
		lcmd1.o \
		lcmd2.o \
		main.o \
		mloop.o \
		parser1.o \
		parser2.o \
		parser3.o \
		parser4.o \
		parser5.o \
		scanner.o \
		startup.o \
		string.o \
		ttf100.o \
		ttgeneric.o \
		tth19.o \
		tth29.o \
		ttinit.o \
		ttoutput.o \
		tttermcap.o \
		tttvi925.o \
		var.o \
		win.o \
		wwadd.o \
		wwalloc.o \
		wwbox.o \
		wwchild.o \
		wwclose.o \
		wwclreol.o \
		wwclreos.o \
		wwcursor.o \
		wwdata.o \
		wwdelchar.o \
		wwdelete.o \
		wwdelline.o \
		wwdump.o \
		wwend.o \
		wwenviron.o \
		wwerror.o \
		wwflush.o \
		wwframe.o \
		wwgets.o \
		wwinit.o \
		wwinschar.o \
		wwinsline.o \
		wwiomux.o \
		wwlabel.o \
		wwmisc.o \
		wwmove.o \
		wwopen.o \
		wwprintf.o \
		wwpty.o \
		wwputc.o \
		wwputs.o \
		wwredraw.o \
		wwredrawwin.o \
		wwrint.o \
		wwscroll.o \
		wwsize.o \
		wwspawn.o \
		wwsuspend.o \
		wwtty.o \
		wwunframe.o \
		wwupdate.o \
		wwwrite.o

PROGRAM	      = a.out

SRCS	      = char.c \
		cmd.c \
		cmd1.c \
		cmd2.c \
		cmd3.c \
		cmd4.c \
		cmd5.c \
		cmd6.c \
		cmd7.c \
		context.c \
		error.c \
		lcmd.c \
		lcmd1.c \
		lcmd2.c \
		main.c \
		mloop.c \
		parser1.c \
		parser2.c \
		parser3.c \
		parser4.c \
		parser5.c \
		scanner.c \
		startup.c \
		string.c \
		ttf100.c \
		ttgeneric.c \
		tth19.c \
		tth29.c \
		ttinit.c \
		ttoutput.c \
		tttermcap.c \
		tttvi925.c \
		var.c \
		win.c \
		wwadd.c \
		wwalloc.c \
		wwbox.c \
		wwchild.c \
		wwclose.c \
		wwclreol.c \
		wwclreos.c \
		wwcursor.c \
		wwdata.c \
		wwdelchar.c \
		wwdelete.c \
		wwdelline.c \
		wwdump.c \
		wwend.c \
		wwenviron.c \
		wwerror.c \
		wwflush.c \
		wwframe.c \
		wwgets.c \
		wwinit.c \
		wwinschar.c \
		wwinsline.c \
		wwiomux.c \
		wwlabel.c \
		wwmisc.c \
		wwmove.c \
		wwopen.c \
		wwprintf.c \
		wwpty.c \
		wwputc.c \
		wwputs.c \
		wwredraw.c \
		wwredrawwin.c \
		wwrint.c \
		wwscroll.c \
		wwsize.c \
		wwspawn.c \
		wwsuspend.c \
		wwtty.c \
		wwunframe.c \
		wwupdate.c \
		wwwrite.c

all:		${PROGRAM}

${PROGRAM}:	$(OBJS) $(LIBS)
		$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:;		rm -f $(OBJS) a.out

depend:		${SRCS}
		mkdep ${CFLAGS} ${SRCS}

install:	$(PROGRAM) windowrc
		install -c -s -m 755 -o root a.out /usr/ucb/window
#		install -c windowrc /usr/local/lib

tags:           $(HDRS) $(SRCS); @ctags $(HDRS) $(SRCS)

lint:;		@lint -z $(SRCS)
