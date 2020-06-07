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
 
#include "dore/internal/dogen.h"

/* ====================================================================== */

DtObject DoGroup (DtFlag open)
{
    return dor_group_create (open, DcFalse);
}

/* ====================================================================== */

DtObject DoInLineGroup (DtFlag open)
{
    return(dor_group_create(open,DcTrue));
}

/* ====================================================================== */

void DgOpen (DtObject group, DtFlag append)
{
    dor_group_open(group,append);
}

/* ====================================================================== */

DtObject DgClose (void)
{
    return(dor_group_close());
}

/* ====================================================================== */

void DgAddObj (DtObject object)
{
    dor_group_add_object(object);
}

/* ====================================================================== */

void DgAddObjToGroup (DtObject group, DtObject object)
{
    dor_group_add_object_into(group,object);
}

/* ====================================================================== */

void DgDelEle (DtInt count)
{
    dor_group_delete_elements(count);
}

/* ====================================================================== */

DtFlag DgDelEleBetweenLabels (DtInt label_1, DtInt label_2)
{
    return dor_group_delete_between_labels(label_1, label_2);
}

/* ====================================================================== */

void DgDelEleRange (DtInt from, DtInt to)
{
    dor_group_delete_element_range(from, to);
}

/* ====================================================================== */

void DgReplaceObj (DtObject object)
{
    dor_group_replace(object);
}

/* ====================================================================== */

void DgReplaceObjInGroup (DtObject group, DtObject object)
{
    dor_group_replace_in(group, object);
}

/* ====================================================================== */

void DgEmpty (DtObject group)
{
    dor_group_empty(group);
}

/* ====================================================================== */

DtGroupNetworkStatus DgCheck (DtObject group)
{
    return dor_group_check(group);
}

/* ====================================================================== */

DtObject DgInqOpen (void)
{
    return dor_group_inq_open();
}

/* ====================================================================== */

DtInt DgInqSize (DtObject group)
{
    return dor_group_inq_size(group);
}

/* ====================================================================== */

DtInt DgInqElePtr (void)
{
    return(dor_group_inq_element_pointer());
}

/* ====================================================================== */

DtObject DgInqObjAtPos (
    DtObject      group,
    DtInt         offset,
    DtRelPosition type)
{
    return dor_group_inquire_object_at_position(group, offset, type) ;
}

/* ====================================================================== */

void DgSetElePtr (DtInt element_ptr, DtRelPosition type)
{
    dor_group_set_element_pointer(element_ptr,type);
}

/* ====================================================================== */

DtFlag DgSetElePtrRelLabel (DtInt label, DtInt offset)
{
    return dor_group_set_ele_ptr_rel_label(label,offset);
}
