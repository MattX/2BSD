CFLAGS = -O -DCP='"$(CP)"' -DMKDIR='"$(MKDIR)"' -DRMDIR='"$(RMDIR)"'    \
	 -DDIFF='"$(DIFF)"' -DDIFFH='"$(DIFFH)"' -DPR='"$(PR)"'
#
BIN =   # the pathname of bin directory
LIB =   # the pathname of lib directory
#
CP =    $(BIN)/cp
MKDIR = $(BIN)/mkdir
RMDIR = $(BIN)/rmdir
DIFF =  $(BIN)/diff
DIFFH = $(LIB)/diffh
PR =    /bin/pr
#

STD = cat chmod cp ln ls mkdir mv rm rmdir

all:    $(STD) diff diffh

cat:    cat.o $(LIB)/libra.a
	cc cat.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/cat

chmod:  chmod.o $(LIB)/libra.a
	cc chmod.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/chmod

cp:     cp.o $(LIB)/libra.a
	cc cp.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/cp

ln:     ln.o $(LIB)/libra.a
	cc ln.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/ln

ls:     ls.o $(LIB)/libra.a
	cc ls.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/ls

mkdir:  mkdir.o $(LIB)/libra.a
	cc mkdir.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/mkdir

mv:     mv.o $(LIB)/libra.a
	cc mv.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/mv

rm:     rm.o $(LIB)/libra.a
	cc rm.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/rm

rmdir:  rmdir.o $(LIB)/libra.a
	cc rmdir.o $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/rmdir

HDRS =  diff.h
OBJS =  diff.o diffdir.o diffreg.o

diff:   $(OBJS) $(LIB)/libra.a
	cc $(OBJS) $(LIB)/libra.a
	strip a.out
	mv a.out $(BIN)/diff

diffh:  diffh.o $(LIB)/libra.a
	cc diffh.o $(LIB)/libra.a
	strip a.out
	mv a.out $(LIB)/diffh

$(OBJS):  $(HDRS)

#  The source codes of cat.c, chmod.c, mkdir.c, rmdir.c, diff.h, diff.c,
#  diffdir.c, diffreg.c and diffh.c are exactly the same as those in
#  /usr/src/bin or /usr/src/bin/diff.
#
#  The source codes of cp.c, ln.c, ls.c, mv.c, rm.c and diffreg.c have
#  minor changes and are listed in this directory.
#


