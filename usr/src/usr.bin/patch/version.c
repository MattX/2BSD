/* 
 * version.c 2.0  (2.11BSD) 2025/12/26
 */

#include "EXTERN.h"
#include "common.h"
#include "util.h"
#include "INTERN.h"
#include "patchlevel.h"
#include "version.h"

/* Print out the version number and die. */

void
version()
{
    extern char rcsid[];

    fatal3("%s\nPatch level: %d\n", rcsid, PATCHLEVEL);
}
