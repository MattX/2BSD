#
# Public domain - 1997/1/10 - sms
#
#	@(#)Makefile	1.1 (2.11BSD) 2025/11/25
#
# DESTDIR is defined here to be something which does *NOT* exist and should be
# the same as specified in ../Makefile (See the comments in ../Makefile)

DESTDIR=/foobar
OWN=root
GRP=wheel

# all the usual targets do nothing
all install depend lint clean tags:

distribution:
	install -c -o ${OWN} -g ${GRP} -m 644 dot.cshrc ${DESTDIR}/.cshrc
	install -c -o ${OWN} -g ${GRP} -m 644 dot.login ${DESTDIR}/.login
	install -c -o ${OWN} -g ${GRP} -m 644 dot.profile ${DESTDIR}/.profile
