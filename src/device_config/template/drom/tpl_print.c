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
#ifdef DEBUG 
/*
  ======================================================================
  Functions:
	void ddr_template_drom_print_point_list (bndboxpts,pointcount,elenrms, 
		vtxlocs,vtxnrms,vtxclrs, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_print_line_list (bndboxpts,linecount,elenrms,
		vtxlocs, vtxnrms,vtxclrs, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_print_connected_line_list (bndboxpts,vertexcount, 
		elenrms,vtxlocs,vtxnrms, vtxclrs, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_print_triangle_list (bndboxpts,trianglecount,
		elenrms, vtxlocs,vtxnrms,vtxclrs, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_print_triangle_mesh (bndboxpts,trianglecount,
		elenrms, vertexcount,vtxlocs,vtxnrms, vtxclrs,vertexlist, 
		compiledvertexlist, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_print_matrix(matrix)
	void ddr_template_drom_print_studio()
	void ddr_template_drom_print_atts()
  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void ddr_template_drom_print_point_list (
    DtRealTriple bndboxpts[8],
    DtInt pointcount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple **uv_list,
    DtInt uvw_count,
    DtRealTriple **uvw_list)
{
    DtInt i, j;
    DtFlag nrms, clrs;

    printf ("\nddr_template_drom_print_point_list\n");
    ddr_template_drom_print_atts();

    if (bndboxpts != (DtRealTriple *)0) {
	printf ("\tbndboxpts\n");
	for (i=0; i<8; i++) {
	    printf ("\t    (%g %g %g)\n", 
		    bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
	}
    } else {
	printf ("\tbndboxpts:\t<NULL>\n");
    }

    if (elenrms != (DtRealTriple *)0) {
	printf ("\t%d elenrms\n", pointcount);
	for (i=0; i<pointcount; i++) {
	    printf ("\t    %d: (%g %g %g)\n", i,
		    elenrms[i][0], elenrms[i][1], elenrms[i][2]);
	}
    }
    nrms = vtxnrms != (DtRealTriple *)0;
    clrs = vtxclrs != (DtRealTriple *)0;
    printf ("\t%d points with locations%s%s", pointcount,
	    nrms ? ", normals" : "", clrs ? ", colors" : "");
    if (uv_count > 0)  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
    if (uvw_count > 0) printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
    printf ("\n");

    for (i=0; i<pointcount; i++) {
	printf ("\t    %d: (%g %g %g)", i,
		vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
	if (nrms) {
	    printf (" (%g %g %g)", 
		    vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
	}
	if (clrs) {
	    printf (" (%g %g %g)", 
		    vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
	}
	for (j=0; j < uv_count; j++) {
	    printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
	}
	for (j=0; j< uvw_count; j++) {
	    printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
	}
	printf ("\n");
    }
}
/*
 ======================================================================
 */
void ddr_template_drom_print_line_list (
    DtRealTriple *bndboxpts,
    DtInt linecount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple **uv_list,
    DtInt uvw_count,
    DtRealTriple **uvw_list)
{
    DtInt i, j;
    DtFlag nrms, clrs;

    printf ("ddr_template_drom_print_line_list\n");
    ddr_template_drom_print_atts();

    if (bndboxpts != (DtRealTriple *)0) {
	printf ("\tbndboxpts\n");
	for (i=0; i<8; i++) {
	    printf ("\t    (%g %g %g)\n", 
		    bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
	}
    } else {
	printf ("\tbndboxpts:\t<NULL>\n");
    }

    if (elenrms != (DtRealTriple *)0) {
	printf ("\t%d elenrms\n", linecount);
	for (i=0; i<linecount; i++) {
	    printf ("\t    %d: (%g %g %g)\n", i,
		    elenrms[i][0], elenrms[i][1], elenrms[i][2]);
	}
    }
    nrms = vtxnrms != (DtRealTriple *)0;
    clrs = vtxclrs != (DtRealTriple *)0;
    printf ("\t%d lines with locations%s%s", linecount,
	    nrms ? ", normals" : "", clrs ? ", colors" : "");
    if (uv_count > 0)  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
    if (uvw_count > 0) printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
    printf ("\n");

    for (i=0; i<linecount*2; i++) {
	printf ("\t    %d-%d: (%g %g %g)", i/2, i%2, 
		vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
	if (nrms) {
	    printf (" (%g %g %g)", 
		    vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
	}
	if (clrs) {
	    printf (" (%g %g %g)", 
		    vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
	}
	for (j=0; j < uv_count; j++) {
	    printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
	}
	for (j=0; j< uvw_count; j++) {
	    printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
	}
	printf ("\n");
		
    }
}
/*
 ======================================================================
 */
void ddr_template_drom_print_connected_line_list (
    DtRealTriple bndboxpts[8],
    DtInt vertexcount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple **uv_list,
    DtInt uvw_count,
    DtRealTriple **uvw_list)
{
    DtInt i, j;
    DtFlag nrms, clrs;

    printf ("ddr_template_drom_print_connected_line_list\n");
    ddr_template_drom_print_atts();

    if (bndboxpts != (DtRealTriple *)0) {
	printf ("\tbndboxpts\n");
	for (i=0; i<8; i++) {
	    printf ("\t    (%g %g %g)\n", 
		    bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
	}
    } else {
	printf ("\tbndboxpts:\t<NULL>\n");
    }

    if (elenrms != (DtRealTriple *)0) {
	printf ("\t%d elenrms\n", vertexcount-1);
	for (i=0; i<vertexcount-1; i++) {
	    printf ("\t    %d: (%g %g %g)\n", i,
		    elenrms[i][0], elenrms[i][1], elenrms[i][2]);
	}
    }
    nrms = vtxnrms != (DtRealTriple *)0;
    clrs = vtxclrs != (DtRealTriple *)0;
    printf ("\t%d vertices with locations%s%s", vertexcount,
	    nrms ? ", normals" : "", clrs ? ", colors" : "");
    if (uv_count > 0)  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
    if (uvw_count > 0) printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
    printf ("\n");

    for (i=0; i<vertexcount; i++) {
	printf ("\t    %d: (%g %g %g)", i,
		vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
	if (nrms) {
	    printf (" (%g %g %g)", 
		    vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
	}
	if (clrs) {
	    printf (" (%g %g %g)", 
		    vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
	}
	for (j=0; j < uv_count; j++) {
	    printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
	}
	for (j=0; j< uvw_count; j++) {
	    printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
	}
	printf ("\n");
    }
}
/*
 ======================================================================
 */
void ddr_template_drom_print_triangle_list (
    DtRealTriple bndboxpts[8],
    DtInt trianglecount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple **uv_list,
    DtInt uvw_count,
    DtRealTriple **uvw_list)
{
    DtInt i, j;
    DtFlag nrms, clrs;

    printf ("ddr_template_drom_print_triangle_list\n");
    ddr_template_drom_print_atts();

    if (bndboxpts != (DtRealTriple *)0) {
	printf ("\tbndboxpts\n");
	for (i=0; i<8; i++) {
	    printf ("\t    (%g %g %g)\n", 
		    bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
	}
    }

    if (elenrms != (DtRealTriple *)0) {
	printf ("\t%d elenrms\n", trianglecount);
	for (i=0; i<trianglecount; i++) {
	    printf ("\t    %d: (%g %g %g)\n", i,
		    elenrms[i][0], elenrms[i][1], elenrms[i][2]);
	}
    }
    nrms = vtxnrms != (DtRealTriple *)0;
    clrs = vtxclrs != (DtRealTriple *)0;
    printf ("\t%d triangles with locations%s%s", trianglecount,
	    nrms ? ", normals" : "", clrs ? ", colors" : "");
    if (uv_count > 0)  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
    if (uvw_count > 0) printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
    printf ("\n");

    for (i=0; i<trianglecount*3; i++) {
	printf ("\t    %d-%d: (%g %g %g)", i/3, i%3, 
		vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
	if (nrms) {
	    printf (" (%g %g %g)", 
		    vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
	}
	if (clrs) {
	    printf (" (%g %g %g)", 
		    vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
	}
	for (j=0; j < uv_count; j++) {
	    printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
	}
	for (j=0; j< uvw_count; j++) {
	    printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
	}
	printf ("\n");
    }
}	    
/*
 ======================================================================
 */
void ddr_template_drom_print_triangle_mesh (
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
    DtRealCouple **uv_list,
    DtInt uvw_count,
    DtRealTriple **uvw_list)
{
    DtInt i, j;
    DtFlag nrms, clrs;

    printf ("ddr_template_drom_print_triangle_mesh\n");
    ddr_template_drom_print_atts();

    if (bndboxpts != (DtRealTriple *)0) {
	printf ("\tbndboxpts\n");
	for (i=0; i<8; i++) {
	    printf ("\t    (%g %g %g)\n", 
		    bndboxpts[i][0], bndboxpts[i][1], bndboxpts[i][2]);
	}
    }

    if (elenrms != (DtRealTriple *)0) {
	printf ("\t%d elenrms\n", trianglecount);
	for (i=0; i<trianglecount; i++) {
	    printf ("\t    %d: (%g %g %g)\n", i,
		    elenrms[i][0], elenrms[i][1], elenrms[i][2]);
	}
    }
    nrms = vtxnrms != (DtRealTriple *)0;
    clrs = vtxclrs != (DtRealTriple *)0;
    printf ("\t%d vertices with locations%s%s", vertexcount,
	    nrms ? ", normals" : "", clrs ? ", colors" : "");
    if (uv_count > 0)  printf (", %d uv set%s", uv_count, uv_count > 1 ? "s" : "");
    if (uvw_count > 0) printf (", %d uvw set%s", uvw_count, uvw_count > 1 ? "s" : "");
    printf ("\n");

    for (i=0; i<vertexcount; i++) {
	printf ("\t    %d: (%g %g %g)", i,
		vtxlocs[i][0], vtxlocs[i][1], vtxlocs[i][2]);
	if (nrms) {
	    printf (" (%g %g %g)", 
		    vtxnrms[i][0], vtxnrms[i][1], vtxnrms[i][2]);
	}
	if (clrs) {
	    printf (" (%g %g %g)", 
		    vtxclrs[i][0], vtxclrs[i][1], vtxclrs[i][2]);
	}
	for (j=0; j < uv_count; j++) {
	    printf (" (%g %g)", uv_list[j][i][0], uv_list[j][i][1]);
	}
	for (j=0; j< uvw_count; j++) {
	    printf (" (%g %g %g)", uvw_list[j][i][0], uvw_list[j][i][1]);
	}
	printf ("\n");
    }

    printf ("\t%d triangles\n", trianglecount);
    for (i=0; i<trianglecount; i++) {
	printf ("\t    %d: (%d %d %d)\n", i, vertexlist[i][0],
		vertexlist[i][1], vertexlist[i][2]);
    }
}
/*
 ======================================================================
 */
void ddr_template_drom_print_matrix (
    DtMatrix4x4 matrix)
{
    DtInt i;
    for (i=0; i<4; i++) {
	printf ("\t\t\t%e %e %e %e\n", 
		matrix[i][0], matrix[i][1], matrix[i][2], matrix[i][3]);
    }
}
/*
 ======================================================================
 */
void ddr_template_drom_print_studio (void)
{
    DtInt i;

    printf ("    device_data:\n");

    printf ("\tvisual_type	");
    switch (dde_template_drom_curdevdat->visual_type) {
    case DcStaticGrey:	printf ("DcStaticGrey\n");	break;
    case DcGreyScale:	printf ("DcGreyScale\n");	break;
    case DcStaticColor:	printf ("DcStaticColor\n");	break;
    case DcPseudoColor:	printf ("DcPseudoColor\n");	break;
    case DcTrueColor:	printf ("DcTrueColor\n");	break;
    case DcDirectColor:	printf ("DcDirectColor\n");	break;
    default:		printf ("<%d>\n", dde_template_drom_curdevdat->visual_type);
    }

    printf ("\tshade_mode	");
    switch (dde_template_drom_curdevdat->shade_mode) {
    case DcRange:	printf ("DcRange\n");		break;
    case DcComponent:	printf ("DcComponent\n");	break;
    default:		printf ("<%d>\n", dde_template_drom_curdevdat->shade_mode);
    }

    printf ("\tctodtrans	(%g %g %g)\n", 
	    dde_template_drom_curdevdat->ctodtrans[0],
	    dde_template_drom_curdevdat->ctodtrans[1],
	    dde_template_drom_curdevdat->ctodtrans[2]);
    printf ("\tctodscale	(%g %g %g)\n", 
	    dde_template_drom_curdevdat->ctodscale[0],
	    dde_template_drom_curdevdat->ctodscale[1],
	    dde_template_drom_curdevdat->ctodscale[2]);
    printf ("\tftodscale	(%g %g %g)\n", 
	    dde_template_drom_curdevdat->ftodscale[0],
	    dde_template_drom_curdevdat->ftodscale[1],
	    dde_template_drom_curdevdat->ftodscale[2]);
    printf ("\tannoscale	(%g %g %g)\n", 
	    dde_template_drom_curdevdat->annoscale[0],
	    dde_template_drom_curdevdat->annoscale[1],
	    dde_template_drom_curdevdat->annoscale[2]);
    printf ("\tstereoswitch	%s\n",
	    dde_template_drom_curdevdat->stereoswitch ? "DcOn" : "DcOff");

    printf ("    view_data:\n");
    printf ("\tclear_flag:	%s\n",
	    dde_template_drom_curviwdat->clear_flag ? "DcOn" : "DcOff");
    printf ("\tshade_index:	%d\n",
	    dde_template_drom_curviwdat->shade_index);
    printf ("\tbackground:	(%g %g %g)\n",
	    dde_template_drom_curviwdat->background_color[0],
	    dde_template_drom_curviwdat->background_color[1],
	    dde_template_drom_curviwdat->background_color[2]);
    printf ("\tbackground_just	(%g %g %g)\n",
	    dde_template_drom_curviwdat->background_just[0],
	    dde_template_drom_curviwdat->background_just[1]);

    printf ("\tcamera:\n");
    printf ("\t   type		");
    switch (dde_template_drom_curcamdat->camera_type) {
    case DcCameraArbitrary:	printf ("DcCameraArbitrary\n");		break;
    case DcCameraParallel:	printf ("DcCameraParallel\n");		break;
    case DcCameraPerspective:	printf ("DcCameraPerspective\n");	break;
    case DcCameraProjection:	printf ("DcCameraProjection\n");	break;
    default:			printf ("<%d>\n", 
					dde_template_drom_curcamdat->camera_type);
    }
    printf ("\t   location	(%g %g %g)\n",
	    dde_template_drom_curcamdat->wcscamloc[0],
	    dde_template_drom_curcamdat->wcscamloc[1],
	    dde_template_drom_curcamdat->wcscamloc[2]);
    printf ("\t   direction	(%g %g %g)\n",
	    dde_template_drom_curcamdat->wcscamnorm[0],
	    dde_template_drom_curcamdat->wcscamnorm[1],
	    dde_template_drom_curcamdat->wcscamnorm[2]);
    printf ("\t   stereoswitch	%s\n",
	    dde_template_drom_curcamdat->stereoswitch ? "DcOn" : "DcOff");
    printf ("\t   eye_sep	%g\n",
	    dde_template_drom_curcamdat->stereo_eyeseparation);
    printf ("\t   focal_dist	%g\n",
	    dde_template_drom_curcamdat->stereo_distance);
    printf ("\t   projmat\n");
    ddr_template_drom_print_matrix (dde_template_drom_curcamdat->projmat);
    printf ("\t   camtowcsmat\n");
    ddr_template_drom_print_matrix (dde_template_drom_curcamdat->camtowcsmat);
    printf ("\t   wcstocammat\n");
    ddr_template_drom_print_matrix (dde_template_drom_curcamdat->wcstocammat);
    printf ("\t   wcstofcsmat\n");
    ddr_template_drom_print_matrix (dde_template_drom_curcamdat->wcstofcsmat);

    printf ("\tlights (%d)\n", dde_template_drom_curviwdat->total_lights);
    for (i=0; i<dde_template_drom_curviwdat->total_lights; i++) {
	printf ("\t   %d: (0x%x) %s ", i, 
		dde_template_drom_curviwdat->light[i].light,
		dde_template_drom_curviwdat->light[i].status ? "DcOn " : "DcOff");
	if (dde_template_drom_curviwdat->light[i].type == DcLightAmbient) {
	    printf ("DcLightAmbient\n");
	    printf ("\t\tcolor\t\t(%g %g %g)\n",
		    dde_template_drom_curviwdat->light[i].color[0],
		    dde_template_drom_curviwdat->light[i].color[1],
		    dde_template_drom_curviwdat->light[i].color[2]);
	} else if (dde_template_drom_curviwdat->light[i].type == DcLightInfinite) {
	    printf ("DcLightInfinite\n");
	    printf ("\t\tcolor\t\t(%g %g %g)\n",
		    dde_template_drom_curviwdat->light[i].color[0],
		    dde_template_drom_curviwdat->light[i].color[1],
		    dde_template_drom_curviwdat->light[i].color[2]);
	    printf ("\t\tdirection\t(%g %g %g)\n",
		    dde_template_drom_curviwdat->light[i].direction[0],
		    dde_template_drom_curviwdat->light[i].direction[1],
		    dde_template_drom_curviwdat->light[i].direction[2]);
	} else if (dde_template_drom_curviwdat->light[i].type == DcLightPoint ||
		   dde_template_drom_curviwdat->light[i].type == DcLightPointAttn) {
	    printf ("DcLightPoint%s\n",
		    dde_template_drom_curviwdat->light[i].type == DcLightPointAttn
		    ? "Attn" : "");
	    printf ("\t\tcolor\t\t(%g %g %g)\n",
		    dde_template_drom_curviwdat->light[i].color[0],
		    dde_template_drom_curviwdat->light[i].color[1],
		    dde_template_drom_curviwdat->light[i].color[2]);
	    printf ("\t\tposition\t(%g %g %g)\n",
		    dde_template_drom_curviwdat->light[i].position[0],
		    dde_template_drom_curviwdat->light[i].position[1],
		    dde_template_drom_curviwdat->light[i].position[2]);
	    if (dde_template_drom_curviwdat->light[i].type == DcLightPointAttn) {
		printf ("\t\tattenuation\t(%g %g)\n",
			dde_template_drom_curviwdat->light[i].c1,
			dde_template_drom_curviwdat->light[i].c2);
	    }
	} else if (dde_template_drom_curviwdat->light[i].type == DcLightSpot ||
		   dde_template_drom_curviwdat->light[i].type == DcLightSpotAttn) {
	    printf ("DcLightSpot%s\n",
		    dde_template_drom_curviwdat->light[i].type == DcLightSpotAttn
		    ? "Attn" : "");
	    printf ("\t\tcolor\t\t(%g %g %g)\n",
		    dde_template_drom_curviwdat->light[i].color[0],
		    dde_template_drom_curviwdat->light[i].color[1],
		    dde_template_drom_curviwdat->light[i].color[2]);
	    printf ("\t\tposition\t(%g %g %g)\n",
		    dde_template_drom_curviwdat->light[i].position[0],
		    dde_template_drom_curviwdat->light[i].position[1],
		    dde_template_drom_curviwdat->light[i].position[2]);
	    printf ("\t\tdirection\t(%g %g %g)\n",
		    dde_template_drom_curviwdat->light[i].direction[0],
		    dde_template_drom_curviwdat->light[i].direction[1],
		    dde_template_drom_curviwdat->light[i].direction[2]);
	    printf ("\t\tangles\t\t(%g %g)\n",
			dde_template_drom_curviwdat->light[i].angle_total,
			dde_template_drom_curviwdat->light[i].angle_delta);
	    printf ("\t\texponent\t%g\n",
			dde_template_drom_curviwdat->light[i].exponent);
	    if (dde_template_drom_curviwdat->light[i].type == DcLightSpotAttn) {
		printf ("\t\tattenuation\t(%g %g)\n",
			dde_template_drom_curviwdat->light[i].c1,
			dde_template_drom_curviwdat->light[i].c2);
	    }
	}
    }

    printf ("   view_data\n");
    printf ("\twindow		(%g %g) (%g %g)\n",
	    dde_template_drom_curwindat->actviewxmin,
	    dde_template_drom_curwindat->actviewymin,
	    dde_template_drom_curwindat->actviewxmax,
	    dde_template_drom_curwindat->actviewymax);
}
/*
 ======================================================================
 */
void ddr_template_drom_print_mapping (
    ddt_template_drom_texmap *texmap,
    char *texname)
{
    DtPtr name;
    DtNameType type;
    DtInt i, j;

    printf ("\t    %10s\t\t%s\n", texname, texmap->status ? 
	    "DcOn" : "DcOff");
    if (!texmap->status) 
	    return;

    for (i=0; i< texmap->count; i++) {
	printf ("\t\t%d: mapping (", i);
	if (texmap->mapping[i] == DcStd3dTableLookup) {
	    printf ("DcStd3dTableLookup");
	} else if (texmap->mapping[i] == DcStdBumpMap) {
	    printf ("DcStdBumpMap");
	} else if (texmap->mapping[i] == DcStdCubeEnvironMap) {
	    printf ("DcStdCubeEnvironMap");
	} else if (texmap->mapping[i] == DcStdSphereEnvironMap) {
	    printf ("DcStdSphereEnvironMap");
	} else if (texmap->mapping[i] == DcStdTableLookup) {
	    printf ("DcStdTableLookup");
	} else {
	    printf ("<0x%x>", texmap->mapping[i]);
	}
	printf (") raster (0x%x)\n", texmap->raster[i]);
	printf ("\t\t    aamode\t");
	switch (texmap->attributes[i].aamode) {
	case DcTextureAntiAliasNone:	printf ("DcTextureAntiAliasNone\n"); break;
	case DcTextureAntiAliasMIP:	printf ("DcTextureAntiAliasMIP\n"); break;
	case DcTextureAntiAliasAdaptive:printf ("DcTextureAntiAliasAdaptive\n"); break;
	default: printf ("<%d>\n", texmap->attributes[i].aamode);
	}
	printf ("\t\t    operator\t");
	switch (texmap->attributes[i].operator) {
	case DcTextureReplace:		printf ("DcTextureReplace\n"); break;
	case DcTextureMultiply:		printf ("DcTextureMultiply\n"); break;
	case DcTextureBlend:		printf ("DcTextureBlend\n"); break;
	case DcTextureAdd:		printf ("DcTextureAdd\n"); break;
	default: printf ("<%d>\n", texmap->attributes[i].operator);
	}
	printf ("\t\t    surface_object\t0x%x",
		texmap->attributes[i].surface_object);
	if (DsInqObjStatus(texmap->attributes[i].surface_object) == DcObjectValid) {

	    DsInqObjName (texmap->attributes[i].surface_object, &type, &name);
	    switch (type) {
	    case DcNameInteger:		printf ("\t(%d)\n", *(int *)name); break;
	    case DcNameString:		printf ("\t(%s)\n", name);  break;
	    default: printf ("\n");
	    }
	}
	printf ("\t\t    projection_type\t");
	switch (texmap->attributes[i].projection_type) {
	case DcObjCentroid:		printf ("DcObjCentroid\n"); break;
	case DcISN:			printf ("DcISN\n"); break;
	default: printf ("<%d>\n", texmap->attributes[i].projection_type);
	}
	printf ("\t\t    uv idx\t%d\n",	texmap->attributes[i].uv.idx);
	printf ("\t\t    uv extend\t(");
	for (j=0; j<2; j++) {
	    switch(texmap->attributes[i].uv.extend[j]) {
	    case DcExtendNone:		printf ("DcExtendNone");	break;
	    case DcExtendBlack:		printf ("DcExtendBlack");	break;
	    case DcExtendClamp:		printf ("DcExtendClamp");	break;
	    case DcExtendRepeat:	printf ("DcExtendRepeat");	break;
	    default: printf ("<%d>", texmap->attributes[i].uv.extend[j]);
	    }
	    if (j==0) 	printf (", ");
	    else	printf (")\n");
	}
	printf ("\t\t    uv matrix\n");
	printf ("\t\t\t(%g %g %g)\n",
		texmap->attributes[i].uv.matrix[0][0],
		texmap->attributes[i].uv.matrix[0][1],
		texmap->attributes[i].uv.matrix[0][2]);
	printf ("\t\t\t(%g %g %g)\n",
		texmap->attributes[i].uv.matrix[1][0],
		texmap->attributes[i].uv.matrix[1][1],
		texmap->attributes[i].uv.matrix[1][2]);
	printf ("\t\t\t(%g %g %g)\n",
		texmap->attributes[i].uv.matrix[2][0],
		texmap->attributes[i].uv.matrix[2][1],
		texmap->attributes[i].uv.matrix[2][2]);
	printf ("\t\t    uvw idx\t%d\n",
		texmap->attributes[i].uvw.idx);
	printf ("\t\t    uvw extend\t(");
	for (j=0; j<3; j++) {
	    switch(texmap->attributes[i].uvw.extend[j]) {
	    case DcExtendNone:		printf ("DcExtendNone");	break;
	    case DcExtendBlack:		printf ("DcExtendBlack");	break;
	    case DcExtendClamp:		printf ("DcExtendClamp");	break;
	    case DcExtendRepeat:	printf ("DcExtendRepeat");	break;
	    default: printf ("<%d>", texmap->attributes[i].uvw.extend[j]);
	    }
	    if (j==2) 	printf (")\n");
	    else	printf (", ");
	}
	printf ("\t\t    uvw matrix\n");
	printf ("\t(%g %g %g %g)\n",
		texmap->attributes[i].uvw.matrix[0][0],
		texmap->attributes[i].uvw.matrix[0][1],
		texmap->attributes[i].uvw.matrix[0][2],
		texmap->attributes[i].uvw.matrix[0][3]);
	printf ("\t(%g %g %g %g)\n",
		texmap->attributes[i].uvw.matrix[1][0],
		texmap->attributes[i].uvw.matrix[1][1],
		texmap->attributes[i].uvw.matrix[1][2],
		texmap->attributes[i].uvw.matrix[1][3]);
	printf ("\t(%g %g %g %g)\n",
		texmap->attributes[i].uvw.matrix[2][0],
		texmap->attributes[i].uvw.matrix[2][1],
		texmap->attributes[i].uvw.matrix[2][2],
		texmap->attributes[i].uvw.matrix[2][3]);
	printf ("\t(%g %g %g %g)\n",
		texmap->attributes[i].uvw.matrix[3][0],
		texmap->attributes[i].uvw.matrix[3][1],
		texmap->attributes[i].uvw.matrix[3][2],
		texmap->attributes[i].uvw.matrix[3][3]);
    }
}
/*
 ======================================================================
 */
void ddr_template_drom_print_atts (void)
{
    DtInt i, j;

    printf ("\tAttributes\n");
    printf ("\t    Enabled lights: ");
    for (i=0; i<dde_template_drom_curviwdat->total_lights; i++) {
	if (dde_template_drom_curviwdat->light[i].status) printf ("%d ", i);
    }
    printf ("\n");

    printf ("\t    lcstowcsmat\n");
    ddr_template_drom_print_matrix (dde_template_drom_data.lcstowcsmat);
    printf ("\t    lcstofcsmat\n");
    ddr_template_drom_print_matrix (dde_template_drom_data.lcstofcsmat);
    printf ("\t    nrmlcstowcsmat\n");
    ddr_template_drom_print_matrix (dde_template_drom_data.nrmlcstowcsmat);
    printf ("\n");

    printf ("\t    ambint\t\t%g\n", dde_template_drom_data.ambint);
    printf ("\t    ambswi\t\t%s\n", dde_template_drom_data.ambswi ? "DcOn" : "DcOff");
    printf ("\t    bacfacculble\t%s\n", dde_template_drom_data.bacfacculble ? "DcOn" : "DcOff");
    printf ("\t    bacfacculswi\t%s\n", dde_template_drom_data.bacfacculswi ? "DcOn" : "DcOff");
    printf ("\t    depcue_zfront\t%g\n", dde_template_drom_data.depcue_zfront);
    printf ("\t    depcue_zback\t%g\n", dde_template_drom_data.depcue_zback);
    printf ("\t    depcue_sfront\t%g\n", dde_template_drom_data.depcue_sfront);
    printf ("\t    depcue_sback\t%g\n", dde_template_drom_data.depcue_sback);
    printf ("\t    depcue_color\t(%g %g %g)\n",
	    dde_template_drom_data.depcue_color[0],
	    dde_template_drom_data.depcue_color[1],
	    dde_template_drom_data.depcue_color[2]);
    printf ("\t    depcueswi\t\t%s\n", dde_template_drom_data.depcueswi ? "DcOn" : "DcOff");
    printf ("\t    difclr\t\t(%g %g %g)\n",
	    dde_template_drom_data.difclr[0],
	    dde_template_drom_data.difclr[1],
	    dde_template_drom_data.difclr[2]);
    printf ("\t    difint\t\t%g\n", dde_template_drom_data.difint);
    printf ("\t    difswi\t\t%s\n", dde_template_drom_data.difswi ? "DcOn" : "DcOff");
    printf ("\t    hidsrfswi\t\t%s\n", dde_template_drom_data.hidsrfswi ? "DcOn" : "DcOff");
    printf ("\t    inttyp\t\t");
    switch (dde_template_drom_data.inttyp) {
    case DcConstantShade:	printf ("DcConstantShade\n");	break;
    case DcVertexShade:		printf ("DcVertexShade\n");	break;
    case DcSurfaceShade:	printf ("DcSurfaceShade\n");	break;
    default:			printf ("<%d>\n", dde_template_drom_data.inttyp);
    }
    printf ("\t    lintyp\t\t");
    switch (dde_template_drom_data.lintyp) {
    case DcLineTypeSolid:	printf ("DcLineTypeSolid\n");	break;
    case DcLineTypeDash:	printf ("DcLineTypeDash\n");	break;
    case DcLineTypeDot:		printf ("DcLineTypeDot\n");	break;
    case DcLineTypeDotDash:	printf ("DcLineTypeDotDash\n");	break;
    default:			printf ("<%d>\n", dde_template_drom_data.lintyp);
    }
    printf ("\t    linwid\t\t%g\n", dde_template_drom_data.linwid);
    printf ("\t    localaasty\t\t");
    switch (dde_template_drom_data.localaasty) {
    case DcLocalAntiAliasFast:	printf ("DcLocalAntiAliasFast\n");	break;
    case DcLocalAntiAliasIntermediate:	printf ("DcLocalAntiAliasIntermediate\n");	break;
    case DcLocalAntiAliasBest:	printf ("DcLocalAntiAliasBest\n");	break;
    default:			printf ("<%d>\n", dde_template_drom_data.localaasty);
    }
    printf ("\t    localaaswi\t\t%s\n", dde_template_drom_data.localaaswi ? "DcOn" : "DcOff");
    printf ("\t    refswi\t\t%s\n", dde_template_drom_data.refswi ? "DcOn" : "DcOff");
    printf ("\t    reptyp\t\t");
    switch (dde_template_drom_data.reptyp) {
    case DcPoints:		printf ("DcPoints\n");			break;
    case DcWireframe:		printf ("DcWireframe\n");		break;
    case DcSurface:		printf ("DcSurface\n");			break;
    case DcOutlines:		printf ("DcOutlines\n");		break;
    case DcSurfaceWireframeEdges:printf ("DcSurfaceWireframeEdges\n");	break;
    case DcSurfaceOutlineEdges:	printf ("DcSurfaceOutlineEdges\n");	break;
    default:			printf ("<%d>\n", dde_template_drom_data.reptyp);
    }
    printf ("\t    shaswi\t\t%s\n", dde_template_drom_data.shaswi ? "DcOn" : "DcOff");
    printf ("\t    shdidx\t\t%d\n", dde_template_drom_data.shdidx);
    printf ("\t    spcclr\t\t(%g %g %g)\n",
	    dde_template_drom_data.spcclr[0],
	    dde_template_drom_data.spcclr[1],
	    dde_template_drom_data.spcclr[2]);
    printf ("\t    spcfct\t\t%g\n", dde_template_drom_data.spcfct);
    printf ("\t    spcint\t\t%g\n", dde_template_drom_data.spcint);
    printf ("\t    spcswi\t\t%s\n", dde_template_drom_data.spcswi ? "DcOn" : "DcOff");
    printf ("\t    srdedgclr\t\t(%g %g %g)\n",
	    dde_template_drom_data.srfedgclr[0],
	    dde_template_drom_data.srfedgclr[1],
	    dde_template_drom_data.srfedgclr[2]);
    printf ("\t    srfshd\t\t");
    if (dde_template_drom_data.srfshd == DcShaderConstant) {
	printf ("DcShaderConstant\n");
    } else if (dde_template_drom_data.srfshd == DcShaderLightSource) {
	printf ("DcShaderLightSource\n");
    } else {
	printf ("0x%x\n", dde_template_drom_data.srfshd);
    }
    printf ("\t    transpclr\t\t(%g %g %g)\n",
	    dde_template_drom_data.transpclr[0],
	    dde_template_drom_data.transpclr[1],
	    dde_template_drom_data.transpclr[2]);
    printf ("\t    transpint\t\t%g\n", dde_template_drom_data.transpint);
    printf ("\t    transpswi\t\t%s\n", dde_template_drom_data.transpswi ? "DcOn" : "DcOff");
    printf ("\n");

    ddr_template_drom_print_mapping (&(dde_template_drom_data.mapbmp),"mapbmp");
    ddr_template_drom_print_mapping (&(dde_template_drom_data.mapdifclr),"mapdifclr");
    ddr_template_drom_print_mapping (&(dde_template_drom_data.mapenv),"mapenv");
    ddr_template_drom_print_mapping (&(dde_template_drom_data.maptrnint),"maptrnint");

}
#endif
