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
	int dor_cmptexuvwcbk_initialize ()
	int dor_cmptexuvwcbk_initialize_types ()
	int dor_cmptexuvwcbk_handler ()
	dot_object * dor_cmptexuvwcbk_create (callback_object)
	int dor_cmptexuvwcbk_destroy(object);
	int dor_cmptexuvwcbk_print (object)
	int dor_cmptexuvwcbk_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct cmptexuvwcbk_data {
    DtObject cmptexuvwcbkobj;
};

DtInt doe_cmptexuvwcbk_class_id = -1;
DtFlag doe_cmptexuvwcbk_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry cmptexuvwcbk_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_cmptexuvwcbk_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_cmptexuvwcbk_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_cmptexuvwcbk_execute },
};

void dor_cmptexuvwcbk_initialize (void)
{
    doe_cmptexuvwcbk_class_id = dor_class_add("DoCompTextureUVWCallback", 3, 
					  cmptexuvwcbk_methods,DcNullPtr);
}
/*
 ======================================================================
 */

DtObject DcStdUVWCallback;

/* Initialise the texture UVW generation callback object(s)
   provided with standard Dore. */

static void dor_cmptexuvwcbk_handler (void) { return; }

void dor_cmptexuvwcbk_initialize_types (void)
{
    dot_object *datavalobj;

    datavalobj = dor_datval_create((Dt32Bits)0);

    DcStdUVWCallback = dor_calbak_create ((DtPFI) dor_cmptexuvwcbk_handler,
                                          datavalobj, DcFalse);

    /* Create dummy reference to this object in the event that it
     * is dereferenced such that its reference count goes to zero:
     * this keeps it from being deleted entirely.
     */

     dor_object_add_reference(DcStdUVWCallback, DcNullObject);
}

/*
 ======================================================================
 */

dot_object *dor_cmptexuvwcbk_create (
    DtObject callback_object)
{
    struct cmptexuvwcbk_data *cmptexuvwcbkdata;

    if (!doe_cmptexuvwcbk_initialized) {
	dor_cmptexuvwcbk_initialize();
	doe_cmptexuvwcbk_initialized = DcTrue;
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(callback_object,dor_class_inq_id("DoCallback"))) {
	
	DDerror(ERR_BAD_OBJECT, "DoCompTextureUVWCallback",
		"callback_object must be DoCallback object");
	return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif

    cmptexuvwcbkdata = (struct cmptexuvwcbk_data *)dor_space_allocate(sizeof
			*cmptexuvwcbkdata);

    cmptexuvwcbkdata->cmptexuvwcbkobj = callback_object;

    return (dor_object_create(doe_cmptexuvwcbk_class_id,cmptexuvwcbkdata));
}

/*
 ======================================================================
 */

void dor_cmptexuvwcbk_destroy (
    DtObject object)
{
    struct cmptexuvwcbk_data *cmptexuvwcbk;

    cmptexuvwcbk = (struct cmptexuvwcbk_data *)(object->data);

    dor_space_deallocate(cmptexuvwcbk);
}

/*
 ======================================================================
 */

void dor_cmptexuvwcbk_print (
    DtObject object)
{
    struct cmptexuvwcbk_data *cmptexuvwcbk;

    cmptexuvwcbk = (struct cmptexuvwcbk_data *)(object->data);

    if (cmptexuvwcbk == NULL) {
	dor_print_output("bad values");
        return;
    } else {
	if (cmptexuvwcbk->cmptexuvwcbkobj == DcStdUVWCallback) {
	    dor_print_output ("callback object = DcStdUVWCallback");
    	} else {
            sprintf(dor_print_get_line(),"callback object (user defined) = [%lx]:",
		cmptexuvwcbk->cmptexuvwcbkobj);
            dor_print_output(dor_print_get_line());
	}
    }
}
/*
 ======================================================================
 */

void dor_cmptexuvwcbk_execute (
    DtObject object)
{
    struct cmptexuvwcbk_data *cmptexuvwcbk;

    if (!dor_condex_query_execute(doe_cmptexuvwcbk_class_id))
	    return;

    cmptexuvwcbk = (struct cmptexuvwcbk_data *)(object->data);

    dor_global_cmptexuvwcbk_set_value(cmptexuvwcbk->cmptexuvwcbkobj);
}
