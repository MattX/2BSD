#
# makefile for learn
#

LESSNS=	files editor morefiles macros eqn C

FILES=	lrndef lrnref copy.c dounit.c learn.c list.c makpipe.c maktee.c \
	mem.c mysys.c selsub.c selunit.c start.c whatnow.c wrapup.c \
	lcount.c tee.c makefile

OBJS=	copy.o dounit.o learn.o list.o mem.o makpipe.o maktee.o mysys.o \
	selsub.o selunit.o start.o whatnow.o wrapup.o

CFLAGS=	-O
LLIB=	${DESTDIR}/usr/lib/learn

all: learn tee lcount

install: all
	install -s -o bin -g bin -m 711 learn ${DESTDIR}/usr/bin
	install -s -o bin -g bin -m 711 tee ${LLIB}
	install -s -o bin -g bin -m 711 lcount ${LLIB}
	@echo "Now do 'make lessons' if you need to extract the lesson archives"
	@echo "Then do 'make play; make log' to make playpen and log directories"

learn: ${OBJS}
	${CC} ${LFLAG} -o $@ ${OBJS}

lcount tee:
	${CC} ${CFLAGS} -n $@.c -o $@ ${LIBRARY}

lessons: ${LESSNS}

${LESSNS}:
	-rm -r ${LLIB}/$@
	-mkdir ${LLIB}/$@
	(cd ${LLIB}/$@; ar x ../$@.a)

play log:
	-rm -rf ${LLIB}/$@; mkdir ${LLIB}/$@; chmod +w ${LLIB}/$@

check:
	-@test -r ${LLIB}/tee || echo 'tee not present; make tee'
	-@test -r ${LLIB}/lcount || echo 'lcount not present; make lcount'
	-@test -r ${LLIB}/play || echo 'play directory not present; make play'
	-@test -r ${LLIB}/log || echo 'log directory not present; make log'
	-@for i in ${LESSNS}; do test -r ${LLIB}/$$i/L0 || echo $$i not unarchived, make $$i; done

clean:
	rm -f ${OBJS} ${ALL}

.c.o:
	${CC} ${CFLAGS} -c $*.c

copy.o:		lrnref copy.c
dounit.o:	lrnref dounit.c
lcount.o:	lrnref lcount.c
learn.o:	lrnref lrndef learn.c
list.o:		lrnref list.c
makpipe.o:	lrnref makpipe.c
maktee.o:	lrnref maktee.c
mem.o:		lrnref mem.c
mysys.o:	lrnref mysys.c
selsub.o:	lrnref selsub.c
selunit.o:	lrnref selunit.c
start.o:	lrnref start.c
tee.o:		lrnref tee.c
whatnow.o:	lrnref whatnow.c
wrapup.o:	lrnref wrapup.c
