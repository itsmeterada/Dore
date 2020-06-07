/**************************************************************************
# Copyright (C) 1994 Kubota Graphics Corp.
# 
# Permission to use, copy, modify, and distribute this material for
# any purpose and without fee is hereby granted, provided that the
# above copyright notice and this permission notice appear in all
# copies, and that the name of Kubota Graphics not be used in
# advertising or publicity pertaining to this material.  Kubota
# Graphics Corporation MAKES NO REPRESENTATIONS ABOUT THE ACCURACY
# OR SUITABILITY OF THIS MATERIAL FOR ANY PURPOSE.  IT IS PROVIDED
# "AS IS", WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE AND KUBOTA GRAPHICS CORPORATION DISCLAIMS ALL WARRANTIES,
# EXPRESS OR IMPLIED.
**************************************************************************/
 
/*
 ==========================================================================
 Functions :
	DtInt drr_samplernd_install_renderer(replace_render_id)
	void drr_samplernd_render_delete()
	void drr_samplernd_render_new_class (class_name, class_id)
	void drr_samplernd_render_inq_wcstofcs_rtn(wcstofcsmat)
 ==========================================================================
*/

#include "samplernd.h"
#include "methods.h"

/* Declare pointer to renderer's private data */
drt_samplernd_render_data *dre_samplernd_render_private = DcNullPtr;

/*
 * Define the routine pointers that will be installed
 */

extern DtPtr drr_samplernd_view_data_crt();
extern void drr_samplernd_view_data_prt();
extern void drr_samplernd_view_data_del();
extern DtPtr drr_samplernd_device_data_crt();
extern void drr_samplernd_device_data_prt();
extern void drr_samplernd_device_data_del();
extern DtPtr drr_samplernd_pntlst_data_crt();
extern void drr_samplernd_pntlst_data_prt();
extern void drr_samplernd_pntlst_data_del();
extern void drr_samplernd_render_invoke_rtn();
extern void drr_samplernd_render_delete();
extern void drr_samplernd_render_new_class();
extern void drr_samplernd_render_inq_wcstofcs_rtn();

/*
===============================================================================
*/

/*
 * Renderer Installation Routine.
 * This is the routine that is going to be called at system
 * initialization time to install the new renderer into Dore.
 */

DtInt
drr_samplernd_install_renderer(replace_render_id)
     DtInt replace_render_id;
{
   DtInt i, samplernd_id;
   DtInt type;

   if (dre_samplernd_render_private != DcNullPtr) {
	DDerror (ERR_INVALID_OPERATION, "drr_samplernd_install_renderer",
			 "renderer already installed");
      return -1;
   }

   /*
    * allocate local data space for renderer
    */

   if ((dre_samplernd_render_private = (drt_samplernd_render_data *)
	DDspace_Allocate (sizeof(drt_samplernd_render_data)))
       == DcNullPtr) {
      DDerror (ERR_CANT_ALLOC_MEM_SIZE, "drr_samplernd_install_renderer",
		  "sizeof (drt_samplernd_render_data)");
      return -1;
   }
   
   /*
    * Create new Studio and Display Traversal Methods
    * Derive them from Dore standard traversal methods
    * Store method identifiers in renderer's private space
    */

   dre_samplernd_render_private->studio_traversal = 
	 DDclass_CopyMethod(DsInqMethodId("StdRenderStudio"),
			       "SamplerndStudio");

   dre_samplernd_render_private->display_traversal = 
	 DDclass_CopyMethod(DsInqMethodId("StdRenderDisplay"),
			       "SamplerndDisplay");

   /*
    * install method routines for selected classes. A class must have been
    * installed into Dore (DsInqClassId returns > -1) for the method
    * routine installation to take place.  The other methods routines
    * will be installed by the new class notification 
    * routine at the time they are instanced for the first time.
    */

   for (i=0; dre_samplernd_studio_method[i].name != 0; i++) {
      if ((type = DsInqClassId(dre_samplernd_studio_method[i].name)) != -1) {
	 DDclass_SetMethod(type, dre_samplernd_render_private->studio_traversal, 
			    dre_samplernd_studio_method[i].routine);
      }
   }
   
   /*
    * Similarly, install the method routines for the display
    * traversal method, and inform Dore of the new renderer.
    */

   for (i=0; dre_samplernd_display_method[i].name != 0; i++) {
      if ((type = DsInqClassId(dre_samplernd_display_method[i].name)) != -1) {
	 DDclass_SetMethod(type,
                dre_samplernd_render_private->display_traversal, 
		dre_samplernd_display_method[i].routine);
      }
   }

   /*
    * Associate data with the device and view classes.
    * First check if classes have been initialized.
    */

   if ((DsInqClassId("DoView") == -1) || (DsInqClassId("DoDevice") == -1)) {
	DDerror (ERR_INVALID_OPERATION, "drr_samplernd_install_renderer",
			 "View or Device class not installed.");
      return -1;
   }

  /*
   * Store additional data indices in renderer's private
   * space for easier access
   */

   dre_samplernd_render_private->view_index = 
		DDclass_AddObjectData(DsInqClassId("DoView"),
				 drr_samplernd_view_data_crt,
				 drr_samplernd_view_data_prt,
				 drr_samplernd_view_data_del);

   dre_samplernd_render_private->device_index =
	 DDclass_AddObjectData(DsInqClassId("DoDevice"),
				 drr_samplernd_device_data_crt,
				 drr_samplernd_device_data_prt,
				 drr_samplernd_device_data_del);

  /*
   * Inform Dore' of the new renderer
   */


   samplernd_id =  DDrender_AddRenderer(replace_render_id, "Sample Renderer", 
			       drr_samplernd_render_invoke_rtn,
			       drr_samplernd_render_delete,
			       drr_samplernd_render_new_class,
			       drr_samplernd_render_inq_wcstofcs_rtn);

   /* return id (rendering style) associated with sample renderer */
   return samplernd_id;
}

/*
===============================================================================
*/

/*
 * Renderer delete routine
 */

void
drr_samplernd_render_delete()
{
   /*
    * Free all space allocated by the renderer for its private use
    */

   if (dre_samplernd_render_private != DcNullPtr) {
      DDspace_Deallocate (dre_samplernd_render_private);
   }

   dre_samplernd_render_private = DcNullPtr;
}

/*
===============================================================================
*/

/*
 * New class notification routine.
 * This routine will be called when a new class is to be installed.
 */

void
drr_samplernd_render_new_class (class_name, class_id)
     DtPtr class_name;
     DtInt class_id;
{
	DtInt i;

   /*
    * install method routine for the new class
    */

   for (i=0; dre_samplernd_studio_method[i].name != 0; i++) {
      if (strcmp(dre_samplernd_studio_method[i].name, class_name) == 0) {
	 DDclass_SetMethod(class_id,
			    dre_samplernd_render_private->studio_traversal, 
			    dre_samplernd_studio_method[i].routine);
	break;
      }
   }
   
   for (i=0; dre_samplernd_display_method[i].name != 0; i++) {
      if (strcmp(dre_samplernd_display_method[i].name, class_name) == 0) {
	 DDclass_SetMethod(class_id,
			    dre_samplernd_render_private->display_traversal, 
			    dre_samplernd_display_method[i].routine);
	break;
      }
   }

   /*
    * associate additional data with point list objects
   */

   if (strcmp(class_name, "DoPointList") == 0) {
      dre_samplernd_render_private->pntlst_index =
   				DDclass_AddObjectData(class_id,
                         drr_samplernd_pntlst_data_crt,
                         drr_samplernd_pntlst_data_prt,
                         drr_samplernd_pntlst_data_del);
   }

}

/*
===============================================================================
*/

/*
 * Return the world-to-frustum coordinate matrix
 */

void
drr_samplernd_render_inq_wcstofcs_rtn(wcstofcsmat)
DtMatrix4x4 wcstofcsmat;
	{
	drr_samplernd_matrix_load(wcstofcsmat,
			dre_samplernd_render_private->viwdat->wcstofcsmat);	
      }

/*
===============================================================================
*/

