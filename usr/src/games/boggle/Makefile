#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.4 (2.11BSD) 2025/11/25
#
CC = /bin/cc
LDFLAGS= -i
CFLAGS = -O

all:	boggle bogdict

bogdict: comp sfile
	-if [ -f /usr/local/lib/w2a ]; \
		then dict=/usr/local/lib/w2a; \
		else dict=/usr/dict/words; fi; \
	sed -f sfile $$dict | uniq | ./comp >bogdict

install: all
	install -c -s -m 755 boggle /usr/games/boggle
	install -m 644 bogdict /usr/games/lib/bogdict

boggle: boggle.c
	cc ${LDFLAGS} -o boggle ${CFLAGS} boggle.c

comp: comp.c
	cc ${LDFLAGS} -o comp ${CFLAGS} comp.c

clean:
	rm -f bogdict boggle comp *.o
