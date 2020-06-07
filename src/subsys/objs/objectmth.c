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
	dor_object_inq_default_method
	dor_object_inq_method_id
	dor_object_get_method
	dor_object_get_current_method
	dor_object_method_null
	dor_object_print_method_table

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include "obj.h"



/* ====================================================================== */

DtMethodPtr dor_object_inq_default_method (dot_object *object)
{
    auto Dt32Bits type;

    type = object->info & dod_object_type_mask;

#   ifdef ERRCHK
	if ((type < 0) || (type >= doe_object_next_type)) {
	    sprintf (DDerror_GetLine(),"type %d",type);
	    DDerror (ERR_INVALID_OBJ_TYPE,
		     "dor_object_get_method", DDerror_GetLine());
	}
#   endif

    return (doe_object_types[type].default_method);
}



/* ====================================================================== */

DtInt dor_object_inq_method_id (char *method_name)
{
    register Dt32Bits i;

    if (!method_name) return -1;

    for (i=0;  i < doe_object_next_method;  i++) {
	if (  (doe_object_method_name[i])
	   && (0 == strcmp (doe_object_method_name[i], method_name))) {
	    return i;	/* Found the method name. */
	}
    }

    return -1;
}



/* ====================================================================== */

DtMethodPtr dor_object_get_method (
    dot_object *object,
    DtInt       method)
{
    register Dt32Bits type;

    type = object->info & dod_object_type_mask;

    if (method == DcMethodNull) return dor_object_method_null;

#   ifdef ERRCHK
	if ((type < 0) || (type >= doe_object_next_type)){
	    sprintf (DDerror_GetLine(),"type %d",type);
	    DDerror (ERR_INVALID_OBJ_TYPE,
		     "dor_object_get_method", DDerror_GetLine());
	}

	if ((method < 0) || (method >= doe_object_next_method)){
	    sprintf (DDerror_GetLine(),"invalid method %d", method);
	    DDerror (ERR_INVALID_METHOD,
		     "dor_object_get_method", DDerror_GetLine());
	}
#   endif

    return doe_object_types[type].methods[method];
}



/* ====================================================================== */

DtMethodPtr dor_object_get_current_method (dot_object *object)
{
    register Dt32Bits type;

    type = object->info & dod_object_type_mask;

    if (doe_system_current_method == DcMethodNull)
	return dor_object_method_null;

#   ifdef ERRCHK
	if ((type < 0) || (type >= doe_object_next_type)) {
	    sprintf (DDerror_GetLine(),"type %d",type);
	    DDerror (ERR_INVALID_OBJ_TYPE,
		     "dor_object_get_current_method", DDerror_GetLine());
	}

	if (  (doe_system_current_method < 0)
	   || (doe_system_current_method >= doe_object_next_method)) {
	    sprintf (DDerror_GetLine(), "method %d", doe_system_current_method);
	    DDerror (ERR_INVALID_METHOD,
		     "dor_object_get_current_method", DDerror_GetLine());
	}
#   endif

    return doe_object_types[type].methods[doe_system_current_method];
}



/* ====================================================================== */

void *dor_object_method_null (dot_object *object)
{
    return object;
}



/* ====================================================================== */

void dor_object_print_method_table (void)
{
    register Dt32Bits i, j;

    for (i=0;  i < doe_object_next_method;  i++) {
	if (doe_object_derived_method[i] != -1) {
	    sprintf (dor_print_get_line(), "%4d: %s derived from %d", i,
		doe_object_method_name[i] ? doe_object_method_name[i]
					  : "<no name>",
		doe_object_derived_method[i]
	    );
	} else {
	    sprintf (dor_print_get_line(), "%4d: %s", i,
		     doe_object_method_name[i] ? doe_object_method_name[i]
					       : "<no name>");
	}
	dor_print_output (dor_print_get_line());
    }

    for (i=0;  i < doe_object_next_type;  i++) {
	register dot_object *object;

	object = doe_object_types[i].class_object;

	switch (dor_object_inq_name_type(object)) {
	    case DcNameNone: {
		sprintf (dor_print_get_line(), "class: (no name) %d", i);
		break;
	    }

	    case DcNameInteger: {
		auto DtInt name = dor_object_inq_name_integer (object);
		sprintf (dor_print_get_line(), "class: (integer name) %d",name);
		break;
	    }

	    case DcNameString: {
		auto char *name = dor_object_inq_name_string (object);
		sprintf (dor_print_get_line(),
			 "class: (string name) \"%s\"", name);
		break;
	    }
	}

	dor_print_output (dor_print_get_line());
	dor_print_indent_up ();

	sprintf (dor_print_get_line(), "default: 0x%lx",
		 doe_object_types[i].default_method);
	dor_print_output (dor_print_get_line());

	for (j=0;  j < doe_object_next_method;  j++) {
	    if (doe_object_types[i].methods[j] != 
		doe_object_types[i].default_method) {
		sprintf (dor_print_get_line(), "%4d:  0x%lx",
			 j, doe_object_types[i].methods[j]);
		dor_print_output (dor_print_get_line());
	    }
	}
	dor_print_indent_down();
    }
}



/*****************************************************************************
**  This function returns true if the given method ID is valid, otherwise it
**  returns false.
*****************************************************************************/

DtFlag dor_object_method_id_valid (DtInt method_id)
{
    return (method_id >= 0) && (method_id < doe_object_next_method);
}
