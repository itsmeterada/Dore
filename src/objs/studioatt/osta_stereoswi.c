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
	int dor_stereoswi_initialize ()
	dot_object * dor_stereoswi_create (stereoswi)
	int dor_stereoswi_print (object)
	int dor_stereoswi_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

/*
 ======================================================================
 */

DtInt doe_stereoswi_class_id;

static DtMethodEntry stereoswi_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_stereoswi_print },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_stereoswi_execute },
};

void dor_stereoswi_initialize (void)
{
    doe_stereoswi_class_id = dor_class_install (DcTypeStereoSwitch, 
		"DoStereoSwitch", 2, stereoswi_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_stereoswi_create (DtSwitch stereoswi)
{
    static DtFlag initialized = DcFalse;
    dot_object *stereoswiobject;

    if (!initialized) {
	dor_stereoswi_initialize();
	initialized = DcTrue;
    }

    stereoswiobject = dor_object_create(doe_stereoswi_class_id,(DtPtr)stereoswi);

    return(stereoswiobject);
}
/*
 ======================================================================
 */

void dor_stereoswi_print (dot_object *object)
{
    switch ((DtSwitch)(object->data)){
    case DcOff:
	dor_print_output("switchvalue = DcOff");
	break;

    case DcOn:
	dor_print_output("switchvalue = DcOn");
	break;

    default:
	dor_print_output("switchvalue = UNKNOWN");
	break;
    }
}
/*
 ======================================================================
 */

void dor_stereoswi_execute (dot_object *object)
{
    if (!dor_condex_query_execute(doe_stereoswi_class_id))
	    return;

    dor_global_stereoswi_set_value((DtSwitch)(object->data));
}
