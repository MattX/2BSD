static	char	*rcsid =
	"$Header: rules.c,v 1.8 83/11/28 09:51:06 tony Exp $";

/*
 * $Log:	rules.c,v $
 * Revision 1.8  83/11/28  09:51:06  tony
 * Added LOADLIBES to the .c single suffix rule.
 * 
 * Revision 1.7  83/11/01  20:51:02  tony
 * Added crl`s latest changes (11/1/83)
 * 
 * Revision 1.6  83/10/24  14:37:46  tony
 * Added default rules for ".CO" and ".CLEANUP"
 * 
 * Revision 1.5  83/10/11  12:54:47  tony
 * Added default values for RM and RMFLAGS which are used when
 * removing files that were checked out previously.
 * 
 * Revision 1.4  83/10/06  16:54:38  tony
 * fixed minor bug in the format of a rule.
 * 
 * Revision 1.3  83/10/06  16:52:29  tony
 * Added rules from the 4.2 make for fortran, pascal, etc.
 * 
 * Revision 1.2  83/10/06  10:52:54  tony
 * Added code from physics for RCS.
 * 
 */

/*	@(#)rules.c	3.1	*/
#include "defs.h"
/* DEFAULT RULES FOR UNIX */

/*
 *	These are the internal rules that "make" trucks around with it at
 *	all times. One could completely delete this entire list and just
 *	conventionally define a global "include" makefile which had these
 *	rules in it. That would make the rules dynamically changeable
 *	without recompiling make. This file may be modified to local
 *	needs. There are currently two versions of this file with the
 *	source; namely, rules.c (which is the version running in Columbus)
 *	and pwbrules.c which is my attempt at satisfying the requirements
 *	of PWB systems.
 *	The makefile for make (make.mk) is parameterized for a different
 *	rules file. The macro $(RULES) defined in "make.mk" can be set
 *	to another file and when "make" is "made" the procedure will
 *	use the new file. The recommended way to do this is on the
 *	command line as follows:
 *		"make -f make.mk RULES=pwbrules"
 */

CHARSTAR builtin[] =
	{
	".SUFFIXES: .out .o .c .c~ .F .f .e .r .y .y~ .yr .ye .l .l~ .s .s~ .cl .p .sh .sh~ .h .h~",

/* PRESET VARIABLES */
	"MAKE=make",
	"YACC=yacc",
	"YACCR=yacc -r",
	"YACCE=yacc -e",
	"YFLAGS=",
	"LEX=lex",
	"LFLAGS=",
	"LD=ld",
	"LDFLAGS=",
	"CC=cc",
	"CFLAGS=-O",
	"AS=as",
	"ASFLAGS=",
	"PC=pc",
	"PFLAGS=",
	"RC=f77",
	"RFLAGS=",
	"FC=f77",
	"EFLAGS=",
	"FFLAGS=",
	"LOADLIBES=",
	"GET=get",
	"GFLAGS=",

	"CO=co",
	"COFLAGS=-q",
	"RM=rm",
	"RMFLAGS=-f",

	".CO :",			/* rule to check out a file */
		"\t$(CO) $(COFLAGS) $@",
	".CLEANUP :",			/* rule to remove co`ed files */
		"\t$(RM) $(RMFLAGS) $?",

/* SINGLE SUFFIX RULES */

	".c:",
		"\t$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ $(LOADLIBES)",
	".c~:",
		"\t$(GET) $(GFLAGS) -p $< > $*.c",
		"\t$(CC) -n -O $*.c -o $* $(LOADLIBES)",
		"\t-rm -f $*.c",
	".sh:",
		"\tcp $< $@",
	".sh~:",
		"\t$(GET) $(GFLAGS) -p $< > $*.sh",
		"\tcp $*.sh $*",
		"\t-rm -f $*.sh",

	".s .o:",
		"\t$(CC) $(CFLAGS) $< $(LOADLIBES) -o $@",
	".f .F .r .e:",
		"\t$(FC) $(EFLAGS) $(RFLAGS) $(FFLAGS) $< $(LOADLIBES) -o $@",
	".y:",
		"\t$(YACC) $(YFLAGS) $<",
		"\t$(CC) $(CFLAGS) y.tab.c $(LOADLIBES) -ly -o $@",
		"\trm y.tab.c",
	".l:",
		"\t$(LEX) $(LFLAGS) $<",
		"\t$(CC) $(CFLAGS) lex.yy.c $(LOADLIBES) -ll -o $@",
		"\trm lex.yy.c",

/* DOUBLE SUFFIX RULES */
	".c.o:",
		"\t$(CC) $(CFLAGS) -c $<",
	".c~.o:",
		"\t$(GET) $(GFLAGS) -p $< > $*.c",
		"\t$(CC) $(CFLAGS) -c $*.c",
		"\t-rm -f $*.c",
	".c~.c:",
		"\t$(GET) $(GFLAGS) -p $< > $*.c",
	".p.o:",
		"\t$(PC) $(PFLAGS) -c $<",
	".cl.o:",
		"\tclass -c $<",
	".e.o .r.o .F.o .f.o :",
		"\t$(FC) $(RFLAGS) $(EFLAGS) $(FFLAGS) -c $<",
	".s.o:",
		"\t$(AS) $(ASFLAGS) -o $@ $<",
	".s~.o:",
		"\t$(GET) $(GFLAGS) -p $< > $*.s",
		"\t$(AS) $(ASFLAGS) -o $*.o $*.s",
		"\t-rm -f $*.s",
	".y.o:",
		"\t$(YACC) $(YFLAGS) $<",
		"\t$(CC) $(CFLAGS) -c y.tab.c",
		"\trm y.tab.c",
		"\tmv y.tab.o $@",
	".yr.o:",
		"\t$(YACCR) $(YFLAGS) $<",
		"\t$(RC) $(RFLAGS) -c y.tab.r",
		"\trm y.tab.r",
		"\tmv y.tab.o $@",
	".ye.o:",
		"\t$(YACCE) $(YFLAGS) $<",
		"\t$(RC) $(RFLAGS) -c y.tab.e",
		"\trm y.tab.e",
		"\tmv y.tab.o $@",
	".y~.o:",
		"\t$(GET) $(GFLAGS) -p $< > $*.y",
		"\t$(YACC) $(YFLAGS) $*.y",
		"\t$(CC) $(CFLAGS) -c y.tab.c",
		"\trm -f y.tab.c $*.y",
		"\tmv y.tab.o $*.o",
	".l.o:",
		"\t$(LEX) $(LFLAGS) $<",
		"\t$(CC) $(CFLAGS) -c lex.yy.c",
		"\trm lex.yy.c",
		"\tmv lex.yy.o $@",
	".l~.o:",
		"\t$(GET) $(GFLAGS) -p $< > $*.l",
		"\t$(LEX) $(LFLAGS) $*.l",
		"\t$(CC) $(CFLAGS) -c lex.yy.c",
		"\trm -f lex.yy.c $*.l",
		"\tmv lex.yy.o $*.o",
	".y.c :",
		"\t$(YACC) $(YFLAGS) $<",
		"\tmv y.tab.c $@",
	".y~.c :",
		"\t$(GET) $(GFLAGS) -p $< > $*.y",
		"\t$(YACC) $(YFLAGS) $*.y",
		"\tmv y.tab.c $*.c",
		"\t-rm -f $*.y",
	".l.c :",
		"\t$(LEX) $<",
		"\tmv lex.yy.c $@",
	".yr.r:",
		"\t$(YACCR) $(YFLAGS) $<",
		"\tmv y.tab.r $@",
	".ye.e:",
		"\t$(YACCE) $(YFLAGS) $<",
		"\tmv y.tab.e $@",
	".c.a:",
		"\t$(CC) -c $(CFLAGS) $<",
		"\tar rv $@ $*.o",
		"\trm -f $*.o",
	".c~.a:",
		"\t$(GET) $(GFLAGS) -p $< > $*.c",
		"\t$(CC) -c $(CFLAGS) $*.c",
		"\tar rv $@ $*.o",
		"\trm -f $*.[co]",
	".s~.a:",
		"\t$(GET) $(GFLAGS) -p $< > $*.s",
		"\t$(AS) $(ASFLAGS) -o $*.o $*.s",
		"\tar rv $@ $*.o",
		"\t-rm -f $*.[so]",

	/* Berkeley`s answer to single suffix rules.... */

	".s.out .c.out .o.out:",
		"\t$(CC) $(CFLAGS) $< $(LOADLIBES) -o $@",
	".f.out .F.out .r.out .e.out:",
		"\t$(FC) $(EFLAGS) $(RFLAGS) $(FFLAGS) $< $(LOADLIBES) -o $@",
	".y.out:",
		"\t$(YACC) $(YFLAGS) $<",
		"\t$(CC) $(CFLAGS) y.tab.c $(LOADLIBES) -ly -o $@",
		"\trm y.tab.c",
	".l.out:",
		"\t$(LEX) $(LFLAGS) $<",
		"\t$(CC) $(CFLAGS) lex.yy.c $(LOADLIBES) -ll -o $@",
		"\trm lex.yy.c",

	".h~.h:",
		"\t$(GET) $(GFLAGS) -p $< > $*.h",
	"markfile.o:	markfile",
		"\tA=@;echo \"static char _sccsid[] = \\042`grep $$A'(#)' markfile`\\042;\" > markfile.c",
		"\tcc -c markfile.c",
		"\trm -f markfile.c",
	0 };

