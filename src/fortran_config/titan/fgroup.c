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

/* ====================================================================== */

DtObject DOG (open)
    int *open;
{
    return(dor_group_create((DtFlag)*open,DcFalse));
}

/* ====================================================================== */

DtObject DOILG (open)
    int *open;
{
    return(dor_group_create((DtFlag)*open,DcTrue));
}

/* ====================================================================== */

void DGO (group, append)
    DtObject *group;
    int *append;
{
    dor_group_open(*group, (DtFlag)*append);
}

/* ====================================================================== */

DtObject DGCS ()
{
    return(dor_group_close());
}

/* ====================================================================== */

void DGAO (object)
    DtObject *object;
{
    dor_group_add_object(*object);
}

/* ====================================================================== */

void DGAOG (group, object)
    DtObject *group;
    DtObject *object;
{
    dor_group_add_object_into(*group,*object);
}

/* ====================================================================== */

DtFlag DGDEL (label_1, label_2)
    DtInt *label_1;
    DtInt *label_2;
{
    return dor_group_delete_between_labels(*label_1, *label_2);
}

/* ====================================================================== */

void DGDE (count)
    DtInt *count;
{
    dor_group_delete_elements(*count);
}

/* ====================================================================== */

void DGDER (from, to)
    DtInt *from;
    DtInt *to;
{
    dor_group_delete_element_range(*from, *to);
}

/* ====================================================================== */

void DGRO (object)
    DtObject *object;
{
    dor_group_replace(*object);
}

/* ====================================================================== */

void DGROG (group, object)
    DtObject *group;
    DtObject *object;
{
    dor_group_replace_in(*group, *object);
}

/* ====================================================================== */

void DGE (group)
    DtObject *group;
{
    dor_group_empty(*group);
}

/* ====================================================================== */

DtGroupNetworkStatus DGCK (group)
    DtObject *group;
{
    return dor_group_check(*group);
}

/* ====================================================================== */

DtObject DGQO ()
{
    return dor_group_inq_open();
}

/* ====================================================================== */

DtInt DGQS (group)
    DtObject *group;
{
    return dor_group_inq_size(*group);
}

/* ====================================================================== */

DtInt DGQEP ()
{
    return(dor_group_inq_element_pointer());
}

/* ====================================================================== */

DtObject DGQOP (group, offset, type)
    DtObject *group;
    DtInt *offset;
    int *type;
{
    return dor_group_inquire_object_at_position(*group, *offset,
    					(DtRelPosition)*type) ;
}

/* ====================================================================== */

void DGSEP (element_ptr, type)
    DtInt *element_ptr;
    int *type;
{
    dor_group_set_element_pointer(*element_ptr, (DtRelPosition)*type);
}

/* ====================================================================== */

DtFlag DGSEPL (label, offset)
    DtInt *label;
    DtInt *offset;
{
    return dor_group_set_ele_ptr_rel_label(*label, *offset);
}
