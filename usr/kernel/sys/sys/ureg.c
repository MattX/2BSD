#include <sys/param.h>
#include <sys/systm.h>
#include <sys/dir.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/text.h>
#include <sys/seg.h>

#ifdef	UCB_SCCSID
static	char sccs_id[] = "@(#)ureg.c	3.1";
#endif


/*
 * Load the user hardware segmentation
 * registers from the software prototype.
 * The software registers must have
 * been setup prior by estabur.
 */
sureg()
{
	register *udp, *uap, *rdp;
	int *rap, *limudp;
	int taddr, daddr;
	struct text *tp;

	taddr = daddr = u.u_procp->p_addr;
	if ((tp=u.u_procp->p_textp) != NULL)
		taddr = tp->x_caddr;
#ifndef NONSEPARATE
	limudp = &u.u_uisd[16];
	if (cputype==40)
#endif
		limudp = &u.u_uisd[8];
	rap = (int *)UISA;
	rdp = (int *)UISD;
	uap = &u.u_uisa[0];
	for (udp = &u.u_uisd[0]; udp < limudp;) {
		*rap++ = *uap++ + (*udp&TX? taddr: (*udp&ABS? 0: daddr));
		*rdp++ = *udp++;
	}
}

/*
 * Set up software prototype segmentation
 * registers to implement the 3 pseudo
 * text,data,stack segment sizes passed
 * as arguments.
 * The argument sep specifies if the
 * text and data+stack segments are to
 * be separated.
 * The last argument determines whether the text
 * segment is read-write or read-only.
 */
estabur(nt, nd, ns, sep, xrw)
unsigned nt, nd, ns;
{
	register a, *ap, *dp;
#ifdef MENLO_OVLY
	register unsigned ts;
#endif

#ifdef MENLO_OVLY
	if (u.u_ovdata.uo_ovbase && nt)
		ts = u.u_ovdata.uo_dbase-1;
	else
		ts = nt;
#endif
	if(sep) {
#ifndef NONSEPARATE
		if(cputype == 40)
			goto err;
#ifdef MENLO_OVLY
		if(ctos(ts) > 8 || ctos(nd)+ctos(ns) > 8)
#else
		if(ctos(nt) > 8 || ctos(nd)+ctos(ns) > 8)
#endif
#endif
			goto err;
	} else
#ifdef MENLO_OVLY
		if(ctos(ts)+ctos(nd)+ctos(ns) > 8)
#else
		if(ctos(nt)+ctos(nd)+ctos(ns) > 8)
#endif
			goto err;
#ifdef MENLO_OVLY
	if (u.u_ovdata.uo_ovbase && nt)
		ts = u.u_ovdata.uo_ov_offst[7];
	if(ts+nd+ns+USIZE > maxmem)
#else
	if(nt+nd+ns+USIZE > maxmem)
#endif
		goto err;
	a = 0;
	ap = &u.u_uisa[0];
	dp = &u.u_uisd[0];
	while(nt >= 128) {
		*dp++ = (127<<8) | xrw|TX;
		*ap++ = a;
		a += 128;
		nt -= 128;
	}
	if(nt) {
		*dp++ = ((nt-1)<<8) | xrw|TX;
		*ap++ = a;
	}
#ifdef MENLO_OVLY
	if (u.u_ovdata.uo_ovbase && ts) {  /* overlay process, adjust accdgly.*/
		register novlseg = 0;
		if (u.u_ovdata.uo_curov != 0) { /* map in current overlay */
			a = u.u_ovdata.uo_ov_offst[u.u_ovdata.uo_curov-1];
			nt = u.u_ovdata.uo_ov_offst[u.u_ovdata.uo_curov] - a;
			while (nt >= 128) {
				*dp++ = (127<<8) | xrw|TX;
				*ap++ = a;
				a += 128;
				nt -= 128;
				novlseg++;
			}
			if (nt) {
				*dp++ = ((nt-1)<<8) | xrw|TX;
				*ap++ = a;
				novlseg++;
			}
		}
#ifndef NONSEPARATE
		if(!sep)
#endif
		for(;novlseg<u.u_ovdata.uo_nseg;novlseg++)
		{
			*ap++ = 0;
			*dp++ = 0;
		}
	}
#endif
#ifndef NONSEPARATE
	if(sep)
	while(ap < &u.u_uisa[8]) {
		*ap++ = 0;
		*dp++ = 0;
	}
#endif

	a = USIZE;
	while(nd >= 128) {
		*dp++ = (127<<8) | RW;
		*ap++ = a;
		a += 128;
		nd -= 128;
	}
	if(nd) {
		*dp++ = ((nd-1)<<8) | RW;
		*ap++ = a;
		a += nd;
	}
	while(ap < &u.u_uisa[8]) {
		if(*dp &ABS) {
			dp++;
			ap++;
			continue;
		}
		*dp++ = 0;
		*ap++ = 0;
	}
#ifndef NONSEPARATE
	if(sep)
	while(ap < &u.u_uisa[16]) {
		if(*dp & ABS) {
			dp++;
			ap++;
			continue;
		}
		*dp++ = 0;
		*ap++ = 0;
	}
#endif
	a += ns;
	while(ns >= 128) {
		a -= 128;
		ns -= 128;
		*--dp = (127<<8) | RW;
		*--ap = a;
	}
	if(ns) {
		*--dp = ((128-ns)<<8) | RW | ED;
		*--ap = a-128;
	}
#ifndef NONSEPARATE
	if(!sep) {
#endif
		ap = &u.u_uisa[0];
		dp = &u.u_uisa[8];
		while(ap < &u.u_uisa[8])
			*dp++ = *ap++;
		ap = &u.u_uisd[0];
		dp = &u.u_uisd[8];
		while(ap < &u.u_uisd[8])
			*dp++ = *ap++;
#ifndef NONSEPARATE
	}
#endif
	sureg();
	return(0);

err:
	u.u_error = ENOMEM;
	return(-1);
}
