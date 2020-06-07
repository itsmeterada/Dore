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
	int dor_srfshd_initialize ()
	int dor_srfshd_initialize_types ()
	int dor_srfshd_constant_handler ()
	int dor_srfshd_lightsource_handler ()
	dot_object * dor_srfshd_create (srfshdobj)
	int dor_srfshd_destroy (object)
	int dor_srfshd_print (object)
	int dor_srfshd_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct srfshd_data {
    DtObject srfshdobj;
};
/*
 ======================================================================
 */

DtInt doe_srfshd_class_id;

static DtMethodEntry srfshd_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_srfshd_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_srfshd_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_srfshd_execute },
};

void dor_srfshd_initialize (void)
{
    doe_srfshd_class_id = dor_class_add ("DoSurfaceShade", 3,
					 srfshd_methods,DcNullPtr);
}
/*
 ======================================================================
 */

DtObject DcShaderConstant;
DtObject DcShaderLightSource;

/*  Initialize the surface shader callback objects provided with standard Dore.  */

void dor_srfshd_initialize_types (void)
{
    dot_object *datavalobj;

    datavalobj = dor_datval_create((Dt32Bits)0);

    DcShaderConstant = dor_calbak_create ((DtPFI) dor_srfshd_constant_handler,
					  datavalobj, DcFalse);
    /*
      dor_object_set_name(DcShaderConstant,DcNameString,"DcShaderConstant",
      DcFalse);
      */

    DcShaderLightSource = dor_calbak_create (
			      (DtPFI) dor_srfshd_lightsource_handler,
			      datavalobj, DcFalse);
    /*
      dor_object_set_name(DcShaderLightSource,DcNameString,"DcShaderLightSource",
      DcFalse);
      */

    /* Create dummy references to these objects in the event that
     * they are dereferenced such that their reference count goes to
     * zero: this keeps them from being deleted entirely.
     */

    dor_object_add_reference(DcShaderConstant, DcNullObject);
    dor_object_add_reference(DcShaderLightSource, DcNullObject);

}

void dor_srfshd_constant_handler (void)
{

}

void dor_srfshd_lightsource_handler (void)
{

}
/*
 ======================================================================
 */

dot_object *dor_srfshd_create (
    DtObject srfshdobj)
{
    static DtFlag initialized = DcFalse;
    struct srfshd_data *srfshddata;
    dot_object *srfshdobject;

    if (!initialized) {
	dor_srfshd_initialize();
	initialized = DcTrue;
    }

    srfshddata = (struct srfshd_data *)dor_space_allocate(sizeof *srfshddata);

    srfshddata->srfshdobj = srfshdobj;

    srfshdobject = dor_object_create(doe_srfshd_class_id,srfshddata);

    return(srfshdobject);
}
/*
 ======================================================================
 */

void dor_srfshd_destroy (
    dot_object *object)
{
    struct srfshd_data *srfshd;

    srfshd = (struct srfshd_data *)(object->data);

    dor_space_deallocate(srfshd);
}
/*
 ======================================================================
 */

void dor_srfshd_print (
    dot_object *object)
{
    struct srfshd_data *srfshddata;

    srfshddata = (struct srfshd_data *)object->data;

    if (srfshddata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "surface shader object= [%lx]:",
	    srfshddata->srfshdobj);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_srfshd_execute (
    dot_object *object)
{
    struct srfshd_data *srfshddat;

    if (!dor_condex_query_execute(doe_srfshd_class_id))
	    return;

    srfshddat = (struct srfshd_data *)object->data;

    dor_global_srfshd_set_value(srfshddat->srfshdobj);
}
