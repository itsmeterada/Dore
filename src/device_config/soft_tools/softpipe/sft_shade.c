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
	ddr_softpipe_drom_shade_elements
	ddr_softpipe_drom_shade_connected_line_list
	ddr_softpipe_drom_shade_triangle_list
	ddr_softpipe_drom_check_shade_buffers
	ddr_softpipe_drom_get_normal_type
	ddr_softpipe_drom_shade_clip_color
	ddr_softpipe_drom_check_backface_triangle

  ======================================================================
 */
#include "drom.h"

/*
 ======================================================================
 */
void ddr_softpipe_drom_shade_elements (
    DtInt elecount,
    DtRealTriple *elenrms,
    DtInt vertexcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt *shade_type)
{
   DtInt normal_type;
   DtRealTriple *shade;
   DtReal ambient_intensity;
   DtReal diffuse_intensity;
   DtRealTriple ambient_reflectance;
   DtRealTriple diffuse_reflectance;
   DtRealTriple specular_reflectance;
   DtInt diffuse_light_switch;
   DtInt specular_light_switch;
   DtRealTriple *normals;
   DtInt eleno;
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

#ifdef DEBUG
   printf("...processing dynout shade elements\n");
   fflush(stdout);
#endif

   vtx_per_element = vertexcount / elecount;

   ddr_softpipe_drom_check_shade_buffers(0, vertexcount, vertexcount);

   shade = dde_softpipe_drom_data.colored_elements;

   /*
    * scale the intensities and reflectances to the range of 0 - 255.
    * This is less computationally expensive to do the scaling now that
    * at the end when all the shades would have to be scaled.
    */

   ambient_intensity = dde_softpipe_drom_data.object_ambient_intensity;
   diffuse_intensity = dde_softpipe_drom_data.object_diffuse_intensity;

   if(vtxclrs == NULL) {
      ambient_reflectance[0] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[0]*
		  dde_softpipe_drom_data.ambient_reflectance[0];
      ambient_reflectance[1] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[1]*
		  dde_softpipe_drom_data.ambient_reflectance[1];
      ambient_reflectance[2] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[2]*
		  dde_softpipe_drom_data.ambient_reflectance[2];
   }	
   else {
      ambient_reflectance[0] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[0];
      ambient_reflectance[1] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[1];
      ambient_reflectance[2] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[2];
   }

   diffuse_reflectance[0] = 
	 dde_softpipe_drom_data.diffuse_reflectance[0];
   diffuse_reflectance[1] = 
	 dde_softpipe_drom_data.diffuse_reflectance[1];
   diffuse_reflectance[2] = 
	 dde_softpipe_drom_data.diffuse_reflectance[2];

   specular_reflectance[0] = 
	 dde_softpipe_drom_data.specular_reflectance[0];
   specular_reflectance[1] = 
	 dde_softpipe_drom_data.specular_reflectance[1];
   specular_reflectance[2] = 
	 dde_softpipe_drom_data.specular_reflectance[2];

   /*
    * determine what type of shading will be done.
    * light source shading requires at least a normal per element and
    * of course light sources.
    */

   diffuse_light_switch = (dde_softpipe_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_softpipe_drom_data.object_diffuse_switch;

   specular_light_switch = (dde_softpipe_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_softpipe_drom_data.object_specular_switch;

   ddr_softpipe_drom_get_normal_type(elenrms,vtxnrms,
				      &normal_type,&normals);

   if(normal_type == ddd_softpipe_drom_no_normals || 
      (!diffuse_light_switch && !specular_light_switch)) {
      /*
       * No light source shading can be done or was not requested.  
       * Use the diffuse color or diffuse intensity and the vertex colors
       */

      if(vtxclrs == NULL) {
	 *shade_type = ddd_softpipe_drom_element_shading;
	 for(eleno=0; eleno<elecount; eleno++) {
	    for(color=0; color<3; color++) {
	       shade[eleno][color] = diffuse_reflectance[color];
	    }
	 }
      }
      else {
	 *shade_type = ddd_softpipe_drom_vertex_shading;
	 for(vtxno=0; vtxno<vertexcount; vtxno++) {
	    for(color=0; color<3; color++) {
	       shade[vtxno][color] = diffuse_intensity
		     * vtxclrs[vtxno][color];
	    }
	 }
      }
   }
   else {
      if(dde_softpipe_drom_data.object_ambient_switch) {
	 if(vtxclrs == NULL) {
	    for(i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0];
	       shade[i][1] = ambient_reflectance[1];
	       shade[i][2] = ambient_reflectance[2];
	    }
	 }
	 else {
	    for(i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0]*vtxclrs[i][0];
	       shade[i][1] = ambient_reflectance[1]*vtxclrs[i][1];
	       shade[i][2] = ambient_reflectance[2]*vtxclrs[i][2];
	    }
	 }
      }
      else {
	 for(i=0; i<vertexcount; i++) {
	    shade[i][0] = 0.0;
	    shade[i][1] = 0.0;
	    shade[i][2] = 0.0;
	 }
      }

      if(diffuse_light_switch) {
	 /*
	  * diffuse light shading
	  */

	 if(normal_type == ddd_softpipe_drom_element_normals) {
	    if(vtxclrs != NULL) {
	       *shade_type = ddd_softpipe_drom_vertex_shading;
	    }
	    else {
	       *shade_type = ddd_softpipe_drom_element_shading;
	    }
	    for(eleno=0; eleno<elecount; eleno++) {
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for(light=0;light<dde_softpipe_drom_curlgtdat->count;
		   light++) {
		  light_normal[0] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][0];
		  light_normal[1] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][1];
		  light_normal[2] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][2];
		  light_color[0] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][0];
		  light_color[1] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][1];
		  light_color[2] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][2];

		  dot_prod = light_normal[0]*normals[eleno][0]
			+ light_normal[1]*normals[eleno][1]
			      + light_normal[2]*normals[eleno][2];
		  if(dot_prod < 0.0) dot_prod = 0.0;

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }
	       if(vtxclrs == NULL) {
		  shade[eleno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[eleno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[eleno][2] += diffuse_reflectance[2] * TLC[2];
	       }
	       else {
		  for(i=0; i<vtx_per_element; i++) {
		     vtxno = eleno*vtx_per_element + i;
		     shade[vtxno][0] += diffuse_intensity * TLC[0]
			   * vtxclrs[vtxno][0];
		     shade[vtxno][1] += diffuse_intensity * TLC[1]
			   * vtxclrs[vtxno][1];
		     shade[vtxno][2] += diffuse_intensity * TLC[2]
			   * vtxclrs[vtxno][2];
		  }
	       }
	    }
	 }
	 else {		/* VERTEX NORMALS */
	    *shade_type = ddd_softpipe_drom_vertex_shading;
	    for(vtxno=0; vtxno<vertexcount; vtxno++) {
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for(light=0;light<dde_softpipe_drom_curlgtdat->count;
		   light++) {
		  light_normal[0] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][0];
		  light_normal[1] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][1];
		  light_normal[2] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][2];
		  light_color[0] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][0];
		  light_color[1] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][1];
		  light_color[2] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][2];
 
		  dot_prod = light_normal[0]*normals[vtxno][0]
			+ light_normal[1]*normals[vtxno][1]
			      + light_normal[2]*normals[vtxno][2];
		  if(dot_prod < 0.0) dot_prod = 0.0;
		  
		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       if(vtxclrs == NULL) {
		  shade[vtxno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[vtxno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[vtxno][2] += diffuse_reflectance[2] * TLC[2];
	       }
	       else {
		  shade[vtxno][0] += diffuse_intensity * TLC[0]
			* vtxclrs[vtxno][0];
		  shade[vtxno][1] += diffuse_intensity * TLC[1]
			* vtxclrs[vtxno][1];
		  shade[vtxno][2] += diffuse_intensity * TLC[2]
			* vtxclrs[vtxno][2];
	       }
	    }
	 }
      }
      if(specular_light_switch) {
	 /*
	  * specular light shading
	  */
	 if(normal_type == ddd_softpipe_drom_element_normals) {
	    if(vtxclrs != NULL) {
	       *shade_type = ddd_softpipe_drom_vertex_shading;
	    }
	    else {
	       *shade_type = ddd_softpipe_drom_element_shading;
	    }
	    for(eleno=0; eleno<elecount; eleno++) {
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for(light=0;light<dde_softpipe_drom_curlgtdat->count;
		   light++) {
		  light_normal[0] = 
			dde_softpipe_drom_curlgtdat->h[light][0];
		  light_normal[1] = 
			dde_softpipe_drom_curlgtdat->h[light][1];
		  light_normal[2] = 
			dde_softpipe_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][0];
		  light_color[1] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][1];
		  light_color[2] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[eleno][0]
			+ light_normal[1]*normals[eleno][1]
			      + light_normal[2]*normals[eleno][2];
		  if(dot_prod < 0.0) dot_prod = 0.0;

		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_softpipe_drom_data.
			specular_factor_table[idx];

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       if(vtxclrs == NULL) {
		  shade[eleno][0] += specular_reflectance[0] * TLC[0];
		  shade[eleno][1] += specular_reflectance[1] * TLC[1];
		  shade[eleno][2] += specular_reflectance[2] * TLC[2];
	       }
	       else {
		  for(i=0; i<vtx_per_element; i++) {
		     vtxno = eleno*vtx_per_element + i;
		     shade[vtxno][0] 
			   += specular_reflectance[0] * TLC[0];
		     shade[vtxno][1] 
			   += specular_reflectance[1] * TLC[1];
		     shade[vtxno][2] 
			   += specular_reflectance[2] * TLC[2];
		  }
	       }
	    }
	 }
	 else {		/* VERTEX NORMALS */
	    *shade_type = ddd_softpipe_drom_vertex_shading;
	    for(vtxno=0; vtxno<vertexcount; vtxno++) {
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for(light=0;light<dde_softpipe_drom_curlgtdat->count;
		   light++) {
		  light_normal[0] = 
			dde_softpipe_drom_curlgtdat->h[light][0];
		  light_normal[1] = 
			dde_softpipe_drom_curlgtdat->h[light][1];
		  light_normal[2] = 
			dde_softpipe_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][0];
		  light_color[1] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][1];
		  light_color[2] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][2];
      
		  dot_prod = light_normal[0]*normals[vtxno][0] 
			+ light_normal[1]*normals[vtxno][1] 
			      + light_normal[2]*normals[vtxno][2];
		  if(dot_prod < 0.0) dot_prod = 0.0;
		  
		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_softpipe_drom_data.
			specular_factor_table[idx];

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


   if(dde_softpipe_drom_data.object_interpolation_type == DcConstantShade
      && *shade_type == ddd_softpipe_drom_vertex_shading) {
      /*
       * average the shade values of the vertices of the element to
       * give a shade per element
       */
      *shade_type = ddd_softpipe_drom_element_shading;

      for(i=0; i<elecount; i++) {
	 shade[i][0] = shade[i*vtx_per_element][0];
	 shade[i][1] = shade[i*vtx_per_element][1];
	 shade[i][2] = shade[i*vtx_per_element][2];
	 for(j=1; j<vtx_per_element; j++) {
	    shade[i][0] += shade[i*vtx_per_element+j][0];
	    shade[i][1] += shade[i*vtx_per_element+j][1];
	    shade[i][2] += shade[i*vtx_per_element+j][2];
	 }
	 shade[i][0] /= (DtReal)vtx_per_element;
	 shade[i][1] /= (DtReal)vtx_per_element;
	 shade[i][2] /= (DtReal)vtx_per_element;
      }
   }
   else if(dde_softpipe_drom_data.object_interpolation_type != DcConstantShade
	    && *shade_type == ddd_softpipe_drom_element_shading) {
      /*
       * only have one shade per element but the shading method is for 
       * one shade per vertex so duplicate the shades
       */

      *shade_type = ddd_softpipe_drom_vertex_shading;
      for(i=elecount-1; i>=0; i--) {
	 for(j=0; j<vtx_per_element; j++)
	       {
		  shade[vtx_per_element*i+j][0] = shade[i][0];
		  shade[vtx_per_element*i+j][1] = shade[i][1];
		  shade[vtx_per_element*i+j][2] = shade[i][2];
	       }
      }
   }

   /*
    * clip the values to the range 0 - 1.
    */

   switch(*shade_type) {
   case ddd_softpipe_drom_element_shading:
      ddr_softpipe_drom_shade_clip_color(elecount,DcOff,1);
      break;

   case ddd_softpipe_drom_vertex_shading:
      ddr_softpipe_drom_shade_clip_color(vertexcount,DcOff,1);
      break;
   }
}

/*
 ======================================================================
 */
void ddr_softpipe_drom_shade_connected_line_list (
    DtInt vertexcount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt *shade_type)
{
   DtInt elecount;
   DtInt normal_type;
   DtRealTriple *shade;
   DtReal ambient_intensity;
   DtReal diffuse_intensity;
   DtRealTriple ambient_reflectance;
   DtRealTriple diffuse_reflectance;
   DtRealTriple specular_reflectance;
   DtInt diffuse_light_switch;
   DtInt specular_light_switch;
   DtRealTriple *normals;
   DtInt eleno;
   DtInt vtxno;
   DtInt color;
   DtRealTriple TLC;
   DtReal light_normal[3];
   DtReal light_color[3];
   DtReal dot_prod;
   DtInt i;
   DtInt idx;
   DtInt light;
   DtRealTriple color_component;
   DtRealTriple save_component;

#ifdef DEBUG
   printf("...processing dynout shade connected line list\n");
   fflush(stdout);
#endif

   elecount = vertexcount - 1;

   /*
    * Make sure that there is enough space to hold all the shades for
    * the batch being processed.  If not then DDspace_Reallocateate the space so
    * there is enough
    */

   ddr_softpipe_drom_check_shade_buffers(0,vertexcount,vertexcount);

   shade = dde_softpipe_drom_data.colored_elements;

   /*
    * scale the intensities and reflectances to the range of 0 - 255.
    * This is less computationally expensive to do the scaling now that
    * at the end when all the shades would have to be scaled.
    */

   ambient_intensity = dde_softpipe_drom_data.object_ambient_intensity;
   diffuse_intensity = dde_softpipe_drom_data.object_diffuse_intensity;

   if(vtxclrs == NULL) {
      ambient_reflectance[0] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[0]*
		  dde_softpipe_drom_data.ambient_reflectance[0];
      ambient_reflectance[1] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[1]*
		  dde_softpipe_drom_data.ambient_reflectance[1];
      ambient_reflectance[2] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[2]*
		  dde_softpipe_drom_data.ambient_reflectance[2];
   }
   else {
      ambient_reflectance[0] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[0];
      ambient_reflectance[1] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[1];
      ambient_reflectance[2] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[2];
   }

   diffuse_reflectance[0] = 
	 dde_softpipe_drom_data.diffuse_reflectance[0];
   diffuse_reflectance[1] = 
	 dde_softpipe_drom_data.diffuse_reflectance[1];
   diffuse_reflectance[2] = 
	 dde_softpipe_drom_data.diffuse_reflectance[2];

   specular_reflectance[0] = 
	 dde_softpipe_drom_data.specular_reflectance[0];
   specular_reflectance[1] = 
	 dde_softpipe_drom_data.specular_reflectance[1];
   specular_reflectance[2] = 
	 dde_softpipe_drom_data.specular_reflectance[2];

   /*
    * determine what type of shading will be done.
    * light source shading requires at least a normal per element and
    * of course light sources.
    */

   diffuse_light_switch = (dde_softpipe_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_softpipe_drom_data.object_diffuse_switch;

   specular_light_switch = (dde_softpipe_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_softpipe_drom_data.object_specular_switch;

   ddr_softpipe_drom_get_normal_type(elenrms,vtxnrms,
				      &normal_type,&normals);

   if(normal_type == ddd_softpipe_drom_no_normals || 
      (!diffuse_light_switch && !specular_light_switch)) {
      /*
       * No light source shading can be done or was not requested.  
       * Use the diffuse color or diffuse intensity and the vertex colors
       */

      if(vtxclrs == NULL) {
	 *shade_type = ddd_softpipe_drom_element_shading;
	 for(eleno=0; eleno<elecount; eleno++) {
	    for(color=0; color<3; color++) {
	       shade[eleno][color] = diffuse_reflectance[color];
	    }
	 }
      }
      else {			/* vtxclrs != NULL */
	 switch(dde_softpipe_drom_data.object_interpolation_type) {
	 case DcConstantShade:
	    *shade_type = ddd_softpipe_drom_element_shading;
	    for(eleno=0; eleno<elecount; eleno++) {
	       for(color=0; color<3; color++) {
		  for(i=0; i<2; i++) {
		     shade[eleno][color] += diffuse_intensity 
			   * vtxclrs[eleno+i][color];
		  }
		  shade[eleno][color] /= 2.;
	       }
	    }
	    break;

	 case DcVertexShade:
	 case DcSurfaceShade:
	    *shade_type = ddd_softpipe_drom_vertex_shading;
	    for(vtxno=0; vtxno<vertexcount; vtxno++) {
	       for(color=0; color<3; color++) {
		  shade[vtxno][color] = diffuse_intensity 
			* vtxclrs[vtxno][color];
	       }
	    }
	    break;
	 }
      } 
   }
   else
	 {			/* Some form of light shading */
	    if(dde_softpipe_drom_data.object_ambient_switch) {
	       if(vtxclrs == NULL) {
		  for(i=0; i<vertexcount; i++) {
		     shade[i][0] = ambient_reflectance[0];
		     shade[i][1] = ambient_reflectance[1];
		     shade[i][2] = ambient_reflectance[2];
		  }
	       }
	       else {
		  for(i=0; i<vertexcount; i++) {
		     shade[i][0] = ambient_reflectance[0]*vtxclrs[i][0];
		     shade[i][1] = ambient_reflectance[1]*vtxclrs[i][1];
		     shade[i][2] = ambient_reflectance[2]*vtxclrs[i][2];
		  }
	       }
	    }
	    else {
	       for(i=0; i<vertexcount; i++) {
		  shade[i][0] = 0.0;
		  shade[i][1] = 0.0;
		  shade[i][2] = 0.0;
	       }
	    }

	    if(diffuse_light_switch) {
	       /*
		* diffuse light shading
		*/

	       if(normal_type == ddd_softpipe_drom_element_normals) {
		  *shade_type = ddd_softpipe_drom_element_shading;
	    
		  for(eleno=0; eleno<elecount; eleno++) {
		     TLC[0] = TLC[1] = TLC[2] = 0;

		     for(light=0; light<dde_softpipe_drom_curlgtdat->count;
			 light++) {
			light_normal[0] = dde_softpipe_drom_curlgtdat->
			      lcscosine[light][0];
			light_normal[1] = dde_softpipe_drom_curlgtdat->
			      lcscosine[light][1];
			light_normal[2] = dde_softpipe_drom_curlgtdat->
			      lcscosine[light][2];
			light_color[0] = dde_softpipe_drom_curlgtdat->
			      scaledcolor[light][0];
			light_color[1] = dde_softpipe_drom_curlgtdat->
			      scaledcolor[light][1];
			light_color[2] = dde_softpipe_drom_curlgtdat->
			      scaledcolor[light][2];

			dot_prod = light_normal[0]*normals[eleno][0] 
			      + light_normal[1]*normals[eleno][1] 
				    + light_normal[2]*normals[eleno][2];
			if(dot_prod < 0.0) dot_prod = 0.0;

			TLC[0] += dot_prod * light_color[0];
			TLC[1] += dot_prod * light_color[1];
			TLC[2] += dot_prod * light_color[2];
		     }
		     if(vtxclrs == NULL) {
			shade[eleno][0] += diffuse_reflectance[0] * TLC[0];
			shade[eleno][1] += diffuse_reflectance[1] * TLC[1];
			shade[eleno][2] += diffuse_reflectance[2] * TLC[2];
		     }
		     else {	/* vtxclrs != NULL */
			color_component[0] = color_component[1] 
			      = color_component[2] = 0.;
			for(i=0; i<2; i++) {
			   color_component[0] += diffuse_intensity * TLC[0]
				 * vtxclrs[eleno+i][0];
			   color_component[1] += diffuse_intensity * TLC[1]
				 * vtxclrs[eleno+i][1];
			   color_component[2] += diffuse_intensity * TLC[2]
				 * vtxclrs[eleno+i][2];
			}
			shade[eleno][0] += color_component[0] / 2.;
			shade[eleno][1] += color_component[1] / 2.;
			shade[eleno][2] += color_component[2] / 2.;
		     }
		  }		/* forall elements */
	       }
	       else {		/* VERTEX NORMALS */
		  switch(dde_softpipe_drom_data.object_interpolation_type) {
		  case DcConstantShade:
		     *shade_type = ddd_softpipe_drom_element_shading;

		     for(vtxno=0; vtxno<vertexcount; vtxno++) {
			TLC[0] = TLC[1] = TLC[2] = 0;
		     
			for(light=0;
			    light<dde_softpipe_drom_curlgtdat->count; 
			    light++) {
			   light_normal[0] = 
				 dde_softpipe_drom_curlgtdat->
				       lcscosine[light][0];
			   light_normal[1] = 
				 dde_softpipe_drom_curlgtdat->
				       lcscosine[light][1];
			   light_normal[2] = 
				 dde_softpipe_drom_curlgtdat->
				       lcscosine[light][2];
			   light_color[0] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][0];
			   light_color[1] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][1];
			   light_color[2] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][2];

			   dot_prod = light_normal[0]*normals[vtxno][0]
				 + light_normal[1]*normals[vtxno][1] 
				       + light_normal[2]*normals[vtxno][2];
			   if(dot_prod < 0.0) dot_prod = 0.0;

			   TLC[0] += dot_prod * light_color[0];
			   TLC[1] += dot_prod * light_color[1];
			   TLC[2] += dot_prod * light_color[2];
			}

			if(vtxclrs != NULL) {
			   color_component[0] = diffuse_intensity 
				 * TLC[0] * vtxclrs[vtxno][0];
			   color_component[1] = diffuse_intensity 
				 * TLC[1] * vtxclrs[vtxno][1];
			   color_component[2] = diffuse_intensity 
				 * TLC[2] * vtxclrs[vtxno][2];
			}
			else {
			   color_component[0] = 
				 diffuse_reflectance[0] * TLC[0];
			   color_component[1] = 
				 diffuse_reflectance[1] * TLC[1];
			   color_component[2] = 
				 diffuse_reflectance[2] * TLC[2];
			}
			if(vtxno > 0) {
			   shade[vtxno-1][0] += (color_component[0] 
						 + save_component[0]) / 2.;
			   shade[vtxno-1][1] += (color_component[1] 
						 + save_component[1]) / 2.;
			   shade[vtxno-1][2] += (color_component[2] 
						 + save_component[2]) / 2.;
			}
			save_component[0] = color_component[0];
			save_component[1] = color_component[1];
			save_component[2] = color_component[2];
		     }
		     break;

		  case DcVertexShade:
		  case DcSurfaceShade:
		     *shade_type = ddd_softpipe_drom_vertex_shading;
		     for(vtxno=0; vtxno<vertexcount; vtxno++)  {
			TLC[0] = TLC[1] = TLC[2] = 0;

			for(light=0; 
			    light<dde_softpipe_drom_curlgtdat->count;
			    light++) {
			   light_normal[0] = 
				 dde_softpipe_drom_curlgtdat->
				       lcscosine[light][0];
			   light_normal[1] = 
				 dde_softpipe_drom_curlgtdat->
				       lcscosine[light][1];
			   light_normal[2] = 
				 dde_softpipe_drom_curlgtdat->
				       lcscosine[light][2];
			   light_color[0] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][0];
			   light_color[1] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][1];
			   light_color[2] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][2];

			   dot_prod = light_normal[0]*normals[vtxno][0]
				 + light_normal[1]*normals[vtxno][1] 
				       + light_normal[2]*normals[vtxno][2];
			   if(dot_prod < 0.0) dot_prod = 0.0;

			   TLC[0] += dot_prod * light_color[0];
			   TLC[1] += dot_prod * light_color[1];
			   TLC[2] += dot_prod * light_color[2];
			}

			if(vtxclrs == NULL) {
			   shade[vtxno][0] += diffuse_reflectance[0] 
				 * TLC[0];
			   shade[vtxno][1] += diffuse_reflectance[1] 
				 * TLC[1];
			   shade[vtxno][2] += diffuse_reflectance[2] 
				 * TLC[2];
			}
			else {
			   shade[vtxno][0] += diffuse_intensity 
				 * TLC[0] * vtxclrs[vtxno][0];
			   shade[vtxno][1] += diffuse_intensity 
				 * TLC[1] * vtxclrs[vtxno][1];
			   shade[vtxno][2] += diffuse_intensity 
				 * TLC[2] * vtxclrs[vtxno][2];
			}
		     }		/* forall vertices */
		  }		/* switch (interpolation_type) */
	       }
	    }			/* diffuse_light_switch */

	    if(specular_light_switch) {
	       /*
		* specular light shading
		*/
	       if(normal_type == ddd_softpipe_drom_element_normals)  {
		  *shade_type = ddd_softpipe_drom_element_shading;

		  for(eleno=0; eleno<elecount; eleno++) {
		     TLC[0] = TLC[1] = TLC[2] = 0;

		     for(light=0; light<dde_softpipe_drom_curlgtdat->count;
			 light++) {
			light_normal[0] = 
			      dde_softpipe_drom_curlgtdat->h[light][0];
			light_normal[1] = 
			      dde_softpipe_drom_curlgtdat->h[light][1];
			light_normal[2] = 
			      dde_softpipe_drom_curlgtdat->h[light][2];
			light_color[0] = dde_softpipe_drom_curlgtdat->
			      scaledcolor[light][0];
			light_color[1] = dde_softpipe_drom_curlgtdat->
			      scaledcolor[light][1];
			light_color[2] = dde_softpipe_drom_curlgtdat->
			      scaledcolor[light][2];
      
			dot_prod = light_normal[0]*normals[eleno][0] 
			      + light_normal[1]*normals[eleno][1] 
				    + light_normal[2]*normals[eleno][2];
			if(dot_prod < 0.0) dot_prod = 0.0;
			idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
			dot_prod = dde_softpipe_drom_data.
			      specular_factor_table[idx];

			TLC[0] += dot_prod * light_color[0];
			TLC[1] += dot_prod * light_color[1];
			TLC[2] += dot_prod * light_color[2];
		     }

		     shade[eleno][0] += specular_reflectance[0] * TLC[0];
		     shade[eleno][1] += specular_reflectance[1] * TLC[1];
		     shade[eleno][2] += specular_reflectance[2] * TLC[2];
		  }
	       }
	       else {		/* VERTEX NORMALS */
		  switch(dde_softpipe_drom_data.object_interpolation_type) {
		  case DcConstantShade:
		     *shade_type = ddd_softpipe_drom_element_shading;

		     for(vtxno=0; vtxno<vertexcount; vtxno++)  {
			TLC[0] = TLC[1] = TLC[2] = 0;

			for(light=0;
			    light<dde_softpipe_drom_curlgtdat->count;
			    light++) {
			   light_normal[0] =
				 dde_softpipe_drom_curlgtdat->h[light][0];
			   light_normal[1] = 
				 dde_softpipe_drom_curlgtdat->h[light][1];
			   light_normal[2] = 
				 dde_softpipe_drom_curlgtdat->h[light][2];
			   light_color[0] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][0];
			   light_color[1] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][1];
			   light_color[2] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][2];

			   dot_prod = light_normal[0]*normals[vtxno][0]
				 + light_normal[1]*normals[vtxno][1] 
				       + light_normal[2]*normals[vtxno][2];
			   if(dot_prod < 0.0) dot_prod = 0.0;

			   idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx; 
			   dot_prod = dde_softpipe_drom_data.
				 specular_factor_table[idx];

			   TLC[0] += dot_prod * light_color[0];
			   TLC[1] += dot_prod * light_color[1];
			   TLC[2] += dot_prod * light_color[2];
			}

			color_component[0] = 
			      specular_reflectance[0]*TLC[0];
			color_component[1] = 
			      specular_reflectance[1]*TLC[1];
			color_component[2] = 
			      specular_reflectance[2]*TLC[2];

			if(vtxno > 0)  {
			   shade[vtxno-1][0] += (color_component[0] 
						 + save_component[0] ) / 2.;
			   shade[vtxno-1][1] += (color_component[1] 
						 + save_component[1] ) / 2.;
			   shade[vtxno-1][2] += (color_component[2] 
						 + save_component[2] ) / 2.;
			}
			save_component[0] = color_component[0];
			save_component[1] = color_component[1];
			save_component[2] = color_component[2];
		     }
		     break;

		  case DcVertexShade:
		  case DcSurfaceShade:
		     *shade_type = ddd_softpipe_drom_vertex_shading;
		     for(vtxno=0; vtxno<vertexcount; vtxno++)  {
			TLC[0] = TLC[1] = TLC[2] = 0;

			for(light=0;
			    light<dde_softpipe_drom_curlgtdat->count;
			    light++) {
			   light_normal[0] = 
				 dde_softpipe_drom_curlgtdat->h[light][0];
			   light_normal[1] = 
				 dde_softpipe_drom_curlgtdat->h[light][1];
			   light_normal[2] = 
				 dde_softpipe_drom_curlgtdat->h[light][2];
			   light_color[0] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][0];
			   light_color[1] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][1];
			   light_color[2] = 
				 dde_softpipe_drom_curlgtdat->
				       scaledcolor[light][2];
 
			   dot_prod = light_normal[0]*normals[vtxno][0]
				 + light_normal[1]*normals[vtxno][1] 
				       + light_normal[2]*normals[vtxno][2];
			   if(dot_prod < 0.0) dot_prod = 0.0;

			   idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx; 
			   dot_prod = dde_softpipe_drom_data.
				 specular_factor_table[idx];

			   TLC[0] += dot_prod * light_color[0];
			   TLC[1] += dot_prod * light_color[1];
			   TLC[2] += dot_prod * light_color[2];
			}

			shade[vtxno][0] += specular_reflectance[0]
			      * TLC[0];
			shade[vtxno][1] += specular_reflectance[1]
			      * TLC[1];
			shade[vtxno][2] += specular_reflectance[2]
			      * TLC[2];
		     }
		     break;
		  }
	       }
	    }
	 }

   /*
    * clip the values to the range 0 - 1.
    */

   switch(*shade_type) {
   case ddd_softpipe_drom_element_shading:
      ddr_softpipe_drom_shade_clip_color(elecount,DcOff,1);
      break;

   case ddd_softpipe_drom_vertex_shading:
      ddr_softpipe_drom_shade_clip_color(vertexcount,DcOff,1);
      break;
   }
}
/*
 ======================================================================
 */
void ddr_softpipe_drom_shade_triangle_list (
    DtInt elecount,
    DtRealTriple *elenrms,
    DtInt vertexcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt *shade_type)
{
   DtInt normal_type;
   DtRealTriple *shade;
   DtInt *backfacing_flags;
   DtReal ambient_intensity;
   DtReal diffuse_intensity;
   DtRealTriple ambient_reflectance;
   DtRealTriple diffuse_reflectance;
   DtRealTriple specular_reflectance;
   DtInt diffuse_light_switch;
   DtInt specular_light_switch;
   DtInt culling_switch;
   DtRealTriple *normals;
   DtInt eleno;
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

#ifdef DEBUG
   printf("...processing dynout shade triangle list\n");
   printf("object_interp_type = %d\n",
	  dde_softpipe_drom_data.object_interpolation_type);
   fflush(stdout);
#endif

   vtx_per_element = vertexcount / elecount;

   ddr_softpipe_drom_check_shade_buffers(vertexcount,vertexcount,
					  vertexcount);

   backfacing_flags = dde_softpipe_drom_data.backfacing_flags;
   shade = dde_softpipe_drom_data.colored_elements;

   ambient_intensity = dde_softpipe_drom_data.object_ambient_intensity;
   diffuse_intensity = dde_softpipe_drom_data.object_diffuse_intensity;

   if(vtxclrs == NULL) {
      ambient_reflectance[0] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[0]*
		  dde_softpipe_drom_data.ambient_reflectance[0];
      ambient_reflectance[1] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[1]*
		  dde_softpipe_drom_data.ambient_reflectance[1];
      ambient_reflectance[2] = 
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[2]*
		  dde_softpipe_drom_data.ambient_reflectance[2];
   }
   else {
      ambient_reflectance[0] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[0];
      ambient_reflectance[1] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[1];
      ambient_reflectance[2] = ambient_intensity *
	    dde_softpipe_drom_curlgtdat->ambient_reflectance[2];
   }

#ifdef DEBUG
   printf("ambient_intensity = %lf\n",ambient_intensity);
   printf("ambient reflectance = (%lf,%lf,%lf)\n",ambient_reflectance[0],
	  ambient_reflectance[1],ambient_reflectance[2]);
#endif

   diffuse_reflectance[0] = 
	 dde_softpipe_drom_data.diffuse_reflectance[0];
   diffuse_reflectance[1] = 
	 dde_softpipe_drom_data.diffuse_reflectance[1];
   diffuse_reflectance[2] = 
	 dde_softpipe_drom_data.diffuse_reflectance[2];

   specular_reflectance[0] = 
	 dde_softpipe_drom_data.specular_reflectance[0];
   specular_reflectance[1] = 
	 dde_softpipe_drom_data.specular_reflectance[1];
   specular_reflectance[2] = 
	 dde_softpipe_drom_data.specular_reflectance[2];
   /*
    * determine what type of shading will be done.
    * light source shading requires at least a normal per element and
    * of course light sources.
    */

   diffuse_light_switch = (dde_softpipe_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_softpipe_drom_data.object_diffuse_switch;

   specular_light_switch = (dde_softpipe_drom_curlgtdat->count != 0) &&
	 ((elenrms != NULL) || (vtxnrms != NULL)) &&
	       dde_softpipe_drom_data.object_specular_switch;
   culling_switch = (dde_softpipe_drom_data.object_cull_switch &&
		     dde_softpipe_drom_data.object_cullable_switch);

   ddr_softpipe_drom_get_normal_type(elenrms,vtxnrms,
				      &normal_type,&normals);
#ifdef DEBUG
   printf("normal type: %d\n",normal_type);
#endif

   /*
    * determine if triangles are backfacing
    */

   for(i=0; i<elecount; i++) {
      backfacing_flags[i] = 
	 ddr_softpipe_drom_check_backface_triangle (vtxlocs + (i*3));
   }

   if(normal_type == ddd_softpipe_drom_no_normals || 
      (!diffuse_light_switch && !specular_light_switch)) {
#ifdef DEBUG
      printf("No_Normal: No_LightSwitch: ");
#endif

      /*
       * No light source shading can be done or was not requested.  
       * Use the diffuse color or diffuse intensity and the vertex colors
       */

      if(vtxclrs == NULL) {
#ifdef DEBUG
	 printf("No_VertexColor -- > Element Shading\n");
#endif
	 *shade_type = ddd_softpipe_drom_element_shading;
	 for(eleno=0; eleno<elecount; eleno++) {
	    for(color=0; color<3; color++)
		  {
		     shade[eleno][color] = diffuse_reflectance[color];
		  }
	 }
      }
      else {
#ifdef DEBUG
	 printf("VertexColor --> Vertex Shading\n");
#endif
	 *shade_type = ddd_softpipe_drom_vertex_shading;
	 for(vtxno=0; vtxno<vertexcount; vtxno++) {
	    for(color=0; color<3; color++) {
	       shade[vtxno][color] = diffuse_intensity
		     *vtxclrs[vtxno][color];
	    }
	 }
      }
   }
   else {
#ifdef DEBUG
      printf("Normal || LightSwitch: ");
#endif
      if(dde_softpipe_drom_data.object_ambient_switch) {
#ifdef DEBUG
	 printf("ambient switch on\n");
#endif
	 if(vtxclrs == NULL) {
	    for(i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0];
	       shade[i][1] = ambient_reflectance[1];
	       shade[i][2] = ambient_reflectance[2];
	    }
	 }
	 else {
	    for(i=0; i<vertexcount; i++) {
	       shade[i][0] = ambient_reflectance[0]*vtxclrs[i][0];
	       shade[i][1] = ambient_reflectance[1]*vtxclrs[i][1];
	       shade[i][2] = ambient_reflectance[2]*vtxclrs[i][2];

	    }
	 }
      }
      else {
	 for(i=0; i<vertexcount; i++) {
	    shade[i][0] = 0.0;
	    shade[i][1] = 0.0;
	    shade[i][2] = 0.0;
	 }
      }

      if(diffuse_light_switch) {
#ifdef DEBUG
	 printf("DiffuseLightSwitch: ");
#endif
	 /*
	  * diffuse light shading
	  */

	 if(normal_type == ddd_softpipe_drom_element_normals) {
#ifdef DEBUG
	    printf("ElementNormal: ");
#endif
	    if(vtxclrs != NULL) {
#ifdef DEBUG
	       printf("VertexColor --> Vertex Shading\n");
#endif
	       *shade_type = ddd_softpipe_drom_vertex_shading;
	    }
	    else {
#ifdef DEBUG
	       printf("No_VertexColor --> Element Shading\n");
#endif
	       *shade_type = ddd_softpipe_drom_element_shading;
	    }
	    for(eleno=0; eleno<elecount; eleno++) {
	       if(culling_switch && backfacing_flags[eleno])
		     continue;
		 
	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for(light=0;light<dde_softpipe_drom_curlgtdat->count;
		   light++) {
		  light_normal[0] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][0];
		  light_normal[1] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][1];
		  light_normal[2] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][2];
		  light_color[0] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][0];
		  light_color[1] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][1];
		  light_color[2] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][2];
		  dot_prod = light_normal[0]*normals[eleno][0]
			+light_normal[1]*normals[eleno][1]
			      +light_normal[2]*normals[eleno][2];
		  if(dot_prod < 0.0) {
		     if(backfacing_flags[eleno])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }
		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }
	       if(vtxclrs == NULL) {
		  shade[eleno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[eleno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[eleno][2] += diffuse_reflectance[2] * TLC[2];
	       }
	       else {
		  for(i=0; i<vtx_per_element; i++) {
		     vtxno = eleno*vtx_per_element + i;
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
	 else {		/* VERTEX NORMALS */
#ifdef DEBUG
	    printf("VertexNormal --> Vertex Shading");
#endif
	    *shade_type = ddd_softpipe_drom_vertex_shading;
	    for(vtxno=0; vtxno<vertexcount; vtxno++) {
	       if(culling_switch && 
		  backfacing_flags[vtxno/vtx_per_element])
		     continue;

	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for(light=0;light<dde_softpipe_drom_curlgtdat->count;
		   light++) {
		  light_normal[0] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][0];
		  light_normal[1] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][1];
		  light_normal[2] = dde_softpipe_drom_curlgtdat->
			lcscosine[light][2];
		  light_color[0] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][0];
		  light_color[1] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][1];
		  light_color[2] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][2];

		  dot_prod = light_normal[0]*normals[vtxno][0] +
			light_normal[1]*normals[vtxno][1] +
			      light_normal[2]*normals[vtxno][2];
		  if(dot_prod < 0.0) {
		     if(backfacing_flags[vtxno / vtx_per_element])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }

		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       if(vtxclrs == NULL) {
		  shade[vtxno][0] += diffuse_reflectance[0] * TLC[0];
		  shade[vtxno][1] += diffuse_reflectance[1] * TLC[1];
		  shade[vtxno][2] += diffuse_reflectance[2] * TLC[2];
	       }
	       else {
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
      if(specular_light_switch) {
#ifdef DEBUG
	 printf("SpecularLightSwitch:");
#endif
	 /*
	  * specular light shading
	  */
	 if(normal_type == ddd_softpipe_drom_element_normals) {
#ifdef DEBUG
	    printf("ElementNormal: ");
#endif
	    if(vtxclrs != NULL) {
#ifdef DEBUG
	       printf("VertexColor --> Vertex Shading\n");
#endif
	       *shade_type = ddd_softpipe_drom_vertex_shading;
	    }
	    else {
#ifdef DEBUG
	       printf("No_VertexColor --> Element Shading\n");
#endif
	       *shade_type = ddd_softpipe_drom_element_shading;
	    }
	    for(eleno=0; eleno<elecount; eleno++) {
	       if(culling_switch && backfacing_flags[eleno])
		     continue;

	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for(light=0;light<dde_softpipe_drom_curlgtdat->count;
		   light++) {
		  light_normal[0] = 
			dde_softpipe_drom_curlgtdat->h[light][0];
		  light_normal[1] = 
			dde_softpipe_drom_curlgtdat->h[light][1];
		  light_normal[2] = 
			dde_softpipe_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][0];
		  light_color[1] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][1];
		  light_color[2] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][2];

		  dot_prod = light_normal[0]*normals[eleno][0]
			+ light_normal[1]*normals[eleno][1]
			      + light_normal[2]*normals[eleno][2];
		  if(dot_prod < 0.0) {
		     if(backfacing_flags[eleno])
			   dot_prod *= -1.0;
		     else
			   dot_prod = 0.0;
		  }

		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_softpipe_drom_data.
			specular_factor_table[idx];
		  TLC[0] += dot_prod * light_color[0];
		  TLC[1] += dot_prod * light_color[1];
		  TLC[2] += dot_prod * light_color[2];
	       }

	       if(vtxclrs == NULL) {
		  shade[eleno][0] += specular_reflectance[0] * TLC[0];
		  shade[eleno][1] += specular_reflectance[1] * TLC[1];
		  shade[eleno][2] += specular_reflectance[2] * TLC[2];
	       }
	       else {
		  for(i=0; i<vtx_per_element; i++) {
		     vtxno = eleno*vtx_per_element + i;
		     shade[vtxno][0] += 
			   specular_reflectance[0] * TLC[0];
		     shade[vtxno][1] += 
			   specular_reflectance[1] * TLC[1];
		     shade[vtxno][2] += 
			   specular_reflectance[2] * TLC[2];
		  }
	       }
	    }
	 }
	 else {		/* VERTEX NORMALS */
#ifdef DEBUG
	    printf("VertexNormal --> Vertex Shading\n");
#endif
	    *shade_type = ddd_softpipe_drom_vertex_shading;
	    for(vtxno=0; vtxno<vertexcount; vtxno++) {
	       if(culling_switch
		  && backfacing_flags[vtxno/vtx_per_element])
		     continue;

	       TLC[0] = TLC[1] = TLC[2] = 0;

	       for(light=0;light<dde_softpipe_drom_curlgtdat->count;
		   light++) {
		  light_normal[0] = 
			dde_softpipe_drom_curlgtdat->h[light][0];
		  light_normal[1] = 
			dde_softpipe_drom_curlgtdat->h[light][1];
		  light_normal[2] = 
			dde_softpipe_drom_curlgtdat->h[light][2];
		  light_color[0] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][0];
		  light_color[1] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][1];
		  light_color[2] = dde_softpipe_drom_curlgtdat->
			scaledcolor[light][2];
 
		  dot_prod = light_normal[0]*normals[vtxno][0]
			+ light_normal[1]*normals[vtxno][1] 
			      + light_normal[2]*normals[vtxno][2];
		  if(dot_prod < 0.0)
			{
			   if(backfacing_flags[vtxno / vtx_per_element])
				 dot_prod *= -1.0;
			   else
				 dot_prod = 0.0;
			}
		  idx = (idx = (int)(dot_prod*255)) > 255 ? 255 : idx;
		  dot_prod = dde_softpipe_drom_data.
			specular_factor_table[idx];

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

   if(dde_softpipe_drom_data.object_interpolation_type == DcConstantShade
      && *shade_type == ddd_softpipe_drom_vertex_shading) {
#ifdef DEBUG
      printf("DcConstantShade && VertexShade --> Element Shading\n");
#endif
      /*
       * average the shade values of the vertices of the element to
       * give a shade per element
       */
      *shade_type = ddd_softpipe_drom_element_shading;

      for(i=0; i<elecount; i++) {
	 if(culling_switch && backfacing_flags[i])
	       continue;
	 shade[i][0] = shade[i*vtx_per_element][0];
	 shade[i][1] = shade[i*vtx_per_element][1];
	 shade[i][2] = shade[i*vtx_per_element][2];
	 for(j=1; j<vtx_per_element; j++) {
	    shade[i][0] += shade[i*vtx_per_element+j][0];
	    shade[i][1] += shade[i*vtx_per_element+j][1];
	    shade[i][2] += shade[i*vtx_per_element+j][2];
	 }
	 shade[i][0] /= (DtReal)vtx_per_element;
	 shade[i][1] /= (DtReal)vtx_per_element;
	 shade[i][2] /= (DtReal)vtx_per_element;
      }
   }
   else if(dde_softpipe_drom_data.object_interpolation_type != 
	   DcConstantShade 
	   && *shade_type == ddd_softpipe_drom_element_shading) {
#ifdef DEBUG
      printf("DcVertexShade && VertexShade --> Vertex Shading\n");
#endif
      /*
       * only have one shade per element but the shading method is for 
       * one shade per vertex so duplicate the shades
       */

      *shade_type = ddd_softpipe_drom_vertex_shading;
      for(i=elecount-1; i>=0; i--) {
	 if(culling_switch && backfacing_flags[i])
	       continue;
	 for(j=0; j<vtx_per_element; j++) {
	    shade[vtx_per_element*i+j][0] = shade[i][0];
	    shade[vtx_per_element*i+j][1] = shade[i][1];
	    shade[vtx_per_element*i+j][2] = shade[i][2];
	 }
      }
   }

   /*
    * clip the values to the range 0 - 1.
    */

   switch(*shade_type) {
   case ddd_softpipe_drom_element_shading:
#ifdef DEBUG
      printf("final shade_type: Element Shading\n");
#endif
      ddr_softpipe_drom_shade_clip_color(elecount,culling_switch,1);
      break;

   case ddd_softpipe_drom_vertex_shading:
#ifdef DEBUG
      printf("final shade_type: Vertex Shading\n");
#endif
      ddr_softpipe_drom_shade_clip_color(vertexcount,
					  culling_switch, vtx_per_element);
      break;
   }
}

/*
 ======================================================================
 */
void ddr_softpipe_drom_check_shade_buffers (
    DtInt backfacing,
    DtInt shading,
    DtInt pseudo)
{
#ifdef DEBUG
   printf("...processing dynout check shade buffers\n");
   fflush(stdout);
#endif

   if(dde_softpipe_drom_data.backfacing_flags_count < backfacing) {
      if(dde_softpipe_drom_data.backfacing_flags == NULL) {
	 dde_softpipe_drom_data.backfacing_flags = 
	       (DtInt *)DDspace_Allocate(backfacing*sizeof(DtInt));
      } 
      else  {
	 dde_softpipe_drom_data.backfacing_flags = 
	       (DtInt *)DDspace_Reallocate(
				dde_softpipe_drom_data.backfacing_flags,
				backfacing*sizeof(DtInt));
      }
      dde_softpipe_drom_data.backfacing_flags_count = backfacing;
   }

   /*
    * Make sure that there is enough space to hold all the shades for
    * the batch being processed.  If not then DDspace_Reallocateate the space so
    * there is enough
    */

   if(dde_softpipe_drom_data.colored_element_count < shading) {
      if(dde_softpipe_drom_data.colored_elements == NULL)  {
	 dde_softpipe_drom_data.colored_elements = 
	       (DtRealTriple *)DDspace_Allocate(
				      shading*sizeof(DtRealTriple));
      }
      else {
	 dde_softpipe_drom_data.colored_elements = 
	       (DtRealTriple *)DDspace_Reallocate(
				       dde_softpipe_drom_data.colored_elements,
				       shading*sizeof(DtRealTriple));
      }
      dde_softpipe_drom_data.colored_element_count = shading;
   }

   /*
    * Make sure that there is enough space for the pseudo color indices
    * for all the vertices being processed.  If not the DDspace_Reallocateate the
    * space so there is enough
    */

   if(dde_softpipe_drom_data.color_indices_count < pseudo) {
      if(dde_softpipe_drom_data.color_indices == NULL) {
	 dde_softpipe_drom_data.color_indices = (DtInt *)
	       DDspace_Allocate(pseudo*sizeof(DtInt));
      }
      else {
	 dde_softpipe_drom_data.color_indices = (DtInt *)
	       DDspace_Reallocate(dde_softpipe_drom_data.color_indices,
		       pseudo*sizeof(DtInt));
      }
      dde_softpipe_drom_data.color_indices_count = pseudo;
   }
}
/*
  ======================================================================
 */
void ddr_softpipe_drom_get_normal_type (
    DtRealTriple *elenrms,
    DtRealTriple *vtxnrms,
    DtInt *normal_type,
    DtRealTriple **normals)
{
#ifdef DEBUG
   printf("...processing dynout get normal type\n");
   fflush(stdout);
#endif

   if(dde_softpipe_drom_data.draw_switch) {
      /* If the current surface shader is "constant", then we force 
	 draw mode by setting the vertex and element normal pointers
	 to NULL */

      *normal_type = ddd_softpipe_drom_no_normals;
      *normals = NULL;
   }
   else {
      switch(dde_softpipe_drom_data.object_interpolation_type) {
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
	 
	 if(elenrms != NULL) {
	    *normal_type = 
		  ddd_softpipe_drom_element_normals;
	    *normals = elenrms;
	 }
	 else if(vtxnrms != NULL) {
	    *normal_type = 
		  ddd_softpipe_drom_vertex_normals;
	    *normals = vtxnrms;
	 }
	 else {
	    *normal_type = ddd_softpipe_drom_no_normals;
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

	 if(vtxnrms != NULL) {
	    *normal_type = 
		  ddd_softpipe_drom_vertex_normals;
	    *normals = vtxnrms;
	 }
	 else if(elenrms != NULL) {
	    *normal_type = 
		  ddd_softpipe_drom_element_normals;
	    *normals = elenrms;
	 }
	 else {
	    *normal_type = ddd_softpipe_drom_no_normals;
	    *normals = NULL;
	 }
	 break;
      }
   }
}

/*
  ======================================================================
 */
void ddr_softpipe_drom_shade_clip_color (
    DtInt count,
    DtSwitch culling,
    DtInt shades_per_element)
{
   DtInt i;
   DtInt *backfacing = dde_softpipe_drom_data.backfacing_flags;
   DtRealTriple *shade = dde_softpipe_drom_data.colored_elements;

#ifdef DEBUG
   printf("... processing shade clip color ...\n");
   fflush(stdout);
#endif

   for(i=0; i<count; i++) {
      if(culling && backfacing[i/shades_per_element])
	    continue;
      shade[i][0] = (shade[i][0] < 0.) ? 0. :
      (shade[i][0] > 1.) ? 1. : shade[i][0];
      shade[i][1] = (shade[i][1] < 0.) ? 0. :
      (shade[i][1] > 1.) ? 1. : shade[i][1];
      shade[i][2] = (shade[i][2] < 0.) ? 0. :
      (shade[i][2] > 1.) ? 1. : shade[i][2];
   }
}
/*
 ======================================================================
 */
DtInt ddr_softpipe_drom_check_backface_triangle (
    DtRealTriple vtxptr[3])
{
   DtInt backface_flag;
   DtReal x1, x2, x3, y1, y2, y3;
   /*
     #ifdef DEBUG
     printf("...processing dynout check backface triangle ");
     fflush(stdout);
     #endif
     */
   x1 = vtxptr[0][0];
   y1 = vtxptr[0][1];

   x2 = vtxptr[1][0];
   y2 = vtxptr[1][1];

   x3 = vtxptr[2][0];
   y3 = vtxptr[2][1];

   /* Check the z-element of the triangle surface normal
      that is crossproduct between vector (V2-V1) and vector (V3-V1). */

   if(((x3-x1)*(y2-y1)) > ((x2-x1)*(y3-y1))) {
      backface_flag = 1;	/* backfacing */
   }
   else {
      backface_flag = 0;	/* frontfacing */
   }
   return(backface_flag);
}
