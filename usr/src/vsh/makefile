CFLAGS=-O
OBJECTS= main.o at.o dir.o xeq.o curdir.o process.o enterf.o \
	help.o page.o dirlist.o tty.o remove.o file.o show.o \
	ascii.o make.o grep.o showopen.o strings.o  \
	curses.o account.o cmdrun.o misccmd.o dircmd.o cmdini.o \
	readarg.o cmdload.o classify.o options.o

vsh:   $(OBJECTS)
	-size vsh;  rm -f vsh;
	cc -o vsh -x $(OBJECTS) -ltermlib;  size vsh;

lint:
	lint -bxac *.c

print:
	/bin/csh /usr/ucb/vprint *.h *.c makefile showhelp genhelp rmhelp

install:
	vshrc.gen
	install -s vsh ${DESTDIR}/usr/local/vsh
	install -c genhelp ${DESTDIR}/usr/local/lib
	install -c rmhelp ${DESTDIR}/usr/local/lib
	install -c showhelp ${DESTDIR}/usr/local/lib
	install dflt.vshrc ${DESTDIR}/usr/local/lib

clean:
	rm -f ${OBJECTS} vsh
