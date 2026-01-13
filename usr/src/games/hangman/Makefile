#       Makefile        2.0 (2.11BSD) 2025/11/25

OBJS=		endgame.o extern.o getguess.o getword.o main.o playgame.o \
		prdata.o prman.o prword.o setup.o
CFILES=		endgame.c extern.c getguess.c getword.c main.c playgame.c \
		prdata.c prman.c prword.c setup.c
HDRS=		hangman.h
CFLAGS=		-O
LDFLAGS= -i

all: hangman

tags: $(HDRS) $(CFILES)
	ctags $(HDRS) $(CFILES)

install: hangman
	install -c -s -m 755 hangman /usr/games/hangman
	
hangman: $(OBJS)
	$(CC) $(LDFLAGS) -o hangman $(OBJS) -lcurses -ltermlib

clean:
	rm -f $(OBJS) hangman ? core
