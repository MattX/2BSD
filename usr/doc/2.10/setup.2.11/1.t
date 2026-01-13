.\" Copyright (c) 1980 Regents of the University of California.
.\" All rights reserved.  The Berkeley software License Agreement
.\" specifies the terms and conditions for redistribution.
.\"
.\"	@(#)1.t	2.7 (2.11BSD) 2025/12/10
.\"
.ds lq ``
.ds rq ''
.ds LH "Installing/Operating \*(2B
.ds RH Introduction
.ds CF \*(DY
.LP
.nr H1 1
.bp
.LG
.B
.ce
1. INTRODUCTION
.sp 2
.R
.NL
.PP
This document explains how to install \*(2B UNIX for the PDP-11 on your
system.  This document has been revised several times since the first
release of \*(2B in 1991. The last significant revision in July 1995 
dealt with the addition of disk labels to the system.  This December 2025
revision is in preparation for the release in 2026 of the 35th Anniversary 
Edition of \*(2B.
.PP
The procedure for performing a full bootstrap is outlined in chapter 2.
The process includes copying a root file system from
the distribution tape into a new file system,
booting that root filesystem, and then reading the remainder of the system
binaries and sources from the archives on the tapes.
.PP
As \*(2B is not compatible at the filesystem level with previous versions
of UNIX on the PDP-11,
any upgrade procedure is essentially a full bootstrap.  There is a
limited ability to access old filesystems which may be used after
the system partitions have been loaded from a full bootstrap.
It is desirable to recompile most local software after the conversion,
as there are changes and performance improvements in the standard
libraries.
.PP
.NH 1
Hardware supported
.PP
This distribution can be booted on a PDP-11 with 1Mb of memory or more,
separate I&D, and with any of the following disks:
.sp
RK06, RK07
.sp
Any MSCP disk
.sp
RM03, RM05
.sp
RP04, RP05, RP06
.sp
Many other SMD disks, for example: CDC 9766, Fuji 160, Fuji Eagle
.PP
Other disks are supported (RX23, RX33, RX50, RD51) but are not large
enough to hold a root filesystem plus a swap partition.  The old restriction
of using RL02 drives in pairs has been lifted.  It is now possible to define
a root ('a') partition and a swap partition ('b') and load at least the
root filesystem to a single RL02.
Discs which are too small to hold even a root filesystem (floppies for 
example)
may be used as data disks or as standalone boot media, but are not useable
for loading the distribution.  Others, while listed above, are not very
well suited to loading the distribution.  The RK06/07 drives are hard pressed
to even hold the system binaries, much less the sources.
.PP
The tape drives supported by this distribution are:
.DS
.TS
lw(1.5i) l.
TS11, TU80, TK25
TM11, AVIV 6250/1600
TE16, TU45, TU77
TK50, TU81, TU81+, TZ30
.TE
.DE
.NH 1
Distribution format
.PP
The basic distribution contains the following items:
.DS
(1)\0\0 SimH magnetic tape image
(1)\0\0 template SimH config file (simh.ini)
(1)\0\0 Soft copy of this document
.DE
.PP
\fBThe distribution does not fit on several standard PDP-11 configurations
that contain only small disks\fP.  If your hardware configuration does not
provide at \fBleast 75\fP Megabytes of disk space you can still install the
distribution, but you will probably have to operate without source for the
user level commands and, possibly, the source for the operating system.
.PP
The root file system now occupies \fBa minimum of 4Mb\fP.  If at all possible
a larger, 6 or 7Mb, root partition should be defined when using the 
standalone
.B disklabel
program.
.PP
The basic bootstrap material is present in six
short files at the beginning of the tape.
The first file on the tape contains preliminary bootstrapping programs.
This is followed by several standalone
utilities (\fIdisklabel\fP, \fImkfs\fP\|(8), \fIrestor\fP\|(8), and 
\fIicheck\fP\|(8)\(ua)
.FS
.IP \(ua
References of the form X(Y) mean the subsection named
X in section Y of the UNIX programmer's manual.
.FE
followed by a full dump of a root file system (see \fIdump\fP\|(8)).
Following the root file system dump is a \fItar\fP archive of \fB/usr\fP
except for \fB/usr/src\fP.  Next a \fItar\fP archive
of the kernel source.  Finally a \fItar\fP archive of
the remaining system sources.
.PP
.KS
.DS L
TAPE:
.TS
n n n l.
Tape file	Record size	Records\(ua	Contents
_
0	512	1	primary tape boot block
	512	1	boot block (some tape boot ROMs go for this copy)
	512	69	standalone \fBboot\fP program
1	1024	37	standalone \fBdisklabel\fP
2	1024	33	standalone \fBmkfs\fP(8)
3	1024	35	standalone \fBrestor\fP(8)
4	1024	32	standalone \fBicheck\fP(8)
5	10240	285	\fIdump\fP of ``root'' file system
6	10240	3368	\fItar\fP dump of /usr, excepting /usr/src
7	10240	519	\fItar\fP dump of /usr/src/sys
8	10240	4092	\fItar\fP dump of /usr/src excepting sys
.TE
.DE
.KE
.IP \(ua
The number of records in each tape file are approximate
and do not necessarily correspond to the actual number on the tape.
.FE
.NH 1
UNIX device naming
.PP
UNIX has a set of names for devices which are different
from the DEC names for the devices.

The disk and tape names used by the bootstrap and the system are:
.DS
.TS
l l.
RK06, RK07 disks	hk
RL01, RL02 disks	rl
RK05	rk
MSCP disks	ra
RM02/03/05	xp
RP04/05/06	xp
SMD disks	xp
TM02/03, TE16, TU45, TU77 tapes	ht
TE10/TM11 tapes	tm
TS11 tapes	ts
TMSCP tapes	tms
.TE
.DE
Additionally, the following non-DEC devices are also supported:
.DS
.TS
l l.
SI 9500, CDC 9766	si
SI, CDC 9775	xp
SI6100, Fujitsu Eagle 2351A	xp
Emulex SC01B or SI9400, Fujitsu 160	xp
Emulex SC-21, xp
.TE
.DE
The generic SMD disk driver, \fIxp\fP, will handle
most types of SMD disks on one or more controllers
(even different types on the same controller).
The \fBxp\fP driver handles RM03, RM05, RP04, RP05 and  RP06
disks on DEC, Emulex, Dilog, and SI UNIBUS or MASSBUS controllers.
.PP
MSCP disks and TMSCP tapes include SCSI drives attached to the 
RQZX1 controller on the PDP-11/93.  MSCP disks and TMSCP tapes also include
SCSI drives attached to the Emulex UC07 or UC08 Q-BUS controllers on Q-bus 
systems as well as the UC17 and UC18 controllers on UNIBUS systems.
.PP
The standalone system used to bootstrap the full UNIX system
uses device names of the form:
.DS
\fIxx\|\fP(\fIc\fP,\fIy\fP,\fIz\fP)
.DE
where \fIxx\fP is one of \fBhk\fP, \fBht\fP, \fBrk\fP, \fBrl\fP,
\fBtm\fP, \fBts\fP, \fBtms\fP, or \fBxp\fP.
The value \fIc\fP specifies the controller number (0-3).  This value is
usually not explicitly given.  The default is 0 if booting from the standard
(first) CSR of a device.
.PP
Example: if there are two MSCP controllers in
the system addressed as 0172150 and 0172154 respectively booting from the
controller at 172154 requires that \fIc\fP be given as 1.  Booting from
the standard CSR of 0172150 would be done by specifying \fIc\fP as 0 or
omitting the \fIc\fP parameter.
\fBboot\fP automatically detects if the 
first (standard) CSR is being used.   All future references will ignore
the \fIc\fP parameter by assuming the default value.
.PP
The value \fIy\fP
specifies the device or drive unit to use.
The \fIz\fP
value is interpreted differently for tapes and disks:
for disks it is a partition number (0 thru 7) corresponding to partitions 
\'a\' thru \'h\'
respectively.  This should always be zero unless you
\fBreally\fP know what you are doing.  The ability to load a kernel from
the swap area is planned for the future but does not presently exist.
For tapes \fIz\fP is a file number on the tape.\(ua
.PP
In all simple cases, a drive with unit number 0 (determined either by
a unit plug on the front of the drive, or jumper settings on the drive
or controller) will be called unit 0 in its UNIX file name.
file name.
If there are multiple controllers, the drive unit numbers
will normally be counted within each controller.  Thus drives on
the the first controller are numbered 0 thru 7 and drives on the second
controller are numbered 0 thru 7 on controller 1.
Returning to the discussion of the standalone system, recall
that tapes also took two integer parameters.  In the case of a
TE16/TU tape formatter on drive 0, the
files on the tape have names ``ht(0,0)'', ``ht(0,1)'', etc.
Here ``file'' means a tape file containing a single data stream
separated by a single tape mark.
.PP
Each UNIX physical disk is divided into 8 logical disk partitions,
each of which may occupy any consecutive cylinder range on the
physical device.  While overlapping partitions are allowed they are not
a good idea, being an accident waiting to happen (making one filesystem will
destroy the other overlapping filesystems).
The cylinders occupied
by the 8 partitions for each drive type
are specified
by the disk label read from the disk.
.PP
If no label exists the disk will
not be bootable and while the kernel attempts not to damage
unlabeled disks (by swapping to or doing a crash dump on a live filesystem)
there is a chance
that filesystem damage will result if a kernel is loaded from an unlabeled 
disk.
.PP
The standalone \fBdisklabel\fP program is used to define the partition tables.
Each partition may be used
either as a raw data area (such as a swapping area) or to store a
UNIX file system.
It is mandatory for the first partition on a disk to start at sector offset 0
because the 'a' partition is used to read and write the label (which is at
the beginning of the disk).
If the drive is to be used 
to bootstrap a UNIX
system then the 'a' partition must be of type \fB2.11BSD\fP (FS_V71K in
\fIdisklabel.h\fP) and at least 4Mb is size.  A 'b' partition of at least
2-3Mb (4Mb is a good choice if space is available) for swapping is 
also needed.
If a drive is being used solely for data then that drive need not
have a 'b' (swap) partition but partition 'a' must still span the first
part of the disk.
The second partition is used as a swapping area, and the
rest of the disk is divided into spaces for additional ``mounted
file systems'' by use of one or more additional partitions.
.PP
The third ('c') logical partition of each physical disk also has a
conventional usage: it allows access to the entire physical device,
including the bad sector forwarding information recorded at the end of
the disk (one track plus 126 sectors).  It is occasionally used to store
a single large file system or to access the entire pack when making a
copy of it on another.  Care must be taken when using this partition not
to overwrite the last few tracks and thereby destroying the bad sector
information.
.NH 1
UNIX devices: block and raw
.PP
UNIX makes a distinction between ``block'' and ``raw'' (character)
devices.  Each disk has a block device interface where
the system makes the device byte addressable and you can write
a single byte in the middle of the disk.  The system will read
out the data from the disk sector, insert the byte you gave it
and put the modified data back.  The disks with the names
``/dev/xx0a'', etc are block devices.
There are also raw devices available.
These have names like ``/dev/rxx0a'', the
``r'' here standing for ``raw''.
Raw devices bypass the buffer cache and use DMA directly to/from
the program's I/O buffers;
they are normally restricted to full-sector transfers.
In the bootstrap procedures we
will often suggest using the raw devices, because these tend
to work faster.
Raw devices are used when making new filesystems,
when checking unmounted filesystems,
or for copying quiescent filesystems.
The block devices are used to mount file systems,
or when operating on a mounted filesystem such as the root.
.PP
You should be aware that it is sometimes important whether to use
the character device (for efficiency) or not (because it wouldn't
work, e.g. to write a single byte in the middle of a sector).
Don't change the instructions by using the wrong type of device
indiscriminately.
.PP
The standalone \fBdisklabel\fP program must be used to alter the 'a' and 'b'
partitions of a drive being used for a bootable system.  This is because
the kernel will not permit an open partition to change size or offset.
The root and and swap partitions are \fBalways\fP open when the kernel is
running.
