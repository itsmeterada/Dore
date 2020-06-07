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


DtObject dog_ (open)
    int *open;
{
    return dor_group_create ((DtFlag)*open, DcFalse);
}

/* ===================================================================== */

DtObject doilg_ (open)
    int *open;
{
    return dor_group_create ((DtFlag)*open, DcTrue);
}

/* ===================================================================== */

void dgo_ (group, append)
    DtObject *group;
    int *append;
{
    dor_group_open (*group, (DtFlag)*append);
}

/* ===================================================================== */

DtObject dgcs_ ()
{
    return dor_group_close();
}

/* ===================================================================== */

void dgao_ (object)
    DtObject *object;
{
    dor_group_add_object (*object);
}

/* ===================================================================== */

void dgaog_ (group, object)
    DtObject *group;
    DtObject *object;
{
    dor_group_add_object_into (*group,*object);
}

/* ===================================================================== */

DtFlag dgdel_ (label_1, label_2)
    DtInt *label_1;
    DtInt *label_2;
{
    return dor_group_delete_between_labels (*label_1, *label_2);
}

/* ===================================================================== */

void dgde_ (count)
    DtInt *count;
{
    dor_group_delete_elements (*count);
}

/* ===================================================================== */

void dgder_ (from, to)
    DtInt *from;
    DtInt *to;
{
    dor_group_delete_element_range (*from, *to);
}

/* ===================================================================== */

void dgro_ (object)
    DtObject *object;
{
    dor_group_replace (*object);
}

/* ===================================================================== */

void dgrog_ (group, object)
    DtObject *group;
    DtObject *object;
{
    dor_group_replace_in (*group, *object);
}

/* ===================================================================== */

void dge_ (group)
    DtObject *group;
{
    dor_group_empty (*group);
}

/* ===================================================================== */

DtGroupNetworkStatus dgck_ (group)
    DtObject *group;
{
    return dor_group_check (*group);
}

/* ===================================================================== */

DtObject dgqo_ ()
{
    return dor_group_inq_open();
}

/* ===================================================================== */

DtInt dgqs_ (group)
    DtObject *group;
{
    return dor_group_inq_size(*group);
}

/* ===================================================================== */

DtInt dgqep_ ()
{
    return(dor_group_inq_element_pointer());
}

/* ===================================================================== */

DtObject dgqop_ (group, offset, type)
    DtObject *group;
    DtInt *offset;
    int *type;
{
    return dor_group_inquire_object_at_position
	       (*group, *offset, (DtRelPosition)*type);
}

/* ===================================================================== */

void dgsep_ (element_ptr, type)
    DtInt *element_ptr;
    int *type;
{
    dor_group_set_element_pointer(*element_ptr, (DtRelPosition)*type);
}

/* ===================================================================== */

DtFlag dgsepl_ (label, offset)
    DtInt *label;
    DtInt *offset;
{
    return dor_group_set_ele_ptr_rel_label (*label, *offset);
}
