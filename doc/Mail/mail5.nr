.bp
.sh 1 "Summary of commands, options, and escapes"
.pp
This section gives a quick summary of all of the
.i Mail
commands, binary and valued options, and tilde escapes.
The following table describes the commands:
.(b
.TS
center ;
c ci
lb l.
Command	Description
_
!	Single command escape to shell
-	Back up to previous message
alias	Define an alias as a set of user names
chdir	Change working directory, home by default
delete	Delete a list of messages
dt	Delete current message, type next message
edit	Edit a list of messages
exit	Leave mail without changing anything
from	List headers of a list of messages
headers	List current window of messages
help	Print brief summary of \fIMail\fR commands
mail	Send mail to specified names
mbox	Arrange to save a list of messages in \fImbox\fR
next	Go to next message and type it
preserve	Arrange to leave list of messages in system mailbox
quit	Leave \fIMail\fR; update system mailbox, \fImbox\fR as appropriate
reply	Compose a reply to a message
save	Append messages, headers included, on a file
set	Set binary or valued options
shell	Invoke an interactive shell
top	Print first so many (5 by default) lines of list of messages
type	Print messages
undelete	Undelete list of messages
unset	Undo the operation of a \fBset\fR
visual	Invoke visual editor on a list of messages
write	Append messages to a file, don't include headers
.TE
.)b
.pp
The following table describes the options.  Each option is
indicated as being either a binary or valued option.
.(b
.TS
center;
c ci ci
l ci l.
Option	Type	Description
_
EDITOR	valued	Pathname of editor for ~e and \fBedit\fR
SHELL	valued	Pathname of shell for \fBshell\fR, ~! and \fB!\fR
VISUAL	valued	Pathname of screen editor for ~v, \fBvisual\fR
append	binary	Always append messages to end of \fImbox\fR
ask	binary	Prompt user for Subject: field when sending
askcc	binary	Prompt user for additional Cc's at end of message
autoprint	binary	Print next message after \fBdelete\fR
escape	valued	Escape character to be used instead of ~
ignore	binary	Ignore \s-2RUBOUT\s0 while sending mail
metoo	binary	Include sending user in aliases
nosave	binary	Don't save partial letter in \fIdead.letter\fR
quiet	binary	Suppress printing of \fIMail\fR version
record	valued	File to save all outgoing mail in
toplines	valued	Number of lines to print in \fBtop\fR
.TE
.)b
.(b
.pp
Finally, the following table summarizes the tilde escapes available
while sending mail.
.TS
center;
c ci ci
l li l.
Escape	Arguments	Description
_
~!	command	Execute shell command
~c	name ...	Add names to Cc: field
~d		Read \fIdead.letter\fR into message
~e		Invoke text editor on partial message
~h		Edit the header fields
~m	messages	Read named messages, right shift by tab
~p		Print message entered so far
~q		Abort entry of letter; like \s-2RUBOUT\s0
~r	filename	Read file into message
~s	string	Set Subject: field to \fIstring\fR
~t	name ...	Add names to To: field
~v		Invoke screen editor on message
~w	filename	Write message on file
~|	command	Pipe message through \fIcommand\fR
~~	string	Quote a ~ in front of \fIstring\fR
.TE
.)b
