/*      
 * Copyright (c) 2020 Anders Magnusson. All rights reserved.
 *      
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:     
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *                      
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if     defined(DOSCCS) && !defined(lint)
static char *sccsid = "@(#)acvt.c 1.0 (2.11BSD) 2025/12/25";
#endif

#include <stdio.h>

#include "local.h"

union U {
	double d;
	short s[4];
};

extern char _hextab[];

/*
 * convert floating point number to hex format.
 * buffer is supposed to be big enough.
 * A dfloat has 13 + 1 significant hex digits (56 bits).
 * if nd < 0 then all significant digits are written.
 */
char *
__acvt(double d, int nd, char *buf, int *sign)
{
	union U U;
	register short s, i;
	register char *b = buf;
	int e = 0;

	U.d = d;
	s = U.s[0];

	*sign = s & 0x8000;
	*b++ = '0';
	*b++ = 'X';
	*b++ = _hextab[((s >> 4) & 7) | 8];
	*b++ = '.';
	*b++ = _hextab[s & 15];
	for (i = 1; i < 4; i++)
		for (s = 12; s >= 0; s -= 4)
			*b++ = _hextab[(U.s[i] >> s) & 15];
	if (nd > 13) {
		while (nd-- > 13)
			*b++ = '0';
	} else if (nd >= 0) {
		/* XXX rounding */
		b = b - 13 + nd;
	} else /* if (nd < 0) */ { /* significant digits */
		*b = 0;
		while (*--b == '0')
			;
		if (*b == '.')
			b--;
		b++;
	}
	*b++ = 'P';
	/* bias 0200, # of bits left of . is four */
	s = ((U.s[0] >> 7) & 0377) - 0200 - 4;
	if (s < 0) {
		*b++ = '-';
		s = -s;
	} else
		*b++ = '+';
	if (s >= 100)
		*b++ = '1';
	s = s % 100;
	if (s >= 10)
		*b++ = _hextab[s / 10];
	s = s % 10;
	*b++ = _hextab[s];
	*b = 0;
	return buf;
}
