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
	void drr_samplernd_light_render(object)
 ==========================================================================
*/

#include "../samplernd.h"


/*
 * Method routine for light object
 */

void
drr_samplernd_light_render(object)
DtObjectStructure *object;
{
   DtPFI routine;
   DtObject lighttype;
   DtColorModel colormodel;
   DtColorRGB lightcolor;
   DtReal lightintens, w;
   DtMatrix4x4 matrix;
   DtPoint3 pos, aim_at;
   DtInt index;
   drt_samplernd_view_data_ptr viwdat;


   /*
    * Binds type, color, intensity and local-to-world  
    * transformation matrix with light source
    */

   routine = DDclass_InqMethod 
			(DsInqClassId("AttLightType"),
			DsInqMethodId("InqGlobalValue"));
   (*routine)(&lighttype);

   routine = DDclass_InqMethod
			(DsInqClassId("AttLightColor"),
			DsInqMethodId("InqGlobalValue"));
   (*routine)(&colormodel, lightcolor);

   routine = DDclass_InqMethod
			(DsInqClassId("AttLightIntens"),
			DsInqMethodId("InqGlobalValue"));
   (*routine)(&lightintens);

   routine = DDclass_InqMethod 
			(DsInqClassId("AttLcstowcsmat"),
			DsInqMethodId("InqGlobalValue"));
   (*routine)(matrix);

   /*
    * Maintain a light list and store it with the view.
    */

   viwdat = dre_samplernd_render_private->viwdat;

   if (lighttype == DcLightAmbient)  {
      /* Add ambient lights together */

      viwdat->lights[0].color[0] += 
			lightcolor[0] * lightintens;
      viwdat->lights[0].color[1] += 
			lightcolor[1] * lightintens;
      viwdat->lights[0].color[2] += 
			lightcolor[2] * lightintens;

   } else {

   /*
    * A non-ambient light is at (0,0,0) in local
    * coordinates with the light direction along the -Z axis
    * (if applicable). The position of the light in world
    * coordinates is obtained by transforming (0,0,0) by the
    * current transf matrix. To get the light direction,
    * (0,0,-1) is transformed.
    */
      
      if (viwdat->light_cnt >= viwdat->max_lights){

     /* Allocate space for 5 more light sources at a time */

	  viwdat->max_lights += 5;
	  viwdat->lights = (drt_samplernd_light_data_ptr) 
	      DDspace_Reallocate(viwdat->lights,
	         viwdat->max_lights*
	         sizeof(drt_samplernd_light_data));
      }

      index = viwdat->light_cnt;
      viwdat->light_cnt++;

      viwdat->lights[index].color[0] =
				lightcolor[0] * lightintens;
      viwdat->lights[index].color[1] =
				lightcolor[1] * lightintens;
      viwdat->lights[index].color[2] =
				lightcolor[2] * lightintens;

      drr_samplernd_math_homo_trns(0., 0., 0.,1.,matrix,
                   &pos[0], &pos[1], &pos[2], &w);
      if (w != 0.0)
            pos[0] /= w; pos[1] /= w; pos[2] /= w;

      if (lighttype == DcLightInfinite) {
         viwdat->lights[index].type = DcLightInfinite;

    	 drr_samplernd_math_homo_trns(0., 0., -1.,1.,matrix,
                   &aim_at[0], &aim_at[1], &aim_at[2], &w);

    	 if (w != 0.0)
             aim_at[0] /= w; aim_at[1] /= w; aim_at[2] /= w;

         viwdat->lights[index].dir[0] = aim_at[0] - pos[0];
         viwdat->lights[index].dir[1] = aim_at[1] - pos[1];
         viwdat->lights[index].dir[2] = aim_at[2] - pos[2];

         drr_samplernd_vector_normalize
                              (viwdat->lights[index].dir);

      } else if (lighttype ==  DcLightPoint) {

         viwdat->lights[index].type = DcLightPoint;

         viwdat->lights[index].pos[0] = pos[0];
         viwdat->lights[index].pos[1] = pos[1];
         viwdat->lights[index].pos[2] = pos[2];

       } else {
         printf("samplernd: Light type not supported.\n");
         return;
         }
   }

   printf("samplernd: Light source being added.\n");
   
   /*
    * Reminder: all ambient lights are stored in viwdat->lights[0]
    * therefore index must be set to 0 for the print statement.
    * This bug was fixed AFTER the Developer's Guide was printed.
    * (With the code shown in the book, we'd get a core dump when an
    * ambient light is used).
    */

   if (lighttype == DcLightAmbient)
      index = 0;

   printf ("\t Light color (%g,%g,%g)\n", 
		 viwdat->lights[index].color[0],
		 viwdat->lights[index].color[1],
		 viwdat->lights[index].color[2]);

   if (lighttype == DcLightInfinite) {
   printf ("\t Light direction (%g,%g,%g)\n",
		 viwdat->lights[index].dir[0],
		 viwdat->lights[index].dir[1],
		 viwdat->lights[index].dir[2]);

   } else if (lighttype == DcLightPoint) {
      printf ("\t Light position (%g,%g,%g)\n",
		 viwdat->lights[index].pos[0],
		 viwdat->lights[index].pos[1],
		 viwdat->lights[index].pos[2]);
   }
}
