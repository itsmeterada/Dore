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
	int dor_intermdisptype_initialize ()
	dot_object * dor_intermdisptype_create (intermdisptype)
	int dor_intermdisptype_destroy (object)
	int dor_intermdisptype_print (object)
	int dor_intermdisptype_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct intermdisptype_data {
    DtIntermediateDispType type;
    DtInt *parms;
};


/*
 ======================================================================
 */

DtInt doe_intermdisptype_class_id;

static DtMethodEntry intermdisptype_methods[] = {
    { DcMethodPrint,           (DtMethodPtr) dor_intermdisptype_print },
    { DcMethodDestroy,         (DtMethodPtr) dor_intermdisptype_destroy },
    { DcMethodStdRenderStudio, (DtMethodPtr) dor_intermdisptype_execute },
};

void dor_intermdisptype_initialize (void)
{
    doe_intermdisptype_class_id = dor_class_add ("DoIntermediateDispType", 3,
                                       intermdisptype_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_intermdisptype_create (
    DtIntermediateDispType type,
    DtInt *parms)
{
    static DtFlag initialized = DcFalse;
    dot_object *intermdisptypeobject;
    struct intermdisptype_data *intermdisptypedata;
    DtInt i;

    if (!initialized) {
	dor_intermdisptype_initialize();
	initialized = DcTrue;
    }
    intermdisptypedata = (struct intermdisptype_data *)dor_space_allocate
               (sizeof *intermdisptypedata);

    intermdisptypedata->type = type;

    switch (type){

    case DcDispIntermediateAll:
    case DcDispFinal:
        intermdisptypedata->parms = DcNullPtr;
        break;

    case DcDispRegularIntervals:
        intermdisptypedata->parms = (DtInt *)dor_space_allocate(sizeof (DtInt));
        intermdisptypedata->parms[0] = parms[0];
        break;

    case DcDispIrregularIntervals:
        intermdisptypedata->parms = (DtInt *)dor_space_allocate
                                    (sizeof (DtInt)*(parms[0]+1)); 

        for (i=0; i<(parms[0]+1); i++)  
             intermdisptypedata->parms[i] = parms[i];
         
        break;
    }

    intermdisptypeobject = dor_object_create(doe_intermdisptype_class_id,
					   (DtPtr)intermdisptypedata);
    return(intermdisptypeobject);
}
/*
 ======================================================================
 */

void dor_intermdisptype_destroy (dot_object *object)
{
    struct intermdisptype_data *intermdisptypedata;

    intermdisptypedata = (struct intermdisptype_data *)(object->data);

    dor_space_deallocate(intermdisptypedata);
}

/*
 ======================================================================
 */

void dor_intermdisptype_print (dot_object *object)
{
    struct intermdisptype_data *intermdisptype;
    DtInt i;

    intermdisptype  = (struct intermdisptype_data *)object->data;

    if (intermdisptype == NULL){
        dor_print_output("bad values");
        return;
    }

    switch (intermdisptype->type){

    case DcDispIntermediateAll:
        dor_print_output("type = DcDispIntermediateAll");
        break;

    case DcDispFinal:
        dor_print_output("type = DcDispFinal");
        break;

    case DcDispRegularIntervals:
        dor_print_output("type = DcDispRegularIntervals");
        sprintf(dor_print_get_line(),"parms[0] = %d",intermdisptype->parms[0]);
        dor_print_output(dor_print_get_line());
        break;

    case DcDispIrregularIntervals:
        dor_print_output("type = DcDispIrregularIntervals");
	sprintf(dor_print_get_line(),"parms[0] (# of parms) = %d", 
                     intermdisptype->parms[0]);
        dor_print_output(dor_print_get_line());

        for (i=1; i<(intermdisptype->parms[0]+1); i++) {
             sprintf(dor_print_get_line(),"parms[%d] = %d", i,
                     intermdisptype->parms[i]);
             dor_print_output(dor_print_get_line());
        }
        break;

    default:
        dor_print_output("type = UNKNOWN");
    }

}
/*
 ======================================================================
 */

void dor_intermdisptype_execute (dot_object *object)
{
    struct intermdisptype_data *intermdisptypedata; 

    if (!dor_condex_query_execute(doe_intermdisptype_class_id))
	    return;

    intermdisptypedata = (struct intermdisptype_data *) object->data;

    dor_global_intermdisptype_set_value(intermdisptypedata->type,
                                        intermdisptypedata->parms);
}
