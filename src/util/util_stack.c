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
	dor_stack_create
	dor_stack_destroy
	dor_stack_push
	dor_stack_pop
	dor_stack_get_top_element
	dor_stack_set_top_element

  ====================================================================== */

#include <dore/internal/dogen.h>
#include <dore/internal/list.h>

/* ====================================================================== */

dot_list *dor_stack_create (void)
{
    return dor_list_create();
}



/* ====================================================================== */

void dor_stack_destroy (dot_list *stack)
{
    struct list_node *topelement;

    while (stack->list_nodemax > 0) {
	topelement = stack->list_header_node->list_node_previous;

	dor_list_remove_node(stack,topelement);

	dor_space_deallocate(topelement);
    }

    dor_space_deallocate (stack->list_header_node);
    dor_space_deallocate (stack);
}



/* ====================================================================== */

void dor_stack_push (dot_list *stack, DtPtr value)
{
    dor_list_add_data_last (stack,value);
}



/* ====================================================================== */

DtFlag dor_stack_pop (dot_list *stack, DtPtr *value)
{
    struct list_node *node;

    if (stack->list_nodemax < 1){
	return(DcFalse);
    }

    *value = dor_list_get_last_data(stack);

    node = dor_list_get_current_node(stack);

    dor_list_remove_node(stack,node);

    dor_space_deallocate(node);

    return(DcTrue);
}



/* ====================================================================== */

DtFlag dor_stack_get_top_element (dot_list *stack, DtPtr *value)
{
    if (stack->list_nodemax < 1){
	return(DcFalse);
    }

    *value = dor_list_get_last_data(stack);

    return(DcTrue);
}



/* ====================================================================== */

void dor_stack_set_top_element (dot_list *stack, DtPtr value)
{
    dor_list_replace_last_data (stack,value);
}
