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
	void ddr_template_drom_render_point_list (colormodel,bndboxpts,pointcount,
		vtxlocs,vtxnrms,vtxclrs, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_render_line_list (colormodel,bndboxpts,linecount,elenrms,
		vtxlocs, vtxnrms,vtxclrs, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_render_connected_line_list (colormodel,bndboxpts,vertexcount, 
		elenrms,vtxlocs,vtxnrms, vtxclrs, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_render_triangle_list (colormodel,bndboxpts,trianglecount,
		elenrms, vtxlocs,vtxnrms,vtxclrs, 
		uv_count, uv_list, uvw_count, uvw_list)
	void ddr_template_drom_render_triangle_mesh (colormodel,bndboxpts,trianglecount,
		elenrms, vertexcount,vtxlocs,vtxnrms, vtxclrs,vertexlist, 
		compiledvertexlist, 
		uv_count, uv_list, uvw_count, uvw_list)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void ddr_template_drom_render_point_list (
    DtColorModel colormodel,
    DtRealTriple bndboxpts[8],
    DtInt pointcount,
    DtRealTriple *vtxlocs,
    DtRealTriple *vtxnrms,
    DtRealTriple *vtxclrs,
    DtInt uv_count,
    DtRealCouple **uv_list,
    DtInt uvw_count,
    DtRealTriple **uvw_list)
{
#ifdef DEBUG
    ddr_template_drom_print_point_list(bndboxpts,pointcount, (DtRealTriple *)0,
				       vtxlocs, vtxnrms, vtxclrs,
				       uv_count, uv_list, uvw_count, uvw_list);
#endif
}
/*
 ======================================================================
 */
void ddr_template_drom_render_line_list (
    DtColorModel colormodel,
    DtRealTriple bndboxpts[8],
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
#ifdef DEBUG
    ddr_template_drom_print_line_list(bndboxpts, linecount, elenrms,
				      vtxlocs, vtxnrms, vtxclrs,
				      uv_count, uv_list, uvw_count, uvw_list);
#endif
}
/*
 ======================================================================
 */
void ddr_template_drom_render_connected_line_list (
    DtColorModel colormodel,
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
#ifdef DEBUG
    ddr_template_drom_print_connected_line_list(bndboxpts,vertexcount,
						elenrms,vtxlocs,vtxnrms,
						vtxclrs, uv_count, uv_list, 
						uvw_count, uvw_list);
#endif
}
/*
 ======================================================================
 */
void ddr_template_drom_render_triangle_list (
    DtColorModel colormodel,
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
#ifdef DEBUG
    ddr_template_drom_print_triangle_list(bndboxpts,trianglecount,elenrms,
					  vtxlocs,vtxnrms,vtxclrs,
					  uv_count, uv_list, uvw_count, uvw_list);
#endif
}
/*
 ======================================================================
 */
void ddr_template_drom_render_triangle_mesh (
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
    DtRealCouple **uv_list,
    DtInt uvw_count,
    DtRealTriple **uvw_list)
{
#ifdef DEBUG
    ddr_template_drom_print_triangle_mesh(bndboxpts,trianglecount,elenrms,
					  vertexcount,vtxlocs,vtxnrms,
					  vtxclrs,vertexlist,
					  compiledvertexlist,
					  uv_count, uv_list, uvw_count, uvw_list);
#endif    
}
/*
 ======================================================================
 */
