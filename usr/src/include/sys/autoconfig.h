/*
 * Possible error codes of the auto configuration program
 */
#define AC_OK		0	/* Everything A-OK for system go */
#define AC_SETUP	1	/* Error in initializing autoconf program */
#define AC_SINGLE	2	/* Non serious error, come to single user */
#define ACP_NXDEV	-1	/* no such device */
#define ACP_IFINTR	0	/* device exists if interrupts OK */
#define ACP_EXISTS	1	/* device exists */
#define ACI_BADINTR	-1	/* device interrupted through wrong vector */
#define ACI_NOINTR	0	/* device didn't interrupt */
#define ACI_GOODINTR	1	/* interrupt OK */
#define YES		1	/* general yes */
#define NO		0	/* general no */

/*
 * Magic number to verify that autoconfig runs only once.
 */
#define CONF_MAGIC	0x1960
