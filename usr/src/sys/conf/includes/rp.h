#define	NRP	%NRP%
/* #define RP_DKN	0	/* drive # for iostat disk monitoring */
#if NRP > 1
#define	UCB_DBUFS		/* parallel xfers for multiple drives */
#endif
