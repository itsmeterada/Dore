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
	ddr_g4vll_drom_render_point_list
	ddr_g4vll_drom_render_line_list
	ddr_g4vll_drom_render_connected_line_list
	ddr_g4vll_drom_render_triangle_list
	ddr_g4vll_drom_render_triangle_mesh
	ddr_g4vll_drom_render_triangle_trip
	ddr_g4vll_drom_render_quad_list
	ddr_g4vll_drom_render_quad_grid

  ======================================================================
 */
#include "drom.h"

DtInt *dde_g4vll_triangle_mesh_uvvertexlist = (DtInt *)0;
DtRealCouple *dde_g4vll_triangle_mesh_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_g4vll_triangle_mesh_bboxuvw = (DtRealTriple *)0;
DtInt *dde_g4vll_triangle_mesh_edge_enable = (DtInt *)0;

DtRealCouple *dde_g4vll_triangle_list_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_g4vll_triangle_list_bboxuvw = (DtRealTriple *)0;
DtInt *dde_g4vll_triangle_list_edge_enable = (DtInt *)0;

DtRealCouple *dde_g4vll_triangle_strip_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_g4vll_triangle_strip_bboxuvw = (DtRealTriple *)0;
DtInt *dde_g4vll_triangle_strip_edge_enable = (DtInt *)0;

DtRealCouple *dde_g4vll_quad_list_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_g4vll_quad_list_bboxuvw = (DtRealTriple *)0;
DtInt *dde_g4vll_quad_list_edge_enable = (DtInt *)0;

DtRealCouple *dde_g4vll_quad_grid_bboxuv = (DtRealCouple *)0;
DtRealTriple *dde_g4vll_quad_grid_bboxuvw = (DtRealTriple *)0;
DtInt *dde_g4vll_quad_grid_edge_enable = (DtInt *)0;

/* may be used by packages to set element colors for geometry */
extern DtRealTriple *dre_dynrnd_drom_element_colors;

    /*** Function Declarations ***/

static int      ddr_g4vll_drom_query_texmapping_type (DtInt, DtInt);
static void     ddr_g4vll_drom_find_required_texture_passes (DtRealTriple *);
static DtObject ddr_g4vll_drom_query_shader_status (void);
static int      ddr_g4vll_drom_valid_transp_with_texture(void);
static DtFlag   ddr_g4vll_drom_transp_without_texture(DtInt, DtInt);

/* ====================================================================== */


void ddr_g4vll_drom_render_point_list (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         pointcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,
    DtRealCouple *uv_list[],
    DtInt         uvw_count,
    DtRealTriple *uvw_list[],
    DtReal       *vtxalpha)
{
    switch(dde_g4vll_drom_data.traversal_type) {
    case ddd_g4vll_drom_texture_pass:
    case ddd_g4vll_drom_shadow_pass:
	/* can't do points in these modes */
	break;

    default:
	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_standard_pass)
	{   dde_g4vll_drom_data.have_opaque_objects = DcTrue;
	}
	VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,bndboxpts);
	VLLset_clip_status(dde_g4vll_drom_data.clip_status);
	VLLpoint_list
	    (pointcount, (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs);
	break;
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_render_line_list (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         linecount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,
    DtRealCouple *uv_list[],
    DtInt         uvw_count,
    DtRealTriple *uvw_list[],
    DtReal       *vtxalpha)
{
    switch(dde_g4vll_drom_data.traversal_type) {
    case ddd_g4vll_drom_texture_pass:
    case ddd_g4vll_drom_shadow_pass:
	/* can't do lines in these modes */
	break;

    default:
	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_standard_pass) {
	    dde_g4vll_drom_data.have_opaque_objects = DcTrue;
	}
	VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,bndboxpts);
	VLLset_clip_status(dde_g4vll_drom_data.clip_status);
	VLLline_list(linecount, (void *)vtxlocs,
		     (void *)elenrms, (void *)vtxnrms, (void *)0, (void *)vtxclrs);
	break;
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_render_connected_line_list (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         vertexcount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,
    DtRealCouple *uv_list[],
    DtInt         uvw_count,
    DtRealTriple *uvw_list[],
    DtReal       *vtxalpha)
{
#ifdef DEBUG
    int i;
    printf ("ddr_g4vll_drom_render_connected_line_list (%d lines)\n",vertexcount-1);
    for (i=0; i<vertexcount; i++) {
	printf ("\t%g %g %g\n", vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
	printf ("\t%g %g %g\n", vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
	printf ("\t%g %g %g\n", vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
    	printf ("\n");
    }
    printf ("\n");
#endif

    switch(dde_g4vll_drom_data.traversal_type) {
    case ddd_g4vll_drom_texture_pass:
    case ddd_g4vll_drom_shadow_pass:
	/* can't do lines in these modes */
	break;

    default:
	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_standard_pass) {
	    dde_g4vll_drom_data.have_opaque_objects = DcTrue;
	}
	VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,bndboxpts);
	VLLset_clip_status(dde_g4vll_drom_data.clip_status);
	VLLpoly_line(vertexcount-1, (void *)vtxlocs,
		     (void *)elenrms, (void *)vtxnrms, (void *)0, (void *)vtxclrs);
	break;
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_render_triangle_list (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         trianglecount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,
    DtRealCouple *uv_list[],
    DtInt         uvw_count,
    DtRealTriple *uvw_list[],
    DtReal       *vtxalpha)
{
    int idx;
    static float black[3] = {0.,0.,0.};
    int texture_mapping;
    int render_mode;

#ifdef DEBUG
    int i;
    printf ("ddr_g4vll_drom_render_triangle_list (%d triangles)\n",trianglecount);
    for (i=0; i<trianglecount; i++) {
	printf ("\t%g %g %g\n", vtxlocs[i*3][0],vtxlocs[i*3][1],vtxlocs[i*3][2]);
	printf ("\t%g %g %g\n", vtxlocs[i*3+1][0],vtxlocs[i*3+1][1],vtxlocs[i*3+1][2]);
	printf ("\t%g %g %g\n", vtxlocs[i*3+2][0],vtxlocs[i*3+2][1],vtxlocs[i*3+2][2]);
    	printf ("\n");
    }
    printf ("\n");
#endif

    /*
     * NOTE:  We do not prevent the user from doing texture mapping and/or
     * transparency when the reptype is not DcSurface, although this is the only
     * reptype supported in these cases.  We _do_ disable local antialiasing as
     * it seems to corrupt textures that are drawn on even non-local-antialiased
     * primitives.
     */

    switch (dde_g4vll_drom_data.traversal_type) {
	case ddd_g4vll_drom_texture_pass: {
	    texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
			uvw_count);
	    switch (texture_mapping) {
		case ddd_g4vll_drom_texmap_none: {
                    /*
                     * Texture mapping was requested, but no uv/w's were
                     * supplied, or the texture isn't valid
                     */
                    if (dde_g4vll_drom_data.transparent_switch) {

                        /* draw later in the transparency pass */
                        dde_g4vll_drom_data.have_transparency = DcTrue;
                    } else {
                        /* just draw opaque */
                
                        dde_g4vll_drom_data.have_opaque_objects = DcTrue;

                        VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
                        VLLset_clip_status(dde_g4vll_drom_data.clip_status);

                        render_mode = VLLquery_render_mode();
                        VLLset_render_mode(VLL_RENDER_STANDARD);

                        VLLtriangle_list(trianglecount,
                            (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
                            (void *)dre_dynrnd_drom_element_colors, (void *)vtxclrs,
                            (void *)0, (void *)vtxalpha, (void *)0,
                            dde_g4vll_triangle_list_edge_enable);

                        VLLset_render_mode(render_mode);
                    }
		} break;

		case ddd_g4vll_drom_texmap_2d:
		case ddd_g4vll_drom_texmap_3d: {
		    if (dde_g4vll_drom_data.current_texpass_mode ==
                        	ddd_g4vll_drom_texpass_draw) {
			if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
	    	    	    idx = uv_count >
			    	dde_g4vll_drom_data.map_diffuse_color_uv_index ?
		    	    	dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
			} else {
	    	    	    idx = uvw_count >
			    	dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
		    	    	dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
			}

	    	    	VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

	    	    	ddr_g4vll_drom_find_required_texture_passes(vtxclrs);
	    	    	if (ddr_g4vll_drom_valid_transp_with_texture()) {
			    /*
		 	     * Mark this primitive as one to do during the
		 	     * transp pass(es) instead of the texture pass(es).
		 	     */
#ifdef DEBUG_FLOW
printf ("\trender textured trilst during transparent texture passes\n");
#endif
			    dde_g4vll_drom_data.have_transp_textured_objects = DcTrue;
	    	    	} else {
			    /*
		 	     * Render this primitive during the texture pass(es)
		 	     * and not during the transparent texture pass(es).
		 	     */
	    		    dde_g4vll_drom_data.have_opaque_objects = DcTrue;
#ifdef DEBUG_FLOW
printf ("\trender textured trilst now (nontransparent texture pass)\n");
#endif

	    		    VLLcompute_clip_status(
				dde_g4vll_drom_data.clip_status, bndboxpts);
			    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     *
                                     * If the primitive has vertex colours, VLL
                                     * overrides the diffuse colour and renders
                                     * a white lit primitive in this pass, and
                                     * then does a multiply with the vertex
                                     * colours only in the extend area in the
                                     * vertex colour pass.
                                     */

                                    VLLset_diffuse_color(black);
	    		    	    VLLtriangle_list(trianglecount,
			    	    	(void*)vtxlocs, (void*)elenrms,
				    	(void*)vtxnrms,
				    	(void*)dre_dynrnd_drom_element_colors,
				    	(void*)0, (void*)0, (void*)vtxalpha,
				    	(void*)uv_list[idx],
				    	dde_g4vll_triangle_list_edge_enable);
				    VLLset_diffuse_color(
					dde_g4vll_drom_data.diffuse_color);
				} else {
	    		    	    VLLtriangle_list(trianglecount,
			    	    	(void*)vtxlocs, (void*)elenrms,
				    	(void*)vtxnrms,
				    	(void*)dre_dynrnd_drom_element_colors,
				    	(void*)vtxclrs, (void*)0, (void*)vtxalpha,
				    	(void*)uv_list[idx],
				    	dde_g4vll_triangle_list_edge_enable);
				}
			    } else /* ddd_g4vll_drom_texmap_3d */ {
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
	    		            VLLtriangle_list(trianglecount,
			    	    	(void*)vtxlocs, (void *)elenrms,
				    	(void*)vtxnrms,
				    	(void*)dre_dynrnd_drom_element_colors,
				    	(void*)0, (void*)0, (void*)vtxalpha,
				    	(void*)uvw_list[idx],
			    	    	dde_g4vll_triangle_list_edge_enable);
				    VLLset_diffuse_color(
					dde_g4vll_drom_data.diffuse_color);
				} else {
	    		            VLLtriangle_list(trianglecount,
			    	    	(void*)vtxlocs, (void *)elenrms,
				    	(void*)vtxnrms,
				    	(void*)dre_dynrnd_drom_element_colors,
				    	(void*)vtxclrs, (void*)0, (void*)vtxalpha,
				    	(void*)uvw_list[idx],
			    	    	dde_g4vll_triangle_list_edge_enable);
				}
			    }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);
	    	    	}
		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
				ddd_g4vll_drom_texpass_light) &&
				!ddr_g4vll_drom_valid_transp_with_texture()) {
			VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

			switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
                        	VLLcompute_clip_status(
				    dde_g4vll_drom_data.clip_status,
                                    bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

			        if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
                        	    VLLtriangle_list(trianglecount,
                                    	(void*)vtxlocs, (void *)elenrms,
				    	(void*)vtxnrms,
				    	(void*)dre_dynrnd_drom_element_colors,
				    	(void*)0,
				    	(void*)0, (void*)vtxalpha, (void*)0,
				    	dde_g4vll_triangle_list_edge_enable);
                        	    VLLset_diffuse_color(
				    	dde_g4vll_drom_data.diffuse_color);
                                } else {
                        	    VLLtriangle_list(trianglecount,
                                    	(void*)vtxlocs, (void *)elenrms,
				    	(void*)vtxnrms,
				    	(void*)dre_dynrnd_drom_element_colors,
				    	(void*)vtxclrs,
				    	(void*)0, (void*)vtxalpha, (void*)0,
				    	dde_g4vll_triangle_list_edge_enable);
				}
			        if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
                            break;
                    	}

		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
			    ddd_g4vll_drom_texpass_vcol) &&
			    ddr_g4vll_drom_do_vertex_color_texture_pass(
				vtxclrs, texture_mapping) &&
	    	    	    !ddr_g4vll_drom_valid_transp_with_texture()) {

			VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);
                    	switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
                        	VLLcompute_clip_status(
				    dde_g4vll_drom_data.clip_status, bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

			        if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

                        	VLLtriangle_list(trianglecount,
                            	    (void *)vtxlocs, (void *)elenrms,
				    (void *)vtxnrms,
				    (void *)dre_dynrnd_drom_element_colors,
				    (void *)vtxclrs,
				    (void *)0, (void *)vtxalpha, (void *)0,
				    dde_g4vll_triangle_list_edge_enable);

			        if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
			    break;
                    	}
		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
			    ddd_g4vll_drom_texpass_spec) &&
                            (dde_g4vll_drom_data.specular_switch) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
	    	    	       !ddr_g4vll_drom_valid_transp_with_texture()) {
			VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                    	switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
				if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
                                }
                        	VLLcompute_clip_status(
				    dde_g4vll_drom_data.clip_status, bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

			        if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

                        	VLLtriangle_list(trianglecount,
                            	    (void *)vtxlocs, (void *)elenrms,
				    (void *)vtxnrms,
				    (void *)dre_dynrnd_drom_element_colors,
				    (void *)vtxclrs,
				    (void *)0, (void *)vtxalpha, (void *)0,
				    dde_g4vll_triangle_list_edge_enable);

			        if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);

				if (dde_g4vll_drom_data.map_extend_black) {
                        	    VLLset_diffuse_color(
				    	dde_g4vll_drom_data.diffuse_color);
				}
                            break;
                    	}
		    }
		} break;

		case ddd_g4vll_drom_texmap_env: {
	    	    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
	    	    VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
						bndboxpts);
		    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(0);

	    	    VLLtriangle_list(trianglecount,
			    (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
			    (void *)dre_dynrnd_drom_element_colors, (void *)vtxclrs,
			    (void *)0, (void *)vtxalpha, (void *)0,
			    dde_g4vll_triangle_list_edge_enable);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(1);
		} break;
	    }
    	} break;

    	case ddd_g4vll_drom_transp_pass: {
	    /*
	     * Determine whether this object should be drawn during this
	     * pass (it may have been drawn in the opaque or texture passes).
	     */
	    if (ddr_g4vll_drom_transp_without_texture(uv_count, uvw_count)) {
#ifdef DEBUG_FLOW
printf ("\ttransparency pass, render trilst\n");
#endif
	        VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
	                               bndboxpts);
	        VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(0);

	        VLLtriangle_list(trianglecount, (void *)vtxlocs,
			(void *)elenrms, (void *)vtxnrms,
			(void *)dre_dynrnd_drom_element_colors,
			(void *)vtxclrs, (void *)0, (void *)vtxalpha, (void *)0,
                    	dde_g4vll_triangle_list_edge_enable);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(1);
	    }
	} break;

	case ddd_g4vll_drom_transp_texture_pass: {
#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass ...\n");
#endif
	    texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
				uvw_count);
	    switch (texture_mapping) {
		case ddd_g4vll_drom_texmap_none: {
		} break;

		case ddd_g4vll_drom_texmap_2d:
		case ddd_g4vll_drom_texmap_3d: {
		    /*
		     * Even though executability hasn't been turned off for
		     * this primitive, it is still possible that it was
		     * rendered during the opaque texture pass.  Check to
		     * see if it can be both transparent and textured.
		     */
	    	    if (ddr_g4vll_drom_valid_transp_with_texture()) {
			if (dde_g4vll_drom_data.current_texpass_mode ==
				ddd_g4vll_drom_texpass_draw) {
			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
	    	    	    	idx = uv_count >
				    dde_g4vll_drom_data.map_diffuse_color_uv_index ?
		    		    dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
			    } else {
	    	    	    	idx = uvw_count >
				    dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
		    		    dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
			    }

	    	    	    VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass, render trilst\n");
#endif

	    	    	    VLLcompute_clip_status(
				dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
		    		VLLset_line_filter(0);

			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     */
                                    VLLset_diffuse_color(black);
	    	    	    	    VLLtriangle_list(trianglecount,
			    	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)0, (void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
			    	    	dde_g4vll_triangle_list_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
				} else {
	    	    	    	    VLLtriangle_list(trianglecount,
			    	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)vtxclrs, (void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
			    	    	dde_g4vll_triangle_list_edge_enable);
				}
			    } else /* ddd_g4vll_drom_texmap_3d */ {
				if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
	    	    	    	    VLLtriangle_list(trianglecount,
			    	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)0, (void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
			    	    	dde_g4vll_triangle_list_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
	    	    	    	    VLLtriangle_list(trianglecount,
			    	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)vtxclrs, (void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
			    	    	dde_g4vll_triangle_list_edge_enable);
				}
			    }
			    if (dde_g4vll_drom_data.localaa_switch)
		    		VLLset_line_filter(1);

		        } else if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_light) {

			    VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status,
                                        bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

			    	    if (dde_g4vll_drom_data.localaa_switch)
		    			VLLset_line_filter(0);

				    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
                                    	VLLtriangle_list(trianglecount,
                                            (void *)vtxlocs, (void *)elenrms,
                                            (void *)vtxnrms,
					    (void *)dre_dynrnd_drom_element_colors,
					    (void *)0,
                                            (void *)0, (void *)vtxalpha, (void *)0,
                                            dde_g4vll_triangle_list_edge_enable);
                                    	VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
                                    } else {
                                    	VLLtriangle_list(trianglecount,
                                            (void *)vtxlocs, (void *)elenrms,
                                            (void *)vtxnrms,
					    (void *)dre_dynrnd_drom_element_colors,
					    (void *)vtxclrs,
                                            (void *)0, (void *)vtxalpha, (void *)0,
                                            dde_g4vll_triangle_list_edge_enable);
				    }
			    	    if (dde_g4vll_drom_data.localaa_switch)
		    			VLLset_line_filter(1);
                                break;
                            }

                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_vcol) &&
				ddr_g4vll_drom_do_vertex_color_texture_pass(
                                	vtxclrs, texture_mapping)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

			    	    if (dde_g4vll_drom_data.localaa_switch)
		    			VLLset_line_filter(0);

                                    VLLtriangle_list(trianglecount,
                                        (void *)vtxlocs, (void *)elenrms,
                                        (void *)vtxnrms,
					(void *)dre_dynrnd_drom_element_colors,
					(void *)vtxclrs,
                                        (void *)0, (void *)vtxalpha, (void *)0,
                                        dde_g4vll_triangle_list_edge_enable);

			    	    if (dde_g4vll_drom_data.localaa_switch)
		    			VLLset_line_filter(1);
                                break;
                            }
                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_spec) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
                            (dde_g4vll_drom_data.specular_switch)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
                                    }
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

			    	    if (dde_g4vll_drom_data.localaa_switch)
		    			VLLset_line_filter(0);

                                    VLLtriangle_list(trianglecount,
                                        (void *)vtxlocs, (void *)elenrms,
                                        (void *)vtxnrms,
					(void *)dre_dynrnd_drom_element_colors,
					(void *)vtxclrs,
                                        (void *)0, (void *)vtxalpha, (void *)0,
                                        dde_g4vll_triangle_list_edge_enable);

			    	    if (dde_g4vll_drom_data.localaa_switch)
		    			VLLset_line_filter(1);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
                                    }
                                break;
                            }
		        }
		    }
		} break;
	    }
    	} break;

    	case ddd_g4vll_drom_standard_pass:
	default: {
	    if (dde_g4vll_drom_data.map_diffuse_color_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have diffuse map\n");
#endif
                /* if the texture isn't valid or no UV/Ws are 
                   provided, we'll detect it in the texture pass */
		dde_g4vll_drom_data.have_textures = DcTrue;
	    } else if (dde_g4vll_drom_data.map_env_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have env map\n");
#endif
                /* if the texture isn't valid  we'll detect
                   it in the texture pass */
		dde_g4vll_drom_data.have_textures = DcTrue;
		dde_g4vll_drom_data.have_env_mapping = DcTrue;
	    } else if (dde_g4vll_drom_data.transparent_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have transparency\n");
#endif
		dde_g4vll_drom_data.have_transparency = DcTrue;
	    } else {
#ifdef DEBUG_FLOW
printf ("\topaque pass -- draw triangle list\n");
#endif
		/*
		 * Object is opaque in the opaque render pass so draw it.
		 */
	    	dde_g4vll_drom_data.have_opaque_objects = DcTrue;

		if ((dde_g4vll_drom_data.traversal_type ==
			ddd_g4vll_drom_shadow_pass) &&
                   	((dde_g4vll_drom_data.reptype == DcOutlines)  ||
                    	(dde_g4vll_drom_data.reptype == DcWireframe) ||
                    	(dde_g4vll_drom_data.reptype == DcPoints))) {
                    /* don't render shadows for these reptypes */
                    break;
		}
	    	VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
						bndboxpts);
		VLLset_clip_status(dde_g4vll_drom_data.clip_status);
	    	VLLtriangle_list(trianglecount,
			(void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
			(void *)dre_dynrnd_drom_element_colors, (void *)vtxclrs,
			(void *)0, (void *)vtxalpha, (void *)0,
			dde_g4vll_triangle_list_edge_enable);
	    }
	} break;
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_render_triangle_mesh (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         trianglecount,
    DtRealTriple *elenrms,
    DtInt         vertexcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         vertexlist[][3],
    DtInt         compiledvertexlist[][3],
    DtInt         uv_count,
    DtRealCouple *uv_list[],
    DtInt         uvw_count,
    DtRealTriple *uvw_list[],
    DtReal       *vtxalpha)
{
    int idx;
    DtInt *connect[3];
    static float black[3] = {0.,0.,0.};
    int texture_mapping;
    int render_mode;

#ifdef DEBUG
    int i;
    printf ("ddr_g4vll_drom_render_triangle_mesh (%d triangles)\n",trianglecount);
    for (i=0; i<vertexcount; i++) {
        printf ("\t%g %g %g\n", vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
    }
    printf ("\n");
#endif

    /*
     * NOTE:  We do not prevent the user from doing texture mapping and/or
     * transparency when the reptype is not DcSurface, although this is the only
     * reptype supported in these cases.  We _do_ disable local antialiasing as
     * it seems to corrupt textures that are drawn on even non-local-antialiased
     * primitives.
     */

    connect[0] = (DtInt *)vertexlist;
    connect[1] = (DtInt *)compiledvertexlist;
    connect[2] = (DtInt *)dde_g4vll_triangle_mesh_uvvertexlist;

    switch (dde_g4vll_drom_data.traversal_type) {
	case ddd_g4vll_drom_texture_pass: {
	    texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
			uvw_count);
	    switch (texture_mapping) {
		case ddd_g4vll_drom_texmap_none: {
                    /*
                     * Texture mapping was requested, but no uv/w's were
                     * supplied, or the texture isn't valid
                     */
                    if (dde_g4vll_drom_data.transparent_switch) {

                        /* draw later in the transparency pass */
                        dde_g4vll_drom_data.have_transparency = DcTrue;
                    } else {
                        /* draw opaque */

                        dde_g4vll_drom_data.have_opaque_objects = DcTrue;

                        VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
                        VLLset_clip_status(dde_g4vll_drom_data.clip_status);

                        render_mode = VLLquery_render_mode();
                        VLLset_render_mode(VLL_RENDER_STANDARD);

                        VLLtriangle_mesh(vertexcount,
                            (void*)vtxlocs, (void*)vtxnrms,
                            (void*)vtxclrs, (void*)vtxalpha, (void*)0,
                            trianglecount, (void *)elenrms,
                            (void*)dre_dynrnd_drom_element_colors, (void*)0,
                            connect, dde_g4vll_triangle_mesh_edge_enable);

                        VLLset_render_mode(render_mode);
                    }
		} break;

		case ddd_g4vll_drom_texmap_2d:
		case ddd_g4vll_drom_texmap_3d: {
		    if (dde_g4vll_drom_data.current_texpass_mode ==
                        	ddd_g4vll_drom_texpass_draw) {
			if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
	    	    	    idx = uv_count >
			    	dde_g4vll_drom_data.map_diffuse_color_uv_index ?
		    	    	dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
			} else {
	    	    	    idx = uvw_count >
			    	dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
		    	    	dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
			}

	    	    	VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

                        ddr_g4vll_drom_find_required_texture_passes(vtxclrs);
                        if (ddr_g4vll_drom_valid_transp_with_texture()) {
			    /*
		 	     * Mark this primitive as one to do during the
		 	     * transp pass(es) instead of the texture pass(es).
		 	     */
#ifdef DEBUG_FLOW
printf ("\trender textured trimsh during transparent texture passes\n");
#endif
			    dde_g4vll_drom_data.have_transp_textured_objects =
					 DcTrue;
	    	    	} else {
			    /*
		 	     * Render this primitive during the texture pass(es)
		 	     * and not during the transparent texture pass(es).
		 	     */
	    		    dde_g4vll_drom_data.have_opaque_objects = DcTrue;
#ifdef DEBUG_FLOW
printf ("\trender textured trimsh now (nontransparent texture pass)\n");
#endif
	    		    VLLcompute_clip_status(
				dde_g4vll_drom_data.clip_status, bndboxpts);
			    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     */
				    VLLset_diffuse_color(black);
            			    VLLtriangle_mesh
					(   vertexcount, (void*)vtxlocs, 
					    (void*)vtxnrms,
				    	    (void*)0, (void*)vtxalpha,
				    	    (void*)uv_list[idx], trianglecount,
				    	    (void*)elenrms,
				    	    (void*)dre_dynrnd_drom_element_colors,
				    	    (void*)0, connect,
				    	    dde_g4vll_triangle_mesh_edge_enable
					);
				    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
				} else {
            			    VLLtriangle_mesh
					(   vertexcount, (void*)vtxlocs, 
					    (void*)vtxnrms,
				    	    (void*)vtxclrs, (void*)vtxalpha,
				    	    (void*)uv_list[idx], trianglecount,
				    	    (void*)elenrms,
				    	    (void*)dre_dynrnd_drom_element_colors,
				    	    (void*)0, connect,
				    	    dde_g4vll_triangle_mesh_edge_enable
					);
				}
			    } else /* ddd_g4vll_drom_texmap_3d */ {
				if (dde_g4vll_drom_data.map_extend_black) {
				    VLLset_diffuse_color(black);
            			    VLLtriangle_mesh
					(   vertexcount, (void*)vtxlocs, 
					    (void*)vtxnrms,
				    	    (void*)0, (void*)vtxalpha,
				    	    (void*)uvw_list[idx], trianglecount,
				    	    (void*)elenrms,
				    	    (void*)dre_dynrnd_drom_element_colors,
				    	    (void*)0, connect,
				    	    dde_g4vll_triangle_mesh_edge_enable
					);
					VLLset_diffuse_color(
				    	    dde_g4vll_drom_data.diffuse_color);
			    	} else {
            			    VLLtriangle_mesh
					(   vertexcount, (void*)vtxlocs, 
					    (void*)vtxnrms,
				    	    (void*)vtxclrs, (void*)vtxalpha,
				    	    (void*)uvw_list[idx], trianglecount,
				    	    (void*)elenrms,
				    	    (void*)dre_dynrnd_drom_element_colors,
				    	    (void*)0, connect,
				    	    dde_g4vll_triangle_mesh_edge_enable
					);
			    	}
			    }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);
	    	    	}
		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
				ddd_g4vll_drom_texpass_light) &&
				!ddr_g4vll_drom_valid_transp_with_texture()) {
			VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

			switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
                        	VLLcompute_clip_status
				    (dde_g4vll_drom_data.clip_status,bndboxpts);
			        VLLset_clip_status
				    (dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
                		    VLLtriangle_mesh
				    (   vertexcount, (void*)vtxlocs, (void*)vtxnrms,
				    	(void*)0, (void*)vtxalpha, (void*)0,
                                    	trianglecount, (void*)elenrms,
				    	(void*)dre_dynrnd_drom_element_colors,
                                    	(void*)0, connect,
                                    	dde_g4vll_triangle_mesh_edge_enable
				    );
                        	    VLLset_diffuse_color
				    	(dde_g4vll_drom_data.diffuse_color);
                                } else {
                		    VLLtriangle_mesh
				    (   vertexcount, (void*)vtxlocs, (void*)vtxnrms,
				    	(void*)vtxclrs, (void*)vtxalpha, (void*)0,
                                    	trianglecount, (void*)elenrms,
				    	(void*)dre_dynrnd_drom_element_colors,
                                    	(void*)0, connect,
                                    	dde_g4vll_triangle_mesh_edge_enable
				    );
				}
				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
                            break;
                    	}

		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
			    ddd_g4vll_drom_texpass_vcol) &&
			    ddr_g4vll_drom_do_vertex_color_texture_pass(
                                vtxclrs, texture_mapping) &&
                            !ddr_g4vll_drom_valid_transp_with_texture()) {

			VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);
                    	switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
                        	VLLcompute_clip_status(
				    dde_g4vll_drom_data.clip_status, bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

                		VLLtriangle_mesh
				(   vertexcount, (void*)vtxlocs, (void*)vtxnrms,
				    (void*)vtxclrs, (void*)vtxalpha, (void*)0,
                                    trianglecount, (void*)elenrms,
				    (void*)dre_dynrnd_drom_element_colors,
                                    (void*)0, connect,
                                    dde_g4vll_triangle_mesh_edge_enable
				);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
                            break;
                    	}
		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
			    ddd_g4vll_drom_texpass_spec) &&
                            (dde_g4vll_drom_data.specular_switch) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
                            !ddr_g4vll_drom_valid_transp_with_texture()) {
			VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                    	switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
                                }
                        	VLLcompute_clip_status(
				    dde_g4vll_drom_data.clip_status, bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

                		VLLtriangle_mesh
				(   vertexcount, (void*)vtxlocs, (void*)vtxnrms,
				    (void*)vtxclrs, (void*)vtxalpha, (void*)0,
                                    trianglecount, (void*)elenrms,
				    (void*)dre_dynrnd_drom_element_colors,
                                    (void*)0, connect,
                                    dde_g4vll_triangle_mesh_edge_enable
				);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);

                                if (dde_g4vll_drom_data.map_extend_black) {
                        	    VLLset_diffuse_color(
				    	dde_g4vll_drom_data.diffuse_color);
				}
                            break;
                    	}
		    }
		} break;

		case ddd_g4vll_drom_texmap_env: {
	    	    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
	    	    VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
						bndboxpts);
		    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(0);

                    VLLtriangle_mesh
		    (   vertexcount, (void*)vtxlocs, (void*)vtxnrms,
			(void*)vtxclrs, (void*)vtxalpha, (void*)0,
                	trianglecount, (void*)elenrms,
			(void*)dre_dynrnd_drom_element_colors, (void*)0,
                        connect, dde_g4vll_triangle_mesh_edge_enable
		    );

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(1);
		} break;
	    }
    	} break;

    	case ddd_g4vll_drom_transp_pass: {
	    /*
	     * Determine whether this object should be drawn during this
	     * pass (it may have been drawn in the opaque or texture passes).
	     */
             if (ddr_g4vll_drom_transp_without_texture(uv_count, uvw_count)) {
#ifdef DEBUG_FLOW
printf ("\ttransparency pass, render trimsh\n");
#endif
                VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                       bndboxpts);
                VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(0);

                VLLtriangle_mesh
		(   vertexcount, (void*)vtxlocs, (void*)vtxnrms, (void*)vtxclrs,
		    (void*)vtxalpha, (void*)0, trianglecount, (void*)elenrms,
		    (void*)dre_dynrnd_drom_element_colors, (void*)0,
		    connect, dde_g4vll_triangle_mesh_edge_enable
		);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(1);
	    }
	} break;

	case ddd_g4vll_drom_transp_texture_pass: {
#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass ...\n");
#endif
	    texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
				uvw_count);
	    switch (texture_mapping) {
		case ddd_g4vll_drom_texmap_none: {
		} break;

		case ddd_g4vll_drom_texmap_2d:
		case ddd_g4vll_drom_texmap_3d: {
		    /*
		     * Even though executability hasn't been turned off for
		     * this primitive, it is still possible that it was
		     * rendered during the opaque texture pass.  Check to
		     * see if it can be both transparent and textured.
		     */
		    if (ddr_g4vll_drom_valid_transp_with_texture()) {
			if (dde_g4vll_drom_data.current_texpass_mode ==
				ddd_g4vll_drom_texpass_draw) {
			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
	    	    	    	idx = uv_count >
				    dde_g4vll_drom_data.map_diffuse_color_uv_index ?
		    		    dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
			    } else {
	    	    	    	idx = uvw_count >
				    dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
		    		    dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
			    }

	    	    	    VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);
#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass, render trimsh\n");
#endif

	    	    	    VLLcompute_clip_status(
				dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     */
                                    VLLset_diffuse_color(black);
				    VLLtriangle_mesh
					(   vertexcount, (void*)vtxlocs, 
					    (void*)vtxnrms,
                                    	    (void*)0, (void*)vtxalpha,
                                    	    (void*)uv_list[idx], trianglecount,
                                    	    (void*)elenrms,
				    	    (void*)dre_dynrnd_drom_element_colors,
				    	    (void*)0, connect,
				    	    dde_g4vll_triangle_mesh_edge_enable
					);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
				    VLLtriangle_mesh
					(   vertexcount, (void*)vtxlocs, 
					    (void*)vtxnrms,
                                    	    (void*)vtxclrs, (void*)vtxalpha,
                                    	    (void*)uv_list[idx], trianglecount,
                                    	    (void*)elenrms,
				    	    (void*)dre_dynrnd_drom_element_colors,
				    	    (void*)0, connect,
				    	    dde_g4vll_triangle_mesh_edge_enable
					);
				}
			    } else /* ddd_g4vll_drom_texmap_3d */ {
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
				    VLLtriangle_mesh
				    (   vertexcount, (void*)vtxlocs, (void*)vtxnrms,
                                    	(void*)0, (void*)vtxalpha,
                                    	(void*)uvw_list[idx], trianglecount,
                                    	(void*)elenrms,
				    	(void*)dre_dynrnd_drom_element_colors,
				    	(void*)0, connect,
				    	dde_g4vll_triangle_mesh_edge_enable
				    );
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
				    VLLtriangle_mesh
				    (   vertexcount, (void*)vtxlocs, (void*)vtxnrms,
                                    	(void*)vtxclrs, (void*)vtxalpha,
                                    	(void*)uvw_list[idx], trianglecount,
                                    	(void*)elenrms,
				    	(void*)dre_dynrnd_drom_element_colors,
				    	(void*)0, connect,
				    	dde_g4vll_triangle_mesh_edge_enable
				    );
			    	}
			    }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);

		        } else if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_light) {

			    VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status,
                                        bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
                                    	VLLtriangle_mesh
				    	(   vertexcount, (void*)vtxlocs,
					    (void*)vtxnrms, (void*)0,
					    (void*)vtxalpha, (void*)0,
                                    	    trianglecount, (void*)elenrms,
					    (void*)dre_dynrnd_drom_element_colors,
                                    	    (void*)0, connect,
                                    	    dde_g4vll_triangle_mesh_edge_enable
				    	);
                                        VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
                                    } else {
                                    	VLLtriangle_mesh
				    	(   vertexcount, (void*)vtxlocs,
					    (void*)vtxnrms, (void*)vtxclrs,
					    (void*)vtxalpha, (void*)0,
                                    	    trianglecount, (void*)elenrms,
					    (void*)dre_dynrnd_drom_element_colors,
                                    	    (void*)0, connect,
                                    	    dde_g4vll_triangle_mesh_edge_enable
				    	);
                                    }
				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);
                                break;
                            }

                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_vcol) &&
				ddr_g4vll_drom_do_vertex_color_texture_pass(
                                	vtxclrs, texture_mapping)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status,
					bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

                                    VLLtriangle_mesh (
					vertexcount, (void*)vtxlocs,
					(void*)vtxnrms, (void*)vtxclrs,
					(void*)vtxalpha, (void*)0,
                                    	trianglecount, (void*)elenrms,
					(void*)dre_dynrnd_drom_element_colors,
                                    	(void*)0, connect,
                                    	dde_g4vll_triangle_mesh_edge_enable
				    );

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);
                                break;
                            }
                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_spec) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
                            (dde_g4vll_drom_data.specular_switch)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    if (dde_g4vll_drom_data.map_extend_black) {
                                    	VLLset_diffuse_color(black);
				    }
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

                                    VLLtriangle_mesh
				    (   vertexcount, (void*)vtxlocs,
					(void*)vtxnrms, (void*)vtxclrs,
					(void*)vtxalpha, (void*)0,
                                    	trianglecount, (void*)elenrms,
					(void*)dre_dynrnd_drom_element_colors,
                                    	(void*)0, connect,
                                    	dde_g4vll_triangle_mesh_edge_enable
				    );

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                    	VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
				    }
                                break;
                            }
		        }
		    }
		} break;
	    }
    	} break;

    	case ddd_g4vll_drom_standard_pass:
	default: {
	    if (dde_g4vll_drom_data.map_diffuse_color_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have diffuse map\n");
#endif
                /* if the texture isn't valid or no UV/Ws are
                   provided, we'll detect it in the texture pass */
		dde_g4vll_drom_data.have_textures = DcTrue;
	    } else if (dde_g4vll_drom_data.map_env_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have env map\n");
#endif
                /* if the texture isn't valid  we'll detect
                   it in the texture pass */
		dde_g4vll_drom_data.have_textures = DcTrue;
		dde_g4vll_drom_data.have_env_mapping = DcTrue;
	    } else if (dde_g4vll_drom_data.transparent_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have transparency\n");
#endif
		dde_g4vll_drom_data.have_transparency = DcTrue;
	    } else {
#ifdef DEBUG_FLOW
printf ("\topaque pass -- draw triangle mesh\n");
#endif
		/*
		 * Object is opaque in the opaque render pass so draw it.
		 */
	    	dde_g4vll_drom_data.have_opaque_objects = DcTrue;

		if ((dde_g4vll_drom_data.traversal_type ==
			ddd_g4vll_drom_shadow_pass) &&
                   	((dde_g4vll_drom_data.reptype == DcOutlines)  ||
                    	(dde_g4vll_drom_data.reptype == DcWireframe) ||
                    	(dde_g4vll_drom_data.reptype == DcPoints))) {
                    /* don't render shadows for these reptypes */
                    break;
		}
	    	VLLcompute_clip_status
		    (dde_g4vll_drom_data.clip_status, bndboxpts);
		VLLset_clip_status (dde_g4vll_drom_data.clip_status);
		VLLtriangle_mesh
		(   vertexcount, (void*)vtxlocs, (void*)vtxnrms, (void*)vtxclrs,
		    (void*)vtxalpha, (void*)0, trianglecount, (void*)elenrms,
		    (void*)dre_dynrnd_drom_element_colors, (void *)0,
		    connect, dde_g4vll_triangle_mesh_edge_enable
		);
	    }
	} break;
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_render_triangle_strip (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         vertexcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtRealTriple *elenrms,
    DtInt         uv_count,
    DtRealCouple *uv_list[],
    DtInt         uvw_count,
    DtRealTriple *uvw_list[],
    DtReal       *vtxalpha)
{
    int idx;
    static float black[3] = {0.,0.,0.};
    int texture_mapping;
    int render_mode;
    DtInt tricount;

#ifdef DEBUG_STRIP
    int i;
    printf ("ddr_g4vll_drom_render_triangle_strip (%d triangles)\n",vertexcount-2);
    for (i=0; i<vertexcount; i++) {
        printf ("\t%g %g %g\n", vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
        if (vtxnrms) {
            printf ("\t%g %g %g\n", vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
        }
    }
    printf ("\n");
#endif

    /*
     * NOTE:  We do not prevent the user from doing texture mapping and/or
     * transparency when the reptype is not DcSurface, although this is the only
     * reptype supported in these cases.  We _do_ disable local antialiasing as
     * it seems to corrupt textures that are drawn on even non-local-antialiased
     * primitives.
     */

    /*
     * VLL wants the number of triangles
     */
    tricount = vertexcount - 2;

    switch (dde_g4vll_drom_data.traversal_type) {
        case ddd_g4vll_drom_texture_pass: {
            texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
                        uvw_count);
            switch (texture_mapping) {
                case ddd_g4vll_drom_texmap_none: {
                    /*
                     * Texture mapping was requested, but no uv/w's were
                     * supplied, or the texture isn't valid
                     */
                    if (dde_g4vll_drom_data.transparent_switch) {

                        /* draw later in the transparency pass */
                        dde_g4vll_drom_data.have_transparency = DcTrue;
                    } else {
                        /* just draw opaque */

                        dde_g4vll_drom_data.have_opaque_objects = DcTrue;

                        VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
                        VLLset_clip_status(dde_g4vll_drom_data.clip_status);

                        render_mode = VLLquery_render_mode();
                        VLLset_render_mode(VLL_RENDER_STANDARD);

                        VLLtriangle_strip(tricount, (void *)vtxlocs,
                            (void *)elenrms, (void *)vtxnrms,
                            (void *)dre_dynrnd_drom_element_colors,
                            (void *)vtxclrs, (void *)0, (void *)vtxalpha,
                            (void *)0, dde_g4vll_triangle_strip_edge_enable);

                        VLLset_render_mode(render_mode);
                    }
                } break;

                case ddd_g4vll_drom_texmap_2d:
                case ddd_g4vll_drom_texmap_3d: {
                    if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_draw) {
                        if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
                            idx = uv_count >
                                dde_g4vll_drom_data.map_diffuse_color_uv_index ?
                                dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
                        } else {
                            idx = uvw_count >
                                dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                                dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
                        }

                        VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

                        ddr_g4vll_drom_find_required_texture_passes(vtxclrs);
                        if (ddr_g4vll_drom_valid_transp_with_texture()) {
                            /*
                             * Mark this primitive as one to do during the
                             * transp pass(es) instead of the texture pass(es).
                             */
#ifdef DEBUG_FLOW
printf ("\trender textured tristrip during transparent texture passes\n");
#endif
                            dde_g4vll_drom_data.have_transp_textured_objects = DcTrue;
                        } else {
                            /*
                             * Render this primitive during the texture pass(es)
                             * and not during the transparent texture pass(es).
                             */
	    		    dde_g4vll_drom_data.have_opaque_objects = DcTrue;
#ifdef DEBUG_FLOW
printf ("\trender textured tristrip now (nontransparent texture pass)\n");
#endif

                            VLLcompute_clip_status(
                                dde_g4vll_drom_data.clip_status, bndboxpts);
			    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

                            if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     *
                                     * If the primitive has vertex colours, VLL
                                     * overrides the diffuse colour and renders
                                     * a white lit primitive in this pass, and
                                     * then does a multiply with the vertex
                                     * colours only in the extend area in the
                                     * vertex colour pass.
                                     */

                                    VLLset_diffuse_color(black);
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)0, (void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
                             	    	dde_g4vll_triangle_strip_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)vtxclrs, (void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
                             	    	dde_g4vll_triangle_strip_edge_enable);
				}
                            } else /* ddd_g4vll_drom_texmap_3d */ {
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)0, (void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
                             	    	dde_g4vll_triangle_strip_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
				} else {
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)vtxclrs, (void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
                             	    	dde_g4vll_triangle_strip_edge_enable);
                            	}
                            }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);
                        }
                    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_light) &&
                                !ddr_g4vll_drom_valid_transp_with_texture()) {
                        VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

                        switch (dde_g4vll_drom_data.map_operator) {
                            case DcTextureMultiply:
                            case DcTextureReplace:
                            default:
                                VLLcompute_clip_status(
                                    dde_g4vll_drom_data.clip_status,
                                    bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)0, (void *)0, (void *)vtxalpha,
				    	(void *)0,
                             	    	dde_g4vll_triangle_strip_edge_enable);
                                    VLLset_diffuse_color(
                                    	dde_g4vll_drom_data.diffuse_color);
			 	} else {
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)vtxclrs, (void *)0, (void *)vtxalpha,
				    	(void *)0,
                             	    	dde_g4vll_triangle_strip_edge_enable);
				}
				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
                            break;
                        }

                    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_vcol) &&
			    ddr_g4vll_drom_do_vertex_color_texture_pass(
                                vtxclrs, texture_mapping) &&
                            !ddr_g4vll_drom_valid_transp_with_texture()) {

                        VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);
                        switch (dde_g4vll_drom_data.map_operator) {
                            case DcTextureMultiply:
                            case DcTextureReplace:
                            default:
                                VLLcompute_clip_status(
                                    dde_g4vll_drom_data.clip_status, bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				VLLtriangle_strip(tricount, (void *)vtxlocs,
				    (void *)elenrms, (void *)vtxnrms,
				    (void *)dre_dynrnd_drom_element_colors,
				    (void *)vtxclrs, (void *)0,
				    (void *)vtxalpha, (void *)0,
                             	    dde_g4vll_triangle_strip_edge_enable);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
                            break;
                        }
                    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_spec) &&
                            (dde_g4vll_drom_data.specular_switch) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
			       !ddr_g4vll_drom_valid_transp_with_texture()) {
                        VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                        switch (dde_g4vll_drom_data.map_operator) {
                            case DcTextureMultiply:
                            case DcTextureReplace:
                            default:
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
                                }
                                VLLcompute_clip_status(
                                    dde_g4vll_drom_data.clip_status, bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				VLLtriangle_strip(tricount, (void *)vtxlocs,
				    (void *)elenrms, (void *)vtxnrms,
				    (void *)dre_dynrnd_drom_element_colors,
				    (void *)vtxclrs, (void *)0,
				    (void *)vtxalpha, (void *)0,
                             	    dde_g4vll_triangle_strip_edge_enable);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
				}
                            break;
                        }
                    }
                } break;

                case ddd_g4vll_drom_texmap_env: {
                    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
                    VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
		    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(0);

		    VLLtriangle_strip(tricount, (void *)vtxlocs,
			     (void *)elenrms, (void *)vtxnrms,
			     (void *)dre_dynrnd_drom_element_colors,
			     (void *)vtxclrs, (void *)0, (void *)vtxalpha,
			     (void *)0,
                             dde_g4vll_triangle_strip_edge_enable);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(1);
                } break;
            }
        } break;
        case ddd_g4vll_drom_transp_pass: {
            /*
             * Determine whether this object should be drawn during this
             * pass (it may have been drawn in the opaque or texture passes).
             */
             if (ddr_g4vll_drom_transp_without_texture(uv_count, uvw_count)) {
#ifdef DEBUG_FLOW
printf ("\ttransparency pass, render tristrip\n");
#endif
		VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
		                       bndboxpts);
		VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(0);

		VLLtriangle_strip(tricount, (void *)vtxlocs,
			(void *)elenrms, (void *)vtxnrms,
			(void *)dre_dynrnd_drom_element_colors,
			(void *)vtxclrs, (void *)0, (void *)vtxalpha, (void *)0,
                        dde_g4vll_triangle_strip_edge_enable);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(1);
            }
        } break;

        case ddd_g4vll_drom_transp_texture_pass: {
#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass ...\n");
#endif
            texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
                                uvw_count);
            switch (texture_mapping) {
                case ddd_g4vll_drom_texmap_none: {
                } break;

                case ddd_g4vll_drom_texmap_2d:
                case ddd_g4vll_drom_texmap_3d: {
                    /*
                     * Even though executability hasn't been turned off for
                     * this primitive, it is still possible that it was
                     * rendered during the opaque texture pass.  Check to
                     * see if it can be both transparent and textured.
                     */
                     if (ddr_g4vll_drom_valid_transp_with_texture()) {
                        if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_draw) {
                            if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
                                idx = uv_count >
                                    dde_g4vll_drom_data.map_diffuse_color_uv_index ?
                                    dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
                            } else {
                                idx = uvw_count >
                                    dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                                    dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0
;
                            }

                            VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass, render tristrip\n");
#endif

                            VLLcompute_clip_status(
                                dde_g4vll_drom_data.clip_status, bndboxpts);
			    VLLset_clip_status(
				dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

                            if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     */
                                    VLLset_diffuse_color(black);
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
                                    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)0, (void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
                             	    	dde_g4vll_triangle_strip_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
				} else {
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
                                    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)vtxclrs, (void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
                             	    	dde_g4vll_triangle_strip_edge_enable);
				}
                            } else /* ddd_g4vll_drom_texmap_3d */ {
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)0, (void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
                             	    	dde_g4vll_triangle_strip_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
				} else {
				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
				    	(void *)dre_dynrnd_drom_element_colors,
				    	(void *)vtxclrs, (void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
                             	    	dde_g4vll_triangle_strip_edge_enable);
				}
                            }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);

                        } else if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_light) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status,
                                        bndboxpts);
			    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
				    	VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	    (void *)elenrms, (void *)vtxnrms,
					    (void *)dre_dynrnd_drom_element_colors,
					    (void *)0, (void *)0,
					    (void *)vtxalpha, (void *)0,
                             	    	    dde_g4vll_triangle_strip_edge_enable);
                                        VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
                                    } else {
				    	VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	    (void *)elenrms, (void *)vtxnrms,
					    (void *)dre_dynrnd_drom_element_colors,
					    (void *)vtxclrs, (void *)0,
					    (void *)vtxalpha, (void *)0,
                             	    	    dde_g4vll_triangle_strip_edge_enable);
                                    }
				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);
                                break;
                            }

                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_vcol) &&
				ddr_g4vll_drom_do_vertex_color_texture_pass(
                                	vtxclrs, texture_mapping)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
			    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
					(void *)dre_dynrnd_drom_element_colors,
					(void *)vtxclrs, (void *)0,
					(void *)vtxalpha, (void *)0,
                             	    	dde_g4vll_triangle_strip_edge_enable);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);
                                break;
                            }
                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_spec) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
                            (dde_g4vll_drom_data.specular_switch)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
                                    }
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
			    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

				    VLLtriangle_strip(tricount, (void *)vtxlocs,
				    	(void *)elenrms, (void *)vtxnrms,
					(void *)0, (void *)vtxclrs, (void *)0,
					(void *)vtxalpha, (void *)0,
                             	    	dde_g4vll_triangle_strip_edge_enable);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
                                    }
				break;
                            }
                        }
                    }
                } break;
            }
        } break;

        case ddd_g4vll_drom_standard_pass:
        default: {
            if (dde_g4vll_drom_data.map_diffuse_color_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have diffuse map\n");
#endif
               /* if the texture isn't valid or no UV/Ws are
                  provided, we'll detect it in the texture pass */
                dde_g4vll_drom_data.have_textures = DcTrue;
            } else if (dde_g4vll_drom_data.map_env_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have env map\n");
#endif
               /* if the texture isn't valid  we'll detect
                  it in the texture pass */
                dde_g4vll_drom_data.have_textures = DcTrue;
                dde_g4vll_drom_data.have_env_mapping = DcTrue;
            } else if (dde_g4vll_drom_data.transparent_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have transparency\n");
#endif
                dde_g4vll_drom_data.have_transparency = DcTrue;
            } else {
#ifdef DEBUG_FLOW
printf ("\topaque pass -- draw triangle strip\n");
#endif
                /*
                 * Object is opaque in the opaque render pass so draw it.
                 */
                dde_g4vll_drom_data.have_opaque_objects = DcTrue;

                if ((dde_g4vll_drom_data.traversal_type ==
                        ddd_g4vll_drom_shadow_pass) &&
                        ((dde_g4vll_drom_data.reptype == DcOutlines)  ||
                        (dde_g4vll_drom_data.reptype == DcWireframe) ||
                        (dde_g4vll_drom_data.reptype == DcPoints))) {
                    /* don't render shadows for these reptypes */
                    break;
                }
                VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
		VLLset_clip_status(dde_g4vll_drom_data.clip_status);
		VLLtriangle_strip(tricount, (void *)vtxlocs,
			(void *)elenrms, (void *)vtxnrms,
			(void *)dre_dynrnd_drom_element_colors,
			(void *)vtxclrs, (void *)0, (void *)vtxalpha,
			(void *)0, dde_g4vll_triangle_strip_edge_enable);
            }
        } break;
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_render_quad_list (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         quadcount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,
    DtRealCouple *uv_list[],
    DtInt         uvw_count,
    DtRealTriple *uvw_list[],
    DtReal       *vtxalpha)
{
    int idx;
    static float black[3] = {0.,0.,0.};
    int texture_mapping;
    int render_mode;

#ifdef DEBUG
    int i;
    printf ("ddr_g4vll_drom_render_quad_list (%d quads)\n",quadcount);
    for (i=0; i<quadcount; i++) {
        printf ("\t%g %g %g %g\n", vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2], vtxlocs
[i][3]);
    }
    printf ("\n");
#endif

    /*
     * NOTE:  We do not prevent the user from doing texture mapping and/or
     * transparency when the reptype is not DcSurface, although this is the only
     * reptype supported in these cases.  We _do_ disable local antialiasing as
     * it seems to corrupt textures that are drawn on even non-local-antialiased
     * primitives.
     */

    switch (dde_g4vll_drom_data.traversal_type) {
        case ddd_g4vll_drom_texture_pass: {
            texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
                        uvw_count);
            switch (texture_mapping) {
                case ddd_g4vll_drom_texmap_none: {
                    /*
                     * Texture mapping was requested, but no uv/w's were
                     * supplied, or the texture isn't valid
                     */
                    if (dde_g4vll_drom_data.transparent_switch) {

                        /* draw later in the transparency pass */
                        dde_g4vll_drom_data.have_transparency = DcTrue;
                    } else {
                        /* just draw opaque */

                        dde_g4vll_drom_data.have_opaque_objects = DcTrue;

                        VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
                        VLLset_clip_status(dde_g4vll_drom_data.clip_status);

                        render_mode = VLLquery_render_mode();
                        VLLset_render_mode(VLL_RENDER_STANDARD);

                        VLLquad_list(quadcount,
                            (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
                            (void *)0, (void *)vtxclrs, (void *)0, (void *)vtxalpha,
                            (void *)0, dde_g4vll_quad_list_edge_enable);

                        VLLset_render_mode(render_mode);
                    }
                } break;

                case ddd_g4vll_drom_texmap_2d:
                case ddd_g4vll_drom_texmap_3d: {
                    if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_draw) {
                        if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
                            idx = uv_count >
                                dde_g4vll_drom_data.map_diffuse_color_uv_index ?
                                dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
                        } else {
                            idx = uvw_count >
                                dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                                dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
                        }

                        VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

                        ddr_g4vll_drom_find_required_texture_passes(vtxclrs);
                        if (ddr_g4vll_drom_valid_transp_with_texture()) {
                            /*
                             * Mark this primitive as one to do during the
                             * transp pass(es) instead of the texture pass(es).
                             */
#ifdef DEBUG_FLOW
printf ("\trender textured quadlst during transparent texture passes\n");
#endif
                            dde_g4vll_drom_data.have_transp_textured_objects = DcTrue;
                        } else {
                            /*
                             * Render this primitive during the texture pass(es)
                             * and not during the transparent texture pass(es).
                             */
	    		    dde_g4vll_drom_data.have_opaque_objects = DcTrue;
#ifdef DEBUG_FLOW
printf ("\trender textured quadlst now (nontransparent texture pass)\n");
#endif

                            VLLcompute_clip_status(
                                dde_g4vll_drom_data.clip_status, bndboxpts);
			    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

                            if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     *
                                     * If the primitive has vertex colours, VLL
                                     * overrides the diffuse colour and renders
                                     * a white lit primitive in this pass, and
                                     * then does a multiply with the vertex
                                     * colours only in the extend area in the
                                     * vertex colour pass.
                                     */
                                    VLLset_diffuse_color(black);
            			    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)0,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
                             	    	dde_g4vll_quad_list_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
				} else {
            			    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)vtxclrs,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
                             	    	dde_g4vll_quad_list_edge_enable);
				}
                            } else /* ddd_g4vll_drom_texmap_3d */ {
				if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
            			    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)0,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
                             	    	dde_g4vll_quad_list_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
            			    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)vtxclrs,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
                             	    	dde_g4vll_quad_list_edge_enable);
                            	}
                            }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);
                        }
                    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_light) &&
                                !ddr_g4vll_drom_valid_transp_with_texture()) {
                        VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

                        switch (dde_g4vll_drom_data.map_operator) {
                            case DcTextureMultiply:
                            case DcTextureReplace:
                            default:
                                VLLcompute_clip_status(
                                    dde_g4vll_drom_data.clip_status,
                                    bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
				    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)0,
				    	(void *)0, (void *)vtxalpha, (void *)0,
                             	    	dde_g4vll_quad_list_edge_enable);
                                    VLLset_diffuse_color(
                                    	dde_g4vll_drom_data.diffuse_color);
                                } else {
				    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)vtxclrs,
				    	(void *)0, (void *)vtxalpha, (void *)0,
                             	    	dde_g4vll_quad_list_edge_enable);
                                }
				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
                            break;
                        }

                    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_vcol) &&
			    ddr_g4vll_drom_do_vertex_color_texture_pass(
                                vtxclrs, texture_mapping) &&
                            !ddr_g4vll_drom_valid_transp_with_texture()) {

                        VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);
                        switch (dde_g4vll_drom_data.map_operator) {
                            case DcTextureMultiply:
                            case DcTextureReplace:
                            default:
                                VLLcompute_clip_status(
                                    dde_g4vll_drom_data.clip_status, bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				VLLquad_list(quadcount,
                             	    (void *)vtxlocs, (void *)elenrms,
				    (void *)vtxnrms, (void *)0, (void *)vtxclrs,
				    (void *)0, (void *)vtxalpha, (void *)0,
                             	    dde_g4vll_quad_list_edge_enable);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
                            break;
                        }
                    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_spec) &&
                            (dde_g4vll_drom_data.specular_switch) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
                            !ddr_g4vll_drom_valid_transp_with_texture()) {
                        VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);
                        switch (dde_g4vll_drom_data.map_operator) {
                            case DcTextureMultiply:
                            case DcTextureReplace:
                            default:
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
                                }
                                VLLcompute_clip_status(
                                    dde_g4vll_drom_data.clip_status, bndboxpts);
			        VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				VLLquad_list(quadcount,
                             	    (void *)vtxlocs, (void *)elenrms,
				    (void *)vtxnrms, (void *)0, (void *)vtxclrs,
				    (void *)0, (void *)vtxalpha, (void *)0,
                             	    dde_g4vll_quad_list_edge_enable);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                }
                            break;
                        }
                    }
                } break;

                case ddd_g4vll_drom_texmap_env: {
                    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
                    VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
		    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(0);

		    VLLquad_list(quadcount,
                           	(void *)vtxlocs, (void *)elenrms,
				(void *)vtxnrms, (void *)0, (void *)vtxclrs,
				(void *)0, (void *)vtxalpha, (void *)0,
                        	dde_g4vll_quad_list_edge_enable);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(1);
                } break;
            }
        } break;

        case ddd_g4vll_drom_transp_pass: {
            /*
             * Determine whether this object should be drawn during this
             * pass (it may have been drawn in the opaque or texture passes).
             */
             if (ddr_g4vll_drom_transp_without_texture(uv_count, uvw_count)) {
#ifdef DEBUG_FLOW
printf ("\ttransparency pass, render quadlst\n");
#endif
		VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
					bndboxpts);
		VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(0);

		VLLquad_list(quadcount, (void *)vtxlocs, (void *)elenrms,
			(void *)vtxnrms, (void *)0, (void *)vtxclrs,
			(void *)0, (void *)vtxalpha, (void *)0,
                        dde_g4vll_quad_list_edge_enable);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(1);
            }
        } break;

        case ddd_g4vll_drom_transp_texture_pass: {
#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass ...\n");
#endif
            texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
                                uvw_count);
            switch (texture_mapping) {
                case ddd_g4vll_drom_texmap_none: {
                } break;

                case ddd_g4vll_drom_texmap_2d:
                case ddd_g4vll_drom_texmap_3d: {
                    /*
                     * Even though executability hasn't been turned off for
                     * this primitive, it is still possible that it was
                     * rendered during the opaque texture pass.  Check to
                     * see if it can be both transparent and textured.
                     */
                     if (ddr_g4vll_drom_valid_transp_with_texture()) {
                        if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_draw) {
                            if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
                                idx = uv_count >
                                    dde_g4vll_drom_data.map_diffuse_color_uv_index ?
                                    dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
                            } else {
                                idx = uvw_count >
                                    dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
                                    dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0
;
                            }

                            VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass, render quadlst\n");
#endif

                            VLLcompute_clip_status(
                                dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

                            if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     */
                                    VLLset_diffuse_color(black);
				    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)0,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
                             	    	dde_g4vll_quad_list_edge_enable);
                                    VLLset_diffuse_color(
					dde_g4vll_drom_data.diffuse_color);
				} else {
				    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)vtxclrs,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx],
                             	    	dde_g4vll_quad_list_edge_enable);
				}
                            } else /* ddd_g4vll_drom_texmap_3d */ {
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
				    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)0,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
                             	    	dde_g4vll_quad_list_edge_enable);
                                    VLLset_diffuse_color(
					dde_g4vll_drom_data.diffuse_color);
				} else {
				    VLLquad_list(quadcount,
                             	    	(void *)vtxlocs, (void *)elenrms,
				    	(void *)vtxnrms, (void *)0, (void *)vtxclrs,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx],
                             	    	dde_g4vll_quad_list_edge_enable);
				}
                            }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);

                        } else if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_light) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status,
                                        bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
				    	VLLquad_list(quadcount,
                             		    (void *)vtxlocs, (void *)elenrms,
					    (void *)vtxnrms, (void *)0,
					    (void *)0,
					    (void *)0, (void *)vtxalpha, (void *)0,
                             		    dde_g4vll_quad_list_edge_enable);
                                        VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
                                    } else {
				    	VLLquad_list(quadcount,
                             		    (void *)vtxlocs, (void *)elenrms,
					    (void *)vtxnrms, (void *)0,
					    (void *)vtxclrs,
					    (void *)0, (void *)vtxalpha, (void *)0,
                             		    dde_g4vll_quad_list_edge_enable);
                                    }
				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);
                                break;
                            }

                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_vcol) &&
				ddr_g4vll_drom_do_vertex_color_texture_pass(
                                	vtxclrs, texture_mapping)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

				    VLLquad_list(quadcount,
                             		(void *)vtxlocs, (void *)elenrms,
					(void *)vtxnrms, (void *)0, (void *)vtxclrs,
					(void *)0, (void *)vtxalpha, (void *)0,
                             		dde_g4vll_quad_list_edge_enable);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);
				break;
                            }
                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_spec) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
                            (dde_g4vll_drom_data.specular_switch)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
                                    }
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

				    VLLquad_list(quadcount,
                             		(void *)vtxlocs, (void *)elenrms,
					(void *)vtxnrms, (void *)0, (void *)vtxclrs,
					(void *)0, (void *)vtxalpha, (void *)0,
                             		dde_g4vll_quad_list_edge_enable);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
                                    }
                                break;
                            }
                        }
                    }
                } break;
            }
        } break;

        case ddd_g4vll_drom_standard_pass:
        default: {
            if (dde_g4vll_drom_data.map_diffuse_color_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have diffuse map\n");
#endif
                /* if the texture isn't valid or no UV/Ws are
                   provided, we'll detect it in the texture pass */
                dde_g4vll_drom_data.have_textures = DcTrue;
            } else if (dde_g4vll_drom_data.map_env_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have env map\n");
#endif
                /* if the texture isn't valid  we'll detect
                   it in the texture pass */
                dde_g4vll_drom_data.have_textures = DcTrue;
                dde_g4vll_drom_data.have_env_mapping = DcTrue;
            } else if (dde_g4vll_drom_data.transparent_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have transparency\n");
#endif
                dde_g4vll_drom_data.have_transparency = DcTrue;
            } else {
#ifdef DEBUG_FLOW
printf ("\topaque pass -- draw quad list\n");
#endif
                /*
                 * Object is opaque in the opaque render pass so draw it.
                 */
                dde_g4vll_drom_data.have_opaque_objects = DcTrue;

                if ((dde_g4vll_drom_data.traversal_type ==
                        ddd_g4vll_drom_shadow_pass) &&
                        ((dde_g4vll_drom_data.reptype == DcOutlines)  ||
                        (dde_g4vll_drom_data.reptype == DcWireframe) ||
                        (dde_g4vll_drom_data.reptype == DcPoints))) {
                    /* don't render shadows for these reptypes */
                    break;
                }
                VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
		VLLset_clip_status(dde_g4vll_drom_data.clip_status);
		VLLquad_list(quadcount,
                        (void *)vtxlocs, (void *)elenrms, (void *)vtxnrms,
			(void *)0, (void *)vtxclrs, (void *)0, (void *)vtxalpha,
			(void *)0, dde_g4vll_quad_list_edge_enable);
            }
        } break;
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_render_quad_grid (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         width,
    DtInt         height,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,
    DtRealCouple *uv_list[],
    DtInt         uvw_count,
    DtRealTriple *uvw_list[],
    DtReal       *vtxalpha)
{
    int idx;
    static float black[3] = {0.,0.,0.};
    int texture_mapping;
    int render_mode;

    /*
     * NOTE:  We do not prevent the user from doing texture mapping and/or
     * transparency when the reptype is not DcSurface, although this is the only
     * reptype supported in these cases.  We _do_ disable local antialiasing as
     * it seems to corrupt textures that are drawn on even non-local-antialiased
     * primitives.
     */

    switch (dde_g4vll_drom_data.traversal_type) {
	case ddd_g4vll_drom_texture_pass: {
	    texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
			uvw_count);
	    switch (texture_mapping) {
		case ddd_g4vll_drom_texmap_none: {
                    /*
                     * Texture mapping was requested, but no uv/w's were
                     * supplied, or the texture isn't valid
                     */
                    if (dde_g4vll_drom_data.transparent_switch) {

                        /* draw later in the transparency pass */
                        dde_g4vll_drom_data.have_transparency = DcTrue;
                    } else {
                        /* draw opaque */

                        dde_g4vll_drom_data.have_opaque_objects = DcTrue;

                        VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
                                                bndboxpts);
                        VLLset_clip_status(dde_g4vll_drom_data.clip_status);

                        render_mode = VLLquery_render_mode();
                        VLLset_render_mode(VLL_RENDER_STANDARD);

                        VLLquad_mesh(width, height,
                            (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs,
                            (void *)vtxalpha, (void *)0, (void *)elenrms, (void *)0,
                            (void *)0, dde_g4vll_quad_grid_edge_enable);

                        VLLset_render_mode(render_mode);
                    }
		} break;

		case ddd_g4vll_drom_texmap_2d:
		case ddd_g4vll_drom_texmap_3d: {
		    if (dde_g4vll_drom_data.current_texpass_mode ==
                        	ddd_g4vll_drom_texpass_draw) {
			if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
	    	    	    idx = uv_count >
			    	dde_g4vll_drom_data.map_diffuse_color_uv_index ?
		    	    	dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
			} else {
	    	    	    idx = uvw_count >
			    	dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
		    	    	dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
			}

	    	    	VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

                        ddr_g4vll_drom_find_required_texture_passes(vtxclrs);
                        if (ddr_g4vll_drom_valid_transp_with_texture()) {
			    /*
		 	     * Mark this primitive as one to do during the
		 	     * transp pass(es) instead of the texture pass(es).
		 	     */
#ifdef DEBUG_FLOW
printf ("\trender textured quadgrid during transparent texture passes\n");
#endif
			    dde_g4vll_drom_data.have_transp_textured_objects = DcTrue;
	    	    	} else {
			    /*
		 	     * Render this primitive during the texture pass(es)
		 	     * and not during the transparent texture pass(es).
		 	     */
	    		    dde_g4vll_drom_data.have_opaque_objects = DcTrue;
#ifdef DEBUG_FLOW
printf ("\trender textured quadgrid now (nontransparent texture pass)\n");
#endif

	    		    VLLcompute_clip_status(
				dde_g4vll_drom_data.clip_status, bndboxpts);
			    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     *
                                     * If the primitive has vertex colours, VLL
                                     * overrides the diffuse colour and renders
                                     * a white lit primitive in this pass, and
                                     * then does a multiply with the vertex
                                     * colours only in the extend area in the
                                     * vertex colour pass.
                                     */

                                    VLLset_diffuse_color(black);
            			    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx], (void *)elenrms,
				    	(void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
            			    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)vtxclrs, (void *)vtxalpha,
				    	(void *)uv_list[idx], (void *)elenrms,
				    	(void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
				}
			    } else /* ddd_g4vll_drom_texmap_3d */ {
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
            			    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx], (void *)elenrms,
				    	(void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
            			    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)vtxclrs, (void *)vtxalpha,
				    	(void *)uvw_list[idx], (void *)elenrms,
				    	(void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
			    	}
			    }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);
	    	    	}
		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
				ddd_g4vll_drom_texpass_light) &&
                                !ddr_g4vll_drom_valid_transp_with_texture()) {

			VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

			switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
                        	VLLcompute_clip_status(
				    dde_g4vll_drom_data.clip_status,
                                    bndboxpts);
			    	VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
            			    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)0, (void *)vtxalpha, (void *)0,
                            	    	(void *)elenrms, (void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
            			    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)vtxclrs, (void *)vtxalpha,
					(void *)0,
                            	    	(void *)elenrms, (void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
                                }
				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
                            break;
                    	}

		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
			    ddd_g4vll_drom_texpass_vcol) &&
			    ddr_g4vll_drom_do_vertex_color_texture_pass(
                                vtxclrs, texture_mapping) &&
			    !ddr_g4vll_drom_valid_transp_with_texture()) {

			VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);
                    	switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
                        	VLLcompute_clip_status(
				    dde_g4vll_drom_data.clip_status, bndboxpts);
			    	VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				VLLquad_mesh(width, height,
                                    (void *)vtxlocs, (void *)vtxnrms,
                                    (void *)vtxclrs, (void *)vtxalpha, (void *)0,
                                    (void *)elenrms, (void *)0, (void *)0,
                                    dde_g4vll_quad_grid_edge_enable);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);
			    break;
                    	}
		    } else if ((dde_g4vll_drom_data.current_texpass_mode ==
			    ddd_g4vll_drom_texpass_spec) &&
                            (dde_g4vll_drom_data.specular_switch) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
			    !ddr_g4vll_drom_valid_transp_with_texture()) {
			VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                    	switch (dde_g4vll_drom_data.map_operator) {
                    	    case DcTextureMultiply:
                    	    case DcTextureReplace:
                    	    default:
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
                                }
                        	VLLcompute_clip_status(
				    dde_g4vll_drom_data.clip_status, bndboxpts);
			    	VLLset_clip_status(
				    dde_g4vll_drom_data.clip_status);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(0);

				VLLquad_mesh(width, height,
                                    (void *)vtxlocs, (void *)vtxnrms,
                                    (void *)vtxclrs, (void *)vtxalpha, (void *)0,
                                    (void *)elenrms, (void *)0, (void *)0,
                                    dde_g4vll_quad_grid_edge_enable);

				if (dde_g4vll_drom_data.localaa_switch)
				    VLLset_line_filter(1);

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                }
                            break;
                    	}
		    }
		} break;

		case ddd_g4vll_drom_texmap_env: {
	    	    VLLset_render_mode(VLL_RENDER_TEXTURE_ENV);
	    	    VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
						bndboxpts);
		    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(0);

		    VLLquad_mesh(width, height,
                            (void *)vtxlocs, (void *)vtxnrms,
                            (void *)vtxclrs, (void *)vtxalpha, (void *)0,
                            (void *)elenrms, (void *)0, (void *)0,
                            dde_g4vll_quad_grid_edge_enable);

		    if (dde_g4vll_drom_data.localaa_switch)
			VLLset_line_filter(1);
		} break;
	    }
    	} break;

    	case ddd_g4vll_drom_transp_pass: {
	    /*
	     * Determine whether this object should be drawn during this
	     * pass (it may have been drawn in the opaque or texture passes).
	     */
             if (ddr_g4vll_drom_transp_without_texture(uv_count, uvw_count)) {

#ifdef DEBUG_FLOW
printf ("\ttransparency pass, render quadgrid\n");
#endif
		VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
					bndboxpts);
		VLLset_clip_status(dde_g4vll_drom_data.clip_status);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(0);

		VLLquad_mesh(width, height,
                            (void *)vtxlocs, (void *)vtxnrms,
                            (void *)vtxclrs, (void *)vtxalpha, (void *)0,
                            (void *)elenrms, (void *)0, (void *)0,
                            dde_g4vll_quad_grid_edge_enable);

		if (dde_g4vll_drom_data.localaa_switch)
		    VLLset_line_filter(1);
	    }
	} break;

	case ddd_g4vll_drom_transp_texture_pass: {
#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass ...\n");
#endif
	    texture_mapping = ddr_g4vll_drom_query_texmapping_type(uv_count,
				uvw_count);
	    switch (texture_mapping) {
		case ddd_g4vll_drom_texmap_none: {
		} break;

		case ddd_g4vll_drom_texmap_2d:
		case ddd_g4vll_drom_texmap_3d: {
		    /*
		     * Even though executability hasn't been turned off for
		     * this primitive, it is still possible that it was
		     * rendered during the opaque texture pass.  Check to
		     * see if it can be both transparent and textured.
		     */
                    if (ddr_g4vll_drom_valid_transp_with_texture()) {
			if (dde_g4vll_drom_data.current_texpass_mode ==
				ddd_g4vll_drom_texpass_draw) {
			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
	    	    	    	idx = uv_count >
				    dde_g4vll_drom_data.map_diffuse_color_uv_index ?
		    		    dde_g4vll_drom_data.map_diffuse_color_uv_index : 0;
			    } else {
	    	    	    	idx = uvw_count >
				    dde_g4vll_drom_data.map_diffuse_color_uvw_index ?
		    		    dde_g4vll_drom_data.map_diffuse_color_uvw_index : 0;
			    }

	    	    	    VLLset_render_mode(VLL_RENDER_TEXTURE_DRAW);

#ifdef DEBUG_FLOW
printf ("\ttransparent texture pass, render quadgrid\n");
#endif

	    	    	    VLLcompute_clip_status(
				dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    VLLset_clip_status(dde_g4vll_drom_data.clip_status);

			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(0);

			    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {

                                if (dde_g4vll_drom_data.map_extend_black) {
                                    /*
                                     * Don't send vertex colours in this pass
                                     * and don't do the vertex colour texture
                                     * pass if the extend mode is black.
                                     */
                                    VLLset_diffuse_color(black);
				    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uv_list[idx], (void *)elenrms,
				    	(void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
				    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
				} else {
				    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)vtxclrs, (void *)vtxalpha,
				    	(void *)uv_list[idx], (void *)elenrms,
				    	(void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
				}
			    } else /* ddd_g4vll_drom_texmap_3d */ {
                                if (dde_g4vll_drom_data.map_extend_black) {
                                    VLLset_diffuse_color(black);
				    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)0, (void *)vtxalpha,
				    	(void *)uvw_list[idx], (void *)elenrms,
				    	(void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
                                    VLLset_diffuse_color(
                                        dde_g4vll_drom_data.diffuse_color);
                                } else {
				    VLLquad_mesh(width, height,
                            	    	(void *)vtxlocs, (void *)vtxnrms,
				    	(void *)vtxclrs, (void *)vtxalpha,
				    	(void *)uvw_list[idx], (void *)elenrms,
				    	(void *)0, (void *)0,
                            	    	dde_g4vll_quad_grid_edge_enable);
                                }
			    }
			    if (dde_g4vll_drom_data.localaa_switch)
				VLLset_line_filter(1);

		        } else if (dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_light) {

			    VLLset_render_mode(VLL_RENDER_TEXTURE_LIGHT);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status,
                                        bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
				    	VLLquad_mesh(width, height,
                            		    (void *)vtxlocs, (void *)vtxnrms,
					    (void *)0, (void *)vtxalpha, (void *)0,
                            		    (void *)elenrms, (void *)0, (void *)0,
                            		    dde_g4vll_quad_grid_edge_enable);
                                        VLLset_diffuse_color(
                                                dde_g4vll_drom_data.diffuse_color);
                                    } else {
				    	VLLquad_mesh(width, height,
                            		    (void *)vtxlocs, (void *)vtxnrms,
					    (void *)vtxclrs, (void *)vtxalpha,
					    (void *)0,
                            		    (void *)elenrms, (void *)0, (void *)0,
                            		    dde_g4vll_quad_grid_edge_enable);
                                    }
				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);
                                break;
                            }

                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                                ddd_g4vll_drom_texpass_vcol) &&
				ddr_g4vll_drom_do_vertex_color_texture_pass(
                                	vtxclrs, texture_mapping)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_VERTEX_COLOR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

				    VLLquad_mesh(width, height,
                            		(void *)vtxlocs, (void *)vtxnrms,
					(void *)vtxclrs, (void *)vtxalpha, (void *)0,
                            		(void *)elenrms, (void *)0, (void *)0,
                            		dde_g4vll_quad_grid_edge_enable);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);
				break;
                            }
                        } else if ((dde_g4vll_drom_data.current_texpass_mode ==
                            ddd_g4vll_drom_texpass_spec) &&
			    (ddr_g4vll_drom_query_shader_status() !=
			       DcShaderConstant) && 
                            (dde_g4vll_drom_data.specular_switch)) {

                            VLLset_render_mode(VLL_RENDER_TEXTURE_SPECULAR);

                            switch (dde_g4vll_drom_data.map_operator) {
                                case DcTextureMultiply:
                                case DcTextureReplace:
                                default:
                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(black);
                                    }
                                    VLLcompute_clip_status(
                                        dde_g4vll_drom_data.clip_status, bndboxpts);
		    	    	    VLLset_clip_status(
					dde_g4vll_drom_data.clip_status);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(0);

				    VLLquad_mesh(width, height,
                            		(void *)vtxlocs, (void *)vtxnrms,
					(void *)vtxclrs, (void *)vtxalpha, (void *)0,
                            		(void *)elenrms, (void *)0, (void *)0,
                            		dde_g4vll_quad_list_edge_enable);

				    if (dde_g4vll_drom_data.localaa_switch)
					VLLset_line_filter(1);

                                    if (dde_g4vll_drom_data.map_extend_black) {
                                        VLLset_diffuse_color(
                                            dde_g4vll_drom_data.diffuse_color);
                                    }
                                break;
                            }
		        }
		    }
		} break;
	    }
    	} break;

    	case ddd_g4vll_drom_standard_pass:
	default: {
	    if (dde_g4vll_drom_data.map_diffuse_color_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have diffuse map\n");
#endif
                /* if the texture isn't valid or no UV/Ws are
                   provided, we'll detect it in the texture pass */
		dde_g4vll_drom_data.have_textures = DcTrue;
	    } else if (dde_g4vll_drom_data.map_env_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have env map\n");
#endif
                /* if the texture isn't valid  we'll detect
                   it in the texture pass */
		dde_g4vll_drom_data.have_textures = DcTrue;
		dde_g4vll_drom_data.have_env_mapping = DcTrue;
	    } else if (dde_g4vll_drom_data.transparent_switch) {
#ifdef DEBUG_FLOW
printf ("\topaque pass but have transparency\n");
#endif
		dde_g4vll_drom_data.have_transparency = DcTrue;
	    } else {
#ifdef DEBUG_FLOW
printf ("\topaque pass -- draw quad mesh\n");
#endif
		/*
		 * Object is opaque in the opaque render pass so draw it.
		 */
	    	dde_g4vll_drom_data.have_opaque_objects = DcTrue;

		if ((dde_g4vll_drom_data.traversal_type ==
			ddd_g4vll_drom_shadow_pass) &&
                   	((dde_g4vll_drom_data.reptype == DcOutlines)  ||
                    	(dde_g4vll_drom_data.reptype == DcWireframe) ||
                    	(dde_g4vll_drom_data.reptype == DcPoints))) {
                    /* don't render shadows for these reptypes */
                    break;
		}
	    	VLLcompute_clip_status(dde_g4vll_drom_data.clip_status,
						bndboxpts);
		VLLset_clip_status(dde_g4vll_drom_data.clip_status);
		VLLquad_mesh(width, height,
                        (void *)vtxlocs, (void *)vtxnrms, (void *)vtxclrs,
			(void *)vtxalpha, (void *)0, (void *)elenrms, (void *)0,
			(void *)0, dde_g4vll_quad_grid_edge_enable);
	    }
	} break;
    }
}

/* ====================================================================== */

static int ddr_g4vll_drom_query_texmapping_type (
    DtInt uv_count,
    DtInt uvw_count)
{
    auto DtFlag draw_mode;	/* True if we're doing the drawing pass */

    draw_mode = (dde_g4vll_drom_data.current_texpass_mode
		== ddd_g4vll_drom_texpass_draw);

    /* Determine what kind of texture mapping, if any, that we are doing.  Note
    ** that if the tex2D_finished flag is set, then we've completed all the
    ** tiles for the current texture, but we're still tiling for some other
    ** texture.  Thus, we can skip the draw pass for the current, completed
    ** texture, so we return texmap_finished. */

    if (dde_g4vll_drom_data.map_diffuse_color_switch)  {

	/* 2D Texture Mapping */

	if ((uv_count > 0) && dde_g4vll_drom_data.texture_2D_valid) {
	    if (draw_mode && dde_g4vll_drom_data.tex2D_finished)
		return ddd_g4vll_drom_texmap_finished;
	    else
		return ddd_g4vll_drom_texmap_2d;
	}

	/* 3D Texture Mapping */

	if ((uvw_count > 0) && dde_g4vll_drom_data.texture_3D_valid) {
	    if (draw_mode && dde_g4vll_drom_data.tex3D_finished)
		return ddd_g4vll_drom_texmap_finished;
	    else
		return ddd_g4vll_drom_texmap_3d;
	}
    } 

    if (dde_g4vll_drom_data.map_env_switch &&
	dde_g4vll_drom_data.texture_env_valid) {
	return ddd_g4vll_drom_texmap_env;
    } 

    if (  (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_texture_pass)
       && (dde_g4vll_drom_data.cur_texture_pass > 1)
       )
	return ddd_g4vll_drom_texmap_finished;
    else
	return ddd_g4vll_drom_texmap_none;
}

/* ====================================================================== */

static void
ddr_g4vll_drom_find_required_texture_passes (DtRealTriple *vtxclrs)
{
    unsigned texture_status;
    float matrix[4][4];

   /*
    * After the texture draw pass, determine if a lighting pass,
    * vertex color pass, and/or specular highlight pass are required.
    * The required passes are OR'd into a global texpass_mode field.
    */

    texture_status  = VLLquery_texture_status(VLLquery_surface_texture());

    if (texture_status & VLL_TEXTURE_STATUS_LIGHT_PASS) {
	dde_g4vll_drom_data.texpass_modes |= ddd_g4vll_drom_texpass_light;
    }

    if ((vtxclrs != NULL) && (!dde_g4vll_drom_data.map_extend_black)) {
#ifdef DEBUG_FLOW
printf ("\tneed to do a TEX_VCOL_PASS\n");
#endif
	dde_g4vll_drom_data.texpass_modes |= ddd_g4vll_drom_texpass_vcol;
    }

    if (dde_g4vll_drom_data.specular_switch) {
#ifdef DEBUG_FLOW
printf ("\tneed to do a TEX_SPECULAR_PASS\n");
#endif
	dde_g4vll_drom_data.texpass_modes |= ddd_g4vll_drom_texpass_spec;
    }

}

/*
======================================================================
 */

static DtObject ddr_g4vll_drom_query_shader_status(void)
{
    DtObject shader;
    static DtMethodPtr srfshd_query = (DtMethodPtr)DcNullPtr;

    /*
     * Return DcFalse if the shading model is DcShaderConstant, indicating
     * that texture mapping's specular pass should not be done.
     */

    if (srfshd_query == (DtMethodPtr)DcNullPtr) {
        srfshd_query = DDclass_InqMethod(DsInqClassId("AttSurfaceShade"),
                                DcMethodInqGlbAttVal);
    }
    (*srfshd_query)(&shader);

    return(shader);
}

/*
======================================================================
 */

static DtFlag
ddr_g4vll_drom_transp_without_texture(DtInt uv_count, DtInt uvw_count)
{
    /* return DcTrue if primitive is both transparent and not textured. */
    
    if (dde_g4vll_drom_data.transparent_switch) {

        if (dde_g4vll_drom_data.map_diffuse_color_switch)  {
	    if ((uv_count > 0) && dde_g4vll_drom_data.texture_2D_valid) 
		return(DcFalse);

	    if ((uvw_count > 0) && dde_g4vll_drom_data.texture_3D_valid)
		return(DcFalse);
        } 

        if (dde_g4vll_drom_data.map_env_switch &&
	    dde_g4vll_drom_data.texture_env_valid) {
	    return(DcFalse);
        } 

        return(DcTrue);
    } else {

        /* not transparent */
        return(DcFalse);
    }
}

/*
======================================================================
 */

static int
ddr_g4vll_drom_valid_transp_with_texture(void)
{
   /* Determine if the current primitive can be both texture
    * mapped and transparent.  This can be done only if the return
    * status from VLL is true and we are doing multipass transparency.
    * This routine should be called only when we know the texture
    * is valid and we've got UV/Ws.
    */

    unsigned texture_status;

    texture_status  = VLLquery_texture_status(VLLquery_surface_texture());

    if (((ddr_g4vll_dcm_inq_transp_mode()) == ddd_g4vll_transp_multi_pass) &&
          dde_g4vll_drom_data.transparent_switch) {

        return((int)texture_status & VLL_TEXTURE_STATUS_TRANSPARENT);

    } else {

        return(DcFalse);
    }
}

/*
======================================================================
 */
DtFlag ddr_g4vll_drom_do_vertex_color_texture_pass (
    DtRealTriple *vtxclrs,
    int texture_mapping)
{
    float matrix[4][4];
    /*
     * Skip the TEXTURE_VERTEX_COLOR pass if:
     *
     *    1) there are no vertex colors 
     *    2) the texture matrix is not the identity matrix+
     *    3) there are vertex colours but the extend mode is one of
     *	     black, repeat, clamp in any one direction.
     *
     *     + this assumes that the only supported texture operator is 
     *	     DcTextureReplace (currently the case). Note that the texture
     * 	     matrix won't be the identity if tiling is being done, but 
     *       time's out for this consideration ...
     */

    if (!vtxclrs) return DcFalse;

    VLLquery_matrix(VLL_TEXTURE_MATRIX, matrix);
    if (ddr_g4vll_drom_matrix_is_identity(matrix)) {
	return DcFalse;
    }

    if (texture_mapping == ddd_g4vll_drom_texmap_2d) {
    	if (!dde_g4vll_drom_data.map_extend_black &&
		(dde_g4vll_drom_data.vll_extend_mode[0] != VLL_EXTEND_CLAMP ||
		 dde_g4vll_drom_data.vll_extend_mode[1] != VLL_EXTEND_CLAMP) &&
		(dde_g4vll_drom_data.vll_extend_mode[0] != VLL_EXTEND_REPEAT ||
		 dde_g4vll_drom_data.vll_extend_mode[1] != VLL_EXTEND_REPEAT)) {
	    return DcTrue;
    	} else {
	    return DcFalse;
    	}
    } else if (texture_mapping == ddd_g4vll_drom_texmap_3d) {
    	if (!dde_g4vll_drom_data.map_extend_black &&
		(dde_g4vll_drom_data.vll_extend_mode[0] != VLL_EXTEND_CLAMP ||
		 dde_g4vll_drom_data.vll_extend_mode[1] != VLL_EXTEND_CLAMP ||
		 dde_g4vll_drom_data.vll_extend_mode[2] != VLL_EXTEND_CLAMP) &&
		(dde_g4vll_drom_data.vll_extend_mode[0] != VLL_EXTEND_REPEAT ||
		 dde_g4vll_drom_data.vll_extend_mode[1] != VLL_EXTEND_REPEAT ||
		 dde_g4vll_drom_data.vll_extend_mode[2] != VLL_EXTEND_REPEAT)) {
	    return DcTrue;
    	} else {
	    return DcFalse;
    	}
    }
}
/*
======================================================================
 */
DtFlag ddr_g4vll_drom_matrix_is_identity (
    float matrix[][4])
{
    if (matrix[0][0] == 1.0 &&
	matrix[0][1] == 0.0 &&
	matrix[0][2] == 0.0 &&
	matrix[0][3] == 0.0 &&

	matrix[1][0] == 0.0 &&
	matrix[1][1] == 1.0 &&
	matrix[1][2] == 0.0 &&
	matrix[1][3] == 0.0 &&

	matrix[2][0] == 0.0 &&
	matrix[2][1] == 0.0 &&
	matrix[2][2] == 1.0 &&
	matrix[2][3] == 0.0 &&

	matrix[3][0] == 0.0 &&
	matrix[3][1] == 0.0 &&
	matrix[3][2] == 0.0 &&
	matrix[3][3] == 1.0) {
	 
	return DcTrue;
    } else {
	return DcFalse;
    }
}
