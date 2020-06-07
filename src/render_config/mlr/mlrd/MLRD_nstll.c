/* vi:set ts=8 sw=4 ai:
 *[
 *$     MLRD_main.c
 *
 *      Steve Gray - Fri Jul  7 15:47:42 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Contents:	install Media Logic Render in Dore
 *
 *      Usage:
 *		MLRD_install_direct()	install the direct renderer
 *		MLRD_install_file()	install the file renderer
 *		MLRD_set_file()		set the file name prefix
 *		MLRD_auto_render()	enable/disable automatic file render
 *
 *      Notes:
 *		the "direct" renderer runs the renderer as part of the
 *		same program and output goes directly to the Dore window
 *
 *		the "file" renderer writes the render control files
 *		needed to render the image to the files specified by
 *		the user
 *
 *
 *?
 *!     Changes History
 *
 *]
 */


/* #define	DEBUG */


/* system includes */
#include <stdio.h>

/* dore includes */
#include "dore.h"

/* Render includes */
#include "primitives.h"

/* MLRD Dore interface includes */
#include "global.h"
#include "method.h"


/*[
 *$     MLRD_install_direct()
 *
 *      Steve Gray - Fri Jul  7 15:50:57 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    install the direct Media Logic renderer
 *
 *      Usage:
 *		MLRD_install_direct()
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_install_direct()
{
Int	MLRD_render_direct();
Int	MLRD_render_delete();
Int	MLRD_render_new_class();
Int	CAM_get_matrix();

/* local vars */
DtInt	id, i, type;


#ifdef	DEBUG
printf( "MLRD_install\n");
#endif

    mlr_control.device = 0;
    mlr_control.view = 0;
    mlr_control.PROM_fcns = 0;

/* create the method functions */
    mlr_control.studio_traversal = DDclass_CopyMethod(
			DsInqMethodId( "StdRenderStudio"),
			"MediaLogicStudioDirect");
    
    mlr_control.display_traversal = DDclass_CopyMethod(
			DsInqMethodId( "StdRenderDisplay"),
			"MediaLogicDisplayDirect");

/* install studio methods */
    for (i=0; studio_method[i].name != 0; i++) 
    {
       if ((type = DsInqClassId(studio_method[i].name)) != -1) 
       {
#ifdef	DEBUG
printf( "install studio method %s (type = %d)\n", studio_method[i].name, type);
#endif

	   DDclass_SetMethod( type,
			       mlr_control.studio_traversal, 
			       studio_method[i].routine);
       }
    }
   
/* install display methods */
    for (i=0; display_method[i].name != 0; i++) 
    {
        if ((type = DsInqClassId(display_method[i].name)) != -1) 
        {
#ifdef	DEBUG
printf( "install display method %s (type = %d)\n",display_method[i].name, type);
#endif

	   DDclass_SetMethod( type,
			       mlr_control.display_traversal, 
			       display_method[i].routine);
        }
    }

/* install the renderer into Dore */
    mlr_control.render_id = DDrender_AddRenderer( -1,
				      "Media Logic Direct Renderer",
				      MLRD_render_direct,
				      MLRD_render_delete,
				      MLRD_render_new_class,
				      CAM_get_matrix);

#ifdef	DEBUG
printf( "render id %d\n", mlr_control.render_id);
#endif
    
    return( mlr_control.render_id);
}

/*[
 *$     MLRD_install_file()
 *
 *      Steve Gray - Fri Jul  7 15:50:57 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    install the Media Logic file renderer
 *
 *      Usage:
 *		MLRD_install_file( file_prefix)
 *		char	*file_prefix	(in)  render control file prefix
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_install_file( file_prefix)
char	*file_prefix;
{
Int	MLRD_render_file();
Int	MLRD_render_delete();
Int	MLRD_render_new_class();
Int	CAM_get_matrix();

/* local vars */
DtInt	id, i, type;


#ifdef	DEBUG
printf( "MLRD_install\n");
#endif

    mlr_control.device = 0;
    mlr_control.view = 0;
    mlr_control.PROM_fcns = 0;

/* save the file prefix */
    strcpy( mlr_files.file_prefix, file_prefix);

/* create the method functions */
    mlr_control.studio_traversal = DDclass_CopyMethod(
			DsInqMethodId( "StdRenderStudio"),
			"MediaLogicStudioFile");
    
    mlr_control.display_traversal = DDclass_CopyMethod(
			DsInqMethodId( "StdRenderDisplay"),
			"MediaLogicDisplayFile");

/* install studio methods */
    for (i=0; studio_method[i].name != 0; i++) 
    {
       if ((type = DsInqClassId(studio_method[i].name)) != -1) 
       {
#ifdef	DEBUG
printf( "install studio method %s (type = %d)\n", studio_method[i].name, type);
#endif

	   DDclass_SetMethod( type,
			       mlr_control.studio_traversal, 
			       studio_method[i].routine);
       }
    }
   
/* install display methods */
    for (i=0; display_method[i].name != 0; i++) 
    {
        if ((type = DsInqClassId(display_method[i].name)) != -1) 
        {
#ifdef	DEBUG
printf( "install display method %s (type = %d)\n",display_method[i].name, type);
#endif

	   DDclass_SetMethod( type,
			       mlr_control.display_traversal, 
			       display_method[i].routine);
        }
    }

/* install the renderer into Dore */
    mlr_control.render_id = DDrender_AddRenderer( -1,
				      "Media Logic File Renderer",
				      MLRD_render_file,
				      MLRD_render_delete,
				      MLRD_render_new_class,
				      CAM_get_matrix);

#ifdef	DEBUG
printf( "render id %d\n", mlr_control.render_id);
#endif

/* set for auto rendering */
    mlr_auto_render = TRUE;
    
/* return the id */
    return( mlr_control.render_id);
}

/*[
 *$     MLRD_set_file
 *
 *      Steve Gray - Fri Oct 13 11:57:51 PDT 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    set the file prefix for file rendering
 *
 *      Usage:
 *		MLRD_set_file( file_prefix)
 *		char	*file_prefix
 *
 *      Notes:  
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_set_file( file_prefix)
char	*file_prefix;
{
    strcpy( mlr_files.file_prefix, file_prefix);
}

/*[
 *$     MLRD_auto_render()
 *
 *      Steve Gray - Tue Dec  5 12:28:30 PST 1989
 *      Media Logic, Inc., Santa Monica, Ca.
 *?
 *      Description:    enable/dissable automatic rendering of file renders
 *
 *      Usage:
 *		MLRD_auto_render( flag)
 *		Int	flag	(in)  status, TRUE => auto render
 *					      FALSE => no auto render
 *
 *      Notes:  
 *		auto rendering is enabled by default
 *
 *?
 *!     Changes History
 *
 *
 *]
 */

MLRD_auto_render( flag)
Int	flag;
{
    mlr_auto_render = flag;
}



