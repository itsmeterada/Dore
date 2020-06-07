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
	int dor_datval_initialize ()
	dot_object * dor_datval_create (value)
	int dor_datval_print (object)
	Dt32Bits dor_datval_get_value (object)
	void dor_datval_destroy (dot_object *datval_object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/datval.h>




/* ====================================================================== */

DtInt doe_datval_class_id;

static DtMethodEntry datval_methods[] = {
    { DcMethodPrint, (DtMethodPtr) dor_datval_print },
    { DcMethodDestroy, (DtMethodPtr) dor_datval_destroy }
};

void dor_datval_initialize (void)
{
    doe_datval_class_id = dor_class_install (DcTypeDataVal, "DoDataVal", 2,
					     datval_methods,DcNullPtr);
}



/* ====================================================================== */

dot_object *dor_datval_create (Dt32Bits value)
{
    static DtFlag initialized = DcFalse;
    struct datval_data *datvaldat;
    dot_object *datvalobj;

    if (!initialized) {
	dor_datval_initialize();
	initialized = DcTrue;
    }

    datvaldat = dor_space_allocate (sizeof *datvaldat);

    datvaldat->value = value;

    datvalobj = dor_object_create (doe_datval_class_id,datvaldat);

    return datvalobj;
}



/* ====================================================================== */

void dor_datval_print (dot_object *object)
{
    struct datval_data *datvaldat;

    datvaldat = (struct datval_data *)object->data;

    if (datvaldat == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"value = %ld",datvaldat->value);
    dor_print_output(dor_print_get_line());
}



/* ====================================================================== */

Dt32Bits dor_datval_get_value (dot_object *object)
{
    struct datval_data *datvaldat;

    datvaldat = (struct datval_data *)object->data;

    return datvaldat->value;
}
/*
 ======================================================================
 */

void dor_datval_destroy (dot_object *datval_object)
{
    struct datval_data *datvaldat = (struct datval_data *)datval_object->data;
    DtFlag ok;

    dor_space_deallocate(datvaldat);
}
