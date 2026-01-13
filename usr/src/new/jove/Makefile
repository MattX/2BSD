#       Makefile        2.0 (2.11BSD) 2025/11/29

###########################################################################
# This program is Copyright (C) 1986, 1987, 1988 by Jonathan Payne.  JOVE #
# is provided to you without charge, and with no warranty.  You may give  #
# away copies of JOVE, including sources, provided that this notice is    #
# included in all the files.                                              #
###########################################################################

# TMPDIR is where the tmp files get stored, usually /tmp or /tmp/jove.  If
# your system does not remove subdirectories of /tmp on reboot (lots do
# remove them these days) then it makes sense to make TMPDIR be /tmp/jove.
# But if you want to recover buffers on system crashes, you should create a
# directory that doesn't get clearned upon reboot, and use that instead.
# You would probably want to clean out that directory periodically with
# ``cron''.  LIBDIR is for online documentation, the PORTSRV process,
# RECOVER, and the system-wide .joverc file.  BINDIR is where to put the
# executables JOVE and TEACHJOVE.  MANDIR is where the manual pages go for
# JOVE, RECOVER and TEACHJOVE.  MANEXT is the extension for the man pages,
# e.g., jove.1 or jove.l or jove.m.

TMPDIR = /tmp
LIBDIR = /usr/new/lib/jove
BINDIR = /usr/new
MANDIR = /usr/new/man/cat1
MANEXT = 0
SHELL = /bin/csh

# These should all just be right if the above ones are.
JOVE = $(BINDIR)/jove
TEACHJOVE = $(BINDIR)/teachjove
RECOVER = $(LIBDIR)/recover
PORTSRV = $(LIBDIR)/portsrv
JOVERC = $(LIBDIR)/.joverc
CMDS.DOC = $(LIBDIR)/cmds.doc
TEACH-JOVE = $(LIBDIR)/teach-jove
JOVEM = $(MANDIR)/jove.$(MANEXT)
TEACHJOVEM = $(MANDIR)/teachjove.$(MANEXT)

LIBS = -ltermcap

LDFLAGS = -i
CFLAGS = -O 

BASESEG = keymaps.o ask.o buf.o ctype.o \
	  disp.o insert.o io.o jove.o misc.o re.o \
	  screen.o term.o tune.o util.o version.o
OVLAY1 = abbrev.o funcdefs.o rec.o paragraph.o fmt.o malloc.o vars.o table.o
OVLAY2 = c.o wind.o marks.o fp.o
OVLAY3 = extend.o macros.o
OVLAY4 = iproc.o re1.o argcount.o
OVLAY5 = proc.o scandir.o case.o delete.o move.o

OBJECTS = $(BASESEG) $(OVLAY1) $(OVLAY2) $(OVLAY3) $(OVLAY4) $(OVLAY5)

C_SRC = funcdefs.c abbrev.c argcount.c ask.c buf.c c.c case.c ctype.c \
	delete.c disp.c extend.c fp.c fmt.c insert.c io.c iproc.c \
	jove.c macros.c malloc.c marks.c misc.c move.c paragraph.c \
	proc.c re.c re1.c rec.c scandir.c screen.c table.c term.c util.c \
	vars.c version.c wind.c getch.c mac.c

SOURCES = $(C_SRC) portsrv.c recover.c setmaps.c teachjove.c

HEADERS = ctype.h io.h jove.h re.h rec.h table.h temp.h termcap.h \
	tune.h externs.h mac.h

DOCS =	doc/cmds.doc.nr doc/example.rc doc/jove.1 doc/jove.2 doc/jove.3 \
	doc/jove.4 doc/jove.5 doc/jove.nr doc/system.rc \
	doc/teach-jove doc/teachjove.nr doc/README doc/jove.qref

MISC = Makefile Ovmakefile Makefile.dos tune.dos tune.template \
	README Readme.dos Readme.mac iproc-pipes.c iproc-ptys.c

SUPPORT = teachjove.c recover.c setmaps.c portsrv.c keymaps.txt \
	macvert.c menumaps.txt mjovers.Hqx

all:	xjove recover teachjove portsrv macvert

xjove:	$(OBJECTS)
	ld $(LDFLAGS) -X /lib/crt0.o \
			-Z $(OVLAY1) \
			-Z $(OVLAY2) \
			-Z $(OVLAY3) \
			-Z $(OVLAY4) \
			-Z $(OVLAY5) \
			-Y $(BASESEG) \
			-o xjove $(LIBS) -lc
	@-size xjove

portsrv:	portsrv.o
	$(CC) $(LDFLAGS) -o portsrv portsrv.o $(LIBS)

recover:	recover.o tune.o rec.h temp.h
	$(CC) $(LDFLAGS) -o recover recover.o tune.o $(LIBS)

teachjove:	teachjove.o
	$(CC) $(LDFLAGS) -o teachjove teachjove.o $(LIBS)

setmaps:	setmaps.o funcdefs.c
	$(CC) $(LDFLAGS) -o setmaps setmaps.o

teachjove.o:	teachjove.c /usr/include/sys/types.h /usr/include/sys/file.h
	cc -c $(CFLAGS) -DTEACHJOVE=\"$(TEACH-JOVE)\" teachjove.c

# don't optimize setmaps.c because it produces bad code in some places
# for some reason
setmaps.o:	funcdefs.c keymaps.txt
	$(CC) $(MEMFLAGS) -c setmaps.c

# ignore error messages from setmaps
# it doesn't understand ifdefs

keymaps.c:	setmaps keymaps.txt
	-./setmaps < keymaps.txt > keymaps.c

keymaps.o:	keymaps.c jove.h

tune.c: Makefile tune.template
	@echo "/* Changes should be made in Makefile, not to this file! */" > tune.c
	@echo "" >> tune.c
	@sed -e 's;TMPDIR;$(TMPDIR);' \
	     -e 's;LIBDIR;$(LIBDIR);' \
	     -e 's;BINDIR;$(BINDIR);' \
	     -e 's;SHELL;$(SHELL);' tune.template >> tune.c

iproc.o: iproc-ptys.c iproc-pipes.c iproc.c
	$(CC) -c $(CFLAGS) iproc.c

macvert:	macvert.c
	$(CC) $(CFLAGS) -o macvert macvert.c

install: DESTDIR_LIBDIR TEACH-JOVE CMDS.DOC JOVERC PORTSRV RECOVER JOVE \
	TEACHJOVE JOVEM TEACHJOVEM

DESTDIR_LIBDIR:
	-mkdir $(LIBDIR)

TEACH-JOVE: doc/teach-jove
	install -c -m 644 doc/teach-jove $(TEACH-JOVE)

doc/cmds.doc:	doc/cmds.doc.nr doc/jove.4 doc/jove.5
	nroff doc/cmds.doc.nr doc/jove.4 doc/jove.5 > doc/cmds.doc

CMDS.DOC: doc/cmds.doc
	install -c -m 644 doc/cmds.doc $(CMDS.DOC)

JOVERC: doc/system.rc
	install -c -m 644 doc/system.rc $(JOVERC)

PORTSRV: portsrv
	install -c -s -m 755 portsrv $(PORTSRV)

RECOVER: recover
	install -c -s -m 755 recover $(RECOVER)

JOVE: xjove
	install -c -s -m 755 xjove $(JOVE)

TEACHJOVE: teachjove
	install -c -s -m 755 teachjove $(TEACHJOVE)

JOVEM: doc/jove.nr
	@sed -e 's;TMPDIR;$(TMPDIR);' \
	     -e 's;LIBDIR;$(LIBDIR);' \
	     -e 's;SHELL;$(SHELL);' doc/jove.nr \
	| /usr/man/manroff \
	> $(JOVEM)
	chmod 444 $(JOVEM)
	chown bin.bin $(JOVEM)

TEACHJOVEM: doc/teachjove.nr
	@sed -e 's;TMPDIR;$(TMPDIR);' \
	     -e 's;LIBDIR;$(LIBDIR);' \
	     -e 's;SHELL;$(SHELL);' doc/teachjove.nr \
	| /usr/man/manroff \
	> $(TEACHJOVEM)
	chmod 444 $(TEACHJOVEM)
	chown bin.bin $(TEACHJOVEM)

lint:
	lint -n $(C_SRC) tune.c keymaps.c
	@echo Done

tags:
	ctags -w $(C_SRC) $(HEADERS) iproc-ptys.c

clean:
	rm -f a.out core *.o keymaps.c tune.c xjove portsrv recover setmaps \
	teachjove macvert

depend:	${SOURCES}
	mkdep ${CFLAGS} ${SOURCES}
