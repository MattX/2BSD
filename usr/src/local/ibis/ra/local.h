

/*******************************************************************
 *
 *  Remote Access Package,  Configuration Dependent Definitions
 *
 *******************************************************************
 *
 *  Copyright (c) 1984 by   Walter F. Tichy and Zuwang Ruan
 *                          Department of Computer Sciences
 *                          Purdue University
 *                          West Lafayette, IN 47907
 *
 *  All rights reserved.  No parts of this software may be sold or
 *  distributed in any form or by any means without the prior written
 *  permission of the authors.
 *
 *******************************************************************/

/* $Header: /a/ruan/src/RCS/local.h,v 1.1 84/06/19 22:41:49 ruan Exp $ */

/* $Log:	local.h,v $
 * Revision 1.1  84/06/19  22:41:49  ruan
 * Initial revision
 *  */

/* following is configuration dependent */

#define nHost       3
	/* max number of hosts, less than 65536, the smaller the better */

#define hostid(phe) (((*(struct in_addr *)phe->h_addr).s_addr >> 24) % nHost)
	/* mapping from host address to iHost, i.e. the host index
	   within [0..nHost-1],  must be one-one, not necessarily onto */

#define HOSTINIT    {"purdue-merlin", "purdue-arthur", "purdue-mordred"}
	/* host names, ordreded by their indices */

