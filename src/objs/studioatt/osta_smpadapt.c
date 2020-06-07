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
	int dor_smpadapt_initialize ()
	dot_object * dor_smpadapt_create (variance)
	int dor_smpadapt_destroy (object)
	int dor_smpadapt_print (object)
	int dor_smpadapt_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct smpadapt_data {
   DtReal variance;
};

DtInt doe_smpadapt_class_id = -1;
DtFlag doe_smpadapt_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry smpadapt_methods[] = {
    { DcMethodDestroy,         (DtMethodPtr) dor_smpadapt_destroy },
    { DcMethodPick,            (DtMethodPtr) dor_smpadapt_execute },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_smpadapt_execute },
};

void dor_smpadapt_initialize (void)
{
    doe_smpadapt_class_id = dor_class_add ("DoSampleAdaptive", 3,
					   smpadapt_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_smpadapt_create (DtReal variance)
{
    struct smpadapt_data *smpadaptdata;

    if (!doe_smpadapt_initialized) {
	dor_smpadapt_initialize();
	doe_smpadapt_initialized = DcTrue;
    }

    smpadaptdata = (struct smpadapt_data *)dor_space_allocate(sizeof *smpadaptdata);

    smpadaptdata->variance = variance;

    return (dor_object_create(doe_smpadapt_class_id,smpadaptdata));
}
/*
 ======================================================================
 */

void dor_smpadapt_destroy (dot_object *object)
{
    struct smpadapt_data *smpadapt;

    smpadapt = (struct smpadapt_data *)(object->data);

    dor_space_deallocate(smpadapt);
}
/*
 ======================================================================
 */

void dor_smpadapt_print (dot_object *object)
{
    struct smpadapt_data *smpadapt;

    smpadapt = (struct smpadapt_data *)object->data;

    if (smpadapt == NULL) {
	dor_print_output("bad values");
	return;
    }
  
    sprintf(dor_print_get_line(),"variance = %lf\n", smpadapt->variance);
    dor_print_output(dor_print_get_line());

}
/*
 ======================================================================
 */

void dor_smpadapt_execute (dot_object *object)
{
    struct smpadapt_data *smpadaptdat;

    if (!dor_condex_query_execute(doe_smpadapt_class_id))
	    return;

    smpadaptdat = (struct smpadapt_data *)object->data;

    dor_global_smpadapt_set_value(smpadaptdat->variance);
}
