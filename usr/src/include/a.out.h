/*
 * Copyright (c) 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)a.out.h	2.1 (2.11BSD GTE) 1/06/94
 */

#ifndef	_AOUT_H_
#define	_AOUT_H_
/*
 * Definitions of the a.out header
 * and magic numbers are shared with
 * the kernel.
 */
#include <sys/exec.h>

#define	_AOUT_INCLUDE_
#include <nlist.h>

/*
 * Macros which take exec structures as arguments and tell whether
 * the file has a reasonable magic number or offset to text.
 */
#define	N_BADMAG(x) \
	(((x).a_magic)!=A_MAGIC1 && ((x).a_magic)!=A_MAGIC2 && \
	((x).a_magic)!=A_MAGIC3 && ((x).a_magic)!=A_MAGIC4 && \
	((x).a_magic)!=A_MAGIC5 && ((x).a_magic)!=A_MAGIC6)

#define	N_TXTOFF(x) \
	((x).a_magic==A_MAGIC5 || (x).a_magic==A_MAGIC6 ? \
	sizeof(struct ovlhdr) + sizeof(struct exec) : sizeof(struct exec))

/*
 * The following were added as part of the new object file format.  They
 * call functions because calculating the sums of overlay sizes was too
 * messy (and verbose) to do 'inline'.
 *
 * NOTE: if the magic number is that of an overlaid object the program
 * must pass an extended header ('xexec') as the argument.
*/

#include <sys/types.h>

off_t	n_stroff(), n_symoff(), n_datoff(), n_dreloc(), n_treloc();

#define	N_STROFF(e) (n_stroff(&e))
#define	N_SYMOFF(e) (n_symoff(&e))
#define	N_DATOFF(e) (n_datoff(&e))
#define	N_DRELOC(e) (n_dreloc(&e))
#define	N_TRELOC(e) (n_treloc(&e))

#endif	/* !_AOUT_H_ */
