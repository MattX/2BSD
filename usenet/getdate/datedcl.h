	/* 	Steven M. Bellovin (unc!smb)			*/
	/*	Dept. of Computer Science			*/
	/*	University of North Carolina at Chapel Hill	*/

#include <sys/types.h>
#include <sys/timeb.h>
#define	NULL	0
#define daysec (24L*60L*60L)
	static int timeflag, zoneflag, dateflag, dayflag, relflag;
	static time_t relsec, relmonth;
	static int hh, mm, ss, merid, daylight;
	static int dayord, dayreq;
	static int month, day, year;
	static int ourzone;
#define AM 1
#define PM 2
#define DAYLIGHT 1
#define STANDARD 2
#define MAYBE    3
