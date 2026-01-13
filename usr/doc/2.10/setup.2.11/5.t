.\" Copyright (c) 1980 Regents of the University of California.
.\" All rights reserved.  The Berkeley software License Agreement
.\" specifies the terms and conditions for redistribution.
.\"
.\"	@(#)5.t	2.6 (2.11BSD) 2025/12/10
.\"
.ds lq ``
.ds rq ''
.ds LH "Installing/Operating \*(2B
.ds RH Network setup
.ds CF \*(DY
.LP
.nr H1 5
.nr H2 0
.bp
.LG
.B
.ce
5. NETWORK SETUP
.sp 2
.R
.NL
.PP
The ``correct''
use of the networking in \*(2B is probably with a list of the local net
addresses in the \fI/etc/hosts\fP file and a default route to a gateway for
all network traffic.  In particular, do not run
.IR routed (8)
unless you're extremely sure that you know what you're doing.  This is
doubly true if SL/IP is being used as the primary connection to the
outside world.
.PP
The networking in \*(2B, runs in supervisor
mode separate from the mainstream kernel.  There is room without overlaying
to hold both a SL/IP and ethernet driver.
The networking portion of the kernel resides in ``/netnix'', and is
loaded after the kernel is running.  Since the kernel only looks for the
file ``/netnix'', it will not run if it is unable to load ``/netnix''.
\fBNOTE\fP:  The ``/unix'' and ``/netnix'' imagines must
have been created at the same time, do not attempt to use mismatched
images.
.PP
\*(2B provides support for the DARPA standard Internet
protocols IP, ICMP, TCP, and UDP.
Network services are split between the
kernel (communication protocols) and user programs (user
services such as TELNET and FTP).  This section describes
how to configure your system to use the Internet networking support.
.NH 2
System configuration
.PP
To configure the kernel to include the Internet communication
protocols, define the INET option.  This automatically defines
the NLOOP option.
Include the pseudo-device ``pty'' in your machine's configuration
file using the NPTY options.
The ``pty'' pseudo-device forces the pseudo terminal device driver
to be configured into the system, see \fIpty\fP\|(4).  The NLOOP
option forces inclusion of the software loopback interface driver.
The loop driver is used in network testing as well as for the
system talking to itself rather than transmitting the data over the wire.
.PP
If you are planning to use the Internet network facilities on a 10Mb/s
Ethernet, the pseudo-device ``ether'' should also be included
in the configuration using the NETHER option; this forces inclusion of
the Address Resolution Protocol module used in mapping between 48-bit
Ethernet and 32-bit Internet addresses.
.PP
Before configuring the appropriate networking hardware, you should
consult the manual pages in section 4 of the Programmer's Manual.
The following table lists the devices for which software support exists.
.DS
.TS
l l.
Device name	Manufacturer and product
_
de	DEC DEUNA/DELUA 10Mb/s Ethernet
qe	DEC DEQNA 10Mb/s Ethernet
qt	DEC DELQA-YM 10Mb/s Ethernet
ec	3Com 10Mb/s Ethernet
il	Interlan 1010 and 10101A 10Mb/s Ethernet interfaces
vv	Proteon ProNET - Token Ring Interface
acc	LH/DH-11 1822 IMP/PSN Interface
.TE
.DE
.PP
SL/IP is also available.  It is surprisingly efficient.  Over a 9600 baud
line it is not unusual to see \fBftp\fP rates in the 800 bytes per second
range.
.PP
All network interface drivers including the loopback interface,
require that their host address(es) be defined at boot time.
This is done with
.IR ifconfig (8)
commands included in the \fI/etc/rc.local\fP file.
Interfaces that are able to dynamically deduce the host
part of an address may check that the host part of the address is correct.
The manual page for each network interface
describes the method used to establish a host's address.
.IR Ifconfig (8)
can also be used to set options for the interface at boot time.
Options are set independently for each interface, and
apply to all packets sent using that interface.
These options include disabling the use of the Address Resolution Protocol;
this may be useful if a network is shared with hosts running software
that does not yet provide this function.
Alternatively, translations for such hosts may be set in advance
or ``published'' by a \*(2B host by use of the
.IR arp (8)
command.
.PP
To use the pseudo terminals just configured, device
entries must be created in the ``/dev'' directory.  To create 32
pseudo terminals (plenty, you can probably get by with many less)
execute the following commands.
.DS
\fB#\fP cd /dev
\fB#\fP MAKEDEV pty0 pty1
.DE
More pseudo terminals may be made by specifying \fIpty2\fP, \fIpty3\fP,
etc.  The kernel normally includes support for 16 pseudo terminals
unless the configuration file specifies a different number.
Each pseudo terminal really consists of two files in /dev:
a master and a slave.  The master pseudo terminal file is named
/dev/ptyp?, while the slave side is /dev/ttyp?.  Pseudo terminals
are also used by several programs not related to the network.  \fBNOTE\fP:
the terminal structures are 78 bytes each, declaring more than 16 pseudo
terminals is potentially wasteful of kernel D space.  See the comment
in the kernel config files.
In addition to creating the pseudo terminals,
be sure to install them in the
.I /etc/ttys
file (with a `none' in the second column so no
.I getty
is started).
.NH 2
Local subnetworks
.PP
In \*(2B the DARPA Internet support
includes the notion of ``subnetworks''.  This is a mechanism
by which multiple local networks may appears as a single Internet
network to off-site hosts.  Subnetworks are useful because
they allow a site to hide their local topology, requiring only a single
route in external gateways;
it also means that local network numbers may be locally administered.
The standard describing this change in Internet addressing is RFC-950.
.PP
To set up local subnetworks one must first decide how the available
address space (the Internet ``host part'' of the 32-bit address)
is to be partitioned.
Sites with a class A network
number have a 24-bit address space with which to work,
sites with a class B network number have a 16-bit address space,
while sites with a class C network number have an 8-bit address space.
To define local subnets you must steal some bits
from the local host address space for use in extending the network
portion of the Internet address.  This reinterpretation of Internet
addresses is done only for local networks; i.e. it is not visible
to hosts off-site.  For example, if your site has a class B network
number, hosts on this network have an Internet address that contains
the network number, 16 bits, and the host number, another
16 bits.  To define 254 local subnets, each
possessing at most 255 hosts, 8 bits may be taken from the local part.
(The use of subnets 0 and all-1's, 255 in this example, is discouraged
to avoid confusion about broadcast addresses.)
These new network
numbers are then constructed by concatenating the original 16-bit network
number with the extra 8 bits containing the local subnetwork number.
.PP
The existence of local subnetworks is communicated to the system at the time a
network interface is configured with the
.I netmask
option to the
.I ifconfig
program.  A ``network mask'' is specified to define the
portion of the Internet address that is to be considered the network part
for that network.
This mask normally contains the bits corresponding to the standard
network part as well as the portion of the local part
that has been assigned to subnets.
If no mask is specified when the address is set,
it will be set according to the class of the network.
For hosts with multiple interfaces, the network mask should
be set for each interface,
although in practice only the mask of the first interface on each network
is actually used.
.NH 2
Internet broadcast addresses
.PP
The address defined as the broadcast address for Internet networks
according to RFC-919 is the address with a host part of all 1's.
The address used by 4.2BSD was the address with a host part of 0.
\*(2B uses the standard broadcast address (all 1's) by default,
but allows the broadcast address to be set (with \fIifconfig\fP)
for each interface.
This allows networks consisting of both 4.2BSD and \*(2B hosts
to coexist.
In the presence of subnets, the broadcast address uses the subnet field
as for normal host addresses, with the remaining host part set to 1's
(or 0's, on a network that has not yet been converted).
\*(2B hosts recognize and accept packets
sent to the logical-network broadcast address as well as those sent
to the subnet broadcast address, and when using an all-1's broadcast,
also recognize and receive packets sent to host 0 as a broadcast.
.NH 2
Routing
.PP
If your environment allows access to networks not directly
attached to your host you will need to set a default route to a gateway.
Do \fBNOT\fP attempt to run \fIrouted\fP on a \*(2B system.
Define a default or wildcard route to a smart
gateway and depend on the gateway to provide ICMP routing
redirect information.  This is done by adding an entry of the form
.DS
route add default \fIsmart-gateway\fP 1
.DE
to \fI/etc/rc.local\fP; see
.IR route (8)
for more information.  The default route
will be used by the system as a ``last resort''
in routing packets to their destination.
.PP
The system always listens, and processes, routing redirect
information.
The
.IR netstat (1)
program may be used to display routing table contents as well
as various routing oriented statistics.  For example,
.DS
\fB#\fP\|netstat \-r
.DE
will display the contents of the routing tables, while
.DS
\fB#\fP\|netstat \-r \-s
.DE
will show the number of routing table entries dynamically
created as a result of routing redirect messages, etc.
.NH 2
Use of \*(2B machines as gateways
.PP
Only sheer insanity could prompt the use of \*(2B machines as gateways.
If you \fBreally\fP want to do this then the best recourse is to
prowl the sources and see what has to be done.  The code is all there,
and the "ipforwarding" variable is present.
.NH 2
Network servers
.PP
In \*(2B most of the server programs are started up by a
``super server'', the Internet daemon.  The Internet
daemon, \fIinetd\fP, acts as a master server for
programs specified in its configuration file, \fI/etc/inetd.conf\fP,
listening for service requests for these servers, and starting
up the appropriate program whenever a request is received.
The configuration file contains lines containing a service
name (as found in \fI/etc/services\fP), the type of socket the
server expects (e.g. stream or dgram), the protocol to be
used with the socket (as found in \fI/etc/protocols\fP), whether
to wait for each server to complete before starting up another,
the user name as which the server should run, the server
program's name, and at most five arguments to pass to the
server program.
Some trivial services are implemented internally in \fIinetd\fP,
and their servers are listed as ``internal.''
For example, an entry for the file
transfer protocol server would appear as
.DS
ftp	stream	tcp	nowait	root	/usr/libexec/ftpd	ftpd -l
.DE
or if you are using the \fItcp_wrapper\fP program as
.DS
ftp	stream	tcp	nowait	root	/usr/libexec/tcpd	ftpd -l
.DE
Consult
.IR inetd (8)
for more detail on the format of the configuration file
and the operation of the Internet daemon.
.NH 2
Network data bases
.PP
Several data files are used by the network library routines
and server programs.  Most of these files are host independent
and updated only rarely.
.DS
.TS
l l l.
File	Manual reference	Use
_
/etc/hosts	\fIhosts\fP\|(5)	host names
/etc/networks	\fInetworks\fP\|(5)	network names
/etc/services	\fIservices\fP\|(5)	list of known services
/etc/protocols	\fIprotocols\fP\|(5)	protocol names
/etc/hosts.equiv	\fIrshd\fP\|(8)	list of ``trusted'' hosts
/etc/rc.local	\fIrc\fP\|(8)	command script for starting servers
/etc/ftpusers	\fIftpd\fP\|(8)	list of ``unwelcome'' ftp users
/etc/hosts.lpd	\fIlpd\fP\|(8)	list of hosts allowed to access printers
/etc/inetd.conf	\fIinetd\fP\|(8)	list of servers started by \fIinetd\fP
.TE
.DE
The files distributed are set up for Internet hosts.
Local networks and hosts should be added to describe the local
configuration.
Network numbers will have to be chosen for each Ethernet.
For sites not connected to the Internet,
these can be chosen more or less arbitrarily,
otherwise the normal channels should be used for allocation of network
numbers.
.NH 3
Use of /etc/hosts
.PP
When using the host address routines that use the Internet name server,
the file \fI/etc/hosts\fP is only used for setting interface addresses
and at other times that the server is not running,
and therefore it need only contain addresses for local hosts.
.PP
If you are connected to the Internet, it is highly recommended that
you use the name server resolver routines for your host name and address
mapping, as this provides access to a much larger set of hosts than are
provided in the host table.
.NH 3
/etc/hosts.equiv
.PP
The remote login and shell servers use an
authentication scheme based on trusted hosts.  The \fIhosts.equiv\fP
file contains a list of hosts that are considered trusted
and, under a single administrative control.  When a user
contacts a remote login or shell server requesting service,
the client process passes the user's name and the official
name of the host on which the client is located.  In the simple
case, if the host's name is located in \fIhosts.equiv\fP and
the user has an account on the server's machine, then service
is rendered (i.e. the user is allowed to log in, or the command
is executed).  Users may expand this ``equivalence'' of
machines by installing a \fI.rhosts\fP file in their login directory.
The root login is handled specially, bypassing the \fIhosts.equiv\fP
file, and using only the \fI/.rhosts\fP file.
.PP
Thus, to create a class of equivalent machines, the \fIhosts.equiv\fP
file should contain the \fIofficial\fP names for those machines.
If you are running the name server, you may omit the domain part
of the host name for machines in your local domain.
.NH 3
/etc/rc.local
.PP
Most network servers are automatically started up at boot time
by the command file /etc/rc (if they are installed in their
presumed locations) or by the Internet daemon (see above).
These include the following:
.DS
.TS
l l l.
Program	Server	Started by
_
rshd	shell server	inetd
rexecd	exec server	inetd
rlogind	login server	inetd
telnetd	TELNET server	inetd
ftpd	FTP server	inetd
fingerd	Finger server	inetd
tftpd	TFTP server	inetd
rwhod	system status daemon	/etc/rc
syslogd	error logging server	/etc/rc
sendmail	SMTP server	/etc/rc
routed	routing table management daemon	/etc/rc
.TE
.DE
Consult the manual pages and accompanying documentation (particularly
for sendmail) for details about their operation.
.PP
To have other network servers started up as well,
the appropriate line should be added
to the Internet daemon's configuration file \fI/etc/inetd.conf\fP, or
commands similar to the following should be placed in the site dependent
file \fI/etc/rc.local\fP.
.DS
if [ -f /usr/sbin/rwhod ]; then
	rwhod & echo -n ' rwhod'			>/dev/console
f\&i
.DE
.NH 3
/etc/ftpusers
.PP
The FTP server included in the system provides support for an
anonymous FTP account.
.PP
An anonymous account is enabled by creating a user \fIftp\fP.
When a client uses the anonymous account a \fIchroot\fP\|(2)
system call is performed by the server to restrict the client
from moving outside that part of the file system where the
user ftp home directory is located.  Because a \fIchroot\fP call
is used, certain programs and files used by the server 
process must be placed in the ftp home directory. 
Further, one must be
sure that all directories and executable images are unwritable.
The following directory setup is recommended.
.DS
\fB#\fP cd ~ftp
\fB#\fP chmod 555 .; chown ftp .; chgrp ftp .
\fB#\fP mkdir bin etc pub
\fB#\fP chown root bin etc
\fB#\fP chmod 555 bin etc
\fB#\fP chown ftp pub
\fB#\fP chmod 777 pub
\fB#\fP cd bin
\fB#\fP cp /bin/sh /bin/ls .
\fB#\fP chmod 111 sh ls
\fB#\fP cd ../etc
\fB#\fP cp /etc/passwd /etc/group .
\fB#\fP chmod 444 passwd group
.DE
When local users wish to place files in the anonymous
area, they must be placed in a subdirectory.  In the
setup here, the directory \fI~ftp/pub\fP is used.
.PP
NOTE: Mode 777 on the 'pub' directory can and has been abused!  Changing the
mode to 555 is a good choice but would require administrative assistance for
placing files in the 'pub' directory.  Probably not a bad idea though.
.PP
Another issue to consider is the copy of \fI/etc/passwd\fP
placed here.  It may be copied by users who use the
anonymous account.  They may then try to break the 
passwords of users on your machine for further access.
A good choice of users to include in this copy might
be root, daemon, uucp, and the ftp user.
.PP
Aside from the problems of directory modes and such,
the ftp server may provide a loophole for interlopers
if certain user accounts are allowed.
The file \fI/etc/ftpusers\fP is checked on each connection.
If the requested user name is located in the file, the
request for service is denied.  This file normally has
the following names on our systems.
.DS
uucp
root
.DE
Accounts with nonstandard shells should be listed in
this file.  Accounts without passwords
need not be listed in this file, the ftp server will
not service these users.
