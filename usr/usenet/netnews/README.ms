. \" @(#)README.ms	1.2 9/14/80
.NH
News installation
.PP
The
.I getdate(3)
routine must be installed prior to
installation of
.I news.
Consult the getdate subdirectory before proceeding.
.PP
Edit /usr/include/whoami.h and change the #define for sysname
to the name chosen for your system.
If you will be receiving news from other systems via uucp,
add an entry for "rnews" to the Cmds array of uuxqt.c,
then remake uuxqt.
(Other useful suggestions may be found in the "usenet/setup.n" document.)
.PP
News runs setuid to keep its own files secure.
You must decide on the uid and gid to be used by
.I news.
In the supplied makefile, these are both "daemon".
You should edit /etc/passwd and /etc/group
to include these two groups,
or you can change the news uid and/or gid
by changing the appropriate make variable (in makefile).
.PP
.I News
may now be installed:
.DS
	su root
	make install
	make clean
.DE
.PP
News starts up much faster if it is made "sticky"
(as is the case with any large shared-text program).
The news.1 manual page may be put in /usr/man/man1/news.1.
Also, you may want to edit the ".ngfile" and ".sys" files
that are described below.
.NH
News Files
.NH 2
/usr/bin/news
.PP
This is the executable news program.
The program "rnews" is a link to
.I news
which,
when invoked,
reads a network-formatted article from its standard input.
.NH 2
/usr/spool/news
.PP
This directory contains the
files used by the news program.
.NH 2
/usr/spool/news/.bitfile
.PP
This file is a bit map
maintained for the benefit of
.I login(1).
The following routine can be embedded
in login to check if a user has news:
.DS
#define BITFILE "/usr/spool/news/.bitfile"
/* return 1 if user has news, else 0. */
newscheck(uid)
register int uid;
{
	register int fd;
	static char c;
	fd = open(BITFILE, 0);
	lseek(fd, (long)(uid>>3), 0);
	read(fd, &c, 1);
	close(fd);
	return((c >> (uid&07)) & 01);
}
.DE
.PP
The bit map is recreated when an article is inserted or cancelled,
and it is updated when each user reads his new news.
Each user's bit is one if he has news, and zero if not.
.PP
The bit map may also be interrogated using the supplied
.I nchk
program.
.NH 2
/usr/spool/news/.ngfile
.PP
This file lists all of the accepted news groups on the system.
Each newsgroup
to which an article is submitted
or to which a user subscribes
must appear in .ngfile.
Newsgroups are listed in the .ngfile one per line.
The local administration may modify this file as desired.
A typical .ngfile for system "xyz" would be:
.DS
general
sys
dept
NET.general
test
to_duke
.DE
The "general" newsgroup should be used for most articles.
The "sys" newsgroup might be for news of interest
to systems programmers,
while the
"dept"
newsgroup would be for news of departmental
interest only
(e.g. parties).
Articles submitted to "NET.general" are put on the net (see below).
The newsgroup "test" may be used for local tests of news.
The newsgroup "to_duke" sends news only to Duke
and may be used for network news tests.
.NH 2
/usr/spool/news/.sys
.PP
This file lists the newsgroups which
the local system and connecting remote systems
receive from the net.
The file for system xyz might be
.DS
xyz:NET.ALL,to_xyz
duke:NET.ALL,to_duke
.DE
Each entry consists of the system name, a colon,
and a list of comma-separated newsgroups.
(The line may also contain
two additional fields
which are described below under
"Transmission of network news.")
The systems xyz and duke subscribe to all "NET" articles,
xyz alone subscribes to "to_xyz" (a test newsgroup),
and duke alone subscribes to "to_duke" (another test newsgroup).
The local administration may edit the .sys file as desired.
For example, most of the systems
in the Research Triangle, NC,
subscribe to the sub-network newsgroup "triangle".
.NH 2
/usr/spool/news/sys.nnn
.PP
Files with names of this form (e.g. "duke.122") are news articles.
According to the protocol,
every network guarantees to give each article
it generates a unique file name.
This is done by appending
a unique integer
to the local system name.
.NH 2
/usr/spool/news/.uindex
.PP
This file contains one line per news user.
Each line has three colon separated fields.
The first field is the user id.
The second field is the time (in seconds since 1970)
when the user last read news.
The third field is the subscription list,
which consists of newsgroups
separated by commas.
A missing third field is taken to be ":general".
.NH 2
/usr/spool/news/.nindex
.PP
This file contains one line per news article.
The three colon separated fields on each line
are for the file name,
the date of submission (locally),
and the list of comma-separated newsgroups
to which the article belongs.
As with .uindex,
a missing third field is taken to be ":general".
.NH 2
/usr/spool/news/.seq
.PP
This file contains the sequence number given to
the last article generated locally.
.NH 2
/usr/spool/news/.history
.PP
In order to prevent redundant traffic
and spurious resubmission of news,
the file name of every news item
ever received by this system is kept
in .history.
.NH
Network news
.NH 2
News article transfer format
.PP
The first character of the transferred file
identifies its format
and will be used to simplify
inevitable changes in article formats.
Currently, this character is 'A'.
The rest of the first line is a unique network-wide name,
which also identifies the originating node.
The article name is used to prevent
the unlimited duplication of news articles
that might otherwise occur.
The second line is a comma-separated list
of newsgroups to which the article belongs.
Newsgroup names may not contain colons.
The third line identifies the contributor of the article.
It is a system-pathname sequence
suitable for mailing a reply via
.I mail(1).
The fourth line is the contribution date in
.I ctime(3)
format.
The fifth line is the article title.
The remaining lines are the text of the article.
The article is ending by '.' alone on a line, or end of file.
.DS
   Figure 1 - news transmission format.

Aduke.405		. Format id ('A') followed by
			  the article name.
test			. newsgroups list.
duke!swd		. path name of author.
Fri May 16 10:29:40 1980. date of original submission.
test			. article title.
testing one two three	. text of article.
.DE
.NH 2
Transmission of network news.
.PP
When a news item is entered into
the local news system,
the .sys file is scanned.
For each system in the file
whose name does not appear in the
author-pathname line of the item,
and whose subscription list
matches the item's newsgroup list,
.I news
attempts to
run
"rnews"
on the remote system with
the article as standard input.
If the last (fourth) field in the
".sys"
file is empty,
then
.I news
uses
uux
to execute rnews on the remote system.
Otherwise, the fourth field in the
."sys"
file is a command line which
which is invoked with the article on standard input.
This transmission program (command line)
assumes responsibility for passing
the article unchanged
to
"rnews"
on the remote system.
Frequently, the program invoked will
be a network mail program.
Setting up
.I news
to use
.I mail(1)
for
transmission of articles is
described in detail in the
"Examples"
section below.
.NH 2
Reception of network news.
.PP
The rnews program reads an article from standard input
and processes it for inclusion in the local news.
.I Rnews
throws away all articles mentioned in the history,
and removes all newsgroups
from the newsgroup line
to which the local system does not
subscribe (as specified in .sys).
Articles that the local system does not subscribe to
at all
are thrown away.
As
.I rnews
copies an article into /usr/spool/news,
the local system name is prepended to the
system-pathname line.
After the article has been copied
the article is retransmitted,
as modified by the reception process,
to all systems which subscribe to the article.
Note that if system xyz
does not subscribe to NET.test (for example),
it will not receive or retransmit
articles submitted only to NET.test,
and will remove NET.test
from the newsgroup line of
any article passing through xyz.
.NH
Examples and little documented features.
.NH 2
Catching rnews diagnostics.
.PP
Some of the diagnostics from
.I rnews
may be interesting.
You can arrange to have these
diagnostics mailed to you
by "mv"ing the rnews link
to rnewsx
and making rnews a shell script:
"rnewsx 2>&1 | mail root".
Be sure to make the rnews script executable.
(The news makefile does not know about rnewsx,
so "make install" will undo this change.)
.NH 2
Users who should not get news.
.PP
.I News
assumes that all users should subscribe to
"general"
so that they will receive
administrative notices of system downtime &etc.
If an article is submitted to general,
then users will be greeted at login time with "You have news."
Some users (e.g. uucp) should not be bothered with such messages.
Fortunately it is possible, though inconvenient,
for a user to subscribe to nothing.
Suppose uid 253 should never receive news.
Manually change
the line in .uindex that begins "253:"
to contain only a subscription to
"None"
.IP
253:328208485:None
.LP
Since "None" is a non-existent newsgroup,
uid 253 will never "have news."
.PP
Such users will still receive
the message of the day and "You have mail" notices,
so eliminating the news notice may be irrelevant.
Duke's login suppresses all messages
when the user has a non-standard shell (e.g. uucico).
.NH 2
Adding a new system to the news network
.PP
To tell news about a new system with which
it is to exchange news,
first add a line to the .sys file:
.IP
newsys:NET.ALL,to_newsys
.LP
If you cannot obtain
.I uux(1)
permission to run
.I rnews
on newsys,
then you will have use some other means
of transmitting articles,
and modify this .sys entry accordingly.
Use of
.I mail(1)
to transmit articles is explained below.
Now add the test newsgroup to your .ngfile:
.IP
echo "to_newsys" >>.ngfile
.LP
and welcome the new system:
.DS
echo "welcome to USENET" | news -i hello -n to_newsys
.DE
.NH 2
Subnetworks
.PP
The newsgroup matching scheme, together with
the system subscription lists (the .sys file),
are designed to allow for the easy creation
of subnetworks.
To create the research triangle
subnetwork which consists of
only the newsgroup
"triangle,"
each of the triangle area systems
added
"triangle"
to the list of newsgroups
received, as specified
by their local entry in the .sys file,
and added
"triangle"
to the list of newsgroups
sent to participating
systems.
.PP
For example, the .sys file at U.N.C. might look like
.DS
unc:NET.ALL,triangle,to_unc
duke:NET.ALL,triangle,to_duke
.DE
.PP
If the triangle network needed
more than one newsgroup,
each system would put
"TRI.ALL"
in their .sys file
instead of
"triangle".
Since newsgroups ending
in
"ALL"
subscribe to any newsgroup
with the same prefix,
all newsgroups beginning
with
"TRI."
would be transmitted throughout the
triangle subnetwork;
however, each newsgroup in the triangle network
should have a separate entry in the .ngfile
since newsgroups containing
"ALL"
are not recommended for .ngfile.
.NH 2
Setting up news to use mail
.PP
Suppose systems Able and Charlie wish to exchange
news, and that Able and Charlie talk to each
other only through Baker, a system which,
alas, does not run news.
Able can still exchange news with Charlie
using the transmission field in the .sys
file
to transmit
news via
.I mail(1),
which understands indirection.
System Able would take the following steps
(and Charlie would take parallel action):
First, Able's .sys line for Charlie should be
.DS L
Charlie:NET.ALL,to_Charlie::sed -e "s/^/N/"|mail Baker!Charlie!news
.DE
The fourth field on the line is run to transmit news
to system Charlie by putting an 'N' on the front of
every line of the article and mailing it to Charlie!news.
Able sets up to receive news mailed from
Charlie by adding an entry in /etc/passwd
for the user "news"
and arranges to run the following
shell script once an hour or so.
.DS
(while true
do
	echo d
done) 2>&1 | mail -f /usr/spool/mail/news | uurec
.DE
This script pipes all the mail that
"news"
has received into
.I uurec,
a utility program distributed with
.I news.
.I Uurec
reads a series of articles,
splits them apart,
removes the "From" lines,
edits the author's path line
to reflect the systems through
which the article was mailed,
strips off the 'N'
on the front of all the article lines,
and passes the result to
.I rnews.
If news is mailed across
a non uucp network,
then uurec.c may have to be modified
to correctly process the header information
supplied by the mail programs on other networks.
.NH
Security considerations.
.PP
It is easy to fake the origin of an article,
since
.I rnews
believes any
syntactically valid article
presented as standard input.
The only cure for this is a public key
encryption scheme.
.PP
Since
.I news
runs set-user-id,
it should not use a powerful
uid.
If
.I news
ran as "bin"
and a security hole were found in news,
the
entire system would be compromised.
.NH
Unimplemented features.
.NH 1
Read and write permissions on newsgroups.
.PP
Newsgroups should be protected from excessive
junk news by a permission scheme.
However, junk news has yet to be a problem,
and no elegant and general architecture
for newsgroup permissions has been proposed.
.NH 1
Per-user bit map.
.PP
.I News
should have a bit map which indicates which
articles each user has yet to read.
This would allow
.I news
to permit the user to leave articles
around in an
undisposed
state.
.NH 1
Activation and cancellation dates.
.PP
News articles should have activation
and cancellation dates.
This would relieve users of the need to
submit and cancel articles at specific times.
Use of
.I at(1)
and
.I calendar(1)
is recommended until such time as
.I news
supports these features.
.NH 1
Remote cancellation of articles.
.PP
When an article is cancelled by it author,
he or she should have to option
of having news send out a cancellation
message which would cause
remote systems to cancel the article.
However, they may not want it cancelled,
which is a problem.
