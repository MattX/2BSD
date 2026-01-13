#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.2 (2.11BSD) 2025/11/26
#
#
CFLAGS=-O
LDFLAGS=-i
SRCS = msgs.c msgs.h Makefile
OBJS = msgs.o

all: msgs

msgs: ${OBJS}
	cc ${LDFLAGS} -o msgs ${OBJS} -ltermlib

install: msgs
	install -c -s -m 755 -o root msgs /usr/ucb

clean:
	-rm -f msgs ${OBJS}
