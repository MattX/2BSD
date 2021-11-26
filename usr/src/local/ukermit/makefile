#####
# makefile: for uxkermit on 2.9BSD
# initial: Peter Klapper, 11.11.2021 (aka 1993)
#
CC=cc -O
RM=rm -f
CP=cp
DST=/usr/bin
DOC=/usr/doc
STRIP=strip
CHOWN=chown
CHGRP=chgrp

all:	bsd

distclean: clean uninstall

bsd:
	$(CC) cuclxb.c cuclker.c -o kermit
	$(STRIP) kermit
sysv:
	$(CC) cuclx5.c cuclker.c -o kermit
	$(STRIP) kermit

clean:
	$(RM) *.o kermit

install:
	@echo installing binary ...
	$(CP) kermit $(DST)
	$(CHOWN) bin $(DST)/kermit
	$(CHGRP) bin $(DST)/kermit
	@echo installing docfile ...
	$(CP) cuclkerdoc.txt $(DOC)/kermit.txt
	$(CHOWN) bin $(DOC)/kermit.txt
	$(CHGRP) bin $(DOC)/kermit.txt

uninstall:
	@echo removing binary ...
	$(RM) $(DST)/kermit
	@echo removing manpage ...
	$(RM) $(DOC)/kermit.txt
