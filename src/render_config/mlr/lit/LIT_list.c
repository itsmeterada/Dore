/* vi:set ts=8 sw=4 ai:
 *[
 *$     LIT_list.c
 *
 *      Steve Gray - Mon Nov 13 15:50:48 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	light list management
 *
 *      Usage:
 *		LIT_list_init()		initialize light list
 *		LIT_list_add()		add light to list
 *
 *		LIT_list_reset()	reset list to all on
 *		LIT_list_process()	process Dore light list
 *		LIT_list_lto()		execute MLR "lto" command
 *
 *		lit_list_find()		find a light in the list
 *
 *      Notes:
 *
 *
 *?
 *!     Changes History
 *
 *]
 */


/* system includes */
#include <stdio.h>

/* dore includes */
#include "dore.h"

/* Render includes */
#include "primitives.h"

/* MLR Dore interface includes */
#include "global.h"
#include "lit.h"



/*[
 *$     LIT_list_init
 *
 *      Steve Gray - Mon Nov 13 15:52:49 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    initialize light list
 *
 *      Usage:
 *		LIT_list_init()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

LIT_list_init()
{
    lit_count = 0;
}



/*[
 *$     LIT_list_add
 *
 *      Steve Gray - Mon Nov 13 15:53:49 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    add light to list
 *
 *      Usage:
 *		LIT_list_add( dore_object)
 *		DtPtr	dore_object	(in)  DORE light object
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

LIT_list_add( dore_object)
DtObject	dore_object;
{
/* check if there are too many lights */
    if ( (lit_count + 1) == LIT_MAX )
    {
	DDerror( ERR_VALUE_OUT_OF_RANGE, "LIT_list_add", "too many lights");
    }

/* add the light to the list */
    lit_info[lit_count].dore_object = dore_object;
    lit_info[lit_count].on = TRUE;

    lit_count++;
}



/*[
 *$     LIT_list_reset()
 *
 *      Steve Gray - Mon Nov 13 16:03:56 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    reset lights to all on
 *
 *      Usage:
 *		LIT_list_reset()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

LIT_list_reset()
{
Int	i;


    for ( i = 0; i < lit_count; i++ )
    {
	lit_info[i].on = TRUE;
    }
}



/*[
 *$     LIT_list_process
 *
 *      Steve Gray - Mon Nov 13 16:04:50 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    process Dore light list
 *
 *      Usage:
 *		LIT_list_process( count, list)
 *		DtInt	count		(in)  number of entries
 *		DtPtr	list		(in)  list of Dore lights
 *
 *      Notes:  
 *		turn off all the lists in the list
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

LIT_list_process( count, list)
DtInt		count;
DtObject	*list;
{
Int	i;
Int	light;


/* since multiple instances of a given light may be in the list, compare each
 * light in the list with all the entries in the off list, if it appears in
 * the off list, turn it off
 */
    for ( light = 0; light < lit_count; light++ )
    {
	for ( i = 0; i < count; i++ )
	{
	    if ( lit_info[light].dore_object == list[i] )
	    {
	        lit_info[light].on = FALSE;
	    }
	}
    }
}



/*[
 *$     LIT_list_lto
 *
 *      Steve Gray - Mon Nov 13 16:06:54 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    execute the LTO command for the current list
 *
 *      Usage:
 *		LIT_list_lto( object)
 *		Int	object		(in)  current object number
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

LIT_list_lto( object)
Int	object;
{
Short	count;
Short	lit_list[LIT_MAX];
Int	i;


    count = 0;
    for ( i = 0; i < lit_count; i++ )
    {
	if ( lit_info[i].on )
	{
	    lit_list[count++] = i;
	}
    }

    if ( count > 0 )
    {
        if ( mlr_mode == MLR_MODE_DIRECT )
        {
#ifdef	RENDER_DIRECT
	    MLR_lto( object, count, lit_list);
#endif
	}
        else if ( mlr_mode == MLR_MODE_FILE )
        {
	    fprintf( mlr_files.light_fp, "lto %d", object);

	    for ( i = 0; i < count; i++ )
	    {
		fprintf( mlr_files.light_fp, " %d", lit_list[i]);
	    }

	    fprintf( mlr_files.light_fp, "\n");
	}
    }
}


/*[
 *$     lit_list_find
 *
 *      Steve Gray - Mon Nov 13 16:16:54 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    find a Dore light in the list
 *
 *      Usage:
 *		lit_list_find( object)
 *		DtPtr	object		(in) Dore light object
 *
 *      Notes:  
 *		returns -1	if NOT found
 *			index	if found
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

lit_list_find( object)
DtObject	object;
{
Int	i;


    for ( i = 0; i < lit_count; i++ )
    {
	if ( lit_info[i].dore_object == object )
	{
	    return( i);
	}
    }

    return( -1);
}



