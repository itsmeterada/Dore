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
	int dor_depcue_initialize ()
	dot_object * dor_depcue_create (zfront,zback,sfront,sback,colormodel,color)
	int dor_depcue_destroy (object)
	int dor_depcue_print (object)
	int dor_depcue_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct depcue_data {
    DtReal zfront;
    DtReal zback;
    DtReal sfront;
    DtReal sback;
    DtColorModel background_colormodel;
    DtColorRGB background_color;
};
/*
 ======================================================================
 */

DtInt doe_depcue_class_id;

static DtMethodEntry depcue_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_depcue_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_depcue_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_depcue_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_depcue_execute },
};

void dor_depcue_initialize (void)
{
    doe_depcue_class_id = dor_class_install (DcTypeDepthCue, "DoDepthCue", 4,
					     depcue_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_depcue_create (
    DtReal       zfront,
    DtReal       zback,
    DtReal       sfront,
    DtReal       sback,
    DtColorModel colormodel,
    DtReal       color[])
{
    static DtFlag initialized = DcFalse;
    dot_object *depcueobject;
    struct depcue_data *depcuedata;

    if (!initialized) {
	dor_depcue_initialize();
	initialized = DcTrue;
    }

    depcuedata = (struct depcue_data *)dor_space_allocate(sizeof *depcuedata);

    depcuedata->zfront = zfront;
    depcuedata->zback = zback;
    depcuedata->sfront = sfront;
    depcuedata->sback = sback;

    depcuedata->background_colormodel = colormodel;

    depcuedata->background_color[0] = color[0];
    depcuedata->background_color[1] = color[1];
    depcuedata->background_color[2] = color[2];

    depcueobject = dor_object_create(doe_depcue_class_id,depcuedata);

    return(depcueobject);
}
/*
 ======================================================================
 */

void dor_depcue_destroy (dot_object *object)
{
    struct depcue_data *depcue;

    depcue = (struct depcue_data *)(object->data);

    dor_space_deallocate(depcue);
}
/*
 ======================================================================
 */

void dor_depcue_print (dot_object *object)
{
    struct depcue_data *depcuedata;

    depcuedata = (struct depcue_data *)object->data;

    if (depcuedata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "(zfront,zback) (sfront,sback) = (%lf,%lf) (%lf,%lf)\n",
	    depcuedata->zfront,depcuedata->zback,
	    depcuedata->sfront,depcuedata->sback);
    dor_print_output(dor_print_get_line());

    switch (depcuedata->background_colormodel) {
    case DcRGB:
	dor_print_output("background colormodel = DcRGB");
	break;

    default:
	dor_print_output("background colormodel = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),
	    "background color = (%f,%f,%f)\n", 
	    depcuedata->background_color[0],
	    depcuedata->background_color[1],
	    depcuedata->background_color[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_depcue_execute (dot_object *object)
{
    struct depcue_data *depcuedata;

    if (!dor_condex_query_execute(doe_depcue_class_id))
	    return;

    depcuedata = (struct depcue_data *)object->data;

    dor_global_depcue_set_value(depcuedata->zfront,depcuedata->zback,
				depcuedata->sfront,depcuedata->sback,
				depcuedata->background_colormodel,
				depcuedata->background_color);
}
