/* 
 * config.h
 */

/* CHARSPRINTF:
 *	This symbol is defined if this system declares "char *sprintf()" in
 *	stdio.h.  The trend seems to be to declare it as "int sprintf()".  It
 *	is up to the package author to declare sprintf correctly based on the
 *	symbol.
 */
#define	CHARSPRINTF 	/**/

/* FCNTL:
 *	This symbol, if defined, indicates to the C program that it should
 *	include fcntl.h.
 */
#define	FCNTL		/**/

/* FTIMER:
 *	This symbol, if defined, indicates that the ftime() routine exists.
 */
#define	FTIMER		/**/

/* GETHOSTNAME:
 *	This symbol, if defined, indicates that the C program may use the
 *	gethostname() routine to derive the host name.  See also DOUNAME
 *	and PHOSTNAME.
 */
#define	GETHOSTNAME	/**/

/* HAVETERMLIB:
 *	This symbol, when defined, indicates that termlib-style routines
 *	are available.  There is nothing to include.
 */
#define	HAVETERMLIB	/**/

/* IOCTL:
 *	This symbol, if defined, indicates that sys/ioctl.h exists and should
 *	be included.
 */
#define	IOCTL		/**/

/* NORMSIG:
 *      This symbol, if defined, indicates that normal signal handling routines
 *      should be used, as opposed to the ones in 4.1bsd (sigset, etc.).
*/
#define NORMSIG         /**/

/* PORTABLE:
 *	This symbol, if defined, indicates to the C program that it should
 *	not assume that it is running on the machine it was compiled on.
 *	The program should be prepared to look up the host name, translate
 *	generic filenames, use PATH, etc.
 */
#define	PORTABLE	/**/

/* SIGNEDCHAR:
 *	This symbol, if defined, indicates that characters are a signed type.
 *	If not defined, things declared as signed characters (and that make
 *	use of negative values) should probably be declared as shorts instead.
 */
#define	SIGNEDCHAR	/**/

/* HOSTNAME:
 *	This symbol contains name of the host the program is going to run on.
 *	The domain is not kept with hostname, but must be gotten from MYDOMAIN.
 *	The dot comes with MYDOMAIN, and need not be supplied by the program.
 *	If gethostname() or uname() exist, HOSTNAME may be ignored.
 */
/* MYDOMAIN:
 *	This symbol contains the domain of the host the program is going to
 *	run on.  The domain must be appended to HOSTNAME to form a complete
 *	host name.  The dot comes with MYDOMAIN, and need not be supplied by
 *	the program.  If the host name is derived from PHOSTNAME, the domain
 *	may or may not already be there, and the program should check.
 */
#define HOSTNAME "kazoo"		/**/
#define MYDOMAIN ".uucp"		/**/


/* PASSNAMES:
 *	This symbol, if defined, indicates that full names are stored in
 *	the /etc/passwd file.
 */
/* BERKNAMES:
 *	This symbol, if defined, indicates that full names are stored in
 *	the /etc/passwd file in Berkeley format (name first thing, everything
 *	up to first comma, with & replaced by capitalized login id, yuck).
 */
#define	PASSNAMES /*  (undef to take name from ~/.fullname) */
#define	BERKNAMES /* (that is, ":name,stuff:") */

/* PREFSHELL:
 *	This symbol contains the full name of the preferred user shell on this
 *	system.  Usual values are /bin/csh, /bin/ksh, /bin/sh.
 */
#define PREFSHELL "/bin/csh"		/**/

/* RANDBITS:
 *	This symbol contains the number of bits of random number the rand()
 *	function produces.  Usual values are 15, 16, and 31.
 */
#define RANDBITS 15		/**/

/* Reg1:
 *	This symbol, along with Reg2, Reg3, etc. is either the word "register"
 *	or null, depending on whether the C compiler pays attention to this
 *	many register declarations.  The intent is that you don't have to
 *	order your register declarations in the order of importance, so you
 *	can freely declare register variables in sub-blocks of code and as
 *	function parameters.  Do not use Reg<n> more than once per routine.
 */

#define Reg1 register		/**/
#define Reg2 register		/**/
#define Reg3 register		/**/
#define Reg4 		/**/
#define Reg5 		/**/
#define Reg6 		/**/
#define Reg7 		/**/
#define Reg8 		/**/
#define Reg9 		/**/
#define Reg10 		/**/
#define Reg11 		/**/
#define Reg12 		/**/
#define Reg13 		/**/
#define Reg14 		/**/
#define Reg15 		/**/
#define Reg16 		/**/

/* ROOTID:
 *	This symbol contains the uid of root, normally 0.
 */
#define ROOTID 0		/**/
