/*
 * User-level probe routines to make devices interrupt.
 * One per device; entered through uprobe table.
 * Return values:
 *	ACP_NXDEV	device doesn't exist
 *	ACP_IFINTR	OK if device has interrupted by now
 *	ACP_EXISTS	OK, not checking interrupt
 *
 * NOTES:
 *	Reads and writes to kmem (done by grab, stuff)
 *	are currently done a byte at a time in the kernel.
 *	Beware!
 *
 *	The hs, rp, hk and dvhp probes have not been tested.
 */

#include	<uprobe.h>
#include	<sys/param.h>
#include	<sys/autoconfig.h>
#include	<sgtty.h>
#include	<sys/dhreg.h>
#include	<sys/dnreg.h>
#include	<sys/drreg.h>
#undef	BITS7
#undef	BITS8
#undef	TWOSB
#undef	PENABLE
#undef	OPAR
#include	<sys/dzreg.h>
#include	<sys/klreg.h>
#include	<sys/lpreg.h>
#include	<sys/vpreg.h>
#include	<sys/hkreg.h>
#include	<sys/hpreg.h>
#include	<sys/hsreg.h>
#include	<sys/rkreg.h>
#include	<sys/rlreg.h>
#include	<sys/rpreg.h>

#include	<sys/htreg.h>
#undef	b_repcnt
#include	<sys/tmreg.h>
#undef	b_repcnt
#include	<sys/tsreg.h>

int	xpprobe(), hkprobe(), hsprobe(), rlprobe(), rkprobe(), rpprobe(),
	htprobe(), tmprobe(), tsprobe(), dnprobe(), klprobe(), dzprobe(),
	dhprobe(), dmprobe(), drprobe(), lpprobe(), vpprobe();

#ifdef	VIRUS
int	caryprobe();
#endif

struct uprobe uprobe[] = {
	"xp",	xpprobe,		/* Disks */
	"rm",	xpprobe,
	"hp",	xpprobe,
	"hk",	hkprobe,
	"hs",	hsprobe,
	"rl",	rlprobe,
	"rk",	rkprobe,
	"rp",	rpprobe,
	"ht",	htprobe,		/* Tapes */
	"tm",	tmprobe,
	"ts",	tsprobe,
	"dn",	dnprobe,		/* Communication interfaces */
	"kl",	klprobe,
	"dz",	dzprobe,
	"dh",	dhprobe,
	"dm",	dmprobe,
	"dr",	drprobe,
	"lp",	lpprobe,		/* Printers */
	"vp",	vpprobe,
#ifdef	VIRUS
	"cary",	caryprobe,		/* Don't ask */
#endif
	0,	0
};

xpprobe(addr)
struct hpdevice *addr;
{
	stuff(HP_IE | HP_RDY, &(addr->hpcs1.w));
	DELAY(10);
	stuff(0, &(addr->hpcs1.w));
	return(ACP_IFINTR);
}

hkprobe(addr)
struct hkdevice *addr;
{
	stuff(HK_CDT | HK_IE | HK_CRDY, (&(addr->hkcs1)));
	DELAY(10);
	stuff(HK_CDT, (&(addr->hkcs1)));
	return(ACP_IFINTR);
}

hsprobe(addr)
struct hsdevice *addr;
{
	stuff(HS_IE | HS_DCLR | HS_GO, (&(addr->hscs1)));
	DELAY(10);
	stuff(0, (&(addr->hscs1)));
	return(ACP_IFINTR);
}

rlprobe(addr)
struct rldevice *addr;
{
	stuff(RL_NOP | RL_IE, (&(addr->rlcs)));
	DELAY(10);
	stuff(RL_CRDY, (&(addr->rlcs)));
	return(ACP_IFINTR);
}

rkprobe(addr)
struct rkdevice *addr;
{
	stuff(RKCS_IDE | RKCS_DRESET | RKCS_GO, (&(addr->rkcs)));
	DELAY(10);
	stuff(0, (&(addr->rkcs)));
	return(ACP_IFINTR);
}

rpprobe(addr)
struct rpdevice *addr;
{
	stuff(RP_IDE | RP_IDLE | RP_GO, (&(addr->rpcs.w)));
	DELAY(10);
	stuff(0, (&(addr->rpcs.w)));
	return(ACP_IFINTR);
}

htprobe(addr)
struct htdevice *addr;
{
	stuff(HT_SENSE | HT_IE | HT_GO, (&(addr->htcs1)));
	DELAY(10);
	stuff(0, (&(addr->htcs1)));
	return(ACP_IFINTR);
}

extern int	errno;

/*
 * TM-11 probe routine.
 * Also check one of the more distant registers
 * to make sure this isn't a TS-11.
 */
tmprobe(addr)
struct tmdevice *addr;
{
	stuff(TM_IE, &(addr->tmcs));
	errno = 0;
	grab(&(addr->tmba));
	if (errno != 0)
		return(ACP_NXDEV);
	return(ACP_IFINTR);
}

/*
 * TS-11 probe.
 * Assume that the device exists if there's no TM-11 there.
 */
tsprobe(addr)
struct tsdevice *addr;
{
	errno = 0;
	grab(&((struct tmdevice *)addr->tmba));
	if (errno == 0)
		return(ACP_NXDEV);
	return(ACP_EXISTS);
}

klprobe(addr)
struct dldevice *addr;
{
	int i;

	stuff(grab(&(addr->dlxcsr)) | DLXCSR_TIE, &(addr->dlxcsr));
	for (i = 0; i < 7; i++)
		DELAY(10000);
	/*
	 *  Leave TIE enabled; kl.c never turns it off
	 *  (and this could be the console).
	 */
	return(ACP_IFINTR);
}

dzprobe(addr)
struct dzdevice *addr;
{
	register int i;

	stuff(grab(&(addr->dzcsr)) | DZ_TIE | DZ_MSE, &(addr->dzcsr));
	stuff(1, &(addr->dztcr));
	for (i = 0; i < 7; i++)
		DELAY(10000);
	stuff(DZ_CLR, &(addr->dzcsr));
	return(ACP_IFINTR);
}

dhprobe(addr)
struct dhdevice *addr;
{
	int i;

	stuff(DH_TIE, &(addr->dhcsr));
	DELAY(5);
	stuff((B9600 << 10) | (B9600 << 6) | BITS7|PENABLE, &(addr->dhlpr));
	stuff(-1, &(addr->dhbcr));
	stuff(0, &(addr->dhcar));
	stuff(1, &(addr->dhbar));
	for (i = 0; i < 7; i++)
		DELAY(10000);
	stuff(0, &(addr->dhcsr));
	return(ACP_IFINTR);
}

dmprobe(addr)
struct dmdevice *addr;
{
	stuff(grab(&(addr->dmcsr)) | DM_DONE | DM_IE, &(addr->dmcsr));
	DELAY(20);
	stuff(0, &(addr->dmcsr));
	return(ACP_IFINTR);
}

/*
 *  Try to make the first unit of a DN-11 interrupt.
 */
dnprobe(addr)
struct dndevice *addr;
{
	stuff(DN_MINAB | DN_INTENB | DN_DONE, (&(addr->dnisr[0])));
	DELAY(5);
	stuff(0, (&(addr->dnisr[0])));
	return(ACP_IFINTR);
}

/*
 * dr-11 whatever
 */
drprobe(addr)
struct drdevice *addr;
{
	stuff(DR_MANT, &(addr->csr));		/* toggle maintence bit */
	stuff(0, &(addr->csr));			/* to reset dr11 */
	return(ACP_EXISTS);			/* can't make it interrupt */
}

lpprobe(addr)
struct lpdevice *addr;
{
	stuff(grab(&(addr->lpcs)) | LP_IE, &(addr->lpcs));
	DELAY(10);
	stuff(0, &(addr->lpcs));
	return(ACP_IFINTR);
}

vpprobe(addr)
struct vpdevice *addr;
{
#ifdef	VAX
	/*
	 * This is the way the 4.1 driver does it.
	 */
	errno = 0;
	stuff(VP_IENABLE | VP_DTCINTR, (&(addr->prcsr)));
	stuff(0, (&(addr->pbaddr)));
	stuff(3, (&(addr->pbxaddr)));
	stuff(01777776, (&(addr->prbcr)));
	DELAY(10000);
	stuff(0, (&(addr->prcsr)));
	if (errno)
		return(ACP_NXDEV);	/* Possibly an LP csr, but no print DMA regs */
	else
		return(ACP_IFINTR);
#else
	errno = 0;
	/*
	 *  Use the plot csr now, to distinguish from a line printer.
	 */
	stuff(VP_IENABLE | VP_CLRCOM, (&(addr->plcsr)));
	DELAY(10000);
	/*
	 *  Make sure that the DMA registers are there.
	 */
	grab(&(addr->plbcr));
	/*
	 * Write the print csr this time, to leave it in print mode.
	 */
	stuff(0, (&(addr->prcsr)));
	if (errno)
		return(ACP_NXDEV);	/* Possibly an LP csr, but no plot regs */
	else
		return(ACP_IFINTR);
#endif
}

#ifdef	VIRUS
/*
 * Can't make the cary interrupt unless it's in fixed-wavelength mode.
 */
/*ARGSUSED*/
caryprobe(addr)
{
	return(ACP_EXISTS);
}
#endif
