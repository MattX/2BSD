.\" Copyright (c) 1980 Regents of the University of California.
.\" All rights reserved.  The Berkeley software License Agreement
.\" specifies the terms and conditions for redistribution.
.\"
.\"	@(#)1.t	6.1 (Berkeley) 8/29/87
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
This document explains how to install the \*(2B release of
UNIX for the PDP on your system.  Because the system call interface
has changed dramatically between \*(2B and all previous versions of
UNIX on the PDP,
you will have to do a full bootstrap from the distribution tape.
The procedure for performing a full bootstrap is outlined in chapter 2.
The process includes copying a root file system from
the distribution tape into a new file system,
booting that root filesystem, and then reading the remainder of the system
binaries and sources from the archives on the tapes.
.NH 2
Hardware supported
.PP
This distribution can be booted on most PDP-11 cpus
with 1Mb of memory or more*,
.FS
* \*(2B would probably only require a moderate amount of squeezing to
fit on machines with less memory, but it would also probably be a little
unhappy about the prospect.
.FE
separate I&D, floating point**
.FS
** The separate I&D and floating point requirements will hopefully be
lifted in the near future.
.FE
and with any of the following disks:
.DS
.TS
lw(1.5i) l.
RL01, RL02
RK06, RK07
RD51, RD52, RD53
RA60, RA80, RA81
RC25
RM02, RM03, RM05
RP04, RP05, RP06
Ampex 9300, CDC 9766, Diva Comp V, Fuji 160, SI Eagle
.TE
.DE
.PP
The tape drives supported by this distribution are:
.DS
.TS
lw(1.5i) l.
TS11, TU80
TM11, AVIV 6250/1600
TE16, TU45, TU77
.TE
.DE
.PP
.NH 2
Distribution format
.PP
The basic distribution contains the following items:
.DS
(2)\0\0 1600bpi 2400' magnetic tapes,
(1)\0\0 Hardcopy of this document,
(1)\0\0 Hardcopy of the \fIChanges in 2.10BSD\fP document, and
(1)\0\0 Hardcopy of the \*(2B /README file.
.DE
Installation on any machine requires a tape unit. 
Since certain standard PDP packages
do not include a tape drive, this means one must either
borrow one from another PDP system or one must be purchased
separately.
.PP
\fBThe distribution does not fit on several standard PDP configurations
that contain only small disks\fP.  If your hardware configuration does not
provide at least \fB75\fP Megabytes of disk space you can still install the
distribution, but you will probably have to operate without source for the
user level commands and, possibly, the source for the operating system.
.PP
The root file system now occupies 4Mb and hence will not fit on the
old \fIa\fP partitions of some disks.  In particular, a new partition
\fIe\fP has been created for the RK06 and RK07 which overlaps
partitions \fIa\fP and \fIb\fP, and a new partition \fIf\fP has
been set up for the RM02 and RM03 which overlaps their \fIa\fP and
\fIb\fP partitions.  The distribution RK06/RK07 kernel assumes that
its root device is drive zero, partition \fIe\fP (/dev/hk0e) and that its
swap device is drive \fBone\fP, partition \fIa\fP (/dev/hk\fB1\fPa).  The
distribution RM02/RM03 kernel assumes that its root device is drive zero,
partition \fIf\fP (/dev/xp0f) and that its swap device is drive
\fBone\fP, partition \fIa\fP (/dev/xp\fB1\fPa).  The RL01/RL02 driver
doesn't support partitioning of drives, so the RL01/RL02 kernel assumes
that its root device is drive zero (/dev/rl0h) and its swap device is
drive \fBone\fP (/dev/rl\fB1\fPh).  All other distribution kernels assume
that their root device is drive zero partition \fIa\fP (/dev/\fIxx\fP0a)
and their swap device is on partition \fIb\fP of the same drive
(/dev/\fIxx\fP0b).
.PP
If you have the facilities, it is a good idea to copy the
magnetic tape(s) in the distribution kit to guard against disaster.
The tapes are 9-track 1600 BPI and contain some
512-byte records, followed by some 1024-byte records,
followed by many 10240-byte records.
There are interspersed tape marks; end-of-tape is signaled
by a double end-of-file.
.PP
The basic bootstrap material is present in five
short files at the beginning of the first tape.
The first file on the tape contains preliminary bootstrapping programs.
This is followed by stand alone versions of several file system
utilities (\fImkfs\fP\|(8), \fIicheck\fP\|(8) and \fIrestor\fP\|(8)*)
.FS
* References of the form X(Y) mean the subsection named
X in section Y of the 
.UX
programmer's manual.
.FE
followed by a full dump of a root file system (see \fIdump\fP\|(8)).
Following the root file system dump is a tape archive image of \fB/usr\fP
excepting \fB/usr/src\fP (see \fItar\fP\|(1)).  Finally, a tape archive
of source for include files and kernel source ends the first tape.  The
second tape contains a tape archive image, also in \fItar\fP format, of
all the remaining source that comes with the system.
.PP
.DS L
TAPE 1:
.TS
n n n l.
Tape file	Record size	Records**	Contents
_
0	512	1	boot block
	512	1	boot block (some tape boot ROMs go for this copy)
	512	28	stand alone \fBboot\fP program
1	1024	24	stand alone \fBmkfs\fP(8)
2	1024	15	stand alone \fBicheck\fP(8)
3	1024	30	stand alone \fBrestore\fP(8)
4	10240	300	\fIdump\fP of ``root'' file system
5	10240	2300	\fItar\fP dump of /usr, excepting /usr/src
6	10240	400	\fItar\fP dump of /usr/src/include and /usr/src/sys
.TE
.PP
TAPE 2:
.TS
n n n l.
Tape file	Record size	Records**	Contents
_
0	10240	3300	\fItar\fP dump of /usr/src, excepting include and sys
.TE
.FS
** The number of records in each tape file are approximate
and do not correspond to the actual tape.
.FE
.DE
.NH 2
UNIX device naming
.PP
UNIX has a set of names for devices which are different
from the DEC names for the devices, viz.:
The disk and tape names used by the bootstrap and the system are:
.DS
.TS
l l.
RK06, RK07 disks	hk
RL01, RL02 disks	rl
UDA disks	ra
RC25 disks	ra
RD51/52/53 disks	ra
MSCP disks	ra
RM02/03/05	xp
RP04/05/06	xp
SMD disks	xp
TE16, TU45, TU77/TM02, 3 tapes	ht
TE10/TM11 tapes	tm
TS11 tapes	ts
.TE
.DE
.PP
The generic SMD disk driver, \fIxp\fP, will handle
most types of SMD disks on one or more controllers
(even different types on the same controller).
The \fBxp\fP driver handles RM02, RM03, RM05, RP04, RP05 and  RP06
disks on DEC, Emulex, Diva, and SI UNIBUS or MASSBUS controllers.
.PP
The standalone system used to bootstrap the full UNIX system
uses device names of the form:
.DS
\fIxx\|\fP(\fIy\fP,\fIz\fP)
.DE
where \fIxx\fP is one of \fBhk\fP, \fBht\fP, \fBrk\fP, \fBrl\fP,
\fBrp\fP, \fBtm\fP, \fBts\fP, or \fBxp\fP.  The value \fIy\fP
specifies the device or drive unit to use.
The \fIz\fP
value is interpreted differently for tapes and disks:
for disks it is a block offset for a file system
and for tapes it is a file number on the tape.
.PP
Each UNIX physical disk is divided into 8 logical disk partitions,
each of which may occupy any consecutive cylinder range on the
physical device.  The cylinders occupied
by the 8 partitions for each drive type
are specified in section 4 of the programmers manual
and in the disk description file /etc/disktab (c.f.
\fIdisktab\fP(5)).*
.FS
* It is possible to change the partitions
by changing the code for the table in the disk driver; it
is often desirable to do this, therefore these tables should
be read off each pack; they may be in a future version of the system.
.FE
Each partition may be used
for either a raw data area such as a swapping area or to store a
UNIX file system.
It is conventional for the first partition on a disk to be used
to store a root file system, from which UNIX may be bootstrapped,
but as already mentioned above, the RK06/07 and RM02/03 generic distribution
kernels use other partitions because of the small size of the \fI\fP
partitions on those disks.
The second partition is traditionally used as a paging area, and the
rest of the disk is divided into spaces for additional ``mounted
file systems'' by use of one or more additional partitions.
.PP
The eighth logical partition of each physical disk also has a conventional
usage: it allows access to the entire physical device, including the bad
sector forwarding information recorded at the end of the disk (one track
plus 126 sectors).  It is
occasionally used to store a single large file system or to access
the entire pack when making a copy of it on another.  Care must be taken when
using this partition not to overwrite the last few tracks and thereby
clobber the bad sector information.
.PP
The disk partitions have names in the standalone system of the form
``\fIxx\|\fP(\fIy\fP,\fIz\fP)'' as described above.  Thus partition
1 of an RK07 at drive 0 would be ``hk(0,5940)''.
When not running
standalone, this partition would normally be available as ``/dev/hk0b''.
Here the prefix ``/dev'' is the name of the directory where all
``special files'' normally live, the ``hk'' serves an obvious purpose,
the ``0'' identifies this as a partition of hk drive number ``0''
and the ``b'' identifies this as partition 1 (where we number
from 0, the 0th partition being ``hk0a'').
Finally, ``5940'' is the \fI512-byte sector\fP offset to partition 1,
as determined from the manual page
.IR hk (4).
.PP
In all simple cases, a drive with unit number 0 (in its unit
plug on the front of the drive) will be called unit 0 in its UNIX
file name.  This is not, however, strictly necessary, since the system
has a level of indirection in this naming.
If there are multiple controllers, the disk unit numbers
will normally be counted sequentially across controllers.
This can be taken
advantage of to make the system less dependent on the interconnect
topology, and to make reconfiguration after hardware
failure extremely easy.  We will not discuss that now.
.PP
Returning to the discussion of the standalone system, we recall
that tapes also took two integer parameters.  In the case of a
TE16/TU tape formatter on drive 0, the
files on the tape have names ``ht(0,0)'', ``ht(0,1)'', etc.
Here ``file'' means a tape file containing a single data stream
separated by a single tape mark.
The distribution tapes have data structures in the tape
files and though the first tape contains only 8 tape files, it contains
several thousand UNIX files.
.NH 2
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
