The C library routine getdate(3) should be put in /lib/libc.a:
.DS
	make install
	make clean
.DE
The manual page getdate.3 may be put in /sys/man/man3/getdate.3.
.PP
News uses getdate to decipher user specified dates.
The current implementation is rather primitive (and bulky).
It is hoped that a future version of getdate
(or something like it)
will become a common date input routine
for commands such as date(1), at(1), and calendar(1).
.PP
Those sites that do not want to put getdate.o in 'libc.a'
should put getdate.o in the news subdirectory
and change the news makefile line which creates the news program:
.DS
cc ... -o news
.DE
to
.DS
cc ... getdate.o -o news
.DE
