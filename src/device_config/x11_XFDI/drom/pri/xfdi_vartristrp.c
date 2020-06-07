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
	int ddr_x11_XFDI_drom_vartristrip_render (object)

  ======================================================================
 */
#include "../drom.h"
#include "dore/dore_develop/private/vartristrp.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_vartristrip_render(object)
     dot_object *object;
{
    struct vartristrip *vartristrip;
    static DtInt classid = -1;
    static DtInt dataindex = -1;
    DtInt i;
    DtPFI routine;
    dot_object *alt;
    unsigned long *colors;
    DtInt sequence_number;
    ddt_x11_XFDI_drom_cvtclrs_data *colordata;
    extern DtFlag DDcondex_QueryRender();
    extern DtPFI DDobject_InqMethod();
    extern DtPFI DDobject_InqCurrentMethod();

    if (classid == -1) {
	classid = DsInqClassId("DoVarTriangleStrip");
    }

    if (!DDcondex_QueryRender(classid))
	    return;

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
  
    vartristrip = (struct vartristrip *)(object->data);

    if (vartristrip->vertex_color != NULL) {
	if (colordata->converted_clrs == NULL) { 
	    colordata->converted_clrs = (unsigned long *) DDspace_Allocate
		    (vartristrip->vertex_count*sizeof(unsigned long));
	    ddr_x11_XFDI_drom_pri_convert_colors(vartristrip->vertex_count, 
						  vartristrip->vertex_color,
						  colordata->converted_clrs);
	    colordata->sequence_number = vartristrip->sequence_number;
	    colordata->shade_mode      = dde_x11_XFDI_drom_curdevdat->shade_mode;
	    colordata->visual_type     = dde_x11_XFDI_drom_curdevdat->visual_type;
	    colordata->shade_index     = dde_x11_XFDI_drom_data.shade_index;

	} else if (colordata->sequence_number != vartristrip->sequence_number ||
		   colordata->shade_mode != dde_x11_XFDI_drom_curdevdat->shade_mode ||
		   colordata->visual_type != dde_x11_XFDI_drom_curdevdat->visual_type ||
		   colordata->shade_index != dde_x11_XFDI_drom_data.shade_index) {
	    colordata->converted_clrs = (unsigned long *) DDspace_Reallocate
		    (colordata->converted_clrs,
		     vartristrip->vertex_count*sizeof(unsigned long));
	    ddr_x11_XFDI_drom_pri_convert_colors(vartristrip->vertex_count, 
						  vartristrip->vertex_color,
						  colordata->converted_clrs);
	    colordata->sequence_number = vartristrip->sequence_number;
	    colordata->shade_mode      = dde_x11_XFDI_drom_curdevdat->shade_mode;
	    colordata->visual_type     = dde_x11_XFDI_drom_curdevdat->visual_type;
	    colordata->shade_index     = dde_x11_XFDI_drom_data.shade_index;
	}
    }

    if (dde_x11_XFDI_drom_data.reptype == DcPoints) {
	ddr_x11_XFDI_drom_output_pntlst(vartristrip->bounding_box_pts,
					 vartristrip->vertex_count,
					 NULL,
					 vartristrip->vertex_location,
					 vartristrip->vertex_normal,
					 colordata->converted_clrs);

    } else if (dde_x11_XFDI_drom_data.reptype == DcWireframe ||
	       dde_x11_XFDI_drom_data.reptype == DcOutlines) {

	routine = DDobject_InqMethod (object, DcMethodUpdStdAltLineObj);
	alt = (dot_object *) (*routine)(object);
	if (alt != dod_object_null) {
	    DDobject_InqCurrentMethod(alt)(alt);
	}

    } else			/* reptype == DcSurface */ { 

	ddr_x11_XFDI_drom_output_tristrip (vartristrip->bounding_box_pts,
					    vartristrip->vertex_count,
					    vartristrip->vertex_location,
					    vartristrip->vertex_normal,
					    colordata->converted_clrs,
					    0,
					    (DtRealCouple **)0,
					    0,
					    (DtRealTriple **)0);

    }
}
