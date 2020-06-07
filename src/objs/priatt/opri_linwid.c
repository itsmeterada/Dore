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
	int dor_linwid_initialize ()
	struct object * dor_linwid_create (linwid)
	int dor_linwid_destroy (object)
	int dor_linwid_print (object)
	int dor_linwid_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>

struct linwid_data {
    DtReal linwid;
};
/*
 ======================================================================
 */

DtInt doe_linwid_class_id;

static DtMethodEntry linwid_methods[] = {
    { DcMethodPick,             (DtMethodPtr) dor_linwid_execute },
    { DcMethodPrint,            (DtMethodPtr) dor_linwid_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_linwid_destroy },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_linwid_execute },
};


void dor_linwid_initialize (void)
{
    doe_linwid_class_id = dor_class_install (DcTypeLineWidth, "DoLineWidth", 4,
					     linwid_methods,DcNullPtr);
}
/*
 ======================================================================
 */

struct object *dor_linwid_create (DtReal linwid)
{
    static DtFlag initialized = DcFalse;
    struct linwid_data *linwiddata;
    struct object *linwidobject;

    if (!initialized) {
	dor_linwid_initialize();
	initialized = DcTrue;
    }

    linwiddata = (struct linwid_data *)dor_space_allocate(sizeof *linwiddata);

    linwiddata->linwid = linwid;

    linwidobject = dor_object_create(doe_linwid_class_id,linwiddata);

    return(linwidobject);
}
/*
 ======================================================================
 */

void dor_linwid_destroy (dot_object *object)
{
    struct linwid *linwid;

    linwid = (struct linwid *)(object->data);

    dor_space_deallocate(linwid);
}
/*
 ======================================================================
 */

void dor_linwid_print (dot_object *object)
{
    struct linwid_data *linwiddata;

    linwiddata = (struct linwid_data *)object->data;

    if (linwiddata == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"value = %f",linwiddata->linwid);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_linwid_execute (struct object *object)
{
    struct linwid_data *linwiddata;

    if( ! dor_condex_query_execute(doe_linwid_class_id))
	    return;

    linwiddata = (struct linwid_data *)object->data;

    dor_global_linwid_set_value(linwiddata->linwid);
}
