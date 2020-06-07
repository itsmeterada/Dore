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
	int dor_texintsrf_initialize ()
	dot_object * dor_texintsrf_create (mode)
	int dor_texintsrf_destroy (object)
	int dor_texintsrf_print (object)
	int dor_texintsrf_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texintsrf_data {
   DtObject  surface_object;
   Dt2PartProjType projection_type;
};

DtInt doe_texintsrf_class_id = -1;
DtFlag doe_texintsrf_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texintsrf_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texintsrf_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texintsrf_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texintsrf_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texintsrf_execute },
};

void dor_texintsrf_initialize (void)
{
    doe_texintsrf_class_id = dor_class_add ("DoTextureIntermediateSurf", 4,
		texintsrf_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texintsrf_create (
    DtObject surface_object,
    Dt2PartProjType projection_type)
{
    struct texintsrf_data *texintsrfdata;

    if (!doe_texintsrf_initialized) {
	dor_texintsrf_initialize();
	doe_texintsrf_initialized = DcTrue;
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(surface_object, dor_class_inq_id("DoPrimSurf"))
	&& !dor_object_confirm_type(surface_object, 
	dor_class_inq_id("DoSimplePolygon"))) {

	DDerror(ERR_BAD_OBJECT, "DoTextureIntermediateSurf",
                "surface_object must be DoPrimSurf or DoSimplePolygon object");
        return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif

    texintsrfdata = (struct texintsrf_data *)dor_space_allocate(
		sizeof *texintsrfdata);

    texintsrfdata->surface_object = surface_object;
    texintsrfdata->projection_type = projection_type;

    return (dor_object_create(doe_texintsrf_class_id,texintsrfdata));
}
/*
 ======================================================================
 */

void dor_texintsrf_destroy (
    dot_object *object)
{
    struct texintsrf_data *texintsrf;

    texintsrf = (struct texintsrf_data *)(object->data);

    dor_space_deallocate(texintsrf);
}
/*
 ======================================================================
 */

void dor_texintsrf_print (
    dot_object *object)
{
    struct texintsrf_data *texintsrf;

    texintsrf = (struct texintsrf_data *)object->data;

    if (texintsrf == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"surface_object = [%lx]:", 
		texintsrf->surface_object);
    dor_print_output(dor_print_get_line());

    switch(texintsrf->projection_type) {
    case DcObjCentroid:
	dor_print_output ("projection_type = DcObjCentroid");
	break;
    case DcISN:
	dor_print_output ("projection_type = DcISN");
	break;
    default:
	sprintf(dor_print_get_line(),"projection_type = <UNKNOWN TYPE> %d",
		texintsrf->projection_type);
	dor_print_output(dor_print_get_line());
	break;
    }
}
/*
 ======================================================================
 */

void dor_texintsrf_execute (
    dot_object *object)
{
    struct texintsrf_data *texintsrf;

    if (!dor_condex_query_execute(doe_texintsrf_class_id))
	    return;

    texintsrf = (struct texintsrf_data *)object->data;

    dor_global_texintsrf_set_value(texintsrf->surface_object,
		texintsrf->projection_type);
}
