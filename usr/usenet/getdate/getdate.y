%token ID MONTH DAY MERIDIAN NUMBER UNIT MUNIT SUNIT ZONE DAYZONE
%{
#include "datedcl.h"
%}

%%
timedate: 		/* empty */
	| timedate item;

item:	tspec =
		{timeflag++;}
	| zone =
		{zoneflag++;}
	| dtspec =
		{dateflag++;}
	| dyspec =
		{dayflag++;}
	| rspec =
		{relflag++;};

tspec:	NUMBER MERIDIAN =
		{hh = $1; mm = 0; ss = 0; merid = $2;}
	| NUMBER ':' NUMBER =
		{hh = $1; mm = $3; merid = 24;}
	| NUMBER ':' NUMBER MERIDIAN =
		{hh = $1; mm = $3; merid = $4;}
	| NUMBER ':' NUMBER ':' NUMBER =
		{hh = $1; mm = $3; ss = $5; merid = 24;}
	| NUMBER ':' NUMBER ':' NUMBER MERIDIAN =
		{hh = $1; mm = $3; ss = $5; merid = $6;};

zone:	ZONE =
		{ourzone = $1; daylight = STANDARD;}
	| DAYZONE =
		{ourzone = $1; daylight = DAYLIGHT;};

dyspec:	DAY =
		{dayord = 1; dayreq = $1;}
	| NUMBER DAY =
		{dayord = $1; dayreq = $2;};

dtspec:	NUMBER '/' NUMBER =
		{month = $1; day = $3;}
	| NUMBER '/' NUMBER '/' NUMBER =
		{month = $1; day = $3; year = $5;}
	| MONTH NUMBER =
		{month = $1; day = $2;}
	| MONTH NUMBER ',' NUMBER =
		{month = $1; day = $2; year = $4;};


rspec:	NUMBER UNIT =
		{relsec +=  60L * $1 * $2;}
	| NUMBER MUNIT =
		{relmonth += $1 * $2;}
	| NUMBER SUNIT =
		{relsec += $1;}
	| UNIT =
		{relsec +=  60L * $1;}
	| MUNIT =
		{relmonth += $1;}
	| SUNIT =
		{relsec++;};
%%

#include "getdate.h"
