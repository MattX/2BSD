.if n .nr LL 7i
.TL
Second\ Distribution\ of\ Berkeley\ PDP\(hy11\ Software\ for\ UNIX\(dg
(revised March 1980)
.sp
Installation\ Instructions
.PP
.FS
\(dg UNIX is a trademark of Bell Laboratories.
.FE
By following the directions here you
should be able to bring up the software on the tape in a very short
period of time, ranging from an hour (if you have a standard version 6 or 7
system and can use the precompiled binaries), to about 6 hours (if you
have a version 6 or 7 system which requires recompilation.)
.SH
Extracting the tape
.PP
This tape has been written using the new
.I tar
tape achiver program, blocked at 20.
If you don't have
.I tar,
you can extract a binary of the program from the tape by giving the following commands:
.DS
. % dd if=/dev/mt0 bs=1b skip=1 count=49 of=tar
% dd if=/dev/rmt0 bs=20b count=3 of=tmp1
% dd if=tmp1 bs=1b skip=1 count=49 of=tar
% chmod 755 tar
.DE
Then, on either version 6 or 7,
you can extract the tape with the (single) command
.DS
% tar xf /dev/rmt0
.DE
This will use ``mkdir'' to make directories as needed.
A manual page for
.I tar
is attached.
.PP
Note: it is normal for tar to complain about not being able to create files.  
All files it complains about should have names ending in slash.
This tar tape was written with a new version of tar that places directory
modes on the tape.
Older versions of tar (including the v6 binary on this tape) will complain
about the directory entries and continue on, making the directories with
a default mode and owner.
.SH
Kernel Tape
.PP
If you have a tape with a v7 kernel on it,
the tape is a boot tape.
Instructions for booting the system from the boot tape are
enclosed separately.
If you are just interested in extracting the tar portion of
the tape,
you must skip 8 files at the beginning of the tape first. Try
``dd files=8 if=/dev/nrmt0 of=/dev/null''
before the tar command to extract the tape.
.SH
Installation Preparation
.PP
The first thing to determine is which version of \s-2UNIX\s0 you are running,
and how much impact modifications you have made to the system will have
on the software here.  
.SH
Version 7
.PP
This is the current version of \s-2UNIX\s0 from Bell Laboratories.
Most of the software here is running on version 7 at Berkeley.
.PP
Version 7 binaries, taken from our Cory Hall system, can be found
in the subdirectory bin.v7.  Thus, it will not be necessary to
recompile if you have a standard v7 PDP-11 system.
Most of the sources can easily be recompiled on other v7 systems,
such as 32V, 4BSD, or Unix for other kinds of hardware.
The only code that depends heavily on the PDP-11 is the Pascal
interpreter, which is largely written in PDP-11 assembly language.
.PP
Some of the older programs have been discontinued from later distributions
and as such may need to have the #include <retrofit.h> line removed for v7.
The include file
.B retrofit.h ,
should be included on v6 systems and should not be included on v7 systems.
Any include files you are missing can usually be found by looking in
.B upgrade/include .
.SH
Version 6
.PP
If you have a standard version 6 \s-2UNIX\s0 system then you can just use
the binaries on the tape and avoid the bother of recompiling.  In fact,
unless you have a late-model C compiler
compilation may be troublesome or impossible.
Note that the v6 binaries on this tape are fairly old, mostly from
the original 2BSD in April, 1979.
The
.I ex
and
.I csh
binaries are current.
Others will require recompilation if you want a more current program.
Since we don't run v6 at Berkeley any more,
these programs have not been recently tested on v6 and may require
a little work.
The most common problem is the include file
.B retrofit.h ,
which should be included on v6 systems and should not be included
on v7 systems.
Some of the older programs have been discontinued from later distributions
and as such may need to have the #include <retrofit.h> line removed for v7.
.PP
On a ``standard'' system the \fIgetuid\fR system call returns
the user id in the low byte of its result word.  If this is
the case on your system, then you should have no trouble installing the
binaries supplied.\(dg
.FS
\(dg I have compiled the code here with a library which uses ``sys sleep''
to implement \fIsleep()\fR, rather than the later, more efficient sleep
implementation using new system calls pause and alarm.  If you have the
latter, you can recompile programs which sleep if you wish.
.FE
.SH
Other Version 6
.PP
If you have a version 6 \s-2UNIX\s0 system which has 16 bit user id's (such as
the old systems at Berkeley)
then you will have to modify the \fBupgrade/libretro\fR
version 7 simulator library and recompile the programs here.
.SH
PWB and Unix/TS
.PP
If you have a \s-2PWB\s0/\s-2UNIX\s0 version 1 system,
the binaries supplied here should work
(as far as I know).
If they don't the \fImake\fR and \fIcc\fR from \s-2PWB\s0
should be adequate to recompile to repair any problems.
.PP
A known problem with PWB 1.0 is that tar will try to
give away the directories it creates, and PWB will let it.
This will be annoying if you are not a super user, since
you will be unable to remove the directories it builds.
.PP
Another known problem is that there is no group field in the password file.
The precompiled ex v6 binary will look for your home directory in
/etc/htmp.  If this fails it will look in /etc/passwd using the routine
.I getpwuid .
The binary expects an extra field, and will find your shell instead
of your home directory.
As a result, your
.I .exrc
file will not be found.
There are three solutions here.
One is to fix your
.I login
program to update /etc/htmp whenever anyone logs in.
This is the preferred solution because it applies to everyone
and is most efficient.
Another solution is for each use to place a
.I sethome
command in their
.I .login
file to be run when they log in.
A third solution is to recompile ex to use the getpwuid routine
on the \s-2PWB\s0 system,
which will work but make entry into the editor slower due to the
search of the password file.
.PP
If you have a UNIX 3.0 system
you can treat the programs as v7 for the most part.
One exception is the teletype driver which is different.
This will make it necessary to recompile the editor using
the version 7 makefile.
(It may be necessary to add the option
.B -DUSG
or
.B -DUSG3TTY
to the CFLAGS in makefiles if on one of these systems.)
.SH
New files to be added.
.PP
The following are the major files and directories
which will be created as you install the tape:
.de BP
.IP \fB\\$1\fR 28
..
.BP /bin/csh
This is the Berkeley shell.  It is not placed in
the directory \fB/usr/ucb\fR because it is often linked to \fB/bin/makesh\fR,
which is on a different file system than \fB/usr/ucb\fR on
most systems.  It also thinks it is /bin/csh, so it will need to be
recompiled if it is located elsewhere.
(Note: the C shell is not intended to be installed as
.B /bin/sh .
Completely throwing away the v7 shell would break several commands,
such as
.B lint ,
which are Bourne shell scripts,
as well as foster incompatibility with the rest of the world.)
.BP /etc/htmp
For version 6 systems, this forms a data base which simulates
version 7 environments, storing home directories and (most
importantly) terminal types for each terminal.
This file is not needed on version 7 or other systems with environments.
.BP /etc/termcap
This is a data base describing terminals, and is used by the \fIex\fR editor,
the \fItset\fR program, and other programs.
.BP /etc/ttytype
This file maps terminal ports to their types, and indicates which
ports are not hard wired.  The
.I tset
program uses this to initialize the terminal type at login.
Note that the format of this file differs on v6 from v7.
.BP /usr/include/retrofit
On version 6 systems, a directory of header files used to simulate version
7 \s-2UNIX\s0.
.BP /usr/lib/Mail.help*
Help files for \fIMail\fR.
.BP /usr/lib/Mail.rc
A startup file for \fIMail\fR.
This file is useful for arranging for mail to be forwarded.
.BP /usr/lib/ex2.*preserve
Preserve command for \fIex\fR.
.BP /usr/lib/ex2.*recover
Recover command for \fIex\fR.
.BP /usr/lib/ex2.*strings
Error messages for \fIex\fR.
.BP /usr/lib/how_p*
Help files for Pascal.
.BP /usr/lib/libretro.a
Library simulating some version 7 calls on version 6.
.BP /usr/lib/libtermlib.a
Library providing terminal independent functions.
.BP /usr/lib/libcurses.a
Screen update library making the cursor motion optimizations
used by vi available to other programs.
.BP /usr/lib/me
[Directory]
The dynamically loaded parts of the \fI\-me\fR macros are placed here.
.BP /usr/lib/pi1.2strings
Error messages for \fIpi\fR, the Pascal translator.
.BP /usr/lib/pi1:2strings
Messages for two process \fIpi\fR for 34's and 40's.
.BP /usr/lib/pi1
Second pass of two process \fIpi\fR translator.
.BP /usr/lib/px_header
Header files which \fIpi\fR prepends to \fIobj\fR files.
.BP /usr/lib/tabset
[Directory] Terminal initialization files for \fItset\fR.
.BP /usr/lib/tmac.e
The \fI\-me\fR macros themselves, on version 6 systems.
.BP /usr/lib/tmac/tmac.e
The \fI\-me\fR macros themselves, on version 7 systems.
.BP /usr/msgs
[Directory]
The \fImsgs\fR program places messages here.
.BP /usr/preserve
[Directory]
Editor temporaries are preserved here after system crashes.
.BP /usr/ucb
[Directory]
Most of the binaries on the tape are placed here.
They can be linked elsewhere (i.e. \fB/usr/bin\fR) but
the makefiles which create the tape software expect
them in \fB/usr/ucb\fR so they should be left there also.
.SH
Installation procedure.
.PP
Now follow the following procedure:
.IP 1.
Chdir to either
.B bin.v6
or
.B bin.v7
as appropriate,
and run the
.B install
script.
.IP 2.
If you have a version 7 system, skip to step 5.
If you have a version 6 system then run the \fBinstall\fR script in the
directory \fBupgrade/include\fR to put a copy of the retrofitting header
files in \fB/usr/include/retrofit\fR.
.IP 3.
If you have a standard version 6 system (with 8 bit user id's) then
run the \fBinstall\fR script in the directory
\fBbin\fR on the tape.  Then skip to step 6.
.IP 4.
If you have a non-standard version 6 system which uses 16 bit user-id's
or has other modifications which would destroy binary compatibility,
then:
.RS
.IP a.
Look at the retrofit library source directory \fBupgrade/libretro\fR and make
needed changes.  Recreate the library and install it.
If you have \fImake\fR you can use the \fBmakefile\fR; otherwise
use your shell with \fBmake.script\fR.
.IP b.
Recompile the termlib library \fBsrc/termlib\fR, using \fBmakefile.v6\fR
and ``make install'', or the shell script \fBmake.script\fR
if you don't have \fImake\fR.
.IP c.
Recompile the programs in \fBupgrade/src\fR using \fBmake.script\fR
or \fBmakefile\fR.  These are versions
of some programs in \fBsrc\fR which are different for version 6.
.IP d.
Follow the rest of the instructions for making a version 7
compilation, using \fBmakefile.v6\fR or \fBmake.script\fR whenever
they exist rather than \fBmakefile\fR.
(You can skip part \fIa\fR since you have done it already.)
.RE
.IP 5.
If you have a version 7 system:
.RS
.IP a.
Run \fImake\fR in \fBsrc/termlib\fR, since this makes an important
library which you will need right away.
.IP b.
Then start in the \fBsrc\fR directory, and run \fImake\fR there and
then in each subdirectory (see below).  Look at the \fBREAD_ME\fR files in
each directory to get an idea of what is going on.  After
creating the binaries ``make install'' will install them in \fB/usr/ucb\fR.
Some makefiles also install things in \fB/usr/lib/\fR or \fB/etc\fR; use
``make \-n'' to see what \fImake\fR will do.
.IP
The following is a reasonable order to do the subdirectories in:
(omitting Pascal for now):
.DS
Mail, csh, ex, me
.DE
.IP c.
Install the Pascal system.
There are two copies of Pascal, one for v6 and one for v7.
They turned out to be different versions (1.1 and 1.2) but
appear substantially the same to the user.  Files which are
identical in both versions are linked together.
Precompiled binaries are provided for all v6 systems, and for
v7 systems with split I/D and hardware floating point.
Pascal has not been tested on v7 systems without split I/D or
without floating point.
It has recently been brought up on a system without split I/D,
but that version is not yet included in this distribution.
.IP
It is not necessary to
compile \fBeyacc\fR or to run \fIeyacc\fR in the \fBpi\fR and
\fBpxp\fR directories; rather just use the supplied \fBy.tab.c\fR files.
(The supplied \fBmakefile\fRs don't run \fIeyacc\fR.)
.IP d.
Now prepare the utilities for the Pascal system in the
directory \fBpascal\fR.  Then prepare the Pascal translator \fBpi\fR,
the interpreter \fBpx\fR and, finally, the profiler \fBpxp\fR.
.IP
If you have a non-separate I/D machine, or if you do not have
hardware floating point, then prepare \fBpi0\fR and
\fBpi1\fR rather than \fBpi\fR, and use the \fBpx34\fR and \fBpxp34\fR
(NOID) versions of \fIpx\fR and \fIpxp\fR.\(dg
.FS
\(dg You can run \fIpi\fR (instead of \fIpi34\fR from \fBpi0\fR and \fBpi1\fR)
on a non-floating point machine with separate
I/D by adding a system call to simulate a mfpi instruction
(see \fBmisc/fetchi.sys\fR).
This \fIpi\fR will run slightly faster, and allow slightly larger programs
to be written.
.FE
You should, on these machines:
.DS
mv /usr/ucb/pi34 /usr/ucb/pi
mv /usr/ucb/px34 /usr/ucb/px
mv /usr/ucb/pxp34 /usr/ucb/pxp
.DE
.RE
.IP 6.
Install the manual sections in \fBman\fR copying them to \fB/usr/man/manu\fR.
If you have version 6, follow the instructions in \fBupgrade/man\fR
on adapting to the different manual macros used.
Note that it is quite reasonable to install the manual pages in section
1 instead of section u.  A few of the manual pages belong in other
sections, such as 3 and 5.
.IP 7.
Add a line of the form
.DS
/usr/lib/ex2.13preserve \-a
.DE
to the file \fB/etc/rc\fR, before it cleans files out of \fB/tmp\fR.  This
will preserve the editor temporaries from \fB/tmp\fR after system crashes,
and implements the editor crash recovery mechanism.\(dg
.FS
\(dg If \fB/usr\fR is a mounted filesystem, be sure it is mounted before
you try to run \fB/usr/lib/ex2.13preserve\fR.
.FE
.IP 7.
So that the \fImsgs\fR program can receive messages which are sent via
.B mail
change, change your mail program to execute
``/usr/ucb/msgs \-s''
with the message on the standard input whenever mail is sent to ``msgs''.
A version 6
.B mail
program which does this is in
``mail.c''
in the directory \fBmisc\fR.
Note: If you choose to install delivermail, msgs can be implemented using
an alias in the delivermail aliases file rather than as a special case
in mail.
.IP 8.
Make sure that the programs \fB/usr/lib/ex2.13preserve\fR and
\fB/usr/lib/ex2.13recover\fR can write the directory \fB/usr/preserve\fR.
For security, these programs should be owned by ``root'', mode 4755,
and the directory \fB/usr/preserve\fR should be mode 755.
.IP
The programs \fB/usr/ucb/setenv\fR and \fB/usr/ucb/tset\fR must
be able to write \fB/etc/htmp\fR.
(Version 6 only.)
It is wise to have \fB/etc/htmp\fR
mode 644 and \fBsethome\fR and \fBttytype\fR mode 4755 to a user who
owns \fB/etc/htmp\fR (this doesn't have to be ``root'', but it can).
The
.B sethome
code should probably be installed into your
.I login
program, along with code from
.B tset
to look in
.I /etc/ttytype
to set the terminal type.
.IP
The programs \fB/usr/ucb/chsh\fP and \fB/usr/ucb/chfn\fP need to be
able to write on /etc/passwd.  These programs allow a user to change
his shell or gcos entry without bothering a super user.
.IP 9.
Initialize the \fB/etc/ttytype\fR data base with the types of the terminals
on your system.  The file contains one line per terminal.  On version
6, each line has the (one character) terminal name, and then a 2 character
code.  On version 7 each line has a two character code, a space, and
then the (arbitrary length) terminal name.  See \fBmisc/ttytype.v6\fR
and \fBmisc/ttytype\fR for samples.  The codes are defined by the file
\fB/etc/termcap\fR.
.IP 10.
Initialize the Mail file \fB/usr/lib/Mail.rc\fR defining any \fIalias\fR
groups for distribution of mail you wish.  A line of the form
.DS
alias staff bill kurt eric
.DE
will cause ``Mail staff'' to send copies to \fIbill\fR, \fIkurt\fR, and
\fIeric\fR.
.SH
Software not installed by the above procedure
.PP
The modifications to the standard i/o library \fBsrc/libNS\fR,
the mail forwarding system
.B delivermail ,
the data base management system
.B ingres ,
the version 7 kernel,
the Berkeley network \fBsrc/net\fR, and the
\fIfinger\fR program \fBsrc/finger.c\fR
are not installed by
the above procedure.
.PP
The standard I/O library modifications may require some care to make
as several slightly different versions of this library are extant.
See the \fBREAD_ME\fR file in the \fBsrc/libNS\fR directory.
.PP
See the READ_ME file in the
.B ingres
directory for information
and installation instructions.
.PP
See the READ_ME file in the
.B kernel
directory for information and installation instructions.
.PP
Delivermail is a program to take care of mail forwarding.
It also handles shipping of mail to other sites on various
networks (berknet, uucp net, arpanet).
Installing it requires modifications to the standard mail program
.B /bin/mail
so that all mail is passed to delivermail.
Sample versions of
.B /bin/mail
for v6, v7, and vax systems are included.
Note that if you run delivermail, not only does
.B /bin/mail
have to know about it,
but compilation flags must be set in the berknet
and Mail programs as well.
.PP
If you wish to run the Berkeley network, read the material in the
\fBsrc/net\fR directory.  The network is not hard to set up, but
this will require a bit of preparation.
.PP
The \fIfinger\fR program requires preparation of some data bases,
and perhaps modifications to the \fIlogin\fR program as well as to
\fIfinger\fR itself to work.  See the comments at the beginning
of the program \fBsrc/finger.c\fR and its manual page for details.
.SH
Problems you may encounter (Version 6 only)
.IP 1.
Recompiling the editor will overflow the standard compiler symbol table.
See \fBupgrade/c\fR for instructions on a trivial change to make a C compiler
with a bigger symbol table, which you can make available via the \fB\-t0\fR
flag to \fBcc\fR.  Some scripts on the tape also reference a \fB\-t1\fR version
of the C compiler, which puts \fIswitch\fR statement code out as instructions
rather than as data.  This makes for programs with larger text spaces but
smaller per-user data.  See \fBupgrade/c\fR for the C compiler change which
implements this.
.IP 2.
If you use the binaries on the tape, some will print times in Pacific time.
They will work in your time zone if you recompile them.
.IP 3.
\fICsh\fR uses an \fIaccess\fR system call which is not part of a
bare version 6 system.  Its manual page and C interface are in the
directory \fBmisc\fR, as well as a file \fBaccess.sys\fR containing information on how
to add it to your system.  The \fIaccess\fR call is in later version 6
and version 7 systems.
.IP 4.
If you don't get mail in the file ``.mail'' in your login directory (v6)
or in ``/usr/spool/mail/yourname'' (v7)
you'll have to finagle the \fIMail\fR
program to know where you do.  Look at its \fBlocal.c\fR and \fBlocal.h\fR
files.  The \fIfrom\fR program in \fBupgrade/src\fR
will also have to be changed.
.IP 5.
If you have changed the \fItimes\fR system call as per 50
changes, returning long integers for \fIproc_user_time\fR and
\fIproc_system_time\fR, the supplied \fIcsh\fR binary will
dump when it calls \fItimes\fR.  You'll have to change some declarations
in the shell and recompile.
.IP 6.
The \fB:sh\fR command of the editor will fork off a shell with the \fB\-i\fR
option.  If your /bin/sh does not have a \fB\-i\fR option, it may complain.
The \fB\-i\fR option to \fBcsh\fR and
the version 7 shell says ``be interactive'',
which is the default with the version 6 shell unless you have modified it.
The easiest solution is to install a \fB\-i\fR option that doesn't
do anything.
.SH
Maintenance
.PP
.BP /etc/termcap
If you get have or get terminals which aren't described
in this data base, you will have to add entries.
The manual page for \fItermcap\fR explains how to write new entries.
Note that there is a source directory for termcap, and the source
file
.B termcap.src
should be edited rather than the object file /etc/termcap.
If you wish to get future updates, you should edit the file
.B reorder
in the termcap source directory rather than
.B termcap.src .
We are interested to hear of terminals you add so we can
install them in the master copy.
.BP /etc/ttytype
This file tells the types of hardwired ports and which
lines are dialups, and is used with \fItset\fR.  It must
be edited when the system configuration changes.
.BP /usr/preserve
Editor temporaries are saved here after a system crash,
when /usr/lib/ex2.0preserve is run out of /etc/rc.
If no one cleans this directory out, it can get very
large.  You can periodically run a find command of the form
.DS
find /usr/preserve \-mtime +7 \-a \-exec rm \-f {} \e;
.DE
to clean out old junk.  It is usefully run by the daemon \fIcron\fR.
.BP /usr/msgs
Must be cleaned out periodically (every few months).
.BP other
New software is often added to 2BSD.
Some of it is not described in this document.
You should look through the heirarchy of the tape
to see what else exists.
Unless you have both v6 and v7 systems,
you will want to remove some of the subdirectories for the
kind of system you don't have in order to save space.
