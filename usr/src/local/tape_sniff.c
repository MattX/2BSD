#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/mtio.h>
#include <sys/ioctl.h>

/*
 *	catalog -- catalog a mag tape
 *	<tape>: "0" or "1"... or "/dev/rmt1", etc.
 */

#define MAXBUF	65535		/* DEC 16 bit tape register limit */
#define UNVAL	-2		/* hopefully impossible buffer size */
#define NO	0
#define YES	1

static int	count;		/* records read */

main(argc,argv)
int	argc;
char	**argv;
{
	extern int	optind;
	extern char	*optarg;
	static long	bufsz = MAXBUF;
	int	fd,
		c,
		eof = NO,		/* if two eofs mean stop */
		seven = NO,		/* if 7-track tape mode */
		size,			/* return from read */
		skip = 0,		/* files to skip */
		lastsize = UNVAL,	/* last read size */
		maxnum = 0,		/* max files to catalog */
		fnum = 0,		/* file number we're on */
		onint();
	char	*buf,			/* buf to read into */
		*myname,		/* name program called with */
		*tape = "/dev/rmt0",	/* device to catalog */
		*malloc();

	myname = *argv;
	while ((c = getopt(argc,argv,"7b:en:s:")) != EOF)
		switch((char)c) {
			case '7':
				seven = YES;
				break;
			case 'b':
				bufsz = atoi(optarg) * 1024;
				break;
			case 'e':
				eof = YES;
				break;
			case 'n':
				maxnum = atoi(optarg);
				break;
			case 's':
				fnum = skip = atoi(optarg);
				break;
			default:
				fprintf(stderr,"usage: %s [-7] [-e] [-b bufsiz] [-n maxnum] [-s skip] tape\n",*argv);
				exit(1);
		}
	if (!(buf = malloc((unsigned)bufsz))) {
		fprintf(stderr,"%s: malloc error.\n",myname);
		exit(1);
	}
	if (*(argv += optind))
		if (**argv >= '0' && **argv <= '7') tape[8] = **argv;
		else tape = *argv;
	if ((fd = open(tape,0)) == -1) {
		perror(tape);
		exit(1);
	}
	if (skip) {
		struct mtop	mt_com;
		mt_com.mt_op = MTFSF;
		if ((mt_com.mt_count = skip) < 0) {
			fprintf(stderr,"%s: negative number of files to skip.\n",myname);
			exit(1);
		}
		if (ioctl(fd,MTIOCTOP,&mt_com) < 0) {
			fprintf(stderr,"%s: %s skip %d ",myname,tape,mt_com.mt_count);
			perror("failed");
			exit(1);
		}
	}
	printf("catalog of %s%s\n\tsize\tcount\n",tape,seven ? "  7-track mode" : "");
	signal(SIGINT,onint);
	for (count = 0;;) {
		while ((size = read(fd,buf,bufsz)) > 0 && size == lastsize) ++count;
		if (count) printf("%9d\n",count);
		switch (size) {
			case -1:
				printf("read error terminates file #%d\n",fnum);
				exit(0);
			case 0:
				if (eof && lastsize == UNVAL) {
					puts("double end of file; end of tape.\n");
					exit(0);
				}
				printf("end of file #%d\n\n",++fnum);
				if (maxnum && fnum == maxnum) exit(0);
				lastsize = UNVAL;
				count = 0;
				break;
			default:
				lastsize = size;
				count = 1;
				printf("%12d",seven ? (int)(size * .750) : size);
		}
	}
}

static
onint()
{
	printf("%9d+\n",count);
	exit(0);
}
