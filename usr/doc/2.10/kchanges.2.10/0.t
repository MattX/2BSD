.EH 'Page %''Changes in 2.10BSD'
.OH 'Changes in 2.10BSD''Page %'
.TL
Changes in 2.10BSD
.sp
.de D
.ie \\n(.$>1 \\$1 \\$2 \\$3
.el DRAFT of \n(mo/\n(dy/\n(yr
..
.D April 20, 1987
.AU
Keith Bostic
.AI
Computer Systems Research Group
Department of Electrical Engineering and Computer Science
University of California, Berkeley
Berkeley, California 94720
seismo!keith; bostic@ucbvax.berkeley.edu
.AU
Casey Leedom
.AI
Department of Computer Science
California State University, Stanislaus
Turlock, California  95380
ucbvax!vangogh!casey; casey@vangogh.berkeley.edu
.PP
This document summarizes changes in PDP-11\(dg UNIX\(dd between
.FS
\u\(dg\d\s-2DEC\s0, \s-2PDP-11\s0, \s-2QBUS\s0, and
\s-2UNIBUS\s0 are trademarks of Digital Equipment Corporation.
.br
\u\(dd\d\s-2UNIX\s0 is a trademark of Bell Laboratories.
.FE
the July 1983 2.9BSD release and the April 1987 2.10BSD distribution.
.PP
It is intended to provide sufficient information that those who maintain
the kernel, have local modifications to install, or who have versions of
2.9BSD modified to run on other hardware should be able to determine how
to integrate this version of the system into their environment.  As always,
the source code is the final source of information, and this document is
intended primarily to point out those areas that have changed.
.PP
With the massive changes made to the system, both in organization
and in content, it may take some time to understand how to carry over
local software.  The majority of this document is devoted to describing
the contents of each important source file in the system.  If you have
local software to incorporate in the system
you should first read this document completely, then study the source
code to more fully understand the changes as they affect you.
.LP
Most of the changes between 2.9BSD and 2.10BSD fall into one of several
categories.  These are:
.RS
.IP \(bu 3
bug fixes,
.IP \(bu 3
performance improvements,
.IP \(bu 3
addition of 4.3BSD system calls,
.IP \(bu 3
removal of features no longer supported in the 4.3BSD release,
.IP \(bu 3
new protocol and hardware support.
.RE
.LP
The major changes to the kernel are:
.RS
.IP \(bu 3
the addition of networking,
.IP \(bu 3
a complete rewrite of the user/kernel interface,
.IP \(bu 3
the addition of numerous system calls,
.IP \(bu 3
replacement of much of the high kernel with portions of the 4.3BSD kernel,
.IP \(bu 3
the addition of the 4.3BSD tty and serial line drivers,
.IP \(bu 3
the addition of inode, swap and text cacheing algorithms,
.IP \(bu 3
restructuring the kernel into the 4.3BSD structure.
.RE
.PP
This document is not intended to be an introduction to the kernel, but
assumes familiarity with prior versions of the kernel, particularly the
2.9BSD release.  Other documents may be consulted for more complete
discussions of the kernel and its subsystems.  It cannot be too strongly
emphasized that 2.10BSD much more closely resembles 4.3BSD than it does
2.9BSD.  We have attempted to be, literally, bug-for-bug compatible with
4.3BSD.  It is \fBSTRONGLY\fP recommended that 4.3BSD manuals be consulted
when using this system.  2.9BSD manuals are no longer correct.  Online
documentation is as complete and correct as time permitted.
.PP
This release is not supported, nor should it be considered an official
Berkeley release.  It was called 2.10BSD because 2.9BSD has clearly
become overworked and System V was already taken.  The ``bugs'' address
supplied with this release (as well as with the 4BSD releases) will
work for some unknown period of time; \fBmake sure\fP that the ``Index:''
line of the bug report indicates that the release is ``2.10BSD''.  See
the \fIsendbug\fP(8) program for more details.  All fixes that
we make, or that are sent to us, will be posted on \fIUSENET\fP, in
the news group ``comp.bugs.2bsd''.
.PP
The authors gratefully acknowledge the contributions of many other
people to the work described here.  Major contributors include
Gregory Travis and Jeff Johnson of the Institute for Social Research,
and Steven Uitti of Purdue University.  Cyrus Rahman of Duke University
should hold some kind of record for being able to get the entire kernel
rewritten with a single 10-line bug report.  Much credit should also go
to the authors of 4.2BSD and 4.3BSD from which we stole everything that
wasn't nailed down and several things that were.  (Just ``diff'' this
document against \fIChanges to the Kernel in 4.2BSD\fP if you don't
believe that!)  We are also grateful for the invaluable guidance
provided by Michael Karels, of the Computer Science Research Group,
at Berkeley \- although we felt that his suggestion that we ``just buy
a VAX'', while perhaps more practical, was not entirely within the spirit
of the project.
