#define	NRK	%NRK%
/* #define	RK_DKN	0	/* drive # for iostat disk monitoring */
#if NRK > 1
#define	UCB_DBUFS		/* parallel xfers for multiple drives */
#endif
