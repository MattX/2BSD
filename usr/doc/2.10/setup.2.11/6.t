.\" Copyright (c) 1980 Regents of the University of California.
.\" All rights reserved.  The Berkeley software License Agreement
.\" specifies the terms and conditions for redistribution.
.\"
.\"	@(#)6.t	2.5 (2.11BSD) 2025/12/10
.\"
.de IR
\fI\\$1\fP\|\\$2
..
.ds LH "Installing/Operating \*(2B
.nr H1 6
.nr H2 0
.ds RH "System Operation
.ds CF \*(DY
.bp
.LG
.B
.ce
6. SYSTEM OPERATION
.sp 2
.R
.NL
.PP
This section describes procedures used to operate a PDP-11 UNIX system.
Procedures described here are used periodically, to reboot the system,
analyze error messages from devices, do disk backups, monitor
system performance, recompile system software and control local changes.
.NH 2
Bootstrap and shutdown procedures
.PP
In a normal reboot, the system checks the disks and comes up multi-user
without intervention at the console.
Such a reboot
can be stopped (after it prints the date) with a ^C (interrupt).
This will leave the system in single-user mode, with only the console
terminal active.
It is also possible to allow the filesystem checks to complete
and then to return to single-user mode by signaling \fIfsck\fP
with a QUIT signal (^\\).
.PP
If booting from the console command level is needed, then the command
.DS
\fB>>>\fP B
.DE
will boot from the default device and
ask for the name of the system to be booted.
Other systems such as the 11/44 require a device name to be given:
.DS
\fB>>>\fP B DU
.DE
to boot from a MSCP/UDA device.
Typing a carriage return will cause the default system (as compiled in
in section 4.1), to be booted.
In any case, the system selected will come up in single-user mode.
.PP
To bring the system up to a multi-user configuration from the single-user
all you have to do is hit ^D on the console.  The system
will then execute /etc/rc,
a multi-user restart script (and /etc/rc.local),
and come up on the terminals listed as
active in the file /etc/ttys.
See
\fIinit\fP\|(8)
and
\fIttys\fP\|(5).
Note, however, that this does not cause a file system check to be performed.
Unless the system was taken down cleanly, you should run
``fsck'' or force a reboot with
\fIreboot\fP\|(8)
to have the disks checked.
.PP
To take the system down to a single user state you can use
.DS
\fB#\fP kill 1
.DE
or use the
\fIshutdown\fP\|(8)
command (which is much more polite, if there are other users logged in.)
when you are up multi-user.
Either command will kill all processes and give you a shell on the console,
as if you had just booted.  File systems remain mounted after the
system is taken single-user.  If you wish to come up multi-user again, you
should do this by:
.DS
\fB#\fP cd /
\fB#\fP umount -a
\fB#\fP ^D
.DE
.PP
Each system shutdown, crash, processor halt and reboot
is recorded in the file /usr/adm/shutdownlog
with the cause.
.NH 2
Device errors and diagnostics
.PP
When serious errors occur on peripherals or in the system, the system
prints a warning diagnostic on the console.
These messages are written to the kernel logger where they are retrieved
by \fIsyslogd\fP\|(8) via \fI/dev/klog\fP - \fIdmesg\fP\|(8) is now
obsolete.  \fIdmesg\fP\|(8) is present in the distribution but no longer
used.  The message buffer is now 4kb in size and external to the kernel.
.PP
Error messages printed by the devices in the system are described with the
drivers for the devices in section 4 of the programmer's manual.
Some drivers have been modified to use the kernel logger, others still
simply do printf statements.
If errors occur suggesting hardware problems, you should contact
your hardware support group or field service.  It is a good idea to
examine the error log files regularly
(e.g. with ``tail \-r \fI/usr/adm/messages\fP'').
.NH 2
File system checks, backups and disaster recovery
.PP
Periodically (say every week or so in the absence of any problems)
and always (usually automatically) after a crash,
all the file systems should be checked for consistency by
\fIfsck\fP\|(8).
The procedures of
\fIreboot\fP\|(8)
should be used to get the system to a state where a file system
check can be performed manually or automatically.
.PP
Dumping of the file systems should be done on a regular schedule,
since once the system is going it is easy to
become complacent.
Complete and incremental dumps are easily done with
\fIdump\fP\|(8).
.PP
Dumping of files by name is best done by
\fItar\fP\|(1)
but the amount of data that can be moved in this way is limited
to a single tape.
Finally if there are enough drives entire
disks can be copied with
\fIdd\fP\|(1)
using the raw special files and an appropriate
blocking factor; the number of sectors per track is usually
a good value to use, consult \fI/etc/disktab\fP.
.PP
A full dump of the root filesystem in ``bootable`` format,
including the standalone programs mentioned back in chapter 2 (boot,
mkfs, restor and icheck) can be made by going to
/sys/pdpstand and:
.DS
make all
 ./maketape /dev/nrmtXX maketape.data
dump 0u /
.DE
If the
root file system is damaged by a hardware or software failure you
can rebuild a workable disk doing a restore in the
same way that the initial root file system was created.
.PP
Exhaustion of user-file space is certain to occur
now and then and may be managed with a combination of disc quotas
(the 4.3BSD disc quota system is available as a kernel configuration
option), threatening
messages of the day, and personal letters.
.NH 2
Moving file system data
.PP
If you have the equipment one way to move a file system
is to dump it to magtape using
\fIdump\fP\|(8),
use
\fInewfs\fP\|(8)
to create the new file system,
and restore the tape using \fIrestor\fP\|(8).
If you don't want to use magtape \fIdump\fP accepts an argument telling 
where to put the dump.  The argument may be a filename or the special 
name ''-'' meaning standard output.  A filesystem can be migrated using a
pipeline.  For example:
.sp
newfs /dev/rXXXa
dump 0f - / | restor rf /dev/rXXXa
.PP
Filesystems may also be moved by piping the output of a \fItar\fP\|(1)
to another \fItar\fP:
.PP
The \fIrestor\fP program accesses the raw device, laying down
inodes and blocks in the same place they came from as recorded by dump.
Care must therefore be taken when restoring a dump into
a file system smaller than the original file system.
.NH 2
Recompiling and reinstalling system software
.PP
It is easy to regenerate the system, and it is a good
idea to try rebuilding pieces of the system to build confidence
in the procedures.
The system consists of two major parts:
the kernel itself (/sys) and the user programs
(/usr/src and subdirectories).
The major part of this is /usr/src.
.PP
The three major libraries are the C library in /usr/src/lib/libc
and the \s-2FORTRAN\s0 libraries /usr/src/usr.lib/libI77 and
/usr/src/usr.lib/libF77.  In each
case the library is remade by changing into the corresponding directory
and doing
.DS
\fB#\fP make
.DE
and then installed by
.DS
\fB#\fP make install
.DE
Similar to the system,
.DS
\fB#\fP make clean
.DE
cleans up.
.PP
The source for all other libraries is kept in subdirectories of
/usr/src/usr.lib; each has a makefile and can be recompiled by the above
recipe.
.PP
If you look at /usr/src/Makefile, you will see that
you can recompile the entire system source with one command.
To recompile a specific program, find
out where the source resides with the \fIwhereis\fP\|(1)
command, then change to that directory and remake it
with the makefile present in the directory.
For instance, to recompile ``date'', 
all one has to do is
.DS
\fB#\fP whereis date
\fBdate: /usr/src/bin/date.c /bin/date /usr/man/man1/date.1\fP
\fB#\fP cd /usr/src/bin
\fB#\fP make date
.DE
this will create an unstriped version of the binary of ``date''
in the current directory.  To install the binary image, use the
install command as in
.DS
\fB#\fP install \-s date /bin/date
.DE
The \-s option will insure the installed version of date has
its symbol table stripped.  The install command should be used
instead of mv or cp as it understands how to install programs
even when the program is currently in use.
.PP
If you wish to recompile and install all programs in a particular
target area you can override the default target by doing:
.DS
\fB#\fP make
\fB#\fP make DESTDIR=\fIpathname\fP install
.DE
.PP
To regenerate all the system source you can do
.DS
\fB#\fP cd /usr/src
\fB#\fP make
.DE
.PP
If you modify the C library, say to change a system call,
and want to rebuild and install everything from scratch you
have to be a little careful.
You must insure that the libraries are installed before the
remainder of the source, otherwise the loaded images will not
contain the new routine from the library.  The following
sequence will accomplish this.
.DS
\fB#\fP cd /usr/src
\fB#\fP make clean
\fB#\fP make build
\fB#\fP make installsrc
.DE
The first \fImake\fP removes any existing binaries in the source trees
to insure that everything is reloaded.
The next \fImake\fP compiles and installs the libraries and compilers,
then compiles the remainder of the sources.
The final line installs all of the commands not installed in the first phase.
This will take about 12 hours on a reasonably configured 11/44.
.NH 2
Making local modifications
.PP
/usr/new is used by default for the programs that constitute
the contributed software portion of the distribution but which may not
have man pages installed.
Locally written commands that aren't distributed (or whose man pages are
not up to date) are kept in /usr/src/local and their binaries are 
kept in /usr/local.  This allows /usr/bin, /usr/ucb,
and /bin to correspond to the distribution tape
People using /usr/local commands are made
aware that the programs may not be in the base system yet.
.NH 2
Accounting
.PP
UNIX optionally records two kinds of accounting information:
connect time accounting and process resource accounting.  The connect
time accounting information is stored in the file \fI/usr/adm/wtmp\fP, which
is summarized by the program
.IR ac (8).
The process time accounting information is stored in the file
\fI/usr/adm/acct\fP after it is enabled by
.IR accton (8),
and is analyzed and summarized by the program
.IR sa (8).
.PP
If you need to recharge for computing time, you can develop
procedures based on the information provided by these commands.
A convenient way to do this is to give commands to the clock daemon
.I cron
to be executed every day at a specified time.  This is done by adding
lines to \fI/usr/adm/crontab\fP; see
.IR cron (8)
for details.
.NH 2
Resource control
.PP
Resource control in \*(2B is more
elaborate than in previous PDP-11 UNIX systems.
The resources consumed
by any single process can be limited by the mechanisms of
\fIsetrlimit\fP\|(2).  As distributed, the mechanism
is voluntary, though sites may choose to modify the login
mechanism to impose limits.  Csh now has the \fIlimits\fP
builtin command enabled.  Another available option
is the 4.3BSD disc quota system.
.NH 2
Files that need periodic attention
.PP
The discussion of system operations is concluded by listing
the files that require periodic attention or are system specific
.de BP
.IP \fB\\$1\fP
.br
..
.TS
center;
lb a.
/etc/fstab	how disk partitions are used
/etc/disktab	disk partition sizes
/etc/printcap	printer data base
/etc/gettytab	terminal type definitions
/etc/remote	names and phone numbers of remote machines for \fItip\fP(1)
/etc/group	group memberships
/etc/motd	message of the day
/etc/master.passwd	password file; each account has a line
/etc/rc.local	local system restart script; runs reboot; starts daemons
/etc/inetd.conf	local internet servers
/etc/hosts	host name data base
/etc/networks	network name data base
/etc/netstart	Startup file to configure network
/etc/services	network services data base
/etc/hosts.equiv	hosts under same administrative control
/etc/syslog.conf	error log configuration for \fIsyslogd\fP\|(8)
/etc/ttys	enables/disables ports
/etc/crontab	commands that are run periodically
/etc/aliases	mail forwarding and distribution groups
/usr/adm/acct	raw process account data
/usr/adm/messages	system error log
/usr/adm/shutdownlog	log of system reboots
/usr/adm/wtmp	login session accounting
.TE
