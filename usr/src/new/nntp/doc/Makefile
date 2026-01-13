#       Makefile        2.0 (2.11BSD) 2025/11/29

# Created by Stan Barber; Mon Apr  6 13:25:27 CDT 1987

MANDIR	= /usr/man

# from here down should be constant
SHELL	= /bin/sh
MACROS	= -man
MANPAGES= nntpd.8c mkgrdates.8c getactive.1
EXTRACT	= ./config.sh

all: $(MANPAGES)
	@echo "Done"

nntpd.8c: nntpd.dst $(EXTRACT)
	$(EXTRACT) nntpd.dst nntpd.8c

mkgrdates.8c: mkgrdates.dst $(EXTRACT)
	$(EXTRACT) mkgrdates.dst mkgrdates.8c

getactive.1: getactive.dst $(EXTRACT)
	$(EXTRACT) getactive.dst getactive.1

install: $(MANPAGES)
	mv nntpd.8c $(MANDIR)/man8/nntpd.8c
	mv mkgrdates.8c $(MANDIR)/man8/mkgrdates.8c
	mv getactive.1 $(MANDIR)/man1/getactive.1
	mv nntpxmit.1 $(MANDIR)/man1/nntpxmit.1

clean:
	-rm -f $(MANPAGES)
