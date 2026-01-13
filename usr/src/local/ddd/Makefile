#       Makefile        2.0 (2.11BSD) 2025/11/25

DEFS = -DBSD
CFLAGS = -O $(DEFS)

CC = cc
LINT = lint
RM = rm -f

SRC = ddd.c
OBJ = ddd.o
HEAD =
BIN = ddd
SHAR = ddd.shar

BINDIR = /usr/local

MANSRC = ddd.1
MAN = ddd.0
MANDIR = /usr/local/man/cat1

all: $(BIN) $(MAN)

$(BIN): $(OBJ) $(HEAD) Makefile
	$(CC) -i $(OBJ) -o $(BIN)

lint: $(SRC) $(HEAD)
	$(LINT) $(DEFS) $(SRC)
	touch lint

install: all
	install -c -s -m 755 $(BIN) $(BINDIR)
	install -c -m 444 -o root -g staff $(MAN) $(MANDIR)

$(MAN): $(MANSRC)
	/usr/man/manroff $(MANSRC) > $(MAN)

clean:
	-$(RM) $(BIN) $(OBJ) all lint a.out core *~ #* $(SHAR) $(MAN)

shar: lint README $(MAN) Makefile $(HEAD $(SRC)
	shar README $(MAN) Makefile $(HEAD) $(SRC) > $(SHAR)
