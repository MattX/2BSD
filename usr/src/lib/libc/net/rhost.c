static char sccsid[] = "%W% %E%";

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>

char	*index(), *rindex(), *malloc();
long    rnumber();

long
rhost(ahost)
	char **ahost;
{
	FILE *hf;
	char hbuf[BUFSIZ], *cp;
	int first = 1;
	long addr;

	if (isdigit(**ahost) && (addr = rnumber(*ahost)) != -1)
		return (addr);
	hf = fopen("/etc/hosts", "r");
	if (hf == NULL) {
		perror("/etc/hosts");
		exit(1);
	}
top:
	while (fgets(hbuf, sizeof (hbuf), hf)) {
		*index(hbuf, '\n') = 0;
		if (hbuf[0] == '#')
			continue;
		for (;;) {
			cp = rindex(hbuf, ' ');
			if (cp == NULL)
				break;
			if (!strcmp(cp+1, *ahost)) {
			    if ((addr = rnumber(hbuf)) == -1)
				goto bad;
			    fclose(hf);
			    *ahost = hbuf;
			    do {
				if (!(*ahost = index(*ahost, ' ')))
				    return(addr);
				while (**ahost == ' ') (*ahost)++;
			    } while (isdigit(**ahost));
			    cp = index(*ahost, ' ');
			    if (cp)
				*cp = 0;
			    cp = malloc(strlen(*ahost)+1);
			    strcpy(cp, *ahost);
			    *ahost = cp;
			    return (addr);
			}
			*cp = 0;
		}
	}
	if (first == 1) {
		first = 0;
		fclose(hf);
		if (hf = fopen("/etc/hosts.local", "r"))
			goto top;
		return (-1);
	}
bad:
	fclose(hf);
	return (-1);
}

long
rnumber(cp)
	register char *cp;
{
	register long val;
	register int base;
	register char c;
	long parts = 0;
	char *pplow = (char *)&parts;
	char *pplim = pplow+4;
	char *pp = pplow;
	long net, imp, hoi;

	if (index(cp, '/') == 0)
		goto again;
	hoi = atoi(cp);
	if (index(cp, ',')) {
		imp = atoi(index(cp, '/') + 1);
		net = atoi(index(cp, ',') + 1);
		hoi = ntohs((short)hoi);
		val = (net<<24)|(hoi<<8)|imp;
	} else {
		net = atoi(index(cp, '/') + 1);
		val = (net<<24)|hoi;
	}
	val = htonl(val);
	return (val);

again:
	val = 0; base = 10;
	if (*cp == '0')
		base = 8, cp++;
	if (*cp == 'x' || *cp == 'X')
		base = 16, cp++;
	while (c = *cp) {
		if (isdigit(c)) {
			val = (val * base) + (c - '0');
			cp++;
			continue;
		}
		if (base == 16 && isxdigit(c)) {
			val = (val << 4) + (c + 10 - (islower(c) ? 'a' : 'A'));
			cp++;
			continue;
		}
		break;
	}
	if (*cp == '.') {
		/*
		 * Internet format:
		 *	net.host.lh.imp
		 */
		if (pp >= pplim)
			return (-1);
		*pp++ = val, cp++;
		goto again;
	}
	if (*cp) {
		if (*cp == 'n') return (htonl(val));
		if ( *cp != ' ') return (-1);
	}
	if (pp >= pplim)
		return (-1);
	*pp++ = val;
	return (parts);
}
