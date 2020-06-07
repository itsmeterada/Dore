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
	int ddr_x11_XFDI_drom_vartrimsh_render (object)

  ======================================================================
 */
#include "../drom.h"
#include "dore/dore_develop/private/vartrimsh.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_vartrimsh_render(object)
     dot_object *object;
{
    struct vartrimsh *vartrimsh;
    static DtInt classid = -1;
    static DtInt dataindex = -1;
    ddt_x11_XFDI_drom_cvtclrs_data *colordata;
    DtInt i;
    DtInt triangleno;
    DtObject shader;
    DtInterpType interptype;
    DtPFI routine;
    dot_object *alt;
    extern DtFlag DDcondex_QueryRender ();
    extern DtPFI DDclass_InqMethod();
    extern DtPFI DDobject_InqMethod();
    extern DtPFI DDobject_InqCurrentMethod();

    if (classid == -1) {
	classid = DsInqClassId("DoVarTriangleMesh");
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

    vartrimsh = (struct vartrimsh *)object->data;

    if (vartrimsh->compiled_triangle_vertexlist == NULL) {
	vartrimsh->compiled_triangle_vertexlist =
		(vartrimsh_vertexlist *)DDspace_Allocate
			(vartrimsh->triangle_count*(sizeof (vartrimsh_vertexlist)));

	for (triangleno=0; triangleno<vartrimsh->triangle_count; 
	     triangleno++){
	    vartrimsh->compiled_triangle_vertexlist[triangleno][0] =
		    vartrimsh->triangle_vertexlist[triangleno][0] * 3;
	    vartrimsh->compiled_triangle_vertexlist[triangleno][1] =
		    vartrimsh->triangle_vertexlist[triangleno][1] * 3;
	    vartrimsh->compiled_triangle_vertexlist[triangleno][2] =
		    vartrimsh->triangle_vertexlist[triangleno][2] * 3;
	}
    }

    DDclass_InqMethod(DcTypeAttSurfaceShader, DcMethodInqGlbAttVal)(&shader);
    if (shader != DcShaderConstant) {
	if (vartrimsh->needelemnrms) {
	    if (vartrimsh->element_normal == NULL) {
		vartrimsh->element_normal = 
			(DtRealTriple *)dor_space_allocate
				(vartrimsh->triangle_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartrimsh_calculate_triangle_normals(vartrimsh);
	    vartrimsh->needelemnrms = DcFalse;
	}

	DDclass_InqMethod(DcTypeAttInterpType, DcMethodInqGlbAttVal)(&interptype);
	if ((interptype == DcVertexShade) && vartrimsh->needvertnrms) {
	    if (vartrimsh->vertex_normal == NULL) {
		vartrimsh->vertex_normal =
			(DtRealTriple *)dor_space_allocate
				(vartrimsh->vertex_count*(sizeof (DtRealTriple)));
	    }
	    dor_vartrimsh_calculate_vertex_normals(vartrimsh);
	    vartrimsh->needvertnrms = DcFalse;
	}
    }

    if (vartrimsh->vertex_color != NULL) {
	if (colordata->converted_clrs == NULL) {
	    colordata->converted_clrs = (unsigned long *)DDspace_Allocate
		    (vartrimsh->vertex_count*sizeof(unsigned long));
	    colordata->sequence_number = vartrimsh->sequence_number;
	    colordata->shade_mode  = dde_x11_XFDI_drom_curdevdat->shade_mode;
	    colordata->visual_type = dde_x11_XFDI_drom_curdevdat->visual_type;
	    colordata->shade_index = dde_x11_XFDI_drom_data.shade_index;

	} else if (colordata->sequence_number != vartrimsh->sequence_number ||
		   colordata->shade_mode != dde_x11_XFDI_drom_curdevdat->shade_mode ||
		   colordata->visual_type != dde_x11_XFDI_drom_curdevdat->visual_type ||
		   colordata->shade_index != dde_x11_XFDI_drom_data.shade_index) {
	    ddr_x11_XFDI_drom_pri_convert_colors(vartrimsh->vertex_count, 
						  vartrimsh->vertex_color,
						  colordata->converted_clrs);
	    colordata->sequence_number = vartrimsh->sequence_number;
	    colordata->shade_mode  = dde_x11_XFDI_drom_curdevdat->shade_mode;
	    colordata->visual_type = dde_x11_XFDI_drom_curdevdat->visual_type;
	    colordata->shade_index = dde_x11_XFDI_drom_data.shade_index;
	}
    }

    if (dde_x11_XFDI_drom_data.reptype == DcPoints) {
	ddr_x11_XFDI_drom_output_pntlst(vartrimsh->bounding_box_pts,
					 vartrimsh->vertex_count,
					 NULL,
					 vartrimsh->vertex_location,
					 vartrimsh->vertex_normal,
					 colordata->converted_clrs);

    } else if (dde_x11_XFDI_drom_data.reptype == DcWireframe ||
	       dde_x11_XFDI_drom_data.reptype == DcOutlines) {

	routine = DDobject_InqMethod (object, DcMethodUpdStdAltLineObj);
	alt = (dot_object *) (*routine)(object);
	if (alt != dod_object_null) {
	    DDobject_InqCurrentMethod(alt)(alt);
	}

    } else			/* reptype == DcSurface */ {

	ddr_x11_XFDI_drom_output_trimsh(vartrimsh->bounding_box_pts,
					 vartrimsh->triangle_count,
					 vartrimsh->element_normal,
					 vartrimsh->vertex_count,
					 vartrimsh->vertex_location,
					 vartrimsh->vertex_normal,
					 colordata->converted_clrs,
					 vartrimsh->triangle_vertexlist,
					 vartrimsh->compiled_triangle_vertexlist);
    }
}
