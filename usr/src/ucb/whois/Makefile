CFLAGS=	-O
LIBS=
LFLAG=-n
ALL=	whois define

all: ${ALL}

whois:	whois.c
	${CC} ${CFLAGS} whois.c -o whois ${LFLAG} ${LIBS}

define:	define.c
	${CC} ${CFLAGS} define.c -o define ${LFLAG} ${LIBS}

install: $(ALL)
	install -s -o bin -g bin -m 755 whois /usr/ucb
	install -s -o bin -g bin -m 755 define /usr/ucb

clean:
	rm -f ${ALL} whois.o define.o
