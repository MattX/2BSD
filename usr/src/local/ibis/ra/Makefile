#
CFLAGS  = -O -DSTART='"$(START)"'

INCLUDE = ra.h local.h

BIN =   # the pathname of bin directory
LIB =   # the pathname of lib directory

START = $(BIN)/start

OBJS =  syscall.o client.o

all:    libra start rfiled

libra:  $(OBJS)
	ar cru $(LIB)/libra.a syscall.o client.o
	ranlib $(LIB)/libra.a

$(OBJS) rfiled.o: $(INCLUDE)
start.o: ra.h

start:  start.o
	cc -o start start.o
	strip start
	mv start $(BIN)
	chown root $(BIN)/start
	chmod 4755 $(BIN)/start

rfiled: rfiled.o
	cc -o rfiled rfiled.o
	strip rfiled
	mv rfiled $(BIN)/rfiled

