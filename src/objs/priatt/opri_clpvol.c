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
 
/* ======================================================================
  Functions:
	dor_clpvol_initialize
	dor_clpvol_create
	dor_clpvol_destroy
	dor_clpvol_print
	dor_clpvol_execute

======================================================================== */

#include <dore/internal/dogen.h>
#include <dore/internal/priatt.h>
#include <dore/dore_develop/private/clpvol.h>

/*
 ======================================================================
 */

DtInt doe_clpvol_class_id;

static DtMethodEntry clpvol_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_clpvol_print   },
    { DcMethodDestroy,          (DtMethodPtr) dor_clpvol_destroy },
    { DcMethodIniPick,          (DtMethodPtr) dor_clpvol_execute },
    { DcMethodPick,             (DtMethodPtr) dor_clpvol_execute },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_clpvol_execute },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_clpvol_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_clpvol_execute },
};

void dor_clpvol_initialize (void)
{
    doe_clpvol_class_id = dor_class_add ("DoClipVol", 7,
					 clpvol_methods,DcNullPtr);

}
/*
 ======================================================================
 */

dot_object *dor_clpvol_create (
    DtClipOperator operator,
    DtInt halfspacecount,
    DtHalfSpace *halfspaces)
{
    static DtFlag initialized = DcFalse;
    dot_object *clpvolobj;
    struct clpvol_data *clpvoldat;
    DtInt halfspaceno;

    if (!initialized) {
	dor_clpvol_initialize();
	initialized = DcTrue;
    }

    clpvoldat = (struct clpvol_data *)dor_space_allocate(sizeof *clpvoldat);

    clpvoldat->operator = operator;

    clpvoldat->halfspacecount = halfspacecount;

    if (halfspacecount > 0){
	clpvoldat->halfspaces = (DtHalfSpace *)dor_space_allocate(
				halfspacecount*(sizeof (DtHalfSpace)));

	for (halfspaceno=0;halfspaceno<halfspacecount;halfspaceno++){
	    clpvoldat->halfspaces[halfspaceno].point[0] =
		    halfspaces[halfspaceno].point[0];
	    clpvoldat->halfspaces[halfspaceno].point[1] =
		    halfspaces[halfspaceno].point[1];
	    clpvoldat->halfspaces[halfspaceno].point[2] =
		    halfspaces[halfspaceno].point[2];

	    clpvoldat->halfspaces[halfspaceno].vector[0] =
		    halfspaces[halfspaceno].vector[0];
	    clpvoldat->halfspaces[halfspaceno].vector[1] =
		    halfspaces[halfspaceno].vector[1];
	    clpvoldat->halfspaces[halfspaceno].vector[2] =
		    halfspaces[halfspaceno].vector[2];
	}
    }
    else{
	clpvoldat->halfspaces = NULL;
    }

    clpvolobj = dor_object_create(doe_clpvol_class_id,clpvoldat);

    return(clpvolobj);
}


/*
 ======================================================================
 */

void dor_clpvol_destroy (
    dot_object *object)
{
    struct clpvol_data *clpvoldat;

    clpvoldat = (struct clpvol_data *)(object->data);

    if (clpvoldat->halfspaces != NULL){
	dor_space_deallocate(clpvoldat->halfspaces);
    }

    dor_space_deallocate(clpvoldat);
}
/*
 ======================================================================
 */

void dor_clpvol_print (
    dot_object *object)
{
    struct clpvol_data *clpvol;
    DtInt i;
    DtHalfSpace *halfspace;

    clpvol = (struct clpvol_data *)object->data;

    if (clpvol == NULL){
	dor_print_output("bad values");
	return;
    }

    switch (clpvol->operator){
    case DcClipAll:
	dor_print_output("operator = DcClipAll");
	break;
    case DcClipAnd:
	dor_print_output("operator = DcClipAnd");
	break;
    case DcClipAndReverse:
	dor_print_output("operator = DcClipAndReverse");
	break;
    case DcClipNoOp:
	dor_print_output("operator = DcClipNoOp");
	break;
    case DcClipAndInverted:
	dor_print_output("operator = DcClipAndInverted");
	break;
    case DcClipReplace:
	dor_print_output("operator = DcClipReplace");
	break;
    case DcClipXOr:
	dor_print_output("operator = DcClipXOr");
	break;
    case DcClipOr:
	dor_print_output("operator = DcClipOr");
	break;
    case DcClipNor:
	dor_print_output("operator = DcClipNor");
	break;
    case DcClipEqv:
	dor_print_output("operator = DcClipEqv");
	break;
    case DcClipInvertVolume:
	dor_print_output("operator = DcClipInvertVolume");
	break;
    case DcClipOrReverse:
	dor_print_output("operator = DcClipOrReverse");
	break;
    case DcClipInvert:
	dor_print_output("operator = DcClipInvert");
	break;
    case DcClipOrInverted:
	dor_print_output("operator = DcClipOrInverted");
	break;
    case DcClipNAnd:
	dor_print_output("operator = DcClipNAnd");
	break;
    case DcClipNone:
	dor_print_output("operator = DcClipNone");
	break;
    }

    for (i = 0; i < clpvol->halfspacecount; i++){
	halfspace = clpvol->halfspaces+i;
	sprintf(dor_print_get_line(),
		"halfspace point = (%f, %f, %f)",
		halfspace->point[0], 
		halfspace->point[1], 
		halfspace->point[2]);
	dor_print_output(dor_print_get_line());
	sprintf (dor_print_get_line(),
		 "vector = (%f, %f, %f)",
		 halfspace->vector[0], 
		 halfspace->vector[1],
		 halfspace->vector[2]);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

void dor_clpvol_execute (
    dot_object *object)
{
    struct clpvol_data *clpvoldat;

    if (!dor_condex_query_execute(doe_clpvol_class_id))
	    return;

    clpvoldat = (struct clpvol_data *)object->data;

    dor_global_clpvol_apply_volume(clpvoldat->operator,
				   clpvoldat->halfspacecount,clpvoldat->halfspaces);
}
