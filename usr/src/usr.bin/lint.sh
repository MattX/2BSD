#!/bin/sh
#
#   @(#)lint.sh  1.0   (2.11BSD) 2025/12/28
#

cat <<END
lint does not understand C prototypes.  When the ANSI compliant stdio package
was added to 2.11BSD lint became useless and would produce a huge stream of
error messages with any program that included stdio.h

There is a plan to port xlint to 2.11BSD.  When that happens a patch will
be released adding lint into the system.
END

exit 0
