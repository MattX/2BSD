# @(#)makefile	1.6 12/3/80
NEWSDIR = /usr/spool/news
BIN = /usr/local
NEWS = netnews

NEWSUSR = daemon
NEWSGRP = daemon

DEBUG=

# following definitions pass the above variables to extern.c
EXTERNF1 = '-DNEWSDIR="$(NEWSDIR)'
EXTERNF2 = '-DNEWSUSR="$(NEWSUSR)"' '-DNEWSGRP="$(NEWSGRP)"'
BINFLAG  = '-DBINDIR="$(BIN)'
# This defines a filter that stops for each screenful.  If you
# leave it out news simply won't stop, or you can use more or
# your favorite local filter.
PAGE=	'-DPAGE="/usr/ucb/more"'

CFLAGS = -O ${DEBUG}
LFLAGS = -n -s
OFILES = cancel.o extern.o index.o input.o main.o\
		makebits.o misc.o readr.o subscr.o getdate.o
LFILES = cancel.c extern.c index.c input.c main.c\
		makebits.c misc.c readr.c subscr.c
SASRCS=	nchk.c uurec.c recnews.c sendanews.c sendbnews.c
SRCS=	params.h $(LFILES) $(SASRCS)
VGRIND=	csh /usr/ucb/vgrind

all:	$(NEWS) nchk uurec recnews sendanews sendbnews

$(NEWS):	$(OFILES)
	$(CC) $(OFILES) $(LFLAGS) -o $(NEWS)

cp:	$(NEWS) uurec recnews sendanews sendbnews nchk
	cp $(NEWS) $(BIN)/$(NEWS)
	chown $(NEWSUSR) $(BIN)/$(NEWS)
	chgrp $(NEWSGRP) $(BIN)/$(NEWS)
	chmod 6755 $(BIN)/$(NEWS)
	-rm -f /usr/bin/rnews
	-ln $(BIN)/$(NEWS) /usr/bin/rnews
	cp nchk $(BIN)/nchk
	cp uurec $(BIN)/uurec
	cp recnews $(BIN)/recnews
	cp sendanews $(BIN)/sendanews
	cp sendbnews $(BIN)/sendbnews

install:	cp
	sh news.install $(NEWSDIR) $(NEWSUSR) $(NEWSGRP)

lint:	$(LFILES)
	lint -bah $(LFILES)

clean:
	rm -f $(NEWS) $(OFILES) nchk uurec recnews sendanews sendbnews
	rm -f core index errs

vgrind:
	cp /dev/null index
	vgrind -h "Duke Network News" $(SRCS)
	vgrind -h "Duke Network News" -n makefile
	vgrind -h "Duke Network News" -x index

cancel.o:	cancel.c params.h
	$(CC) $(CFLAGS) -c cancel.c

extern.o:	extern.c params.h makefile
	$(CC) $(CFLAGS) $(EXTERNF1) $(EXTERNF2) -c extern.c

index.o:	index.c params.h
	$(CC) $(CFLAGS) -c index.c

input.o:	input.c params.h
	$(CC) $(CFLAGS) -c input.c

main.o:	main.c params.h
	$(CC) $(CFLAGS) -c main.c

makebits.o:	makebits.c params.h
	$(CC) $(CFLAGS) -c makebits.c

misc.o:	misc.c params.h
	$(CC) $(CFLAGS) -c misc.c

readr.o:	readr.c params.h
	$(CC) $(CFLAGS) $(PAGE) -c readr.c

subscr.o:	subscr.c params.h
	$(CC) $(CFLAGS) -c subscr.c

nchk:	nchk.c
	$(CC) $(CFLAGS) -s $(EXTERNF1) nchk.c -o nchk

uurec:	uurec.c
	$(CC) $(CFLAGS) $(BINFLAG) -s uurec.c -o uurec

recnews:	recnews.c
	$(CC) $(CFLAGS) $(BINFLAG) -s recnews.c -o recnews

sendanews:	sendanews.c
	$(CC) $(CFLAGS) $(BINFLAG) -s sendanews.c -o sendanews

sendbnews:	sendbnews.c
	$(CC) $(CFLAGS) $(BINFLAG) -s sendbnews.c -o sendbnews
