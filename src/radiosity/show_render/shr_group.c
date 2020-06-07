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
 
#include "method.h"
#include "dore/dore_develop/private/group.h"

/*
 ======================================================================
 Functions:
   void dpr_radiosity_group_execute_current_show_render(group_object)

 ======================================================================
*/

extern DtInt doe_group_class_id;

/* This is a modified copy of the kernel function
   dor_group_execute_current. This routine executes every object of
   the group with their show_render method routine.
 */

void dpr_radiosity_group_execute_current_show_render (
    dot_object *group_object)
{
    dot_group *group = (dot_group *)group_object->data;
    dot_group_element *element;
    DtInt type = dor_object_inq_type(group_object);
    DtMethodPtr routine;

    if (! dor_condex_query_execute(type))
            return;

    if (type == doe_group_class_id) {
            dor_group_push(group_object, DcFalse);
    }

    doe_group_traversal_state = dod_group_traversal_continue;

    for (element = group->first_element;
         element != (dot_group_element *)0;
         element = element->next) {

        /* what we execute is the routine for the radiosity 
           show_render method */

        routine = DDobject_InqMethod(element->object,
                    dpe_rad_show_data.show_render_method_id);

        (*routine)(element->object);

        if (doe_group_traversal_state)
                break;
    }

    if (type == doe_group_class_id) {
        /* if this group was executed from a callback, current method
           is show_render. make sure global attributes of the base
           method are used for popping */
        dpr_radiosity_set_current_method_base();

        dor_group_pop(DcFalse);

        /* reset current method for next object to be executed */
        dpr_radiosity_set_current_method_show_render();
    }

    if (doe_group_traversal_state == dod_group_traversal_return) {
        doe_group_traversal_state = dod_group_traversal_continue;
    }

}

/*
 ======================================================================
 */
