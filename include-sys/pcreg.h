/* Registers and bits for the pro3xx serial ports "pc" */
#define BITS7   0112
#define BITS8   0116
#define TWOSB   0200
#define PENABLE 020
#define EPAR    040
#define PERROR  010
#define FRERROR 040
#define DIVR    0260
#define RXDONE  02
#define TXDONE  01
#define GOCMD   067
#define DSRON   0200
#define PCDELAY 4

#define ERESET  060
#define RXCA    01
#define CFERROR 0100
#define CPERROR 020
#define CTXDONE 04
#define CARDTC  020
#define INTRP   02
#define EOFINTR 070
#define REXINTR 020
#define CHNRST  030
#define RTINTP  050

#define TRAN    0
#define XTERN   1
#define RCV     2
#define SRCV    3

#define ON      1
#define OFF     0
#define SSPEED  7       /* standard speed: 300 baud */

struct pcdevice {
        int     cdb;
        int     csa;
        int     dumb;
        int     csb;
        int     mc0;
        int     mc1;
        int     baud;
        int     dud[25];
        int     dbuf;
        int     stat;
        int     mode;
        int     csr;
};
