.\" Copyright (c) 1980 Regents of the University of California.
.\" All rights reserved.  The Berkeley software License Agreement
.\" specifies the terms and conditions for redistribution.
.\"
.\"	@(#)2.t	1.6 (GTE) Feb 6, 1993
.\"
.ds lq ``
.ds rq ''
.ds LH "Installing/Operating \*(2B
.ds RH Bootstrapping
.ds CF \*(DY
.bp
.nr H1 2
.nr H2 0
.bp
.LG
.B
.ce
2. BOOTSTRAP PROCEDURE
.sp 2
.R
.NL
.PP
This section explains the bootstrap procedure that can be used
to get the kernel supplied with this distribution running on your machine.
It is mandatory to do a full bootstrap since the filesystem has changed
from \*(1B to \*(2B.
.PP
The safest route is to use \fItar\fP\|(1) to dump all of your current
file systems, do a full bootstrap of \*(2B and then restore user files
from the backups.
There is also an untested version of \fI512restor\fP\|(8) available for
V7 sites that need to read old dump tapes.
.PP
It is also desirable to make a convenient copy of system configuration
files for use as guides when setting up the new system; the list of files
to save from earlier PDP-11 UNIX systems, found in chapter 3, may be used
as a guideline.
.PP
\*(2B \fIrestor\fP\|(8) is able to read and automatically convert to the
new on disk directory format
\fIdump\fP\|(8) tapes made under 2.9BSD, \*(Ps and \*(1B.
.NH 2
Booting from tape
.PP
The tape bootstrap procedure used to create a
working system involves the following major
steps:
.IP 1)
Loading the tape bootstrap monitor.
.IP 2)
Creating a UNIX ``root'' file system system on disk using \fImkfs\fP\|(8).
.IP 3)
Restore the full root file system using \fIrestor\fP\|(8).
.IP 4)
Boot the UNIX system on the new root file system and copy the
appropriate \fIdead start boot block\fP to your boot device.
.IP 5)
Build and restore the /usr file system from tape
with \fItar\fP\|(1).
.IP 6)
Restore the include and kernel sources from tape.
.IP 7)
Extract the remaining source from the second tape.
.IP 8)
Build a \fI/boot\fP configured to locate your root file system for
auto-rebooting (see section 4.1).  Tailor a version of UNIX to your
specific hardware (see section 4.2).
.PP
Certain of these steps are dependent on your hardware
configuration.  If your disks require formatting, standard DEC
diagnostic utilities will have to be used, they are not supplied on the
\*(2B distribution tape.
.NH 3
Step 1: loading the tape bootstrap monitor
.PP
To load the tape bootstrap monitor, first
mount the magnetic tape on drive 0 at load point, making
sure that the write ring is not inserted.
Then use the normal bootstrap ROM, console monitor or other
bootstrap to boot from the tape.
.PP
NOTE: The boot blocks expect the CSR of the booting controller in r0
and the unit number in r1.  \fBboot\fP may be booted from any controller
or unit, the earlier restrictions of controller 0 and unit 0 have been
lifted.
.PP
If no other means are available, the following code can be keyed in
and executed at (say) 0100000 to boot from a TM tape drive (the magic number
172526 is the address of the TM-11 current memory address register;
an adjustment may be necessary if your controller is at a nonstandard
address):
.DS
.TS
l l.
012700  (mov $unit, r1)
000000    (normally unit 0)
012700	(mov $172526, r0)
172526
010040	(mov r0, -(r0))
012740	(mov $60003, -(r0))
060003    (if unit 1 use 060403, etc)
000777	(br .)
.TE
.DE
When this is executed, the first block of the tape will be read into memory.
Halt the CPU and restart at location 0.
.PP
The console should type
.DS
.B
\fInn\fPBoot from \fIxx\fP(\fIctlr\fP,0,\fIcsr\fP)
:
.R
.DE
where \fInn\fP is the CPU type on which it believes it is running.
The value will be one of 23, 24, 40, 44, 45, 53, 60, 70, 73, 83, 84, 93
or 94 depending whether
separate instruction and data (separate I/D) and/or a UNIBUS map are detected.
For KDJ-11 systems the System Maintenance Register is examined to determine
the cpu type.  At present \*(2B runs on the 44, 53, 70, 73, 83, 84, 93 and 94
\fBonly\fP.  It must be emphasized that \*(2B requires separate I/D.
.sp
\fIctlr\fP encodes the controller number and unit number that \fBBoot\fP
was loaded from.  The top two bits (bits 6 and 7) are the controller
number and the low three bits (bits 0, 1 and 2) are the unit number.
.sp
\fIcsr\fP is an octal number telling the CSR of the controller from which
\fBBoot\fP was loaded.
.sp
.PP
You are now talking to the tape bootstrap monitor.
At any point in the following procedure you can return
to this section, reload the tape bootstrap, and restart.
Through the rest of this section,
substitute the correct disk type for \fIdk\fP
and the tape type for \fItp\fP.
.NH 3
Step 2: creating a UNIX ``root'' file system system
.PP
Now create the root file system using the following procedures.
First determine the size of your root file system from the
following table:
.DS
.TS
l l.
Disk	Root File System Size
	  (1K-byte blocks)

hk	4158
ra	7942 (All RA, RZ and RD drives except RD52 and RD53)
	4850 (RD52/53 - \fBbarely useable\fP)
rl01	5120\(dd
rl02	10240\(dd
xp	4800 (RM02/RM03)
	4560 (RM05)
	5120 (CDC 9775)
	4807 (RP04/RP05/RP06)
	4800 (Fujitsu Eagle)
	4702 (DIVA, Ampex 9300)
	8192 (Ampex Capricorn)
	5760 (SI Eagle)
.TE
.sp
.FS
.IP \(dd
These partitions cover the entire pack.
.FE
.DE
.PP
If the disk on which you are creating a root file system is an \fBxp\fP
disk, you should check the drive type register at this time to make sure
it holds a value that will be recognized correctly by the driver.  There
are numbering conflicts; the following numbers are used internally:
.DS
.TS
c l
c l.
Drive Type Register	Drive Assumed
Low Byte (standard address: 0776726)

020	RP04
021	RP05
022	RP06
024	RM03
025	RM02
027	RM05
072	Ampex Capricorn
073	SI, CDC 9775 (direct)
074	SI 6100, Fuji Eagle 2351A
075	Emulex SC01B/SC03B or SI 9400, Fuji 160 (in RM80 mode)
076	Emulex SC-21, Ampex 815 cylinder RM05
077	Diva Comp V, Ampex 9300
.TE
.DE
Check the drive type number in your controller manual,
or halt the CPU and examine this register.
If the value does not correspond to the actual drive type,
you must patch the \fIxptype\fP variable in memory
before any attempt is made to access the drive.
This will override the drive type register.
This value must be present at the time each program
(including the bootstrap itself) first tries to access the disk.
After starting each utility
but before accessing the disk, halt the CPU, place the new drive type
number at the proper memory location using the front panel or monitor
and then continue.  The address of \fIxptype\fP in each utility is
printed out on the console the first time the disc is accessed.
Because \fBboot\fP relocates itself you must remember to add 0600000 
to the address of \fIxptype\fP.
.PP
Once UNIX itself is booted (see section ``\fBStep 4: booting UNIX\fP'') you must
patch it also.  The \fIxp\fP driver in the \fBGENERIC\fP kernel prints to
the console the address of \fIxp_drive\fP and \fIxp_controller\fP and delays
for about 10 seconds.
.PP
Finally, determine the proper interleaving factors \fIm\fP and \fIn\fP
for your disk from the following table.  Extensive testing has demonstrated
that the choice of \fIm\fP is non critical (performance of a file system
varying only by 3 to 4% for a wide range of \fIm\fP values).  Values for
\fIm\fP within the range from  2 to 5 give almost identical performance.
Increasing \fIm\fP too much actually causes degraded performance because
the free blocks are too far apart.  Slower processors (such as the 73 and
44) may want to start with a \fIm\fP of 4 or 5, faster processors (such as the
70 and 84) may start with a \fIm\fP of 3 or 4.
On the other
hand, the \fIn\fP value is moderately important.  It should be the number
of filesystem blocks contained by one cylinder of the disc but nothing
bad will happen if you have to guess at a number.
These numbers determine the layout of the free list that will be constructed;
the proper interleaving will help increase the speed of the file system.
If you have a non-DEC disk that emulates one of the disks listed,
you may be able to use these numbers as well, but check that
the actual disk geometry is the same as the emulated disk
(rather than the controller mapping onto a different physical disk).
.PP
The earlier tables went into great detail specifying separate tables for
each cpu type.  This is not necessary.  In several cases the \fIm\fP
values were too high or the \fIn\fP values were too low.  The following
table specifies the \fIn\fP parameter only.  The \fIm\fP parameter selection
was explained in a previous paragraph.
.KS
.DS
.TS
cB s
l l.
Disk Interleaving Factor (\fIn\fP)
DISK
RL01/2	20
RK06/7	33
RM02	80
RM03	80
RM05	304
RP04/5/6	209
RA60	84
RA70	181
RA71	357
RA72	510
RA73	735
RA80	217
RA81	357
RA82	435
RA90	448
RA92	448
RC25	31
RD31	34
RD32	51
RD33	59
RD52	63
RD53	72
RD54	127
RZ22	66
RZ23	132
RZ23L	78
RZ24	152
RZ24L	130
RZ25	279
RZ26	399
RZ55	270
RZ56	405
RZ57	532
RZ58	637
.TE
.DE
.KE
.sp
See
\fImkfs\fP\|(8)
for more explanation of the values of \fIm\fP and \fIn\fP.
For \fIm\fP/\fIn\fP numbers for other drive types see \fI/etc/disktab\fP.
\fIn\fP would be calculated using the \fBnt\fP and \fBns\fP parameters
from \fI/etc/disktab\fP.  For a disc with 512 byte sectors compute \fIn\fP
by multiplying \fBns\fP times \fBnt\fP and dividing the result by 2.
.PP
Then run the standalone version of the \fImkfs\fP (8) program.
In the following procedure, substitute the correct types
for \fItp\fP and \fIdk\fP and the size determined above for \fIsize\fP:
.DS
.TS
lw(1.5i) l.
\fB:\|\fP\fItp\|\fP(0,1)	(\fImkfs\fP is tape file 1)
\fBBoot: bootdev=0nnnn bootcsr=0mmmmmm\fP
\fBMkfs\fP
\fBfile system:\fP \fIdk\|\fP(0,0)	(root is the first file system on drive 0)
\fBfile system size:\fP \fIsize\fP	(count of 1024 byte blocks in root)
\fBinterleaving factor (m, 5 default):\fP \fIm\fP	(interleaving, see above)
\fBinterleaving modulus (n, 10 default):\fP \fIn\fP	(interleaving, see above)
\fBisize = XX\fP	(count of inodes in root file system)
\fBm/n = \fP\fIm n\fP	(interleave parameters)
\fBExit called\fP
\fInn\fP\fBBoot\fP
\fB:\fP	(back at tape boot level)
.TE
.DE
.sp
The number \fBnnnn\fP is the device number of the device (high byte is the
major device number and the low byte is the unit number).  The \fBmmmmmm\fP
number is the CSR of the device.  This information is mainly used as 
a reminder and diagnostic/testing purposes.
.sp
You now have an empty UNIX root file system.
.NH 3
Step 3: restoring the root file system
.PP
To restore the root file system onto it, type
.DS
.TS
lw(1.5i) l.
\fB:\|\fP\fItp\|\fP(0,2)	(\fIrestor\fP is tape file 2)
\fBBoot: bootdev=0nnnn bootcsr=0mmmmmm\fP
\fBRestor\fP
\fBTape?\fP \fItp\|\fP(0,4)	(root \fIdump\fP is tape file 4)
\fBDisk?\fP \fIdk\|\fP(0,0)	(into root file system)
\fBLast chance before scribbling on disk.\fP  (type a carriage return to start)
.B
"End of tape"	"(appears on same line as message above)"
Exit called
\fInn\fPBoot
\fB:\fR	(back at tape boot level)
.R
.TE
.sp
This takes about 8 minutes with a TZ30 on a 11/93.
.DE
If you wish, you may use the \fIicheck\fP program on the tape,
\fItp\|\fP(0,3), to check the consistency of the file system you have just
installed.
.NH 3
Step 4: booting UNIX
.PP
You are now ready to boot from disk.  It is best to read the rest
of this section first, since some systems must be patched while booting.
Then type:
.DS
.TS
lw(1.5i) l.
\fB:\fP\fIdk\|\fP(0,0)unix	(bring in unix from the root system)
\fBBoot: bootdev=0nnnn bootcsr=0mmmmmm\fP
.TE
.DE
The standalone boot program will then load unix from
the root file system you just created, and the system should boot:
.DS
.B
.\"CHECK
\*(2B BSD UNIX #1: Sat Jul 4 01:33:03 PDT 1992
    root@wlonex.iipo.gtegsc.com:/usr/src/sys/GENERIC
phys mem  = \fI???\fP\(dd
avail mem = \fI???\fP
user mem  = \fI???\fP

configure system
\fI\&... information about available devices ...\fP
.R
(Information about various devices will print;
most of them will probably not be found until
the addresses are set below.)
.B
erase=^?, kill=^U, intr=^C
#
.R
.DE
.FS
.IP \(dd
\fBNote:\fP If you are booting from an \fIxp\fP with a drive type
that is not recognized, it will be necessary to patch the system before
it first accesses the root file system.
The \fBGENERIC\fP kernel \fIxp\fP driver prints out the addresses of
\fIxp_drive\fP and \fIxp_controller\fP and then delays for 10 seconds
(approximately).
Halt the processor after it has printed out the addresses
but before the disk has been accessed.
.PP
Place the drive type number corresponding to your drive
at the address of \fIxp_drive\fP plus 2.  Modify only the low byte, the
high byte is the drive unit number.
The address for the first controller's CSR is \fIxp_controller\fP plus 4.
It should be noted that at present the kernel may only be booted from a
controller at the standard/first CSR.  An exception to this is that a MSCP
based kernel may be booted from drive 0 of any controller.
If you plan to use any drives other than 0 before you recompile
the system, you should patch the location for those drives.  The \fIxp_drive\fP
structure in the \fBGENERIC\fP kernel is 012 bytes long.
Make the patches and continue the CPU.
The value before patching must be zero.  If it is not, you have halted too
late and should try again.
.FE
.PP
UNIX itself then runs for the first time and begins by printing out a banner
identifying the release and
version of the system that is in use and the date that it was compiled.  
.PP
Next the
.I mem
messages give the amount of real (physical) memory, the amount of memory
left over after the system has allocated various data structures, and the
amount of memory available to user programs in bytes.
.PP
The information about different devices being attached or not being found
is produced by the \fIautoconfig\fP\|(8) program.  Most of this is not
important for the moment, but later the device table, \fI/etc/dtab\fP,
can be edited to correspond to your hardware.  However, the tape drive of
the correct type should have been detected and attached.
.PP
The \*(lqerase ...\*(rq message is part of /.profile
that was executed by the root shell when it started.  This message
is present to remind you that the character erase,
line erase, and interrupt characters are set to what is
standard for DEC systems; this insures that things are
consistent with the DEC console interface characters.
.PP
UNIX is now running single user on the installed root file system,
and the `UNIX Programmer's Manual' applies.
The next section tells how to complete
the installation of distributed software on the /usr file system.
The `#' is the prompt from the shell,
and lets you know that you are the super-user,
whose login name is \*(lqroot\*(rq.
.PP
The disk with the new root file system on it will not be bootable
directly until the block 0 bootstrap program for your disk has been installed.
There are copies of the bootstraps in /mdec.
Use \fIdd\fP\|(1) to copy the right boot block onto block 0 of the disk.
.DS
\fB#\fP dd if=/mdec/\fIboot\fP of=/dev/r\fIdk\^\fP0a count=1
.DE
Block zero bootstraps and the devices they support are:
.DS
.TS
l l l.
boot	driver	devices
_
hkuboot	hk	RK06/07
rauboot	ra	All RA, RD, RZ, RX (except RX01,02) and RC25 drives
rkuboot	rk	RK05
rluboot	rl	RL01/02
si95uboot	si	SI 9500, CDC 9766
dvhpuboot	xp	Diva Comp V, Ampex 9300
hpuboot	xp	RP04/05/06
rm03uboot	xp	RM02/03
rm05uboot	xp	RM05 or SI 9500, CDC 9766
si51uboot	xp	SI 6100, Fujitsu Eagle 2351A
si94uboot	xp	Emulex SC01B/SC03B or SI 9400, Fujitsu 160
.TE
.DE
Once this is done, booting from this disk will load and execute the block
0 bootstrap, which will in turn load /boot.  The console will print
.DS
.TS
lw(1.5i) l.
.B
\fInn\fPBoot from \fIdk\fP(\fIctlr\fP,0,\fIcsr\fP) \fR(printed by /boot)\fP
:
.R
.TE
.DE
It automatically boots /\fIboot\fP for you;
if /\fIboot\fP is not found, the system will hang/loop forever.
The block 0 program is very small (has to fit in 512 bytes) and simple 
program, however, and can only
boot the second-stage boot from the first file system.
Once /boot is running and prints its ``: '' prompt,
boot unix as above.
.PP
As distributed /\fIboot\fP will load \fIdk\fP(0,0)unix by default if a
carriage return is typed at the \fB:\fP prompt.
.PP
\fBNOTE:\fP NONE the primary bootstraps have a prompt or alternate program
name capability because of space considerations.  No diagnostic message
results if the file cannot be found.
.NH 3
Step 5: setting up the /usr file system
.PP
First set a shell variable to the name of your disk, so
the commands we give will work regardless of the disk you
have; do one of the following:
.DS
.TS
l l.
\fB#\fP disk=hk	(if you have RK06's or RK07's)
\fB#\fP disk=rl	(if you have RL01's or RL02's)
\fB#\fP disk=ra	(if you have an MSCP drive)
\fB#\fP disk=xp	(if you have an RP06, RM03, RM05, or other SMD drive)
.TE
.DE
.PP
The next thing to do is to extract the rest of the data from the tape.
You might wish to review the disk configuration information in section
titled \fB``Disk configuration''\fP before continuing; you will have
to select a partition to restore the /usr file system into which is
at least \fB25\fP Megabytes in size (this is just barely enough for
the system binaries and such and leaves no room for the system
source.)  The partitions used below are those most appropriate in
size.  Find the disk you have in the following table and
execute the commands in the right hand portion of the table.  Some
smaller discs (such as the RD51 and RL01/02) are not listed because they 
do not have room to even begin loading /usr.  These smaller drives can be 
used as data discs (perhaps to mount /tmp on) but are not useable for loading
the /usr file system on to.
.DS
.TS
l l.
DEC RM02/03	\fB#\fP name=xp0h; type=rm03
DEC RM05	\fB#\fP name=xp0e; type=rm05
DEC RP04/05	\fB#\fP name=xp0c; type=rp04
DEC RP06	\fB#\fP name=xp0g; type=rp06
DEC RK07	\fB#\fP name=hk0h; type=rk07
DEC RA60	\fB#\fP name=ra0c; type=ra60
DEC RA70	\fB#\fP name=ra0d; type=ra70
DEC RA71	\fB#\fP name=ra0d; type=ra71
DEC RA72	\fB#\fP name=ra0d; type=ra72
DEC RA73	\fB#\fP name=ra0d; type=ra73
DEC RA80	\fB#\fP name=ra0c; type=ra80
DEC RA81	\fB#\fP name=ra0c; type=ra81
DEC RA82	\fB#\fP name=ra0d; type=ra82
DEC RA90	\fB#\fP name=ra0d; type=ra90
DEC RA91	\fB#\fP name=ra0d; type=ra91
DEC RA92	\fB#\fP name=ra0d; type=ra92
DEC RD32	\fB#\fP name=ra0g; type=rd32
DEC RD33	\fB#\fP name=ra0g; type=rd33
DEC RD52	\fB#\fP name=ra0g; type=rd52
DEC RD53	\fB#\fP name=ra0d; type=rd53
DEC RD54	\fB#\fP name=ra0g; type=rd54
DEC RZ22	\fB#\fP name=ra0g; type=rz22
DEC RZ23	\fB#\fP name=ra0g; type=rz23
DEC RZ23L	\fB#\fP name=ra0g; type=rz23l
DEC RZ24	\fB#\fP name=ra0d; type=rz24
DEC RZ24L	\fB#\fP name=ra0d; type=rz24l
DEC RZ25	\fB#\fP name=ra0d; type=rz25
DEC RZ26	\fB#\fP name=ra0d; type=rz26
DEC RZ55	\fB#\fP name=ra0d; type=rz55
DEC RZ56	\fB#\fP name=ra0d; type=rz56
DEC RZ57	\fB#\fP name=ra0d; type=rz57
DEC RZ58	\fB#\fP name=ra0d; type=rz58
-
CDC 9766	\fB#\fP name=xp0e\(dd
CDC 9775	\fB#\fP name=xp0e\(dd
AMPEX 300M	\fB#\fP name=xp0e\(dd
FUJITSU 160M	\fB#\fP name=xp0d\(dd
FUJITSU 2351A	\fB#\fP name=xp0d\(dd
AMPEX 330M	\fB#\fP name=xp0c\(dd
.TE
.DE
.FS
.IP \(ua
Unfortunately the \fInewfs\fP(8) program is relatively primitive and
doesn't know the free list spacing for these non-DEC drives.  An
appropriate \fImkfs\fP(8) command will have to be substituted for the
\fInewfs\fP command specified below.  See \fI/etc/disktab\fP for
[hopefully] appropriate free list \fIm/n\fP numbers for some non-DEC
disks.
.FE
Find the tape you have in the following table and execute the
commands in the right hand portion of the table:
.DS
.TS
l l.
DEC TM02/03, TE16/TU45/TU77	\fB#\fP cd /dev; rm *mt*; ./MAKEDEV ht0; sync
DEC TS11, TU80/TS05	\fB#\fP cd /dev; rm *mt*; ./MAKEDEV ts0; sync
DEC TM11, TU10/TE10/TS03	\fB#\fP cd /dev; rm *mt*; ./MAKEDEV tm0; sync
DEC TMSCP, TK50/TZ30/TU81	\fB#\fP cd /dev; rm *mt*; ./MAKEDEV tu0; sync
EMULEX TC11	\fB#\fP cd /dev; rm *mt*; ./MAKEDEV tm0; sync
.TE
.DE
Then execute the following commands:
.br
.ne 5
.sp
.DS
.TS
lw(2i) l.
\fB#\fP date \fIyymmddhhmm\fP	(set date, see \fIdate\fP\|(1))
\&....
\fB#\fP passwd root	(set password for super-user)
\fBNew password:\fP	(password will not echo)
\fBRetype new password:\fP
\fB#\fP hostname \fImysitename\fP	(set your hostname)
\fB#\fP newfs ${name} ${type}  (create empty user file system)
(this takes a few minutes)
(if you wish to specify a different \fIm\fP value, use a command of the 
 form: newfs -m # ${name} ${type}
 where # is the \fIm\fP parameter and must be between 1 and 31)
\fB#\fP mount /dev/${name} /usr	(mount the usr file system)
\fB#\fP cd /usr	(make /usr the current directory)
\fB#\fP mt rew
\fB#\fP mt fsf 5
\fB#\fP tar xpbf 20 /dev/rmt12 	(extract all of usr except usr/src)
(this takes about 15-20 minutes except for the TK50 and TZ30 which are 
\fBmuch\fP slower)
.TE
.DE
The data on the sixth tape file has now been extracted.
All that remains on the first tape is a small archive containing
source for the kernel and include files.
.PP
If you have an existing/old password file to be merged back into
\*(2B, special steps are necessary to convert the old password file
to the shadow password file format (shadow password file 
and password aging were ported from 4.3BSD and are standard in \*(2B ).
.DS
.TS
lw(2i) l.
\fB#\fP mt fsf	(position tape at beginning of next tape file)
\fB#\fP mkdir src	(make directory for source)
\fB#\fP cd src	(make /usr/src the current directory)
\fB#\fP tar xpbf 20 /dev/rmt12 	(extract the system and include source)
(this takes about 5-10 minutes)
\fB#\fP cd /	(back to root)
\fB#\fP chmod 755  /  /usr  /usr/src /usr/src/sys
\fB#\fP rm \-f sys
\fB#\fP ln \-s usr/src/sys sys	(make a symbolic link to the system source)
\fB#\fP umount /dev/${name}	(unmount /usr)
.TE
.DE
.PP
The first tape has been been completely loaded.
You can check the consistency of the /usr file system by doing
.DS
\fB#\fP fsck /dev/r${name}
.DE
The output from
.I fsck
should look something like:
.DS
.B
** /dev/r\fIxx\fP0g
File System: /usr

NEED SCRATCH FILE (179 BLKS)
ENTER FILENAME:  /tmp/xxx
** Last Mounted on /usr
** Phase 1 - Check Blocks and Sizes
** Phase 2 - Check Pathnames
** Phase 3 - Check Connectivity
** Phase 4 - Check Reference Counts
** Phase 5 - Check Free List
671 files, 3497 used, 137067 free
.R
.DE
.PP
If there are inconsistencies in the file system, you may be prompted
to apply corrective action; see the document describing
.I fsck
for information.
.PP
To use the /usr file system, you should now remount it by
saying
.DS
\fB#\fP /etc/mount /dev/${name} /usr
.DE
.NH 3
Step 7: extracting remaining source from the second tape
.PP
You can then extract the source code for the commands from the
second distribution tape\(ua
.FS
.IP \(ua
On the TK50 the remaining source is the 8th file on the cartridge.
.FE
(with the exception on RK07's, RM03's, and RD52's and other small discs
this will fit in the /usr file system):
.DS
\fB#\fP cd /usr/src
\fB#\fP tar xpb 20
.DE
If you get an error at this point, most likely it was
a problem with tape positioning.  Rewind the tape and
use the \fBmt\fP command to skip files, then retry the
\fBtar\fP command.
.NH 2
Additional conversion information
.PP
After setting up the new \*(2B filesystems, you may restore the user
files that were saved on tape before beginning the conversion.  Note that
the \*(2B \fIrestor\fP program does its work by accessing the raw file
system device and depositing inodes in the appropriate locations on
disk.  This means that file system dumps might not restore correctly if
the characteristics of the file system have changed (eg. if you're
restoring a dump of a file system into a file system smaller than the
original.)  To restore a dump tape for, say, the /u file system something
like the following would be used:
.DS
\fB#\fP restor r /dev/rxp1e
.DE
.PP
If \fItar\fP images were written instead of doing a dump, you should
be sure to use the `p' option when reading the files back.
No matter how you restore a file system, be sure and check its
integrity with \fIfsck\fP when the job is complete.
.PP
\fItar\fP tapes are preferred (when possible) because the inode
allocation is performed by the kernel rather than the \fIrestor\fP\|(8)
program.  This has the benefit of allocating inodes sequentially starting
from the beginning of the inode portion of the filesystem rather than
preserving the fragmented/randomized order of the old filesystem.
