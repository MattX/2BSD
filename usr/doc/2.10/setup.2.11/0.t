.\" Copyright (c) 1980 Regents of the University of California.
.\" All rights reserved.  The Berkeley software License Agreement
.\" specifies the terms and conditions for redistribution.
.\"
.\"	@(#)0.t	2.4 (2.11BSD) 2025/12/10
.\"
.EH 'setup.2.11 - %''Installing and Operating 2.11BSD on the PDP-11'
.OH 'Installing and Operating 2.11BSD on the PDP-11''Setup.2.11 - %'
.ds Ps 2.10BSD
.ds 1B 2.10.1BSD
.ds 2B 2.11BSD
.bd S B 3
.TL
Installing and Operating \*(2B on the PDP-11
.br
December 10, 2025
.AU
Steven Schultz
.AI
sms@2bsd.com
.de IR
\\fI\\$1\|\\fP\\$2
..
.de UX
UNIX\\$1
..
.AB
.PP
.FS
.IP \(ua
DEC, PDP-11, VAX, IDC, SBI, UNIBUS and MASSBUS are trademarks of
Digital Equipment Corporation.
.FE
.FS
.IP \(dd
UNIX is a Trademark of Bell Laboratories.
.FE
This document contains instructions for the
installation and operation of the \*(2B PDP-11\(ua
.UX \(dd
system.
.PP
It discusses procedures for installing \*(2B UNIX on a PDP-11, including
explanations of how to lay out file systems on available disks, how to set
up terminal lines and user accounts, how to do system-specific tailoring,
and how to install and configure the networking facilities.  Finally, the
document details system operation procedures: shutdown and startup,
hardware error reporting and diagnosis, file system backup procedures,
resource control, performance monitoring, and procedures for recompiling
and reinstalling system software.
.PP
The ``bugs'' address supplied with this release
will work for some unknown period of time; make sure
the ``Index:'' line of the bug report indicates that the release is
``\*(2B''.  See the \fIsendbug\fP(8) program for more details.
.PP
Updates are available at the following locations:
.sp
ftp://ftp.dfupdate.se/pub/pdp11/2.11BSD
.sp
https://www.tuhs.org/Archive/Distributions/UCB/2.11BSD/Patches/
.sp
ftp://ftp.2bsd.com/2.11BSD
.sp
http://www.2bsd.com/2.11BSD
.AE
