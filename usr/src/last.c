#
	char	*sccsid = "@(#)last.c	2.4";

char	*calloc();
/*
 * last
 * prints out the last logins of users
 * last with no options prints out EVERYONES last logins
 * last <namelist> will print out the last logins of everyone on <namelist>
 * last -t <ttylist> will print out the peolpe who haave logged in on the named tty's
 *
 * Author: Joe Kalash
 */
# include	<stdio.h>
# include	<utmp.h>
# include	<signal.h>
# define	reg	register



# define	TABLESIZE	100
# define	BLK		1000
# define	offline(a)	(( a->ut_name[0] == 'd' && a->ut_name[1] == 'z') || ( a->ut_name[0] == 'm' && a->ut_name[1] == 'x'))

/*
 * structure where the ttynames, and login names are kept
 */

struct	list {
	long	time;
	char	*name;
	struct	list	*next;
}   *ttyname[TABLESIZE], *logname[TABLESIZE];

struct	other {
	struct other	*Next;
	char	*c;
} *top;

struct	utmp	*List;
/*
 * the hash function. Takes a word, and makes a distinctive number for it
 */

int	hash(neword)
reg	char	*neword;		/* the word that is being hashed */
{
	reg	int	i = 0;
	reg	int	j = 0;
	char	tmp;

	while ( j++ <= 7 && *neword ) 
	{
		tmp = *neword++;
		i = i * (tmp) + tmp;

	}
	if ( i < 0 )
		i = -i;
	return ( (i % TABLESIZE) );
}

/*
 * look for word 'neword' in hash Table
 */

struct	list	*lookup(neword,Table)
reg	char	*neword;		/* the word it self */
reg	struct	list	*Table[];	/* table to look into */
{
	reg	struct	list	*pt;	/*  pointer, used for dirfping hash Table*/
	int	hash();				/* hash function */


	pt = Table[hash(neword)];
	for ( ; pt > NULL ; pt = pt->next)	/* move along the list */
		if ( !strcmp(neword,pt->name) )
			return(pt); 		/* return pointer, if it is found */
	return(NULL);			/* entry not found */
}



/*
 * save a copy of the string 'word', some place.
 */

char	*strsave(word)
reg	char	*word;
{
	char 	*place;
	reg	char	*tmp;
	reg	int	i;
	char	*calloc();

	if ( ( i = strlen(word) ) >= 8 )
		i = 8;
	if ((place = calloc(1, i+1 )) <= NULL  ) 
		Perror("strsave",1);	/* too much space allocated */
	tmp = place;
	i = 0;
	while ( i++ <= 7 &&  *word )
		 *tmp++ = *word++;
	*tmp = 0;
	return(place);
}

/*
 *place the name in the hash Table
 */



insert(person,Table,time)
reg	char	*person;
reg	struct	list	*Table[];
long	time;
{
	reg	struct	list	*pt;
	struct	list	*tmp;
	int	exit();
	char	*strsave();
	int	hashval;	/* easy to access */

	if ( ( tmp = lookup(person,Table) ) != NULL )
	{
		tmp->time = time;
		return;
	}
	alarm(10);
	signal(SIGALRM,exit);
	if ( ( pt = malloc (sizeof  (struct list))) <= NULL)
		Perror("insert");

	signal(SIGALRM,1);
	alarm(0);
	pt->name = strsave(person);	/* put file name into pointer */
	pt->time = time;
	hashval = hash(person);		/* hash word */
	pt->next = Table[hashval];	/* place whatever is in the table on the end of the link */
	Table[hashval] = pt;		/* make it the new head of the symbol table */
}

Perror(str,val)
reg	char	*str;
reg	int	val;
{
	perror(str);
	exit(val);
}

int	cflg = 0;

main(ac,av)
int	ac;
char	**av;
{
	reg	struct	utmp	*Wtmp, Rtmp[BLK];

	int	orig;
	char	*wtmp = "/usr/adm/wtmp";
	long	new , Rsize, tvec,  lseek(), size, tell();
	extern	char	_sobuf[];
	int	onintr();
	reg	int	fp;
	int	i;
	int	allflag = 0;				/* print EVERYONE */
	int	ttyflag = 0;				/* Charisse wanted a tty option */
	int	perflag = 1;				/* just print people */

	char	*ctime();
	int	exit();

	init(ttyname);
	init(logname);
	
	ac--;
	av++;
	signal(SIGINT,onintr);
	while ( av[0][0] == '-' )
	{
		switch(av[0][1] )
		{
			case 'c':
				  cflg++;
				  perflag = ttyflag = allflag = 0;
				  break;
			case 't':
				  perflag = 0;
				  ttyflag = 1;
				  break;
			case 'f':
				  wtmp = *++av;
				  ac--;
				  break;
			default :
				  fprintf(stderr,"no such option %c\n",av[0][1]);
				  exit(2);
		}
		av++;
		ac--;
	}
	if ( ac == 0 && !cflg )
	{
		ttyflag = perflag = 0;
		allflag = 1;
	}

	if ( ( fp = open(wtmp,0) ) < 0 )
		Perror(wtmp);
	while ( ac-- > 0 )
		insert(*av++,logname,0L);

	Rsize =  (long) sizeof ( Rtmp);

	if ( lseek(fp, (long) 0, 2) < (long)0 )
		Perror("seek1");		/* end of file */
	size = tell(fp);

	if ( lseek(fp,(long)(size - Rsize),0) < (long)0 )
		if ( lseek(fp,0L,0) < (long)0 )
			Perror("seek2");

	size -= (long) Rsize;
	setbuf(stdout,_sobuf); 
	List = &Rtmp[0];
	time(tvec);
	printf("%8.8s %8.8s %24.24s \t %20.20s %s"," Login "," Term"," Login Time       "," Logout Time    ","e-time\n");
	printf("==============================================================================\n");
	fred:

	while ( ( i = read(fp,Rtmp,(int)Rsize) )> 0 )
	{

		i /= sizeof (struct utmp );
		orig = i;

		/*
		 * cycle through the entrys processing them
		 */

		for ( i-- ; i >= 0 ; i-- )
		{
			Wtmp = &Rtmp[i] ;
			if ( !Wtmp->ut_name[0] )
			{
				if ( !strcmp("~",Wtmp->ut_line) )
					crash(Wtmp);
				else
					if ( !cflg &&  Wtmp->ut_line[0] == 'd' || Wtmp->ut_line[0] == 'm' || Wtmp->ut_line[0] == 'c')
						insert(Wtmp->ut_line,ttyname,Wtmp->ut_time);
			}
			else
			{
				if ( offline(Wtmp) )
				{
					i++;
					lseek(fp,(long)(size ),0);
					size += 8L;
					new =  -(long)(i*sizeof ( struct utmp)) ;
					lseek(fp,(new +8L),1);
					fflush(stdout);
					goto fred;
				}
				else
				{
					if ( cflg )
						continue;
					if ( allflag || ( perflag && lookup(Wtmp->ut_name,logname) ) || ( ttyflag && lookup(Wtmp->ut_line,logname)) )
					{
						if ( Wtmp->ut_time > 0L )
						{
							printf("%8.8s %8.8s %24.24s \t",Wtmp->ut_name,Wtmp->ut_line,ctime(&Wtmp->ut_time));
							remove(Wtmp);
							fflush(stdout);
						}
					}
				}
			}
		}

		lseek(fp,-(2*Rsize),1) ;
		size -= (long) Rsize;
	}
	for ( i = 0 ; i < TABLESIZE ; i++ )
		if ( Rtmp[i].ut_time > 0L )
		{
			printf("\nWtmp begins at %s\n",ctime(&Rtmp[i].ut_time));
			break;
		}
}



remove(Wtmp)
reg	struct	utmp	*Wtmp;
{

	reg	struct	list	*pt;
	char	*ctime();

	pt = lookup(Wtmp->ut_line,ttyname);

	if ( pt != NULL )
	{
		if ( pt->time == (long) -1 )
			puts(" system crash");
		else
		{
			printf(" %20.20s",ctime(&pt->time));
			timeon(Wtmp->ut_time,pt->time);
		}
	}
	else
		puts("  still logged in");
}


/*
 * indicate a crash, by setting the time to -1
 */
crash(Wtmp)
reg	struct	utmp	*Wtmp;
{
	reg	struct	list	*tmp;
	reg	int	i;
	extern	char	*ctime();

	if ( cflg )
	{
		printf("System Crashed at %s",ctime(&Wtmp->ut_time));
		fflush(stdout);
		return;
	}

	for ( i = 0 ; i < TABLESIZE ; i++ )
	{
		tmp = ttyname[i];
		while ( tmp != NULL )
		{
			tmp->time = (long) -1;
			tmp = tmp -> next;
		}
	}
}

strcmp(a,b)
reg	char *a, *b;
{
	reg	int	i;

	for ( i = 0 ; i < 8 ; i++ )
	{
		if ( *a != *b++ )
			return(1);
		if ( !*a++ )
			return(0);
	}
	return(0);
}

timeon(in,out)
long	in,
	out;
{
	char	sp[20];
	int	days;
	reg	int	hrs,
			mins,
			sec;
	
	out = out - in;

	sec = (int) ( out % (long) 60 );
	mins = (int)(out / (long) 60 );
	hrs = mins / 60;
	mins %= 60;
	days = hrs / 24;
	hrs %= 24;

	if ( days > 0 )
		sprintf(sp,"(%d:%d:%d:%d)",days,hrs,mins,sec);
	else
		sprintf(sp,"(%d:%d:%d)",hrs,mins,sec);
	puts(sp);
}

onintr()
{
	char	*ctime();
	short	i;

	for ( i = 0 ; i < BLK ; i++ )
	{
		if ( List->ut_time > 0L  )
		{
			printf("\nInterrupted at %s",ctime(&List->ut_time));
			exit(0);
		}
		List++;
	}

	printf("\n");
	exit(0);
}

init(table)
struct	list	*table[];
{
	short	i;
	table = malloc(2*TABLESIZE);
}

