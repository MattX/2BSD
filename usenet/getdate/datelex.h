#include <ctype.h>
static char *lptr;

yylex()
{
	extern int yylval;
	int sign;
	register char c;
	register char *p;
	char idbuf[20];

	while (isspace(*lptr)) lptr++;

	if (isdigit(c = *lptr) || c == '-' || c == '+') {
		if (c== '-' || c == '+') {
			if (c=='-') sign = -1;
			else sign = 1;
			if (!isdigit(*++lptr)) {
				yylval = sign;
				return (NUMBER);
			}
		} else sign = 1;
		yylval = 0;
		while (isdigit(c = *lptr++)) yylval = 10*yylval + c - '0';
		yylval *= sign;
		lptr--;
		return (NUMBER);

	} else if (isalpha(c)) {
		p = idbuf;
		while (isalpha(c = *lptr++) || c=='.')
			*p++ = c;
		*p = '\0';
		lptr--;
		return (lookup(idbuf));
	}

	else return (*lptr++);
}

struct table {
	char *name;
	int type, value;
};

struct table mdtab[] = {
	{"January", MONTH, 1},
	{"February", MONTH, 2},
	{"March", MONTH, 3},
	{"April", MONTH, 4},
	{"May", MONTH, 5},
	{"June", MONTH, 6},
	{"July", MONTH, 7},
	{"August", MONTH, 8},
	{"September", MONTH, 9},
	{"Sept", MONTH, 9},
	{"October", MONTH, 10},
	{"November", MONTH, 11},
	{"December", MONTH, 12},

	{"Sunday", DAY, 0},
	{"Monday", DAY, 1},
	{"Tuesday", DAY, 2},
	{"Tues", DAY, 2},
	{"Wednesday", DAY, 3},
	{"Wednes", DAY, 3},
	{"Thursday", DAY, 4},
	{"Thur", DAY, 4},
	{"Thurs", DAY, 4},
	{"Friday", DAY, 5},
	{"Saturday", DAY, 6},
	{0, 0, 0}};

struct table mztab[] = {
	{"a.m.", MERIDIAN, AM},
	{"am", MERIDIAN, AM},
	{"p.m.", MERIDIAN, PM},
	{"pm", MERIDIAN, PM},
	{"ast", ZONE, 4},		/* Atlantic */
	{"a.s.t.", ZONE, 4},
	{"adt", DAYZONE, 4},
	{"a.d.t.", DAYZONE, 4},
	{"est", ZONE, 5},		/* Eastern */
	{"e.s.t.", ZONE, 5},
	{"edt", DAYZONE, 5},
	{"e.d.t.", DAYZONE, 5},
	{"cst", ZONE, 6},		/* Central */
	{"c.s.t.", ZONE, 6},
	{"cdt", DAYZONE, 6},
	{"c.d.t.", DAYZONE, 6},
	{"mst", ZONE, 7},		/* Mountain */
	{"m.s.t.", ZONE, 7},
	{"mdt", DAYZONE, 7},
	{"m.d.t.", DAYZONE, 7},
	{"pst", ZONE, 8},		/* Pacific */
	{"p.s.t.", ZONE, 8},
	{"pdt", DAYZONE, 8},
	{"p.d.t.", DAYZONE, 8},
	{"gmt", ZONE, 0},
	{"g.m.t.", ZONE, 0},
	{0, 0, 0}};

struct table unittb[] = {
	{"year", MUNIT, 12},
	{"month", MUNIT, 1},
	{"fortnight", UNIT, 14*24*60},
	{"week", UNIT, 7*24*60},
	{"day", UNIT, 1*24*60},
	{"hour", UNIT, 60},
	{"minute", UNIT, 1},
	{"min", UNIT, 1},
	{"second", SUNIT, 1},
	{"sec", SUNIT, 1},
	{0, 0, 0}};

struct table othertb[] = {
	{"tomorrow", UNIT, 1*24*60},
	{"yesterday", UNIT, -1*24*60},
	{"today", UNIT, 0},
	{"now", UNIT, 0},
	{"last", NUMBER, -1},
	{"next", NUMBER, 2},
	{"first", NUMBER, 1},
	/* {"second", NUMBER, 2}, */
	{"third", NUMBER, 3},
	{"fourth", NUMBER, 4},
	{"fifth", NUMBER, 5},
	{"sixth", NUMBER, 6},
	{"seventh", NUMBER, 7},
	{"eigth", NUMBER, 8},
	{"ninth", NUMBER, 9},
	{"tenth", NUMBER, 10},
	{"eleventh", NUMBER, 11},
	{"twelfth", NUMBER, 12},
	{0, 0, 0}};

lookup(id) char *id;
{
#define gotit (yylval=i->value,  i->type)
#define getid for(j=idvar, k=id; *j++ = *k++; )

	char idvar[20];
	register char *j, *k;
	register struct table *i;
	int abbrev;

	getid;
	if (strlen(idvar) == 3) abbrev = 1;
	else if (strlen(idvar) == 4 && idvar[3] == '.') {
		abbrev = 1;
		idvar[3] = '\0';
	}
	else abbrev = 0;

	if (islower(*idvar)) *idvar = toupper(*idvar);

	for (i = mdtab; i->name; i++) {
		k = idvar;
		for (j = i->name; *j++ == *k++;) {
			if (abbrev && j==i->name+3) return gotit;
			if (j[-1] == 0) return gotit;
		}
	}

	getid;
	for (i = mztab; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	for (j = idvar; *j; j++) if (isupper(*j)) *j = tolower(*j);
	for (i=mztab; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	getid;
	for (i=unittb; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	if (idvar[strlen(idvar)-1] == 's') idvar[strlen(idvar)-1] = '\0';
	for (i=unittb; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	getid;
	for (i = othertb; i->name; i++)
		if (strcmp(i->name, idvar) == 0) return gotit;

	return(ID);
}
