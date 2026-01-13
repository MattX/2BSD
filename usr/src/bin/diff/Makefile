#	@(#)Makefile	4.2 (2.11BSD) 2025/11/25
#
DIFF=	/bin/diff
DIFFH=	/usr/libexec/diffh
PR=	/bin/pr
CFLAGS=	-O -DDIFF='"${DIFF}"' -DDIFFH='"${DIFFH}"' -DPR='"${PR}"'
LDFLAGS=-i

SRCS=	diff.c diffdir.c diffreg.c
HDRS=	diff.h
OBJS=	diff.o diffdir.o diffreg.o

all: diff diffh

diff: ${OBJS}
	cc ${CFLAGS} -o diff ${OBJS} ${LDFLAGS}

diffh: diffh.o
	cc -o diffh diffh.o ${LDFLAGS}

clean:
	rm -f diff diffh ${OBJS} diffh.o

install: all
	install -c -s -m 755 -o root -g staff diff /bin/diff
	install -c -s -m 755 -o root -g staff diffh /usr/libexec/diffh
