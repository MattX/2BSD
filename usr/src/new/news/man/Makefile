#
# Makefile for news manual pages
#
# If you install news documentation in an appropriately structured
#	man tree (e.g. /usr/local/man) then you can do this better
#

DESTDIR =
MANROFF= /usr/man/manroff
MANDIR = /usr/man

MANFILES = checknews.1 expire.8 getdate.3 inews.8 news.5 newsrc.5 postnews.1 \
	readnews.1 recnews.8 sendbatch.8 sendnews.8 uurec.8 vnews.1

NMANFILES = checknews.0 expire.0 getdate.0 inews.0 news.0 newsrc.0 postnews.0 \
	readnews.0 recnews.0 sendbatch.0 sendnews.0 uurec.0 vnews.0

all: ${NMANFILES}

install: ${NMANFILES}
	install -m 444 -o bin -g bin checknews.0 ${DESTDIR}${MANDIR}/cat1
	install -m 444 -o bin -g bin postnews.0 ${DESTDIR}${MANDIR}/cat1
	install -m 444 -o bin -g bin readnews.0 ${DESTDIR}${MANDIR}/cat1
	install -m 444 -o bin -g bin vnews.0 ${DESTDIR}${MANDIR}/cat1
	install -m 444 -o bin -g bin getdate.0 ${DESTDIR}${MANDIR}/cat3
	install -m 444 -o bin -g bin news.0 ${DESTDIR}${MANDIR}/cat5
	install -m 444 -o bin -g bin newsrc.0 ${DESTDIR}${MANDIR}/cat5
	install -m 444 -o bin -g bin expire.0 ${DESTDIR}${MANDIR}/cat8
	install -m 444 -o bin -g bin inews.0 ${DESTDIR}${MANDIR}/cat8
	install -m 444 -o bin -g bin recnews.0 ${DESTDIR}${MANDIR}/cat8
	install -m 444 -o bin -g bin sendbatch.0 ${DESTDIR}${MANDIR}/cat8
	install -m 444 -o bin -g bin sendnews.0 ${DESTDIR}${MANDIR}/cat8
	install -m 444 -o bin -g bin uurec.0 ${DESTDIR}${MANDIR}/cat8

checknews.0: checknews.1
	${DESTDIR}/${MANROFF} checknews.1 > checknews.0

expire.0: expire.8
	${DESTDIR}/${MANROFF} expire.8 > expire.0

getdate.0: getdate.3
	${DESTDIR}/${MANROFF} getdate.3 > getdate.0

inews.0: inews.8
	${DESTDIR}/${MANROFF} inews.8 > inews.0

news.0: news.5
	${DESTDIR}/${MANROFF} news.5 > news.0

newsrc.0: newsrc.5
	${DESTDIR}/${MANROFF} newsrc.5 > newsrc.0

postnews.0: postnews.1
	${DESTDIR}/${MANROFF} postnews.1 > postnews.0

readnews.0: readnews.1
	${DESTDIR}/${MANROFF} readnews.1 > readnews.0

recnews.0: recnews.8
	${DESTDIR}/${MANROFF} recnews.8 > recnews.0

sendbatch.0: sendbatch.8
	${DESTDIR}/${MANROFF} sendbatch.8 > sendbatch.0

sendnews.0: sendnews.8
	${DESTDIR}/${MANROFF} sendnews.8 > sendnews.0

uurec.0: uurec.8
	${DESTDIR}/${MANROFF} uurec.8 > uurec.0

vnews.0: vnews.1
	${DESTDIR}/${MANROFF} vnews.1 > vnews.0

clean:
	rm -f $(NMANFILES)
