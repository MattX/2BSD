#	@(#)Makefile	5.3	(Berkeley)	86/05/15
#
DESTDIR=
CFLAGS=	-O
SEPFLAG=-i

INCPATH=
LFLAGS=	
LIBS=

SRCS=	ansitape.c vmsprep.c
MSRC=	ansitape.1 vmsprep.1
MOBJ=	ansitape.0 vmsprep.0

.SUFFIXES: .1 .0

.1.0:
	/usr/man/manroff $*.1 > $*.0

all:	ansitape vmsprep

ansitape: ansitape.c
	cc ${CFLAGS} ${SEPFLAG} -o ansitape ansitape.c

vmsprep: vmsprep.c
	cc ${CFLAGS} ${SEPFLAG} -o vmsprep vmsprep.c

install: vmsprep ansitape ${MOBJ}
	install -s ansitape ${DESTDIR}/usr/new/ansitape
	install -s vmsprep ${DESTDIR}/usr/new/vmsprep
	install -c -m 444 ansitape.0 ${DESTDIR}/usr/new/man/cat1/ansitape.0
	install -c -m 444 vmsprep.0 ${DESTDIR}/usr/new/man/cat1/vmsprep.0

clean:
	rm -f a.out core errs makefile.bak Makefile.bak ansitape vmsprep *.0

depend:
	cc -M ${INCPATH} vmsprep.c ansitape.c | sed 's/\.o//' | \
	awk ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$2) > 78) { print rec; rec = $$0; } \
		       else rec = rec " " $$2 } } \
	      END { print rec } ' > makedep
	echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	echo '$$r makedep' >>eddep
	echo 'w' >>eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep
	echo '# DEPENDENCIES MUST END AT END OF FILE' >> Makefile
	echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> Makefile
	echo '# see make depend above' >> Makefile

# DO NOT DELETE THIS LINE -- make depend uses it


vmsprep: vmsprep.c /usr/include/stdio.h /usr/include/ctype.h
vmsprep: /usr/include/strings.h
ansitape: ansitape.c /usr/include/sys/types.h /usr/include/sys/time.h
ansitape: /usr/include/time.h /usr/include/sys/mtio.h /usr/include/sys/ioctl.h
ansitape: /usr/include/sys/ttychars.h /usr/include/sys/ttydev.h
ansitape: /usr/include/sys/file.h /usr/include/sys/stat.h /usr/include/a.out.h
ansitape: /usr/include/sys/exec.h /usr/include/stdio.h /usr/include/ctype.h
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
