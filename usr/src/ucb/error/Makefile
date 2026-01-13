#
# Copyright (c) 1980 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	5.2 (2.11BSD) 2025/11/26
#
#
#	error.h			error header file
#	errorinput.c		does all of the input, and canonicalizes errors
#	errorpi.c		canonicalizes pi errors
#	errormain.c		main
#	errorsubr.c		does grotty subroutine work
#	errorfilter.c		"greps" out unwanted messages
#	errortouch.c		touches all mentioned files
#

HDRS = error.h
SRCS = errormain.c errorinput.c errorpi.c errorsubr.c errorfilter.c errortouch.c
OBJS = errormain.o errorinput.o errorpi.o errorsubr.o errorfilter.o errortouch.o

LDFLAGS= -i
CFLAGS =  -O $(DFLAGS)

LINTFLAGS = -haxc

error:	$(OBJS)
	$(CC) ${LDFLAGS} $(OBJS) -o error

$(OBJS): $(HDRS)

install: error
	install -c -s -m 755 -o root error /usr/ucb

clean:
	rm -f error $(OBJS)

lint:	
	lint $(LINTFLAGS) $(SRCS)
