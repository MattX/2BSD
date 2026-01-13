#	Makefile	4.1.2	(2.11BSD) 2025/11/25
#
CFLAGS=	-O
LDFLAGS= -i

calendar: calendar.c
	${CC} ${CFLAGS} ${LDFLAGS} -o calendar calendar.c

install: calendar calendar.sh
	install -c -s -m 755 -o root calendar /usr/libexec
	install -c -m 755 -o root calendar.sh /usr/bin/calendar

clean:
	rm -f calendar
