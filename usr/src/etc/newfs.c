/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#if	!defined(lint) && defined(DOSCCS)
char copyright[] =
"@(#) Copyright (c) 1983 Regents of the University of California.\n\
 All rights reserved.\n";

static char sccsid[] = "@(#)newfs.c	6.0 (2.11BSD) 12/28/91";
#endif

/*
 * newfs: friendly front end to mkfs
 */
#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <disktab.h>

main(argc, argv)
	int	argc;
	char	**argv;
{
	extern char	*optarg;
	extern int	optind;
	register struct disktab	*dp;
	register struct partition	*pp;
	register char	*cp;
	struct stat	st;
	long	fssize;
	int	ch, status, logsec, m;
	int	just_looking, copy_boot;
	char	*uboot;
	char	device[MAXPATHLEN], cmd[BUFSIZ],
		*index(), *rindex();

	just_looking = 0;
	copy_boot = 0;
	m = 0;
	uboot = (char *)0;
	while ((ch = getopt(argc,argv,"NvbB:m:")) != EOF)
		switch((char)ch) {
		case 'N':
		case 'v':
			++just_looking;
			break;
		case 'B':
			uboot = optarg;
			/*FALLTHROUGH*/
		case 'b':
			++copy_boot;
			break;
		case 'm':
			m = atoi(optarg);
			break;
		case '?':
		default:
			usage();
		}

	argc -= optind;
	argv += optind;
	if (argc != 2)
		usage();

	/* figure out device name */
	cp = rindex(argv[0], '/');
	if (cp)
		++cp;
	else
		cp = argv[0];
	if (*cp == 'r' && cp[1] != 'a' && cp[1] != 'b')
		++cp;
	sprintf(device, "/dev/r%s", cp);

	/* see if it exists and of a legal type */
	if (stat(device, &st)) {
		fprintf(stderr, "newfs: ");
		perror(device);
		exit(1);
	}
	if ((st.st_mode & S_IFMT) != S_IFCHR) {
		fprintf(stderr, "newfs: %s: not a character device.\n", device);
		exit(1);
	}

	/* see if disk is in disktab table */
	dp = getdiskbyname(argv[1]);
	if (dp == 0) {
		fprintf(stderr, "newfs: %s not in /etc/disktab.\n", argv[1]);
		exit(1);
	}

	/* grab partition letter */
	cp = index(argv[0], '\0') - 1;
	if (cp == 0 || *cp < 'a' || *cp > 'h') {
		fprintf(stderr, "newfs: %s: can't figure out file system partition.\n", argv[0]);
		exit(1);
	}

	/* get default partition size */
	pp = &dp->d_partitions[*cp - 'a'];
	fssize = pp->p_size;
	if (fssize < 0) {
		fprintf(stderr, "newfs: %s: no default size for `%c' partition.\n", argv[1], *cp);
		exit(1);
	}
	/*
	 * Convert from sectors to logical blocks.  Note that sector size
	 * must evenly devide DEV_BSIZE!!!!!
	 */
	logsec = DEV_BSIZE/dp->d_secsize;
	fssize /= logsec;

	/* build command */
	if (m <= 0 || m > 31)
		m = 2;
	sprintf(cmd, "/etc/mkfs %s %ld %d %d", device, fssize, m,
	    (dp->d_ntracks * dp->d_nsectors) / logsec);
	printf("newfs: %s\n", cmd);
	if (just_looking)
		exit(0);
	if (status = system(cmd))
		exit(status >> 8);

	/* copy boot if requested */
	if (copy_boot) {
		if (!uboot && !(uboot = dp->d_uboot)) {
			fprintf(stderr, "newfs: no default boot block available for an %s.\n",
				argv[1]);
			exit(1);
		}
		sprintf(cmd, "/bin/dd if=%s of=%s bs=512 count=1 conv=sync",
		    uboot, device);
		if (status = system(cmd))
			exit(status >> 8);
	}
	exit(0);
}

static
usage()
{

	fputs("usage: newfs [ -N ] [ -b ] [ -B boot-block ] [-m free-list-gap] special-device device-type\n", stderr);
	exit(1);
}
