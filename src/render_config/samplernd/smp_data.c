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
	DtPtr drr_samplernd_view_data_crt(object)
	void drr_samplernd_view_data_prt(object, data)
	void drr_samplernd_view_data_del(object, data)
	DtPtr drr_samplernd_device_data_crt(object)
	void drr_samplernd_device_data_prt(object, data
	void drr_samplernd_device_data_del(object, data
	DtPtr drr_samplernd_pntlst_data_crt(object)
	void drr_samplernd_pntlst_data_prt(object, altobj)
	void drr_samplernd_pntlst_data_del(object, altobj)
 ==========================================================================
*/

#include "stdio.h"
#include "samplernd.h"
#include "dore/dore_develop/private/pntlst.h"

typedef struct object dot_object;

#define RADII_PNTLST_ALT_OBJ    0.5

/*
 * Additional data routines for view
 */

DtPtr
drr_samplernd_view_data_crt(object)
     DtObject object;
{
   drt_samplernd_view_data *data;

   if ((data = (drt_samplernd_view_data *)DDspace_Allocate
		(sizeof(drt_samplernd_view_data))) == DcNullPtr) {
      DDerror (ERR_CANT_ALLOC_MEM_SIZE,
		  "drr_samplernd_view_data_crt",
		  "sizeof (drt_samplernd_view_data)");
      return DcNullPtr;
   }

   /* Initially make room for 5 lights in the light list */

   data->max_lights = 5;
   data->light_cnt = 0;

   if ((data->lights = (drt_samplernd_light_data *)
	 DDspace_Allocate((data->max_lights)*sizeof(drt_samplernd_light_data)))
         == DcNullPtr) {
      DDerror (ERR_CANT_ALLOC_MEM_SIZE,
		  "drr_samplernd_view_data_crt",
		  "sizeof (drt_samplernd_light_data)");
      return DcNullPtr;
   }

   printf("samplernd: created additional data for view object.\n");
   return (DtPtr)data;
}

/*
===============================================================================
*/

void
drr_samplernd_view_data_prt(object, data)
     DtObject object;
     drt_samplernd_view_data *data;
{
   DtInt i;

   DDprint("samplernd: printing additional data for view:");
   DDprint_IndentUp();

   sprintf(DDprint_GetLine(), "Camera type is %d", data->camera_type);
   DDprint(DDprint_GetLine());

   DDprint("Projection matrix:");
   drr_samplernd_matrix_indent_print(data->projmat, stdout);

   DDprint("Camera-to-world matrix:");
   drr_samplernd_matrix_indent_print(data->ccstowcsmat,stdout);

   DDprint("World-to-camera matrix:");
   drr_samplernd_matrix_indent_print(data->wcstoccsmat,stdout);

   DDprint("World-to-frustum matrix:");
   drr_samplernd_matrix_indent_print(data->wcstofcsmat,stdout);

   sprintf(DDprint_GetLine(), "Max_lights is %d", data->max_lights);
   DDprint(DDprint_GetLine());

   sprintf(DDprint_GetLine(), "Light count is %d", data->light_cnt);
   DDprint(DDprint_GetLine());

   DDprint("Light list is:");
   DDprint_IndentUp();
   for (i=0; i<data->light_cnt; i++) {
	sprintf(DDprint_GetLine(), "Light number %d:", i);
        DDprint(DDprint_GetLine());

        DDprint_IndentUp();
   	sprintf(DDprint_GetLine(),"Light color: (%g %g %g)", 
                data->lights[i].color[0], data->lights[i].color[1],
                data->lights[i].color[2]);
        DDprint(DDprint_GetLine());

        if (data->lights[i].type == DcLightPoint) {
   	   sprintf(DDprint_GetLine(), "Light position: (%g %g %g)",
                  data->lights[i].pos[0], data->lights[i].pos[1],
                  data->lights[i].pos[2]);
           DDprint(DDprint_GetLine());
        }

        if (data->lights[i].type == DcLightInfinite) {
   	   sprintf(DDprint_GetLine(), "Light direction: (%g %g %g)",
                  data->lights[i].dir[0], data->lights[i].dir[1],
                  data->lights[i].dir[2]);
           DDprint(DDprint_GetLine());
   	}
        DDprint_IndentDown();
   }
   DDprint_IndentDown();
   sprintf(DDprint_GetLine(), "Clear flag: %d", data->clear_flag);
   DDprint(DDprint_GetLine());

   sprintf(DDprint_GetLine(), "Background color model: %d",
           data->background_color_model); 
   DDprint(DDprint_GetLine());

   sprintf(DDprint_GetLine(),"Background_color: (%g %g %g)",
           data->background_color[0], data->background_color[1],
           data->background_color[2]);
   DDprint(DDprint_GetLine());

   sprintf(DDprint_GetLine(),"Background shade Index: %d",
           data->background_shade_index);
   DDprint(DDprint_GetLine());

   DDprint_IndentDown();
   DDprint("end of additional data for view");
}

/*
===============================================================================
*/

void
drr_samplernd_view_data_del(object, data)
     DtObject object;
     drt_samplernd_view_data *data;
{
   if (data->lights != (drt_samplernd_light_data *)0)
   	DDspace_Deallocate(data->lights);

   if (data != (drt_samplernd_view_data *)0)
        DDspace_Deallocate(data);

   printf("samplernd: deallocating space for additional data of view object.\n");
}

/*
===============================================================================
*/

/*
 * Additional data routines for device
 */

DtPtr
drr_samplernd_device_data_crt(object)
     DtObject object;
{
   drt_samplernd_device_data *data;

   if ((data = (drt_samplernd_device_data *)DDspace_Allocate
   	(sizeof(drt_samplernd_device_data))) == (drt_samplernd_device_data *)
   	DcNullPtr) {
      DDerror (ERR_CANT_ALLOC_MEM_SIZE,
		  "drr_samplernd_device_data_crt",
		  "sizeof (drt_samplernd_device_data)");
      return DcNullPtr;
   }

   printf("samplernd: created additional data for device object.\n");

   /* return pointer to additional data */
   return (DtPtr)data;
}

/*
===============================================================================
*/

void
drr_samplernd_device_data_prt(object, data)
     DtObject object;
     drt_samplernd_device_data *data;
{
   DDprint("samplernd: printing additional data for device:");
   DDprint_IndentUp();

   sprintf(DDprint_GetLine(), "Extent is: (%g %g) to (%g %g)",
           data->extent[0], data->extent[1],
   	   data->extent[2], data->extent[3]);
   DDprint(DDprint_GetLine());

   sprintf(DDprint_GetLine(), "Scale factor is: (%g %g %g)",
           data->scale[0], data->scale[1], data->scale[2]);
   DDprint(DDprint_GetLine());

   sprintf(DDprint_GetLine(), "Translate factor is: (%g %g %g)",
           data->trans[0], data->trans[1], data->trans[2]);
   DDprint(DDprint_GetLine());

   sprintf(DDprint_GetLine(), "Shade mode is: %d", data->shade_mode);
   DDprint(DDprint_GetLine());

   sprintf(DDprint_GetLine(), "Visual type is : %d", data->visual_type);
   DDprint(DDprint_GetLine());

   DDprint_IndentDown();
   DDprint("end of additional data for device");
}

/*
===============================================================================
*/

void
drr_samplernd_device_data_del(object, data)
     DtObject object;
     drt_samplernd_device_data *data;
{
   printf("samplernd: deallocating additional data space for device.\n");
   DDspace_Deallocate(data);
}

/*
===============================================================================
*/
     
/*
 * Additional data routines for point list primitive
 */

DtPtr
drr_samplernd_pntlst_data_crt(object)
     DtObjectStructure  *object;
{

   struct pntlst *pntlst;
   DtObject altobj; 
   DtReal *radii;
   DtInt i, index;

   /* Get pointer to data private to point list object.
    * The structure of the data is defined in
    * "dore_develop/private/pntlst.h",
    *  which is included above
    */

   pntlst = (struct pntlst *) object->data;

   /*
    * Fill radii array with as many values as there are
    * points in the list
    */

   radii = (DtReal *) DDspace_Allocate(pntlst->point_count*sizeof(DtReal));
   for (i=0; i<pntlst->point_count; i++)
       radii[i] = RADII_PNTLST_ALT_OBJ;

    /*
    * Create a sphere list as an alternate representation
    * for the sphere.
    * For efficiency, the sphere list could alternatively be
    * created only when it is needed in the display method
    * routine for point list.
    */

   altobj = DoSphereList(DcRGB, pntlst->point_count,
   		pntlst->vertex_location, radii, pntlst->vertex_color);

   DDspace_Deallocate(radii);

   printf("samplernd: created additional data for point list object.\n");

   /* return pointer to additional data */
   return ((DtPtr)altobj);
}

/*
===============================================================================
*/

void
drr_samplernd_pntlst_data_prt(object, altobj)
     DtObject object;
     DtObject altobj;
{
   DDprint("samplernd: printing additional data for point list = sphere list:");

   DDprint_IndentUp();
   DsPrintObj(altobj);
   DDprint_IndentDown();

   DDprint("samplernd: end of additional data for point list = sphere list");
}

/*
===============================================================================
*/

void
drr_samplernd_pntlst_data_del(object, altobj)
     DtObject object;
     DtObject altobj;
{
   printf("samplernd: deallocating additional data space ");
   printf("for point list object.\n");
   DeDeleteObject(altobj);
}


/*
===============================================================================
*/
