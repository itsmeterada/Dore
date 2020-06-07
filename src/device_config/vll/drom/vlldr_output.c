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
	void ddr_vll_drom_render_point_list (colormodel,bndboxpts,pointcount,
		vtxlocs,vtxnrms, vtxclrs)
	void ddr_vll_drom_render_line_list (colormodel,bndboxpts,linecount,
		elenrms, vtxlocs,vtxnrms,vtxclrs)
	void ddr_vll_drom_render_connected_line_list (colormodel,bndboxpts,
		vertexcount,elenrms,vtxlocs, vtxnrms,vtxclrs)
	void ddr_vll_drom_render_triangle_list (colormodel,bndboxpts,
		trianglecount,elenrms,vtxlocs, vtxnrms,vtxclrs, uv_count,uv_list,
		uvw_count, uvw_list)
	void ddr_vll_drom_render_triangle_mesh (colormodel,bndboxpts,
		trianglecount,elenrms,vertexcount, vtxlocs,vtxnrms,vtxclrs,
		vertexlist, compiledvertexlist, uv_count,uv_list, 
		uvw_count,uvw_list)
	void ddr_vll_drom_render_triangle_strip (colormodel,bndboxpts,
		vertexcount, vtxlocs,vtxnrms,vtxclrs, elenrms,
		uv_count,uv_list, uvw_count,uvw_list)
	void ddr_vll_drom_render_quad_list (colormodel,bndboxpts, quadcount,
		elenrms, vtxlocs,vtxnrms,vtxclrs, uv_count,uv_list, 
		uvw_count,uvw_list)
	void ddr_vll_drom_render_quad_mesh (colormodel,bndboxpts, quadcount,
		elenrms,vertexcount, vtxlocs,vtxnrms,vtxclrs, vertexlist, 
		compiledvertexlist, uv_count,uv_list, uvw_count,uvw_list)

  ======================================================================
 */
#include "drom.h"

DtInt *dde_vll_triangle_mesh_uvvertexlist = (DtInt *)0;
DtRealCouple *dde_vll_triangle_mesh_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_vll_triangle_mesh_bboxuvw = (DtRealTriple *)0;
DtInt *dde_vll_triangle_mesh_edge_enable = (DtInt *)0;

DtInt *dde_vll_triangle_list_edge_enable = (DtInt *)0;
DtRealCouple *dde_vll_triangle_list_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_vll_triangle_list_bboxuvw = (DtRealTriple *)0;

DtRealCouple *dde_vll_triangle_strip_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_vll_triangle_strip_bboxuvw = (DtRealTriple *)0;
DtInt *dde_vll_triangle_strip_edge_enable = (DtInt *)0;

DtInt *dde_vll_quad_mesh_uvvertexlist = (DtInt *)0;
DtRealCouple *dde_vll_quad_mesh_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_vll_quad_mesh_bboxuvw = (DtRealTriple *)0;
DtInt *dde_vll_quad_mesh_edge_enable = (DtInt *)0;

DtRealCouple *dde_vll_quad_list_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_vll_quad_list_bboxuvw = (DtRealTriple *)0;
DtInt *dde_vll_quad_list_edge_enable = (DtInt *)0;

/* may be used by packages to set element colors for geometry */
extern DtRealTriple *dre_dynrnd_drom_element_colors;

/*
 ======================================================================
 */
void 
ddr_vll_drom_render_point_list(colormodel,bndboxpts,pointcount,
	vtxlocs,vtxnrms,vtxclrs)
DtColorModel colormodel;
DtRealTriple bndboxpts[8];
DtInt pointcount;
DtRealTriple *vtxlocs;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;
{
    switch(dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_texture_pass:
    case ddd_vll_drom_shadow_pass:
	/* can't do points in these modes */
	break;

    default:
	VLLpoint_list(pointcount, (void *)bndboxpts, (void *)vtxlocs, 
		      (void *)vtxnrms, (void *)vtxclrs);
	break;
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_render_line_list(colormodel,bndboxpts,linecount,elenrms,
	vtxlocs,vtxnrms,vtxclrs)
DtColorModel colormodel;
DtRealTriple bndboxpts[8];
DtInt linecount;
DtRealTriple *elenrms;
DtRealTriple *vtxlocs;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;
{
    switch(dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_texture_pass:
    case ddd_vll_drom_shadow_pass:
	/* can't do lines in these modes */
	break;

    default:
	VLLline_list(linecount, (void *)bndboxpts, (void *)vtxlocs, 
		     (void *)elenrms, (void *)vtxnrms, (void *)0, (void *)vtxclrs);
	break;
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_render_connected_line_list(colormodel,bndboxpts,vertexcount,
	elenrms,vtxlocs, vtxnrms,vtxclrs)
DtColorModel colormodel;
DtRealTriple bndboxpts[8];
DtInt vertexcount;
DtRealTriple *elenrms;
DtRealTriple *vtxlocs;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;
{
    switch(dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_texture_pass:
    case ddd_vll_drom_shadow_pass:
	/* can't do lines in these modes */
	break;

    default:
	VLLpoly_line(vertexcount-1, (void *)bndboxpts, (void *)vtxlocs, 
		     (void *)elenrms, (void *)vtxnrms, (void *)0, (void *)vtxclrs);
	break;
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_render_triangle_list(colormodel,bndboxpts,trianglecount,elenrms,
	vtxlocs,vtxnrms,vtxclrs, uv_count, uv_list, uvw_count, uvw_list)
DtColorModel colormodel;
DtRealTriple bndboxpts[8];
DtInt trianglecount;
DtRealTriple *elenrms;
DtRealTriple *vtxlocs;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;
DtInt uv_count;
DtRealCouple *uv_list[];
DtInt uvw_count;
DtRealTriple *uvw_list[];
{
    int idx;
    static float white[3] = {1.,1.,1.};
    static float black[3] = {0.,0.,0.};
    int texture_mapping;

    /* 
     * Determine what kind of texture mapping, if any, that we are doing. 
     */
    texture_mapping = 0;
    if (dde_vll_drom_data.map_diffuse_color) {
	if (uv_count > 0 && dde_vll_drom_data.texture_2D_valid)
		texture_mapping = 1;
	else if (uvw_count > 0 && dde_vll_drom_data.texture_3D_valid)
		texture_mapping = 2;
    } else if (dde_vll_drom_data.map_env &&
	       dde_vll_drom_data.texture_env_valid) {
	texture_mapping = 3;
    }

    if (texture_mapping) {
	/* 
	 * This primitive can and should be texture mapped.   Determine
	 * whether we know this is texture pass:  if we don't then
	 * abort the pass; if we do, continue.  In either case, indicate
	 * that we have texture mapping to do.
	 */
	if ( ddd_vll_drom_texture_pass != dde_vll_drom_data.traversal_type &&
	    !dde_vll_drom_data.have_textures) {

	    dde_vll_drom_data.have_textures = DcTrue;
	    DsExecutionAbort();
	    return;
	} 

	dde_vll_drom_data.have_textures = DcTrue;
    }

    switch (dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_texture_pass:
	switch (texture_mapping) {
	case 0:			/* NONE */
	    break;

	case 1:			/* 2D Texture Map */
	    idx = uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		    dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLtriangle_list(trianglecount, (void *)bndboxpts, 
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)dre_dynrnd_drom_element_colors,
                             (void *)0, (void *)0, (void *)0, 
			     (void *)uv_list[idx], (void *)0, 
			     dde_vll_triangle_list_edge_enable,
			     (void *)dde_vll_triangle_list_bboxuv);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;

	case 2:			/* 3D Texture Map */
	    idx = uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
		    dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLtriangle_list(trianglecount, (void *)bndboxpts, 
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)dre_dynrnd_drom_element_colors,
                             (void *)0, (void *)0, (void *)0, 
			     (void *)uvw_list[idx], (void *)0,
			     dde_vll_triangle_list_edge_enable,
			     (void *)dde_vll_triangle_list_bboxuvw);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;

	case 3:			/* Environment Map */
	    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
	    VLLtriangle_list(trianglecount, (void *)bndboxpts, 
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)dre_dynrnd_drom_element_colors,
                             (void *)0, (void *)0, (void *)0, 
			     (void *)0, (void *)0, 
			     dde_vll_triangle_list_edge_enable,
			     (void *)0);
	    break;
	}
	break;

    case ddd_vll_drom_transp_pass:
	/* 
	 * Cannot do both texture mapping and transparency, so texture 
	 * mapping wins.
	 */
	if (texture_mapping) {
	    break;
	} else {
	    VLLtriangle_list(trianglecount, (void *)bndboxpts,
                             (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
                             (void *)dre_dynrnd_drom_element_colors,
                             (void *)vtxclrs, (void *)0,
                             (void *)0, (void *)0, (void *)0,
                             dde_vll_triangle_list_edge_enable,
                             (void *)0);
	}
	break;

    case ddd_vll_drom_standard_pass:
	/* 
	 * Fall into the default code if we're texture mapping, regardless of
	 * transparency, as this is now the second pass.  Otherwise, a
	 * standard pass with transparency shouldn't happen, so break.
	 */
	if ((dde_vll_drom_data.transparent_switch) &&
	    !(dde_vll_drom_data.map_diffuse_color || dde_vll_drom_data.map_env)) {
		break;
	}

    default:
	if (!texture_mapping) {
	    /*
	     * Do standard pass.
	     */
	    if ((dde_vll_drom_data.traversal_type == ddd_vll_drom_shadow_pass) &&
	    	   ((dde_vll_drom_data.reptype == DcOutlines)  ||
		    (dde_vll_drom_data.reptype == DcWireframe) ||
		    (dde_vll_drom_data.reptype == DcPoints))) {
		    /* don't render shadows for these reptypes */
		    break;
	    }
        
	    VLLtriangle_list(trianglecount, (void *)bndboxpts, 
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
			     (void *)dre_dynrnd_drom_element_colors,
                             (void *)vtxclrs, (void *)0,
			     (void *)0, (void *)0, (void *)0, 
			     dde_vll_triangle_list_edge_enable,
			     (void *)0);
	} else {
	    /*
	     * Do the lighting pass for texture mapped triangle list.
	     */
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);
	    }
	    switch (dde_vll_drom_data.map_operator) {
	    case DcTextureMultiply:
		VLLtriangle_list(trianglecount, (void *)bndboxpts, 
				 (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
				 (void *)dre_dynrnd_drom_element_colors,
                                 (void *)vtxclrs, (void *)0,
				 (void *)0, (void *)0, (void *)0,
				 dde_vll_triangle_list_edge_enable,
				 (void *)0);
		break;
	    case DcTextureReplace:
	    default:
		VLLset_diffuse_color(white);
		VLLtriangle_list(trianglecount, (void *)bndboxpts, 
				 (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
				 (void *)dre_dynrnd_drom_element_colors,
                                 (void *)0, (void *)0,
				 (void *)0, (void *)0, (void *)0,
				 dde_vll_triangle_list_edge_enable,
				 (void *)0);
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_STANDARD);
	    }
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_render_triangle_mesh(colormodel,bndboxpts,trianglecount,elenrms,
	vertexcount, vtxlocs,vtxnrms,vtxclrs,vertexlist, compiledvertexlist, 
	uv_count, uv_list, uvw_count, uvw_list)
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
DtInt uv_count;
DtRealCouple *uv_list[];
DtInt uvw_count;
DtRealTriple *uvw_list[];
{
    int idx;
    DtInt *connect[3];
    static float white[3] = {1.,1.,1.};
    static float black[3] = {0.,0.,0.};
    int texture_mapping;

    connect[0] = (DtInt *)vertexlist;
    connect[1] = (DtInt *)compiledvertexlist;
    connect[2] = (DtInt *)dde_vll_triangle_mesh_uvvertexlist;

    /*
     * Determine what kind of texture mapping, if any, that we are doing.
     */
    texture_mapping = 0;
    if (dde_vll_drom_data.map_diffuse_color) {
	if (uv_count > 0 && dde_vll_drom_data.texture_2D_valid)
		texture_mapping = 1;
	else if (uvw_count > 0 && dde_vll_drom_data.texture_3D_valid)
		texture_mapping = 2;
    } else if (dde_vll_drom_data.map_env &&
	       dde_vll_drom_data.texture_env_valid) {
	texture_mapping = 3;
    }

    if (texture_mapping) {
	/* 
         * This primitive can and should be texture mapped.   Determine
         * whether we know this is texture pass:  if we don't then
         * abort the pass; if we do, continue.  In either case, indicate
         * that we have texture mapping to do.
	 */
	if (ddd_vll_drom_texture_pass != dde_vll_drom_data.traversal_type &&
	    !dde_vll_drom_data.have_textures) {

	    dde_vll_drom_data.have_textures = DcTrue;
	    DsExecutionAbort();
	    return;
	} 

	dde_vll_drom_data.have_textures = DcTrue;
    }

    switch (dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_texture_pass:
	switch (texture_mapping) {
	case 0:			/* NONE */
	    break;

	case 1:			/* 2D Texture Map */
	    idx = uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		    dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLtriangle_mesh((void *)bndboxpts,vertexcount,
			     (void *)vtxlocs, (void *)vtxnrms, (void *)0,
			     (void *)0, (void *)uv_list[idx], (void *)0, 
			     trianglecount, (void *)elenrms,
                             (void *)dre_dynrnd_drom_element_colors, 
			     (void *)0, connect, 
			     dde_vll_triangle_mesh_edge_enable,
			     (void *)dde_vll_triangle_mesh_bboxuv);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;
	case 2:			/* 3D Texture Map */
	    idx = uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
		    dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLtriangle_mesh((void *)bndboxpts,vertexcount,
			     (void *)vtxlocs, (void *)vtxnrms, (void *)0, 
			     (void *)0, (void *)uvw_list[idx], (void *)0,
			     trianglecount, (void *)elenrms,
                             (void *)dre_dynrnd_drom_element_colors,
			     (void *)0, connect, 
			     dde_vll_triangle_mesh_edge_enable,
			     (void *)dde_vll_triangle_mesh_bboxuvw);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;
	case 3:			/* Environment Map */
	    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
	    VLLtriangle_mesh((void *)bndboxpts,vertexcount,
			     (void *)vtxlocs, (void *)vtxnrms, (void *)0,
			     (void *)0, (void *)0, (void *)0,
			     trianglecount, (void *)elenrms,
                             (void *)dre_dynrnd_drom_element_colors,
			     (void *)0, connect, 
			     dde_vll_triangle_mesh_edge_enable,
			     (void *)0);
	    break;
	}
	break;

    case ddd_vll_drom_transp_pass:
	/* 
	 * Cannot do both texture mapping and transparency, so texture 
	 * mapping wins.
	 */
	if (texture_mapping) {
	    break;
	} else {            
	    VLLtriangle_mesh((void *)bndboxpts,vertexcount,
                             (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs,
                             (void *)0, (void *)0, (void *)0,
                             trianglecount, (void *)elenrms,
                             (void *)dre_dynrnd_drom_element_colors,
                             (void *)0, connect,
                             dde_vll_triangle_mesh_edge_enable, (void *)0);
	}
	break;

    case ddd_vll_drom_standard_pass:
	/* 
	 * Fall into the default code if we're texture mapping, regardless of
	 * transparency, as this is now the second pass.  Otherwise, a 
	 * standard pass with transparency shouldn't happen, so break. 
	 */ 
	if ((dde_vll_drom_data.transparent_switch) &&
	    !(dde_vll_drom_data.map_diffuse_color || dde_vll_drom_data.map_env)) {
		break;
	}

    default:
	if (!texture_mapping) {
	   /*
	    * Do standard pass.
	    */
	    if ((dde_vll_drom_data.traversal_type == ddd_vll_drom_shadow_pass) &&
	    	   ((dde_vll_drom_data.reptype == DcOutlines)  ||
		    (dde_vll_drom_data.reptype == DcWireframe) ||
		    (dde_vll_drom_data.reptype == DcPoints))) {
		    /* don't render shadows for these reptypes */
		    break;
	    }
	    VLLtriangle_mesh((void *)bndboxpts,vertexcount,
			     (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs,
			     (void *)0, (void *)0, (void *)0,
			     trianglecount, (void *)elenrms,
                             (void *)dre_dynrnd_drom_element_colors,
			     (void *)0, connect, 
			     dde_vll_triangle_mesh_edge_enable, (void *)0);
	} else {
	    /* 
	     * Do the lighting pass for texture mapped triangle mesh.
	     */
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);
	    }
	    switch (dde_vll_drom_data.map_operator) {
	    case DcTextureMultiply:
		VLLtriangle_mesh((void *)bndboxpts,vertexcount,
				 (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs,
				 (void *)0, (void *)0, (void *)0,
				 trianglecount, (void *)elenrms,
                                 (void *)dre_dynrnd_drom_element_colors,
				 (void *)0, connect, 
				 dde_vll_triangle_mesh_edge_enable,
				 (void *)0);
		break;
	    case DcTextureReplace:
	    default:
		VLLset_diffuse_color(white);
		VLLtriangle_mesh((void *)bndboxpts,vertexcount,
				 (void *)vtxlocs, (void *)vtxnrms, (void *)0,
				 (void *)0, (void *)0, (void *)0,
				 trianglecount, (void *)elenrms,
                                 (void *)dre_dynrnd_drom_element_colors,
				 (void *)0, connect, 
				 dde_vll_triangle_mesh_edge_enable,
				 (void *)0);
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
		break;
	    }
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_STANDARD);
	    }
	}
    }
}
/*
 ======================================================================
 */
void ddr_vll_drom_render_triangle_strip (colormodel,bndboxpts, 
	vertexcount, vtxlocs,vtxnrms,vtxclrs, elenrms, 
	uv_count,uv_list, uvw_count,uvw_list)
DtColorModel colormodel;
DtRealTriple bndboxpts[8];
DtInt vertexcount;
DtRealTriple *vtxlocs;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;
DtRealTriple *elenrms;
DtInt uv_count;
DtRealCouple *uv_list[];
DtInt uvw_count;
DtRealTriple *uvw_list[];
{
    int idx;
    static float white[3] = {1.,1.,1.};
    static float black[3] = {0.,0.,0.};
    int texture_mapping;
    DtInt tricount;

    /*
     * Determine what kind of texture mapping, if any, that we are doing.
     */
    texture_mapping = 0;
    if (dde_vll_drom_data.map_diffuse_color) {
	if (uv_count > 0 && dde_vll_drom_data.texture_2D_valid)
		texture_mapping = 1;
	else if (uvw_count > 0 && dde_vll_drom_data.texture_3D_valid)
		texture_mapping = 2;
    } else if (dde_vll_drom_data.map_env &&
	       dde_vll_drom_data.texture_env_valid) {
	texture_mapping = 3;
    }

    /* VLL wants the number of triangles */
    tricount = vertexcount - 2;

    if (texture_mapping) {
        /*
         * This primitive can and should be texture mapped.   Determine
         * whether we know this is texture pass:  if we don't then
         * abort the pass; if we do, continue.  In either case, indicate
         * that we have texture mapping to do.
         */
	if (ddd_vll_drom_texture_pass != dde_vll_drom_data.traversal_type &&
	    !dde_vll_drom_data.have_textures) {

	    dde_vll_drom_data.have_textures = DcTrue;
	    DsExecutionAbort();
	    return;
	} 

	dde_vll_drom_data.have_textures = DcTrue;
    }

    switch (dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_texture_pass:
	switch (texture_mapping) {
	case 0:			/* NONE */
	    break;

	case 1:			/* 2D Texture Map */
	    idx = uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		    dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLtriangle_strip(tricount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, 
			     (void *)vtxnrms, (void *)0, (void *)0, 
			     (void *)0, (void *)0, 
			     (void *)uv_list[idx], (void *)0, 
			     dde_vll_triangle_strip_edge_enable,
			     (void *)dde_vll_triangle_strip_bboxuv);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;
	case 2:			/* 3D Texture Map */
	    idx = uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
		    dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLtriangle_strip(tricount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, 
			     (void *)vtxnrms, (void *)0, (void *)0,
			     (void *)0, (void *)0, (void *)uvw_list[idx], 
			     (void *)0, dde_vll_triangle_strip_edge_enable,
			     (void *)dde_vll_triangle_strip_bboxuvw);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;
	case 3:			/* Environment Map */
	    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
	    VLLtriangle_strip(tricount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, 
			     (void *)vtxnrms, (void *)0,
			     (void *)0, (void *)0, (void *)0,
			     (void *)0, (void *)0,
			     dde_vll_triangle_strip_edge_enable,
			     (void *)0);
	    break;
	}
	break;

    case ddd_vll_drom_transp_pass:
	/* 
	 * Cannot do both texture mapping and transparency, so texture
	 * mapping wins.
	 */
	if (texture_mapping) {
	    break;
	} else {
	    VLLtriangle_strip(tricount, (void *)bndboxpts,
                             (void *)vtxlocs, (void *)elenrms,
                             (void *)vtxnrms, (void *)0, (void *)vtxclrs,
                             (void *)0, (void *)0, (void *)0, (void *)0,
                             dde_vll_triangle_strip_edge_enable,
                             (void *)0);
	}
	break;

    case ddd_vll_drom_standard_pass:
        /*
         * Fall into the default code if we're texture mapping, regardless of
         * transparency, as this is now the second pass.  Otherwise, a
         * standard pass with transparency shouldn't happen, so break.
         */
	if ((dde_vll_drom_data.transparent_switch) &&
            !(dde_vll_drom_data.map_diffuse_color || dde_vll_drom_data.map_env)) {
                break;
        }

    default:
	if (!texture_mapping) {
	    /*
	     * Do standard pass.
	     */
	    if ((dde_vll_drom_data.traversal_type == ddd_vll_drom_shadow_pass) &&
	    	   ((dde_vll_drom_data.reptype == DcOutlines)  ||
		    (dde_vll_drom_data.reptype == DcWireframe) ||
		    (dde_vll_drom_data.reptype == DcPoints))) {
		    /* don't render shadows for these reptypes */
		    break;
	    }
	    VLLtriangle_strip(tricount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, 
			     (void *)vtxnrms, (void *)0, (void *)vtxclrs,
			     (void *)0, (void *)0, (void *)0, (void *)0, 
			     dde_vll_triangle_strip_edge_enable, 
			     (void *)0); 
	} else { 
	    /*
             * Do the lighting pass for texture mapped triangle strip.
             */
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);
	    }
	    switch (dde_vll_drom_data.map_operator) {
	    case DcTextureMultiply:
		VLLtriangle_strip(tricount, (void *)bndboxpts,
				 (void *)vtxlocs, (void *)elenrms,
				 (void *)vtxnrms, (void *)0, (void *)vtxclrs,
				 (void *)0, (void *)0, (void *)0, (void *)0,
				 dde_vll_triangle_strip_edge_enable,
				 (void *)0);
		break;
	    case DcTextureReplace:
	    default:
		VLLset_diffuse_color(white);
		VLLtriangle_strip(tricount, (void *)bndboxpts,
				 (void *)vtxlocs, (void *)elenrms, 
				 (void *)vtxnrms, (void *)0,
				 (void *)0, (void *)0, (void *)0,
				 (void *)0, (void *)0, 
				 dde_vll_triangle_strip_edge_enable,
				 (void *)0);
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
		break;
	    }
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_STANDARD);
	    }
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_render_quad_list(colormodel,bndboxpts,quadcount,elenrms,
	vtxlocs,vtxnrms,vtxclrs, uv_count,uv_list, uvw_count,uvw_list)
DtColorModel colormodel;
DtRealTriple bndboxpts[8];
DtInt quadcount;
DtRealTriple *elenrms;
DtRealTriple *vtxlocs;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;
DtInt uv_count;
DtRealCouple *uv_list[];
DtInt uvw_count;
DtRealTriple *uvw_list[];
{
    int idx;
    static float white[3] = {1.,1.,1.};
    static float black[3] = {0.,0.,0.};
    int texture_mapping;

    /*
     * Determine what kind of texture mapping, if any, that we are doing.
     */
    texture_mapping = 0;
    if (dde_vll_drom_data.map_diffuse_color) {
	if (uv_count > 0 && dde_vll_drom_data.texture_2D_valid)
		texture_mapping = 1;
	else if (uvw_count > 0 && dde_vll_drom_data.texture_3D_valid)
		texture_mapping = 2;
    } else if (dde_vll_drom_data.map_env &&
	       dde_vll_drom_data.texture_env_valid) {
	texture_mapping = 3;
    }

    if (texture_mapping) {
	/* 
         * This primitive can and should be texture mapped.   Determine
         * whether we know this is texture pass:  if we don't then
         * abort the pass; if we do, continue.  In either case, indicate
         * that we have texture mapping to do.
	 */
	if (ddd_vll_drom_texture_pass != dde_vll_drom_data.traversal_type &&
	    !dde_vll_drom_data.have_textures) {

	    dde_vll_drom_data.have_textures = DcTrue;
	    DsExecutionAbort();
	    return;
	} 

	dde_vll_drom_data.have_textures = DcTrue;
    }

    switch (dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_texture_pass:
	switch (texture_mapping) {
	case 0:			/* NONE */
	    break;

	case 1:			/* 2D Texture Map */
	    idx = uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		    dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLquad_list(quadcount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)0, (void *)0, (void *)0, (void *)0, 
			     (void *)uv_list[idx], (void *)0, 
			     dde_vll_quad_list_edge_enable,
			     (void *)dde_vll_quad_list_bboxuv);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;
	case 2:			/* 3D Texture Map */
	    idx = uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
		    dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLquad_list(quadcount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)0, (void *)0, (void *)0, (void *)0, 
			     (void *)uvw_list[idx], (void *)0, 
			     dde_vll_quad_list_edge_enable,
			     (void *)dde_vll_quad_list_bboxuvw);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;
	case 3:			/* Environment Map */
	    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
	    VLLquad_list(quadcount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)0, (void *)0, (void *)0, (void *)0,
			     (void *)0, (void *)0,
			     dde_vll_quad_list_edge_enable, (void *)0);
	    break;
	}
	break;

    case ddd_vll_drom_transp_pass:
	/* 
	 * Cannot do both texture mapping and transparency, so texture 
	 * mapping wins.
	 */
	if (texture_mapping) {
	    break;
	} else {
	    VLLquad_list(quadcount, (void *)bndboxpts,
                             (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
                             (void *)0, (void *)vtxclrs, (void *)0, (void *)0,
                             (void *)0, (void *)0,
                             dde_vll_quad_list_edge_enable, (void *)0);
	}
	break;

    case ddd_vll_drom_standard_pass:
        /*
         * Fall into the default code if we're texture mapping, regardless of
         * transparency, as this is now the second pass.  Otherwise, a
         * standard pass with transparency shouldn't happen, so break.
         */
        if ((dde_vll_drom_data.transparent_switch) &&
            !(dde_vll_drom_data.map_diffuse_color || dde_vll_drom_data.map_env)) {
                break;
        }

    default:
	if (!texture_mapping) {
            /*
             * Do standard pass.
             */
	    if ((dde_vll_drom_data.traversal_type == ddd_vll_drom_shadow_pass) &&
	    	   ((dde_vll_drom_data.reptype == DcOutlines)  ||
		    (dde_vll_drom_data.reptype == DcWireframe) ||
		    (dde_vll_drom_data.reptype == DcPoints))) {
		    /* don't render shadows for these reptypes */
		    break;
	    }
	    VLLquad_list(quadcount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)0, (void *)vtxclrs, (void *)0, (void *)0,
			     (void *)0, (void *)0,
			     dde_vll_quad_list_edge_enable, (void *)0);
	} else {
            /*
             * Do the lighting pass for texture mapped quad list.
             */
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);
	    }
	    switch (dde_vll_drom_data.map_operator) {
	    case DcTextureMultiply:
	        VLLquad_list(quadcount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)0, (void *)vtxclrs, (void *)0, (void *)0,
			     (void *)0, (void *)0,
			     dde_vll_quad_list_edge_enable, 
			     (void *)0);
		break;
	    case DcTextureReplace:
	    default:
		VLLset_diffuse_color(white);
	        VLLquad_list(quadcount, (void *)bndboxpts,
			     (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms, 
			     (void *)0, (void *)0, (void *)0, (void *)0,
			     (void *)0, (void *)0, 
			     dde_vll_quad_list_edge_enable,
			     (void *)0);
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
		break;
	    }
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_STANDARD);
	    }
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_render_quad_mesh(colormodel,bndboxpts,quadcount,elenrms, vertexcount, 
	vtxlocs,vtxnrms,vtxclrs,vertexlist, compiledvertexlist, uv_count, uv_list, 
	uvw_count, uvw_list)
DtColorModel colormodel;
DtRealTriple bndboxpts[8];
DtInt quadcount;
DtRealTriple *elenrms;
DtInt vertexcount;
DtRealTriple *vtxlocs;
DtRealTriple *vtxnrms;
DtRealTriple *vtxclrs;
DtInt vertexlist[][4];
DtInt compiledvertexlist[][4];
DtInt uv_count;
DtRealCouple *uv_list[];
DtInt uvw_count;
DtRealTriple *uvw_list[];
{
    int idx;
    DtInt *connect[3];
    static float white[3] = {1.,1.,1.};
    static float black[3] = {0.,0.,0.};
    int texture_mapping;

    connect[0] = (DtInt *)vertexlist;
    connect[1] = (DtInt *)compiledvertexlist;
    connect[2] = (DtInt *)dde_vll_quad_mesh_uvvertexlist;

    /*
     * Determine what kind of texture mapping, if any, that we are doing.
     */
    texture_mapping = 0;
    if (dde_vll_drom_data.map_diffuse_color) {
	if (uv_count > 0 && dde_vll_drom_data.texture_2D_valid)
		texture_mapping = 1;
	else if (uvw_count > 0 && dde_vll_drom_data.texture_3D_valid)
		texture_mapping = 2;
    } else if (dde_vll_drom_data.map_env &&
	       dde_vll_drom_data.texture_env_valid) {
	texture_mapping = 3;
    }

    if (texture_mapping) {
	/* 
         * This primitive can and should be texture mapped.   Determine
         * whether we know this is texture pass:  if we don't then
         * abort the pass; if we do, continue.  In either case, indicate
         * that we have texture mapping to do.
	 */
	if (ddd_vll_drom_texture_pass != dde_vll_drom_data.traversal_type &&
	    !dde_vll_drom_data.have_textures) {

	    dde_vll_drom_data.have_textures = DcTrue;
	    DsExecutionAbort();
	    return;
	} 

	dde_vll_drom_data.have_textures = DcTrue;
    }

    switch (dde_vll_drom_data.traversal_type) {
    case ddd_vll_drom_texture_pass:
	switch (texture_mapping) {
	case 0:			/* NONE */
	    break;

	case 1:			/* 2D Texture Map */
	    idx = uv_count > dde_vll_drom_data.map_diffuse_color_uv_index ?
		    dde_vll_drom_data.map_diffuse_color_uv_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLquad_mesh((void *)bndboxpts,vertexcount,
			     (void *)vtxlocs, (void *)vtxnrms, (void *)0,
			     (void *)0, (void *)uv_list[idx], (void *)0, 
			     quadcount, (void *)elenrms, (void *)0,
			     (void *)0, connect, 
			     dde_vll_quad_mesh_edge_enable,
			     (void *)dde_vll_quad_mesh_bboxuv);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;
	case 2:			/* 3D Texture Map */
	    idx = uvw_count > dde_vll_drom_data.map_diffuse_color_uvw_index ?
		    dde_vll_drom_data.map_diffuse_color_uvw_index : 0;

	    VLLset_render_mode(VLL_RENDER_TEXTURE_UV);
	    if (dde_vll_drom_data.map_extend_black) {
		VLLset_diffuse_color(black);
	    } else if (dde_vll_drom_data.map_operator == DcTextureMultiply &&
		       vtxclrs != (DtRealTriple *)0) {
		VLLset_diffuse_color(white);
	    }
	    VLLquad_mesh((void *)bndboxpts,vertexcount,
			     (void *)vtxlocs, (void *)vtxnrms, (void *)0, 
			     (void *)0, (void *)uvw_list[idx], (void *)0, 
			     quadcount, (void *)elenrms, (void *)0,
			     (void *)0, connect, 
			     dde_vll_quad_mesh_edge_enable,
			     (void *)dde_vll_quad_mesh_bboxuvw);
	    if (dde_vll_drom_data.map_extend_black ||
		(dde_vll_drom_data.map_operator == DcTextureMultiply &&
		 vtxclrs != (DtRealTriple *)0)) {
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
	    }
	    break;
	case 3:			/* Environment Map */
	    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
	    VLLquad_mesh((void *)bndboxpts,vertexcount,
			     (void *)vtxlocs, (void *)vtxnrms, (void *)0,
			     (void *)0, (void *)0, (void *)0,
			     quadcount, (void *)elenrms, (void *)0,
			     (void *)0, connect, 
			     dde_vll_quad_mesh_edge_enable,
			     (void *)0);
	    break;
	}
	break;

    case ddd_vll_drom_transp_pass:
	/* 
	 * Cannot do both texture mapping and transparency, so texture 
	 * mapping wins.
	 */
	if (texture_mapping) {
	    break;
	} else {
	    VLLquad_mesh((void *)bndboxpts,vertexcount,
                             (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs,
                             (void *)0, (void *)0, (void *)0,
                             quadcount, (void *)elenrms, (void *)0,
                             (void *)0, connect,
                             dde_vll_quad_mesh_edge_enable, (void *)0);
	}
	break;

    case ddd_vll_drom_standard_pass:
        /*
         * Fall into the default code if we're texture mapping, regardless of
         * transparency, as this is now the second pass.  Otherwise, a
         * standard pass with transparency shouldn't happen, so break.
         */
        if ((dde_vll_drom_data.transparent_switch) &&
            !(dde_vll_drom_data.map_diffuse_color || dde_vll_drom_data.map_env)) {
                break;
        }

    default:
	if (!texture_mapping) {
            /*
             * Do standard pass.
             */
	    if ((dde_vll_drom_data.traversal_type == ddd_vll_drom_shadow_pass) &&
	    	   ((dde_vll_drom_data.reptype == DcOutlines)  ||
		    (dde_vll_drom_data.reptype == DcWireframe) ||
		    (dde_vll_drom_data.reptype == DcPoints))) {
		    /* don't render shadows for these reptypes */
		    break;
	    }
	    VLLquad_mesh((void *)bndboxpts,vertexcount,
			     (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs,
			     (void *)0, (void *)0, (void *)0,
			     quadcount, (void *)elenrms, (void *)0,
			     (void *)0, connect, 
			     dde_vll_quad_mesh_edge_enable, (void *)0);
	} else {
            /*
             * Do the lighting pass for texture mapped quad mesh.
             */
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);
	    }
	    switch (dde_vll_drom_data.map_operator) {
	    case DcTextureMultiply:
		VLLquad_mesh((void *)bndboxpts,vertexcount,
				 (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs,
				 (void *)0, (void *)0, (void *)0,
				 quadcount, (void *)elenrms, (void *)0,
				 (void *)0, connect, 
				 dde_vll_quad_mesh_edge_enable,
				 (void *)0);
		break;
	    case DcTextureReplace:
	    default:
		VLLset_diffuse_color(white);
		VLLquad_mesh((void *)bndboxpts,vertexcount,
				 (void *)vtxlocs, (void *)vtxnrms, (void *)0,
				 (void *)0, (void *)0, (void *)0,
				 quadcount, (void *)elenrms, (void *)0,
				 (void *)0, connect, 
				 dde_vll_quad_mesh_edge_enable,
				 (void *)0);
		VLLset_diffuse_color(dde_vll_drom_data.diffuse_color);
		break;
	    }
	    if (ddd_vll_drom_standard_pass == dde_vll_drom_data.traversal_type) {
		VLLset_render_mode(VLL_RENDER_STANDARD);
	    }
	}
    }
}
