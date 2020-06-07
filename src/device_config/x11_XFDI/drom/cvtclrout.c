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
	int ddr_x11_XFDI_drom_output_pntlst (bndboxpts,pointcount,elenrms,vtxlocs,vtxnrms, vtxclrs)
	int ddr_x11_XFDI_drom_output_linlst (bndboxpts,linecount,elenrms, vtxlocs,vtxnrms,vtxclrs)
	int ddr_x11_XFDI_drom_output_plylin (bndboxpts,vertexcount,elenrms,vtxlocs, vtxnrms,vtxclrs)
	int ddr_x11_XFDI_drom_output_trilst (bndboxpts,trianglecount,elenrms,vtxlocs, vtxnrms,vtxclrs)
	int ddr_x11_XFDI_drom_output_trimsh (bndboxpts,trianglecount,elenrms, vertexcount, vtxlocs,vtxnrms,vtxclrs,vertexlist, compiledvertexlist)
	int ddr_x11_XFDI_drom_output_tristrip (bndboxpts, vertex_count, vertex_location, vertex_normal, vertex_color, uv_count, uv_list, uvw_count, uvw_list)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */

/*
 * Output pntlst using converted colors for GS 
 */
      
ddr_x11_XFDI_drom_output_pntlst(bndboxpts,pointcount,elenrms,vtxlocs,vtxnrms,
				  vtxclrs)
     DtRealTriple bndboxpts[8];
     DtInt pointcount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     unsigned long *vtxclrs;
{
    /*   ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */

    /*  We may want to check for non-null vertex normals here and simulate
	shading in software.  */

    if (vtxclrs == NULL) {
	XFDIPolyMarker(dde_x11_XFDI_drom_curdevdat->display,
		       dde_x11_XFDI_drom_curdevdat->gc,
		       vtxlocs,pointcount);
    } else {

	XFDIPolyMarkerColors(dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     vtxlocs, vtxclrs, pointcount);
    }
}
/*
 ======================================================================
 */

/*
 * Output linlst using converted colors for GS 
 */

ddr_x11_XFDI_drom_output_linlst(bndboxpts,linecount,elenrms,
				vtxlocs,vtxnrms,vtxclrs)
     DtRealTriple bndboxpts[8];
     DtInt linecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     unsigned long *vtxclrs;

{

    /* ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */

    if (vtxclrs == NULL) {
	XFDIDisjointPolyLine(dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     vtxlocs,linecount*2);
    } else {
	XFDIDisjointPolyLineVcolors(dde_x11_XFDI_drom_curdevdat->display,
				    dde_x11_XFDI_drom_curdevdat->gc,
				    vtxlocs, vtxclrs, linecount*2);
    }
}
/*
 ======================================================================
 */

/*
 * Output plylin using converted colors for GS 
 */
      
ddr_x11_XFDI_drom_output_plylin(bndboxpts,vertexcount,elenrms,vtxlocs,
				vtxnrms,vtxclrs)
     DtRealTriple bndboxpts[8];
     DtInt vertexcount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     unsigned long *vtxclrs;
{

    /*   ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */

    if (vtxclrs == NULL) {
	XFDIPolyLine(dde_x11_XFDI_drom_curdevdat->display,
		     dde_x11_XFDI_drom_curdevdat->gc,
		     vtxlocs,vertexcount);
    } else {
	XFDIPolyLineVcolors(dde_x11_XFDI_drom_curdevdat->display,
			    dde_x11_XFDI_drom_curdevdat->gc,
			    vtxlocs, vtxclrs, vertexcount);
    }
}
/*
 ======================================================================
 */

/*
 * Output trilst using converted colors for GS
 */

ddr_x11_XFDI_drom_output_trilst(bndboxpts,trianglecount,elenrms,vtxlocs,
				vtxnrms,vtxclrs)
     DtRealTriple bndboxpts[8];
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     unsigned long *vtxclrs;
{
    static int list_size = 0;
    static int *vlist = NULL;
    int *newvlist;
    int i, *p;
    int size;

    /*   ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */

    size = trianglecount*4 + 1;
    if (size > list_size) {
	if (vlist == NULL) {
	    newvlist = (int *)DDspace_Allocate(size*sizeof(int));
	} else {
	    newvlist = (int *)
		    DDspace_Reallocate(vlist, size*sizeof(int));
	}
	if (newvlist != NULL) {
	    vlist = newvlist;
	    list_size = size;
	} else {
	    return;
	}
    }

    for (i=0, p=vlist; i<trianglecount; i++) {
	*p++ = 3;
	*p++ = i*3 + 1;
	*p++ = i*3 + 2;
	*p++ = i*3 + 3;
    }
    *p = 0;

    if (vtxclrs != NULL) {

	if (vtxnrms == NULL  || dde_x11_XFDI_drom_data.constant_shading_switch) {
	    XFDIPolyhTriVcolors(dde_x11_XFDI_drom_curdevdat->display,
				dde_x11_XFDI_drom_curdevdat->gc,
				vtxlocs, vtxclrs, trianglecount*3, vlist);
	} else {
	    /*
	     * NOTE: Phong shading does not work with vertex colors
	     * the object disappears so only allow up to Gauroud shading
	     * with vertex colors
	     * Also Does not work with Flat shading object turns white,
	     * so always use Gauroud Shading !! -- Boo Hiss
	     */
	    if (dde_x11_XFDI_drom_data.interptype == DcSurfaceShade ||
		dde_x11_XFDI_drom_data.interptype == DcConstantShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeColor);
	    }
	    XFDIVnormPolyhTriVcolors(dde_x11_XFDI_drom_curdevdat->display,
				     dde_x11_XFDI_drom_curdevdat->gc,
				     vtxlocs, vtxnrms, vtxclrs,
				     trianglecount*3, vlist);
	    if (dde_x11_XFDI_drom_data.interptype == DcSurfaceShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeNormal);
	    } else if (dde_x11_XFDI_drom_data.interptype == DcConstantShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeNone);
	    }
	} 
    } else {
	if (vtxnrms == NULL ||  dde_x11_XFDI_drom_data.constant_shading_switch) {
	    XFDIPolyhTri(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 vtxlocs,trianglecount*3, vlist);
	} else {
	    XFDIVnormPolyhTri(dde_x11_XFDI_drom_curdevdat->display,
			      dde_x11_XFDI_drom_curdevdat->gc,
			      vtxlocs,vtxnrms,trianglecount*3, vlist);
	}
    }
}
/*
 ======================================================================
 */

/*
 * Output trimsh using converted colors for GS 
 */
      
ddr_x11_XFDI_drom_output_trimsh(bndboxpts,trianglecount,elenrms,
				vertexcount, vtxlocs,vtxnrms,vtxclrs,vertexlist, 
				compiledvertexlist)
     DtRealTriple bndboxpts[8];
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtInt vertexcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     unsigned long *vtxclrs;
     DtInt vertexlist[][3];
     DtInt compiledvertexlist[][3];
{
    static int list_size = 0;
    static int *vlist = NULL;
    int *newvlist;
    int i, *p;
    int size;

    /*   ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */

    size = trianglecount*4 + 1;
    if (size > list_size) {
	if (vlist == NULL) {
	    newvlist = (int *)DDspace_Allocate(size*sizeof(int));
	} else {
	    newvlist = (int *)
		    DDspace_Reallocate(vlist, size*sizeof(int));
	}
	if (newvlist != NULL) {
	    vlist = newvlist;
	    list_size = size;
	} else {
	    return;
	}
    }

    for (i=0, p=vlist; i<trianglecount; i++) {
	*p++ = 3;
	*p++ = vertexlist[i][0] + 1;
	*p++ = vertexlist[i][1] + 1;
	*p++ = vertexlist[i][2] + 1;
    }
    *p = 0;

    if (vtxclrs != NULL) {

	if (vtxnrms == NULL  || dde_x11_XFDI_drom_data.constant_shading_switch) {
	    XFDIPolyhTriVcolors(dde_x11_XFDI_drom_curdevdat->display,
				dde_x11_XFDI_drom_curdevdat->gc,
				vtxlocs, 
				vtxclrs,
				vertexcount, vlist);
	} else {
	    /*
	     * NOTE: Phong shading does not work with vertex colors
	     * the object disappears so only allow up to Gauroud shading
	     * with vertex colors
	     * Also Does not work with Flat shading object turns white,
	     * so always use Gauroud Shading !! -- Boo Hiss
	     */
	    if (dde_x11_XFDI_drom_data.interptype == DcSurfaceShade ||
		dde_x11_XFDI_drom_data.interptype == DcConstantShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeColor);
	    }
	    XFDIVnormPolyhTriVcolors(dde_x11_XFDI_drom_curdevdat->display,
				     dde_x11_XFDI_drom_curdevdat->gc,
				     vtxlocs, vtxnrms, vtxclrs,
				     vertexcount, vlist);
	    if (dde_x11_XFDI_drom_data.interptype == DcSurfaceShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeNormal);
	    } else if (dde_x11_XFDI_drom_data.interptype == DcConstantShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeNone);
	    }
	} 
    } else {
	if (vtxnrms == NULL ||  dde_x11_XFDI_drom_data.constant_shading_switch) {
	    XFDIPolyhTri(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 vtxlocs,vertexcount, vlist);
	} else {
	    XFDIVnormPolyhTri(dde_x11_XFDI_drom_curdevdat->display,
			      dde_x11_XFDI_drom_curdevdat->gc,
			      vtxlocs,vtxnrms,vertexcount, vlist);
	}
    }
}

/*
 ======================================================================
 */
ddr_x11_XFDI_drom_output_tristrip(bndboxpts, vertex_count, vertex_location,
				  vertex_normal, vertex_color,
				  uv_count, uv_list, uvw_count, uvw_list)
     DtRealTriple	bndboxpts[8];
     DtInt 		vertex_count;
     DtRealTriple 	*vertex_location;
     DtRealTriple	*vertex_normal;
     unsigned long	*vertex_color;
     DtInt 		uv_count;
     DtRealCouple	**uv_list;
     DtInt		uvw_count;
     DtRealTriple	**uvw_list;
{
    int idx;

    if (dde_x11_XFDI_drom_data.map_diffuse_color &&
	((uv_count  > 0 && dde_x11_XFDI_drom_data.texture_2D_valid) ||
	 (uvw_count > 0 && dde_x11_XFDI_drom_data.texture_3D_valid))) {
	/* 
	 * this primitive can be texture mapped, and it should be texture
	 * mapped
	 */
	if (!dde_x11_XFDI_drom_data.texture_pass_required &&
	    !dde_x11_XFDI_drom_data.texture_pass) {
	    /*
	     * Didn't think there was texture mapping to be done, but
	     * wrong,  Abort the traversal, and indicate we have texture
	     * mapping to do.
	     */
	    dde_x11_XFDI_drom_data.texture_pass_required = DcTrue;
	    DsExecutionAbort();
	    return;
	} 
	if (!dde_x11_XFDI_drom_data.texture_pass_required) {
	    /*
	     * This is the first textured object so indicate that
	     * the blend will be required
	     */
	    dde_x11_XFDI_drom_data.texture_pass_required = DcTrue;
	}
	if (dde_x11_XFDI_drom_data.texture_pass) {
	    /*
	     * Texture Pass to draw with UV's
	     */
	    if (dde_x11_XFDI_drom_data.texture_2D_valid && uv_count > 0) {
		/*
		 * 2D Texturing
		 */
		idx = uv_count > dde_x11_XFDI_drom_data.map_diffuse_color_uv_index ?
			dde_x11_XFDI_drom_data.map_diffuse_color_uv_index : 0;

		XFDIPolyTriUV(dde_x11_XFDI_drom_curdevdat->display,
			      dde_x11_XFDI_drom_curdevdat->gc,
			      vertex_location,
			      uv_list[idx],
			      vertex_count);
	    } else {
		/*
		 * 3D Texturing
		 */
		idx = uvw_count > dde_x11_XFDI_drom_data.map_diffuse_color_uvw_index ?
			dde_x11_XFDI_drom_data.map_diffuse_color_uvw_index : 0;

		XFDIPolyTriUVW(dde_x11_XFDI_drom_curdevdat->display,
			       dde_x11_XFDI_drom_curdevdat->gc,
			       vertex_location,
			       uvw_list[idx],
			       vertex_count);
	    }
	    if (vertex_normal == (DtRealTriple *)0) {
		dde_x11_XFDI_drom_data.texture_no_norms = DcTrue;
		doe_system_timing_triangles_drawn += vertex_count - 2;
	    }
	} else if (vertex_normal != (DtRealTriple *)0) {
	    XFDIVnormPolyTriTexture(dde_x11_XFDI_drom_curdevdat->display,
				    dde_x11_XFDI_drom_curdevdat->gc,
				    vertex_location,
				    vertex_normal,
				    vertex_count);

	    doe_system_timing_triangles_drawn += vertex_count - 2;
	}
    } else {
	/*
	 * No Texturing
	 */

	doe_system_timing_triangles_drawn += vertex_count - 2;

	/*   ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */

	if (vertex_color != (unsigned long *)0) {
	    /*
	     * NOTE: Phong shading does not work with vertex colors
	     * so only allow up to Gouraud shading  with vertex colors
	     *
	     * Also Does not work with Flat shading object turns white,
	     * so always use Gauroud Shading !! -- Boo Hiss
	     */
	    if (dde_x11_XFDI_drom_data.interptype == DcSurfaceShade ||
		dde_x11_XFDI_drom_data.interptype == DcConstantShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeColor);
	    }

	    if (dde_x11_XFDI_drom_data.constant_shading_switch ||
		vertex_normal == NULL) {
		XFDIPolyTriVcolors(dde_x11_XFDI_drom_curdevdat->display,
				   dde_x11_XFDI_drom_curdevdat->gc,
				   vertex_location,
				   vertex_color,
				   vertex_count);

	    } else		/* vertex colors and vertex normals */ { 
		XFDIVnormPolyTriVcolors(dde_x11_XFDI_drom_curdevdat->display,
					dde_x11_XFDI_drom_curdevdat->gc,
					vertex_location, 
					vertex_normal,
					vertex_color,
					vertex_count);
	    }
	    if (dde_x11_XFDI_drom_data.interptype == DcSurfaceShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeNormal);
	    } else if (dde_x11_XFDI_drom_data.interptype == DcConstantShade) {
		XFDISetSurfaceShading(dde_x11_XFDI_drom_curdevdat->display,
				      dde_x11_XFDI_drom_curdevdat->gc,
				      XFDIShadeNone);
	    }
	} else			/* vertex_color == NULL */ { 
	    if (dde_x11_XFDI_drom_data.constant_shading_switch ||
		vertex_normal == NULL) {
		XFDIPolyTri(dde_x11_XFDI_drom_curdevdat->display,
			    dde_x11_XFDI_drom_curdevdat->gc,
			    vertex_location,
			    vertex_count);
	    } else		/* no vertex colors and vertex normals */ {
		XFDIVnormPolyTri(dde_x11_XFDI_drom_curdevdat->display,
				 dde_x11_XFDI_drom_curdevdat->gc,
				 vertex_location, 
				 vertex_normal,
				 vertex_count);
	    }
	}
    }
}
