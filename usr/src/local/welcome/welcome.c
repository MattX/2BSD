#include <time.h>
#include <stdio.h>
#include "cpu.h"
#include <nlist.h>

extern	struct nlist	namelist[];
extern	struct nlist	nl[];

struct nlist	nl[] = { { "_cpu" }, { "" } };
double ldvec[4];
int kmem;

struct nlist	namelist[] = {
	{ "_avenrun" },
	{ 0 }
};

char *days[] = { "Sunday", "Monday", "Tuesday", "Wednesday",
		 "Thursday", "Friday", "Saturday" };

char *mons[] = { "January", "February", "March", "April",
		 "May", "June", "July", "August",
		 "September", "October", "November", "December" };

char *ttime[] = { "morning ", "afternoon ", "evening ", "night " };

main()
{
	register struct tm *det;
	int a, b, x, y, num, cpu;
	long secs;
	char	bot[100], *foo, goo[50], ap;
	time(&secs);
#ifdef	LOAD
#if	defined(PDP)
	loadav(ldvec);
#else	defined(PDP)
	nlist("/vmunix", namelist);
	if (namelist[0].n_type == 1)
		puts("/vmunix no namelist\n"), exit(1);
	kmem = open("/dev/kmem", 0);
	if (kmem <= 0)
		printf("cannot open /dev/kmem\n"), exit(1);
	lseek(kmem, (long)namelist[0].n_value, 0);
	read(kmem, &ldvec[0], sizeof ldvec);
	close(kmem);
#endif	defined(PDP)
#endif	LOAD
#ifdef CPU
	nlist("/vmunix", nl);
	if (nl[0].n_type == 1)
		puts("/vmunix no namelist\n"), exit(1);
	kmem = open("/dev/kmem", 0);
	if (kmem <= 0)
		printf("cannot open /dev/kmem\n"), exit(1);
	lseek(kmem, nl[0].n_value, 0);
	read(kmem, &cpu, sizeof(cpu));
	close(kmem);
#endif CPU
	ap = "AP"[(det = localtime(&secs))->tm_hour >= 12];
	if (det->tm_hour > 0 && det->tm_hour < 12)
		foo = ttime[0];
	else if (det->tm_hour > 11 && det->tm_hour < 18)
		foo = ttime[1];
	else if (det->tm_hour > 17 && det->tm_hour < 24)
		foo = ttime[2];
	if ((det->tm_hour %= 12) == 0)
		det->tm_hour = 12;
	fflush(stdout);
	printf("\033[2J\033(0\033)0\033[m");
#ifdef	LOAD
	printf("\033[3;11H\033(B\033)B   Load Average\033(0\033)0");
	printf("\033[4;11Hlqqqqqqqqqqqqqqqqk");
	printf("\033[5;11Hx                x");
	printf("\033[6;11Hmqqqqqqqqqqqqqqwqj");
	printf("\033[5;13H%.02f %.02f %.02f", ldvec[0], ldvec[1], ldvec[2]);
	printf("\033[7;16Hlqqqqqqqqqvqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");
#else   LOAD
	printf("\033[7;16Hlqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");
#endif	LOAD
#ifdef	TTY
	printf("\033[3;51H\033(B\033)BTTY Name\033(0\033)0");
	printf("\033[4;46Hlqqqqqqqqqqqqqqqqk");
	printf("\033[5;46Hx                x");
	printf("\033[6;46Hmqwqqqqqqqqqqqqqqj");
	printf("\033[5;50H\033(B\033)B%s\033(0\033)0",ttyname(isatty(ttyslot())));
	printf("\033[7;16Hlqqqqqqqqqvqqqqqqqqqqqqqqqqqqqqqvqqqqqqqqk");
#else   TTY
	printf("\033[7;16Hlqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqk");
#endif	TTY
	printf("\033[8;16Hx                                        x");
	printf("\033[9;13Hlqqvqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqvqqk");
	printf("\033[10;10Hlqqvqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqvqqk");
	printf("\033[11;10Hx\033[11;63Hx\033[12;10Hx\033[12;63Hx\033[13;10Hx\033[13;63Hx");
	printf("\033[14;10Hmqqwqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqwqqj");
	printf("\033[15;13Hmqqwqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqwqqj");
	printf("\033[16;16Hx                                        x");
	printf("\033[17;16Hmqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqj");
	printf("\033(B\033)B");
#ifdef  LIGHT
	printf("\033[11;13H\033[7m                                                ");
#endif  LIGHT
	printf("\033[11;13H    Good %s%s,", foo, getenv("USER"));
	gethostname(goo, sizeof(goo));
#ifdef  LIGHT
	printf("\033[12;13H\033[7m                                                ");
	printf("\033[12;13H\033[7m        You have connected to %s", goo);
#else   LIGHT
	printf("\033[12;13H        You have connected to %s", goo);
#endif
#ifdef  LIGHT
	printf("\033[13;13H\033[7m                                                ");
#endif  LIGHT
#ifdef  CPU
	printf("\033[m");
	switch(cpu) {
		case VAX_780:
#ifdef LIGHT
			printf("\033[13;13H\033[7m        A VAX 11/780 running Ultrix");
#else LIGHT
			printf("\033[13;13H        A VAX 11/780 running Ultrix");
#endif LIGHT
			break;
		case VAX_750:
#ifdef LIGHT
			printf("\033[13;13H\033[7m        A VAX 11/750 running Ultrix");
#else LIGHT
			printf("\033[13;13H        A VAX 11/750 running Ultrix");
#endif LIGHT
			break;
		case VAX_730:
#ifdef LIGHT
			printf("\033[13;13H\033[7m        A VAX 11/730 running Ultrix");
#else LIGHT
			printf("\033[13;13H        A VAX 11/730 running Ultrix");
#endif LIGHT
			break;
		case VAX_8600:
#ifdef LIGHT
			printf("\033[13;13H\033[7m        A VAX 8600 running Ultrix");
#else LIGHT
			printf("\033[13;13H        A VAX 8600 running Ultrix");
#endif LIGHT
			break;
		case VAX_8200:
#ifdef LIGHT
			printf("\033[13;13H\033[7m        A VAX 8200 running Ultrix");
#else LIGHT
			printf("\033[13;13H        A VAX 8200 running Ultrix");
#endif LIGHT
			break;
		case VAX_8800:
#ifdef LIGHT
			printf("\033[13;13H\033[7m        A VAX 8800 running Ultrix");
#else LIGHT
			printf("\033[13;13H        A VAX 8800 running Ultrix");
#endif LIGHT
			break;
		case MVAX_I:
#ifdef LIGHT
			printf("\033[13;13H\033[7m        A MicroVAX I running Ultrix");
#else LIGHT
			printf("\033[13;13H        A MicroVAX I running Ultrix");
#endif LIGHT
			break;
		case MVAX_II:
#ifdef LIGHT
			printf("\033[13;13H\033[7m        A MicroVAX II running Ultrix");
#else LIGHT
			printf("\033[13;13H        A MicroVAX II running Ultrix");
#endif LIGHT
			break;
		default:
			printf("\033[13;13H        CPU ident error");
			break;
	}
#else   CPU
#ifdef LIGHT
#if defined(PDP)
	printf("\033[13;13H\033[7m        A PDP-11 running 2.11BSD");
#else defined(PDP)
	printf("\033[13;13H\033[7m        A system running Ultrix");
#endif defined(PDP)
#else  LIGHT
#if defined(PDP)
	printf("\033[13;13H        A PDP-11 running 2.11BSD");
#else defined(PDP)
	printf("\033[13;13H        A system running Ultrix");
#endif defined(PDP)
#endif LIGHT
#endif  CPU
#ifdef	UVERS
	fp = fopen("/etc/motd", "r");
	fscanf(fp, "%s%s", poo, doo);
	printf(" V%c.%c", doo[1], doo[3]);
	fclose(fp);
	unlink("/tmp/foo");
#endif	UVERS
#ifdef	LIGHT
	printf("\033[8;18H                                      ");
	printf("\033[16;18H                                      ");
#endif	LIGHT
	x = 40 - (strlen(days[det->tm_wday]) / 2);
	y = (69 - x);
	printf("\033[8;%dH%s", y, days[det->tm_wday]);
	sprintf(bot, "%s %d, 19%d %d:%02d %cM", mons[det->tm_mon],
	        det->tm_mday, det->tm_year, det->tm_hour,
		det->tm_min, ap);
	a = 40 -(strlen(bot) / 2);
	b = (54 - a);
	printf("\033[m");
#ifdef LIGHT
	printf("\033[7m");
#endif LIGHT
	printf("\033[16;%dH%s", b, bot);
	printf("\033[23;1H");
	printf("\033[m");
	exit(0);
}
