#
# Public domain - 1996/10/21 - sms.
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#

SRCS=	README end.awk endnote s.acc s.cov s.eqn s.ref s.tbl s.ths s.toc

all:	${SRCS}

install:${SRCS}
	-mkdir -p /usr/share/ms
	-chmod a+r /usr/share/ms
	install -c -m 444 ${SRCS} /usr/share/ms
	chmod a+x /usr/share/ms/endnote

clean:
