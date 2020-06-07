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
	int dor_txtupvec_initialize ()
	struct object * dor_txtupvec_create (xup, yup)
	int dor_txtupvec_print (object)
	int dor_txtupvec_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

/*
 ======================================================================
 */

DtInt doe_txtupvec_class_id;

static DtMethodEntry txtupvec_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_txtupvec_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_txtupvec_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_txtupvec_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_txtupvec_execute },
};

void dor_txtupvec_initialize (void)
{
    doe_txtupvec_class_id = dor_class_install (DcTypeTextUpVector, "DoTextUpVec", 4,
					       txtupvec_methods,DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_txtupvec_create (
    DtReal xup,
    DtReal yup)
{
    static DtFlag initialized = DcFalse;
    dot_txtupvec *txtupvecdata;
    struct object *txtupvecobject;

    if (!initialized) {
	dor_txtupvec_initialize();
	initialized = DcTrue;
    }

    txtupvecdata = (dot_txtupvec *)dor_space_allocate(sizeof *txtupvecdata);

    txtupvecdata->xup = xup;
    txtupvecdata->yup = yup;

    txtupvecobject = dor_object_create(doe_txtupvec_class_id,txtupvecdata);

    return(txtupvecobject);
}
/*
 ======================================================================
 */

void dor_txtupvec_print (dot_object *object)
{
    dot_txtupvec *txtupvecdata;

    txtupvecdata = (dot_txtupvec *)object->data;

    if (txtupvecdata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"x = %f, y = %f",
	    txtupvecdata->xup, txtupvecdata->yup);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_txtupvec_execute (struct object *object)
{
    dot_txtupvec *txtupvecdata;

    if( ! dor_condex_query_execute(doe_txtupvec_class_id))
	    return;

    txtupvecdata = (dot_txtupvec *)object->data;

    dor_global_txtupvec_set_value(txtupvecdata);
}
