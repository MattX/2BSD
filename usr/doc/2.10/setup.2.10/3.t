.\" Copyright (c) 1980 Regents of the University of California.
.\" All rights reserved.  The Berkeley software License Agreement
.\" specifies the terms and conditions for redistribution.
.\"
.\"	@(#)3.t	6.1 (Berkeley) 5/14/86
.\"
.ds lq ``
.ds rq ''
.ds LH "Installing/Operating \*(2B
.ds RH "Upgrading a PDP UNIX System
.ds CF \*(DY
.LP
.nr H1 3
.nr H2 0
.bp
.LG
.B
.ce
3. UPGRADING A 2.8BSD or 2.9BSD SYSTEM
.sp 2
.R
.NL
.PP
If you have local system modifications to the
kernel to install, look at the document
``Changes to the Kernel in \*(2B'' to get an idea of how
the system changes will affect your local modifications.
If you have local device drivers, see the file \fI/sys/OTHERS/README\fP
for hints on how to integrate your drivers into \*(2B.
.PP
If you are running 2.[89]BSD running the \fI1K file system\fP,
upgrading your system involves replacing your kernel and system utilities.
Binaries compiled under 2.[89]BSD will \fBnot\fP work without recompilation
under \*(2B.  This means that you will not be able to upgrade gradually,
but will have to create a new root containing the new kernel and system
binaries.  Note that the 2.[89]BSD compiler will \fBnot\fP be able to
compile the 2.10BSD kernel and will probably fail on a large number of
the applications.
.PP
Section 3.1 lists the files to be saved as part of the conversion process.
Section 3.2 describes the bootstrap process.
Section 3.3 discusses the merger of the saved files back into the new system.
.NH 2
Files to save
.PP
The easiest upgrade path to \*(2B is to do a full bootstrap as described
in Chapter 2.  If it is absolutely necessary to upgrade in place,
\fBmake dumps of all your file systems\fP, create new root and /usr file
systems from the distribution tape on unused disk partitions, boot the
new system.  All user file systems can be retained unmodified.  The
astute reader will detect that this differs little from a full bootstrap.
.PP
The following list enumerates the standard set of files you will want to
save and suggests directories in which site specific files should be
present.  Note that because \*(2B has changed so radically from previous
versions of UNIX on the PDP, many of these files may not exist on your
system, and will almost certainly require extensive changes for \*(2B,
but it's still handy to have them around as you're configuring \*(2B.
This list will likely be augmented with non-standard files you have added
to your system.
.PP
If you do not have enough space to create parallel
file systems, you should create a \fItar\fP image of the
following files before the new file systems are created.
In addition, you should do a full dump before rebuilding the file system
to guard against missing something the first time around.
.DS
.TS
l l.
/.cshrc	root csh startup script
/.login	root csh login script
/.profile	root sh startup script
/.rhosts	for trusted machines and users
/dev/MAKEDEV	in case you added anything here (/dev/MAKE in 2.9BSD)
/dev/MAKEDEV.local	for making local devices
/etc/disktab	in case you changed disk partition sizes
/etc/dtab	table of devices to attach at boot time
/etc/fstab	disk configuration data
/etc/ftpusers	for local additions
/etc/gateways	routing daemon database
/etc/gettytab	getty database
/etc/group	group data base
/etc/hosts	for local host information
/etc/hosts.equiv	for local host equivalence information
/etc/networks	for local network information
/etc/passwd	user data base
/etc/printcap	line printer database
/etc/protocols	in case you added any local protocols
/etc/rc	for any local additions
/etc/rc.local	site specific system startup commands
/etc/remote	auto-dialer configuration
/etc/services	for local additions
/etc/syslog.conf	system logger configuration
/etc/securettys	for restricted list of ttys where root can log in
/etc/ttys	terminal line configuration data
/etc/ttytype	terminal line to terminal type mapping data
/etc/termcap	for any local entries that may have been added
/lib	for any locally developed language processors
/usr/dict/*	for local additions to words and papers
/usr/hosts/MAKEHOSTS	for local changes
/usr/include/*	for local additions
/usr/lib/aliases	mail forwarding data base
/usr/lib/crontab	cron daemon data base
/usr/lib/font/*	for locally developed font libraries
/usr/lib/lib*.a	for locally libraries
/usr/lib/lint/*	for locally developed lint libraries
/usr/lib/sendmail.cf	sendmail configuration
/usr/lib/tabset/*	for locally developed tab setting files
/usr/lib/term/*	for locally developed nroff drive tables
/usr/lib/tmac/*	for locally developed troff/nroff macros
/usr/lib/uucp/*	for local uucp configuration files
/usr/man/manl	for manual pages for locally developed programs
/usr/msgs	for current msgs
/usr/spool/*	for current mail, news, uucp files, etc.
/usr/src/local	for source for locally developed programs
/sys/conf/HOST	configuration file for your machine
/sys/conf/files.HOST	list of special files in your kernel
.TE
.DE
.NH 3
Installing \*(2B
.PP
The next step is to build a working \*(2B system.
This can be done by following the steps in section 2 of
this document for extracting the root and /usr file systems
from the distribution tape onto unused disk partitions.
.PP
Once you have extracted the \*(2B system and booted from it,
you will have to build a kernel customized for your configuration.
If you have any local device drivers,
they will have to be incorporated into the new kernel.
See section 4.2.3 and ``Building 2.10BSD UNIX Systems.''
.PP
The disk partitions in \*(2B are the same as those in 2.[89]BSD, except
for those additions to the \fIRK06/07\fP and \fIRM02/03\fP already
mentioned earlier.  If you have changed the disk partition sizes, be sure
to make the necessary table changes and boot your custom kernel BEFORE
trying to access any of your 2.[89]BSD file systems!
.PP
\fIA note to our BETA sites\fP: The partition tables for the \fIra\fP
were changed between the BETA release of \*(2B and the final release.  In
general, partitions \fIc\fP and \fIh\fP were swapped (though a couple
required more extensive rearrangement), in an effort to maintain the
standard that partition \fIh\fP represents the entire pack.  We were at
least partially successful.
.PP
In any case, the manual pages in section 4 of the manual \fIare\fP correct
and describe the standard release partition arrangements accurately.
Note that this is not an endorsement of those partition layouts.  As
always, the BUGS section lists disk labeling as a good thing.
.NH 2
Merging your files from earlier PDP UNIX systems into \*(2B
.PP
When your system is booting reliably and you have the \*(2B
root and /usr file systems fully installed you will be ready
to continue with the next step in the conversion process,
merging your old files into the new system.
.PP
If you saved the files on a \fItar\fP tape, extract them
into a scratch directory, say /usr/convert:
.DS
\fB#\fP mkdir /usr/convert
\fB#\fP cd /usr/convert
\fB#\fP tar x
.DE
.PP
There is very little that can be said here as the variety of previous
versions of PDP UNIX systems and how they were administered is large.
As an example, most previous versions of PDP UNIX systems used the files
\fI/etc/ttys\fP and \fI/etc/ttytype\fP to administer which terminals should
have login processes attached to them and what the types of those terminals
were.  Under \*(2B /etc/ttytype has disappeared entirely, its functions
subsumed by /etc/ttys along with several new functions.
.PP
In general you will simply have to use your previous configuration files
as references as you configure \*(2B to your site needs.
Familiarity with 4.3BSD configuration is very helpful at this point since
\*(2B is nearly identical in most of the files listed in the previous
section.
.PP
If you have any homegrown device drivers
that use major device numbers reserved by the system you
will have to modify the commands used to create the devices or alter
the system device configuration tables in /sys/pdp/conf.c.
Note that almost all \*(2B major device numbers are different from
those in previous PDP UNIX systems.
.PP
System security changes require adding several new ``well-known'' groups 
to /etc/group.
The groups that are needed by the system as distributed are:
.DS
.TS
l c.
name	number
_
wheel	0
daemon	1
kmem	2
sys	3
tty	4
operator	5
staff	10
.TE
.DE
Only users in the ``wheel'' group are permitted to \fIsu\fP to ``root''.
Most programs that manage directories in /usr/spool
now run set-group-id to ``daemon'' so that users cannot
directly access the files in the spool directories.
The special files that access kernel memory, \fI/dev/kmem\fP
and \fI/dev/mem\fP, are made readable only by group ``kmem''.
Standard system programs that require this access are
made set-group-id to that group.
The group ``sys'' is intended to control access to system sources,
and other sources belong to group ``staff.''
Rather than make user's terminals writable by all users,
they are now placed in group ``tty'' and made only group writable.
Programs that should legitimately have access to write on user's terminals
such as \fItalk\fP and \fIwrite\fP now run set-group-id to ``tty''.
The ``operator'' group controls access to disks.
By default, disks are readable by group ``operator'',
so that programs such as \fIdf\fP can access the file system
information without being set-user-id to ``root''.
.PP
Several new users have also been added to the group of ``well-known'' users 
in /etc/passwd.
The current list is:
.DS
.TS
l c.
name	number
_
root	0
daemon	1
operator	2
uucp	66
nobody	32767
.TE
.DE
The ``daemon'' user is used for daemon processes that
do not need root privileges.
The ``operator'' user-id is used as an account for dumpers
so that they can log in without having the root password.
By placing them in the ``operator'' group, 
they can get read access to the disks.
The ``uucp'' login has existed long before \*(2B,
and is noted here just to provide a common user-id.
The password entry ``nobody'' has been added to specify
the user with least privilege.
.PP
After installing your updated password file,
you must run \fImkpasswd\fP\|(8) to create the \fIndbm\fP
password database.
Note that \fImkpasswd\fP is run whenever \fIvipw\fP\|(8) is run.
.PP
The format of the cron table, /usr/lib/crontab, has been changed
to specify the user-id that should be used to run a process.
The userid ``nobody'' is frequently useful for non-privileged programs.
.PP
Some of the commands previously in /etc/rc.local have been 
moved to /etc/rc;
several new functions are now handled by /etc/rc.local.
You should look closely at the prototype version of /etc/rc.local
and read the manual pages for the commands contained in it
before trying to merge your local copy.
Note in particular that \fIifconfig\fP has had many changes,
and that host names are now fully specified as domain-style names
(e.g, monet.Berkeley.EDU).
.PP
The C library and system binaries on the distribution tape
are compiled with versions of
\fIgethostbyname\fP and \fIgethostbyaddr\fP which use
ndbm host table lookup routines instead of the name server.
You must run \fImkhosts\fP\|(8) to create the \fIndbm\fP
host table database from \fI/etc/hosts\fP.
It's not clear that the name server could ever be made to run
on a PDP.  A copy of the 4.3BSD name server is provided in
/usr/src/etc/PORT/named.tar.Z for those brave souls with a
masochistic bent.
.PP
If you want to compile your system to use the
name server lookup routines instead of the ndbm host table, you will
need to modify /usr/src/lib/libc/Makefile according to the instructions there
and then recompile all of the system and local programs (see section 6.5).
.PP
The format of /etc/ttys has changed, see \fIttys\fP\|(5)
for details.
It now includes the terminal type and security options that were previously
placed in /etc/ttytype and /etc/securettys.
.PP
There is a new version of \fIsyslog\fP that uses a more generalized
facility/priority scheme.
This has changed the format of the syslog.conf file.
See \fIsyslogd\fP\|(8) for details.
It is used by many of the system daemons
to monitor system problems more closely, for example
network routing changes.
.PP
If you are using the name server, 
your \fIsendmail\fP configuration file will need some minor
updates to accommodate it.
See the ``Sendmail Installation and Operation Guide'' and the sample
\fIsendmail\fP configuration files in /usr/src/usr.lib/sendmail/nscf.
Be sure to regenerate your sendmail frozen configuration file after
installation of your updated configuration file.
Again, it must be emphasized that it is probably an impossible task to
port the 4.3BSD name server to the PDP.  At the very least a more recent
distribution than the standard 4.3BSD distribution should be obtained since
several ``memory leaks'' have been sealed in recently distributed patches.
.PP
The spooling directories saved on tape may be restored in their
eventual resting places without too much concern.  Be sure to
use the `p' option to \fItar\fP so that files are recreated with the
same file modes:
.DS
\fB#\fP cd /usr
\fB#\fP tar xp msgs spool/mail spool/uucp spool/uucppublic spool/news
.DE
.PP
The ownership and modes of two of these directories
\fIat\fP now runs set-user-id ``daemon'' instead of root.
Also, the uucp directory no longer needs to be publicly writable,
as \fItip\fP reverts to privileged status to remove its lock files.
After copying your version of /usr/spool, you should do:
.DS
\fB#\fP chown \-R daemon /usr/spool/at
\fB#\fP chown \-R root /usr/spool/uucp
\fB#\fP chgrp \-R daemon /usr/spool/uucp
\fB#\fP chmod \-R o\-w /usr/spool/uucp
.DE
.PP
Whatever else is left is likely to be site specific or require
careful scrutiny before placing in its eventual resting place.
Refer to the documentation and source code 
before arbitrarily overwriting a file.
.NH 2
Hints on converting from previous PDP UNIX systems to \*(2B
.PP
This section summarizes some of the significant changes in \*(2B
from previous PDP UNIX systems.
It does not include changes in the network;
see chapter 5 for information on setting up the network.
.PP
The mailbox locking protocol has changed;
it now uses the advisory locking facility to avoid concurrent
update of users' mail boxes.
If you have your own mail interface, be sure to update its locking protocol.
.PP
The kernel's limit on the number of open files has been
increased from 20 to 30.  It is now possible to change this limit almost
arbitrarily (there used to be a hard limit of 20).  The standard I/O library
autoconfigures to the kernel limit.  Note that file (``_iob'') entries may
be allocated by \fImalloc\fP from \fIfopen\fP; this allocation has been
known to cause problems with programs that use their own memory allocators.
This does not occur until after 20 files have been opened
by the standard I/O library.
.PP
\fISelect\fP can be used with at most 32 descriptors although this limit
probably wouldn't require an amazing amount of effort to change since
select uses arrays of \fBlong\fPs for the bit fields rather than single
\fBlong\fPs.  Programs that used \fIgetdtablesize\fP as their first
argument to \fIselect\fP will no longer work correctly.  Usually the
program can be modified to correctly specify the number of bits in a
\fBlong\fP.
See
.IR select (2).
.PP
Old core files will not be intelligible by the current debuggers
because of numerous changes to the user structure
and because the kernel stack has been enlarged.
The \fIa.out\fP header that was in the user structure is no longer present.
Locally-written debuggers that try to check the magic number
will need modification.
.PP
\fIFind\fP now has a database of file names,
constructed once a week from \fIcron\fP.
To find a file by name only,
the command \fIfind name\fP will look in the database for
files that match the name.  This is much faster than
\fIfind / \-name name \-print\fP.
.PP
Files may not be deleted from directories having the ``sticky'' (ISVTX) bit
set in their modes
except by the owner of the file or of the directory, or by the superuser.
This is primarily to protect users' files in publicly-writable directories
such as \fI/tmp\fP and \fI/usr/tmp\fP.
All publicly-writable directories should have their ``sticky'' bits set
with ``chmod +t.''
.PP
The include file \fI<time.h>\fP has returned to \fI/usr/include\fP,
and again contains the definitions for the C library time routines of
\fIctime\fP\|(3).  Note, once your system is installed and running, you
should make sure that you recompile and reinstall the directory
\fIusr/src/etc/tzone\fP.  Read through the Makefile first, if you're
not located on the West Coast you will have to change it.  This directory
is an addition since 4.3BSD, and is intended to solve the Daylight
Savings Time problems once and for all.
.PP
The \fIcompact\fP and \fIuncompact\fP programs have been supplanted
by the faster \fIcompress\fP.
If your user population has \fIcompact\fPed files, you will want
to install \fIuncompact\fP found in /usr/src/old/compact.
.PP
And again, \fB\*(2B is not binary compatible with any previous PDP UNIX
system\fP.
.PP
If you want to use \fIps\fP after booting a new kernel,
and before going multiuser, you must initialize its name list
database by running \fIps \-U\fP.
