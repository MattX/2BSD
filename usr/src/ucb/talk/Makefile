
DESTDIR=
DOBJECT = talkd.o announce.o process.o table.o print.o
TOBJECT = talk.o get_names.o display.o io.o ctl.o init_disp.o\
	  msgs.o get_addrs.o ctl_trans.o invite.o look_up.o
DSOURCE = talkd.c announce.c process.c table.c print.c
TSOURCE = talk.c get_names.c display.c io.c ctl.c init_disp.c\
	  msgs.c get_addrs.c ctl_trans.c invite.c look_up.c
INCLUDE = talk.h ctl.h talk_ctl.h
SOURCE = ${DSOURCE} ${TSOURCE}
CFLAGS= -O
ALL=	talk talkd

all:	${ALL}

talkd:	${DOBJECT}
	${CC} ${LFLAG} -o talkd ${DOBJECT}

talk:	${TOBJECT}
	${CC} ${LFLAG} -o talk ${TOBJECT} -lcurses -ltermlib

${DOBJECT} : ctl.h
${TOBJECT} : talk.h ctl.h talk_ctl.h

install: $(ALL)
	install -s -o bin -g bin -o 755 talk ${DESTDIR}/usr/ucb
	install -s -o bin -g bin talkd ${DESTDIR}/etc

lint:
	lint ${DSOURCE}

clean:
	rm -f *.[os] $(ALL)

tags:	${SOURCE} ${INCLUDE}
	ctags ${SOURCE} ${INCLUDE}
