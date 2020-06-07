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
	dor_object_initialize
	dor_object_terminate
	dor_object_create
	dor_object_check_deletion
	dor_object_hold
	dor_object_release
	dor_object_allocate
	dor_object_add_reference
	dor_object_delete_reference
	dor_object_reference_insert
	dor_object_reference_remove
	dor_object_confirm_type
	dor_object_validate
	dor_object_inq_class_type
	dor_object_set_name_integer
	dor_object_set_name_string
	dor_object_inq_name_type
	dor_object_inq_name_integer
	dor_object_inq_name_string
	dor_object_inq_obj_integer
	dor_object_inq_obj_string
	dor_object_inq_hold
	dor_object_inq_ntypes
	dor_object_inq_type
	dor_object_print_type
	dor_object_print

  ======================================================================
 */
#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/internal/list.h>
#include <dore/internal/system.h>
#include "obj.h"

/* doubly linked chain of object header blocks */
dot_object_block *doe_object_blocks;

/* linked list of free object slots */
dot_object *doe_object_free_list;

/* linked list of deleted, but not yet reclaimed objects */
dot_object *doe_object_deleted_list;

/* next available method number */
DtInt doe_object_next_method;

/* next available object type number */
Dt32Bits doe_object_next_type;

/* object dictionary */
struct dictionary *doe_object_dictionary;

/* Dore dispatch table */
dot_object_type *doe_object_types;

/* list of methods derived from other ones */
Dt32Bits  *doe_object_derived_method;
char     **doe_object_method_name;

/* list of functions to call when new class added */
DtPFV    *doe_object_notify;
Dt32Bits  doe_object_notify_count;
Dt32Bits  doe_object_notify_max;

/* variables used for inq first, inq next */
dot_object_block * dor_object_scan_block;
Dt32Bits doe_object_scan_item;
Dt32Bits doe_object_scan_type;




/* ====================================================================== */

static DtMethodEntry class_methods[] = {
    { DcMethodPrint,   (DtMethodPtr) dor_class_print  },
    { DcMethodDestroy, (DtMethodPtr) dor_class_delete }
};

void dor_object_initialize (void)
{
    register DtInt ii;

    doe_object_blocks       = DcNullPtr;
    doe_object_free_list    = DcNullPtr;
    doe_object_deleted_list = DcNullPtr;
    dor_object_scan_block   = DcNullPtr;
    doe_object_dictionary   = dor_dictionary_create ();

    doe_object_notify       = DcNullPtr;
    doe_object_notify_count = 0;
    doe_object_notify_max   = 0;

    /* set up required methods and types */

    doe_object_types = DcNullPtr;
    doe_object_next_method = dod_object_first_defined_method;
    doe_object_derived_method =
	dor_space_allocate
	    (doe_object_next_method * sizeof(*doe_object_derived_method));
    doe_object_method_name =
	dor_space_allocate
	    (doe_object_next_method * sizeof(*doe_object_method_name));

    for (ii=0;  ii < doe_object_next_method;  ++ii) {
	doe_object_derived_method[ii] = -1;
	doe_object_method_name[ii]    = DcNullPtr;
    }

    /* Set standard method names. */

    doe_object_method_name[DcMethodInqGlbAttVal] = (char*)
	    dor_space_allocate (strlen(dod_InqGlbAttValName) + 1);
    strcpy (doe_object_method_name[DcMethodInqGlbAttVal], dod_InqGlbAttValName);

    doe_object_method_name[DcMethodUpdStdAltObj] =
	    dor_space_allocate (strlen(dod_UpdStdAltObjName) + 1);
    strcpy (doe_object_method_name[DcMethodUpdStdAltObj], dod_UpdStdAltObjName);

    doe_object_method_name[DcMethodUpdStdAltLineObj] =
            dor_space_allocate (strlen(dod_UpdStdAltLineObjName) + 1);
    strcpy (doe_object_method_name[DcMethodUpdStdAltLineObj],
	    dod_UpdStdAltLineObjName);

    doe_object_method_name[DcMethodStdRenderStudio] =
	    dor_space_allocate (strlen(dod_StdRenderStudioName) + 1);
    strcpy (doe_object_method_name[DcMethodStdRenderStudio],
	    dod_StdRenderStudioName);

    doe_object_method_name[DcMethodStdRenderDisplay] =
	    dor_space_allocate (strlen(dod_StdRenderDisplayName) + 1);
    strcpy (doe_object_method_name[DcMethodStdRenderDisplay],
	    dod_StdRenderDisplayName);

    doe_object_next_type = 0;

    dor_class_install (DcTypeClass, "Class", 1, class_methods, DcNullPtr);
    dor_class_install (DcTypeDeleted, "Deleted", 0, DcNullPtr, DcNullPtr);
}



/* ====================================================================== */

void dor_object_terminate (void)
{
}



/* ====================================================================== */

dot_object *dor_object_create (
    DtInt object_type,
    DtPtr object_data)
{
    dot_object *object;
    dot_class *class_data;
    dot_additional_data *add_data;

#   ifdef ERRCHK
	if ((object_type != 0) &&
	    ((object_type < 0) ||
	     (object_type >= doe_object_next_type) ||
	     (!doe_object_types[object_type].class_object))) {
	    sprintf (DDerror_GetLine(), "type %d", object_type);
	    DDerror (ERR_INVALID_OBJ_TYPE,
		"dor_object_create", DDerror_GetLine());
	    return DcNullPtr;
	}
#   endif

    if (doe_object_types[object_type].class_object) {
	class_data = (dot_class *)
		doe_object_types[object_type].class_object->data;
    }

    /*  If the free list of objects is empty, then make the deleted object
    **  list the free list, and empty the deleted object list.  */

    if (!doe_object_free_list) {
	doe_object_free_list = doe_object_deleted_list;
	doe_object_deleted_list = DcNullPtr;
    }

    /*  If the deleted object list was also empty, then allocate some new
    **  objects and put them in the free list.  */

    if (!doe_object_free_list) {
	dor_object_allocate();
    }

    /*  Obtain the first object from the free list.  */

    object = doe_object_free_list;
    doe_object_free_list = (dot_object *)(object->data);

    object->ref_count = 0;
    object->info = object_type;
    object->data = object_data;

    if (doe_object_types[object_type].class_object && class_data->count > 0) {
	object->additional_data =
		dor_space_allocate (class_data->count * sizeof(DtPtr));
	if (!(object->additional_data)) {
	    return DcNullPtr;
	}
	for (add_data=dor_list_get_first_data(class_data->additional_data_info);
	     add_data;
	     add_data=dor_list_get_next_data(class_data->additional_data_info))
	{   object->additional_data[add_data->offset]
		= (DtPtr) (*(add_data->crt_rtn))(object);
	}
    } else {
	object->additional_data = DcNullPtr;
    }

    return object;
}



/* ====================================================================== */

void dor_object_check_deletion (dot_object *object)
{
    dot_class *class_data;
    Dt32Bits class_id;
    dot_additional_data *add_data;

    if ((object->ref_count == 0) && !(object->info & dod_object_hold_mask)) {
	class_id = dor_object_inq_type (object);

	if(DcObjectValid != dor_object_validate(object)) {
	    DDerror(ERR_BAD_OBJECT, "dor_object_check_deletion", DcNullPtr);
	    return;
	}

	if ((class_id < 0) ||
	    (class_id >= doe_object_next_type) ||
	    (!doe_object_types[class_id].class_object)) {
	    sprintf(DDerror_GetLine(), "type %d",class_id);
	    DDerror (ERR_INVALID_OBJ_TYPE,
		     "dor_object_check_deletion", DDerror_GetLine());
	    return;
	}

	class_data = (dot_class *)doe_object_types[class_id].class_object->data;

	if (class_data->count > 0) {
	    for (add_data = dor_list_get_first_data(class_data->additional_data_info);
		 add_data != (dot_additional_data *)0;
		 add_data = dor_list_get_next_data(class_data->additional_data_info)) {
		(*(add_data->del_rtn))(object,
				       object->additional_data[add_data->offset]);
	    }
	    dor_space_deallocate (object->additional_data);
	    object->additional_data = (DtPtr *)0;
	}

	dor_object_get_method (object, DcMethodDestroy) (object);

	/*  Put the deleted object header on the beginning of the
	**  deleted object list.  */

	object->data = (DtPtr)doe_object_deleted_list;
	doe_object_deleted_list = object;

	/*  Indicate that the object is of type "DcTypeDeleted".  */

	object->info = DcTypeDeleted;
    }
}



/* ====================================================================== */

dot_object *dor_object_hold (dot_object *object)
{
#   ifdef ERRCHK
	if (DcObjectValid != dor_object_validate(object)){
	    DDerror(ERR_BAD_OBJECT, "DsHoldObj",DcNullPtr);
	    return(object);
	}
#   endif

    object->info |= dod_object_hold_mask;

    dor_object_get_method(object,DcMethodAddReference) (object, DcNullPtr);

    return object;
}



/* ====================================================================== */

void dor_object_release (dot_object *object)
{
#   ifdef ERRCHK
	if(DcObjectValid != dor_object_validate(object)){
	    DDerror(ERR_BAD_OBJECT, "DsReleaseObj", DcNullPtr);
	    return;
	}
#   endif

    object->info &= ~dod_object_hold_mask;

    dor_object_get_method(object,DcMethodRemoveReference)(object,DcNullPtr);

    dor_object_check_deletion(object);
}



/* ====================================================================== */

void dor_object_allocate (void)
{
    Dt32Bits i;
    dot_object_block *block;

    block = dor_space_allocate (sizeof(dot_object_block));

    if (block == (dot_object_block *)NULL){
	return;	/* no memory available */
    }

    /* link allocated block onto head of object block list */

    if (doe_object_blocks != (dot_object_block *)NULL){
	doe_object_blocks->previous = block;
    }

    block->previous = (dot_object_block *)NULL;
    block->next = doe_object_blocks;
    doe_object_blocks = block;

    /* chain elements of allocated block together */

    for (i = 0; i < dod_object_block_size; i++){
	doe_object_blocks->objects[i].ref_count = 0;
	doe_object_blocks->objects[i].info = DcTypeDeleted;
	doe_object_blocks->objects[i].data =
		(DtPtr)(doe_object_blocks->objects+i+1);
    }

    doe_object_blocks->objects[dod_object_block_size-1].data = DcNullPtr;

    doe_object_free_list = doe_object_blocks->objects;
}



/* ====================================================================== */

void dor_object_add_reference (
    dot_object *object,
    dot_object *referencing_object)
{
    object->ref_count++;

    dor_object_get_method(object,DcMethodAddReference)
	(object, referencing_object);
}



/* ====================================================================== */

void dor_object_delete_reference (
    dot_object *object,
    dot_object *referencing_object)
{
    object->ref_count--;

    dor_object_get_method(object,DcMethodRemoveReference)
	(object,referencing_object);

    dor_object_check_deletion (object);
}



/* ====================================================================== */

void dor_object_reference_insert (
    dot_object_reference **object_reference,
    dot_object            *reference_object)
{
    dot_object_reference *new;

    if (reference_object == DcNullPtr)
	return;

    new = dor_space_allocate (sizeof(dot_object_reference));

    if (new == ( dot_object_reference *)0)
	return;

    new->next   = *object_reference;
    new->object =  reference_object;
    *object_reference = new;
}



/* ====================================================================== */

void dor_object_reference_remove (
    dot_object_reference **object_reference,
    dot_object            *reference_object)
{
    dot_object_reference *ref, *previous;

    if (reference_object == DcNullPtr)
	    return;

    previous = (dot_object_reference *)0;

    for ( ref = *object_reference;
	 ref != (dot_object_reference *)0;
	 ref = ref->next ){
	if ( ref->object == reference_object ){
	    if ( ref == *object_reference ){
		*object_reference = (*object_reference)->next;
	    }

	    if ( previous != (dot_object_reference *)0 ){
		previous->next = ref->next;
	    }

	    dor_space_deallocate(ref);

	    return;
	}
	previous = ref;
    }
}



/* ====================================================================== */
/* returns DcTrue if the given object is valid and of the specified type. */
/* ====================================================================== */

DtFlag dor_object_confirm_type (
    dot_object *object,
    DtInt       type)
{
    DtObjectStatus status;

    status = (DtObjectStatus) dor_object_validate(object);

    if (status != DcObjectValid){
	return(DcFalse);
    }
    else if (type != dor_object_inq_type(object)){
	return(DcFalse);
    }

    return(DcTrue);
}



/* ====================================================================== */

DtInt dor_object_validate (dot_object *object)
{
    dot_object_block *block;

    /* find block, if any in which object is defined */

    block = doe_object_blocks;

    while ( block ){
	if (  ( object >= block->objects ) &&
	    ( object < block->objects+dod_object_block_size ) ){
	    /* block found, object is valid or deleted */

	    if (( object->info & dod_object_type_mask) == DcTypeDeleted )
		    return DcObjectDeleted;
	    else return DcObjectValid;
	}
	block = block->next;
    }

    return DcObjectInvalid;
}



/* ====================================================================== */

DtInt dor_object_inq_class_type (dot_object *class)
{
    dot_class *classdata;

    if ( ( class->info & dod_object_type_mask ) == DcTypeClass ) {
	classdata = (dot_class *)class->data;
	return classdata->type;
    } else {
	sprintf(DDerror_GetLine(), "type was %d",
		class->info & dod_object_type_mask);
	DDerror (ERR_INVALID_CLASS,
		    "dor_object_inq_class_type",
		    DDerror_GetLine());
    }

    return -1;
}



/*****************************************************************************
**  This function names the specified object with the given integer ID.  This
**  function is used on top of the dictionary function to provide a slot for
**  object handle validation.
*****************************************************************************/

void dor_object_set_name_integer (
    dot_object *object,		/* Target Object to Name */
    DtInt       name,		/* Integer-Valued Name */
    DtFlag      replace)	/* Name-Replace Flag */
{
#   ifdef ERRCHK
	if (DcObjectValid != dor_object_validate (object)){
	    DDerror (ERR_BAD_OBJECT, "DsSetObjNameInteger", DcNullPtr);
	    return;
	}
#   endif

    dor_dictionary_set_name_integer (doe_object_dictionary,object,name,replace);
}



/*****************************************************************************
**  This function names the specified object with the given string ID.  This
**  function is used on top of the dictionary function to provide a slot for
**  object handle validation.
*****************************************************************************/

void dor_object_set_name_string (
    dot_object *object,		/* Target Object to Name */
    char       *name,		/* String-Valued Name */
    DtFlag      replace)	/* Name-Replace Flag */
{
#   ifdef ERRCHK
	if (DcObjectValid != dor_object_validate (object)){
	    DDerror (ERR_BAD_OBJECT, "DsSetObjNameString", DcNullPtr);
	    return;
	}
#   endif

    dor_dictionary_set_name_string (doe_object_dictionary,object,name,replace);
}



/*****************************************************************************
**  This function inquires the name type of the specified object.  This sits
**  on top of the dictionary function to provide a slot for object handle
**  validation (if error-checking is enabled).
*****************************************************************************/

DtNameType dor_object_inq_name_type (dot_object *object)
{
#   ifdef ERRCHK
	if (DcObjectValid != dor_object_validate(object)) {
	    DDerror (ERR_BAD_OBJECT, "DsInqObjNameType", DcNullPtr);
	    return DcNameNone;
	}
#   endif

    return dor_dictionary_inq_name_type (doe_object_dictionary, object);
}



/*****************************************************************************
**  This function inquires the integer name of the specified object.  This
**  sits on top of the dictionary function to provide a slot for object handle
**  validation (if error-checking is enabled).
*****************************************************************************/

DtInt dor_object_inq_name_integer (dot_object *object)
{
#   ifdef ERRCHK
	if (DcObjectValid != dor_object_validate(object)) {
	    DDerror (ERR_BAD_OBJECT, "DsInqObjNameInteger", DcNullPtr);
	    return 0;
	}
#   endif

    return dor_dictionary_inq_name_integer (doe_object_dictionary, object);
}



/*****************************************************************************
**  This function inquires the string name of the specified object.  This
**  sits on top of the dictionary function to provide a slot for object handle
**  validation (if error-checking is enabled).
*****************************************************************************/

char *dor_object_inq_name_string (dot_object *object)
{
#   ifdef ERRCHK
	if (DcObjectValid != dor_object_validate(object)) {
	    DDerror (ERR_BAD_OBJECT, "DsInqObjNameString", DcNullPtr);
	    return DcNullPtr;
	}
#   endif

    return dor_dictionary_inq_name_string (doe_object_dictionary, object);
}



/*****************************************************************************
**  This function returns the handle to the object with the specified integer
**  name.
*****************************************************************************/

dot_object *dor_object_inq_via_integer (
    DtInt name,		/* Integer-Valued Name */
    DtInt class_id)	/* Object Class Identifier */
{
    return dor_dictionary_inq_obj_integer (doe_object_dictionary,name,class_id);
}



/*****************************************************************************
**  This function returns the object handle associated with the given string
**  name.
*****************************************************************************/

dot_object *dor_object_inq_via_string (
    char  *name,	/* String-Valued Name */
    DtInt  class_id)	/* Object Class Identifier */
{
    return dor_dictionary_inq_obj_string (doe_object_dictionary,name,class_id);
}



/* ====================================================================== */

DtFlag dor_object_inq_hold (dot_object *object)
{
#   ifdef ERRCHK
	if(DcObjectValid != dor_object_validate(object)){
	    DDerror(ERR_BAD_OBJECT, "DsInqHoldObj", DcNullPtr);
	    return(DcFalse);
	}
#   endif

    return (object->info & dod_object_hold_mask);
}



/* ====================================================================== */

int dor_object_inq_ntypes (void)
{
    return doe_object_next_type;
}



/* ====================================================================== */

DtInt dor_object_inq_type (dot_object *object)
{
/*
#   ifdef ERRCHK
	if (DcObjectValid != dor_object_validate(object)){
	    DDerror (ERR_BAD_OBJECT, "DsInqObjClassId", DcNullPtr);
	    return(-2);
	}
#   endif
*/

    return (object->info & dod_object_type_mask);
}



/* ====================================================================== */

void dor_object_print_type (dot_object *object)
{
    switch (dor_object_inq_name_type (object)) {
        case DcNameInteger: {
	    auto DtInt object_name = dor_object_inq_name_integer (object);
	    printf ("class object: (integer name: %d), %lx\n",
		object_name, object->data);
	    break;
	}
	case DcNameString: {
	    auto char *object_name = dor_object_inq_name_string (object);
	    printf ("class object: (string name: %s), %lx\n",
		   object_name, object->data);
	    break;
	}
	default: {
	    printf ("class object: (no name), %lx\n", object->data);
	    break;
	}
    }
}



/* ====================================================================== */

void dor_object_print (dot_object *object)
{
    dot_class           *class_data;
    Dt32Bits             class_id;
    dot_additional_data *add_data;
    DtInt                class_name_int;
    char                *class_name_str;
    DtNameType           class_name_type;
    DtInt                hold_method;

#   ifdef ERRCHK
	if(DcObjectValid != dor_object_validate(object)) {
	    DDerror(ERR_BAD_OBJECT, "DsPrintObj", DcNullPtr);
	    return;
	}
#   endif

    class_id = dor_object_inq_type (object);

#   ifdef ERRCHK
	if ((class_id < 0) ||
	    (class_id >= doe_object_next_type) ||
	    (!doe_object_types[class_id].class_object)) {
	    sprintf(DDerror_GetLine(), "type %d",class_id);
	    DDerror (ERR_INVALID_OBJ_TYPE, "DsPrintObj", DDerror_GetLine());
	    return;
	}
#   endif

    hold_method = doe_system_current_method;
    doe_system_current_method = DcMethodPrint;

    class_name_type = dor_object_inq_name_type
			  (doe_object_types[class_id].class_object);
    switch (class_name_type) {
	case DcNameInteger: {
	    class_name_int = dor_object_inq_name_integer
				 (doe_object_types[class_id].class_object);
	    sprintf (dor_print_get_line(), "class %d [%lx]",
		class_name_int, object);
	    break;
	}
	case DcNameString: {
	    class_name_str = dor_object_inq_name_string
		       		 (doe_object_types[class_id].class_object);
	    sprintf (dor_print_get_line(), "\"%s\" [%lx]",
		class_name_str, object);
	    break;
	}
	default: {
	    sprintf (dor_print_get_line(), "class %d [%lx]", class_id, object);
	    break;
	}
    }

    dor_print_output(dor_print_get_line());
    dor_print_indent_up();

    switch (dor_object_inq_name_type (object)) {
	case DcNameInteger: {
	    auto DtInt name = dor_object_inq_name_integer (object);
	    sprintf (dor_print_get_line(), "(name = %d)", name);
	    dor_print_output (dor_print_get_line());
	    break;
	}
	case DcNameString: {
	    auto char *name = dor_object_inq_name_string (object);
	    sprintf (dor_print_get_line(), "(name = \"%s\")", name);
	    dor_print_output (dor_print_get_line());
	    break;
	}
	case DcNameNone: {
	    sprintf (dor_print_get_line(), "(name = <NONE>)");
	    dor_print_output (dor_print_get_line());
	    break;
	}
    }

    dor_object_get_method (object, DcMethodPrint) (object);

    class_data = (dot_class *) doe_object_types[class_id].class_object->data;

    if (class_data->count > 0) {
	for (add_data = dor_list_get_first_data(class_data->additional_data_info);
	     add_data != (dot_additional_data *)0;
	     add_data = dor_list_get_next_data(class_data->additional_data_info)) {
	    (*(add_data->prt_rtn))(object,
				   object->additional_data[add_data->offset]);
	}
    }
    dor_print_indent_down();

    switch (class_name_type) {
	case DcNameInteger:
	    sprintf (dor_print_get_line(), "class %d end", class_name_int);
	    break;
	case DcNameString:
	    sprintf (dor_print_get_line(), "%s end", class_name_str);
	    break;
	default:
	    sprintf (dor_print_get_line(), "class %d end", class_id);
	    break;
    }
    dor_print_output(dor_print_get_line());

    doe_system_current_method = hold_method;
}
