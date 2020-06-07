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
	DtPtr ddr_x11_XFDI_drom_trimsh_create (object)
	int ddr_x11_XFDI_drom_trimsh_print (object, data)
	int ddr_x11_XFDI_drom_trimsh_delete (object, data)
	dot_object * ddr_x11_XFDI_drom_render_trimsh_create_alternate (trimsh)
	int ddr_x11_XFDI_drom_trimsh_render (object)

  ======================================================================
 */
#include "../drom.h"
#include "dore/dore_develop/private/trimsh.h"
/*
 ======================================================================
 */
DtPtr
ddr_x11_XFDI_drom_trimsh_create(object)
     DtObject object;
{
    return (DcNullPtr);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_trimsh_print(object, data)
     DtObject object;
     DtObjectStructure *data;
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_trimsh_delete(object, data)
     DtObject object;
     DtObjectStructure *data;
{
    if (data) DeDeleteObject(data);
}
/*
 ======================================================================
 */
dot_object *ddr_x11_XFDI_drom_render_trimsh_create_alternate(trimsh)
struct trimsh *trimsh;

{
    dot_object *tristrobj;
    dot_object *dor_tristrip_create_from_mesh();
    DtVertexType vertextype;

    vertextype = trimsh->vertextype;
    if (trimsh->smoothflag) {
   	if (trimsh->vertextype == DcLoc)
   		vertextype = DcLocNrm;
   	else if (trimsh->vertextype == DcLocClr)
   		vertextype = DcLocNrmClr;
    }

    tristrobj = dor_tristrip_create_from_mesh(
					      trimsh->colormodel,
					      vertextype |
					      dor_system_texture_uv_count(trimsh->uv_count) |
					      dor_system_texture_uvw_count(trimsh->uvw_count),
					      trimsh->triangle_count,
					      trimsh->triangle_vertexlist,
					      trimsh->element_normal,
					      trimsh->vertex_location,
					      trimsh->vertex_normal,
					      trimsh->vertex_color,
					      trimsh->uv_list,
					      trimsh->uvw_list,
					      DcTrue);

    return(tristrobj);
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_trimsh_render(object)
dot_object *object;

{
    struct trimsh *trimsh;
    DtPFI DDobject_InqMethod();
    DtPFI DDobject_InqCurrentMethod();
    static DtInt dataindex = -1;
    static DtInt classid = -1;
    DtPFI routine;
    DtObjectStructure *ddr_x11_XFDI_drom_render_trimsh_create_alternate();
    DtObjectStructure *alt_obj;
    DtInt i;

    classid = DsInqClassId("DoTriangleMesh");

    if (!DDcondex_QueryRender(classid))
	    return;

    if (dataindex == -1) {
	for (i=0; dde_x11_XFDI_drom_class_data[i].name != 0; i++) {
	    if (dde_x11_XFDI_drom_class_data[i].id == classid)
		    dataindex = dde_x11_XFDI_drom_class_data[i].index;
	    continue;
 	}
    }

    trimsh = (struct trimsh *)(object->data);

    alt_obj = dod_object_null;

    if (dde_x11_XFDI_drom_data.reptype == DcPoints ||
   	dde_x11_XFDI_drom_data.reptype == DcWireframe ||
   	dde_x11_XFDI_drom_data.reptype == DcOutlines) {

	routine = DDobject_InqMethod(object, DcMethodUpdStdAltLineObj);
	alt_obj = (DtObjectStructure *) (*routine)(object);
  	if (alt_obj != dod_object_null)
   		DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	return;
    }

    alt_obj = (DtObjectStructure *)object->additional_data[dataindex];	
    if (alt_obj != dod_object_null) {
   	DDobject_InqCurrentMethod(alt_obj) (alt_obj);

   	return;

    } else {
   	alt_obj = ddr_x11_XFDI_drom_render_trimsh_create_alternate(trimsh);
   	object->additional_data[dataindex] = (DtPtr)alt_obj;

   	if (alt_obj == dod_object_null) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "ddr_x11_XFDI_drom_trimsh_render", "");
	    return;
   	}

   	DDobject_InqCurrentMethod(alt_obj)(alt_obj);

   	return;
    }
}
