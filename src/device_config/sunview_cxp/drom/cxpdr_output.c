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
	int ddr_sunview_cxp_drom_render_point_list (colormodel,bndboxpts,pointcount,vtxlocs,vtxnrms, vtxclrs)
	int ddr_sunview_cxp_drom_render_line_list (colormodel,bndboxpts,linecount,elenrms,vtxlocs,vtxnrms, vtxclrs)
	int ddr_sunview_cxp_drom_render_connected_line_list (colormodel,bndboxpts,vertexcount,elenrms,vtxlocs, vtxnrms,vtxclrs)
	int ddr_sunview_cxp_drom_render_triangle_list (colormodel,bndboxpts,trianglecount,elenrms,vtxlocs, vtxnrms,vtxclrs)
	int ddr_sunview_cxp_drom_render_triangle_mesh (colormodel,bndboxpts,trianglecount,elenrms,vertexcount, vtxlocs,vtxnrms,vtxclrs,vertexlist, compiledvertexlist)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_render_point_list(colormodel,bndboxpts,pointcount,
				       vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt pointcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
   extern GP_data_ptr dde_sunview_cxp_current_gp;
   DtFlag zclipflag;
   int i, j;
   DtRealTriple *loc;
   GP_color clist[3];
   GP_point_3D vlist[3];
   DtRealTriple *colors;

   ddr_sunview_cxp_drom_shade_elements (pointcount, (DtRealTriple *)0, pointcount,
					vtxlocs, vtxnrms, vtxclrs);

   GP_draw_3D_point_list (dde_sunview_cxp_current_gp, pointcount, vtxlocs,
			  dde_sunview_cxp_drom_data.color_indices, 
			  dde_sunview_cxp_drom_data.object_representation_type,
			  dde_sunview_cxp_drom_data.object_interpolation_type);
   
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_render_line_list(colormodel,bndboxpts,linecount,
				      elenrms,vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt linecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
   extern GP_data_ptr dde_sunview_cxp_current_gp;

   ddr_sunview_cxp_drom_shade_elements (linecount, elenrms, linecount*2,
			       vtxlocs, vtxnrms, vtxclrs);

   GP_draw_3D_line_list (dde_sunview_cxp_current_gp, linecount, vtxlocs,
			 dde_sunview_cxp_drom_data.color_indices, 
			 dde_sunview_cxp_drom_data.object_representation_type,
			 dde_sunview_cxp_drom_data.object_interpolation_type);
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_render_connected_line_list(colormodel,bndboxpts,
						vertexcount,elenrms,vtxlocs,
						vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt vertexcount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
   extern GP_data_ptr dde_sunview_cxp_current_gp;
   DtFlag zclipflag;
   int i, j;
   DtRealTriple *loc;
   GP_color clist[3];
   GP_point_3D vlist[3];
   DtRealTriple *colors;
   DtInt shades_per_element;

   ddr_sunview_cxp_drom_shade_connected_line_list (vertexcount, elenrms, vtxlocs,
					  vtxnrms, vtxclrs, 
					  &shades_per_element);

   if (shades_per_element == 1) {
      GP_draw_3D_connline_list (dde_sunview_cxp_current_gp, 
				vertexcount, vtxlocs, 
				dde_sunview_cxp_drom_data.color_indices,
				dde_sunview_cxp_drom_data.object_representation_type,
				DcConstantShade);
   } else {
      GP_draw_3D_connline_list (dde_sunview_cxp_current_gp, 
				vertexcount, vtxlocs, 
				dde_sunview_cxp_drom_data.color_indices,
				dde_sunview_cxp_drom_data.object_representation_type,
				DcVertexShade);
   }
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_render_triangle_list(colormodel,bndboxpts,trianglecount,
					  elenrms,vtxlocs, vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{
   extern GP_data_ptr dde_sunview_cxp_current_gp;
   DtFlag zclipflag;
   int i, j;
   DtRealTriple *loc;
   DtInt *colors;
   DtInt *backfacing;

   ddr_sunview_cxp_drom_shade_triangle_list (trianglecount, elenrms, trianglecount*3,
				    vtxlocs, vtxnrms, vtxclrs, &backfacing);

   for (i=0, loc = vtxlocs; i<trianglecount; i++, loc += 3) {
      if (backfacing != NULL && backfacing[i]) 
	    continue;

     if (dde_sunview_cxp_drom_data.object_interpolation_type == DcConstantShade) {
	 colors = &dde_sunview_cxp_drom_data.color_indices[i];
      } else {
	 colors = &dde_sunview_cxp_drom_data.color_indices[i*3];
      }
      GP_draw_3D_triangle (dde_sunview_cxp_current_gp, loc, colors, 
			   dde_sunview_cxp_drom_data.object_representation_type,
			   dde_sunview_cxp_drom_data.object_interpolation_type);
   }
}
/*
 ======================================================================
 */

ddr_sunview_cxp_drom_render_triangle_mesh(colormodel,bndboxpts,trianglecount,
					  elenrms,vertexcount,
					  vtxlocs,vtxnrms,vtxclrs,vertexlist,
					  compiledvertexlist)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtInt vertexcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
     DtInt vertexlist[][3];
     DtInt compiledvertexlist[][3];

{
   extern GP_data_ptr dde_sunview_cxp_current_gp;
   DtFlag zclipflag;
   int i, j;
   DtRealTriple vlist[3];
   DtInt colors[3];
   DtInt shades_per_element;
   DtInt *backfacing = NULL;
   DtInt culling;

   culling = dde_sunview_cxp_drom_data.object_cull_switch &&
	 dde_sunview_cxp_drom_data.object_cullable_switch;

   ddr_sunview_cxp_drom_shade_triangle_mesh (trianglecount, elenrms, vertexcount,
				    vtxlocs, vtxnrms, vtxclrs, vertexlist,
				    &shades_per_element, &backfacing);

   if (shades_per_element == 1) {
      for (i=0; i<trianglecount; i++) {
	 if (culling && backfacing[i]) continue;

	 colors[0] = colors[1] = colors[2] = 
	       dde_sunview_cxp_drom_data.color_indices[i];
	 for (j=0; j<3; j++) {
	    vlist[j][0] = vtxlocs[vertexlist[i][j]][0];
	    vlist[j][1] = vtxlocs[vertexlist[i][j]][1];
	    vlist[j][2] = vtxlocs[vertexlist[i][j]][2];
	 }
      
	 GP_draw_3D_triangle (dde_sunview_cxp_current_gp, vlist, colors, 
			      dde_sunview_cxp_drom_data.object_representation_type,
			      dde_sunview_cxp_drom_data.object_interpolation_type);
      }
   } else if (shades_per_element == 3) {
      for (i=0; i<trianglecount; i++) {
	 if (culling && backfacing[i]) continue;
	 for (j=0; j<3; j++) {
	    vlist[j][0] = vtxlocs[vertexlist[i][j]][0];
	    vlist[j][1] = vtxlocs[vertexlist[i][j]][1];
	    vlist[j][2] = vtxlocs[vertexlist[i][j]][2];
	    colors[j] = dde_sunview_cxp_drom_data.color_indices[vertexlist[i][j]];
	 }
      
	 GP_draw_3D_triangle (dde_sunview_cxp_current_gp, vlist, colors, 
			      dde_sunview_cxp_drom_data.object_representation_type,
			      dde_sunview_cxp_drom_data.object_interpolation_type);
      }
   } else if (shades_per_element == 6) {
      for (i=0; i<trianglecount; i++) {
	 if (culling && backfacing[i]) continue;
	 for (j=0; j<3; j++) {
	    vlist[j][0] = vtxlocs[vertexlist[i][j]][0];
	    vlist[j][1] = vtxlocs[vertexlist[i][j]][1];
	    vlist[j][2] = vtxlocs[vertexlist[i][j]][2];
	    if (backfacing[i]) {
	       colors[j] =
		     dde_sunview_cxp_drom_data.color_indices[vertexlist[i][j]*2+1];
	    } else {
	       colors[j] = 
		     dde_sunview_cxp_drom_data.color_indices[vertexlist[i][j]*2+0];
	    }
	 }
      
	 GP_draw_3D_triangle (dde_sunview_cxp_current_gp, vlist, colors, 
			      dde_sunview_cxp_drom_data.object_representation_type,
			      dde_sunview_cxp_drom_data.object_interpolation_type);
      }
   }
}

