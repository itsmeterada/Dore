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
	ddr_gl_drom_render_connected_line_list
	ddr_gl_drom_render_line_list
	ddr_gl_drom_render_point_list
	ddr_gl_drom_render_triangle_list
	ddr_gl_drom_render_triangle_mesh

  ======================================================================
 */
#include "../gl_driver.h"



/* ======================================================================== */

void ddr_gl_drom_render_point_list (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         pointcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,	/* Unused */
    DtRealCouple *uv_list[],	/* Unused */
    DtInt         uvw_count,	/* Unused */
    DtRealTriple *uvw_list[],	/* Unused */
    DtReal       *vtxalpha)	/* Unused */
{
    auto int   p, i;
    auto short restore_lighting = 0;

    if ((DevStatus.traversal_number == 1 && DevStatus.frame_coord_prim) ||
	(DevStatus.traversal_number == 2 && !DevStatus.frame_coord_prim))
	    return;

    if (!DevStatus.lighting || DevStatus.depcueswitch) {
	vtxnrms = 0;
    } else if (!vtxnrms) {
	restore_lighting = 1;
	ddr_gl_drom_set_att_srfshd (DcShaderConstant);
    }

    for (p=0; p<pointcount;) {
	bgnpoint();

	for (i=0;  (i < 250) && (p < pointcount);  ++i, ++p) {
	    if (vtxclrs) ddr_iris_set_diffuse_color (vtxclrs[p]);
	    if (vtxnrms) SET_NORMAL (vtxnrms[p]);
	    SET_LOCATION (vtxlocs[p]);
	}
	endpoint();
    }

    if (restore_lighting)
	ddr_gl_drom_set_att_srfshd (DcShaderLightSource);
    else if (vtxclrs)
	ddr_iris_restore_diffuse_color ();
}



/* ======================================================================== */


void ddr_gl_drom_render_line_list (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         linecount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,	/* Unused */
    DtRealCouple *uv_list[],	/* Unused */
    DtInt         uvw_count,	/* Unused */
    DtRealTriple *uvw_list[],	/* Unused */
    DtReal       *vtxalpha)	/* Unused */
{
    int   l,l2;
    short restore_lighting = 0;

    if ((DevStatus.traversal_number == 1 && DevStatus.frame_coord_prim) ||
	(DevStatus.traversal_number == 2 && !DevStatus.frame_coord_prim))
	    return;

    if (!DevStatus.lighting || DevStatus.depcueswitch) {
	elenrms = 0;
	vtxnrms = 0;
    } else if (!vtxnrms && !elenrms) {
	restore_lighting = 1;
	ddr_gl_drom_set_att_srfshd (DcShaderConstant);
    }

    for (l=0, l2=0; l<linecount; l++) {
	if (!DevStatus.smoothswitch) {
	    if (vtxclrs) ddr_iris_set_diffuse_color (vtxclrs[l2]);
	    if (elenrms)
		SET_NORMAL (elenrms[l]);
	    else if (vtxnrms)
		SET_NORMAL (vtxnrms[l2]);
	} else {
	    if (!(vtxnrms) && !(vtxclrs) && (elenrms))
		SET_NORMAL (elenrms[l]);
	}

	if (DevStatus.reptype == DcPoints)
	    bgnpoint();
	else
	    bgnline();

	if (DevStatus.smoothswitch) {
	    if (vtxclrs) ddr_iris_set_diffuse_color (vtxclrs[l2]);
	    if (vtxnrms) SET_NORMAL(vtxnrms[l2]);
	    else if ((vtxclrs) && (elenrms))
		SET_NORMAL(elenrms[l]);
	}
	SET_LOCATION (vtxlocs[l2]);
	l2++;

	if (DevStatus.smoothswitch) {
	    if (vtxclrs) ddr_iris_set_diffuse_color (vtxclrs[l2]);
	    if (vtxnrms)
		SET_NORMAL (vtxnrms[l2]);
	    else if ((vtxclrs) && (elenrms))
		SET_NORMAL (elenrms[l]);
	}
	SET_LOCATION (vtxlocs[l2]);
	l2++;

	if (DevStatus.reptype == DcPoints)
	    endpoint();
	else
	    endline();
    }

    if (restore_lighting)
	ddr_gl_drom_set_att_srfshd (DcShaderLightSource);
    else if (vtxclrs)
	ddr_iris_restore_diffuse_color ();
}



/* ======================================================================== */

void ddr_gl_drom_render_connected_line_list (
    DtColorModel  colormodel,
    DtRealTriple  bndboxpts[8],
    DtInt         vertexcount,
    DtRealTriple *elenrms,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt         uv_count,	/* Unused */
    DtRealCouple *uv_list[],	/* Unused */
    DtInt         uvw_count,	/* Unused */
    DtRealTriple *uvw_list[],	/* Unused */
    DtReal       *vtxalpha)	/* Unused */
{
    auto int   l,i;
    auto short restore_lighting = 0;

    if ((DevStatus.traversal_number == 1 && DevStatus.frame_coord_prim) ||
	(DevStatus.traversal_number == 2 && !DevStatus.frame_coord_prim))
	    return;

    if (!DevStatus.lighting || DevStatus.depcueswitch) {
	elenrms = 0;
	vtxnrms = 0;
    } else if (!vtxnrms && !elenrms) {
	restore_lighting = 1;
	ddr_gl_drom_set_att_srfshd (DcShaderConstant);
    }

    for (l=0; l<vertexcount;) {
	if (DevStatus.reptype == DcPoints) 
	    bgnpoint();
	else
	    bgnline();

	for (i=0; i<250 && l<vertexcount; i++, l++) {
	    if (vtxnrms || l==vertexcount-1) {
		if (vtxclrs) ddr_iris_set_diffuse_color((vtxclrs)[l]);
		if (vtxnrms) SET_NORMAL(vtxnrms[l]);
		SET_LOCATION ((vtxlocs)[l]);
	    } else {
		if (vtxclrs) ddr_iris_set_diffuse_color((vtxclrs)[l]);
		if (elenrms) SET_NORMAL(elenrms[l]);
		SET_LOCATION ((vtxlocs)[l]);
	    }
	}
	if (DevStatus.reptype == DcPoints) 
	    endpoint();
	else
	    endline();
    }

    if (restore_lighting)
	ddr_gl_drom_set_att_srfshd (DcShaderLightSource);
    else if (vtxclrs)
	ddr_iris_restore_diffuse_color ();
}



/* ======================================================================== */

void ddr_gl_drom_render_triangle_list (
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
    static float white[3] = {1.,1.,1.};
    auto   int   t,t3;
    auto   int   idx;
    auto   short texture_mapping = 0;
    auto   short restore_lighting = 0;

    if ((DevStatus.traversal_number == 1 && DevStatus.frame_coord_prim) ||
	(DevStatus.traversal_number == 2 && !DevStatus.frame_coord_prim))
	    return;

    if (DevStatus.map_diffuse_color_switch && DevStatus.texture_2D &&
	uv_count > 0 && DevStatus.visualtype != DcPseudoColor) {
	idx = uv_count > DevStatus.texture_uv_index ? 
		DevStatus.texture_uv_index : 0;

	if (DevStatus.texture_op != DcTextureMultiply) {
	    ddr_iris_set_diffuse_color(white);
	    vtxclrs = (DtRealTriple *)0;
	}
	texture_mapping = 1;
	texbind(TX_TEXTURE_0, DevStatus.texture_2D);
    }

    if (!DevStatus.lighting || DevStatus.depcueswitch) {
	elenrms = 0;
	vtxnrms = 0;
    } else if (!vtxnrms && !elenrms) {
	restore_lighting = 1;
	ddr_gl_drom_set_att_srfshd (DcShaderConstant);
    }

    for (t=0, t3=0; t<trianglecount; t++, t3+=3) {
	int j;

	if (!DevStatus.smoothswitch) {
	    if (vtxclrs) ddr_iris_set_diffuse_color(vtxclrs[t3]);
	    if (elenrms) { SET_NORMAL(elenrms[t]); }
	    else if (vtxnrms) SET_NORMAL(vtxnrms[t3]);
	} else {
	    if (!(vtxnrms) && !(vtxclrs) && (elenrms)) 
		SET_NORMAL(elenrms[t]);
	}

	if (DevStatus.reptype == DcPoints)
	    bgnpoint();
	else if (DevStatus.reptype == DcWireframe ||
		 DevStatus.reptype == DcOutlines)
	    bgnclosedline();
	else
	    bgnpolygon();

	for (j=0; j<3; j++) {
	    if (texture_mapping)
		SET_TEXTURE (uv_list[idx][t3+j]);   

	    if (DevStatus.smoothswitch) {
		if (vtxclrs) ddr_iris_set_diffuse_color((vtxclrs)[t3+j]);
		if (vtxnrms) { SET_NORMAL(vtxnrms[t3+j]); }
		else if ((vtxclrs) && (elenrms)) SET_NORMAL(elenrms[t]);
	    }
	    SET_LOCATION (vtxlocs[t3+j]);
	}

	if (DevStatus.reptype == DcPoints)
	    endpoint();
	else if (  (DevStatus.reptype == DcWireframe)
		|| (DevStatus.reptype == DcOutlines)
		)
	    endclosedline();
	else
	    endpolygon();
    }

    if (restore_lighting)
	ddr_gl_drom_set_att_srfshd(DcShaderLightSource);
    else if (vtxclrs)
	ddr_iris_restore_diffuse_color ();

    if (texture_mapping) {
	if (DevStatus.texture_op != DcTextureMultiply) {
	    ddr_iris_restore_diffuse_color ();
	}
	texbind(TX_TEXTURE_0, 0);
    }
}



/* ======================================================================== */

void ddr_gl_drom_render_triangle_mesh (
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
    static float white[3] = {1.,1.,1.};
    auto   int   i;
    auto   int   idx;
    auto   short texture_mapping  = 0;
    auto   short restore_lighting = 0;

    if ((DevStatus.traversal_number == 1 && DevStatus.frame_coord_prim) ||
	(DevStatus.traversal_number == 2 && !DevStatus.frame_coord_prim))
	    return;

    if (  DevStatus.map_diffuse_color_switch && DevStatus.texture_2D
       && (uv_count > 0) && (DevStatus.visualtype != DcPseudoColor)
       ) {
	idx = (uv_count > DevStatus.texture_uv_index) ? 
		DevStatus.texture_uv_index : 0;

	if (DevStatus.texture_op != DcTextureMultiply) {
	    ddr_iris_set_diffuse_color(white);
	    vtxclrs = (DtRealTriple *)0;
	}
	texture_mapping = 1;
	texbind(TX_TEXTURE_0, DevStatus.texture_2D);
    }

    if (!DevStatus.lighting || DevStatus.depcueswitch) {
	elenrms = 0;
	vtxnrms = 0;
    } else if (!vtxnrms && !elenrms) {
	restore_lighting = 1;
	ddr_gl_drom_set_att_srfshd (DcShaderConstant);
    }

    for (i=0; i<trianglecount; i++) {
	int j;

	if (!DevStatus.smoothswitch) {
	    if (vtxclrs)
		ddr_iris_set_diffuse_color (vtxclrs[vertexlist[i][0]]);
	    if (elenrms)
		SET_NORMAL (elenrms[i]);
	    else if (vtxnrms)
		SET_NORMAL (vtxnrms[vertexlist[i][0]]);
	} else {
	    if (!(vtxnrms) && !(vtxclrs) && (elenrms)) 
		SET_NORMAL(elenrms[i]);
	}

	if (DevStatus.reptype == DcPoints)
	    bgnpoint();
	else if (DevStatus.reptype == DcWireframe ||
		 DevStatus.reptype == DcOutlines)
	    bgnclosedline();
	else
	    bgnpolygon();

	for (j=0; j<3; j++) {
	    if (texture_mapping)
		SET_TEXTURE (uv_list[idx][vertexlist[i][j]]);   
		    
	    if (DevStatus.smoothswitch) {
		if (vtxclrs)
		    ddr_iris_set_diffuse_color((vtxclrs)[vertexlist[i][j]]);
		if (vtxnrms)
		    SET_NORMAL(vtxnrms[vertexlist[i][j]]);
		else if ((vtxclrs) && (elenrms))
		    SET_NORMAL(elenrms[i]);
	    }
	    SET_LOCATION ((vtxlocs)[vertexlist[i][j]]);
	}

	if (DevStatus.reptype == DcPoints)
	    endpoint();
	else if (DevStatus.reptype == DcWireframe ||
		 DevStatus.reptype == DcOutlines)
	    endclosedline();
	else
	    endpolygon();
    }

    if (restore_lighting)
	ddr_gl_drom_set_att_srfshd (DcShaderLightSource);
    else if (vtxclrs)
	ddr_iris_restore_diffuse_color ();

    if (texture_mapping) {
	if (DevStatus.texture_op != DcTextureMultiply) {
	    ddr_iris_restore_diffuse_color ();
	}
	texbind(TX_TEXTURE_0, 0);
    }
}
