h06959
s 00001/00000/00056
d D 2.2 80/09/02 13:51:49 ucb 2 1
c UCB version, SCCS id keyword added (mss)
e
s 00056/00000/00000
d D 2.1 80/09/02 13:47:02 ucb 1 0
c date and time created 80/09/02 13:47:02 by ucb
e
u
U
f b 
f n 
t
T
I 2
/*	%W%	SCCS id keyword	*/
E 2
I 1
#define NULL 0
char	*getenv();
static	char	*searchpath;

resetfp()
{
	static	char	*path = NULL;
	if (path == NULL) {
		path = getenv("UCBPATH");
		if (path == NULL) {
			path = getenv("PATH");
			if (path == NULL)
				path = "/bin:/usr/bin";
		}
	}
	searchpath = path;
}

char *makefp(bestsofar, buffer, list)
char	*bestsofar;
char	*buffer;
char	**list;
{
	char	*retval;
	register	char	*cp1, *cp2;

	cp2 = searchpath;

	do {
		retval = cp2;
		if (cp2 == NULL) {
			if (bestsofar)
				cp2 = bestsofar;
			else 
				cp2 = "...bin";
		}

		cp1 = buffer;
		while (*cp2 && *cp2 != ':')
			*cp1++ = *cp2++;
		if (*cp2)
			cp2++;
		else
			cp2 = NULL;

	} while (
		(cp1 - buffer < 3  || cp1[-3] != 'b' ||
			cp1[-2] != 'i' || cp1[-1] != 'n')
	);
	cp1 -= 3;

	searchpath = cp2;
	
	_concat(cp1, list);
	return(retval);
}
E 1
