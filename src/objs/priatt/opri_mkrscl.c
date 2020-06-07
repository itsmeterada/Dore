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
	int dor_mkrscl_initialize ()
	struct object * dor_mkrscl_create (mkrscl)
	int dor_mkrscl_print (object)
	int dor_mkrscl_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct mkrscl_data {
    DtReal mkrscl;
};
/*
 ======================================================================
 */

DtInt doe_mkrscl_class_id;

static DtMethodEntry mkrscl_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_mkrscl_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_mkrscl_execute },
};

void dor_mkrscl_initialize (void)
{
    doe_mkrscl_class_id = dor_class_install (DcTypeMarkerScale, "DoMarkerScale",
					     2, mkrscl_methods,DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_mkrscl_create (DtReal mkrscl)
{
    static DtFlag initialized = DcFalse;
    struct mkrscl_data *mkrscldata;
    struct object *mkrsclobject;

    if (!initialized) {
	dor_mkrscl_initialize();
	initialized = DcTrue;
    }

    mkrscldata = (struct mkrscl_data *)dor_space_allocate(sizeof *mkrscldata);

    mkrscldata->mkrscl = mkrscl;

    mkrsclobject = dor_object_create(doe_mkrscl_class_id, mkrscldata);

    return(mkrsclobject);
}
/*
 ======================================================================
 */

void dor_mkrscl_print (dot_object *object)
{
    struct mkrscl_data *mkrscldata;

    mkrscldata = (struct mkrscl_data *)object->data;

    if (mkrscldata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"value = %f",mkrscldata->mkrscl);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_mkrscl_execute (struct object *object)
{
    struct mkrscl_data *mkrscldata;

    if( ! dor_condex_query_execute(doe_mkrscl_class_id))
	    return;

    mkrscldata = (struct mkrscl_data *)object->data;

    dor_global_mkrscl_set_value(mkrscldata->mkrscl);
}
