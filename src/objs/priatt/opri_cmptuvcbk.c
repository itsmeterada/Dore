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
	dor_cmptexuvcbk_initialize
	dor_cmptexuvcbk_initialize_types
	dor_cmptexuvcbk_handler
	dor_cmptexuvcbk_create
	dor_cmptexuvcbk_destroy
	dor_cmptexuvcbk_print
	dor_cmptexuvcbk_execute

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct cmptexuvcbk_data {
    DtObject cmptexuvcbkobj;
};

DtInt doe_cmptexuvcbk_class_id = -1;
DtFlag doe_cmptexuvcbk_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry cmptexuvcbk_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_cmptexuvcbk_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_cmptexuvcbk_print   },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_cmptexuvcbk_execute },
};

void dor_cmptexuvcbk_initialize (void)
{
    doe_cmptexuvcbk_class_id = dor_class_add("DoCompTextureUVCallback", 3, 
					  cmptexuvcbk_methods,DcNullPtr);
}
/*
 ======================================================================
 */

DtObject DcStdUVCallback;

/* Initialise the texture UV generation callback object(s)
   provided with standard Dore. */

static void dor_cmptexuvcbk_handler (void) { return; }

void dor_cmptexuvcbk_initialize_types (void)
{
    dot_object *datavalobj;

    datavalobj = dor_datval_create((Dt32Bits)0);

    DcStdUVCallback = dor_calbak_create ((DtPFI) dor_cmptexuvcbk_handler,
                                         datavalobj, DcFalse);

    /* Create dummy reference to this object in the event that it
     * is dereferenced such that its reference count goes to zero:
     * this keeps it from being deleted entirely.
     */

     dor_object_add_reference(DcStdUVCallback, DcNullObject);
}

/*
 ======================================================================
 */

dot_object *dor_cmptexuvcbk_create (
    DtObject callback_object)
{
    struct cmptexuvcbk_data *cmptexuvcbkdata;

    if (!doe_cmptexuvcbk_initialized) {
	dor_cmptexuvcbk_initialize();
	doe_cmptexuvcbk_initialized = DcTrue;
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(callback_object,dor_class_inq_id("DoCallback"))) {
	
	DDerror(ERR_BAD_OBJECT, "DoCompTextureUVCallback",
		"callback_object must be DoCallback object");
	return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif

    cmptexuvcbkdata = (struct cmptexuvcbk_data *)dor_space_allocate(sizeof
			*cmptexuvcbkdata);

    cmptexuvcbkdata->cmptexuvcbkobj = callback_object;

    return (dor_object_create(doe_cmptexuvcbk_class_id,cmptexuvcbkdata));
}

/*
 ======================================================================
 */

void dor_cmptexuvcbk_destroy (
    DtObject object)
{
    struct cmptexuvcbk_data *cmptexuvcbk;

    cmptexuvcbk = (struct cmptexuvcbk_data *)(object->data);

    dor_space_deallocate(cmptexuvcbk);
}

/*
 ======================================================================
 */

void dor_cmptexuvcbk_print (
    DtObject object)
{
    struct cmptexuvcbk_data *cmptexuvcbk;

    cmptexuvcbk = (struct cmptexuvcbk_data *)(object->data);

    if (cmptexuvcbk == NULL) {
	dor_print_output("bad values");
        return;
    } else {
	if (cmptexuvcbk->cmptexuvcbkobj == DcStdUVCallback) {
	    dor_print_output ("callback object = DcStdUVCallback");
    	} else {
            sprintf(dor_print_get_line(),"callback object (user defined) = [%lx]:",
		cmptexuvcbk->cmptexuvcbkobj);
            dor_print_output(dor_print_get_line());
	}
    }
}
/*
 ======================================================================
 */

void dor_cmptexuvcbk_execute (DtObject object)
{
    struct cmptexuvcbk_data *cmptexuvcbk;

    if (!dor_condex_query_execute(doe_cmptexuvcbk_class_id)) return;

    cmptexuvcbk = (struct cmptexuvcbk_data *)(object->data);

    dor_global_cmptexuvcbk_set_value (cmptexuvcbk->cmptexuvcbkobj);
}
