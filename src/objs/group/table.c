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
	int dor_group_table_add_block ()
	int dor_group_table_delete_group (group)
	int dor_group_table_initialize ()
	int dor_group_table_print ()

  ======================================================================
 */
#include <dore/dore_develop/private/group.h>

/*
 ======================================================================
 */

void dor_group_table_add_block (void) 
{
    dot_group_table_block *new_block;
    DtInt i;

    new_block = (dot_group_table_block *)dor_space_allocate(
							    sizeof(dot_group_table_block));

    if (new_block == (dot_group_table_block *)0){
	DDerror(ERR_CANT_ALLOC_MEM_SIZE,
		   "dor_group_table_add_block",
		   "sizeof(dot_group_table_block)");
	return;
    }

    for (i = 0; i < dod_group_table_block_size; i++){
	new_block->groups[i].next_group = new_block->groups+i+1;
    }

    new_block->groups[dod_group_table_block_size-1].next_group = DcNullPtr;

    new_block->next = doe_group_table_blocks;
    doe_group_table_blocks = new_block;
    doe_group_next_free_group = new_block->groups;
}
/*
 ======================================================================
 */

void dor_group_table_delete_group (dot_group *group)
{
    /* it is asumed that the caller has already deleted references
       to this group,the groups contents,and the name of the group,
       if any */

    dot_group *next,*previous;

    previous = group->previous_group;
    next = group->next_group;

    if (!previous){
	/* deleting first element on chain */
	doe_group_defined_groups = next;
    } else {
	/* deleting somewhere in middle or at end of chain */
	previous->next_group = next;
    }

    if (next)
	next->previous_group = previous;

    /* add group back onto free list */

    group->next_group = doe_group_next_free_group;
    doe_group_next_free_group = group;
}
/*
 ======================================================================
 */

void dor_group_table_initialize (void)
{
    doe_group_next_free_group = DcNullPtr;
    doe_group_table_blocks = (dot_group_table_block *)0;

    dor_group_table_add_block();
}
/*
 ======================================================================
 */

void dor_group_table_print (void)
{
    dot_group *group; 

    printf("Dore Group Table:\n");
    if (doe_group_defined_groups){
	for (group=doe_group_defined_groups;  group;  group=group->next_group){
	    printf ("group %#lx: ",group);
	    printf ("previous group %#lx,next group %#lx\n",
		    group->previous_group,group->next_group);
	    dor_group_print (group->object);
	}
    } else {
	printf("\n\tgroup table is empty\n");
    }
}
