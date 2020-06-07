#ifndef DD_GROUP_H
#define DD_GROUP_H
 
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
 
#include <dore/internal/dogen.h>

#define dod_group_table_block_size 100

/* group attribute flags */
/* none yet */

/* group traversal state constants */
#define dod_group_traversal_continue	0
#define dod_group_traversal_return	1
#define dod_group_traversal_abort	2

typedef struct group_element {
    dot_object           *object;
    struct group_element *next;	/* next element in group */
} dot_group_element;

typedef struct group {
    Dt32Bits		flags;
    dot_group_element 	*first_element;
    dot_group_element 	*previous_element;
    dot_group_element 	*current_element;
    struct list 	*attribute_stack;
    struct group   	*previous_group;
    struct group   	*next_group;
    dot_object  	*object;
    dot_object_reference *references;
} dot_group;

typedef struct group_table_block {
    dot_group groups[dod_group_table_block_size];
    struct group_table_block *next;
} dot_group_table_block;

/* points to group currently open for editing, or NULL */
extern dot_object *doe_group_current_group;

/* table of all defined groups */
extern dot_group *doe_group_defined_groups;

/* list of free group slots */
extern dot_group *doe_group_next_free_group;

/* list of blocks in group table */
extern dot_group_table_block *doe_group_table_blocks;

/* group traversal stack */
extern struct list *doe_group_stack;
extern struct list *doe_group_att_flag_stack;

/* group traversal state */ 
extern DtInt doe_group_traversal_state;

/* group network state for DcMethodCheckGroup */
extern DtGroupNetworkStatus doe_group_network_state;

/* open group stack */
extern struct list *doe_group_edit_stack;

/* class ids */
extern DtInt doe_group_class_id;
extern DtInt doe_group_inline_class_id;

#endif
