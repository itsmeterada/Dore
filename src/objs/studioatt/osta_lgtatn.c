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
	int dor_lgtatn_initialize ()
	dot_object * dor_lgtatn_create (c1, c2)
	int dor_lgtatn_destroy (object)
	int dor_lgtatn_print (object)
	int dor_lgtatn_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct lgtatn_data {
    DtReal c1;
    DtReal c2;
};
/*
 ======================================================================
 */

DtInt doe_lgtatn_class_id;

static DtMethodEntry lgtatn_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_lgtatn_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_lgtatn_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_lgtatn_execute },
};

void dor_lgtatn_initialize (void)
{
    doe_lgtatn_class_id = dor_class_add ("DoLightAttenuation", 
					 3, lgtatn_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_lgtatn_create (
    DtReal c1,
    DtReal c2)
{
    static DtFlag initialized = DcFalse;
    struct lgtatn_data *lgtatndata;
    dot_object *lgtatn_object;

    if (!initialized) {
	dor_lgtatn_initialize();
	initialized = DcTrue;
    }

    lgtatndata = (struct lgtatn_data *)dor_space_allocate(sizeof *lgtatndata);
	
    lgtatndata->c1 = c1;
    lgtatndata->c2 = c2;

    lgtatn_object = dor_object_create(doe_lgtatn_class_id,lgtatndata);

    return(lgtatn_object);
}
/*
 ======================================================================
 */

void dor_lgtatn_destroy (dot_object *object)
{
    struct lgtatn_data *lgtatn;

    lgtatn = (struct lgtatn_data *)(object->data);

    dor_space_deallocate(lgtatn);
}
/*
 ======================================================================
 */

void dor_lgtatn_print (dot_object *object)
{
    struct lgtatn_data *lgtatn;

    lgtatn = (struct lgtatn_data *)object->data;

    if (lgtatn == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"attenuation c1 = %lf",lgtatn->c1);
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(),"attenuation c2 = %lf",lgtatn->c2);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_lgtatn_execute (dot_object *object)
{
    struct lgtatn_data *lgtatndat;

    if (!dor_condex_query_execute(doe_lgtatn_class_id))
	    return;

    lgtatndat = (struct lgtatn_data *)object->data;

    dor_global_lgtatn_set_value(lgtatndat->c1, lgtatndat->c2);
}
