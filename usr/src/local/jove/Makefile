#
# jove Makefile
#

CFLAGS=	-O -DBRL -Dpdp11 -DUSNA
LB=	-ltermlib
BINDIR=	/usr/local
LIBDIR=	/usr/brl/lib/jove

all:	jove findcom

OBJS=	jove_buf.o jove_c.o jove_delete.o jove_disp.o jove_draw.o \
	jove_extend.o jove_funcs.o jove_insert.o jove_io.o jove_main.o \
	jove_marks.o jove_misc.o jove_proc.o jove_re.o jove_screen.o \
	jove_term.o jove_wind.o

jove:	${OBJS}
	${CC} ${LFLAG} -o $@ ${OBJS} ${LB}

findcom: findcom.c jove_tune.h
	${CC} findcom.c -o $@

${OBJS}: jove.h jove_tune.h
jove_io.c jove_screen.c: jove_temp.h

clean:
	rm -f ${OBJS} jove findcom

tags:	${OBJS}
	ctags *.c

install:
	install -s -o bin -g bin -m 711 jove /usr/local
	install -s -o bin -g bin -m 711 findcom /usr/local
