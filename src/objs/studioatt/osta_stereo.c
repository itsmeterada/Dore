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
	int dor_stereo_initialize ()
	dot_object * dor_stereo_create (eyeseparation, distance)
	int dor_stereo_destroy (object)
	int dor_stereo_print (object)
	int dor_stereo_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct stereo_data {
   DtReal eyeseparation;
   DtReal distance;
};
/*
 ======================================================================
 */

DtInt doe_stereo_class_id;

static DtMethodEntry stereo_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_stereo_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_stereo_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_stereo_execute },
};

void dor_stereo_initialize (void)
{
    doe_stereo_class_id = dor_class_install (DcTypeStereo, "DoStereo", 3,
					     stereo_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_stereo_create (
    DtReal eyeseparation,
    DtReal distance)
{
    static DtFlag initialized = DcFalse;
    struct stereo_data *stereodata;
    dot_object *stereoobject;

    if (!initialized) {
	dor_stereo_initialize();
	initialized = DcTrue;
    }

    stereodata = (struct stereo_data *)dor_space_allocate(sizeof *stereodata);

    stereodata->eyeseparation = eyeseparation;
    stereodata->distance = distance;

    stereoobject = dor_object_create(doe_stereo_class_id,stereodata);

    return(stereoobject);
}
/*
 ======================================================================
 */

void dor_stereo_destroy (dot_object *object)
{
    struct stereo_data *stereo;

    stereo = (struct stereo_data *)(object->data);

    dor_space_deallocate(stereo);
}
/*
 ======================================================================
 */

void dor_stereo_print (dot_object *object)
{
    struct stereo_data *stereo;

    stereo = (struct stereo_data *)object->data;

    if (stereo == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"eyeseparation = %lf",stereo->eyeseparation);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"distance = %lf",stereo->distance);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_stereo_execute (dot_object *object)
{
    struct stereo_data *stereodat;

    if (!dor_condex_query_execute(doe_stereo_class_id))
	    return;

    stereodat = (struct stereo_data *)object->data;

    dor_global_stereo_set_value(stereodat->eyeseparation, 
				stereodat->distance);
}
