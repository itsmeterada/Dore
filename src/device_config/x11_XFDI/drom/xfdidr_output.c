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
	static do_pseudocolor (count, origcolor, newcolor)
	static do_truecolor (count, origcolor, newcolor)
	int ddr_x11_XFDI_drom_pri_convert_colors (count, origcolor, newcolor)
	ddr_x11_XFDI_drom_convert_colors ddr_x11_XFDI_drom_convert_colors (count, origcolor)
	int ddr_x11_XFDI_drom_render_point_list (colormodel,bndboxpts,pointcount,vtxlocs,vtxnrms, vtxclrs)
	int ddr_x11_XFDI_drom_render_line_list (colormodel,bndboxpts,linecount,elenrms, vtxlocs,vtxnrms,vtxclrs)
	int ddr_x11_XFDI_drom_render_connected_line_list (colormodel,bndboxpts,vertexcount,elenrms,vtxlocs, vtxnrms,vtxclrs)
	int ddr_x11_XFDI_drom_render_triangle_list (colormodel,bndboxpts,trianglecount,elenrms,vtxlocs, vtxnrms,vtxclrs)
	int ddr_x11_XFDI_drom_render_triangle_mesh (colormodel,bndboxpts,trianglecount,elenrms,vertexcount, vtxlocs,vtxnrms,vtxclrs,vertexlist, compiledvertexlist)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
static do_pseudocolor(count, origcolor, newcolor)
     DtInt count;
     DtColorRGB origcolor[];
     unsigned long newcolor[];
{
    int i;
    DtReal rweight, gweight, bweight, scale;
    int shade_min, shade_max;

    rweight = dde_x11_XFDI_drom_curdevdat->cwtavgpsweight[0];
    gweight = dde_x11_XFDI_drom_curdevdat->cwtavgpsweight[1];
    bweight = dde_x11_XFDI_drom_curdevdat->cwtavgpsweight[2];
    scale = dde_x11_XFDI_drom_curdevdat->shade_range_scale;
    shade_min = dde_x11_XFDI_drom_curdevdat->current_shade_range_min;
    shade_max = dde_x11_XFDI_drom_curdevdat->current_shade_range_max;

    /*$dir no_recurrence*/
    for(i=0; i<count; i++) {
	newcolor[i] = shade_min + (int)((origcolor[i][0] * rweight +
					 origcolor[i][1] * gweight +
					 origcolor[i][2] * bweight)*scale);
    }
}     
/*
 ======================================================================
 */
static do_truecolor (count, origcolor, newcolor)
     DtInt count;
     DtColorRGB origcolor[];
     unsigned long newcolor[];
{
    int i;
    /*
     * multiply rather than << since the GS does not have vector shift
     */

    /*$dir no_recurrence*/
    for (i=0; i<count; i++) {
	newcolor[i] = (256*256*(255 & (int)(255.* origcolor[i][0]))) |
		(256*(255 & (int)(255.* origcolor[i][1]))) |
			(255 & (int)(255.* origcolor[i][2]));
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_pri_convert_colors(count, origcolor, newcolor)
     DtInt count;
     DtColorRGB origcolor[];
     unsigned long newcolor[];
{
    int i;
    DtReal rweight, gweight, bweight, scale;
    int shade_min, shade_max;

    if (dde_x11_XFDI_drom_curdevdat->visual_type == DcPseudoColor) {
	do_pseudocolor(count, origcolor, newcolor);

    } else {
	do_truecolor(count, origcolor, newcolor);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_convert_colors (count, origcolor)
     DtInt count;
     DtColorRGB origcolor[];
{
    int r, g, b;
    unsigned long *newcolors;
    int i;

    if (count > dde_x11_XFDI_drom_data.max_cvt_colors) {
	if (dde_x11_XFDI_drom_data.converted_colors == NULL) {
	    newcolors = (unsigned long *)
		    DDspace_Allocate(count*sizeof(unsigned long));
	} else {
	    newcolors = (unsigned long *)
		    DDspace_Reallocate(dde_x11_XFDI_drom_data.converted_colors,
				       count*sizeof(unsigned long));
	}
	if (newcolors != NULL) {
	    dde_x11_XFDI_drom_data.converted_colors = newcolors;
	    dde_x11_XFDI_drom_data.max_cvt_colors = count;
	} else {
	    count = dde_x11_XFDI_drom_data.max_cvt_colors;
	}
    }

    ddr_x11_XFDI_drom_pri_convert_colors(count, origcolor, 
					 dde_x11_XFDI_drom_data.converted_colors);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_render_point_list(colormodel,bndboxpts,pointcount,
				    vtxlocs,vtxnrms, vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt pointcount;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
    /*   ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */

    /*  We may want to check for non-null vertex normals here and simulate
	shading in software.  */

    if (vtxclrs == NULL) {
	XFDIPolyMarker(dde_x11_XFDI_drom_curdevdat->display,
		       dde_x11_XFDI_drom_curdevdat->gc,
		       vtxlocs,pointcount);
    } else {
	ddr_x11_XFDI_drom_convert_colors(pointcount, vtxclrs);

	XFDIPolyMarkerColors(dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     vtxlocs,
			     dde_x11_XFDI_drom_data.converted_colors,
			     pointcount);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_render_line_list(colormodel,bndboxpts,linecount,elenrms,
				   vtxlocs,vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt linecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;

{

    if (dde_x11_XFDI_drom_data.reptype == DcPoints) {
	ddr_x11_XFDI_drom_render_point_list(bndboxpts,linecount*2,
					    vtxlocs,vtxnrms,vtxclrs);
	return;
    }

    /* ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */

    if (vtxclrs == NULL) {
	XFDIDisjointPolyLine(dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     vtxlocs,linecount*2);
    } else {
	ddr_x11_XFDI_drom_convert_colors(linecount*2, vtxclrs);

	XFDIDisjointPolyLineVcolors(dde_x11_XFDI_drom_curdevdat->display,
				    dde_x11_XFDI_drom_curdevdat->gc,
				    vtxlocs,
				    dde_x11_XFDI_drom_data.converted_colors,
				    linecount*2);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_render_connected_line_list(colormodel,bndboxpts,vertexcount,elenrms,vtxlocs,
					     vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt vertexcount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
    if (dde_x11_XFDI_drom_data.reptype == DcPoints) {
	ddr_x11_XFDI_drom_render_point_list(bndboxpts,vertexcount,vtxlocs,
					    vtxnrms,vtxclrs);

	return;
    }

    /*   ddr_x11_XFDI_drom_check_bounding_box(bndboxpts); */


    if (vtxclrs == NULL) {
	XFDIPolyLine(dde_x11_XFDI_drom_curdevdat->display,
		     dde_x11_XFDI_drom_curdevdat->gc,
		     vtxlocs,vertexcount);
    } else {
	ddr_x11_XFDI_drom_convert_colors(vertexcount, vtxclrs);

	XFDIPolyLineVcolors(dde_x11_XFDI_drom_curdevdat->display,
			    dde_x11_XFDI_drom_curdevdat->gc,
			    vtxlocs,
			    dde_x11_XFDI_drom_data.converted_colors,
			    vertexcount);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_render_triangle_list(colormodel,bndboxpts,trianglecount,elenrms,vtxlocs,
				       vtxnrms,vtxclrs)
     DtColorModel colormodel;
     DtRealTriple bndboxpts[8];
     DtInt trianglecount;
     DtRealTriple *elenrms;
     DtRealTriple *vtxlocs;
     DtRealTriple *vtxnrms;
     DtRealTriple *vtxclrs;
{
    static int list_size = 0;
    static int *vlist = NULL;
    int *newvlist;
    int i, *p;
    int size;

    if (dde_x11_XFDI_drom_data.reptype == DcPoints) {
	ddr_x11_XFDI_drom_render_point_list(bndboxpts,trianglecount*3,vtxlocs,
					    vtxnrms,vtxclrs);

	return;
    }

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

	ddr_x11_XFDI_drom_convert_colors(trianglecount*3, vtxclrs);

	if (vtxnrms == NULL  || dde_x11_XFDI_drom_data.constant_shading_switch) {
	    XFDIPolyhTriVcolors(dde_x11_XFDI_drom_curdevdat->display,
				dde_x11_XFDI_drom_curdevdat->gc,
				vtxlocs, 
				dde_x11_XFDI_drom_data.converted_colors,
				trianglecount*3, vlist);
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
				     vtxlocs, vtxnrms, 
				     dde_x11_XFDI_drom_data.converted_colors,
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
ddr_x11_XFDI_drom_render_triangle_mesh(colormodel,bndboxpts,trianglecount,elenrms,vertexcount,
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
    static int list_size = 0;
    static int *vlist = NULL;
    int *newvlist;
    int i, *p;
    int size;

    if (dde_x11_XFDI_drom_data.reptype == DcPoints) {
	ddr_x11_XFDI_drom_render_point_list(bndboxpts,vertexcount,vtxlocs,
					    vtxnrms,vtxclrs);

	return;
    }

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

	ddr_x11_XFDI_drom_convert_colors(vertexcount, vtxclrs);

	if (vtxnrms == NULL  || dde_x11_XFDI_drom_data.constant_shading_switch) {
	    XFDIPolyhTriVcolors(dde_x11_XFDI_drom_curdevdat->display,
				dde_x11_XFDI_drom_curdevdat->gc,
				vtxlocs, 
				dde_x11_XFDI_drom_data.converted_colors,
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
				     vtxlocs, vtxnrms, 
				     dde_x11_XFDI_drom_data.converted_colors,
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

