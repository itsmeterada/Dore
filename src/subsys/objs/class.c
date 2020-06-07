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
	dor_class_install
	dor_class_add
	dor_class_delete
	dor_class_add_notify
	dor_class_add_object_data
	dor_class_all_add_method
	dor_class_all_add_initialized_method
	dor_class_all_copy_method
	dor_class_all_install_method
	dor_class_all_replace_method
	dor_class_inq_default_method
	dor_class_inq_method
	dor_class_set_method
	dor_class_print
	dor_class_inq_id

  ======================================================================
 */
#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/list.h>
#include "obj.h"


/* ====================================================================== */

dot_object *dor_class_create (
    DtInt         class_id,
    DtInt         n,
    DtMethodEntry methods_list[],
    DtMethodPtr   default_method)
{
    register   DtInt      ii,jj;
    auto       DtInt      space;
    register   dot_class *class_data;

    if (class_id < 0) {
	sprintf(DDerror_GetLine(), "type %d", class_id);
	DDerror (ERR_INVALID_OBJ_TYPE, "dor_class_create", DDerror_GetLine());
	return DcNullPtr;
    }

    if (!default_method)
	default_method = dor_object_method_null;

    /* Extend dispatch table to hold type class_id. */

    if (class_id >= doe_object_next_type) {
	space = (class_id+1) * sizeof(dot_object_type);
	doe_object_types = dor_space_reallocate (doe_object_types, space);

	if (!doe_object_types) {
	    return DcNullPtr;	/* memory exhausted */
	}

	for (ii = doe_object_next_type;  ii <= class_id;  ++ii) {
	    /* allocate methods table */

	    space = (doe_object_next_method+1) * sizeof(DtMethodPtr);
	    doe_object_types[ii].methods = dor_space_allocate (space);

	    if (!doe_object_types) {
		return DcNullPtr;	/* memory exhausted */
	    }

	    /* initialize methods tables */

	    doe_object_types[ii].class_object   = DcNullPtr;
	    doe_object_types[ii].default_method = dor_object_method_null;

	    for (jj=0;  jj < doe_object_next_method;  ++jj) {
		doe_object_types[ii].methods[jj] = dor_object_method_null;
	    }
	}

	doe_object_next_type = class_id+1;
    }

    /* install default methods */

    doe_object_types[class_id].default_method = default_method;

    for (ii = 0;  ii < doe_object_next_method;  ++ii) {
	doe_object_types[class_id].methods[ii] = default_method;
    }

    /* Install type-specific methods. */

    for (ii=0;  ii < n;  ++ii) {
	auto DtInt method_id;		/* Method Type Identifier */

	method_id = methods_list[ii].id;

	if ((method_id < 0) || (method_id >= doe_object_next_method)) {
	    sprintf (DDerror_GetLine(),
		    "invalid method index %d, defining type %d",
		    method_id, class_id);
	    DDerror (ERR_VALUE_OUT_OF_RANGE,
		     "dor_class_create", DDerror_GetLine());
	    return DcNullPtr;
	}

	doe_object_types[class_id].methods[method_id]
	    = methods_list[ii].routine;
    }

    for (ii=0;  ii < doe_object_next_method;  ++ii) {
	if (  (doe_object_derived_method[ii] >= 0)
	   && (doe_object_derived_method[ii] < doe_object_next_method)
	   && (doe_object_types[class_id].methods[ii] == default_method)
	   ) {
	    doe_object_types[class_id].methods[ii] =
		    doe_object_types[class_id]
			.methods[doe_object_derived_method[ii]];
	}
    }

    if (!(class_data = dor_space_allocate (sizeof (*class_data)))) {
	return DcNullPtr;	/* memory exhausted */
    }

    class_data->type  = class_id;
    class_data->count = 0;
    class_data->additional_data_info = dor_list_create();

    doe_object_types[class_id].class_object
	= dor_object_create (DcTypeClass, class_data);

    return doe_object_types[class_id].class_object;
}



/* ====================================================================== */

DtInt dor_class_install (
    DtInt          class_id,
    char          *class_name,
    DtInt          count,
    DtMethodEntry  list[],
    DtMethodPtr    default_routine)
{
    register dot_object *class_object;
    register DtInt       i;

    if (class_id+1 > dor_object_inq_ntypes()) {
	dor_global_exeset_obj_types_report (class_id+1);
    }

    class_object = dor_class_create (class_id, count, list, default_routine);
    dor_object_set_name_string (class_object, class_name, DcFalse);

    for (i=0;  i < doe_object_notify_count;  ++i) {
	(*(doe_object_notify[i])) (class_name, class_id);
    }

    return class_id;
}



/* ====================================================================== */

DtInt dor_class_add (
    char          *class_name,
    DtInt          count,
    DtMethodEntry  list[],
    DtMethodPtr    default_routine)
{
    auto     DtInt       class_id;
    auto     dot_object *class_object;
    register DtInt       i;

    class_id = doe_object_next_type;

    dor_global_exeset_obj_types_report (class_id+1);

    class_object = dor_class_create(class_id, count, list, default_routine);
    dor_object_set_name_string (class_object, class_name, DcFalse);

    for (i=0;  i < doe_object_notify_count;  ++i) {
	(*(doe_object_notify[i])) (class_name, class_id);
    }

    return class_id;
}



/* ====================================================================== */

void dor_class_delete (dot_object *object)
{
    dot_class *dataptr;

    dataptr = (dot_class *) object->data;

    if (dataptr->additional_data_info) {
	dor_list_destroy (dataptr->additional_data_info);
    }

    dor_space_deallocate (dataptr);
}



/* ====================================================================== */

void dor_class_add_notify (DtPFV rtn)
{
    DtPFV *new_list;
    DtInt  size;

    if ((doe_object_notify_count + 1) > doe_object_notify_max) {
	size = (doe_object_notify_count + 5) * sizeof (DtPFV);
	new_list = dor_space_reallocate(doe_object_notify,size);
	if (!new_list) {
	    DDerror (ERR_CANT_ALLOC_MEM, "dor_class_add_notify", DcNullPtr);
	    return;
	}
	doe_object_notify      = new_list;
	doe_object_notify_max += 5;
    }

    doe_object_notify[doe_object_notify_count++] = rtn;
}



/* ====================================================================== */

DtInt dor_class_add_object_data (
    DtInt       class_id,
    DtMethodPtr crt_rtn,
    DtMethodPtr prt_rtn,
    DtMethodPtr dst_rtn)
{
    dot_class           *class_data;
    dot_additional_data *add_data;

#   ifdef ERRCHK
	if (  (class_id < 0)
	   || (class_id >= doe_object_next_type)
	   || (!doe_object_types[class_id].class_object)
	   ) {
	    sprintf(DDerror_GetLine(), "class_id %d",class_id);
	    DDerror (ERR_INVALID_OBJ_TYPE,
		     "dor_class_add_object_data", DDerror_GetLine());
	    return -1;
	}

	if (!crt_rtn || !prt_rtn || !dst_rtn) {
	    DDerror (ERR_VALUE_OUT_OF_RANGE,
		     "dor_class_add_object_data", "NULL routine pointer");
	    return -1;
	}
#   endif

    class_data = (dot_class *) doe_object_types[class_id].class_object->data;
    add_data   = dor_space_allocate (sizeof(dot_additional_data));

    if (!add_data){
	sprintf (DDerror_GetLine(), "%d bytes", sizeof(dot_additional_data));
	DDerror (ERR_CANT_ALLOC_MEM,
		 "dor_class_add_object_data", DDerror_GetLine());
	return -1;
    }

    add_data->offset  = class_data->count;
    add_data->crt_rtn = crt_rtn;
    add_data->prt_rtn = prt_rtn;
    add_data->del_rtn = dst_rtn;

    dor_list_add_data_last (class_data->additional_data_info, add_data);

    class_data->count++;

    return add_data->offset;
}



/* ====================================================================== */

DtInt dor_class_all_add_method (
    char         *method_name,
    DtInt         n,
    DtMethodEntry methods_list[])
{
    auto DtInt new_method_id;

    new_method_id = doe_object_next_method;

    if (!dor_class_all_install_method
	     (new_method_id, n, methods_list, -1, (DtMethodPtr)0)) {
	return -1;	/* error: add failed */
    }

    /* Install name of new method. */

    if (doe_object_method_name[new_method_id]) {
	dor_space_deallocate (doe_object_method_name[new_method_id]);
    }

    if (!method_name) {
	doe_object_method_name[new_method_id] = DcNullPtr;
    } else {
	doe_object_method_name[new_method_id] =
		dor_space_allocate (strlen(method_name)+1);
	strcpy (doe_object_method_name[new_method_id], method_name);
    }

    return new_method_id;
}



/* ====================================================================== */

DtInt dor_class_all_add_initialized_method (
    char          *method_name,
    DtInt          n,
    DtMethodEntry  methods_list[],
    DtMethodPtr    initial_routine_all)
{
    auto DtInt new_method_id;

    new_method_id = doe_object_next_method;

    if (!dor_class_all_install_method
	     (new_method_id, n, methods_list, -1, initial_routine_all)) {
	return -1;	/* error: add failed */
    }

    /* Install name of new method. */

    if (doe_object_method_name[new_method_id]) {
	dor_space_deallocate (doe_object_method_name[new_method_id]);
    }

    if (!method_name) {
	doe_object_method_name[new_method_id] = DcNullPtr;
    } else {
	doe_object_method_name[new_method_id] =
		dor_space_allocate (strlen(method_name)+1);
	strcpy (doe_object_method_name[new_method_id], method_name);
    }

    return new_method_id;
}



/* ====================================================================== */

DtInt dor_class_all_copy_method (
    DtInt         method_id,
    char         *method_name,
    DtInt         n,
    DtMethodEntry methods_list[])
{
    auto DtInt new_method_id;

    new_method_id = doe_object_next_method;

    if (!dor_class_all_install_method
	     (new_method_id, n, methods_list, method_id, DcNullPtr)) {
	return -1;	/* error: add failed */
    }

    /* Install name of new method. */

    if (doe_object_method_name[new_method_id]) {
	dor_space_deallocate (doe_object_method_name[new_method_id]);
    }

    if (!method_name) {
	doe_object_method_name[new_method_id] = DcNullPtr;
    } else {
	doe_object_method_name[new_method_id] =
		dor_space_allocate(strlen(method_name)+1);
	strcpy (doe_object_method_name[new_method_id], method_name);
    }

    return new_method_id;
}



/* ====================================================================== */

DtFlag dor_class_all_install_method (
    DtInt         method,
    DtInt         n,
    DtMethodEntry methods_list[],
    DtInt         copy_method,
    DtMethodPtr   initial_routine_all)
{
    register DtInt i,j;
    register DtInt space;

    /*  First, every class is initialized to either the system's defaults
    **  routine, or to the user specified initial routine if different from
    **  "DcNullPtr".  Later, the methods_list array is used to overwrite
    **  specific class routines.  In that sense, if both a methods_list array
    **  and a default initial routine are passed, the methods_list array has
    **  precedence.  */

    if (method < 0) {
	sprintf (DDerror_GetLine(), "method %d",method);
	DDerror (ERR_INVALID_METHOD,
		 "dor_class_all_install_method", DDerror_GetLine());
	return DcFalse;
    }

    if (copy_method >= doe_object_next_method) {
	sprintf (DDerror_GetLine(), "copy method %d", copy_method);
	DDerror (ERR_INVALID_METHOD,
		 "dor_class_all_install_method", DDerror_GetLine());
	return DcFalse;
    }

    /* Allocate more space. */

    if (method >= doe_object_next_method) {

	/* Expand the derived method list */
	{
	    auto Dt32Bits *ptr;

	    space = (method+1) * sizeof(*ptr);
	    ptr = dor_space_reallocate (doe_object_derived_method, space);
	    if (!ptr) return DcFalse;

	    doe_object_derived_method = ptr;
	}

	/* expand the method name list */
	{
	    auto char **ptr;

	    space = (method+1) * sizeof(*ptr);
	    ptr = dor_space_reallocate (doe_object_method_name, space);
	    if (!ptr) return DcFalse;
	    doe_object_method_name = ptr;
	}

	for (i=doe_object_next_method;  i <= method;  ++i) {
	    doe_object_derived_method[i] = -1;
	    doe_object_method_name[i]    = DcNullPtr;
	}

	/* Expand the method list for each class. */

	space = (method+1) * sizeof(DtMethodPtr);

	for (i = 0; i < doe_object_next_type; i++) {
	    doe_object_types[i].methods =
		    dor_space_reallocate (doe_object_types[i].methods, space);

	    if (!(doe_object_types[i].methods))  {
		return DcFalse;
	    }

            if (initial_routine_all) {
                /* A default routine was specified; use it for all classes. */

	        for (j=doe_object_next_method;  j <= method;  ++j) {
	   	    doe_object_types[i].methods[j] = initial_routine_all;
	        }

	    } else {
                /* Use the kernel default routine for the class. */

	        for (j=doe_object_next_method;  j <= method;  ++j) {
	   	    doe_object_types[i].methods[j] =
                        doe_object_types[i].default_method;
	        }
	    }
	}

	doe_object_next_method = method + 1;
    }

    /* set derived flag */

    doe_object_derived_method[method] = copy_method;

    /* if copy_method specified install copied method routines */

    if (copy_method >= 0) {
	for (i=0;  i < doe_object_next_type;  i++) {
	    doe_object_types[i].methods[method] =
		    doe_object_types[i].methods[copy_method];
	}
    }

    /* insert any specified methods */

    for (i = 0; i < n; i++) {
	auto DtInt method_id;	/* Method Name/Identifier */

	method_id = methods_list[i].id;

	if ((method_id < 0) || (method_id >= doe_object_next_type)) {
	    sprintf (DDerror_GetLine(),
		     "invalid type index %d while defining method %d",
		     method_id, method);

	    DDerror (ERR_VALUE_OUT_OF_RANGE,
		     "dor_object_add_primitive_method", DDerror_GetLine());

	    return DcFalse;
	}

	doe_object_types[j].methods[method] = methods_list[i].routine;
    }

    return DcTrue;
}



/* ====================================================================== */

DtInt dor_class_all_replace_method (
    char          *method_name,
    DtInt          method_id,
    DtInt          n,
    DtMethodEntry  methods_list[],
    DtInt          cpy_method_id)
{
    if (!dor_class_all_install_method
	     (method_id, n, methods_list, cpy_method_id, DcNullPtr)) {
	return -1;	/* error: add failed */
    }

    /* Install name of new method */

    if (doe_object_method_name[method_id]) {
	dor_space_deallocate (doe_object_method_name[method_id]);
    }

    if (!method_name) {
	doe_object_method_name[method_id] = DcNullPtr;
    } else {
	doe_object_method_name[method_id]
	    = dor_space_allocate (strlen(method_name)+1);
	strcpy (doe_object_method_name[method_id], method_name);
    }

    return method_id;
}



/* ====================================================================== */

DtMethodPtr dor_class_inq_default_method (DtInt class_id)
{
#   ifdef ERRCHK
    {   if (  (class_id < 0)
	   || (class_id >= doe_object_next_type)
	   || (!doe_object_types[class_id].class_object)
	   ) {
	    sprintf(DDerror_GetLine(), "class_id %d",class_id);
	    DDerror (ERR_INVALID_OBJ_TYPE,
		"dor_class_inq_default_method", DDerror_GetLine());
	    return dor_object_method_null;
	}
    }
#   endif

    return doe_object_types[class_id].default_method;
}



/* ====================================================================== */

DtMethodPtr dor_class_inq_method (
    DtInt class_id,
    DtInt method_id)
{
    if (method_id == DcMethodNull) {
	return dor_object_method_null;
    }

#   ifdef ERRCHK
    {   if ((class_id < 0) || (class_id >= doe_object_next_type)) {
	    sprintf(DDerror_GetLine(),"class_id %d",class_id);
	    DDerror (ERR_INVALID_OBJ_TYPE,
			"dor_class_inq_method", DDerror_GetLine());
	}

	if ((method_id < 0) || (method_id >= doe_object_next_method)) {
	    sprintf(DDerror_GetLine(),"invalid method id %d",method_id);
	    DDerror (ERR_INVALID_METHOD,
			"dor_class_inq_method", DDerror_GetLine());
	}
    }
#   endif

    return (doe_object_types[class_id].methods[method_id]);
}



/* ====================================================================== */

void dor_class_set_method (
    DtInt       class_id,
    DtInt       method_id,
    DtMethodPtr rtn)
{
#   ifdef ERRCHK
    {   if ((class_id < 0) || (class_id >= doe_object_next_type)) {
	    sprintf (DDerror_GetLine(), "class_id %d", class_id);
	    DDerror (ERR_INVALID_OBJ_TYPE,
			"dor_class_inq_method", DDerror_GetLine());
	}

	if ((method_id < 0) || (method_id >= doe_object_next_method)) {
	    sprintf (DDerror_GetLine(), "invalid method id %d", method_id);
	    DDerror (ERR_INVALID_METHOD,
			"dor_class_inq_method", DDerror_GetLine());
	}
    }
#   endif

    doe_object_types[class_id].methods[method_id] =
	rtn ? rtn : dor_object_method_null;
}



/* ====================================================================== */

void dor_class_print (dot_object *object)
{
    DtInt object_name_type;

    object_name_type = dor_object_inq_name_type (object);

    switch (object_name_type) {
	case DcNameNone: {
	    printf ("class object:  (no name), %lx\n", object->data);
	    break;
	}

	case DcNameInteger: {
	    auto DtInt object_name = dor_object_inq_name_integer (object);

	    printf ("class object: (integer name %ld), %lx\n",
		(long) object_name, object->data);
	    break;
	}

	case DcNameString: {
	    auto char *object_name = dor_object_inq_name_string (object);

	    printf ("class object: (string name \"%s\"), %lx\n",
		object_name, object->data);
	    break;
	}
    }
}



/* ====================================================================== */

DtInt dor_class_inq_id (char *class_name)
{
    dot_object *obj;

    obj = dor_object_inq_via_string (class_name, DcTypeClass);

    return (!obj) ? (-1) : dor_object_inq_class_type (obj);
}
