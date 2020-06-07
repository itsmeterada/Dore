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
	int dor_group_initialize ()
	dot_object * dor_group_create (open,is_inline)
	int dor_group_destroy (group_object)
	int dor_group_add_reference (group_object,reference_object)
	int dor_group_remove_reference (group_object,reference_object)
	int dor_group_execute_current (group_object)
	int dor_group_pick (group_object)
	int dor_group_print (group_object)
	int dor_group_check_network (group_object)

  ======================================================================
 */
#include <dore/dore_develop/private/group.h>
#include <dore/internal/dogen.h>
#include <dore/internal/list.h>

/* points to group currently open for editing, or NULL */
dot_object *doe_group_current_group;

/* table of all defined groups */
dot_group *doe_group_defined_groups;

/* list of free group slots */
dot_group *doe_group_next_free_group;

/* list of blocks in group table */
dot_group_table_block *doe_group_table_blocks;

/* group traversal stack */
struct list *doe_group_stack;
struct list *doe_group_att_flag_stack;

/* group traversal state */
DtInt doe_group_traversal_state;

/* group network state for DcMethodCheckGroup */
DtGroupNetworkStatus doe_group_network_state;

/* open group stack */
struct list *doe_group_edit_stack;

/* class ids */

DtInt doe_group_class_id;
DtInt doe_group_inline_class_id;
/*
 ======================================================================
 */

static DtMethodEntry group_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_group_print },
    { DcMethodAddReference,     (DtMethodPtr) dor_group_add_reference },
    { DcMethodRemoveReference,  (DtMethodPtr) dor_group_remove_reference },
    { DcMethodDestroy,          (DtMethodPtr) dor_group_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_group_pick },
    { DcMethodCheckGroup,       (DtMethodPtr) dor_group_check_network },
    { DcMethodIniPick,          (DtMethodPtr) dor_group_execute_current },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_group_execute_current },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_group_execute_current },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_group_execute_current },
};

static DtMethodEntry inline_group_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_group_print },
    { DcMethodAddReference,     (DtMethodPtr) dor_group_add_reference },
    { DcMethodRemoveReference,  (DtMethodPtr) dor_group_remove_reference },
    { DcMethodDestroy,          (DtMethodPtr) dor_group_destroy },
    { DcMethodPick,             (DtMethodPtr) dor_group_pick },
    { DcMethodCheckGroup,       (DtMethodPtr) dor_group_check_network },
    { DcMethodIniPick,          (DtMethodPtr) dor_group_execute_current },
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_group_execute_current },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_group_execute_current },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_group_execute_current },
};

void dor_group_initialize (void)
{
    doe_group_current_group = DcNullPtr;
    dor_group_table_initialize();
    doe_group_stack = dor_stack_create();
    doe_group_att_flag_stack = dor_stack_create();
    doe_group_edit_stack = dor_stack_create();

    doe_group_class_id = dor_class_install (DcTypeGroup, "DoGroup", 
					10, group_methods,DcNullPtr);

    doe_group_inline_class_id = dor_class_install (DcTypeInlineGroup, 
					"DoInLineGroup", 10, 
					inline_group_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_group_create (
    DtFlag open,
    DtFlag is_inline)
{
    dot_group *group;
    dot_object *group_object;

    /* allocate space for new group */

    if (!doe_group_next_free_group) {
	dor_group_table_add_block();
    }

    if (!doe_group_next_free_group) {
	return DcNullPtr;
    }

    group = doe_group_next_free_group;
    doe_group_next_free_group = group->next_group;
	
    /* chain group onto table of defined groups */

    group->flags = 0;
    group->first_element = (dot_group_element *)0;
    group->previous_element = (dot_group_element *)0;
    group->current_element = (dot_group_element *)0;
    group->attribute_stack = dor_stack_create();
    group->previous_group = DcNullPtr;
    group->next_group = doe_group_defined_groups;
    group->references = (dot_object_reference *)0;
	
    if (is_inline)
	    group_object = dor_object_create(doe_group_inline_class_id,group);
    else	
	    group_object = dor_object_create(doe_group_class_id,group);

    group->object = group_object;

    if (doe_group_defined_groups) {
	doe_group_defined_groups->previous_group = group;
    }

    doe_group_defined_groups = group;

    if (open) {
	if (doe_group_current_group != DcNullPtr) {
	    dor_stack_push(doe_group_edit_stack, doe_group_current_group);
	}
	doe_group_current_group = group_object;
    }

    return group_object;
}
/*
 ======================================================================
 */

void dor_group_destroy (dot_object *group_object)
{
    dot_group *group = (dot_group *)group_object->data;
    DtFlag ok;

    dor_group_empty(group_object);

    if (group_object == doe_group_current_group) {
	while ((ok = dor_stack_pop(doe_group_edit_stack,
				   (DtPtr*)(&doe_group_current_group))) &&
	       DcObjectValid != 
	       dor_object_validate(doe_group_current_group));
	if (!ok) {
	    /*
	     * empty stack so no current group
	     */
	    doe_group_current_group = DcNullPtr;
	}
    }

    dor_stack_destroy(group->attribute_stack);
    dor_group_table_delete_group(group);
}
/*
 ======================================================================
 */

void dor_group_add_reference (
    dot_object *group_object,
    dot_object *reference_object)
{
    dot_group *group = (dot_group *)group_object->data;

    dor_object_reference_insert(&(group->references),reference_object);
}
/*
 ======================================================================
 */

void dor_group_remove_reference (
    dot_object *group_object,
    dot_object *reference_object)
{
    dot_group *group = (dot_group *)group_object->data;

    dor_object_reference_remove(&(group->references),reference_object);
}
/*
 ======================================================================
 */

void dor_group_execute_current (dot_object *group_object)
{
    dot_group *group = (dot_group *)group_object->data;
    dot_group_element *element;
    DtInt type = dor_object_inq_type(group_object);

    if (! dor_condex_query_execute(type))
	    return;

    if (type == doe_group_class_id)
	    dor_group_push(group_object, DcFalse);

    doe_group_traversal_state = dod_group_traversal_continue;

    for (element = group->first_element; 
	 element != (dot_group_element *)0; 
	 element = element->next) {
	dor_object_get_current_method(element->object)(element->object);

	if (doe_group_traversal_state)
		break;
    }

    if (type == doe_group_class_id)
	    dor_group_pop(DcFalse);

    if (doe_group_traversal_state == dod_group_traversal_return) {
	doe_group_traversal_state = dod_group_traversal_continue;
    }
}
/*
 ======================================================================
 */

void dor_group_pick (dot_object *group_object)
{
    dot_group *group = (dot_group *)group_object->data;
    dot_group_element *element;
    DtInt type = dor_object_inq_type(group_object);

    if (! dor_condex_query_execute(type))
	    return;

    if (type == doe_group_class_id)
	    dor_group_push(group_object, DcFalse);

    dor_pick_push_group (group_object);

    doe_group_traversal_state = dod_group_traversal_continue;

    for (element = group->first_element; 
	 element != (dot_group_element *)0; 
	 element = element->next){
	dor_pick_initialize_element(DcFalse);	
	dor_object_get_method(element->object,DcMethodPick)
		(element->object);
	dor_pick_terminate_element();
	if (doe_group_traversal_state)
		break;
    }

    dor_pick_pop_group();

    if (type == doe_group_class_id)
	    dor_group_pop(DcFalse);

    if (doe_group_traversal_state == dod_group_traversal_return){
	doe_group_traversal_state = dod_group_traversal_continue;
    }
}
/*
 ======================================================================
 */

void dor_group_print (dot_object *group_object)
{
    dot_group *group = (dot_group *)group_object->data;
    dot_group_element *element;
    DtInt elementno;

    if (group->first_element == (dot_group_element *)NULL){
	dor_print_output("group is empty");
	return;
    }

    elementno = 0;

    element = group->first_element; 

    doe_group_traversal_state = dod_group_traversal_continue;

    while (element != (dot_group_element *)NULL) {
	if (element == group->current_element) {
	    sprintf(dor_print_get_line(),"*element %d:",elementno);
	    dor_print_output(dor_print_get_line());
	} else {
	    sprintf(dor_print_get_line(),"element %d:",elementno);
	    dor_print_output(dor_print_get_line());
	}

	dor_print_indent_up();

	dor_object_print(element->object);

	dor_print_indent_down();

	++elementno;

	element = element->next;

	if (doe_group_traversal_state) {
	    break;
	}
    }

    if (doe_group_traversal_state == dod_group_traversal_return) {
	doe_group_traversal_state = dod_group_traversal_continue;
    }
}
/*
 ======================================================================
 */

void dor_group_check_network (dot_object *group_object)
{
    dot_group *group = (dot_group *)group_object->data;
    dot_group_element *element;
    dot_object *check_group;

    /*
     * Search group stack for existence of the current group on
     * the stack.  If it is on the stack then there is a 
     * circularity.
     */

    doe_group_network_state = DcGroupOk;

    for (check_group = dor_list_get_first_data(doe_group_stack);
	 check_group != (dot_object *)0;
	 check_group = dor_list_get_next_data(doe_group_stack)) {
	if (check_group == group_object) {
	    doe_group_network_state = DcGroupBad;
	    return;
	}
    }

    /*
     * This group is not on the stack so push it on the stack
     * and execute the method DcMethodCheckGroup on each element
     * of the group
     */

    dor_group_push(group_object, DcFalse);
    doe_group_traversal_state = dod_group_traversal_continue;

    for (element = group->first_element; 
	 element != (dot_group_element *)0; 
	 element = element->next) {
	dor_object_get_method(element->object,DcMethodCheckGroup)
		(element->object);

	if (doe_group_network_state != DcGroupOk) 
		break;

	if (doe_group_traversal_state != dod_group_traversal_continue)
		break;

    }

    dor_group_pop(DcFalse);

    if (doe_group_traversal_state == dod_group_traversal_return) {
	doe_group_traversal_state = dod_group_traversal_continue;
    }
    return;
}
