#
# Public domain - 1997/1/10 - sms
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
# DESTDIR is defined here to be something which does *NOT* exist and should be
# the same as specified in ../Makefile (See the comments in ../Makefile)

DESTDIR=/foobar

OWN=uucp
GRP=daemon

FILES= L-devices L-dialcodes L.cmds L.sys L_stat L_sub SEQF USERFILE \
	clean.daily clean.weekly uucp-map

# all the usual targets do nothing
all install depend lint clean tags:

distribution:
	-rm -rf ${DESTDIR}/etc/uucp
	mkdir ${DESTDIR}/etc/uucp
	chown uucp.daemon ${DESTDIR}/etc/uucp
	chmod 755 ${DESTDIR}/etc/uucp
	install -c -o ${OWN} -g ${GRP} -m 644 ${FILES} ${DESTDIR}/etc/uucp
	chmod 600 ${DESTDIR}/etc/uucp/L.sys
	chmod 640 ${DESTDIR}/etc/uucp/L_stat ${DESTDIR}/etc/uucp/L_sub
	chmod 640 ${DESTDIR}/etc/uucp/SEQF
	chmod 755 ${DESTDIR}/etc/uucp/clean.*
