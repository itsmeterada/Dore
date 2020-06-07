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
	dor_lokatfrm_initialize
	dor_lokatfrm_create
	dor_lokatfrm_destroy
	dor_lokatfrm_print
	dor_lokatfrm_execute

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/geoatt.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/lokatfrm.h>

/*
 ======================================================================
 */

DtInt doe_lokatfrm_class_id;

static DtMethodEntry lokatfrm_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_lokatfrm_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_lokatfrm_destroy },
    { DcMethodIniPick,          (DtMethodPtr) dor_lokatfrm_execute },
    { DcMethodPick,             (DtMethodPtr) dor_lokatfrm_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_lokatfrm_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_lokatfrm_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_lokatfrm_execute },
};

void dor_lokatfrm_initialize (void)
{
    doe_lokatfrm_class_id = dor_class_install (DcTypeLookAtFrom, "DoLookAtFrom",
				7, lokatfrm_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_lokatfrm_create (
    DtPoint3 at,
    DtPoint3 from,
    DtVector3 up)
{
    static DtFlag initialized = DcFalse;
    struct lokatfrm_data *lokatfrmdat;
    dot_object *lokatfrmobj;

    if (!initialized) {
	dor_lokatfrm_initialize();
	initialized = DcTrue;
    }

    lokatfrmdat = dor_space_allocate(sizeof(*lokatfrmdat));

    lokatfrmdat->from[0] = from[0];
    lokatfrmdat->from[1] = from[1];
    lokatfrmdat->from[2] = from[2];

    lokatfrmdat->at[0] = at[0];
    lokatfrmdat->at[1] = at[1];
    lokatfrmdat->at[2] = at[2];

    lokatfrmdat->up[0] = up[0];
    lokatfrmdat->up[1] = up[1];
    lokatfrmdat->up[2] = up[2];

    lokatfrmobj = dor_object_create(doe_lokatfrm_class_id,lokatfrmdat);

    return(lokatfrmobj);
}
/*
 ======================================================================
 */

void dor_lokatfrm_destroy (
    dot_object *object)
{
    dor_space_deallocate(object->data);
}
/*
 ======================================================================
 */

void dor_lokatfrm_print (
    dot_object *object)
{
    struct lokatfrm_data *lokatfrmdat;

    lokatfrmdat = (struct lokatfrm_data *)object->data;

    if (lokatfrmdat == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "At = (%f,%f,%f)", 
	    lokatfrmdat->at[0],lokatfrmdat->at[1],lokatfrmdat->at[2]);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "From = (%f,%f,%f)", 
	    lokatfrmdat->from[0],lokatfrmdat->from[1],lokatfrmdat->from[2]);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "Up = (%f,%f,%f)",
	    lokatfrmdat->up[0],lokatfrmdat->up[1],lokatfrmdat->up[2]);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_lokatfrm_execute (
    dot_object *object)
{
    struct lokatfrm_data *lokatfrmdat;
    DtMatrix4x4 newmatrix;

    if (!dor_condex_query_execute(doe_lokatfrm_class_id))
	    return;

    lokatfrmdat = (struct lokatfrm_data *)object->data;

    dor_global_lcstowcsmat_get_value(newmatrix);

    dor_matrix_look_at_from(newmatrix,
			    lokatfrmdat->at[0],lokatfrmdat->at[1],lokatfrmdat->at[2],
			    lokatfrmdat->from[0],lokatfrmdat->from[1],lokatfrmdat->from[2],
			    lokatfrmdat->up[0],lokatfrmdat->up[1],lokatfrmdat->up[2],
			    DcPreConcatenate);

    dor_global_lcstowcsmat_set_value(newmatrix);
}
