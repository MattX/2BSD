.bp
.sh 1 "Common usage"
.pp
The
.i Mail
command has two distinct usages, according to whether one
wants to send or receive mail.  Sending mail is simple:  to send a
message to a user whose login name is, say,
\*(lqroot,\*(rq
use the shell
command:
.(l
% Mail root
.)L
then type your message.  When you reach the end of the message, type
an EOT (control\-d) at the beginning of a line, which will cause
.i Mail
to echo \*(lqEOT\*(rq and return you to the Shell.  When the user you sent mail
to next logs in, he will receive the message:
.(l
You have mail.
.)L
to alert him to the existence of your message.
.pp
If, while you are composing the message
you decide that you do not wish to send it after all, you can
abort the letter with a \s-2RUBOUT\s0.  Typing a single \s-2RUBOUT\s0
causes
.i Mail
to print
.(l
(Interrupt -- one more to kill letter)
.)L
Typing a second
\s-2RUBOUT\s0 causes
.i Mail
to save your partial letter on the file
.q dead.letter
in your home directory and abort the letter.
Once you have
in fact sent mail to someone, there is no way to undo the act, so be
careful.
.pp
The message your recipient reads will consist of the message you
typed, preceded by a line telling who sent the message (your login name)
and the date and time it
was sent.
.pp
If you want to send the same message to several other people, you can list
all of their login names on the command line.
Thus,
.(l
% Mail sam bob john
Tuition fees are due next Friday.  Don't forget!!
<Control\-d>
EOT
%
.)L
will send the reminder to sam, bob, and john.
.pp
If, when you log in, you see the message,
.(l
You have mail.
.)L
you can read the mail by typing simply:
.(l
% Mail
.)L
.i Mail
will respond by typing its version number and date and then listing
the messages you have waiting.  Then it will type an underscore and await
your command.  The messages are assigned numbers starting with 1 \*- you
refer to the messages with these numbers.
.pp
To look at a specific message, use the
.b type
command, which may be abbreviated to simply
.b t .
For example, if you had the following messages:
.(l
   1 root     Wed Sep 21 09:21  "Tuition fees"
   2 sam      Tue Sep 20 22:55
.)L
you could examine the first message by giving the command:
.(l
type 1
.)L
which might cause
.i Mail
to respond with, for example:
.(l
Message  1:
From root  Wed Sep 21 09:21:45 1978
Subject: Tuition fees

Tuition fees are due next Wednesday.  Don't forget!!

.)L
Many
.i Mail
commands which operate on messages take a message number as an
argument like the
.b type
command.  For all of these commands, there is a notion of a current
message.  When you enter the
.i Mail
program, the current message is initially the first one.  Thus,
you can often omit the message number and use, for example,
.(l
t
.)L
to type the current message.  As a further shorthand, you can type a message
by simply giving its message number.  Hence,
.(l
1
.)L
would type the first message.
.pp
Frequently, it is useful to read the messages in your mailbox in order,
one after another.  You can read the next message in
.i Mail
by simply typing a newline.  As a special case, you can type a newline
as your very first command to
.i Mail
to type the first message.
.pp
If, after typing a message, you wish to immediately send a reply,
you can do so with the
.b reply
command.
.b Reply ,
like
.b type ,
takes a message number as an argument.
.i Mail
then begins a message addressed to the user who sent you the message.
You may then type in your letter in reply, followed by a <control-d>
at the beginning of a line, as before.
.i Mail
will type EOT, then type the underscore prompt to indicate its readiness
to accept another command.  In our example, if, after typing the
first message, you wished to reply to it, you might give the command:
.(l
reply
.)L
.i Mail
responds by typing:
.(l
To: root
Subject: Tuition fees
.)L
and waiting for you to enter your letter.  Note that it copies the subject
header from the original message.  This is useful in that correspondence
about a particular matter will tend to retain the same subject heading,
making it easy to recognize.  If there are other header fields in
the message, the information found will also be used.
For example, if the letter had a
.q "To:"
header listing a number of recipients,
.i Mail
would arrange to send your replay to the same people as well.
Similarly, if the original message contained a
.q "Cc:"
(carbon copies to) field,
.i Mail
would send your reply to
.i those
users, too.
.i Mail
is careful, though, not too send the message to
.i you ,
even if you appear in the
.q "To:"
or
.q "Cc:"
field, unless you ask to included explicitly.  See section 3 for more
details.
.pp
After typing in your letter, the dialogue with
.i Mail
might look like the following:
.(l
reply
To: root
Subject: Tuition fees

Thanks for the reminder
EOT
\(ul
.)L
.pp
The
.b reply
command is especially useful for sustaining extended conversations
over the message system, with other
.q listening
users receiving copies of the conversation.  The
.b reply
command can be abbreviated to
.b r .
.pp
If you wish, while reading your mail, to send a message to someone,
but not as a reply to one of your messages, you can send the message
directly with the
.b mail
command, which takes as arguments the names of the recipients you wish
to send to.  For example, to send a message to
.q frank,
you would do:
.(l
mail frank
This is to confirm our meeting next Friday at 4.
EOT
\(ul
.)L
The
.b mail
command can be abbreviated to
.b m .
.pp
Normally, each message you receive is saved in the file
.i mbox
in your login directory at the time you leave
.i Mail .
Often,
however, you will not want to save a particular message you
have received because it is only of passing interest.  To avoid
saving a message in
.i mbox
you can delete it using the
.b delete
command.  In our example,
.(l
delete 1
.)L
will prevent
.i Mail
from saving message 1 (from root) in
.i mbox .
In addition to not saving deleted messages,
.i Mail
will not let
you type them, either.  The effect is to make the message disappear
altogether, along with its number.  The
.b delete
command can be abbreviated to simply
.b d .
.pp
A number of features of
.i Mail
can be tailored to your liking with the
.b set
command.  The
.b set
command has two forms, depending on whether you are setting
a
.i binary
option or a
.i valued
option.
Binary options are either on or off.  For example, the
.q ask
option informs
.i Mail
that each time you send a message, you want it to prompt you for
a subject header, to be included in the message.
To set the
.q ask
option, you would type
.(l
set ask
.)L
.pp
Valued options are values which
.i Mail
uses to adapt to your tastes.  For example, the
.q SHELL
option tells
.i Mail
which shell you like to use, and is specified by
.(l
set SHELL=/bin/csh
.)L
for example.  Note that no spaces are allowed in
.q "SHELL=/bin/csh."
A complete list of the
.i Mail
options appears in section 4.
.pp
Another adaptation to user needs that
.i Mail
provides is that of
.i aliases .
An alias is simply a name which stands for one or more
real user names.
.i Mail
sent to an alias is actually sent to the list of real users
associated with it.  For example, an alias can be defined for the
members of a project, so that you can send mail to the whole project
by sending mail to just a single name.  The
.b alias
command in
.i Mail
is used to define an alias.  Suppose that the users in a project are
named Sam, Sally, Steve, and Susan.  To define an alias called
.q project
for them, you would use the
.i Mail
command:
.(l
alias project sam sally steve susan
.)L
The
.b alias
command can also be used to provide a convenient name for someone
whose user name is inconvenient.  For example, if a user named
.q "Bob Anderson"
had the login name
.q anderson,"
you might want to use:
.(l
alias bob anderson
.)L
so that you could send mail to the shorter name,
.q bob.
.pp
While the
.b alias
and
.b set
commands allow you to customize
.i Mail ,
they have the drawback that they must be retyped each time you enter
.i Mail .
To make them more convenient to use,
.i Mail
always looks for two files when it is invoked.  It first reads
a system wide file
.q /usr/lib/Mail.rc,
then a user specific file,
.q .mailrc,
which is found in the user's home directory.  The system wide file
is maintained by the system administrator and
is usually used to define
aliases that are of general interest, such as the list of users which
constitutes the system staff.  The
.q .mailrc
file is usually used by each user to set options the way he likes.
For example, my .mailrc file looks like this:
.(l
set ask nosave SHELL=/bin/csh
.)L
As you can see, it is possible to set many options in the
same
.b set
command.  The
.q nosave
option is described in section 4.
.pp
We have seen that
.i Mail
can be invoked with command line arguments which are people
to send the message to, or with no arguments to read mail.
In addition, there are two flag arguments to
.i Mail
which are useful.
First, unreliable terminal connections (such as poor connections
over a phone line) cause spurious \s-2RUBOUT\s0 characters to
be produced.  \s-2RUBOUT\s0  characters cause
.i Mail
to terminate a message, as described previously.
To prevent these spurious \s-2RUBOUT\s0 characters from causing
trouble, you can give the
.rb \-i
flag to ignore them.  For example,
.(l
% Mail \-i
.)L
reads your mail with \s-2RUBOUT\s0 characters ignored, and
.(l
% Mail \-i bob joe sarah
.)L
mails to the named people with \s-2RUBOUT\s0  characters ignored.
Unfortunately, even if
.i Mail
ignores \s-2RUBOUT\s0's, the system discards all text typed on the
current line when it receives a  \s-2RUBOUT\s0.  To warn the user
that this has happened,
.i Mail
echoes \s-2RUBOUT\s0s as @'s.  If, when using
.rb \-i
an @ at appears on your terminal, you must retype the text on
the line you were typing.  If the @ appears at the beginning
of a line, you can safely ignore it.
.pp
The other
.i Mail
flag is
.rb \-f
which allows you to use
.i Mail
to examine a file of messages other than your default system
mailbox.  For example, if you have a collection of messages in
the file
.q letters
you can use
.i Mail
to read them with:
.(l
% Mail \-f letters
.)L
You can use all of
the
.i Mail
commands described in this document to examine, modify, or delete
messages from your
.q letters
file, which will be rewritten when you leave
.i Mail
with the
.b quit
command described below.
.pp
Since mail that you read is saved in the file
.i mbox
in your home directory by default, you can read
.i mbox
in your home directory by using simply
.(l
% Mail \-f
.)L
.pp
Normally, messages which you examine using the
.b type
command are saved in the file
.q mbox
in your home directory
if you leave
.i Mail
with the
.b quit
command described below.
If you wish to retain a message in your system mailbox
you can use the
.b preserve
command to tell
.i Mail
to leave it there.
The
.b preserve
command accepts a list of message numbers, just like
.b type
and may be abbreviated to
.b pre .
.pp
Messages in your system mailbox which you do not examine are
normally retained in your system mailbox automatically.
If you wish to have such a message saved in
.i mbox
without actually reading it, you may use the
.b mbox
command to have them so saved.  For example,
.(l
mbox 2
.)L
in our example would cause the second message (from sam)
to be saved in
.i mbox
when the
.b quit
command is executed.
.b Mbox
can be abbreviated to
.b mb .
.pp
When you have perused all of the messages of interest, you can leave
.i Mail
with the
.b quit
command, which saves all of the messages you have typed but not
deleted in the file
.i mbox
in your login directory.  Deleted messages are discarded irretrievably,
and messages left untouched are preserved in your system mailbox so
that you will see them the next time you type:
.(l
% Mail
.)L
The
.b quit
command can be abbreviated to simply
.b q .
.pp
If you wish for some reason to leave
.i Mail
quickly without altering either your system mailbox or
.i mbox ,
you can type the
.b x
command (short for
.b exit ),
which will immediately return you to the Shell without changing anything.
.pp
If, instead, you want to execute a Shell command without leaving
.i Mail ,
you
can type the command preceded by an exclamation point, just as in the
text editor.  Thus, for instance:
.(l
!date
.)L
will print the current date without leaving
.i Mail .
.pp
Finally, the
.b help
command is available to print out a brief summary of the
.i Mail
commands, using only the single character command abbreviations.
