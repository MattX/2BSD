/*
 *
 *  finger [login]     Stuart Cracraft         October 1980
 *
 *      Put the finger on people.
 *
 */

#include <stdio.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <utmp.h>
#include <pwd.h>
#include <ctype.h>
#include <lastlog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef	MICOM
#include <local/micom.h>
#endif	MICOM

struct sockaddr_in hisctladdr = {AF_INET, 79 };
char buf[256];

#define FALSE	0
#define	TRUE	1

#define	DIV60(t)	((t+30)/60)/* x/60 rounded */
char   *fields =
       " User     Personal Name        Tty      Idle  Login         Location";
char   *lastlog = "/usr/adm/lastlog";
struct lastlog ll;
struct utmp utmp;
struct passwd  *pw,
               *getpwnam ();

#ifdef	MICOM
struct micinfo micinfo[NTOTAL];
short conninfo[NTOTAL];
struct lineinfo *entry;
#endif	MICOM

struct userent
{
    char    uname[15];		/* User name */
    time_t idle;		/* number of minutes user is idle */
    char    tty[8];		/* Tty name */
    time_t time;		/* Time on */
    char    personal[20];	/* Personal name */
    char    location[14];	/* location */
};

#define	MAXFUSERS	40	/* max logged users we can handle */
struct userent  finger[MAXFUSERS];/* Accumulate finger info here */

int     tflg,
        oflg,
        lflg,
        iflg;			/* Sort features */
int     singleflg;		/* Doing single user? */
int     rflg = 1;		/* Default is sort alphabetic */
int     savargc;		/* Saves argc */
int     found = FALSE;		/* If single user found */
char    uhack[9];		/* Cretinous who truncates */
char    lastperson[15];		/* Last uname typed out */
char    singleuser[15];		/* Single user to finger */
char	sysname[20];		/* local system name */
int     numentries = 0;		/* Number user entries */
int     numfing = 0;		/* Number finger entries */
char   *ttyname (), *rindex (), *ctime (), *strcpy (), *index (), *malloc();
long    lseek (), findidle ();
int     compar ();

main (argc, argv)
char  **argv;
{
    register char  *tp;
    register    FILE * fi;
    char *p,user[20],host[20];
    int     i, j, netfd;
    long hostnum, rhost();

    setbuf(stdout, 0);
    if (argc > 1 && index(argv[1],'@')) /* Want to finger remote? */
    {
	for (i = j = 0; argv[1][i] != '@'; i++)	/* Copy user */
	    user[j++] = argv[1][i];
	for (i++, j = 0; argv[1][i] != NULL; i++)	/* Copy host */
	    host[j++] = argv[1][i];
	p = malloc (strlen (host) + 1);		/* Stash host someplace */
	strcpy (p, host);
        hostnum = rhost (&p);		/* Get host number */
        if (hostnum == -1)
        {
	    fprintf (stderr, "Unknown hostname: %s\n", host);
	    exit(1);
	}
        hisctladdr.sin_port = htons (hisctladdr.sin_port);
        hisctladdr.sin_addr.s_addr = hostnum;
	if ((netfd = socket(SOCK_STREAM, 0, (struct sockaddr *)0, 0)) < 0
	    || connect(netfd, &hisctladdr)) {
		perror(raddr(hisctladdr.sin_addr.s_addr));
		exit(1);
	}
	fprintf(stdout,"[%s]\n",host);
	if (user[0])
	    sprintf(buf, "%s\r\n", user);	/* Specific user */
	else
	    sprintf(buf, "\r\n");		/* Everyone */
	write (netfd, buf, strlen(buf));
	while ((i = read(netfd, buf, sizeof(buf))) > 0) write(1, buf, i);
 	close(netfd);
	exit(0);
    }
    iflg = lflg = oflg = tflg = 0;
    savargc = argc;
    getmfile ();
    if (--argc > 0 && *argv[1] == '-')
    {
	argv++;
	while (*++*argv)
	    switch (**argv)
	    {
		case 'r': 
		    rflg = -1;
		    continue;
		case 'l': 
		    lflg++;
		    continue;
		case 'i': 
		    iflg++;
		    continue;
		case 'o': 
		    oflg++;
		    continue;
		case 't': 
		    tflg++;
		    continue;
		default: 
		    continue;
	    }
	argc--;
    }
    if (argc > 0)
    {
	strcpy (singleuser, *++argv);
	singck (singleuser);	/* Make sure he's valid */
	singleflg++;
    }
    if ((lflg + oflg + tflg) > 1)
    {
	printf ("Too many flags.\n");
	exit (1);
    }
    if (!singleflg)
	printf ("%s\n", fields);
    if ((fi = fopen ("/etc/utmp", "r")) == NULL)
    {
	puts ("finger: cannot open utmp");
	exit (1);
    }
    while (fread ((char *) & utmp, sizeof (utmp), 1, fi) == 1)
    {
	if (utmp.ut_name[0] == '\0')
	    continue;
	for (i = 0; utmp.ut_name[i] != '\0' && i < 8; i++)
	    uhack[i] = utmp.ut_name[i];
	uhack[i++] = '\0';
	if (!singleflg || (singleflg && (strncmp (singleuser, uhack, 8) == 0)))
	{
	    pw = getpwnam(uhack);
	    saveline ();
	    found = TRUE;
	}
    }
    if (singleflg && !found)
    {
	if (!oneuser ())
	    printf ("Sorry, no information on %s.\n", singleuser);
    }
    else
    {
	qsort (finger, numfing, sizeof (struct userent), compar);
	showall ();
    }
}

singck (user)
char    user[];
{
    int i;
    char tuser[15];

    if ((pw = getpwnam (user)) == NULL)
    {
	for (i = 0; user[i] != '\0'; i++)
	    if (isupper(user[i]))
		tuser[i] = tolower(user[i]);
	if ((pw = getpwnam (tuser)) == NULL)
	{
	    printf ("%s not a valid user.\n", user);
	    exit (1);
	}
    }
}

showplan (user)
char    user[];
{
    int     planfd,
            i;
    char    planbuf[512];
    char    planfile[40];

    if ((pw = getpwnam (user)) != NULL)
    {
	strcpy (planfile, pw -> pw_dir);
	strcat (planfile, "/.plan");
	if ((planfd = open (planfile, 0)) < 0)
	    printf ("\n[No plan]\n");
	else
	{
	    printf ("\nPlan:\n");
	    while ((i = read (planfd, planbuf, 512)) > 0)
		write (1, planbuf, i);
	    close (planfd);
	    printf ("\n");
	}
    }
}

oneuser ()
{
    int     found = FALSE,
            logout = FALSE,
	    hisuid;
    time_t lasttime,logtim;
    struct utmp wbuff[10];
    register int    wtmp, llfd;
    register struct utmp   *wptr;
    char    logtty[10];

    hisuid = getpwnam(singleuser) ->pw_uid;
    if ((llfd = open (lastlog, 0)) < 0)
    {
	printf("can't open %s\n",lastlog);
	return(FALSE);
    }
    lseek (llfd, (long) ((unsigned) hisuid * sizeof (struct lastlog)), 0);
    wtmp = read (llfd, (char *) &ll, sizeof ll);
    close(llfd);
    if (wtmp != sizeof ll)
	return(FALSE);
    if ((wtmp = open ("/usr/adm/wtmp", 0)) == -1)
    {
	perror ("/usr/adm/wtmp");
	return (FALSE);
    }
    lseek (wtmp, (long) sizeof (wbuff), 2);
    while (lseek (wtmp, -2 * (long) sizeof (wbuff), 1) > sizeof (wbuff))
    {
	read (wtmp, wbuff, sizeof (wbuff));
	for (wptr = &wbuff[9]; wptr >= wbuff; wptr--)
	    if (strncmp (wptr -> ut_name, singleuser, 8) == 0)
	    {
		logtim = wptr -> ut_time;
		strcpy (logtty, wptr -> ut_line);
		for (wptr++; wptr <= &wbuff[9]; wptr++)
		    if (strncmp (logtty, wptr -> ut_line, 8) == 0)
		    {
			logtim = wptr -> ut_time;
			logout = TRUE;
			goto showit;
		    }
		goto fndout;
	    }
    }
	/* Couldn't find him in wtmp. Try /usr/adm/lastlog */
	if (ll.ll_time != 0L)
    {
	logtim = ll.ll_time;
	logout = FALSE;
	strcpy(logtty, ll.ll_line);
	goto showit;
    }
    else {
	return(FALSE);
    }
fndout: 
    while (read (wtmp, wbuff, sizeof (wbuff)) > 0)
	for (wptr = wbuff; wptr <= &wbuff[9]; wptr++)
	    if (strncmp (wptr -> ut_line, logtty, 8) == 0)
	    {
		logout = TRUE;
		logtim = wptr -> ut_time;
		goto showit;
	    }
showit: 
    printf ("%s (%s) last %s at %-24.24s ",
	    singleuser,
	    pw->pw_gecos,
	    logout ? "logout" : "login",
	    ctime (&logtim));
	    if (*ll.ll_host == '\0')
		printf("on %s\n", logtty);
	    else
		printf("from %s\n", ll.ll_host);
    showplan (singleuser);
    return (TRUE);
}

compar (pp1, pp2)
struct userent *pp1,
           *pp2;
{
    register struct userent   *p1,
                           *p2;

    p1 = pp1;
    p2 = pp2;
    if (iflg == 1)
    {
	if (p2 -> idle == p1 -> idle)
	    return (0);
	if (p2 -> idle > p1 -> idle)
	    return (1);
	return (-1);
    }
    if (lflg == 1)
    {
	if (p2 -> time == p1 -> time)
	    return (0);
	if (p2 -> time > p1 -> time)
	    return (rflg);
	return (-rflg);
    }
    if (oflg == 1)
	return (rflg * strcmp (p1 -> location, p2 -> location));
    if (tflg == 1)
	return (rflg * strcmp (p1 -> tty, p2 -> tty));
    return (rflg * strcmp (p1 -> uname, p2 -> uname));
}

#ifdef	MICOM
userinfo (ttynm)
char    ttynm[];
{
    register     i;
    int          found = FALSE;
    register struct portinfo *pp;

    for (i = 0; i < NTOTAL; i++)
    {
	pp = &micinfo[i].m_un.m_port;
	if (micinfo[i].m_type == PORT && 
	    !strcmp(sysname, pp->p_misc) &&
	    !strcmp(ttynm, pp->p_name))
	{
	    found = TRUE;
	    entry = &micinfo[conninfo[i]].m_un.m_line;
	    break;
	}
    }
    if (!found)
	entry = NULL;
}
#endif	MICOM

getmfile ()
{
    int i, ufd;

    gethostname(sysname, sizeof sysname);
    for (i=0; sysname[i] && i < (sizeof sysname); i++)
    	if (islower(sysname[i]))
		sysname[i]=toupper(sysname[i]);
#ifdef	MICOM
    if ((ufd = open (MICOMINFO, 0)) < 0)
    {
	perror(MICOMINFO);
	exit (1);
    }
    read(ufd, (char *)micinfo, sizeof micinfo);
    close (ufd);
    if ((ufd = open(CONNFILE, 0)) < 0)
    {
    	perror(CONNFILE);
	exit(1);
    }
    read(ufd, (char *)conninfo, sizeof conninfo);
    close(ufd);
#endif	MICOM
}

saveline ()
{
    char *tptr;
	int llfd;

#ifdef	MICOM
    userinfo (utmp.ut_line);
#endif	MICOM
    strcpy (finger[numfing].uname, utmp.ut_name);
    if (tptr = index(pw->pw_gecos, ','))
    	*tptr = NULL;
    strcpy (finger[numfing].personal, pw->pw_gecos);
    finger[numfing].idle = findidle ();
    strcpy (finger[numfing].tty, utmp.ut_line);
#ifndef	MICOM
    strcpy (finger[numfing].location, utmp.ut_host);
#else
    strcpy (finger[numfing].location, "");
    if (entry)
	strcpy (finger[numfing].location, entry->l_loc);
    else
	if (!strncmp(utmp.ut_line,"ttyp",4) && (llfd = open(lastlog, 0)) > 0){
	    lseek(llfd, (long)((unsigned)pw->pw_uid*sizeof(struct lastlog)),
		0);
	    if((read(llfd,(char *)&ll,sizeof ll)==sizeof ll)&& ll.ll_time!=0L)
		strcpy(finger[numfing].location, ll.ll_host);
	    close(llfd);
	}
#endif	MICOM
    finger[numfing].time = utmp.ut_time;
    numfing++;
}
showall ()
{
    register int    didhrs;
    register char  *cbuf;
    char    idlehr[4];
    char    idlemi[4];
    char    idlest[8];
    time_t idle, tim;
    int     i;

    for (i = 0; i < numfing; i++)
    {
	didhrs = 0;
	cbuf = ctime (&finger[i].time);
	tim = finger[i].idle;
	if (tim >= 60)
	{
	    sprintf (idlehr, "%3ld:", tim / 60);
	    didhrs++;
	}
	else
	    sprintf (idlehr, "    ");
	tim %= 60;
	if (tim > 0 || didhrs)
	    sprintf (idlemi, didhrs && tim < 10 ? "%02ld" : "%2ld", tim);
	else
	    sprintf (idlemi, "  ");
	strcpy (idlest, idlehr);
	strcat (idlest, idlemi);
	if (*finger[i].personal && !strcmp (finger[i].personal, lastperson))
	{
	    printf("                              %-8.7s%s %-14.12s%-14.13s\n"
		    ,finger[i].tty,
		    idlest,
		    cbuf + 4,
		    finger[i].location);
	}
	else
	{
	    strcpy (lastperson, finger[i].personal);
	    printf ("%-9.8s%-21.20s%-8.7s%s %-14.12s%-14.13s\n",
		    finger[i].uname,
		    finger[i].personal,
		    finger[i].tty,
		    idlest,
		    cbuf + 4,
		    finger[i].location);
	}
    }
}
/* find & return number of minutes current tty has been idle */
long    findidle ()
{
    struct stat stbuf;
    time_t lastaction, diff, now;
    char    tername[20];

    strcpy (tername, "/dev/");
    strncat (tername, utmp.ut_line, 8);
    stat (tername, &stbuf);
    time (&now);
    lastaction = stbuf.st_atime;
    diff = now - lastaction;
    diff = DIV60 (diff);
    if (diff < 0)
	diff = 0;
    return (diff);
}
