/* STARTUP PROCEDURE FOR UNIX FORTRAN PROGRAMS */
char id_libF77[] = "@(#)main.c	2.12	5/19/83";

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#ifdef	pdp11
#include <sys/fperr.h>
#endif
#include "../libI77/fiodefs.h"

int xargc;
char **xargv;

main(argc, argv, arge)
int argc;
char **argv;
char **arge;
{
int sigdie();
int (*sigf)();
int signum;

xargc = argc;
xargv = argv;

for (signum=1; signum<=16; signum++)
{
	if (signum == (int) SIGILL || signum == (int) SIGKILL)
		continue;
	if ((sigf = signal(signum, sigdie)) != SIG_DFL && sigf != BADSIG)
		signal(signum, sigf);
}

#ifdef notdef
#ifdef pdp11
	ldfps(FPS_FIV | FPS_FD); /* detect overflow as an exception */
#endif pdp11
#endif notdef

f_init();
MAIN__();
f_exit();
}

struct action {
	char *mesg;
	int   core;
} sig_act[16] = {
	{"Hangup", 0},			/* SIGHUP  */
	{"Interrupt!", 0},		/* SIGINT  */
	{"Quit!", 1},			/* SIGQUIT */
	{"Illegal instruction", 1},	/* SIGILL  */
	{"Trace Trap", 1},		/* SIGTRAP */
	{"IOT Trap", 1},		/* SIGIOT  */
	{"EMT Trap", 1},		/* SIGEMT  */
	{"Floating Point Exception", 1},/* SIGFPE  */
	{ 0, 0},			/* SIGKILL */
	{"Bus error", 1},		/* SIGBUS  */
	{"Segmentation violation", 1},	/* SIGSEGV */
	{"Sys arg", 1},			/* SIGSYS  */
	{"Open pipe", 0},		/* SIGPIPE */
	{"Alarm", 0},			/* SIGALRM */
	{"Terminated", 0},		/* SIGTERM */
	{"Sig 16", 0},			/* unassigned */
};

sigdie(s)
int s;
{
extern unit units[];
register struct action *act = &sig_act[s-1];
/* print error message, then flush buffers */

if (act->mesg)
	fprintf(units[STDERR].ufd, "*** %s\n", act->mesg);
f_exit();
_cleanup();

if(act->core)
	{
	/* now get a core */
#if	vax
	signal(SIGILL, SIG_DFL);
#else	vax
	signal(SIGIOT, SIG_DFL);
#endif	vax
	abort();
	}
exit(s);
}
