static int mdays[12] =
	{31, 0, 31,  30, 31, 30,  31, 31, 30,  31, 30, 31};
#define epoch 1970

#include <time.h>
extern struct tm *localtime();
time_t dateconv(mm, dd, yy, h, m, s, mer, zone, dayflag)
int mm, dd, yy, h, m, s, mer, zone, dayflag;
{
	time_t tod, jdate;
	register int i;
	time_t timeconv();

	if (yy < 100) yy += 1900;
	mdays[1] = 28 + (yy%4 == 0);
	if (yy < epoch || yy > 1999 || mm < 1 || mm > 12 ||
		dd < 1 || dd > mdays[--mm]) return (-1);
	jdate = dd-1;
        for (i=0; i<mm; i++) jdate += mdays[i];
	for (i = epoch; i < yy; i++) jdate += 365 + (i%4 == 0);
	jdate *= daysec;
	jdate += zone * (60L*60L);
	if ((tod = timeconv(h, m, s, mer)) < 0) return (-1);
	jdate += tod;
	if (dayflag==DAYLIGHT || (dayflag==MAYBE&&localtime(&jdate)->tm_isdst))
		jdate += -1*60*60;
	return (jdate);
}

time_t dayconv(ord, day, now) int ord, day; time_t now;
{
	register struct tm *loctime;
	time_t tod;
	time_t daylcorr();

	tod = now;
	loctime = localtime(&tod);
	tod += daysec * ((day - loctime->tm_wday + 7) % 7);
	tod += 7*daysec*(ord<=0?ord:ord-1);
	return daylcorr(tod, now);
}

time_t timeconv(hh, mm, ss, mer) register int hh, mm, ss, mer;
{
	if (mm < 0 || mm > 59 || ss < 0 || ss > 59) return (-1);
	switch (mer) {
		case AM: if (hh < 1 || hh > 12) return(-1);
			 return (60L * ((hh%12)*60L + mm)+ss);
		case PM: if (hh < 1 || hh > 12) return(-1);
			 return (60L * ((hh%12 +12)*60L + mm)+ss);
		case 24: if (hh < 0 || hh > 23) return (-1);
			 return (60L * (hh*60L + mm)+ss);
		default: return (-1);
	}
}
time_t monthadd(sdate, relmonth) time_t sdate, relmonth;
{
	struct tm *ltime;
	time_t dateconv();
	time_t daylcorr();
	int mm, yy;

	if (relmonth == 0) return 0;
	ltime = localtime(&sdate);
	mm = 12*ltime->tm_year + ltime->tm_mon + relmonth;
	yy = mm/12;
	mm = mm%12 + 1;
	return daylcorr(dateconv(mm, ltime->tm_mday, yy, ltime->tm_hour,
		ltime->tm_min, ltime->tm_sec, 24, ourzone, MAYBE), sdate);
}

time_t daylcorr(future, now) time_t future, now;
{
	int fdayl, nowdayl;

	nowdayl = (localtime(&now)->tm_hour+1) % 24;
	fdayl = (localtime(&future)->tm_hour+1) % 24;
	return (future-now) + 60L*60L*(nowdayl-fdayl);
}
