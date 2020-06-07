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
	dor_calbak_initialize
	dor_calbak_create
	dor_calbak_print
	dor_calbak_execute
	dor_calbak_get_dataobject
	dor_calbak_get_function
	dor_calbak_get_pass_by_ref

  ======================================================================
 */
#include <dore/internal/dogen.h>

struct calbak_data {
    DtPFI       function;		/* nothing is done with the value */
    DtFlag      pass_by_ref;
    dot_object *dataobject;
};
/*
 ======================================================================
 */

DtInt doe_calbak_class_id;

static DtMethodEntry calbak_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_calbak_print },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_calbak_execute },
    { DcMethodIniPick,          (DtMethodPtr) dor_calbak_execute },
    { DcMethodPick,             (DtMethodPtr) dor_calbak_execute },
    { DcMethodCheckGroup,       (DtMethodPtr) dor_calbak_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_calbak_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_calbak_execute },
    { DcMethodDestroy,		(DtMethodPtr) dor_calbak_destroy }
};

void dor_calbak_initialize (void)
{
    doe_calbak_class_id = dor_class_install (DcTypeCallback, "DoCallback", 8,
					     calbak_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_calbak_create (
    DtPFI    function,
    DtObject dataobject,
    DtFlag   pass_by_ref)
{
    static DtFlag initialized = DcFalse;
    struct calbak_data *calbakdat;
    dot_object *calbakobj;
    DtInt type;
    DtInt dataptr_class_id = dor_class_inq_id("DoDataPtr");
    DtInt dataval_class_id = dor_class_inq_id("DoDataVal");

    if (!initialized) {
	dor_calbak_initialize();
	initialized = DcTrue;
    }

    if ((dataobject != DcNullObject) &&
	((DcObjectValid != dor_object_validate(dataobject)) ||
	 (((type = dor_object_inq_type(dataobject)) != 
	   dataptr_class_id) &&
	  (type != dataval_class_id)))) {
	DDerror (ERR_INVALID_DATAOBJECT, "DoCallback", DcNullPtr);
	return (DcNullPtr);
    }

    calbakdat = dor_space_allocate(sizeof *calbakdat);

    calbakdat->function    = function;
    calbakdat->pass_by_ref = pass_by_ref;
    calbakdat->dataobject  = dataobject;

    calbakobj = dor_object_create(doe_calbak_class_id,calbakdat);

    if (dataobject != NULL) dor_object_add_reference(dataobject,calbakobj);

    return(calbakobj);
}



/* ====================================================================== */

void dor_calbak_print (dot_object *object)
{
    struct calbak_data *calbak;

    calbak = (struct calbak_data *)object->data;

    if (calbak == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"function = [%lx]",calbak->function);
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(),"   (pass by %s)", 
	    calbak->pass_by_ref ? "reference" : "value");
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"dataobject = [%lx]",calbak->dataobject);
    dor_print_output(dor_print_get_line());
    if (calbak->dataobject != DcNullObject) {
	dor_print_indent_up();
	dor_object_print(calbak->dataobject);
	dor_print_indent_down();
    }
}



/* ====================================================================== */

void dor_calbak_execute (dot_object *object)
{
    DtPFI       function;
    dot_object *dataobject;
    Dt32Bits    datval;
    DtPtr       datptr;
    DtInt       type;
    DtInt       dataptr_class_id = dor_class_inq_id("DoDataPtr");
    DtInt       dataval_class_id = dor_class_inq_id("DoDataVal");

    function = dor_calbak_get_function(object);

    dataobject = dor_calbak_get_dataobject(object);

    /*  Call the callback function with the appropriate data object.  */

    if (dataobject == DcNullObject) {
	datval = 0;
	if (dor_calbak_get_pass_by_ref(object)) {
	    (*function)(&datval);
	} else {
	    (*function)(datval);
	}
    }
    else if ((DcObjectValid == dor_object_validate(dataobject)) &&
	     (((type = dor_object_inq_type(dataobject)) == dataptr_class_id) ||
	      (type == dataval_class_id))) {
	if (type == dataptr_class_id) {
	    datptr = dor_datptr_get_value(dataobject);
	    if (dor_calbak_get_pass_by_ref(object)) {
		(*function)(&datptr);
	    } else {
		(*function)(datptr);
	    }
	} else if (type == dataval_class_id) {
	    datval = dor_datval_get_value(dataobject);
	    if (dor_calbak_get_pass_by_ref(object)) {
		(*function)(&datval);
	    } else {
		(*function)(datval);
	    }
	}
    } else {
	DDerror (ERR_INVALID_DATAOBJECT, "dor_execute_calbck", DcNullPtr);
    }
}



/* ====================================================================== */

dot_object *dor_calbak_get_dataobject (dot_object *object)
{
    struct calbak_data *calbakdat;

    calbakdat = (struct calbak_data *)object->data;

    return calbakdat->dataobject;
}



/* ====================================================================== */

DtPFI dor_calbak_get_function (dot_object *object)
{
    struct calbak_data *calbakdat;

    calbakdat = (struct calbak_data *)object->data;

    return calbakdat->function;
}



/* ====================================================================== */

DtFlag dor_calbak_get_pass_by_ref (dot_object *object)
{
    struct calbak_data *calbakdat;

    calbakdat = (struct calbak_data *)object->data;

    return calbakdat->pass_by_ref;
}


/*
 ======================================================================
 */

void dor_calbak_destroy (dot_object *calbakobj)
{
    struct calbak_data *calbakdat = (struct calbak_data *)calbakobj->data;
    DtFlag ok;

    if (calbakdat->dataobject != NULL) 
	dor_object_delete_reference(calbakdat->dataobject,calbakobj);
    dor_space_deallocate(calbakdat);
}
