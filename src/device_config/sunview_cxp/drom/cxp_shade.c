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
  ======================================================================
  Functions:
	ddr_sunview_cxp_drom_shade_elements ddr_sunview_cxp_drom_shade_elements (elementcount, elenrms, vertexcount, vtxlocs, vtxnrms, vtxclrs)
	ddr_sunview_cxp_drom_shade_connected_line_list ddr_sunview_cxp_drom_shade_connected_line_list (vertexcount, elenrms, vtxlocs, vtxnrms, vtxclrs, shades_per_element)
	ddr_sunview_cxp_drom_shade_triangle_list ddr_sunview_cxp_drom_shade_triangle_list (elementcount, elenrms, vertexcount, vtxlocs, vtxnrms, vtxclrs, backfacing)
	ddr_sunview_cxp_drom_shade_triangle_mesh ddr_sunview_cxp_drom_shade_triangle_mesh (trianglecount, elenrms, vertexcount, vtxlocs, vtxnrms, vtxclrs, vertexlist, shades_per_element, backfacing)
	ddr_sunview_cxp_drom_check_shade_buffers ddr_sunview_cxp_drom_check_shade_buffers (backfacing, shading, pseudo)
	ddr_sunview_cxp_drom_get_normal_type ddr_sunview_cxp_drom_get_normal_type (elenrms, vtxnrms, normal_type, normals)
	ddr_sunview_cxp_drom_shade_clip_color ddr_sunview_cxp_drom_shade_clip_color (count, culling, shades_per_element)
	ddr_sunview_cxp_drom_shade_to_pseudo ddr_sunview_cxp_drom_shade_to_pseudo (count, culling, shade_mode, shades_per_element)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_shade_elements (elementcount, elenrms, vertexcount, vtxlocs,
			    vtxnrms, vtxclrs)
     DtInt elementcount;
     DtRealTriple *elenrms;
     DtInt vertexcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
   DtInt normal_type;
   DtInt shade_type;
   DtRealTriple *shade;
   DtInt *color_indices;
   DtReal ambient_intensity;
   DtReal diffuse_intensity;
   DtReal specular_intensity;
   DtRealTriple ambient_reflectance;
   DtRealTriple diffuse_reflectance;
   DtRealTriple specular_reflectance;
   DtInt diffuse_light_switch;
   DtInt specular_light_switch;
   DtRealTriple *normals;
   DtInt elementno;
   DtInt vtxno;
   DtInt color;
   DtRealTriple TLC;
   DtReal light_normal[3];
   DtReal light_color[3];
   DtReal dot_prod;
   DtInt vtx_per_element;
   DtInt i, j;
   DtInt idx;
   DtInt light;

   vtx_per_element = vertexcount / elementcount;

   ddr_sunview_cxp_drom_check_shade_buffers (0, vertexcount, vertexcount);

   shade = dde_sunview_cxp_drom_data.colored_elements;
   color_indices = dde_sunview_cxp_drom_data.color_indices;

   /*
    * scale the intensities and reflectances to the range of 0 - 255.
    * This is less computationally expensive to do the scaling now that
    * at the end when all the shades would have to be scaled.
    */

   ambient_intensity = dde_sunview_cxp_drom_data.object_ambient_intensity * 255;
   diffuse_intensity = dde_sunview_cxp_drom_data.object_diffuse_intensity * 255;
   specular_intensity = dde_sunview_cxp_drom_data.object_specular_intensity * 255;

   if (vtxclrs == NULL) {
      ambient_reflectance[0] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[0]*255;
      ambient_reflectance[1] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[1]*255;
      ambient_reflectance[2] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[2]*255;
   } else {
      ambient_reflectance[0] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] 
	    * ambient_intensity;
      ambient_reflectance[1] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] 
	    * ambient_intensity;
      ambient_reflectance[2] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] 
	    * ambient_intensity;
   }      

   diffuse_reflectance[0] = dde_sunview_cxp_drom_data.diffuse_reflectance[0] * 255;
   diffuse_reflectance[1] = dde_sunview_cxp_drom_data.diffuse_reflectance[1] * 255;
   diffuse_reflectance[2] = dde_sunview_cxp_drom_data.diffuse_reflectance[2] * 255;

   specular_reflectance[0] = dde_sunview_cxp_drom_data.specular_reflectance[0] * 255;
   specular_reflectance[1] = dde_sunview_cxp_drom_data.specular_reflectance[1] * 255;
   specular_reflectance[2] = dde_sunview_cxp_drom_data.specular_reflectance[2] * 255;

   /*
    * determine what type of shading will be done.
    * light source shading requires at least a normal per element and
    * of course light sources.
    */

   diffuse_light_switch = (dde_sunview_cxp_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_sunview_cxp_drom_data.object_diffuse_switch;

   specular_light_switch = (dde_sunview_cxp_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_sunview_cxp_drom_data.object_specular_switch;

   ddr_sunview_cxp_drom_get_normal_type (elenrms, vtxnrms, &normal_type, &normals);

   if (normal_type == ddd_sunview_cxp_drom_no_normals || 
       (!diffuse_light_switch && !specular_light_switch)) {
      /*
       * No light source shading can be done or was not requested.  
       * Use the diffuse color or diffuse intensity and the vertex colors
       */

      if (vtxclrs == NULL) {
	 shade_type = ddd_sunview_cxp_drom_element_shading;
	 for (elementno=0; elementno<elementcount; elementno++) {
	    shade[elementno][0] = diffuse_reflectance[0];
	    shade[elementno][1] = diffuse_reflectance[1];
	    shade[elementno][2] = diffuse_reflectance[2];
	 }
      } else {
	 shade_type = ddd_sunview_cxp_drom_vertex_shading;
	 for (vtxno=0; vtxno<vertexcount; vtxno++) {
	    shade[vtxno][0] = diffuse_intensity*vtxclrs[vtxno][0];
	    shade[vtxno][1] = diffuse_intensity*vtxclrs[vtxno][1];
	    shade[vtxno][2] = diffuse_intensity*vtxclrs[vtxno][2];
	 }
      }
   } else {
      if (dde_sunview_cxp_drom_data.object_ambient_switch) {
	 if (vtxclrs == NULL) {
	    for (i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0];
	       shade[i][1] = ambient_reflectance[1];
	       shade[i][2] = ambient_reflectance[2];
	    }
	 } else {
	    for (i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0]*vtxclrs[i][0];
	       shade[i][1] = ambient_reflectance[1]*vtxclrs[i][1];
	       shade[i][2] = ambient_reflectance[2]*vtxclrs[i][2];
	    }
	 }
      } else {
	 for (i=0; i<vertexcount; i++) {
	    shade[i][0] = 0.0;
	    shade[i][1] = 0.0;
	    shade[i][2] = 0.0;
	 }
      }

      if (diffuse_light_switch) {
	 /*
	  * diffuse light shading
	  */

	 if (normal_type == ddd_sunview_cxp_drom_element_normals) {
	    if (vtxclrs != NULL) {
	       shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    } else {
	       shade_type = ddd_sunview_cxp_drom_element_shading;
	    }
	    for (elementno=0; elementno<elementcount; elementno++) {
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
		  
		  dot_prod = light_normal[0]*normals[elementno][0] +
			light_normal[1]*normals[elementno][1] +
			      light_normal[2]*normals[elementno][2];
		  if (dot_prod < 0.0) dot_prod = 0.0;

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }
	       if (vtxclrs == NULL) {
		  shade[elementno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[elementno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[elementno][2] += diffuse_reflectance[2] * TLC[2];
	       } else {
		  for (i=0; i<vtx_per_element; i++) {
		     vtxno = elementno*vtx_per_element + i;
		     shade[vtxno][0] += diffuse_intensity * TLC[0] *
			   vtxclrs[vtxno][0];
		     shade[vtxno][1] += diffuse_intensity * TLC[1] *
			   vtxclrs[vtxno][1];
		     shade[vtxno][2] += diffuse_intensity * TLC[2] *
			   vtxclrs[vtxno][2];
		  }
	       }
	    }
	 } else { /* VERTEX NORMALS */
	    shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    for (vtxno=0; vtxno<vertexcount; vtxno++) {
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[vtxno][0] +
			light_normal[1]*normals[vtxno][1] +
			      light_normal[2]*normals[vtxno][2];
		  if (dot_prod < 0.0) dot_prod = 0.0;
		  
		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       if (vtxclrs == NULL) {
		  shade[vtxno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[vtxno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[vtxno][2] += diffuse_reflectance[2] * TLC[2];
	       } else {
		  shade[vtxno][0] += diffuse_intensity * TLC[0] *
			vtxclrs[vtxno][0];
		  shade[vtxno][1] += diffuse_intensity * TLC[1] *
			vtxclrs[vtxno][1];
		  shade[vtxno][2] += diffuse_intensity * TLC[2] *
			vtxclrs[vtxno][2];
	       }
	    }
	 }
      }
      if (specular_light_switch) {
	 /*
	  * specular light shading
	  */
	 if (normal_type == ddd_sunview_cxp_drom_element_normals) {
	    if (vtxclrs != NULL) {
	       shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    } else {
	       shade_type = ddd_sunview_cxp_drom_element_shading;
	    }
	    for (elementno=0; elementno<elementcount; elementno++) {
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[elementno][0]
			+ light_normal[1]*normals[elementno][1] +
			      light_normal[2]*normals[elementno][2];
		  if (dot_prod < 0.0) dot_prod = 0.0;

		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       if (vtxclrs == NULL) {
		  shade[elementno][0] += specular_reflectance[0] * TLC[0];
		  shade[elementno][1] += specular_reflectance[1] * TLC[1];
		  shade[elementno][2] += specular_reflectance[2] * TLC[2];
	       } else {
		  for (i=0; i<vtx_per_element; i++) {
		     vtxno = elementno*vtx_per_element + i;
		     shade[vtxno][0] += specular_reflectance[0] * TLC[0];
		     shade[vtxno][1] += specular_reflectance[1] * TLC[1];
		     shade[vtxno][2] += specular_reflectance[2] * TLC[2];
		  }
	       }
	    }
	 } else { /* VERTEX NORMALS */
	    shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    for (vtxno=0; vtxno<vertexcount; vtxno++) {
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[vtxno][0]
			+ light_normal[1]*normals[vtxno][1] +
			      light_normal[2]*normals[vtxno][2];
		  if (dot_prod < 0.0) dot_prod = 0.0;
		  
		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       shade[vtxno][0] += specular_reflectance[0] * TLC[0];
	       shade[vtxno][1] += specular_reflectance[1] * TLC[1];
	       shade[vtxno][2] += specular_reflectance[2] * TLC[2];
	    }
	 }
      }
   }


   if (dde_sunview_cxp_drom_data.object_interpolation_type == DcConstantShade &&
       shade_type == ddd_sunview_cxp_drom_vertex_shading) {
      /*
       * average the shade values of the vertices of the element to
       * give a shade per element
       */
      shade_type = ddd_sunview_cxp_drom_element_shading;

      for (i=0; i<elementcount; i++) {
	 shade[i][0] = shade[i*vtx_per_element][0];
	 shade[i][1] = shade[i*vtx_per_element][1];
	 shade[i][2] = shade[i*vtx_per_element][2];
	 for (j=1; j<vtx_per_element; j++) {
	    shade[i][0] += shade[i*vtx_per_element+j][0];
	    shade[i][1] += shade[i*vtx_per_element+j][1];
	    shade[i][2] += shade[i*vtx_per_element+j][2];
	 }
	 shade[i][0] /= (DtReal)vtx_per_element;
	 shade[i][1] /= (DtReal)vtx_per_element;
	 shade[i][2] /= (DtReal)vtx_per_element;
      }
   } else if (dde_sunview_cxp_drom_data.object_interpolation_type != DcConstantShade &&
	      shade_type == ddd_sunview_cxp_drom_element_shading) {
      /*
       * only have one shade per element but the shading method is for 
       * one shade per vertex so duplicate the shades
       */

      shade_type = ddd_sunview_cxp_drom_vertex_shading;
      for (i=elementcount-1; i>=0; i--) {
	 for (j=0; j<vtx_per_element; j++) {
	    shade[vtx_per_element*i+j][0] = shade[i][0];
	    shade[vtx_per_element*i+j][1] = shade[i][1];
	    shade[vtx_per_element*i+j][2] = shade[i][2];
	 }
      }
   }

   /*
    * clip the values to the range 0 - 255
    */

   switch (shade_type) {
   case ddd_sunview_cxp_drom_element_shading:
      ddr_sunview_cxp_drom_shade_clip_color (elementcount, DcFalse, 1);
      break;
   case ddd_sunview_cxp_drom_vertex_shading:
      ddr_sunview_cxp_drom_shade_clip_color (vertexcount, DcFalse, 1);
      break;
   }

   /*
    * Do pseudo color conversion
    */

   if (dde_sunview_cxp_drom_data.pseudocolor_switch) {
      switch (shade_type) {
      case ddd_sunview_cxp_drom_element_shading:
	 ddr_sunview_cxp_drom_shade_to_pseudo (elementcount, DcFalse, 
				     dde_sunview_cxp_drom_curdevdat->shade_mode, 1);
	 break;
      case ddd_sunview_cxp_drom_vertex_shading:
	 ddr_sunview_cxp_drom_shade_to_pseudo (vertexcount, DcFalse, 
				     dde_sunview_cxp_drom_curdevdat->shade_mode, 1);
	 break;
      }
   }
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_shade_connected_line_list (vertexcount, elenrms,
				       vtxlocs, vtxnrms, vtxclrs,
				       shades_per_element)
     DtInt vertexcount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
     DtInt *shades_per_element;
{
   DtInt elementcount;
   DtInt normal_type;
   DtInt shade_type;
   DtRealTriple *shade;
   DtInt *color_indices;
   DtReal ambient_intensity;
   DtReal diffuse_intensity;
   DtReal specular_intensity;
   DtRealTriple ambient_reflectance;
   DtRealTriple diffuse_reflectance;
   DtRealTriple specular_reflectance;
   DtInt diffuse_light_switch;
   DtInt specular_light_switch;
   DtRealTriple *normals;
   DtInt elementno;
   DtInt vtxno;
   DtInt color;
   DtRealTriple TLC;
   DtReal light_normal[3];
   DtReal light_color[3];
   DtReal dot_prod;
   DtInt i, j;
   DtInt idx;
   DtInt light;
   DtRealTriple color_component;
   DtRealTriple save_component;

   elementcount = vertexcount - 1;

   /*
    * Make sure that there is enough space to hold all the shades for
    * the batch being processed.  If not then reallocate the space so
    * there is enough
    */

   ddr_sunview_cxp_drom_check_shade_buffers (0, vertexcount, vertexcount);

   shade = dde_sunview_cxp_drom_data.colored_elements;
   color_indices = dde_sunview_cxp_drom_data.color_indices;

   /*
    * scale the intensities and reflectances to the range of 0 - 255.
    * This is less computationally expensive to do the scaling now that
    * at the end when all the shades would have to be scaled.
    */

   ambient_intensity = dde_sunview_cxp_drom_data.object_ambient_intensity * 255;
   diffuse_intensity = dde_sunview_cxp_drom_data.object_diffuse_intensity * 255;
   specular_intensity = dde_sunview_cxp_drom_data.object_specular_intensity * 255;

if (vtxclrs == NULL) {
      ambient_reflectance[0] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[0]*255;
      ambient_reflectance[1] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[1]*255;
      ambient_reflectance[2] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[2]*255;
   } else {
      ambient_reflectance[0] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] 
	    * ambient_intensity;
      ambient_reflectance[1] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] 
	    * ambient_intensity;
      ambient_reflectance[2] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] 
	    * ambient_intensity;
   }      

   diffuse_reflectance[0] = dde_sunview_cxp_drom_data.diffuse_reflectance[0] * 255;
   diffuse_reflectance[1] = dde_sunview_cxp_drom_data.diffuse_reflectance[1] * 255;
   diffuse_reflectance[2] = dde_sunview_cxp_drom_data.diffuse_reflectance[2] * 255;

   specular_reflectance[0] = dde_sunview_cxp_drom_data.specular_reflectance[0] * 255;
   specular_reflectance[1] = dde_sunview_cxp_drom_data.specular_reflectance[1] * 255;
   specular_reflectance[2] = dde_sunview_cxp_drom_data.specular_reflectance[2] * 255;

   /*
    * determine what type of shading will be done.
    * light source shading requires at least a normal per element and
    * of course light sources.
    */

   diffuse_light_switch = (dde_sunview_cxp_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_sunview_cxp_drom_data.object_diffuse_switch;

   specular_light_switch = (dde_sunview_cxp_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_sunview_cxp_drom_data.object_specular_switch;

   ddr_sunview_cxp_drom_get_normal_type (elenrms, vtxnrms, &normal_type, &normals);

   if (normal_type == ddd_sunview_cxp_drom_no_normals || 
       (!diffuse_light_switch && !specular_light_switch)) {
      /*
       * No light source shading can be done or was not requested.  
       * Use the diffuse color or diffuse intensity and the vertex colors
       */

      if (vtxclrs == NULL) {
	 shade_type = ddd_sunview_cxp_drom_element_shading;
	 for (elementno=0; elementno<elementcount; elementno++) {
	    for (color=0; color<3; color++) {
	       shade[elementno][color] = diffuse_reflectance[color];
	    }
	 }
      } else { /* vtxclrs != NULL */
	 switch (dde_sunview_cxp_drom_data.object_interpolation_type) {
	 case DcConstantShade:
	    shade_type = ddd_sunview_cxp_drom_element_shading;
	    for (elementno=0; elementno<elementcount; elementno++) {
	       for (color=0; color<3; color++) {
		  for (i=0; i<2; i++) {
		     shade[elementno][color] += diffuse_intensity *
			   vtxclrs[elementno+i][color];
		  }
		  shade[elementno][color] /= 2.;
	       }
	    }
	    break;
	 case DcVertexShade:
	 case DcSurfaceShade:
	    shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    for (vtxno=0; vtxno<vertexcount; vtxno++) {
	       for (color=0; color<3; color++) {
		  shade[vtxno][color] = diffuse_intensity *
			vtxclrs[vtxno][color];
	       }
	    }
	    break;
	 }
      } 
   } else { /* Some form of light shading */
      if (dde_sunview_cxp_drom_data.object_ambient_switch) {
	 if (vtxclrs == NULL) {
	    for (i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0];
	       shade[i][1] = ambient_reflectance[1];
	       shade[i][2] = ambient_reflectance[2];
	    }
	 } else {
	    for (i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0]*vtxclrs[i][0];
	       shade[i][1] = ambient_reflectance[1]*vtxclrs[i][1];
	       shade[i][2] = ambient_reflectance[2]*vtxclrs[i][2];
	    }
	 }
      } else {
	 for (i=0; i<vertexcount; i++) {
	    shade[i][0] = 0.0;
	    shade[i][1] = 0.0;
	    shade[i][2] = 0.0;
	 }
      }

      if (diffuse_light_switch) {
	 /*
	  * diffuse light shading
	  */

	 if (normal_type == ddd_sunview_cxp_drom_element_normals) {
	    shade_type = ddd_sunview_cxp_drom_element_shading;
	    
	    for (elementno=0; elementno<elementcount; elementno++) {
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
		  
		  dot_prod = light_normal[0]*normals[elementno][0] +
			light_normal[1]*normals[elementno][1] +
			      light_normal[2]*normals[elementno][2];
		  if (dot_prod < 0.0) dot_prod = 0.0;

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }
	       if (vtxclrs == NULL) {
		  shade[elementno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[elementno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[elementno][2] += diffuse_reflectance[2] * TLC[2];
	       } else { /* vtxclrs != NULL */
		  color_component[0] = color_component[1] =
			color_component[2] = 0.;
		  for (i=0; i<2; i++) {
		     color_component[0] += diffuse_intensity * TLC[0] *
			   vtxclrs[elementno+i][0];
		     color_component[1] += diffuse_intensity * TLC[1] *
			   vtxclrs[elementno+i][1];
		     color_component[2] += diffuse_intensity * TLC[2] *
			   vtxclrs[elementno+i][2];
		  }
		  shade[elementno][0] += color_component[0] / 2.;
		  shade[elementno][1] += color_component[1] / 2.;
		  shade[elementno][2] += color_component[2] / 2.;
	       }
	    } /* forall elements */
	 } else { /* VERTEX NORMALS */
	    switch (dde_sunview_cxp_drom_data.object_interpolation_type) {
	    case DcConstantShade:
	       shade_type = ddd_sunview_cxp_drom_element_shading;

	       for (vtxno=0; vtxno<vertexcount; vtxno++) {
		  TLC[0] = TLC[1] = TLC[2] = 0;
		     
		  for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		     light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		     light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		     light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		     light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		     light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		     light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		     dot_prod = light_normal[0]*normals[vtxno][0] +
			   light_normal[1]*normals[vtxno][1] +
				 light_normal[2]*normals[vtxno][2];
		     if (dot_prod < 0.0) dot_prod = 0.0;
			   
		     TLC[0] += dot_prod * light_color[0];
		     TLC[1] += dot_prod * light_color[1];
		     TLC[2] += dot_prod * light_color[2];
		  }
		     
		  if (vtxclrs != NULL) {
		     color_component[0] = diffuse_intensity*TLC[0]*
			   vtxclrs[vtxno][0];
		     color_component[1] = diffuse_intensity*TLC[1]*
			   vtxclrs[vtxno][1];
		     color_component[2] = diffuse_intensity*TLC[2]*
			   vtxclrs[vtxno][2];
		  } else {
		     color_component[0] = diffuse_reflectance[0]*TLC[0];
		     color_component[1] = diffuse_reflectance[1]*TLC[1];
		     color_component[2] = diffuse_reflectance[2]*TLC[2];
		  }
		  if (vtxno > 0) {
		     shade[vtxno-1][0] += (color_component[0] +
					   save_component[0] ) / 2.;
		     shade[vtxno-1][1] += (color_component[1] +
					   save_component[1] ) / 2.;
		     shade[vtxno-1][2] += (color_component[2] +
					   save_component[2] ) / 2.;
		  }
		  save_component[0] = color_component[0];
		  save_component[1] = color_component[1];
		  save_component[2] = color_component[2];
	       }
	       break;

	    case DcVertexShade:
	    case DcSurfaceShade:
	       shade_type = ddd_sunview_cxp_drom_vertex_shading;
	       for (vtxno=0; vtxno<vertexcount; vtxno++) {
		  
		  TLC[0] = TLC[1] = TLC[2] = 0;

		  for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		     light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		     light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		     light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		     light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		     light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		     light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		     dot_prod = light_normal[0]*normals[vtxno][0] +
			   light_normal[1]*normals[vtxno][1] +
				 light_normal[2]*normals[vtxno][2];
		     if (dot_prod < 0.0) dot_prod = 0.0;
		  
		     TLC[0] += dot_prod * light_color[0];
		     TLC[1] += dot_prod * light_color[1];
		     TLC[2] += dot_prod * light_color[2];
		  }

		  if (vtxclrs == NULL) {
		     shade[vtxno][0] += diffuse_reflectance[0] * TLC[0];
		     shade[vtxno][1] += diffuse_reflectance[1] * TLC[1];
		     shade[vtxno][2] += diffuse_reflectance[2] * TLC[2];
		  } else {
		     shade[vtxno][0] += diffuse_intensity * TLC[0] *
			   vtxclrs[vtxno][0];
		     shade[vtxno][1] += diffuse_intensity * TLC[1] *
			   vtxclrs[vtxno][1];
		     shade[vtxno][2] += diffuse_intensity * TLC[2] *
			   vtxclrs[vtxno][2];
		  }
	       } /* forall vertices */
	    } /* switch (interpolation_type) */
	 }
      } /* diffuse_light_switch */

      if (specular_light_switch) {
	 /*
	  * specular light shading
	  */
	 if (normal_type == ddd_sunview_cxp_drom_element_normals) {
	    shade_type = ddd_sunview_cxp_drom_element_shading;

	    for (elementno=0; elementno<elementcount; elementno++) {
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[elementno][0]
			+ light_normal[1]*normals[elementno][1] +
			      light_normal[2]*normals[elementno][2];
		  if (dot_prod < 0.0) dot_prod = 0.0;

		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       shade[elementno][0] += specular_reflectance[0] * TLC[0];
	       shade[elementno][1] += specular_reflectance[1] * TLC[1];
	       shade[elementno][2] += specular_reflectance[2] * TLC[2];
	    }
	 } else { /* VERTEX NORMALS */
	    switch (dde_sunview_cxp_drom_data.object_interpolation_type) {
	    case DcConstantShade:
	       shade_type = ddd_sunview_cxp_drom_element_shading;

	       for (vtxno=0; vtxno<vertexcount; vtxno++) {
		  TLC[0] = TLC[1] = TLC[2] = 0;

		  for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		     light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		     light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		     light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		     light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		     light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		     light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		     dot_prod = light_normal[0]*normals[vtxno][0]
			   + light_normal[1]*normals[vtxno][1] +
				 light_normal[2]*normals[vtxno][2];
		     if (dot_prod < 0.0) dot_prod = 0.0;
		     
		     idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		     dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

		     TLC[0] += dot_prod * light_color[0];
		     TLC[1] += dot_prod * light_color[1];
		     TLC[2] += dot_prod * light_color[2];
		  }

		  color_component[0] = specular_reflectance[0]*TLC[0];
		  color_component[1] = specular_reflectance[1]*TLC[1];
		  color_component[2] = specular_reflectance[2]*TLC[2];

		  if (vtxno > 0) {
		     shade[vtxno-1][0] += (color_component[0] +
					   save_component[0] ) / 2.;
		     shade[vtxno-1][1] += (color_component[1] +
					   save_component[1] ) / 2.;
		     shade[vtxno-1][2] += (color_component[2] +
					   save_component[2] ) / 2.;
		  }
		  save_component[0] = color_component[0];
		  save_component[1] = color_component[1];
		  save_component[2] = color_component[2];
	       }
	       break;
	    case DcVertexShade:
	    case DcSurfaceShade:
	       shade_type = ddd_sunview_cxp_drom_vertex_shading;
	       for (vtxno=0; vtxno<vertexcount; vtxno++) {
		  
		  TLC[0] = TLC[1] = TLC[2] = 0;

		  for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		     light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		     light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		     light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		     light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		     light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		     light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		     dot_prod = light_normal[0]*normals[vtxno][0]
			   + light_normal[1]*normals[vtxno][1] +
				 light_normal[2]*normals[vtxno][2];
		     if (dot_prod < 0.0) dot_prod = 0.0;
		  
		     idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		     dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

		     TLC[0] += dot_prod * light_color[0];
		     TLC[1] += dot_prod * light_color[1];
		     TLC[2] += dot_prod * light_color[2];
		  }

		  shade[vtxno][0] += specular_reflectance[0] * TLC[0];
		  shade[vtxno][1] += specular_reflectance[1] * TLC[1];
		  shade[vtxno][2] += specular_reflectance[2] * TLC[2];
	       }
	       break;
	    }
	 }
      }
   }

   /*
    * clip the values to the range 0 - 255
    */

   switch (shade_type) {
   case ddd_sunview_cxp_drom_element_shading:
      ddr_sunview_cxp_drom_shade_clip_color (elementcount, DcFalse, 1);
      break;
   case ddd_sunview_cxp_drom_vertex_shading:
      ddr_sunview_cxp_drom_shade_clip_color (vertexcount, DcFalse, 1);
      break;
   }

   /*
    * Do pseudo color conversion
    */

   if (dde_sunview_cxp_drom_data.pseudocolor_switch) {
      switch (shade_type) {
      case ddd_sunview_cxp_drom_element_shading:
	 ddr_sunview_cxp_drom_shade_to_pseudo (elementcount, DcFalse, 
				     dde_sunview_cxp_drom_curdevdat->shade_mode, 1);
	 break;
      case ddd_sunview_cxp_drom_vertex_shading:
	 ddr_sunview_cxp_drom_shade_to_pseudo (vertexcount, DcFalse, 
				     dde_sunview_cxp_drom_curdevdat->shade_mode, 1);
	 break;
      }
   }
   if (shade_type == ddd_sunview_cxp_drom_element_shading)
	 *shades_per_element = 1;
   else
	 *shades_per_element = 2;
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_shade_triangle_list (elementcount, elenrms, vertexcount, vtxlocs,
				 vtxnrms, vtxclrs, 
				 backfacing)
     DtInt elementcount;
     DtRealTriple *elenrms;
     DtInt vertexcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
     DtInt **backfacing;
{
   DtInt normal_type;
   DtInt shade_type;
   DtRealTriple *shade;
   DtInt *color_indices;
   DtInt *backfacing_flags;

   DtReal ambient_intensity;
   DtReal diffuse_intensity;
   DtReal specular_intensity;
   DtRealTriple ambient_reflectance;
   DtRealTriple diffuse_reflectance;
   DtRealTriple specular_reflectance;
   DtInt diffuse_light_switch;
   DtInt specular_light_switch;
   DtInt culling_switch;
   DtRealTriple *normals;
   DtInt elementno;
   DtInt vtxno;
   DtInt color;
   DtRealTriple TLC;
   DtReal light_normal[3];
   DtReal light_color[3];
   DtReal dot_prod;
   DtInt vtx_per_element;
   DtInt i, j;
   DtInt idx;
   DtInt light;
   DtReal xx1, xx2, xx3, xy1, xy2, xy3;

   vtx_per_element = vertexcount / elementcount;

   ddr_sunview_cxp_drom_check_shade_buffers (vertexcount, vertexcount, vertexcount);

   backfacing_flags = dde_sunview_cxp_drom_data.backfacing_flags;
   shade = dde_sunview_cxp_drom_data.colored_elements;
   color_indices = dde_sunview_cxp_drom_data.color_indices;

   /*
    * scale the intensities and reflectances to the range of 0 - 255.
    * This is less computationally expensive to do the scaling now that
    * at the end when all the shades would have to be scaled.
    */

   ambient_intensity = dde_sunview_cxp_drom_data.object_ambient_intensity * 255;
   diffuse_intensity = dde_sunview_cxp_drom_data.object_diffuse_intensity * 255;
   specular_intensity = dde_sunview_cxp_drom_data.object_specular_intensity * 255;

   if (vtxclrs == NULL) {
      ambient_reflectance[0] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[0]*255;
      ambient_reflectance[1] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[1]*255;
      ambient_reflectance[2] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[2]*255;
   } else {
      ambient_reflectance[0] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] 
	    * ambient_intensity;
      ambient_reflectance[1] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] 
	    * ambient_intensity;
      ambient_reflectance[2] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] 
	    * ambient_intensity;
   }      

   diffuse_reflectance[0] = dde_sunview_cxp_drom_data.diffuse_reflectance[0] * 255;
   diffuse_reflectance[1] = dde_sunview_cxp_drom_data.diffuse_reflectance[1] * 255;
   diffuse_reflectance[2] = dde_sunview_cxp_drom_data.diffuse_reflectance[2] * 255;

   specular_reflectance[0] = dde_sunview_cxp_drom_data.specular_reflectance[0] * 255;
   specular_reflectance[1] = dde_sunview_cxp_drom_data.specular_reflectance[1] * 255;
   specular_reflectance[2] = dde_sunview_cxp_drom_data.specular_reflectance[2] * 255;

   /*
    * determine what type of shading will be done.
    * light source shading requires at least a normal per element and
    * of course light sources.
    */

   diffuse_light_switch = (dde_sunview_cxp_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_sunview_cxp_drom_data.object_diffuse_switch;

   specular_light_switch = (dde_sunview_cxp_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_sunview_cxp_drom_data.object_specular_switch;

   culling_switch = (dde_sunview_cxp_drom_data.object_cull_switch &&
		     dde_sunview_cxp_drom_data.object_cullable_switch);

   ddr_sunview_cxp_drom_get_normal_type (elenrms, vtxnrms, &normal_type, &normals);

   /*
    * determine if triangles are backfacing
    */

   for (i=0; i<elementcount; i++) {
      xx1 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][0]*vtxlocs[i*3+0][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][1]*vtxlocs[i*3+0][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][2]*vtxlocs[i*3+0][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][3];
      xy1 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][0]*vtxlocs[i*3+0][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][1]*vtxlocs[i*3+0][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][2]*vtxlocs[i*3+0][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][3];
      xx2 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][0]*vtxlocs[i*3+1][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][1]*vtxlocs[i*3+1][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][2]*vtxlocs[i*3+1][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][3];
      xy2 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][0]*vtxlocs[i*3+1][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][1]*vtxlocs[i*3+1][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][2]*vtxlocs[i*3+1][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][3];
      xx3 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][0]*vtxlocs[i*3+2][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][1]*vtxlocs[i*3+2][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][2]*vtxlocs[i*3+2][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][3];
      xy3 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][0]*vtxlocs[i*3+2][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][1]*vtxlocs[i*3+2][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][2]*vtxlocs[i*3+2][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][3];
      
      if (((xx3 - xx1)*(xy2 - xy1)) > ((xx2 - xx1)*(xy3 - xy1))) {
	 backfacing_flags[i] = 1;
      } else {
	 backfacing_flags[i] = 0;
      }
   }

   if (normal_type == ddd_sunview_cxp_drom_no_normals || 
       (!diffuse_light_switch && !specular_light_switch)) {
      /*
       * No light source shading can be done or was not requested.  
       * Use the diffuse color or diffuse intensity and the vertex colors
       */

      if (vtxclrs == NULL) {
	 shade_type = ddd_sunview_cxp_drom_element_shading;
	 for (elementno=0; elementno<elementcount; elementno++) {
	    for (color=0; color<3; color++) {
	       shade[elementno][color] = diffuse_reflectance[color];
	    }
	 }
      } else {
	 shade_type = ddd_sunview_cxp_drom_vertex_shading;
	 for (vtxno=0; vtxno<vertexcount; vtxno++) {
	    for (color=0; color<3; color++) {
	       shade[vtxno][color] = diffuse_intensity*vtxclrs[vtxno][color];
	    }
	 }
      }
   } else {
      if (dde_sunview_cxp_drom_data.object_ambient_switch) {
	 if (vtxclrs == NULL) {
	    for (i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0];
	       shade[i][1] = ambient_reflectance[1];
	       shade[i][2] = ambient_reflectance[2];
	    }
	 } else {
	    for (i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0]*vtxclrs[i][0];
	       shade[i][1] = ambient_reflectance[1]*vtxclrs[i][1];
	       shade[i][2] = ambient_reflectance[2]*vtxclrs[i][2];
	    }
	 }
      } else {
	 for (i=0; i<vertexcount; i++) {
	    shade[i][0] = 0.0;
	    shade[i][1] = 0.0;
	    shade[i][2] = 0.0;
	 }
      }

      if (diffuse_light_switch) {
	 /*
	  * diffuse light shading
	  */

	 if (normal_type == ddd_sunview_cxp_drom_element_normals) {
	    if (vtxclrs != NULL) {
	       shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    } else {
	       shade_type = ddd_sunview_cxp_drom_element_shading;
	    }
	    for (elementno=0; elementno<elementcount; elementno++) {
	       if (culling_switch && backfacing_flags[elementno])
		     continue;
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
		  
		  dot_prod = light_normal[0]*normals[elementno][0] +
			light_normal[1]*normals[elementno][1] +
			      light_normal[2]*normals[elementno][2];
		  if (dot_prod < 0.0) {
		     if (backfacing_flags[elementno])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }
		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }
	       if (vtxclrs == NULL) {
		  shade[elementno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[elementno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[elementno][2] += diffuse_reflectance[2] * TLC[2];
	       } else {
		  for (i=0; i<vtx_per_element; i++) {
		     vtxno = elementno*vtx_per_element + i;
		     shade[vtxno][0] += diffuse_intensity * TLC[0] *
			   vtxclrs[vtxno][0];
		     shade[vtxno][1] += diffuse_intensity * TLC[1] *
			   vtxclrs[vtxno][1];
		     shade[vtxno][2] += diffuse_intensity * TLC[2] *
			   vtxclrs[vtxno][2];
		  }
	       }
	    }
	 } else { /* VERTEX NORMALS */
	    shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    for (vtxno=0; vtxno<vertexcount; vtxno++) {
	       if (culling_switch && backfacing_flags[vtxno/vtx_per_element])
		     continue;
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[vtxno][0] +
			light_normal[1]*normals[vtxno][1] +
			      light_normal[2]*normals[vtxno][2];
		  if (dot_prod < 0.0) {
		     if (backfacing_flags[vtxno / vtx_per_element])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }
		  
		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       if (vtxclrs == NULL) {
		  shade[vtxno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[vtxno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[vtxno][2] += diffuse_reflectance[2] * TLC[2];
	       } else {
		  shade[vtxno][0] += diffuse_intensity * TLC[0] *
			vtxclrs[vtxno][0];
		  shade[vtxno][1] += diffuse_intensity * TLC[1] *
			vtxclrs[vtxno][1];
		  shade[vtxno][2] += diffuse_intensity * TLC[2] *
			vtxclrs[vtxno][2];
	       }
	    }
	 }
      }
      if (specular_light_switch) {
	 /*
	  * specular light shading
	  */
	 if (normal_type == ddd_sunview_cxp_drom_element_normals) {
	    if (vtxclrs != NULL) {
	       shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    } else {
	       shade_type = ddd_sunview_cxp_drom_element_shading;
	    }
	    for (elementno=0; elementno<elementcount; elementno++) {
	       if (culling_switch && backfacing_flags[elementno])
		     continue;
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[elementno][0]
			+ light_normal[1]*normals[elementno][1] +
			      light_normal[2]*normals[elementno][2];
		  if (dot_prod < 0.0) {
		     if (backfacing_flags[elementno])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }

		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       if (vtxclrs == NULL) {
		  shade[elementno][0] += specular_reflectance[0] * TLC[0];
		  shade[elementno][1] += specular_reflectance[1] * TLC[1];
		  shade[elementno][2] += specular_reflectance[2] * TLC[2];
	       } else {
		  for (i=0; i<vtx_per_element; i++) {
		     vtxno = elementno*vtx_per_element + i;
		     shade[vtxno][0] += specular_reflectance[0] * TLC[0];
		     shade[vtxno][1] += specular_reflectance[1] * TLC[1];
		     shade[vtxno][2] += specular_reflectance[2] * TLC[2];
		  }
	       }
	    }
	 } else { /* VERTEX NORMALS */
	    shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    for (vtxno=0; vtxno<vertexcount; vtxno++) {
	       if (culling_switch && backfacing_flags[vtxno/vtx_per_element])
		     continue;
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[vtxno][0]
			+ light_normal[1]*normals[vtxno][1] +
			      light_normal[2]*normals[vtxno][2];
		  if (dot_prod < 0.0) {
		     if (backfacing_flags[vtxno / vtx_per_element])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }
		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       shade[vtxno][0] += specular_reflectance[0] * TLC[0];
	       shade[vtxno][1] += specular_reflectance[1] * TLC[1];
	       shade[vtxno][2] += specular_reflectance[2] * TLC[2];
	    }
	 }
      }
   }


   if (dde_sunview_cxp_drom_data.object_interpolation_type == DcConstantShade &&
       shade_type == ddd_sunview_cxp_drom_vertex_shading) {
      /*
       * average the shade values of the vertices of the element to
       * give a shade per element
       */
      shade_type = ddd_sunview_cxp_drom_element_shading;

      for (i=0; i<elementcount; i++) {
	 if (culling_switch && backfacing_flags[i])
	       continue;
	 shade[i][0] = shade[i*vtx_per_element][0];
	 shade[i][1] = shade[i*vtx_per_element][1];
	 shade[i][2] = shade[i*vtx_per_element][2];
	 for (j=1; j<vtx_per_element; j++) {
	    shade[i][0] += shade[i*vtx_per_element+j][0];
	    shade[i][1] += shade[i*vtx_per_element+j][1];
	    shade[i][2] += shade[i*vtx_per_element+j][2];
	 }
	 shade[i][0] /= (DtReal)vtx_per_element;
	 shade[i][1] /= (DtReal)vtx_per_element;
	 shade[i][2] /= (DtReal)vtx_per_element;
      }
   } else if (dde_sunview_cxp_drom_data.object_interpolation_type != DcConstantShade &&
	      shade_type == ddd_sunview_cxp_drom_element_shading) {
      /*
       * only have one shade per element but the shading method is for 
       * one shade per vertex so duplicate the shades
       */

      shade_type = ddd_sunview_cxp_drom_vertex_shading;
      for (i=elementcount-1; i>=0; i--) {
	 if (culling_switch && backfacing_flags[i])
	       continue;
	 for (j=0; j<vtx_per_element; j++) {
	    shade[vtx_per_element*i+j][0] = shade[i][0];
	    shade[vtx_per_element*i+j][1] = shade[i][1];
	    shade[vtx_per_element*i+j][2] = shade[i][2];
	 }
      }
   }

   /*
    * clip the values to the range 0 - 255
    */

   switch (shade_type) {
   case ddd_sunview_cxp_drom_element_shading:
      ddr_sunview_cxp_drom_shade_clip_color (elementcount, culling_switch, 1);
      break;
   case ddd_sunview_cxp_drom_vertex_shading:
      ddr_sunview_cxp_drom_shade_clip_color (vertexcount, culling_switch, 
				   vtx_per_element);
      break;
   }

   /*
    * Do pseudo color conversion
    */

   if (dde_sunview_cxp_drom_data.pseudocolor_switch) {
      switch (shade_type) {
      case ddd_sunview_cxp_drom_element_shading:
	 ddr_sunview_cxp_drom_shade_to_pseudo (elementcount, culling_switch, 
				     dde_sunview_cxp_drom_curdevdat->shade_mode,
				     1);
	 break;
      case ddd_sunview_cxp_drom_vertex_shading:
	 ddr_sunview_cxp_drom_shade_to_pseudo (vertexcount, culling_switch,
				     dde_sunview_cxp_drom_curdevdat->shade_mode,
				     vtx_per_element);
	 break;
      }
   }
   if (culling_switch)
	 *backfacing = backfacing_flags;
   else
	 *backfacing = NULL;
}





/*
 ======================================================================
 */

ddr_sunview_cxp_drom_shade_triangle_mesh (trianglecount, elenrms, vertexcount, 
				 vtxlocs, vtxnrms, vtxclrs, vertexlist,
				 shades_per_element, backfacing)
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtInt vertexcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
     DtInt vertexlist[][3];
     DtInt *shades_per_element;
     DtInt **backfacing;
{
   DtInt normal_type;
   DtInt shade_type;
   DtRealTriple *shade;
   DtInt *color_indices;
   DtInt *backfacing_flags;
   DtReal ambient_intensity;
   DtReal diffuse_intensity;
   DtReal specular_intensity;
   DtRealTriple ambient_reflectance;
   DtRealTriple diffuse_reflectance;
   DtRealTriple specular_reflectance;
   DtInt diffuse_light_switch;
   DtInt specular_light_switch;
   DtInt culling_switch;
   DtRealTriple *normals;
   DtInt maxshades;
   DtInt elementno;
   DtInt vtxno;
   DtInt color;
   DtRealTriple TLC;
   DtRealTriple TLC2[2];
   DtReal light_normal[3];
   DtReal light_color[3];
   DtReal dot_prod;
   DtInt i, j;
   DtInt idx;
   DtInt light;
   DtRealTriple color_component;
   DtReal xx1, xx2, xx3, xy1, xy2, xy3;

   maxshades = 2*vertexcount > trianglecount ? 2*vertexcount : 
	 trianglecount;

   ddr_sunview_cxp_drom_check_shade_buffers (trianglecount, maxshades, maxshades);

   backfacing_flags = dde_sunview_cxp_drom_data.backfacing_flags;
   shade = dde_sunview_cxp_drom_data.colored_elements;
   color_indices = dde_sunview_cxp_drom_data.color_indices;

   /*
    * scale the intensities and reflectances to the range of 0 - 255.
    * This is less computationally expensive to do the scaling now that
    * at the end when all the shades would have to be scaled.
    */

   ambient_intensity = dde_sunview_cxp_drom_data.object_ambient_intensity * 255;
   diffuse_intensity = dde_sunview_cxp_drom_data.object_diffuse_intensity * 255;
   specular_intensity = dde_sunview_cxp_drom_data.object_specular_intensity * 255;

   if (vtxclrs == NULL) {
      ambient_reflectance[0] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[0]*255;
      ambient_reflectance[1] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[1]*255;
      ambient_reflectance[2] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] 
	    * dde_sunview_cxp_drom_data.ambient_reflectance[2]*255;
   } else {
      ambient_reflectance[0] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[0] 
	    * ambient_intensity;
      ambient_reflectance[1] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[1] 
	    * ambient_intensity;
      ambient_reflectance[2] = dde_sunview_cxp_drom_curlgtdat->ambient_reflectance[2] 
	    * ambient_intensity;
   }      

   diffuse_reflectance[0] = dde_sunview_cxp_drom_data.diffuse_reflectance[0] * 255;
   diffuse_reflectance[1] = dde_sunview_cxp_drom_data.diffuse_reflectance[1] * 255;
   diffuse_reflectance[2] = dde_sunview_cxp_drom_data.diffuse_reflectance[2] * 255;

   specular_reflectance[0] = dde_sunview_cxp_drom_data.specular_reflectance[0] * 255;
   specular_reflectance[1] = dde_sunview_cxp_drom_data.specular_reflectance[1] * 255;
   specular_reflectance[2] = dde_sunview_cxp_drom_data.specular_reflectance[2] * 255;

   /*
    * determine what type of shading will be done.
    * light source shading requires at least a normal per element and
    * of course light sources.
    */

   diffuse_light_switch = (dde_sunview_cxp_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_sunview_cxp_drom_data.object_diffuse_switch;

   specular_light_switch = (dde_sunview_cxp_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_sunview_cxp_drom_data.object_specular_switch;

   culling_switch = (dde_sunview_cxp_drom_data.object_cull_switch &&
		     dde_sunview_cxp_drom_data.object_cullable_switch);

   ddr_sunview_cxp_drom_get_normal_type (elenrms, vtxnrms, &normal_type, &normals);

   /*
    * determine if triangles are backfacing
    */

   for (i=0; i<trianglecount; i++) {
      xx1 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][0]*vtxlocs[vertexlist[i][0]][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][1]*vtxlocs[vertexlist[i][0]][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][2]*vtxlocs[vertexlist[i][0]][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][3];
      xy1 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][0]*vtxlocs[vertexlist[i][0]][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][1]*vtxlocs[vertexlist[i][0]][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][2]*vtxlocs[vertexlist[i][0]][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][3];
      xx2 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][0]*vtxlocs[vertexlist[i][1]][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][1]*vtxlocs[vertexlist[i][1]][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][2]*vtxlocs[vertexlist[i][1]][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][3];
      xy2 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][0]*vtxlocs[vertexlist[i][1]][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][1]*vtxlocs[vertexlist[i][1]][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][2]*vtxlocs[vertexlist[i][1]][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][3];
      xx3 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][0]*vtxlocs[vertexlist[i][2]][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][1]*vtxlocs[vertexlist[i][2]][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][2]*vtxlocs[vertexlist[i][2]][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[0][3];
      xy3 = dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][0]*vtxlocs[vertexlist[i][2]][0] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][1]*vtxlocs[vertexlist[i][2]][1] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][2]*vtxlocs[vertexlist[i][2]][2] + 
	    dde_sunview_cxp_drom_curcamdat->lcstofcsmat[1][3];
      
      if (((xx3 - xx1)*(xy2 - xy1)) > ((xx2 - xx1)*(xy3 - xy1))) {
	 backfacing_flags[i] = 1;
      } else {
	 backfacing_flags[i] = 0;
      }
   }

   if (normal_type == ddd_sunview_cxp_drom_no_normals || 
       (!diffuse_light_switch && !specular_light_switch)) {
      /*
       * No light source shading can be done or was not requested.  
       * Use the diffuse color or diffuse intensity and the vertex colors
       */

      if (vtxclrs == NULL) {
	 shade_type = ddd_sunview_cxp_drom_element_shading;
	 for (elementno=0; elementno<trianglecount; elementno++) {
	    if (culling_switch && backfacing_flags[elementno])
		  continue;
	    for (color=0; color<3; color++) {
	       shade[elementno][color] = diffuse_reflectance[color];
	    }
	 }
      } else { /* vtxclrs != NULL */
	 switch (dde_sunview_cxp_drom_data.object_interpolation_type) {
	 case DcConstantShade:
	    shade_type = ddd_sunview_cxp_drom_element_shading;
	    for (elementno=0; elementno<trianglecount; elementno++) {
	       if (culling_switch && backfacing_flags[elementno])
		     continue;
	       for (color=0; color<3; color++) {
		  for (i=0; i<3; i++) {
		     shade[elementno][color] += diffuse_intensity *
			   vtxclrs[vertexlist[elementno][i]][color];
		  }
		  shade[elementno][color] /= 3.;
	       }
	    }
	    break;
	 case DcVertexShade:
	 case DcSurfaceShade:
	    shade_type = ddd_sunview_cxp_drom_vertex_shading;
	    for (vtxno=0; vtxno<vertexcount; vtxno++) {
	       for (color=0; color<3; color++) {
		  shade[vtxno][color] += diffuse_intensity *
			vtxclrs[vtxno][color];
	       }
	    }
	    break;
	 }
      } 
   } else { /* Some form of light shading */
      if (dde_sunview_cxp_drom_data.object_ambient_switch) {
	 if (vtxclrs == NULL) {
	    for (i=0; i<maxshades; i++) {
	       shade[i][0] = ambient_reflectance[0];
	       shade[i][1] = ambient_reflectance[1];
	       shade[i][2] = ambient_reflectance[2];
	    }
	 } else {
	    for (i=0; i<maxshades; i++) {
	       shade[i][0] = ambient_reflectance[0]*vtxclrs[i][0];
	       shade[i][1] = ambient_reflectance[1]*vtxclrs[i][1];
	       shade[i][2] = ambient_reflectance[2]*vtxclrs[i][2];
	    }
	 }
      } else {
	 for (i=0; i<maxshades; i++) {
	    shade[i][0] = 0.0;
	    shade[i][1] = 0.0;
	    shade[i][2] = 0.0;
	 }
      }

      if (diffuse_light_switch) {
	 /*
	  * diffuse light shading
	  */

	 if (normal_type == ddd_sunview_cxp_drom_element_normals) {
	    shade_type = ddd_sunview_cxp_drom_element_shading;
	    
	    for (elementno=0; elementno<trianglecount; elementno++) {
	       if (culling_switch && backfacing_flags[elementno])
		     continue;
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
		  
		  dot_prod = light_normal[0]*normals[elementno][0] +
			light_normal[1]*normals[elementno][1] +
			      light_normal[2]*normals[elementno][2];
		  if (dot_prod < 0.0) {
		     if (backfacing_flags[elementno])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }
		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }
	       if (vtxclrs == NULL) {
		  shade[elementno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[elementno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[elementno][2] += diffuse_reflectance[2] * TLC[2];
	       } else { /* vtxclrs != NULL */
		  color_component[0] = color_component[1] =
			color_component[2] = 0.;
		  for (i=0; i<3; i++) {
		     color_component[0] += diffuse_intensity * TLC[0] *
			   vtxclrs[vertexlist[elementno][i]][0];
		     color_component[1] += diffuse_intensity * TLC[1] *
			   vtxclrs[vertexlist[elementno][i]][1];
		     color_component[2] += diffuse_intensity * TLC[2] *
			   vtxclrs[vertexlist[elementno][i]][2];
		  }
		  shade[elementno][0] += color_component[0] / 3.;
		  shade[elementno][1] += color_component[1] / 3.;
		  shade[elementno][2] += color_component[2] / 3.;
	       }
	    } /* forall elements */
	 } else { /* VERTEX NORMALS */
	    switch (dde_sunview_cxp_drom_data.object_interpolation_type) {
	    case DcConstantShade:
	       shade_type = ddd_sunview_cxp_drom_element_shading;

	       for (elementno=0; elementno<trianglecount; elementno++) {
		  if (culling_switch && backfacing_flags[elementno])
			continue;
		  
		  color_component[0] = color_component[1] =
			color_component[2] = 0.;

		  for (vtxno=0; vtxno<3; vtxno++) {
		     TLC[0] = TLC[1] = TLC[2] = 0;
		     
		     for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
			light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
			light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
			light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
			light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
			light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
			light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
			dot_prod = light_normal[0]*normals[vtxno][0] +
			      light_normal[1]*normals[vtxno][1] +
				    light_normal[2]*normals[vtxno][2];
			if (dot_prod < 0.0) {
			   if (backfacing_flags[elementno])
				 dot_prod *= -1.0;
			   else
				 dot_prod = 0.0;
			}
			TLC[0] += dot_prod * light_color[0];
			TLC[1] += dot_prod * light_color[1];
			TLC[2] += dot_prod * light_color[2];
		     }
		     
		     if (vtxclrs != NULL) {
			color_component[0] += diffuse_intensity*TLC[0]*
			      vtxclrs[vertexlist[elementno][vtxno]][0];
			color_component[1] += diffuse_intensity*TLC[1]*
			      vtxclrs[vertexlist[elementno][vtxno]][1];
			color_component[2] += diffuse_intensity*TLC[2]*
			      vtxclrs[vertexlist[elementno][vtxno]][2];
		     } else {
			color_component[0] += diffuse_reflectance[0]*TLC[0];
			color_component[1] += diffuse_reflectance[1]*TLC[1];
			color_component[2] += diffuse_reflectance[2]*TLC[2];
		     }
		  } /* for 3 vertices */
		  shade[elementno][0] += color_component[0] / 3.;
		  shade[elementno][1] += color_component[1] / 3.;
		  shade[elementno][2] += color_component[2] / 3.;
	       } /* for all elements */
	       break;

	    case DcVertexShade:
	    case DcSurfaceShade:
	       shade_type = ddd_sunview_cxp_drom_vertex_shading;
	       for (vtxno=0; vtxno<vertexcount; vtxno++) {
		  
		  TLC2[0][0] = TLC2[0][1] = TLC2[0][2] = 0;
		  TLC2[1][0] = TLC2[1][1] = TLC2[1][2] = 0;

		  for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		     light_normal[0] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][0];
		     light_normal[1] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][1];
		     light_normal[2] = dde_sunview_cxp_drom_curlgtdat->lcscosine[light][2];
		     light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		     light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		     light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		     dot_prod = light_normal[0]*normals[vtxno][0] +
			   light_normal[1]*normals[vtxno][1] +
				 light_normal[2]*normals[vtxno][2];
		     if (dot_prod < 0.0) {
			dot_prod *= -1.0;
			TLC2[1][0] += dot_prod * light_color[0];
			TLC2[1][1] += dot_prod * light_color[1];
			TLC2[1][2] += dot_prod * light_color[2];
		     } else {
			TLC2[0][0] += dot_prod * light_color[0];
			TLC2[0][1] += dot_prod * light_color[1];
			TLC2[0][2] += dot_prod * light_color[2];
		     }
		  }

		  if (vtxclrs == NULL) {
		     shade[2*vtxno+0][0] += diffuse_reflectance[0]*TLC2[0][0];
		     shade[2*vtxno+0][1] += diffuse_reflectance[1]*TLC2[0][1];
		     shade[2*vtxno+0][2] += diffuse_reflectance[2]*TLC2[0][2];
		     shade[2*vtxno+1][0] += diffuse_reflectance[0]*TLC2[1][0];
		     shade[2*vtxno+1][1] += diffuse_reflectance[1]*TLC2[1][1];
		     shade[2*vtxno+1][2] += diffuse_reflectance[2]*TLC2[1][2];
		  } else {
		     shade[2*vtxno+0][0] += diffuse_intensity * TLC2[0][0] *
			   vtxclrs[vtxno][0];
		     shade[2*vtxno+0][1] += diffuse_intensity * TLC2[0][1] *
			   vtxclrs[vtxno][1];
		     shade[2*vtxno+0][2] += diffuse_intensity * TLC2[0][2] *
			   vtxclrs[vtxno][2];
		     shade[2*vtxno+1][0] += diffuse_intensity * TLC2[1][0] *
			   vtxclrs[vtxno][0];
		     shade[2*vtxno+1][1] += diffuse_intensity * TLC2[1][1] *
			   vtxclrs[vtxno][1];
		     shade[2*vtxno+1][2] += diffuse_intensity * TLC2[1][2] *
			   vtxclrs[vtxno][2];
		  }
	       } /* forall vertices */
	    } /* switch (interpolation_type) */
	 }
      } /* diffuse_light_switch */

      if (specular_light_switch) {
	 /*
	  * specular light shading
	  */
	 if (normal_type == ddd_sunview_cxp_drom_element_normals) {
	    shade_type = ddd_sunview_cxp_drom_element_shading;

	    for (elementno=0; elementno<trianglecount; elementno++) {
	       if (culling_switch && backfacing_flags[elementno])
		     continue;
		  
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		  light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		  light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		  light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		  light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		  light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[elementno][0]
			+ light_normal[1]*normals[elementno][1] +
			      light_normal[2]*normals[elementno][2];
		  if (dot_prod < 0.0) {
		     if (backfacing_flags[elementno])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }
		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       shade[elementno][0] += specular_reflectance[0] * TLC[0];
	       shade[elementno][1] += specular_reflectance[1] * TLC[1];
	       shade[elementno][2] += specular_reflectance[2] * TLC[2];
	    }
	 } else { /* VERTEX NORMALS */
	    switch (dde_sunview_cxp_drom_data.object_interpolation_type) {
	    case DcConstantShade:
	       shade_type = ddd_sunview_cxp_drom_element_shading;

	       for (elementno=0; elementno<trianglecount; elementno++) {
		  if (culling_switch && backfacing_flags[elementno])
			continue;
		  
		  color_component[0] = color_component[1] =
			color_component[2] = 0.;

		  for (vtxno=0; vtxno<3; vtxno++) {
		     TLC[0] = TLC[1] = TLC[2] = 0;

		     for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
			light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
			light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
			light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
			light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
			light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
			light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
			dot_prod = light_normal[0]*normals[vtxno][0]
			      + light_normal[1]*normals[vtxno][1] +
				    light_normal[2]*normals[vtxno][2];
			if (dot_prod < 0.0) {
			   if (backfacing_flags[elementno])
				 dot_prod *= -1.0;
			   else
				 dot_prod = 0.0;
			}
		  
			idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
			dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

			TLC[0] += dot_prod * light_color[0];
			TLC[1] += dot_prod * light_color[1];
			TLC[2] += dot_prod * light_color[2];
		     }

		     color_component[0] += specular_reflectance[0]*TLC[0];
		     color_component[1] += specular_reflectance[1]*TLC[1];
		     color_component[2] += specular_reflectance[2]*TLC[2];
		  }
		  shade[elementno][0] += color_component[0] / 3.;
		  shade[elementno][1] += color_component[1] / 3.;
		  shade[elementno][2] += color_component[2] / 3.;
	       } /* for all elements */
	       break;
	    case DcVertexShade:
	    case DcSurfaceShade:
	       shade_type = ddd_sunview_cxp_drom_vertex_shading;
	       for (vtxno=0; vtxno<vertexcount; vtxno++) {
		  
		  TLC2[0][0] = TLC2[0][1] = TLC2[0][2] = 0;
		  TLC2[1][0] = TLC2[1][1] = TLC2[1][2] = 0;

		  for (light=0; light<dde_sunview_cxp_drom_curlgtdat->count; light++){
		     light_normal[0] = dde_sunview_cxp_drom_curlgtdat->h[light][0];
		     light_normal[1] = dde_sunview_cxp_drom_curlgtdat->h[light][1];
		     light_normal[2] = dde_sunview_cxp_drom_curlgtdat->h[light][2];
		     light_color[0] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][0];
		     light_color[1] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][1];
		     light_color[2] = dde_sunview_cxp_drom_curlgtdat->scaledcolor[light][2];
      
		     dot_prod = light_normal[0]*normals[vtxno][0]
			   + light_normal[1]*normals[vtxno][1] +
				 light_normal[2]*normals[vtxno][2];
		     if (dot_prod < 0.0) {
			dot_prod *= -1.0;
			idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
			dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];
			TLC2[1][0] += dot_prod * light_color[0];
			TLC2[1][1] += dot_prod * light_color[1];
			TLC2[1][2] += dot_prod * light_color[2];
		     } else {
			idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
			dot_prod = dde_sunview_cxp_drom_data.specular_factor_table[idx];

			TLC2[0][0] += dot_prod * light_color[0];
			TLC2[0][1] += dot_prod * light_color[1];
			TLC2[0][2] += dot_prod * light_color[2];
		     }
		  }

		  shade[2*vtxno+0][0] += specular_reflectance[0]*TLC2[0][0];
		  shade[2*vtxno+0][1] += specular_reflectance[1]*TLC2[0][1];
		  shade[2*vtxno+0][2] += specular_reflectance[2]*TLC2[0][2];
		  shade[2*vtxno+1][0] += specular_reflectance[0]*TLC2[1][0];
		  shade[2*vtxno+1][1] += specular_reflectance[1]*TLC2[1][1];
		  shade[2*vtxno+1][2] += specular_reflectance[2]*TLC2[1][2];
	       }
	       break;
	    }
	 }
      }
   }

   /*
    * clip the values to the range 0 - 255
    */

   switch (shade_type) {
   case ddd_sunview_cxp_drom_element_shading:
      ddr_sunview_cxp_drom_shade_clip_color (trianglecount, culling_switch, 1);
      break;
   case ddd_sunview_cxp_drom_vertex_shading:
      if (normal_type == ddd_sunview_cxp_drom_vertex_normals) {
	 ddr_sunview_cxp_drom_shade_clip_color (2*vertexcount, DcFalse, 1);
      } else {
	 ddr_sunview_cxp_drom_shade_clip_color (vertexcount, DcFalse, 1);
      }
      break;
   }

   /*
    * Do pseudo color conversion
    */

   if (dde_sunview_cxp_drom_data.pseudocolor_switch) {
      switch (shade_type) {
      case ddd_sunview_cxp_drom_element_shading:
	 ddr_sunview_cxp_drom_shade_to_pseudo (trianglecount, culling_switch, 
				     dde_sunview_cxp_drom_curdevdat->shade_mode, 1);
	 break;
      case ddd_sunview_cxp_drom_vertex_shading:
	 if (normal_type == ddd_sunview_cxp_drom_vertex_normals) {
	    ddr_sunview_cxp_drom_shade_to_pseudo (2*vertexcount, DcFalse, 
					dde_sunview_cxp_drom_curdevdat->shade_mode, 
					1);
	 } else {
	    ddr_sunview_cxp_drom_shade_to_pseudo (vertexcount, DcFalse, 
					dde_sunview_cxp_drom_curdevdat->shade_mode,
					1);
	 }
	 break;
      }
   }
   if (shade_type == ddd_sunview_cxp_drom_element_shading)
	 *shades_per_element = 1;
   else {
      if (normal_type == ddd_sunview_cxp_drom_vertex_normals) 
	    *shades_per_element = 6;
      else
	    *shades_per_element = 3;
   }
   *backfacing = backfacing_flags;
}
/*
 ======================================================================
 */

/*
  ======================================================================
 */
ddr_sunview_cxp_drom_check_shade_buffers (backfacing, shading, pseudo)
     DtInt backfacing;
     DtInt shading;
     DtInt pseudo;
{
    if (dde_sunview_cxp_drom_data.backfacing_flags_count < backfacing) {
	if (dde_sunview_cxp_drom_data.backfacing_flags == NULL) {
	    dde_sunview_cxp_drom_data.backfacing_flags = (DtInt *)
		    DDspace_Allocate (backfacing*sizeof(DtInt));
	} else {
	    dde_sunview_cxp_drom_data.backfacing_flags = (DtInt *)
		    DDspace_Reallocate (dde_sunview_cxp_drom_data.backfacing_flags,
					backfacing*sizeof(DtInt));
	}
	dde_sunview_cxp_drom_data.backfacing_flags_count = backfacing;
    }

    /*
     * Make sure that there is enough space to hold all the shades for
     * the batch being processed.  If not then reallocate the space so
     * there is enough
     */

    if (dde_sunview_cxp_drom_data.colored_element_count < shading) {
	if (dde_sunview_cxp_drom_data.colored_elements == NULL) {
	    dde_sunview_cxp_drom_data.colored_elements = (DtRealTriple *)
		    DDspace_Allocate (shading*sizeof(DtRealTriple));
	} else {
	    dde_sunview_cxp_drom_data.colored_elements = (DtRealTriple *)
		    DDspace_Reallocate (dde_sunview_cxp_drom_data.colored_elements,
					shading*sizeof(DtRealTriple));
	}
	dde_sunview_cxp_drom_data.colored_element_count = shading;
    }

    /*
     * Make sure that there is enough space for the pseudo color indices
     * for all the vertices being processed.  If not the reallocate the
     * space so there is enough
     */

    if (dde_sunview_cxp_drom_data.color_indices_count < pseudo) {
	if (dde_sunview_cxp_drom_data.color_indices == NULL) {
	    dde_sunview_cxp_drom_data.color_indices = (DtInt *)
		    DDspace_Allocate (pseudo*sizeof(DtInt));
	} else {
	    dde_sunview_cxp_drom_data.color_indices = (DtInt *)
		    DDspace_Reallocate (dde_sunview_cxp_drom_data.color_indices,
					pseudo*sizeof(DtInt));
	}
	dde_sunview_cxp_drom_data.color_indices_count = pseudo;
    }
}
/*
  ======================================================================
 */
ddr_sunview_cxp_drom_get_normal_type (elenrms, vtxnrms, normal_type, normals)
     DtRealTriple *elenrms;
     DtRealTriple *vtxnrms;
     DtInt *normal_type;
     DtRealTriple **normals;
{
    if (dde_sunview_cxp_drom_data.draw_switch) {
	/* If the current surface shader is "constant", then we force 
	   draw mode by setting the vertex and element normal pointers
	   to NULL */

	*normal_type = ddd_sunview_cxp_drom_no_normals;
	*normals = NULL;
    } else {
	switch (dde_sunview_cxp_drom_data.object_interpolation_type) {
	case DcConstantShade:
	    /*  The user has requested a single shade per element.  If the
		object can provide element normals (ie. triangle meshes and
		triangle lists), then he will have provided them.  If element
		normals were not provided, then two cases can exist.  Either
		the object can supply vertex normals (ie. line lists with vertex
		normals) and it will have done so, or the object cannot provide
		vertex normals (ie. line lists and point lists without attached
		vertex normals).  If vertex normals have been provided, then we
		will use them, otherwise we assume that no light source shading
		is to be performed, even though it was requested by the user.
		In this case, it is also assumed that if the object contained an
		object implicit normal (ie. text), then the diffuse color global
		attribute was pushed with the light source shaded value for the
		entire object, so that non light source shading will result in
		the correct shade for the object.  */
	 
	    if (elenrms != NULL) {
		*normal_type = ddd_sunview_cxp_drom_element_normals;
		*normals = elenrms;
	    } else if (vtxnrms != NULL) {
		*normal_type = ddd_sunview_cxp_drom_vertex_normals;
		*normals = vtxnrms;
	    } else {
		*normal_type = ddd_sunview_cxp_drom_no_normals;
		*normals = NULL;
	    }
	    break;
	case DcVertexShade:
	case DcSurfaceShade:
	    /*  The user has requested a shade per vertex.  If the
		object can provide vertex normals then he will have provided
		them.  If vertex normals were not provided, then two cases can
		exist.  Either the object can supply element normals and it will
		have done so, or the object cannot provide element normals.
		If element normals have been provided, then we will use them,
		otherwise we assume that no light source shading is to be
		performed, even though it was requested by the user.
		In this case, it is also assumed that if the object contained an
		object implicit normal, then the diffuse color global
		attribute was pushed with the light source shaded value for the
		entire object, so that non light source shading will result in
		the correct shade for the object.  Note that if the user requested
		surface shading (one shade per pixel) it is ignored for this
		rendering style at this time, and is backed down to 
		vertex shading.  */

	    if (vtxnrms != NULL) {
		*normal_type = ddd_sunview_cxp_drom_vertex_normals;
		*normals = vtxnrms;
	    } else if (elenrms != NULL) {
		*normal_type = ddd_sunview_cxp_drom_element_normals;
		*normals = elenrms;
	    } else {
		*normal_type = ddd_sunview_cxp_drom_no_normals;
		*normals = NULL;
	    }
	    break;
	}
    }
}
/*
  ======================================================================
 */
ddr_sunview_cxp_drom_shade_clip_color (count, culling, shades_per_element)
     DtInt count;
     DtSwitch culling;
     DtInt shades_per_element;
{
    DtInt i;
    DtInt *backfacing = dde_sunview_cxp_drom_data.backfacing_flags;
    DtRealTriple *shade = dde_sunview_cxp_drom_data.colored_elements;

    for (i=0; i<count; i++) {
	if (culling && backfacing[i/shades_per_element])
		continue;
	shade[i][0] = (shade[i][0] < 0.) ? 0. :
	(shade[i][0] > 255.) ? 255. : shade[i][0];
	shade[i][1] = (shade[i][1] < 0.) ? 0. :
	(shade[i][1] > 255.) ? 255. : shade[i][1];
	shade[i][2] = (shade[i][2] < 0.) ? 0. :
	(shade[i][2] > 255.) ? 255. : shade[i][2];
    }
}
/*
  ======================================================================
 */
ddr_sunview_cxp_drom_shade_to_pseudo (count, culling, shade_mode, shades_per_element)
     DtInt count;
     DtSwitch culling;
     DtShadeMode shade_mode;
     DtInt shades_per_element;
{
    static DtReal cbtselpsweight[] = { 256./256., 32./256., 4./256. };
    static DtInt cbtselpsmask[] = { 0xe0, 0x1c, 0x03 };
    static DtReal cwtavgpsweight[] = { .30, .59, .11 };
    static DtReal cwtavgpslim[3];

    DtInt i;
    DtInt *backfacing = dde_sunview_cxp_drom_data.backfacing_flags;
    DtRealTriple *shade = dde_sunview_cxp_drom_data.colored_elements;
    DtInt *color_indices = dde_sunview_cxp_drom_data.color_indices;

    if (shade_mode == DcComponent) {
	for (i=0; i<count; i++) {
	    if (culling && backfacing[i/shades_per_element])
		    continue;
	    color_indices[i] = 
		    ((int)(cbtselpsweight[0]*shade[i][0]) & cbtselpsmask[0]) + 
			    ((int)(cbtselpsweight[1]*shade[i][1]) & cbtselpsmask[1]) +
				    ((int)(cbtselpsweight[2]*shade[i][2]) & cbtselpsmask[2]);
	    color_indices[i] = (color_indices[i] < 0.) ?
		    0. : (color_indices[i] > 255.) ? 255. : 
	    color_indices[i];
	}
    } else if (dde_sunview_cxp_drom_curdevdat->shade_mode == DcRange) {
	cwtavgpslim[0] = dde_sunview_cxp_drom_curdevdat->current_shade_range_min;
	cwtavgpslim[1] = dde_sunview_cxp_drom_curdevdat->current_shade_range_max;
	cwtavgpslim[2] = (cwtavgpslim[1]-cwtavgpslim[0])/255.;

	for (i=0; i<count; i++) {
	    if (culling && backfacing[i/shades_per_element])
		    continue;
	    color_indices[i] = cwtavgpslim[0] +
		    (cwtavgpsweight[0]*shade[i][0] +
		     cwtavgpsweight[1]*shade[i][1] +
		     cwtavgpsweight[2]*shade[i][2])*
			     cwtavgpslim[2];
	    if (color_indices[i] > cwtavgpslim[1])
		    color_indices[i] = cwtavgpslim[1];
	    color_indices[i] = (color_indices[i] < 0.) ?
		    0. : (color_indices[i] > 255.) ? 255. : 
	    color_indices[i];
	}
    }
}
