h27890
s 00001/00000/00011
d D 2.2 80/09/02 13:51:29 ucb 2 1
c UCB version, SCCS id keyword added (mss)
e
s 00011/00000/00000
d D 2.1 80/09/02 13:46:52 ucb 1 0
c date and time created 80/09/02 13:46:52 by ucb
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
_concat(out, list)
register	char	*out;
register	char	**list;
{
	register	char	*cp;

	while (cp = *list++)
		while (*cp)
			*out++ = *cp++;
	*out = '\0';
}
E 1
