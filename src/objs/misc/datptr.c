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
	int dor_datptr_initialize ()
	dot_object * dor_datptr_create (value)
	int dor_datptr_print (object)
	DtPtr dor_datptr_get_value (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/datptr.h>

/*
 ======================================================================
 */

DtInt doe_datptr_class_id;

static DtMethodEntry datptr_methods[] = {
    { DcMethodPrint, (DtMethodPtr) dor_datptr_print },
    { DcMethodDestroy, (DtMethodPtr) dor_datptr_destroy }
};

void dor_datptr_initialize (void)
{
    doe_datptr_class_id = dor_class_install (DcTypeDataPtr, "DoDataPtr", 2,
					     datptr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_datptr_create (DtPtr value)
{
    static DtFlag initialized = DcFalse;
    struct datptr_data *datptrdat;
    dot_object *datptrobj;

    if (!initialized) {
	dor_datptr_initialize();
	initialized = DcTrue;
    }

    datptrdat = (struct datptr_data *)dor_space_allocate(sizeof *datptrdat);

    datptrdat->value = value;

    datptrobj = dor_object_create(doe_datptr_class_id,datptrdat);

    return(datptrobj);
}



/* ====================================================================== */

void dor_datptr_print (dot_object *object)
{
    struct datptr_data *datptrdat;

    datptrdat = (struct datptr_data *)object->data;

    if (datptrdat == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"ptr = %lx",datptrdat->value);
    dor_print_output(dor_print_get_line());
}



/* ====================================================================== */

DtPtr dor_datptr_get_value (dot_object *object)
{
    struct datptr_data *datptrdat;

    datptrdat = (struct datptr_data *)object->data;

    return datptrdat->value;
}

/*
 ======================================================================
 */

void dor_datptr_destroy (dot_object *datptr_object)
{
    struct datptr_data *datptrdat = (struct datptr_data *)datptr_object->data;
    DtFlag ok;

    dor_space_deallocate(datptrdat);
}
