.bp
.sh 1 "Additional features"
This section describes some additional commands of use for
reading your mail, setting options, and handling lists of messages.
.sh 2 "Additional commands"
.pp
This section describes additional
.i Mail
commands available when
receiving mail.
.pp
The
.b next
command goes to the next message and types it.  If given a message list,
.b next
goes to the first such message and types it.  Thus,
.(l
next root
.)L
goes to the next message sent by
.q root
and types it.  The
.b next
command can be abbreviated to simply a newline, which means that one
can go to and type a message by simply giving its message number or
one of the magic characters
.q "\(ua"
.q "."
or
.q "$".
Thus,
.(l
\&.
.)L
prints the current message and
.(l
4
.)L
prints message 4, as described previously.
.pp
The
.rb \-
command goes to the previous message and prints it.  The
.rb \-
command may be given a decimal number
.i n
as an argument, in which case the
.i n th
previous message is gone to and printed.
.pp
It is often useful to be able to save messages on related topics
in a file.  The
.b save
command gives you ability to do this.  The
.b save
command takes as argument a lit of message numbers, followed by
the name of the file on which to save the messages.  The messages
are appended to the named file, thus allowing one to keep several
messages in the file, stored in the order they were put there.
The
.b save
command can be abbreviated to
.b s .
An example of the
.b save
command relative to our running example is:
.(l
s 1 2 tuitionmail
.)L
.b Saved
messages are not automatically saved in
.i mbox
at quit time, nor are they selected by the
.b next
command described above, unless explicitly specified.
.pp
The
.b save
command always writes the entire message, including the headers,
into the file.  If you want to write just the message itself, you
can use the
.b write
command.  The
.b write
command has the same syntax as the
.b save
command, and can be abbreviated to simply
.b w .
Thus, we could write the second message by doing:
.(l
w 2 file.c
.)L
As suggested by this example, the
.b write
command is useful for such tasks as sending and receiving
source program text over the message system.
.pp
The
.b undelete
command causes a message which had been deleted previously to regain
its initial status.  Only messages which are already deleted may be
undeleted.  This command may be abbreviated to
.b u .
.pp
In order to edit individual messages using the text editor, the
.b edit
command is provided.  The
.b edit
command takes a list of message as described under the
.b type
command and processes each by writing it into the file
Message\c
.i x
where
.i x
is the message number being edited and executing the text editor on it.
When you have edited the message to your satisfaction, write the message
out and quit, upon which
.i Mail
will read the message back and remove the file.
.b Edit
may be abbreviated to
.b e .
.pp
It is often useful to be able to invoke one of two editors,
based on the type of terminal one is using.  To invoke
a display oriented editor, you can use the
.b visual
command.  The operation of the
.b visual
command is otherwise identical to that of the
.b edit
command.
.pp
Both the
.b edit
and
.b visual
commands assume some default text editors.  These default editors
can be overriden by the valued options
.q EDITOR
and
.q VISUAL
for the standard and screen editors.  You might want to do:
.(l
set EDITOR=/usr/ucb/ex VISUAL=/usr/ucb/vi
.pp
The
.b chdir
and
.b shell
commands allow you to change your current directory and
escape to the shell, respectively.
.b Chdir
takes a single argument, which is taken to be the pathname of
the directory to change to.  If no argument is given,
.b chdir
changes to your home directory.
.b Shell
invokes an interactive shell and allows you to type commands to it.
When you leave the shell, you will return to
.i Mail .
The shell used is a default assumed by
.i Mail ;
you can override this default by setting the valued option
.q SHELL,
eg:
.(l
set SHELL=/bin/csh
.)L
.pp
When you start up
.i Mail
to read your mail, it lists the message headers that you have.
These headers tell you who each message is from, when they were
sent, how many lines and characters each message is, and the
.q "Subject:"
header field of each message, if present.  In addition,
.i Mail
tags the message header of each message which has been the object
of the
.b preserve
command with a
.q P.
Messages which have been
.b saved
or
.b written
are flagged with a
.q *.
Finally,
.b deleted
messages are not printed at all.  If you wish to reprint the current
list of message headers, you can do so with the
.b headers
command.  The
.b headers
command (and thus the initial header listing)
only lists the first 18 message headers.
.i Mail
maintains a notion of the current
.q window
into your messages for the purposes of printing headers.  You can
move
.i Mail's
attention forward to the next window by giving the
.(l
headers +
.)L
command.  Analogously, you can move to the previous window with:
.(l
headers \-
.)L
Finally, you can move
.i Mail's
notion of the current window directly to a particular message by
using, for example,
.(l
headers 40
.)L
to move
.i Mail's
attention to the messages around message 40.  The
.b headers
command can be abbreviated to
.b h .
.pp
The
.b from
command takes a list of messages and prints out the header lines for each one;
hence
.(l
from joe
.)L
is the easy way to display all the message headers from \*(lqjoe.\*(rq
.pp
The
.b top
command takes a message list and prints the first five lines
of each addressed message.  It may be abbreviated to
.b to .
If you wish, you can change the number of lines that
.b top
prints out by setting the valued option
.q "toplines."
On a CRT terminal,
.(l
set toplines=10
.)L
might be preferred.
.pp
The
.b dt
command deletes the current message and prints the next message.
It is useful for quickly reading and disposing of mail.
.sh 2 "Message lists"
.pp
The
.b type
and
.b delete
commands described in section two take a list of messages as argument,
as do many of the commands described in section six.  This section
describes the construction of message lists in general.
.pp
A
.i "message list"
consists of a list of message numbers, ranges, and names,
separated by spaces or tabs.  Message numbers may be either
decimal numbers, which directly specify messages, or one of the
special characters
.q \(ua
.q "."
or
.q "$"
to specify the first relevant, current, or last
relevant message, respectively.
.i Relevant
here means, for most commands
.q "not deleted"
and
.q "deleted"
for the
.b undelete
command.
.pp
A range of messages consists of two message numbers (of the form
described in the previous paragraph) separated by a dash.
Thus, to print the first four messages, use
.(l
type 1\-4
.)L
and to print all the messages from the current message to the last
message, use
.(l
type .\-$
.)L
.pp
A
.i name
is a user name.  All of the user names given in the message list are
collected together and each message selected by other means
is checked to make sure it was sent by one of the named users.
If the message consists entirely of user names, then every
message sent by one those users which is
.i relevant
(in the sense described earlier)
is selected.  Thus, to print every message sent to you by
.q root,
do
.(l
type root
.)L
.pp
As a shorthand notation, you can specify simply
.q *
to get every
.i relevant
(same sense)
message.  Thus,
.(l
type *
.)L
prints all undeleted messages,
.(l
delete *
.)L
deletes all undeleted messages, and
.(l
undelete *
.)L
undeletes all deleted messages.
.sh 2 "Other options"
.pp
Throughout this manual, we have seen examples of binary and valued options.
This section describes each of the options in alphabetical order, including
some which you have not seen yet.
To avoid confusion, please note that all of the options are either
all lower case letters or all upper case letters.  When I start a sentence
such as:
.q "Ask"
causes
.i Mail
to prompt you for a subject header,
I am only capitalizing
.q ask
as a courtesy to English.
.pp
The
.q append
option is binary and
causes messages saved in
.i mbox
to be appended to the end rather than prepended.
Normally,
.i Mail will put messages in
.i mbox
in the same order that the system puts messages in your system mailbox.
By setting
.q append,
you are requesting that
.i mbox
be appended to regardless.  It is in any event quicker to append.
.pp
.q "Ask"
is a binary option which
causes
.i Mail
to prompt you for the subject of each message you send.
If you respond with simply a newline, no subject field will be sent.
.pp
.q Askcc
is a binary option which
causes you to be prompted for additional carbon copy recipients at the
end of each message.  Responding with a newline indicates your
satisfaction with the current list.
.pp
.q Autoprint
is a binary option which
causes the
.b delete
command to behave like
.b dp
\*- thus, after deleting a message, the next one will be typed
automatically.  This is useful to quickly scanning and deleting
messages in your mailbox.
.pp
The binary option
.q ignore
causes \s-2RUBOUT\s0 characters from your terminal to be ignored and echoed
as @'s while you are sending mail.  \s-2RUBOUT\s0 characters retain their
original meaning in
.i Mail
command mode.
.pp
When sending mail to an alias,
.i Mail
makes sure that if you are included in the alias, that mail will not
be sent to you.  This is useful if a single alias is being used by
all members of the group.  If however, you wish to receive a copy of
all the messages you send to the alias, you can set the binary option
.q metoo.
.pp
The binary option
.q quiet
suppresses the printing of the version when
.i Mail
is first invoked,
as well as printing the for example
.q "Message 4:"
from the
.b type
command.
.pp
Normally,
when you abort a message with two \s-2RUBOUTs\s0,
.i Mail
copies the partial letter to the file
.q dead.letter
in your home directory.  Setting the binary option
.q nosave
prevents this.
.pp
The valued option
.q EDITOR
defines the pathname of the text editor to be used in the
.b edit
command and ~e.  If not defined, a standard editor is used.
.pp
The valued option
.q SHELL
gives the path name of your shell.  This shell is used for the
.b !
command and ~! escape.  In addition, this shell is used to expand
file names with shell metacharacters like * and ? in them.
.pp
The valued option
.q VISUAL
defines the pathname of your screen editor for use in the
.b visual
command
and ~v escape.  A standard screen editor is used if you do not define one.
.pp
In order to allow you to change the escape character used when sending
mail, you can set the valued option
.q escape.
Only the first character of the
.q escape
option is used, and it must be doubled if it is to appear literally as
the first character of a line of your message.  If you change your escape
character, then ~ loses all its special meaning, and need no longer be doubled
at the beginning of a line.
.pp
If you love to keep records, then the
valued option
.q record
can be set to the name of a file to save all of your outgoing mail.
Each new message you send is appended to the end of the file.
.pp
The valued option
.q toplines
defines the number of lines that the
.q top
command will print out instead of the default five lines.
