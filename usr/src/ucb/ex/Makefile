#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	7.13.3 (2.11BSD) 2025/11/26
#
VERSION=3.7
#
# Ex is very large - this version will not fit on PDP-11's without overlay
# software.  Things that can be turned off to save
# space include LISPCODE (-l flag, showmatch and lisp options), CHDIR (the 
# undocumented chdir command.)
#
# Don't define VFORK unless your system has the VFORK system call,
# which is like fork but the two processes have only one data space until the
# child execs. This speeds up ex by saving the memory copy.
#
# If your system expands tabs to 4 spaces you should -DTABS=4 below
#
BINDIR=	/usr/ucb
SBINDIR= /usr/sbin
MISCDIR= /usr/share/misc
CTAGS=	${BINDIR}/ctags
XSTR=	${BINDIR}/xstr
DEBUGFLAGS=	-DTRACE -g
NONDEBUGFLAGS=	-O
DEB=	${NONDEBUGFLAGS}	# or ${DEBUGFLAGS} to to debug
OPTIONS= -DLISPCODE -DCHDIR -DVFORK -DFASTTAG -DTEMP512K
CFLAGS=	-DTABS=8 ${OPTIONS} ${DEB}
LDFLAGS= -X -i
TERMLIB=	-ltermcap
MKSTR=	${BINDIR}/mkstr
OBJS=	ex.o ex_addr.o ex_cmds.o ex_cmds2.o ex_cmdsub.o \
	ex_data.o ex_extern.o ex_get.o ex_io.o ex_put.o ex_re.o \
	ex_set.o ex_subr.o ex_tagio.o ex_temp.o ex_tty.o ex_unix.o \
	ex_v.o ex_vadj.o ex_vget.o ex_vmain.o ex_voper.o \
	ex_vops.o ex_vops2.o ex_vops3.o ex_vput.o ex_vwind.o \
	printf.o strings.o
HDRS=	ex.h ex_argv.h ex_re.h ex_temp.h ex_tty.h ex_tune.h ex_vars.h ex_vis.h
SRC1=	ex.c ex_addr.c ex_cmds.c ex_cmds2.c ex_cmdsub.c
SRC2=	ex_data.c ex_get.c ex_io.c ex_put.c ex_re.c
SRC3=	ex_set.c ex_subr.c ex_tagio.c ex_temp.c ex_tty.c ex_unix.c
SRC4=	ex_v.c ex_vadj.c ex_vget.c ex_vmain.c ex_voper.c
SRC5=	ex_vops.c ex_vops2.c ex_vops3.c ex_vput.c ex_vwind.c
SRC6=	printf.c expreserve.c exrecover.c

.c.o:
	${MKSTR} - exstrings x $*.c
	${CC} -E ${CFLAGS} x$*.c | ${XSTR} -c -
	rm -f x$*.c
	${CC} ${CFLAGS} -c x.c
	mv x.o $*.o

all:	a.out exrecover expreserve

a.out: ${OBJS}
	ld ${LDFLAGS} /lib/crt0.o \
		-Z ex_voper.o ex_vget.o ex_vops.o ex_vops2.o ex_vops3.o \
		-Z ex_set.o ex_re.o ex_io.o ex_tty.o ex_unix.o ex.o ex_v.o \
		-Y ex_addr.o ex_cmds.o ex_cmds2.o ex_cmdsub.o ex_tagio.o \
		   ex_get.o ex_temp.o ex_vadj.o ex_vmain.o ex_vwind.o\
		   ex_vput.o ex_put.o ex_subr.o printf.o strings.o \
		   ex_data.o ex_extern.o ${TERMLIB} -lc

tags:	/tmp
	${CTAGS} -w ex.[hc] ex_*.[hc]

${OBJS}: ${HDRS}

ex_vars.h:
	csh makeoptions ${CFLAGS}

# xstr: hands off!
strings.o: strings
	${XSTR}
	${CC} -c -o strings.o xs.c
	
exrecover: exrecover.o ex_extern.o
	${CC} ${LDFLAGS} exrecover.o ex_extern.o -o exrecover

exrecover.o: exrecover.c
	${CC} ${CFLAGS} -c exrecover.c

expreserve: expreserve.o
	${CC} ${LDFLAGS} expreserve.o -o expreserve

expreserve.o:
	${CC} ${CFLAGS} -c expreserve.c

clean:
#	If we dont have ex we cant make it so don't rm ex_vars.h
	-rm -f a.out exrecover expreserve exstrings strings
	-rm -f *.o x*.[cs] core errs trace

# install in standard place (/usr/ucb)
install: a.out exrecover expreserve
	-rm -f ${BINDIR}/ex
	-rm -f ${BINDIR}/vi
	-rm -f ${BINDIR}/view
	-rm -f /usr/bin/ex
	install -c -s -m 1755 a.out ${BINDIR}/ex
	install -c -m 444  exstrings ${MISCDIR}/exstrings
	ln ${BINDIR}/ex ${BINDIR}/vi
	ln ${BINDIR}/ex ${BINDIR}/view
	ln ${BINDIR}/ex /usr/bin/ex
	chmod 1755 ${BINDIR}/ex
	install -s -o root -m 4755 exrecover ${SBINDIR}/exrecover
	install -s -o root -m 4755 expreserve ${SBINDIR}/expreserve
# The following line normally fails.  This is OK.
	-mkdir /usr/preserve

lint:
	lint -haxc ex.c ex_?*.c
	lint -haxcu exrecover.c
	lint -haxc expreserve.c
