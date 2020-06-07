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
	int dor_group_push (group_object, att_flag)
	int dor_group_pop (att_flag)
	dot_object * dor_group_get_current_group ()
	int dor_group_record_attribute (group_object,attpopgrouproutine)
	dot_object_reference * dor_group_inq_references (group_object)
	int dor_group_traversal_abort ()
	int dor_group_traversal_return ()

  ======================================================================
 */
#include <dore/dore_develop/private/group.h>
#include <dore/internal/dogen.h>
#include <dore/internal/list.h>

/*
 ======================================================================
 */

void dor_group_push (dot_object *group_object, DtFlag att_flag)
{
#   ifdef debug
	printf("push group called\n");
#   endif

    dor_stack_push (doe_group_stack, group_object);
    dor_stack_push (doe_group_att_flag_stack, (DtPtr) att_flag);
}
/*
 ======================================================================
 */

void dor_group_pop (DtFlag att_flag)
{
    dot_object *group_object;
    dot_group *group;
    struct list *globalattributestack;
    DtPFI attpopgrouproutine;
    char *curr_att_flag;
    char *prev_att_flag;

#ifdef debug
    printf("pop group called\n");
#endif
	
    /*  Find the current group,and use it to look up all of the 
	global attributes which must be automatically popped (they 
	may have left some values on their global attribute stacks). 
	Note that we leave the current group set to the one being 
	popped,so that the gas mechanism can know what the current 
	group is, so that it can compare this value to the group 
	value stored with the data items on the global attribute
	stacks.  */

    dor_stack_get_top_element (doe_group_stack, (DtPtr*)(&group_object));
    dor_stack_get_top_element (doe_group_att_flag_stack,
			       (DtPtr*)(&curr_att_flag));

    if (att_flag && !curr_att_flag) {
	/*
	 * error: trying to pop attributes without corresponding push
	 */
	DDerror (ERR_CANT_POP_STACK, "DoPopAtts", 
		    "NO corresponding DoPushAtts");
    } else {
	do {
	    group = (dot_group *)(group_object->data);
	    globalattributestack = group->attribute_stack;
#ifdef debug
	    printf ("   popping: att_flag: %d, current: %d\n",
		    att_flag, curr_att_flag);
#endif
	    while (dor_stack_pop (globalattributestack,
				  (DtPtr*)(&attpopgrouproutine))) {
		(*attpopgrouproutine)();
	    }

	    dor_stack_pop (doe_group_stack, (DtPtr*)(&group_object));
	    dor_stack_pop (doe_group_att_flag_stack, (DtPtr*)(&prev_att_flag));

	    dor_stack_get_top_element (doe_group_stack,
				        (DtPtr*)(&group_object));
	    dor_stack_get_top_element (doe_group_att_flag_stack,
					(DtPtr*)(&curr_att_flag));
	} while (!att_flag && prev_att_flag);
	/*
	 * when popping a group (!att_flag) remove all extra 
	 * groups that correspond to DoPushAtts
	 */
    }
}
/*
 ======================================================================
 */

dot_object *dor_group_get_current_group (void)
{
    dot_object *group_object;

    if (!dor_stack_get_top_element (doe_group_stack, (DtPtr*)(&group_object))){
	/*
	 * No current group on scene group stack
	 */
	return((struct object *)0);
    }

    return(group_object);
}
/*
 ======================================================================
 */

void dor_group_record_attribute (
    dot_object *group_object,
    DtPFV       attpopgrouproutine)
{
    struct list *attributeobjectstack;

    attributeobjectstack = ((struct group*)group_object->data)->attribute_stack;
	
    dor_stack_push (attributeobjectstack, (void *)attpopgrouproutine);
}
/*
 ======================================================================
 */

dot_object_reference *dor_group_inq_references (dot_object *group_object)
{
    dot_group *group = (dot_group *)group_object->data;

    return(group->references);
}
/*
 ======================================================================
 */

void dor_group_traversal_abort (void)
{
    doe_group_traversal_state = dod_group_traversal_abort;
}
/*
 ======================================================================
 */

void dor_group_traversal_return (void)
{
    doe_group_traversal_state = dod_group_traversal_return;
}
