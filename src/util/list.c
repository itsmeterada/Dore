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
	dor_list_create
	dor_list_create_node
	dor_list_destroy_current_data
	dor_list_print
	dor_list_add_data_before
	dor_list_add_data_first
	dor_list_add_data_last
	dor_list_add_node_before
	dor_list_add_node_first
	dor_list_add_node_last
	dor_list_delete_node
	dor_list_empty
	dor_list_get_current_data
	dor_list_get_current_node
	dor_list_get_first_data
	dor_list_get_first_node
	dor_list_get_list_maximum
	dor_list_get_last_data
	dor_list_get_node_data
	dor_list_get_next_data
	dor_list_get_next_node
	dor_list_get_previous_data
	dor_list_remove_current_data
	dor_list_remove_current_node
	dor_list_remove_data_node
	dor_list_remove_node
	dor_list_replace_current_data
	dor_list_replace_last_data

  ======================================================================
 */
#include "dore/internal/dogen.h"
#include "dore/internal/list.h"

#define dod_list_direction_forward      0
#define dod_list_direction_backward     1

/* ====================================================================== */

dot_list *dor_list_create (void)
{
    dot_list *newlist;
    dot_list_node *headernode;

    newlist = (dot_list *)dor_space_allocate(sizeof *newlist);

    headernode = dor_list_create_node (DcNullPtr);

    headernode->list_node_next = headernode;
    headernode->list_node_previous = headernode;

    newlist->list_header_node = headernode;
    newlist->list_nodemax = 0;
    newlist->list_current_node = headernode;

    return(newlist);
}
/*
 ======================================================================
 */

dot_list_node *dor_list_create_node (DtPtr data)
{
    dot_list_node *newnode;

    newnode = dor_space_allocate (sizeof *newnode);
    newnode->list_node_data = data;

    return newnode;
}
/*
 ======================================================================
 */

void dor_list_destroy (dot_list *list)
{
    dot_list_node *headernode;
    dot_list_node *node;
    dot_list_node *nextnode;

    headernode = list->list_header_node;
    node = headernode->list_node_next;

    while (node != headernode){
	nextnode = node->list_node_next;

	dor_space_deallocate(node);

	node = nextnode;
    }

    dor_space_deallocate(headernode);

    dor_space_deallocate(list);
}
/*
 ======================================================================
 */

DtPtr dor_list_destroy_current_data (dot_list *list)
{
    dot_list_node *currentnode;
    DtPtr currentdata;

    currentnode = dor_list_remove_current_node(list);

    if (!currentnode) {
	DDerror (ERR_INVALID_OPERATION,
		    "dor_list_destroy_current_data", 
		    "attempt to delete header node");
	return(NULL);
    }

    currentdata = currentnode->list_node_data;

    dor_space_deallocate((DtPtr)currentnode);

    return(currentdata);
}
/*
 ======================================================================
 */

void dor_list_print (dot_list *list)
{
    dot_list_node *listnode;

    sprintf(dor_print_get_line(),"list [%lx]:",list);
    dor_print_output(dor_print_get_line());

    dor_print_indent_up();

    sprintf(dor_print_get_line(),
	    "number of list nodes = %d",list->list_nodemax);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "current node = %lx",list->list_current_node);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "header node = %lx",list->list_header_node);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"-> next = %lx",
	    list->list_header_node->list_node_next);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"-> prev = %lx",
	    list->list_header_node->list_node_previous);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"-> data = %lx",
	    list->list_header_node->list_node_data);
    dor_print_output(dor_print_get_line());

    listnode = list->list_header_node->list_node_next;

    while (listnode != list->list_header_node){
	sprintf(dor_print_get_line(),"list node [%lx]:",listnode);
	dor_print_output(dor_print_get_line());

	dor_print_indent_up();

	sprintf(dor_print_get_line(),
		"-> next = %lx",listnode->list_node_next);
	dor_print_output(dor_print_get_line());

	sprintf(dor_print_get_line(),"-> prev = %lx",
		listnode->list_node_previous);
	dor_print_output(dor_print_get_line());

	sprintf(dor_print_get_line(),"-> data = %lx",
		listnode->list_node_data);
	dor_print_output(dor_print_get_line());

	dor_print_indent_down();
	dor_print_output("`list node end'");

	listnode = listnode->list_node_next;
    }

    dor_print_indent_down();

    dor_print_output("`list end'");
}
/*
 ======================================================================
 */

dot_list_node *dor_list_add_data_before (
    dot_list      *list,
    dot_list_node *fixnode,
    DtPtr          data)
{
    dot_list_node *newnode;

    newnode = dor_list_create_node(data);

    dor_list_add_node_before(list,fixnode,newnode);

    return newnode;
}
/*
 ======================================================================
 */

dot_list_node *dor_list_add_data_first (dot_list *list, DtPtr data)
{
    dot_list_node *newnode;

    newnode = dor_list_create_node(data);

    dor_list_add_node_first(list,newnode);

    return(newnode);
}
/*
 ======================================================================
 */

dot_list_node *dor_list_add_data_last (dot_list *list, DtPtr data)
{
    dot_list_node *newnode;

    newnode = dor_list_create_node(data);

    dor_list_add_node_last(list,newnode);

    return(newnode);
}
/*
 ======================================================================
 */

void dor_list_add_node_before (
    dot_list      *list,
    dot_list_node *fixnode,
    dot_list_node *newnode)
{
    dot_list_node *prevnode;

    prevnode = fixnode->list_node_previous;
    prevnode->list_node_next = newnode;

    newnode->list_node_previous = prevnode;
    newnode->list_node_next = fixnode;
    fixnode->list_node_previous = newnode;

    list->list_nodemax++;
}
/*
 ======================================================================
 */

void dor_list_add_node_first (
    dot_list      *list,
    dot_list_node *node)
{
    dot_list_node *headernode;
    dot_list_node *firstnode;

    headernode = list->list_header_node;

    firstnode = headernode->list_node_next;
    firstnode->list_node_previous = node;

    node->list_node_next = firstnode;
    node->list_node_previous = headernode;

    headernode->list_node_next = node;

    list->list_nodemax++;
}
/*
 ======================================================================
 */

void dor_list_add_node_last (
    dot_list      *list,
    dot_list_node *node)
{
    dot_list_node *headernode;
    dot_list_node *lastnode;

    headernode = list->list_header_node;

    lastnode = headernode->list_node_previous;
    lastnode->list_node_next = node;

    node->list_node_previous = lastnode;
    node->list_node_next = headernode;

    headernode->list_node_previous = node;

    list->list_nodemax++;
}
/*
 ======================================================================
 */

void dor_list_delete_node (
    dot_list      *list,
    dot_list_node *node)
{
    if (node == list->list_current_node){
	if (list->list_direction == dod_list_direction_forward)
	    list->list_current_node
		= list->list_current_node->list_node_previous;
	else
	    list->list_current_node = list->list_current_node->list_node_next;
    }

    dor_list_remove_node (list,node);
}
/*
 ======================================================================
 */

void dor_list_empty (dot_list *list)
{
    dot_list_node *headernode;
    dot_list_node *node;
    dot_list_node *nextnode;

    headernode = list->list_header_node;

    node = headernode->list_node_next;

    while(node != headernode){

	if (node->list_node_data)
	    dor_space_deallocate((DtPtr)(node->list_node_data));

	nextnode = node->list_node_next;

	dor_list_remove_node(list,node);

	dor_space_deallocate((DtPtr)node);

	node = nextnode;
    }
}
/*
 ======================================================================
 */

DtPtr dor_list_get_current_data (dot_list *list)
{
    dot_list_node *headernode;

    headernode = list->list_header_node;

    if (list->list_current_node == headernode)
	return DcNullPtr;
    else
	return list->list_current_node->list_node_data;
}
/*
 ======================================================================
 */

dot_list_node *dor_list_get_current_node (dot_list *list)
{
    if (list->list_current_node == list->list_header_node)
	return DcNullPtr;
    else
	return list->list_current_node;
}
/*
 ======================================================================
 */

DtPtr dor_list_get_first_data (dot_list *list)
{
    dot_list_node *headernode;
    dot_list_node *firstnode;

    list->list_direction = dod_list_direction_forward;

    headernode = list->list_header_node;

    firstnode = headernode->list_node_next;

    if (firstnode == headernode){
	list->list_current_node = headernode;

	return DcNullPtr;
    } else {
	list->list_current_node = firstnode;

	return(firstnode->list_node_data);
    }
}
/*
 ======================================================================
 */

dot_list_node *dor_list_get_first_node (dot_list *list)
{
    dot_list_node *headernode;

    headernode = list->list_header_node;

    return(headernode->list_node_next);
}
/*
 ======================================================================
 */

int dor_list_get_list_maximum (dot_list *list)
{
    return list->list_nodemax;
}
/*
 ======================================================================
 */

DtPtr dor_list_get_last_data (dot_list *list)
{
    dot_list_node *headernode;
    dot_list_node *lastnode;

    list->list_direction = dod_list_direction_backward;

    headernode = list->list_header_node;

    lastnode = headernode->list_node_previous;

    if (lastnode == headernode){
	list->list_current_node = headernode;

	return DcNullPtr;
    } else {
	list->list_current_node = lastnode;

	return(lastnode->list_node_data);
    }
}
/*
 ======================================================================
 */

DtPtr dor_list_get_node_data (dot_list *list, dot_list_node *node)
{
    /*  We have to check if this node is the list's header node, because if it
    **  is, then it has no data structure associated with it.  Unfortunately,
    **  even though the data structure pointer of the list header is initially
    **  set to null at list creation time, it is not maintained properly by
    **  the node removal functions, so we have to do this check, rather than
    **  just returning the value of the list node data structure.  */

    if (node == list->list_header_node)
	return DcNullPtr;
    else
	return(node->list_node_data);
}
/*
 ======================================================================
 */

DtPtr dor_list_get_next_data (dot_list *list)
{
    dot_list_node *nextnode;

    nextnode = list->list_current_node->list_node_next;

    if (nextnode == list->list_header_node){
	return DcNullPtr;

    } else {
	list->list_current_node = nextnode;

	return(nextnode->list_node_data);
    }
}
/*
 ======================================================================
 */

dot_list_node *dor_list_get_next_node (dot_list_node *node)
{
    return node->list_node_next;
}
/*
 ======================================================================
 */

DtPtr dor_list_get_previous_data (dot_list *list)
{
    dot_list_node *prevnode;

    prevnode = list->list_current_node->list_node_previous;

    if (prevnode == list->list_header_node){
	return DcNullPtr;
    } else {
	list->list_current_node = prevnode;

	return(prevnode->list_node_data);
    }
}
/*
 ======================================================================
 */

DtPtr dor_list_remove_current_data (dot_list *list)
{
    dot_list_node *currentnode;
    DtPtr currentdata;

    currentnode = dor_list_remove_current_node(list);

    if (!currentnode) {
	DDerror (ERR_INVALID_OPERATION, "dor_list_data_remove_current", 
		 "attempt to delete header node");
	return DcNullPtr;
    }

    currentdata = currentnode->list_node_data;

    return(currentdata);
}
/*
 ======================================================================
 */

dot_list_node *dor_list_remove_current_node (dot_list *list)
{
    dot_list_node *currentnode;

    currentnode = dor_list_get_current_node(list);

    if (!currentnode) {
	DDerror (ERR_INVALID_OPERATION, "dor_list_remove_current_node", 
		 "attempt to remove header node");
	return DcNullPtr;
    }

    if (list->list_direction == dod_list_direction_forward)
	list->list_current_node = list->list_current_node->list_node_previous;
    else
	list->list_current_node = list->list_current_node->list_node_next;

    dor_list_remove_node (list, currentnode);

    return currentnode;
}
/*
 ======================================================================
 */

DtPtr dor_list_remove_data_node (dot_list *list, dot_list_node *node)
{
    DtPtr data;

    if (node == list->list_header_node){
	DDerror (ERR_INVALID_OPERATION,
		    "dor_list_data_remove_node", 
		    "attempt to delete header node");
	return(NULL);
    }

    dor_list_delete_node(list,node);

    data = node->list_node_data;

    return(data);
}
/*
 ======================================================================
 */

void dor_list_remove_node (dot_list *list, dot_list_node *node)
{
    dot_list_node *prevnode, *nextnode;

    prevnode = node->list_node_previous;

    nextnode = node->list_node_next;

    prevnode->list_node_next = nextnode;

    nextnode->list_node_previous = prevnode;

    list->list_nodemax = list->list_nodemax - 1;
}
/*
 ======================================================================
 */

void dor_list_replace_current_data (dot_list *list, DtPtr data)
{
    if (list->list_current_node == list->list_header_node){
	DDerror (ERR_INVALID_OPERATION, "dor_list_replace_current_data", 
		"attempt to replace data of the header node");
	return;
    }

    list->list_current_node->list_node_data = data;
}
/*
 ======================================================================
 */

void dor_list_replace_last_data (dot_list *list, DtPtr value)
{
    dot_list_node *headernode;
    dot_list_node *lastnode;

    headernode = list->list_header_node;

    lastnode = headernode->list_node_previous;

    if (lastnode == headernode){
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_list_replace_last_data",
		    "list contains no elements");
	return;
    }

    lastnode->list_node_data = value;
}
