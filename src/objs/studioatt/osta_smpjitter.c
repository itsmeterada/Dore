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
	int dor_smpjitter_initialize ()
	dot_object * dor_smpjitter_create (factor)
	int dor_smpjitter_destroy (object)
	int dor_smpjitter_print (object)
	int dor_smpjitter_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct smpjitter_data {
   DtReal factor;
};

DtInt doe_smpjitter_class_id = -1;
DtFlag doe_smpjitter_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry smpjitter_methods[] = {
    { DcMethodDestroy,         (DtMethodPtr) dor_smpjitter_destroy },
    { DcMethodPick,            (DtMethodPtr) dor_smpjitter_execute },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_smpjitter_execute },
};

void dor_smpjitter_initialize (void)
{
    doe_smpjitter_class_id = dor_class_add ("DoSampleJitter", 3,
					    smpjitter_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_smpjitter_create (DtReal factor)
{
    struct smpjitter_data *smpjitterdata;

    if (!doe_smpjitter_initialized) {
	dor_smpjitter_initialize();
	doe_smpjitter_initialized = DcTrue;
    }

    smpjitterdata = (struct smpjitter_data *)dor_space_allocate(sizeof *smpjitterdata);

    smpjitterdata->factor = factor;

    return (dor_object_create(doe_smpjitter_class_id,smpjitterdata));
}
/*
 ======================================================================
 */

void dor_smpjitter_destroy  (dot_object *object)
{
    struct smpjitter_data *smpjitter;

    smpjitter = (struct smpjitter_data *)(object->data);

    dor_space_deallocate(smpjitter);
}
/*
 ======================================================================
 */

void dor_smpjitter_print (dot_object *object)
{
    struct smpjitter_data *smpjitter;

    smpjitter = (struct smpjitter_data *)object->data;

    if (smpjitter == NULL) {
	dor_print_output("bad values");
	return;
    }
  
    sprintf (dor_print_get_line(),"factor = %lf\n", smpjitter->factor);
    dor_print_output(dor_print_get_line());

}
/*
 ======================================================================
 */

void  dor_smpjitter_execute (dot_object *object)
{
    struct smpjitter_data *smpjitterdat;

    if (!dor_condex_query_execute(doe_smpjitter_class_id))
	    return;

    smpjitterdat = (struct smpjitter_data *)object->data;

    dor_global_smpjitter_set_value(smpjitterdat->factor);
}
