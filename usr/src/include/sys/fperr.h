/*
 * Structure of the floating point error register save/return
 */

struct fperr
{
	short	f_fec;
	caddr_t	f_fea;
};
