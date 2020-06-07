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
	int dor_texscluvw_initialize ()
	dot_object * dor_texscluvw_create (su, sv, sw)
	int dor_texscluvw_destroy (object)
	int dor_texscluvw_print (object)
	int dor_texscluvw_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>

struct texscluvw_data {
   DtReal su;
   DtReal sv;
   DtReal sw;
};

DtInt doe_texscluvw_class_id = -1;
DtFlag doe_texscluvw_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry texscluvw_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_texscluvw_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_texscluvw_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_texscluvw_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_texscluvw_execute },
};

void dor_texscluvw_initialize (void)
{
    doe_texscluvw_class_id = dor_class_add ("DoTextureScaleUVW", 4,
					    texscluvw_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_texscluvw_create (
    DtReal su,
    DtReal sv,
    DtReal sw)
{
    struct texscluvw_data *texscluvwdata;

    if (!doe_texscluvw_initialized) {
	dor_texscluvw_initialize();
	doe_texscluvw_initialized = DcTrue;
    }

    texscluvwdata = (struct texscluvw_data *)dor_space_allocate(sizeof *texscluvwdata);

    texscluvwdata->su = su;
    texscluvwdata->sv = sv;
    texscluvwdata->sw = sw;

    return (dor_object_create(doe_texscluvw_class_id,texscluvwdata));
}
/*
 ======================================================================
 */

void dor_texscluvw_destroy (dot_object *object)
{
    struct texscluvw_data *texscluvw;

    texscluvw = (struct texscluvw_data *)(object->data);

    dor_space_deallocate(texscluvw);
}
/*
 ======================================================================
 */

void dor_texscluvw_print (dot_object *object)
{
    struct texscluvw_data *texscluvw;

    texscluvw = (struct texscluvw_data *)object->data;

    if (texscluvw == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"scale = %lf %lf %lf", texscluvw->su,
	    texscluvw->sv, texscluvw->sw);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_texscluvw_execute (dot_object *object)
{
    struct texscluvw_data *texscluvwdat;
    DtMatrix4x4            newmatrix;

    if (!dor_condex_query_execute(doe_texscluvw_class_id))
	    return;

    texscluvwdat = (struct texscluvw_data *)object->data;

    /* Overwrite the global UVW texture matrix scale factors. */
    dor_global_texmatuvw_get_value(newmatrix);
    newmatrix[0][0] = texscluvwdat->su;
    newmatrix[1][1] = texscluvwdat->sv;
    newmatrix[2][2] = texscluvwdat->sw;
    dor_global_texmatuvw_set_value(newmatrix);
}
