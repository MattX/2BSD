#	Makefile	1.2	83/07/02
#

# CFLAGS=-O -Dunix=1 -DFLEXNAMES
# If you don't have split I/D, you'll have to edit the constant
# SBSIZE in cpp.c to reflect this.

CFLAGS=-O -Dpdp11=1 -DFLEXNAMES
LFLAGS=-i

cpp :	cpp.o cpy.o rodata.o
	$(CC) $(CFLAGS) $(LFLAGS) -o cpp rodata.o cpp.o cpy.o

cpy.c : cpy.y
	yacc cpy.y
	../pcc/:yyfix yyexca yyact yypact yypgo yyr1 yyr2 yychk yydef
	mv y.tab.c cpy.c

cpy.o :	cpy.c yylex.c

rodata.o : cpy.c
	$(CC) $(CFLAGS) -R -c rodata.c

clean :
	rm -f *.s *.o cpy.c rodata.c

install :
	install -s -o bin -g bin -m 711 cpp $(DESTDIR)/lib

sources : cpp.c cpy.y yylex.c README
cpp.c cpy.y yylex.c README :
	sccs get $@
