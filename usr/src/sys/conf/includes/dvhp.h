#define NDVHP	%NDVHP%
#define DVHP_DKN 0	/* drive # for iostat disk monitoring */
#if NDVHP > 1
#define	UCB_DBUFS		/* parallel xfers for multiple drives */
#endif
