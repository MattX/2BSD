#
/*
 */

/*
 *	Memory special file
 *	minor device 0 is physical memory
 *	minor device 1 is kernel memory
 *	minor device 2 is EOF/RATHOLE
 */

#include <sys/param.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/conf.h>
#include <sys/seg.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "%W%";
#endif


mmread(dev)
{
	register c, bn, on;
	int a, d;

	if(minor(dev) == 2)
		return;
	on = u.u_count;
	if (minor(dev)==1 && u.u_segflg==0 &&
		((u.u_offset | u.u_base | on) & 01) == 0) {
		c = copyout((caddr_t)u.u_offset, u.u_base, on);
		if (c) {
			u.u_error = EFAULT;
			return;
		}
		u.u_base += on;
		u.u_offset += on;
		u.u_count -= on;
		return;
	}
	do {
		bn = u.u_offset >> 6;
		on = u.u_offset & 077;
		a = UISA->r[0];
		d = UISD->r[0];
		spl7();
		UISA->r[0] = bn;
		UISD->r[0] = 077406;
		if(minor(dev) == 1)
			UISA->r[0] = (ka6-6)->r[(bn>>7)&07] + (bn & 0177);
		if ((c = fuibyte((caddr_t)on)) < 0)
			u.u_error = ENXIO;
		UISA->r[0] = a;
		UISD->r[0] = d;
		spl0();
	} while(u.u_error==0 && passc(c)>=0);
}

mmwrite(dev)
{
	register c, bn, on;
	int a, d;

	if(minor(dev) == 2) {
		u.u_count = 0;
		return;
	}
	for(;;) {
		bn = u.u_offset >> 6;
		on = u.u_offset & 077;
		if ((c=cpass())<0 || u.u_error!=0)
			break;
		a = UISA->r[0];
		d = UISD->r[0];
		spl7();
		UISA->r[0] = bn;
		UISD->r[0] = 077406;
		if(minor(dev) == 1)
			UISA->r[0] = (ka6-6)->r[(bn>>7)&07] + (bn & 0177);
		if (suibyte((caddr_t)on, c) < 0)
			u.u_error = ENXIO;
		UISA->r[0] = a;
		UISD->r[0] = d;
		spl0();
	}
}
#ifdef MENLO_ECC
/*
 * Internal versions of mmread(),mmwrite().
 * (used by hpecc() ) -wfj 12/80
 */

char
getmemc(addr)
long addr;
{
	unsigned int bn, on;
	int a, c, d;

	bn = addr >> 6;
	on = addr & 077;
	a = UISA->r[0];
	d = UISD->r[0];
	spl7();
	UISA->r[0] = bn;
	UISD->r[0] = 077406;
	c = fuibyte((caddr_t)on);
	UISA->r[0] = a;
	UISD->r[0] = d;
	spl0();
	return(c);
}

putmemc(addr,contents)
long addr;
char contents;
{
	unsigned int bn, on;
	int a, d;

	bn = addr >> 6;
	on = addr & 077;
	a = UISA->r[0];
	d = UISD->r[0];
	spl7();
	UISA->r[0] = bn;
	UISD->r[0] = 077406;
	suibyte((caddr_t)on, contents);
	UISA->r[0] = a;
	UISD->r[0] = d;
	spl0();
}
#endif
