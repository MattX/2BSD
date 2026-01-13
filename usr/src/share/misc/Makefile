#
# Public domain - 1996/10/21 - sms
#
#	@(#)Makefile	1.3 (2.11BSD) 2025/11/25
#
#  Makefile for misc data files (lib.b from 'bc', usw.)
#

SRCS= eign lib.b Mail.help Mail.tildehelp units \
	vlpmacs mkproto.data

all:	${SRCS}

install:${SRCS}
	-mkdir -p /usr/share/misc
	-chmod a+r,a+x /usr/share/misc
	install -c -m 444 ${SRCS} /usr/share/misc

clean:
