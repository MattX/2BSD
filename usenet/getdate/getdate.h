#include "dateconv.h"
#include "datelex.h"

time_t getdate(p, now) char *p; struct timeb *now;
{
#define mcheck(f)	if (f>1) err++
	time_t monthadd();
	int err;
	struct tm *lt;
	struct timeb ftz;

	time_t sdate, tod;

	lptr = p;
	if (now == ((struct timeb *) NULL)) {
		now = &ftz;
		ftime(&ftz);
	}
	lt = localtime(&now->time);
	year = lt->tm_year;
	month = lt->tm_mon+1;
	day = lt->tm_mday;
	relsec = 0; relmonth = 0;
	timeflag=zoneflag=dateflag=dayflag=relflag=0;
	ourzone = now->timezone / 60;
	daylight = MAYBE;
	hh = mm = ss = 0;
	merid = 24;

	if (err = yyparse()) return (-1);

	mcheck(timeflag);
	mcheck(zoneflag);
	mcheck(dateflag);
	mcheck(dayflag);

	if (err) return (-1);

	if (dateflag || timeflag || dayflag) {
		sdate = dateconv(month,day,year,hh,mm,ss,merid,ourzone,daylight);
		if (sdate < 0) return -1;
	}
	else {
		sdate = now->time;
		if (relflag == 0)
			sdate -= (lt->tm_sec + lt->tm_min*60 +
				lt->tm_hour*(60L*60L));
	}

	sdate += relsec;
	sdate += monthadd(sdate, relmonth);

	if (dayflag) {
		tod = dayconv(dayord, dayreq, sdate);
		sdate += tod;
	}
	
	return sdate;
}

yyerror(s) char *s;
{}
