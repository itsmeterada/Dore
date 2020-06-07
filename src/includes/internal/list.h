#ifndef DORE_LIST_H
#define DORE_LIST_H

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


    /***  Type Definitions  ***/

typedef struct list      dot_list;
typedef struct list_node dot_list_node;

struct list_node {
    struct list_node *list_node_next;
    struct list_node *list_node_previous;
    DtPtr             list_node_data;
};

struct list {
    struct list_node *list_header_node;
    DtInt             list_nodemax;
    short             list_direction;
    struct list_node *list_current_node;
};


    /***  Function Prototypes  ***/

	/* List Functions */

dot_list_node *dor_list_add_data_before      (dot_list*, dot_list_node*, DtPtr);
dot_list_node *dor_list_add_data_first       (dot_list*, DtPtr);
dot_list_node *dor_list_add_data_last        (dot_list*, DtPtr);
void           dor_list_add_node_before      (dot_list*, dot_list_node*,
					      dot_list_node*);
void           dor_list_add_node_first       (dot_list*, dot_list_node*);
void           dor_list_add_node_last        (dot_list*, dot_list_node*);
dot_list      *dor_list_create               (void);
dot_list_node *dor_list_create_node          (DtPtr);
void           dor_list_delete_node          (dot_list*, dot_list_node*);
void           dor_list_destroy              (dot_list*);
DtPtr          dor_list_destroy_current_data (dot_list*);
void           dor_list_empty                (dot_list*);
DtPtr          dor_list_get_current_data     (dot_list*);
dot_list_node *dor_list_get_current_node     (dot_list*);
DtPtr          dor_list_get_first_data       (dot_list*);
dot_list_node *dor_list_get_first_node       (dot_list*);
DtPtr          dor_list_get_last_data        (dot_list*);
int            dor_list_get_list_maximum     (dot_list*list);
DtPtr          dor_list_get_next_data        (dot_list*);
dot_list_node *dor_list_get_next_node        (dot_list_node*);
DtPtr          dor_list_get_node_data        (dot_list*, dot_list_node *);
DtPtr          dor_list_get_previous_data    (dot_list*);
void           dor_list_print                (dot_list*);
DtPtr          dor_list_remove_current_data  (dot_list*);
dot_list_node *dor_list_remove_current_node  (dot_list*);
DtPtr          dor_list_remove_data_node     (dot_list*, dot_list_node*);
void           dor_list_remove_node          (dot_list*, dot_list_node*);
void           dor_list_replace_current_data (dot_list*, DtPtr);
void           dor_list_replace_last_data    (dot_list*, DtPtr);

	/* Stack Functions */

dot_list *dor_stack_create          (void);
void      dor_stack_destroy         (dot_list *stack);
void      dor_stack_push            (dot_list *stack, DtPtr value);
DtFlag    dor_stack_pop             (dot_list *stack, DtPtr *value);
DtFlag    dor_stack_get_top_element (dot_list *stack, DtPtr *value);
void      dor_stack_set_top_element (dot_list *stack, DtPtr value);

#endif
