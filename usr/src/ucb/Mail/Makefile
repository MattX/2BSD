#
# Unix version 7.
#
# Sccs Id = "@(#)Makefile.11	1.3 10/27/82";
#
# Mail is too large to fit on a nonseparate I/D machine without
# text overlays; use Ovmakefile to do this.
#

OPTIONS=	-DV7 -DVMUNIX			# vfork and job control
CFLAGS=		-O ${OPTIONS} ${INCLUDES}

SIGRETROSRC=					# for those with job control
SIGRETROOBJ=					# for those with job control

XSTR=		/usr/ucb/xstr
CTAGS=		/usr/ucb/ctags -w
ED=		-ed
AS=		-as
RM=		-rm
OBJS=	aux.o cmd1.o cmd2.o cmd3.o cmdtab.o collect.o config.o edit.o fio.o \
	getname.o head.o v7.local.o lock.o lex.o list.o main.o names.o \
	optim.o popen.o quit.o send.o strings.o temp.o tty.o vars.o \
	version.o ${SIGRETROOBJ} str.o
SRCS=	aux.c cmd1.c cmd2.c cmd3.c cmdtab.c collect.c config.c edit.c fio.c \
	getname.c head.c v7.local.c lock.c lex.c list.c main.c names.c \
	optim.c popen.c quit.c send.c strings.c temp.c tty.c vars.c \
	version.c ${SIGRETROSRC}
HDRS=	rcv.h configdefs.h def.h glob.h v7.local.h local.h

S = $(SRCS) $(HDRS)

#
# Special massaging of C files for sharing of strings
#
.c.o:
	${CC} -E ${OV} ${CFLAGS} $*.c | ${XSTR} -c -
	${CC} -c ${OV} ${CFLAGS} x.c
	mv x.o $*.o
	rm -f x.c

all:	Mail fmt

ovall:	ovMail fmt

install:
	install -s -o bin -g bin -m 711 Mail ${DESTDIR}/usr/ucb
	install -s -o bin -g bin -m 711 fmt ${DESTDIR}/usr/ucb
	(cd misc;install -c -o bin -g bin -m 644 Mail.help ${DESTDIR}/usr/lib)

Mail: $(OBJS)
	$(RM) -f Mail
	@echo Loading ...
	@${CC} ${LFLAG} -o Mail ${OBJS} ${LIBES}
	@size Mail

ovMail: $(OBJS)
	$(RM) -f Mail
	@echo Loading ...
	@ld -X ${CRT} ${LFLAG} -o Mail \
	-Z	getname.o names.o optim.o popen.o send.o strings.o temp.o \
		v7.local.o vars.o \
	-Z	aux.o edit.o head.o lex.o list.o lock.o main.o quit.o tty.o \
	-Z	cmd1.o cmd2.o cmd3.o collect.o ${SIGRETROOBJ} \
	-L	cmdtab.o config.o fio.o \
		str.o version.o -lovc
	@size Mail

tags:	$(SRCS)
	${CTAGS} $(SRCS);

clean:
	$(RM) -f *.o
	rm -f a.out fmt x.c xs.c core
	cp /dev/null strings

lint:
	lint $(CFLAGS) $(SRCS)

fmt:	fmt.c head.c
	$(CC) fmt.c head.c -O -o fmt
	$(RM) fmt.o head.o
	size fmt

str.o: strings
	$(XSTR)
	$(CC) -c $(CFLAGS) xs.c
	mv xs.o str.o
	$(RM) xs.c

config.o:	config.c
	$(CC) -c $(CFLAGS) config.c

cmdtab.o: cmdtab.c
	$(CC) -c $(CFLAGS) cmdtab.c

wc:
	@wc rcv.h def.h glob.h local.h v7.local.h $(SRCS)

sc:
	@grep -c \; rcv.h def.h glob.h local.h v7.local.h $(SRCS)
