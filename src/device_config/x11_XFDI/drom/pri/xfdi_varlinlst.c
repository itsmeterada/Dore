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
	int ddr_x11_XFDI_drom_varlinlst_render (object)

  ======================================================================
 */
#include "../drom.h"
#include "dore/dore_develop/private/varlinlst.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_varlinlst_render(object)
     dot_object *object;
{
    extern DtFlag DDcondex_QueryRender ();
    struct varlinlst *varlinlst;
    static DtInt classid = -1;
    static DtInt dataindex = -1;
    DtInt i;
    ddt_x11_XFDI_drom_cvtclrs_data *colordata;

    if (classid == -1) {
	classid = DsInqClassId("DoVarLineList");
    }

    if (!DDcondex_QueryRender(classid))
	    return;

    varlinlst = (struct varlinlst *)object->data;

    if (varlinlst->line_count == 0) {
	return;
    }

    doe_system_timing_lines_drawn += varlinlst->line_count;

    if (dataindex == -1) {
	for (i=0; dde_x11_XFDI_drom_class_data[i].name != 0; i++) {
	    if (dde_x11_XFDI_drom_class_data[i].id == classid) {
		dataindex = dde_x11_XFDI_drom_class_data[i].index;
		break;
	    }
	}
    }

    colordata = (ddt_x11_XFDI_drom_cvtclrs_data *)
	    object->additional_data[dataindex];

    if (varlinlst->vertex_color != NULL) {

	if (colordata->converted_clrs == NULL) {
	    colordata->converted_clrs = (unsigned long *) DDspace_Allocate
		    (varlinlst->line_count*2*sizeof(unsigned long));
	    ddr_x11_XFDI_drom_pri_convert_colors(varlinlst->line_count*2,
						  varlinlst->vertex_color,
						  colordata->converted_clrs);
	    colordata->sequence_number = varlinlst->sequence_number;
	    colordata->shade_mode      = dde_x11_XFDI_drom_curdevdat->shade_mode;
	    colordata->visual_type     = dde_x11_XFDI_drom_curdevdat->visual_type;
	    colordata->shade_index     = dde_x11_XFDI_drom_data.shade_index;
	} else if (colordata->sequence_number != varlinlst->sequence_number ||
		   colordata->shade_mode != dde_x11_XFDI_drom_curdevdat->shade_mode ||
		   colordata->visual_type != dde_x11_XFDI_drom_curdevdat->visual_type ||
		   colordata->shade_index != dde_x11_XFDI_drom_data.shade_index) {
	    colordata->converted_clrs = (unsigned long *) DDspace_Reallocate
		    (colordata->converted_clrs,
		     varlinlst->line_count*2*sizeof(unsigned long));
	    ddr_x11_XFDI_drom_pri_convert_colors(varlinlst->line_count*2, 
						  varlinlst->vertex_color,
						  colordata->converted_clrs);
	    colordata->sequence_number = varlinlst->sequence_number;
	    colordata->shade_mode      = dde_x11_XFDI_drom_curdevdat->shade_mode;
	    colordata->visual_type     = dde_x11_XFDI_drom_curdevdat->visual_type;
	    colordata->shade_index     = dde_x11_XFDI_drom_data.shade_index;
	}
    }

    if (dde_x11_XFDI_drom_data.reptype == DcPoints) {
	ddr_x11_XFDI_drom_output_pntlst(varlinlst->bounding_box_pts,
					 varlinlst->line_count*2,
					 DcNullPtr,
					 varlinlst->vertex_location,
					 varlinlst->vertex_normal,
					 colordata->converted_clrs);
    } else {
	ddr_x11_XFDI_drom_output_linlst(varlinlst->bounding_box_pts,
					 varlinlst->line_count,
					 DcNullPtr,
					 varlinlst->vertex_location,
					 varlinlst->vertex_normal,
					 colordata->converted_clrs);
    }

}
