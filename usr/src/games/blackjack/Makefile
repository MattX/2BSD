#	Makefile

CFILES=	cbj.c
OBJS=	cbj.o
CFLAGS=	-Dpdp11

cbj:	$(OBJS)
	$(CC) $(OBJS) -o $@

clean:
	rm -f $(OBJS) core

install: cbj
	install -o games -g games -m 711 cbj /usr/games
