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
	void ddr_softpipe_drom_render_point_list
	void ddr_softpipe_drom_render_line_list
	void ddr_softpipe_drom_render_connected_line_list
	void ddr_softpipe_drom_render_triangle_list
	void ddr_softpipe_drom_render_triangle_mesh

  ======================================================================
 */
#include "drom.h"

/* ====================================================================== */

void ddr_softpipe_drom_render_point_list (
    DtColorModel colormodel,
    DtRealTriple bndboxpts[8],
    DtInt pointcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple *uv_list[],
    DtInt uvw_count,
    DtRealTriple *uvw_list[],
    DtReal *vtxalpha)
{
    DtFlag zclipflag;
    DtRealTriple *vtxptr;
    DtInt shade_type;
    DtInt i;
    DtRealTriple *clrptr;

#   ifdef DEBUG
	printf ("...processing dynout render point list\n");
	fflush (stdout);
#   endif

    ddr_softpipe_drom_clip_box
	(bndboxpts,&zclipflag, &(dde_softpipe_drom_data.xyclipped_flag));

    ddr_softpipe_drom_transform_vertices (pointcount,vtxlocs,&vtxptr);

    ddr_softpipe_drom_shade_elements
	(pointcount,DcNullPtr,pointcount, vtxptr,vtxnrms,vtxclrs,&shade_type);

    clrptr = dde_softpipe_drom_data.colored_elements;

    if (zclipflag) {
	for(i=0; i<pointcount; i++) {
	    ddr_softpipe_drom_render_point_clipped(vtxptr[i],clrptr[i]);
	}
    }
    else if (dde_softpipe_drom_data.xyclipped_flag) {
	for(i=0; i<pointcount; i++) {
	    ddr_softpipe_drom_render_point_clipped(vtxptr[i],clrptr[i]);
	}
    }
    else {
	for(i=0; i<pointcount; i++) {
	    ddr_softpipe_drom_output_point(vtxptr[i],clrptr[i]);
	}
    }
}

/* ====================================================================== */

void ddr_softpipe_drom_render_line_list (
    DtColorModel colormodel,
    DtRealTriple bndboxpts[8],
    DtInt linecount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple *uv_list[],
    DtInt uvw_count,
    DtRealTriple *uvw_list[],
    DtReal *vtxalpha)
{
   DtFlag zclipflag;
   DtRealTriple *vtxptr;
   DtInt shade_type;
   DtInt i;
   DtRealTriple *clrptr;
   DtColorRGB color[2];

#ifdef DEBUG
   printf("...processing dynout render line list\n");
   fflush(stdout);
#endif

   ddr_softpipe_drom_clip_box(bndboxpts,&zclipflag,
			       &(dde_softpipe_drom_data.xyclipped_flag));

   ddr_softpipe_drom_transform_vertices(linecount*2,vtxlocs,&vtxptr);

   ddr_softpipe_drom_shade_elements(linecount,elenrms,linecount*2,vtxptr,
				     vtxnrms,vtxclrs,&shade_type);

   clrptr = dde_softpipe_drom_data.colored_elements;

   if(zclipflag) {
      for(i=0; i<linecount; i++) {
	 if(shade_type == ddd_softpipe_drom_element_shading) {
	    color[0][0] = color[1][0] = clrptr[i][0];
	    color[0][1] = color[1][1] = clrptr[i][1];
	    color[0][2] = color[1][2] = clrptr[i][2];
	 }
	 else if(shade_type == ddd_softpipe_drom_vertex_shading) {
	    color[0][0] = clrptr[i*2][0];
	    color[0][1] = clrptr[i*2][1];
	    color[0][2] = clrptr[i*2][2];
	    color[1][0] = clrptr[i*2+1][0];
	    color[1][1] = clrptr[i*2+1][1];
	    color[1][2] = clrptr[i*2+1][2];
	 }

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    ddr_softpipe_drom_render_point_clipped(vtxptr[i*2+0], color[0]);
	    ddr_softpipe_drom_render_point_clipped(vtxptr[i*2+1], color[1]);
	    break;

	 default:
	    ddr_softpipe_drom_render_clip_line(vtxptr[i*2+0],vtxptr[i*2+1],
						color[0],color[1]);
	    break;
	 }
      }
   }
   else if(dde_softpipe_drom_data.xyclipped_flag) {
      for(i=0; i<linecount; i++) {
	 if(shade_type == ddd_softpipe_drom_element_shading) {
	    color[0][0] = color[1][0] = clrptr[i][0];
	    color[0][1] = color[1][1] = clrptr[i][1];
	    color[0][2] = color[1][2] = clrptr[i][2];
	 }
	 else if(shade_type == ddd_softpipe_drom_vertex_shading) {
	    color[0][0] = clrptr[i*2][0];
	    color[0][1] = clrptr[i*2][1];
	    color[0][2] = clrptr[i*2][2];
	    color[1][0] = clrptr[i*2+1][0];
	    color[1][1] = clrptr[i*2+1][1];
	    color[1][2] = clrptr[i*2+1][2];
	 }

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    ddr_softpipe_drom_render_point_clip2d(vtxptr[i*2+0], color[0]);
	    ddr_softpipe_drom_render_point_clip2d(vtxptr[i*2+1], color[1]);
	    break;

	 default:
	    ddr_softpipe_drom_render_clip_line2d(vtxptr[i*2+0], vtxptr[i*2+1],
						  color[0],color[1]);
	    break;
	 }
      }
   }
   else {
      for(i=0; i<linecount; i++) {
	 if(shade_type == ddd_softpipe_drom_element_shading) {
	    color[0][0] = color[1][0] = clrptr[i][0];
	    color[0][1] = color[1][1] = clrptr[i][1];
	    color[0][2] = color[1][2] = clrptr[i][2];
	 }
	 else if(shade_type == ddd_softpipe_drom_vertex_shading) {
	    color[0][0] = clrptr[i*2][0];
	    color[0][1] = clrptr[i*2][1];
	    color[0][2] = clrptr[i*2][2];
	    color[1][0] = clrptr[i*2+1][0];
	    color[1][1] = clrptr[i*2+1][1];
	    color[1][2] = clrptr[i*2+1][2];
	 }

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    ddr_softpipe_drom_output_point(vtxptr[i*2+0],color[0]);
	    ddr_softpipe_drom_output_point(vtxptr[i*2+1], color[1]);
	    break;

	 default:
	    ddr_softpipe_drom_output_line(vtxptr[i*2+0], vtxptr[i*2+1],
					   color[0],color[1]);
	    break;
	 }
      }
   }
}

/* ====================================================================== */

void ddr_softpipe_drom_render_connected_line_list (
    DtColorModel colormodel,
    DtRealTriple bndboxpts[8],
    DtInt vertexcount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple *uv_list[],
    DtInt uvw_count,
    DtRealTriple *uvw_list[],
    DtReal *vtxalpha)
{
    DtFlag zclipflag;
    DtRealTriple *vtxptr;
    DtInt shade_type;
    DtInt i;
    DtInt linecount;
    DtRealTriple *clrptr;
    DtColorRGB color[2];

#   ifdef DEBUG
	printf("...processing dynout render connected line list\n");
	fflush(stdout);
#   endif

    ddr_softpipe_drom_clip_box
	(bndboxpts,&zclipflag, &(dde_softpipe_drom_data.xyclipped_flag));

    ddr_softpipe_drom_transform_vertices (vertexcount,vtxlocs,&vtxptr);

    ddr_softpipe_drom_shade_connected_line_list
	(vertexcount,elenrms, vtxptr,vtxnrms,vtxclrs,&shade_type);

    clrptr = dde_softpipe_drom_data.colored_elements;
    linecount = vertexcount - 1;

    if (zclipflag) {
        for(i=0; i<linecount; i++) {
	    if (shade_type == ddd_softpipe_drom_element_shading) {
	        color[0][0] = color[1][0] = clrptr[i][0];
	        color[0][1] = color[1][1] = clrptr[i][1];
	        color[0][2] = color[1][2] = clrptr[i][2];
	    }
	    else if(shade_type == ddd_softpipe_drom_vertex_shading) {
	        color[0][0] = clrptr[i][0];
	        color[0][1] = clrptr[i][1];
	        color[0][2] = clrptr[i][2];
	        color[1][0] = clrptr[i+1][0];
	        color[1][1] = clrptr[i+1][1];
	        color[1][2] = clrptr[i+1][2];
	    }
  
	    switch (dde_softpipe_drom_data.object_representation_type) {
	        case DcPoints:
		    ddr_softpipe_drom_render_point_clipped
			(vtxptr[i+0], color[0]);
		    ddr_softpipe_drom_render_point_clipped
			(vtxptr[i+1], color[1]);
		    break;
  
	        default:
		    ddr_softpipe_drom_render_clip_line
			(vtxptr[i+0], vtxptr[i+1], color[0],color[1]);
	    }
        }
    }
    else if (dde_softpipe_drom_data.xyclipped_flag) {
        for (i=0; i<linecount; i++) {
  	    if (shade_type == ddd_softpipe_drom_element_shading) {
  	        color[0][0] = color[1][0] = clrptr[i][0];
  	        color[0][1] = color[1][1] = clrptr[i][1];
  	        color[0][2] = color[1][2] = clrptr[i][2];
  	    }
  	    else if (shade_type == ddd_softpipe_drom_vertex_shading) {
  	        color[0][0] = clrptr[i][0];
  	        color[0][1] = clrptr[i][1];
  	        color[0][2] = clrptr[i][2];
  	        color[1][0] = clrptr[i+1][0];
  	        color[1][1] = clrptr[i+1][1];
  	        color[1][2] = clrptr[i+1][2];
  	    }
  
  	    switch(dde_softpipe_drom_data.object_representation_type) {
  	        case DcPoints:
  	            ddr_softpipe_drom_render_point_clip2d(vtxptr[i+0],color[0]);
  	            ddr_softpipe_drom_render_point_clip2d(vtxptr[i+1],color[1]);
  	            break;
  
		default:
		    ddr_softpipe_drom_render_clip_line2d
			(vtxptr[i+0], vtxptr[i+1], color[0],color[1]);
  	    }
	}
    }
    else {
	for (i=0; i<vertexcount-1; i++) {
	    if (shade_type == ddd_softpipe_drom_element_shading) {
		color[0][0] = color[1][0] = clrptr[i][0];
		color[0][1] = color[1][1] = clrptr[i][1];
		color[0][2] = color[1][2] = clrptr[i][2];
	    }
	    else if (shade_type == ddd_softpipe_drom_vertex_shading) {
		color[0][0] = clrptr[i][0];
		color[0][1] = clrptr[i][1];
		color[0][2] = clrptr[i][2];
		color[1][0] = clrptr[i+1][0];
		color[1][1] = clrptr[i+1][1];
		color[1][2] = clrptr[i+1][2];
	    }
 
	    switch (dde_softpipe_drom_data.object_representation_type) {
		case DcPoints:
		    ddr_softpipe_drom_output_point(vtxptr[i+0],color[0]);
		    ddr_softpipe_drom_output_point(vtxptr[i+1], color[1]);
		    break;
	 
		default:
		    ddr_softpipe_drom_output_line
			(vtxptr[i+0], vtxptr[i+1], color[0],color[1]);
		    break;
	    }
        }
    }
}

/* ====================================================================== */

void ddr_softpipe_drom_render_triangle_list (
    DtColorModel colormodel,
    DtRealTriple bndboxpts[8],
    DtInt trianglecount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple *uv_list[],
    DtInt uvw_count,
    DtRealTriple *uvw_list[],
    DtReal *vtxalpha)
{
   DtFlag zclipflag;
   DtRealTriple *vtxptr;
   DtInt shade_type;
   DtInt i;
   DtInt culling_switch;
   DtInt *backfacing_flags;
   DtRealTriple *clrptr;
   DtColorRGB color[3];

#ifdef DEBUG
   printf("...processing dynout render triangle list\n");
   fflush(stdout);
#endif

#ifdef DEBUG
   for(i=0; i<trianglecount; i++) {
      printf("(%lf,%lf,%lf)-(%lf,%lf,%lf)-(%lf,%lf,%lf)\n",
	     vtxlocs[i*3][0],vtxlocs[i*3][1],vtxlocs[i*3][2],
	     vtxlocs[i*3+1][0],vtxlocs[i*3+1][1],vtxlocs[i*3+1][2],
	     vtxlocs[i*3+2][0],vtxlocs[i*3+2][1],vtxlocs[i*3+2][2]);
   }
   fflush(stdout);
#endif

   ddr_softpipe_drom_clip_box(bndboxpts,&zclipflag,
			       &(dde_softpipe_drom_data.xyclipped_flag));

   ddr_softpipe_drom_transform_vertices(trianglecount*3,vtxlocs,&vtxptr);

   ddr_softpipe_drom_shade_triangle_list(trianglecount,elenrms,
					  trianglecount*3,vtxptr,vtxnrms,
					  vtxclrs,&shade_type);

   culling_switch = (dde_softpipe_drom_data.object_cull_switch &&
		     dde_softpipe_drom_data.object_cullable_switch);
   backfacing_flags = dde_softpipe_drom_data.backfacing_flags;
   clrptr = dde_softpipe_drom_data.colored_elements;

   if(zclipflag) {
      for(i=0; i<trianglecount; i++) {
	 if(culling_switch && backfacing_flags[i])
	       continue;

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+0],clrptr[i*3+0]);
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+1],clrptr[i*3+1]);
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+2],clrptr[i*3+2]);
	    }
	    else {
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+0],clrptr[i]);
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+1],clrptr[i]);
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+2],clrptr[i]);
	    }
	    break;
	
	 case DcWireframe:
	 case DcOutlines:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_render_clip_line(vtxptr[i*3+0],
						vtxptr[i*3+1],color[0],color[1]);
	    ddr_softpipe_drom_render_clip_line(vtxptr[i*3+1],
						vtxptr[i*3+2],color[1],color[2]);
	    ddr_softpipe_drom_render_clip_line(vtxptr[i*3+2],
						vtxptr[i*3],color[2],color[0]);
	    break;

	 case DcSurface:
	 case DcSurfaceWireframeEdges:
	 case DcSurfaceOutlineEdges:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_render_clip_triangle(vtxptr[i*3+0],
						    vtxptr[i*3+1],vtxptr[i*3+2],
						    color[0],color[1],color[2]);
	    break;
	 }
      }
   }
   else if(dde_softpipe_drom_data.xyclipped_flag) {
      for(i=0; i<trianglecount; i++) {
	 if(culling_switch && backfacing_flags[i])
	       continue;

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+0],clrptr[i*3+0]);
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+1],clrptr[i*3+1]);
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+2],clrptr[i*3+2]);
	    }
	    else {
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+0],clrptr[i]);
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+1],clrptr[i]);
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+2],clrptr[i]);
	    }
	    break;

	 case DcWireframe:
	 case DcOutlines:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_render_clip_line2d(vtxptr[i*3+0],
						  vtxptr[i*3+1],color[0],color[1]);
	    ddr_softpipe_drom_render_clip_line2d(vtxptr[i*3+1],
						  vtxptr[i*3+2],color[1],color[2]);
	    ddr_softpipe_drom_render_clip_line2d(vtxptr[i*3+2],
						  vtxptr[i*3+0],color[2],color[0]);
	    break;

	 case DcSurface:
	 case DcSurfaceWireframeEdges:
	 case DcSurfaceOutlineEdges:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_render_clip_triangle2d(vtxptr[i*3+0],
						      vtxptr[i*3+1],vtxptr[i*3+2],
						      color[0],color[1],color[2]);
	    break;
	 }

      }
   }
   else {
      for(i=0; i<trianglecount; i++) {
	 if(culling_switch && backfacing_flags[i])
	       continue;

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       ddr_softpipe_drom_output_point(vtxptr[i*3+0], clrptr[i*3+0]);
	       ddr_softpipe_drom_output_point(vtxptr[i*3+1], clrptr[i*3+1]);
	       ddr_softpipe_drom_output_point(vtxptr[i*3+2], clrptr[i*3+2]);
	    }
	    else {
	       ddr_softpipe_drom_output_point(vtxptr[i*3+0], clrptr[i]);
	       ddr_softpipe_drom_output_point(vtxptr[i*3+1], clrptr[i]);
	       ddr_softpipe_drom_output_point(vtxptr[i*3+2], clrptr[i]);
	    }
	    break;

	 case DcWireframe:
	 case DcOutlines:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_output_line(vtxptr[i*3+0],
					   vtxptr[i*3+1],color[0],color[1]);
	    ddr_softpipe_drom_output_line(vtxptr[i*3+1],
					   vtxptr[i*3+2],color[1],color[2]);
	    ddr_softpipe_drom_output_line(vtxptr[i*3+2],
					   vtxptr[i*3+0],color[2],color[0]);
	    break;

	 case DcSurface:
	 case DcSurfaceWireframeEdges:
	 case DcSurfaceOutlineEdges:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_output_triangle(vtxptr[i*3+0],
					       vtxptr[i*3+1],vtxptr[i*3+2],
					       color[0],color[1],color[2]);
	    break;
	 }
      }
   }
}

/* ====================================================================== */

void ddr_softpipe_drom_render_triangle_mesh (
    DtColorModel colormodel,
    DtRealTriple bndboxpts[8],
    DtInt trianglecount,
    DtRealTriple *elenrms,
    DtInt vertexcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt vertexlist[][3],
    DtInt compiledvertexlist[][3],
    DtInt uv_count,
    DtRealCouple *uv_list[],
    DtInt uvw_count,
    DtRealTriple *uvw_list[],
    DtReal *vtxalpha)
{
   DtFlag zclipflag;
   DtRealTriple *vtxptr;
   DtRealTriple *newvtxptr;
   DtRealTriple *newvtxnrms;
   DtRealTriple *newvtxclrs;
   DtInt shade_type;
   DtInt i;
   DtInt culling_switch;
   DtInt *backfacing_flags;
   DtRealTriple *clrptr;
   DtColorRGB color[3];

#ifdef DEBUG
   printf("...processing dynout render triangle mesh\n");
   printf("TRIMSH: triangle count = %d\n",trianglecount);
   fflush(stdout);
#endif

   ddr_softpipe_drom_clip_box(bndboxpts,&zclipflag,
			       &(dde_softpipe_drom_data.xyclipped_flag));

   ddr_softpipe_drom_transform_vertices(vertexcount,vtxlocs,&vtxptr);

   /* note: the unfold routines, return pointers of memory that is
      DDspace_Allocate'ed but saved to keep from allocating and freeing for
      each primitive */

   newvtxptr = ddr_softpipe_drom_unfold_triangle_mesh(trianglecount,
					       vertexcount,vtxptr,
						       vertexlist);
   vtxptr = newvtxptr;

   newvtxnrms = ddr_softpipe_drom_unfold_vertex_normals(trianglecount,
						 vertexcount,vtxnrms,
							 vertexlist);

   newvtxclrs = ddr_softpipe_drom_unfold_vertex_colors(trianglecount,
						vertexcount,vtxclrs,
							vertexlist);

   ddr_softpipe_drom_shade_triangle_list(trianglecount,elenrms,
				  trianglecount*3,vtxptr,newvtxnrms,
					  newvtxclrs,&shade_type);

   culling_switch = (dde_softpipe_drom_data.object_cull_switch &&
		     dde_softpipe_drom_data.object_cullable_switch);
   backfacing_flags = dde_softpipe_drom_data.backfacing_flags;
   clrptr = dde_softpipe_drom_data.colored_elements;

   if(zclipflag) {
      for(i=0; i<trianglecount; i++) {
	 if(culling_switch && backfacing_flags[i])
	       continue;

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+0],
						      clrptr[i*3+0]);
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+1],
						      clrptr[i*3+1]);
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+2],
						      clrptr[i*3+2]);
	    }
	    else {
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+0],
						      clrptr[i]);
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+1],
						      clrptr[i]);
	       ddr_softpipe_drom_render_point_clipped(vtxptr[i*3+2],
						      clrptr[i]);
	    }
	    break;
	
	 case DcWireframe:
	 case DcOutlines:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_render_clip_line(vtxptr[i*3+0],
				vtxptr[i*3+1],color[0],color[1]);
	    ddr_softpipe_drom_render_clip_line(vtxptr[i*3+1],
				vtxptr[i*3+2],color[1],color[2]);
	    ddr_softpipe_drom_render_clip_line(vtxptr[i*3+2],
				vtxptr[i*3],color[2],color[0]);
	    break;

	 case DcSurface:
	 case DcSurfaceWireframeEdges:
	 case DcSurfaceOutlineEdges:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_render_clip_triangle(vtxptr[i*3+0],
				    vtxptr[i*3+1],vtxptr[i*3+2],
				    color[0],color[1],color[2]);
	    break;
	 }
      }
   }
   else if(dde_softpipe_drom_data.xyclipped_flag) {
      for(i=0; i<trianglecount; i++) {
	 if(culling_switch && backfacing_flags[i])
	       continue;

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+0],
						     clrptr[i*3+0]);
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+1],
						     clrptr[i*3+1]);
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+2],
						     clrptr[i*3+2]);
	    }
	    else {
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+0],
						     clrptr[i]);
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+1],
						     clrptr[i]);
	       ddr_softpipe_drom_render_point_clip2d(vtxptr[i*3+2],
						     clrptr[i]);
	    }
	    break;

	 case DcWireframe:
	 case DcOutlines:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_render_clip_line2d(vtxptr[i*3+0],
				  vtxptr[i*3+1],color[0],color[1]);
	    ddr_softpipe_drom_render_clip_line2d(vtxptr[i*3+1],
				  vtxptr[i*3+2],color[1],color[2]);
	    ddr_softpipe_drom_render_clip_line2d(vtxptr[i*3+2],
				  vtxptr[i*3+0],color[2],color[0]);
	    break;

	 case DcSurface:
	 case DcSurfaceWireframeEdges:
	 case DcSurfaceOutlineEdges:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_render_clip_triangle2d(vtxptr[i*3+0],
				      vtxptr[i*3+1],vtxptr[i*3+2],
				      color[0],color[1],color[2]);
	    break;
	 }

      }
   }
   else {
      for(i=0; i<trianglecount; i++) {
	 if(culling_switch && backfacing_flags[i])
	       continue;

	 switch(dde_softpipe_drom_data.object_representation_type) {
	 case DcPoints:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       ddr_softpipe_drom_output_point(vtxptr[i*3+0], 
					      clrptr[i*3+0]);
	       ddr_softpipe_drom_output_point(vtxptr[i*3+1], 
					      clrptr[i*3+1]);
	       ddr_softpipe_drom_output_point(vtxptr[i*3+2], 
					      clrptr[i*3+2]);
	    }
	    else {
	       ddr_softpipe_drom_output_point(vtxptr[i*3+0], 
					      clrptr[i]);
	       ddr_softpipe_drom_output_point(vtxptr[i*3+1], 
					      clrptr[i]);
	       ddr_softpipe_drom_output_point(vtxptr[i*3+2], 
					      clrptr[i]);
	    }
	    break;

	 case DcWireframe:
	 case DcOutlines:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_output_line(vtxptr[i*3+0],
				   vtxptr[i*3+1],color[0],color[1]);
	    ddr_softpipe_drom_output_line(vtxptr[i*3+1],
				   vtxptr[i*3+2],color[1],color[2]);
	    ddr_softpipe_drom_output_line(vtxptr[i*3+2],
				   vtxptr[i*3+0],color[2],color[0]);
	    break;

	 case DcSurface:
	 case DcSurfaceWireframeEdges:
	 case DcSurfaceOutlineEdges:
	    if(shade_type == ddd_softpipe_drom_vertex_shading) {
	       color[0][0] = clrptr[i*3][0];
	       color[0][1] = clrptr[i*3][1];
	       color[0][2] = clrptr[i*3][2];
	       color[1][0] = clrptr[i*3+1][0];
	       color[1][1] = clrptr[i*3+1][1];
	       color[1][2] = clrptr[i*3+1][2];
	       color[2][0] = clrptr[i*3+2][0];
	       color[2][1] = clrptr[i*3+2][1];
	       color[2][2] = clrptr[i*3+2][2];
	    }
	    else {
	       color[0][0] = color[1][0] = color[2][0] = clrptr[i][0];
	       color[0][1] = color[1][1] = color[2][1] = clrptr[i][1];
	       color[0][2] = color[1][2] = color[2][2] = clrptr[i][2];
	    }
	    ddr_softpipe_drom_output_triangle(vtxptr[i*3+0],
				       vtxptr[i*3+1],vtxptr[i*3+2],
				       color[0],color[1],color[2]);
	    break;
	 }
      }
   }
}
