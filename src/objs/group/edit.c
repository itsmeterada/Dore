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
	int dor_group_open (group_handle,append)
	dot_object * dor_group_close ()
	void dor_group_add_object (object)
	void dor_group_add_object_into (group_object,object)
	DtFlag dor_group_delete_between_labels (label_1,label_2)
	int dor_group_delete_elements (count)
	int dor_group_delete_element_range (from,to)
	int dor_group_replace_element (group_object,object)
	int dor_group_replace (object)
	int dor_group_replace_in (group_object,object)
	int dor_group_empty (group_object)
	int dor_group_insert_element (group_object,object)

  ======================================================================
 */
#include <dore/dore_develop/private/group.h>
#include <dore/internal/dogen.h>
#include <dore/internal/list.h>

/*
 ======================================================================
 */

void dor_group_open (
    dot_object *group_handle,
    DtFlag append)
{
    dot_group *group;
	
#ifdef ERRCHK
    DtInt type;

    if ((DcObjectValid != dor_object_validate(group_handle)) ||
	(((type = dor_object_inq_type(group_handle)) != doe_group_class_id) &&
	 (type != doe_group_inline_class_id))) {
	DDerror(ERR_INVALID_GROUP,
		   "DgOpen",DcNullPtr);
	return;
    }
#endif

    group = (dot_group *)group_handle->data;

    if (doe_group_current_group != DcNullPtr){
	dor_stack_push(doe_group_edit_stack, 
		       doe_group_current_group);
    }

    doe_group_current_group = group_handle;

    if (append){
	group->current_element = group->first_element;
	group->previous_element = (dot_group_element *)0;

	if (group->current_element){
	    while (group->current_element){
		group->previous_element = group->current_element;
		group->current_element = group->current_element->next;
	    }
	}
    }
}
/*
 ======================================================================
 */

dot_object *dor_group_close (void)
{
    dot_object *group_object;
    DtFlag ok;

    if (doe_group_current_group == DcNullPtr) {
	return(DcNullPtr);
    } else {
	group_object = doe_group_current_group;

	while ((ok = dor_stack_pop(doe_group_edit_stack,
				   (DtPtr*)(&doe_group_current_group))) &&
	       DcObjectValid != dor_object_validate(doe_group_current_group));
	if (!ok) {
	    /*
	     * empty stack so no current group
	     */
	    doe_group_current_group = DcNullPtr;
	}
	 
	return(group_object);
    }
}
/*
 ======================================================================
 */

void dor_group_add_object (dot_object *object)
{
#ifdef ERRCHK
    if (DcObjectValid != dor_object_validate(object)) {
	DDerror(ERR_BAD_OBJECT,
		   "DgAddObj",DcNullPtr);
	return;
    }

    if (doe_group_current_group == DcNullPtr) {
	DDerror(ERR_GROUP_NOT_OPEN,
		   "DgAddObj", DcNullPtr);
	return;
    }
#endif

    dor_group_insert_element(doe_group_current_group,object);
}
/*
 ======================================================================
 */

void dor_group_add_object_into (
    dot_object *group_object,
    dot_object *object)
{
#ifdef ERRCHK
    DtInt type;

    if ((DcObjectValid != dor_object_validate(group_object)) ||
	(((type = dor_object_inq_type(group_object)) != doe_group_class_id) &&
	 (type != doe_group_inline_class_id))) {
	DDerror(ERR_INVALID_GROUP,
		   "DgAddObjToGroup",DcNullPtr);
	return;
    }

    if (DcObjectValid != dor_object_validate(object)) {
	DDerror(ERR_BAD_OBJECT,
		   "DgAddObjToGroup",DcNullPtr);
	return;
    }
#endif

    dor_group_insert_element(group_object,object);
}
/*
 ======================================================================
 */

DtFlag dor_group_delete_between_labels (
    DtInt label_1,
    DtInt label_2)
{
    int from,to;
    dot_group *group;
    dot_group_element *element;
    int element_pointer;
    static int label_class_id = -1;

    if (label_class_id == -1) {
	label_class_id = dor_class_inq_id("DoLabel");
    }

    if (doe_group_current_group == DcNullPtr){
	DDerror(ERR_GROUP_NOT_OPEN,
		   "DgDelEleBetweenLabels", DcNullPtr);
	return(DcFalse);
    }

    group = (struct group *)doe_group_current_group->data;
    element = group->current_element;
    element_pointer = dor_group_inq_element_pointer();

	

    /* search for first label */

    from = 0;

    while (element != (dot_group_element *)0){
	if ((dor_object_inq_type(element->object) == label_class_id) &&
	    (label_1 == ((int)(element->object->data)))){
	    from = element_pointer;
	    break;
	}
	element = element->next;
	++element_pointer;
    }

    if (from == 0){
	DDerror(ERR_CANT_FIND_FST_LAB,
		   "DgDelEleBetweenLabels", DcNullPtr);
	return(DcFalse);
    }

    /* dont want to delete label */

    from++;

    /* search for second label */

    to = 0;

    while (element != (dot_group_element *)0){
	if ((dor_object_inq_type(element->object) == label_class_id) &&
	    (label_2 == ((int)(element->object->data)))){
	    to = element_pointer;
	    break;
	}
	element = element->next;
	++element_pointer;
    }

    if (to == 0){
	DDerror(ERR_CANT_FIND_SND_LAB,
		   "DgDelEleBetweenLabels", DcNullPtr);
	return(DcFalse);
    }

    /* dont want to delete label */

    to--;

    if (to < from)
	    return(DcFalse);	/* null range to delete */

    dor_group_delete_element_range(from,to);

    return(DcTrue);
}
/*
 ======================================================================
 */

void dor_group_delete_elements (DtInt count)
{
    dot_group *group;
    dot_group_element *previous,*current,*next;
    int i;

    if (doe_group_current_group == DcNullPtr){
	DDerror (ERR_GROUP_NOT_OPEN, "DgDelEle", DcNullPtr);
	return;
    }

    if (count <= 0) return;

    group = (dot_group *)doe_group_current_group->data;
    current = group->current_element;
    previous = group->previous_element;

    if (!current) return;

    for (i = 1; (i <= count) && (current != (dot_group_element *)0); i++){
	next = current->next;
	dor_object_delete_reference (current->object,doe_group_current_group);
	dor_space_deallocate (current);
	current = next;
    }

    if (previous == (dot_group_element *)0){
	group->first_element = next;
    } else{
	previous->next = next;
    }

    group->current_element = next;

}
/*
 ======================================================================
 */

void dor_group_delete_element_range (
    DtInt from,
    DtInt to)
{
    if (doe_group_current_group == DcNullPtr){
	DDerror(ERR_GROUP_NOT_OPEN,
		   "DgDelEleRange", DcNullPtr);
	return;
    }

    dor_group_set_element_pointer(from,DcBeginning);
    dor_group_delete_elements(to-from+1);
}
/*
 ======================================================================
 */

void dor_group_replace_element (
    dot_object *group_object,
    dot_object *object)
{
    dot_group *group;
    dot_group_element *current,*previous;

    group = (dot_group *)group_object->data;
    current = group->current_element;
    previous = group->previous_element;

    if (current == (dot_group_element *)0) {
	dor_group_insert_element(group_object,object);
	group->current_element = group->previous_element;
	group->previous_element = previous;
	return;
    } else {
	dor_object_delete_reference(current->object,group_object);
	current->object = object;
	dor_object_add_reference(current->object,group_object);
    }
}
/*
 ======================================================================
 */

void dor_group_replace (dot_object *object)
{
    if (doe_group_current_group == DcNullPtr){
	DDerror(ERR_GROUP_NOT_OPEN,
		   "DgReplaceObj", DcNullPtr);
	return;
    }

    dor_group_replace_element(doe_group_current_group,object);
}
/*
 ======================================================================
 */

void dor_group_replace_in (
    dot_object *group_object,
    dot_object *object)
{
    dor_group_replace_element(group_object,object);
}
/*
 ======================================================================
 */

void dor_group_empty (dot_object *group_object)
{
    dot_group *group = (dot_group *)group_object->data;
    dot_group_element *next,*current;

    current = group->first_element;

    while (current != (dot_group_element *)0){
	next = current->next;
	dor_object_delete_reference(current->object,group_object);
	dor_space_deallocate(current);
	current = next;
    }

    group->first_element = (dot_group_element *)NULL;
    group->previous_element = (dot_group_element *)NULL;
    group->current_element = (dot_group_element *)NULL;
}
/*
 ======================================================================
 */

void dor_group_insert_element (
    dot_object *group_object,
    dot_object *object)
{
    dot_group *group;
    dot_group_element *new_element;

    group = (dot_group *)group_object->data;

    new_element = (dot_group_element *)dor_space_allocate(
							  sizeof(dot_group_element));

    if (new_element == (dot_group_element *)0)
	    return;

    new_element->object = object;
    new_element->next = group->current_element;

    /* add element between previous and current */

    if (group->previous_element != (dot_group_element *)0){
	group->previous_element->next = new_element;
    }
    else {
	group->first_element = new_element;
    }

    group->previous_element = new_element;

    dor_object_add_reference(object,group_object);
}
