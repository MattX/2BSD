/*
 *			F I N D C O M . C 
 *
 * $Revision: 1.2 $
 *
 * $Log:	findcom.c,v $
 * Revision 1.2  83/12/16  00:07:05  dpk
 * Added distinctive RCS header
 * 
 */
#ifndef lint
static char RCSid[] = "@(#)$Header: findcom.c,v 1.2 83/12/16 00:07:05 dpk BRL $";
#endif

#include <stdio.h>
#include "jove_tune.h"

extern char	_sobuf[];

main(argc, argv)
int	argc;
char	*argv[];
{
	register FILE	*fp;
	register char	*com;
	register int	len;
	char	line[200];

	if (argc != 2) {
		printf("Incorrect number of arguments to findcom\n");
		exit(1);
	}
	if ((fp = fopen(DESCRIBE, "r")) == NULL) {
		printf("Cannot open %s\n", DESCRIBE);
		exit(1);
	}

	com = argv[1];
	len = strlen(com);

	setbuf(stdout, _sobuf);
	while (fgets(line, 200, fp)) {
		if (line[0] != '\n')
			continue;
		/* Next line begins a topic */
		fgets(line, 200, fp);
		if (strncmp(line + 5, com, len))
			continue;
		printf("%s\n", line);
		while (fgets(line, 200, fp) && line[0] != '\n')
			printf(line);
		putchar('\n');
		break;
	}
	fclose(fp);
}
