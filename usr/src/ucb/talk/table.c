/* $Header: /a/guest/moore/talk/RCS/table.c,v 1.9 83/07/06 00:11:38 moore Exp $ */

/* routines to handle insertion, deletion, etc on the table
   of requests kept by the daemon. Nothing fancy here, linear
   search on a double-linked list. A time is kept with each 
   entry so that overly old invitations can be eliminated.

   Consider this a mis-guided attempt at modularity
 */

#include <whoami.h>
#include "ctl.h"
#include <sys/time.h>

#ifdef	pdp11
#define MAX_ID 16000L /* << 2^15 so I don't have sign troubles */
#else
#define MAX_ID 16000 /* << 2^15 so I don't have sign troubles */
#endif	pdp11

#define NIL ( (TABLE_ENTRY *) 0)

extern int debug;

#ifndef	pdp11
struct timeval tp;
struct timezone *txp;
#endif	pdp11

typedef struct table_entry TABLE_ENTRY;

struct table_entry {
    CTL_MSG request;
    long time;
    TABLE_ENTRY *next;
    TABLE_ENTRY *last;
};

TABLE_ENTRY *table = NIL;
CTL_MSG *find_request();
CTL_MSG *find_match();
char *malloc();

    /*
     * Look in the table for an invitation that matches the current
     * request looking for an invitation
     */

CTL_MSG *find_match(request)
CTL_MSG *request;
{
    TABLE_ENTRY *ptr;
#ifdef	pdp11
    time_t time(),cur_time;

    cur_time = time( (time_t *)0 );
#else
    long cur_time;

    gettimeofday(&tp, &txp);
    cur_time = tp.tv_sec;
#endif	pdp11

    ptr = table;

    if (debug) {
	printf("Entering Look-Up with : \n");
	p_request(request);
    }

    while (ptr != NIL) {

	if ( (ptr->time - cur_time) > MAX_LIFE ) {
		/* the entry is too old */
	    if (debug) printf("Deleting expired entry : \n");
	    if (debug) p_request(&ptr->request);
	    delete(ptr);
	    ptr = ptr->next;
	    continue;
	}

	if (debug) p_request(&ptr->request);

	if ( strcmp(request->l_name, ptr->request.r_name) == 0 &&
	     strcmp(request->r_name, ptr->request.l_name) == 0 &&
	     ptr->request.type == LEAVE_INVITE ) {
	    return(&ptr->request);
	}
	
	ptr = ptr->next;
    }
    
    return((CTL_MSG *) 0);
}

    /*
     * look for an identical request, as opposed to a complimentary
     * one as find_match does 
     */

CTL_MSG *find_request(request)
CTL_MSG *request;
{
    TABLE_ENTRY *ptr;
#ifdef	pdp11
    time_t time(),cur_time;

    cur_time = time( (time_t *)0 );
#else
    long cur_time;

    gettimeofday(&tp, &txp);
    cur_time = tp.tv_sec;
#endif	pdp11

	/* See if this is a repeated message, and check for
	   out of date entries in the table while we are it.
	 */

    ptr = table;

    if (debug) {
	printf("Entering find_request with : \n");
	p_request(request);
    }

    while (ptr != NIL) {

	if ( (ptr->time - cur_time) > MAX_LIFE ) {
		/* the entry is too old */
	    if (debug) printf("Deleting expired entry : \n");
	    if (debug) p_request(&ptr->request);
	    delete(ptr);
	    ptr = ptr->next;
	    continue;
	}

	if (debug) p_request(&ptr->request);

	if ( strcmp(request->r_name, ptr->request.r_name) == 0 &&
	     strcmp(request->l_name, ptr->request.l_name) == 0 &&
	     request->type == ptr->request.type &&
	     request->pid == ptr->request.pid) {
	    
		/* update the time if we 'touch' it */
	    ptr->time = cur_time;
	    return(&ptr->request);
	}

	ptr = ptr->next;
    }

    return((CTL_MSG *) 0);
}

insert_table(request, response)
CTL_MSG *request;
CTL_RESPONSE *response;
{
    TABLE_ENTRY *ptr;
#ifdef	pdp11
    time_t time(),cur_time;
    long new_id();

    cur_time = time( (time_t *)0 );
#else
    long cur_time;

    gettimeofday(&tp, &txp);
    cur_time = tp.tv_sec;
#endif	pdp11

    response->rid_num = request->id_num = new_id();

	/* insert a new entry into the top of the list */
    
    ptr = (TABLE_ENTRY *) malloc(sizeof(TABLE_ENTRY));

    if (ptr == NIL) {
	p_error("malloc in insert_table");
    }

    ptr->time = cur_time;
    ptr->request = *request;

    ptr->next = table;
    if (ptr->next != NIL) {
	ptr->next->last = ptr;
    }
    ptr->last = NIL;
    table = ptr;
}

    /* generate a unique non-zero sequence number */

#ifdef	pdp11
long
new_id()
{
    static long cur_id = 0L;

    cur_id = (cur_id + 1L) % MAX_ID;

	/* 0 is reserved, helps to pick up bugs */

    if (cur_id == 0L) cur_id = 1L;

    return(cur_id);
}
#else
new_id()
{
    static int cur_id = 0;

    cur_id = (cur_id + 1) % MAX_ID;

	/* 0 is reserved, helps to pick up bugs */

    if (cur_id == 0) cur_id = 1;

    return(cur_id);
}
#endif	pdp11

    /* delete the invitation with id 'id_num' */

delete_invite(id_num)
#ifdef	pdp11
long id_num;
#else
int id_num;
#endif	pdp11
{
    TABLE_ENTRY *ptr;

    ptr = table;

    if (debug) printf("Entering delete_invite with %d\n", (int)id_num);

    while (ptr != NIL && ptr->request.id_num != id_num) {
	if (debug) p_request(&ptr->request);
	ptr = ptr->next;
    }
	
    if (ptr != NIL) {
	delete(ptr);
	return(SUCCESS);
    }
    
    return(NOT_HERE);
}

    /* classic delete from a double-linked list */

delete(ptr)
TABLE_ENTRY *ptr;
{
    if (debug) printf("Deleting : ");
    if (debug) p_request(&ptr->request);

    if (table == ptr) {
	table = ptr->next;
    } else if (ptr->last != NIL) {
	ptr->last->next = ptr->next;
    }

    if (ptr->next != NIL) {
	ptr->next->last = ptr->last;
    }

    free((char *) ptr);
}
