/*	getnetname.c	4.2	82/10/05	*/

#include <sys/types.h>
#include <netdb.h>

struct netent *
getnetbyname(name)
	register char *name;
{
	register struct netent *p;
	register char **cp;

	setnetent(0);
	while (p = getnetent()) {
		if (strcmp(p->ne_name, name) == 0)
			break;
		for (cp = p->ne_aliases; *cp != 0; cp++)
			if (strcmp(*cp, name) == 0)
				goto found;
	}
found:
	endnetent();
	return (p);
}
