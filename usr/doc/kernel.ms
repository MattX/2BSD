.TL
Introduction to
.br
Berkeley Kernel Modifications
.br
Unix
\- Seventh Edition 
.sp
Last PDP-11 Kernel Distribution
.br
July, 1981
.AU
Bob Kridle
.AI
Electrical Engineering and Computer Sciences Department
University of California at Berkeley
Berkeley, California  94720
.AU
William F. Jolitz
.AI
U. S. Geological Survey
Office of Earthquake Studies
Menlo Park, California  94025
.AB
This tape contains work done at Berkeley,
USGS, and several other sites
in the last two years to make the UNIX \- Version 7 PDP-11
Kernel usable in our environments.
Appended is a list of most of the modifications made
to the kernel.
Since Berkeley does not plan to buy any more PDP-11s as UNIX vehicles
and is planning to replace the existing PDP-11 systems with VAXen in
the next year, we are rapidly losing interest in PDP-11 work.
The system included here is by no means a finished product, but it
is unlikely that much more significant progress will be made.
It is our purpose to make available all work done until now.
The most recent addition to this system is a mechanism to allow
overlaid kernel text permitting fairly robust systems to be run
on non-separate I and D machines such as the PDP-11/34.
In addition, more device handlers have been added.
The system should boot on almost anything.
.AE
.PP
The source for the kernel as well as bootstrap
tools to facilitate conversion to a faster 1 Kbyte blocked
file system are included on this tape.
A small root file system is also included and contains
several bootable kernels and all necessary auxiliary files
to bring up a temporary kernel on a large variety of hardware.
The kernels all include the same set of device handlers,
and differ only in which device is assigned as the root and swap
device.
The kernels configure all processors as if they were 11/34s and
and do not use any of the features of the larger machines such 
as separate I and D, or the UNIBUS map. These kernels are intended
to be used only long enough to configure a kernel appropriate for
your specific hardware.
We assume that potential users already have at least a
Bell Version 7 distribution tape.
The root dump image and /kernel subtree on the
following tar image contain only a subset
of UNIX tools sufficient to configure and build new kernels.
We also assume that users have experience in configuring
and modifying the UNIX Kernel.
.PP
There are, no doubt, many remaining bugs and documentation
omissions.
Although we are running some version of the kernel here on several systems,
we are aware of many configurations and combinations of options which
have never been tried.
The tape is, unfortunately, rather documentation poor.
Where possible, we have kept things as similar to Bell as
we could and the Bell manuals will be a help.
Unfortunately, we have almost no time to help others bringing up this
system.
A newsgroup on the Duke news network USENET called net.2bsd-bugs
has been set up at Berkeley so that users of this
tape can attempt to coordinate their efforts and share solutions.
People working on the kernel are strongly urged to join USENET
(see the usenet directory elsewhere on this tape)
to share bugs (and hopefully fixes) with other users.
If you cannot join Usenet but can send electronic mail,
you can post messages to this newsgroup by sending mail
via the Arpanet to csvax.2bsd-bugs@Berkeley or
via uucp net to ucbvax!2bsd-bugs.
In order to receive the reports, it will be necessary to
join Usenet.
The only hardware required to join Usenet is either an Arpanet connection
or a dialup.  An auto-dialer helps, but isn't essential.
It will be necessary to find another site to connect to
(we can't offer a connection at Berkeley, sorry) \-
for names of some current sites in your area see the Usenet directory.
.SH 
Tape Contents
.PP
The first few files on the tape follow fairly closely what
is described in the beginning of the
.I "Setting Up Unix"
paper in the
.I "Unix Programmers Manual."
That is, there is a standalone bootstrap followed by standalone
versions of a few utilities followed by a dump of a small root
file system.
These programs perform the same functions as those on the Bell
tape but are modified to create and use 1K block file systems.
In addition, the standalone code on this tape supports a few more
peripherals, ie. rm03/5, rl01/2, rk05, and rk07 disk drives
and ts11 tape drives.
The remainder of the tape contains a tar of the kernel source
and source for those utilities which had to be modified to
be used with 1K blocks. The tape looks like this:
.DS
File	Name			Record Size

0	boot block		512
	(EOR)
	boot block		512
	(EOR)
	Standalone Boot 	512
	(EOF)
1	Standalone \fBcat\fR	1024
	(EOF)
2	This file		1024
	(use \fBcat\fR to read)
	(EOF)
3	A tape index		1024
	(use \fBcat\fR to read)
	(EOF)
4	Standalone \fBmkfs\fR	1024
	(EOF)
5	Standalone \fBrestor\fR	1024
	(EOF)
6	Standalone \fBicheck\fR	1024
	(EOF)
7	Dump of small root	10K
	(About 4,000 512 byte blks)
	(EOF)
8	Tar of Source		10K
	(About 25,000 512 byte blks)
	(The kernel support dir, /kernel,
	is small enough to fit on 1 rk05)
	(EOF)
	(EOF)
.DE
You may first want to simply pull the source from the tape
onto your current file system and peruse it for a while.
Use
.DS
.B
mkdir ucb
chdir ucb
dd if=/dev/nrmt0 of=/dev/null bs=20b files=8
tar xbf 20 /dev/rmt0 \fR(\fPkernel\fR)\fP
.R
(the subtree kernel is about 4800 512Kb blocks if you are space limited)
.DE
.SH
Using the Tape
.PP
We suggest the following procedure for introducing yourself
to the tape.
.IP 1.
Read this document carefully from beginning to end.
.IP 2.
Gather the following ingredients:
.RS
.IP A.
An idle PDP-11 available for at least eight hours.
.IP B.
A free formatted pack.
.IP C.
A full dump of your /usr file system or
a Bell V7 Distribution Tape.
.RE
.IP 3.
Mount the master magtape on tape drive 0 at load point.
.IP 4.
Mount a formatted disk pack on disk drive 0.
.IP 5.
Load the beginning block of the tape into memory.
For machines with the M9312 bootstrap, this
means loading address 173404 and pressing start
on the cpu console (see the appendix if you don't
have a firmware boot).  If you have a Remote Diagnostic Console (in addition
to a M9312), type \fB<cntrl-p> h 173404g\fR.
If you have no boot ROM see \fBboot(VIII)\fR in
the \fIUnix Programmers Manual\fR.
.IP 6.
The console should type:
.DS
.I
xxBoot
:
.R
.DE
where xx is the class of PDP 11 you are booting on;

LAST MINUTE NOTE:  The standalone boot on this tape will print
`40boot' no matter what type of system you boot on. It was discovered
in the final moments that the standalone boot, after discovering it was
running on a separate I and D machine failed to load a non-separate
kernel correctly. The standalone boot on the tape as well as /40boot
on the root file system do not attempt to discover what type
of processor they are running on and assume an 11/40 style machine.
.PP
Now the standalone bootstrap is running.  It really is a
miniature operating system, with it's own small set
of commands.
As shown in this document,
the machine's printouts are shown in \fIitalics\fR,
operator responses are in \fBboldface\fR, and
explanatory comments are within ( ).
Terminate each line you type by carriage return or line-feed.
.PP
Should you make a mistake while typing, the character \fB#\fR
erases the last character typed up to the beginning of the line,
and the character \fB@\fR erases the entire line typed.
Some consoles cannot print lower case letters.  Adjust the
instructions accordingly.
.PP
There are three classes of tape drives known to the standalone system:
.DS
\fBtm\fR	TU10
\fBht\fR	TU16/TE16
\fBts\fR	TS11
.DE
There are also six classes of disks.
They are:
.DS
\fBxp\fR	RP04/5/6	(Assumed RH70 on 11/70)
	RM02/3/5	(Assumed RH70 on 11/70)

\fBrk\fR	RK05

\fBhk\fR	RK07

\fBrp\fR	RP02/3

\fBrl\fR	RL01/2

\fBdv\fR	DIVA COMP-V/300 Mbyte SMD Drives
	(33 sect./track)
.DE
\fBNote for those booting on foreign vendor disk controllers:\fR

The \fBxp\fR disk handlers in both the kernel and standalone
system use the drive type register low byte to determine the size parameters
of the attached disk drive as follows:
.DS
.ta 2i 5i

Drive Type      Drive Assumed	(Sectors/Surfaces)
Reg Low Byte
Addr=0776726

022	RP04/04/06	22/19
024	RM02/RM03	32/5
025	Emulex SC-21V/300 Mb    	32/19
027	RM05	32/19
.DE
Check this now and determine if your controller implements the
correct value.
If it does not, you must place the correct value in the switch
register after the standlone bootstrap is booted from
tape and before any attempt is made to access the drive.
This will override the drive type register for the standalone \fBxp\fR
handler.
Once the system is read in (see \fBBooting Unix\fR below) you must
halt the processor \fBimmediately\fR after the `mem = xxx' message
is printed and patch the location \fBxp_type\fR given below.
The value prior to patching must be zero. If not you have halted too
late and should try again.
.SH
Making a Disk From Tape
.PP
Copy the magtape to disk by the following procedure.
.DS
.ta 3i
(bring in the program mkfs)
\fI:\|\fBht(0,4)\fR	(use `tm(0,4)' for the TU10)
	(use `ts(0,4)' for the TS11)
\fIMkfs
\fIfile system size:\fB 2000\fR	(size in 1K blocks for an rk05)
\fIfile system:\fBrk(0,0)\fR	(or `hk(0,0)' or `rl(0,0)', etc.)

or

\fIfile system size:\fB 4096\fR	(size in 1K blocks for anything else)
\fIfile system:\fBxp(0,0)\fR	(or `hk(0,0)' or `rl(0,0)', etc.)
.I
isize = XX
m/n = XX
.R
(after a while)
.I
exit called
xxBoot
:
.R
.DE
This step makes an empty file system.
.IP 7.
Next, restore the data
onto the new empty file system.  To do this you respond to
the `\fI:\fR' printed in the last step with:
.DS
(bring in the program restor)
\fI:\|\fBht(0,5)\fR
\fIRestor
tape?\fB ht(0,7)\fR		
\fIdisk?\fB hp(0,0)\fR
\fILast chance before scribbling on disk.\fR (you type \fBreturn\fR)
(the tape moves, perhaps 5-10 minutes pass)
\fIend of tape\fR
.I
xxBoot
:
.R
(On RK05 disks you may get an "out of inodes" message.
Ignore this.)
.DE
You have a UNIX root file system now!
.SH
Booting Unix
.PP
You probably have
the bootstrap running, left over from the last step above;
if not,
repeat the boot process (step 4) again.
Then use the appropriate one of the following:
.DS
\fI:\|\fBxp(0,0)xpunix\fR	(for RP04/5/6)
\fI:\|\fBrp(0,0)rpunix\fR	(for RP02/3)
\fI:\|\fBrl(0,0)rlunix\fR	(for RL01/2)
\fI:\|\fBrk(0,0)rkunix\fR	(for RK05)
\fI:\|\fBhk(0,0)hkunix\fR	(for RK07)
\fI:\|\fBdv(0,0)dvunix\fR	(for DIVA Comp-V)
.DE
The machine should type the following:
.DS
.I
mem = xxx
#
.R
.DE
The
.I
mem
.R
message gives the memory available to user programs
in bytes.
One more step is necessary if you have a TU/E-16 or TS-11 tape drive.
These boot systems contain tape handlers for all three
TU/E-16, TU-10, and TS-11 tape systems.
However, all these drives use the same interrupt vector
location.
We have defaulted these locations for the TU-10 handler;
To use the others you must patch them.
Halt your CPU and make the following changes using switches
or the console subsystem:
.DS
.ta 1i 2i 4i
.I
DRIVE	LOCATION	OLD CONTENTS	NEW CONTENTS

TS-11	224	0472	0502
TU/E-16	224	0472	0512
.R
.DE
Continue the CPU.
The system you have just booted actually contains all the possible
disk and tape handlers as well as a DZ-11 handler for 8 ports.
Some of the standard interrupt vectors overlap and some are floating
meaning that they are not likely to be correct for your system.
Since dz unibus addresses are also floating they may also
have to be altered.
The following table indicates a number of locations which can be
patched to tailor the system to your hardware.
Note that in the case of the xp/rp/dv overlap location 0254 has
already been patched in each of xpunix, rpunix, and dvunix to
be correct.
If you are satisfied with running only on the root device and
doing all your work from the console until you have compiled
a local system, none of this is necessary.
.DS

Device		Default/(std) Vect	Contents	Bdev/Cdev

tmio		0224/(0224)		0472		3/12
htio		0230/(0224)		0512		7/15
tsio		0234/(0224)		0502		13/27
rkio		0220/(0220)		0552		0/9
hkio		0210/(0210)		0562		10/25
rlio		0320/(flt)		0572		8/18
xpio		0254/(0254)		0522		12/20
dvio		0260/(0254)		0532		14/23
rpio		0264/(0254)		0542		1/14
dzin		0300/(flt)		0612		NA/22
dzou		0304/(flt)		0622		NA/22
lpou		0320/(0320)		0602		NA/2

Offset of data in load image 0116340
Offset of data in loaded core image 0

Physical address of xp_type = 064210
Physical address of dz_addr = 065114
Default value of dz_addr = 0160000
.DE
You may also want to patch the system load image you used with
adb.
We leave this as an early exercise.
.PP
UNIX is now running,
and the \fBUNIX Programmer's Manual\fR applies (sort of);
references below of the form X(Y) mean the subsection named
X in section Y of the manual.
The `#' is the prompt from the Shell,
and indicates you are the super-user.
The user name of the super-user is \fBroot\fR
if you should find yourself in multi-user mode and
need to log in;
no password is necessary.
.PP
To simplify your life later, rename the appropriate version
of the system as specified above plain \fBunix\fR.
For example, use \fImv\fR(UA1) as follows if you have an RP04/5/6.
.DS
\fBmv xpunix unix\fR
.DE
In the future, when you reboot,
you can type one of:
.DS
\fBxp(0,0)unix\fR	(for the hp system)
\fBrl(0,0)unix
rk(0,0)unix\fR
or whatever
.DE
to the `:' prompt.
.PP
You may wish at this point to extract 
the remainder of the tape.
We have set up all the drivers in the bootable systems
so that the first logical (minor dev. 0) is the root
with swap at the end
and the remainder (minor dev. 1)
covers the rest of the device.
Make an appropriate size file system (mkfs speaks in
1K blocks!!) and mount it, and use tar to extract
the remainder of 2BSD.
You will need a minimum of about 25K blocks for
our tape plus whatever space you need for your /usr
to extract the whole tape.
If you have limited file space, you can extract only the
kernel support directory, /kernel. There is enough in
this directory along with the root you have just constructed
to reconfigure and compile a new kernel.
About 4877 512Kb blocks will be required for this subtree.
.PP
We leave the remainder of reconfiguration for your local
guru.
Some hints may be found in
kernel/include/sys/localopts.h.
Also examine the makefiles carefully.
The bootstrap systems were created in /kernel/sys/40SYS.
Other clues may be gained by scanning the source for
conditional compilation flags such as UCB_NKB or
UCB_IHASH as most of our functional changes are
on compilation flags.
.PP
Good Luck!!
.SH
Acknowledgments
.PP
A great many people have worked on this system in the last
year and a half.
Bob Kridle, Bill Jolitz, and Mike Karels have spent the most time
recently ironing out last minute bugs.
Jeff Schriebman and Ed Gould did the work
on the buffers as well as much else.
Many pieces were lifted from the VAX system used here
at Berkeley.
Bill Joy was responsible for most of the original work
on the VAX.
Others who contributed include Mark Horton,
Steve Feldman, Mark Schwartz, Ken Arnold, Joe Kalash,
and Eric Allman.
I am sure I have neglected some who contributed or
provided constructive criticism.
.SH
Major System Modifications - User Visible
.sp 2
.SH
UCB_QUOTAS \- Dynamic file system quotas
.PP
By no means bullet proof, these nevertheless prevent many
painful "file system full" accidents.
They will require about one more in core inode for each user
with a file quota.
See man sections on \fbpq\fr and \fbsetquot\fr.
.SH
UCB_GRPMAST \- Group master accounts
.PP
Give away some of those weighty root responsibilities to a unique
member of each group.
.SH
UCB_NKB	 \- "n" KB byte system buffers 
.PP
Cut seek time by half at a cost of slightly greater fragmentation.
Almost doubles I/O bandwidth.
Requires UCB_BUFOUT below.
.SH
UCB_PGRP \- Count process limit by process group
.PP
Fixes a botch in std. V7.
.SH
UCB_STICKYDIR \- Can't rm file from sticky dir unless owned
.PP
Protection for tmp files, etc.
.SH
UCB_LSTAT \- Extended stat call (for quotas)
.PP
Returns first two block pointers which we use for
quota info.
.SH
UCB_DKEXT \- Extended I/O Monitoring
.PP
This is not complete.
.SH
UCB_SMINO \- Small inodes (NADDR == 7)
.PP
Reduces size of inode table.
Small files are still about same size due to 1K block size.
.SH
UCB_LOGIN \- login sys call is available
.PP
Used for extended accounting.
.SH
UCB_SUBM \- "submit" processing
.PP
Hook for accounting background processes.
.SH
UCB_XACC \- extended process accounting
.PP
More accounting.
.SH
UCB_LOAD \- load average and uptime
.PP
Gives real idea of system load.
Number of runnable or short sleep processes averaged over
previous 1, 5, 15 minute interval.
.SH
MENLO_OVLY \- process text overlays
.PP
Run those big ones from the VAXen.
.SH
MENLO_KOV \- overlaid kernel text
.PP
Must be used for non-separate I/D machines.
Can be used on all.
.SH
UCB_METER \- vmstat performance metering
.PP
More monitoring of performance stats.
.sp 2
.ce 1
User Invisible Changes
.SH
UCB_BUFOUT \- Cache buffers moved out of sys data space
.PP
We use 60-100 buffers and and improved cache scheme.
Care must be used so that the virtual space pointed to
by the segmentation register which is remapped to these
buffers in not normally occupied by data which is accessed
in the same routines as the buffers.
Check the name list for this.
Buffers are accessed through KDSA5 (0120000-0140000.)
.PP
A recently discovered bug as resulted in a quick fix of
protecting part of this code with spl(7)s.
This is a poor solution which will increase interrupt
latency too much.
The clock may lose time, etc.
We hope to have a better solution soon.
.SH
UCB_CLIST \- Clists moved out of regular D-space
.PP
Good for processes which would like to write a buffer
at a time to ttys.
Lower rescheduling load.
.SH
UCB_MOUNT \- Multiple superblocks per internal buffer
.PP
Two per 1K buffer as these use in-space buffers.
.SH
UCB_SCCSID \- Put SCCS ID string in each system module
.PP
Takes up to much room in D space - a bad idea.
.SH
UCB_BHASH \- hashed buffer accessing
.PP
Cuts time in getblk by 50% or better.
.SH
UCB_IHASH \- hashed inode accessing
.PP
Same for iget.
.SH
UCB_FRCSWAP \- Force swap on expand/fork
.PP
For CPU bound systems; let disk controllers help
out with all those copysegs in fork and expand.
.SH
Other Enhancements - Not on Flags
.PP
There are a number of other improvements which were
not put on compilation flags due to readability
problems.
These can be found by diffing with a pure V7 system
if desired.
In particular, the ``new teletype driver'' is included
as line discipline 2 (NTTYDISC).
This is really sexy - see the enclosed stty command and
manual page.
.DE
.SH
KNOWN BUGS
.SH
LD
.PP
There are two versions of the overlay loader, one for the system
and one for user programs. These should be merged. In addition, we
have experienced problems regenerating either one from the source we
have. Don't assume what you compile from the sources here will work.
The binaries in /bin/ld and kernel/sys/40SYS/covld do work and are
different; the latter is used to make kernels.
.SH
ADB
.PP
Modification of this to work with overlaid code (ovadb) is
only partially complete, buggy, and undocumented. The file
kernel/sys/40SYS/getsymb.sh uses ovadb and may give you a clue
or two.
.SH
OVERLAID BUFFERS IN KERNEL
.PP
See kernel/sys/HINTS about this.
.SH
DISK ECC
.PP
This is turned off in xp.c. The original code was shown to
be incorrect a few weeks ago and the corrections have not been tested.
.SH
SECURITY
.PP
Jim Haynes at U.C. Santa Cruz
contributed a number of security fixes (in hard copy,
unfortunately) which we did not have time to include. He also has a very
good paper on UNIX security in general. Contact him for more info.
.SH
MFREE
.PP
Does not check the end of the coremap or swapmap
tables when returning released
space. Enough fragmentation will cause destruction of the data structures
following these tables.
.SH
UCB_DKEXT
.PP
Should be removed; an aborted experiment.
.SH
NETLDISC
.PP
The ``network line discipline'' number 1,
which was put in for the Berknet to make it reasonable to run
at 9600 baud,
does not work.
Strangely enough, the same code works fine on the Vax and the Onyx.
.SH
MANY MANY MORE FOR YOU TO DISCOVER!!
