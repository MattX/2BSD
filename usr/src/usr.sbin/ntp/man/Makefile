#       Makefile        2.0 (2.11BSD) 2025/11/25

MANDIR=/usr/man/cat8
MANSRC= ntp.8 ntpd.8 ntpdc.8
MAN= ntp.0 ntpd.0 ntpdc.0

.SUFFIXES: .8 .0
 
.8.0:
	/usr/man/manroff $*.8 > $*.0

all: ${MAN}

install: ${MAN}
	install -c -m 444 -o root -g staff ntp.0 ${MANDIR}
	install -c -m 444 -o root -g staff ntpd.0 ${MANDIR}
	install -c -m 444 -o root -g staff ntpdc.0 ${MANDIR}

clean:
	@rm -f *~ ${MAN}
