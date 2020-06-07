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
	dor_group_check
	dor_group_inq_open
	dor_group_inq_size
	dor_group_inq_element_pointer
	dor_group_inquire_object_at_position
	dor_group_set_element_pointer
	dor_group_set_ele_ptr_rel_label
	dor_group_find_label

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/group.h>

/*
 ======================================================================
 */

DtGroupNetworkStatus dor_group_check (dot_object *object)
{
#ifdef ERRCHK
    DtInt type;
    if ((DcObjectValid != dor_object_validate(object)) ||
	(((type = dor_object_inq_type(object)) != doe_group_class_id) &&
	 (type != doe_group_inline_class_id))) {
	DDerror(ERR_INVALID_GROUP,
		   "DgCheck",DcNullPtr);
	return(DcGroupBad);
    }
#endif

    /*
     * The variable doe_system_active_traversals is used to indicate
     * that a function is active that requires traversals of the
     * data base.  These functions include:
     *   DdUpdate		
     *   DdPick		
     *   DfUpdate
     *   DgCheck		
     *   DsCompBoundingVol
     *   DvUpdate	
     * Multiple active traversals are not possible because
     * the state of a traversal is maintained in the global 
     * attributes.  This state would need to be saved and 
     * restored if traversals were allowed to be re-entrant.
     * Further each of the above sub-systems also maintains state
     * during traversal and that state would likewise need to be 
     * saved and restored.
     */

    if (doe_system_active_traversals) {
	DDerror (ERR_MULTIPLE_TRAVERSAL,
		    "DgCheck", DcNullPtr);
	return(DcGroupBad);
    }

    doe_system_active_traversals++;

    doe_system_current_method = DcMethodCheckGroup;

    dor_object_get_current_method(object)(object);

    doe_system_current_method = DcMethodNull;

    doe_system_active_traversals--;

    return (doe_group_network_state);
}
/*
 ======================================================================
 */

dot_object *dor_group_inq_open (void)
{
    return(doe_group_current_group);
}
/*
 ======================================================================
 */

DtInt dor_group_inq_size (dot_object *group_object)
{
    dot_group *group;
    dot_group_element *element;
    int size;

    group = (dot_group *)group_object->data;

    element = group->first_element;

    size = 0;

    for (element = group->first_element; 
	 element != (dot_group_element *)0; 
	 element = element->next){
	++size;
    }

    return(size);
}
/*
 ======================================================================
 */

DtInt dor_group_inq_element_pointer (void)
{
    dot_group *group;
    dot_group_element *element;
    int current = 0;


    if (doe_group_current_group == DcNullPtr){
	DDerror(ERR_GROUP_NOT_OPEN,
		   "DgInqElePtr", DcNullPtr);
	return -1;
    }

    group = (dot_group *)doe_group_current_group->data;

    for (element = group->first_element; 
	 element != (dot_group_element *)0; 
	 element = element->next){
	if (element == group->current_element)
		return current;
	++current;
    }
    return current;
}
/*
 ======================================================================
 */

dot_object *dor_group_inquire_object_at_position (
    dot_object *group_object,
    DtInt offset,
    DtInt type)
{
    dot_group *group;
    dot_group_element *object;
    int size;

    group = (dot_group *)group_object->data;
    size = dor_group_inq_size(group_object);

    switch (type){
    case DcEnd:
	offset += size; 
	break;

    case DcCurrent:
	for (object = group->first_element; 
	     object != (dot_group_element *)NULL;
	     object = object->next){
	    if (object == group->current_element){
		break;
	    }

	    offset++;
	}
	break;
    }

    if ((0 > offset) || (offset >= size)){
	return (dot_object *)NULL;
    }

    object = group->first_element;

    while (offset--){
	object = object->next;
    }

    return(object->object);
}
/*
 ======================================================================
 */

void dor_group_set_element_pointer (
    DtInt element_ptr,
    DtInt type)
{
    dot_group *group;
    int size;

    if (doe_group_current_group == DcNullPtr){
	DDerror(ERR_GROUP_NOT_OPEN,
		   "DgSetElePtr", DcNullPtr);
	return;
    }

    group = (dot_group *)doe_group_current_group->data;
    size = dor_group_inq_size(doe_group_current_group);

    switch (type) {
    case DcEnd: 
	element_ptr += size; 
	break;
    case DcCurrent:
	element_ptr += dor_group_inq_element_pointer();
	break;
    }

    element_ptr = max(element_ptr,0);
    element_ptr = min(element_ptr,size);

    group->current_element = group->first_element;
    group->previous_element = (dot_group_element *)0;

    while (element_ptr--){
	group->current_element = group->current_element->next;
	if (group->previous_element != (dot_group_element *)0)
		group->previous_element= group->previous_element->next;
	else group->previous_element = group->first_element;
    }
}
/*
 ======================================================================
 */

DtFlag dor_group_set_ele_ptr_rel_label (
    DtInt label,
    DtInt offset)
{
    int i;

    if (doe_group_current_group == DcNullPtr){
	DDerror(ERR_GROUP_NOT_OPEN,
		   "DgSetElePtrRelLabel", DcNullPtr);
	return(DcFalse);
    }

    i = dor_group_find_label(label);

    if (i == -1){
	return(DcFalse);
    }

    i += offset;
    dor_group_set_element_pointer(i,DcBeginning);
    return(DcTrue);
}
/*
 ======================================================================
 */

DtInt dor_group_find_label (DtInt label)
{
    dot_group *group;
    dot_group_element *element;
    DtInt element_pointer;
    static int label_class_id = -1;

    if (label_class_id == -1) {
	label_class_id = dor_class_inq_id("DoLabel");
    }

    group = (dot_group *)doe_group_current_group->data;
    element = group->current_element;
    element_pointer = dor_group_inq_element_pointer();

    /* search for label */

    while (element != (dot_group_element *)0) {
	if ((dor_object_inq_type(element->object) == label_class_id) &&
	    (label == ((int)(element->object->data))))
		return element_pointer;

	element = element->next;
	++element_pointer;
    }

    return -1;
}
