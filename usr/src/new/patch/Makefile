DESTDIR=

DEFS=	
CFLAGS=	-O ${DEFS}
SEPFLAG=-i

HDRS=	EXTERN.h INTERN.h common.h config.h inp.h patchlevel.h pch.h \
	util.h version.h
SRCS=	patch.c pch.c inp.c util.c version.c
OBJS=	patch.o pch.o inp.o util.o version.o

.SUFFIXES: .1 .0

.1.0:
	/usr/man/manroff $*.1 > $*.0

patch: ${OBJS}
	cc ${SEPFLAG} ${OBJS} -o patch

install: patch patch.0
	install patch ${DESTDIR}/usr/new
	install -m 444 patch.0 ${DESTDIR}/usr/new/man/cat1

clean:
	rm -f *.o *.orig core tags patch *.0

# The following lint has practically everything turned on.  Unfortunately,
# you have to wade through a lot of mumbo jumbo that can't be suppressed.
# If the source file has a /*NOSTRICT*/ somewhere, ignore the lint message
# for that spot.

lint:
	lint -phbvxac ${DEFS} ${SRCS}

tags:
	ctags ${HDRS} ${SRCS}

depend:
	mkdep ${DEFS} ${SRCS}

# DO NOT DELETE THIS LINE == mkdep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.

