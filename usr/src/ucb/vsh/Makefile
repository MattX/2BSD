#
# vsh makefile
#
# FP should be -f for non-floating point machines

CFLAGS=	-O
OBJS=	account.o ascii.o at.o classify.o cmdini.o cmdload.o cmdrun.o \
	curdir.o curses.o dir.o dircmd.o dirlist.o enterf.o file.o \
	grep.o help.o main.o make.o misccmd.o options.o page.o process.o \
	readarg.o remove.o show.o showopen.o strings.o tty.o xeq.o
 
all: vsh

install:
	install -s -o bin -g bin -m 711 vsh ${DESTDIR}/usr/ucb
	install -c -o bin -g bin -m 644 genhelp ${DESTDIR}/usr/lib/vsh
	install -c -o bin -g bin -m 644 rmhelp ${DESTDIR}/usr/lib/vsh
	install -c -o bin -g bin -m 644 showhelp ${DESTDIR}/usr/lib/vsh
	install -c -o bin -g bin -m 644 dflt.vshrc ${DESTDIR}/usr/lib/vsh

vsh: $(OBJS)
	${CC} -o $@ ${LFLAG} ${FP} ${OBJS} -ltermcap
	size vsh

clean:
	rm -f ${OBJS} vsh

.c.o:
	${CC} ${CFLAGS} ${FP} -c $*.c

account.o:	account.c
ascii.o:	ascii.c
at.o:		at.c
classify.o:	classify.c
cmdini.o:	cmdini.c
cmdload.o:	cmdload.c
cmdrun.o:	cmdrun.c
curdir.o:	curdir.c
curses.o:	curses.c
dir.o:		dir.c
dircmd.o:	dircmd.c
dirlist.o:	dirlist.c
enterf.o:	enterf.c
file.o:		file.c
grep.o:		grep.c
help.o:		help.c
main.o:		main.c
make.o:		make.c
misccmd.o:	misccmd.c
options.o:	options.c
page.o:		page.c
process.o:	process.c
readarg.o:	readarg.c
remove.o:	remove.c
show.o:		show.c
showopen.o:	showopen.c
strings.o:	strings.c
tty.o:		tty.c
xeq.o:		xeq.c
