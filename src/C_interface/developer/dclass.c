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
#include <dore/dore_develop/develop.h>

/* ====================================================================== */

DtInt DDclass_AddMethod (char *method_name)
{
    return dor_class_all_add_method (method_name, 0, DcNullPtr);
}

/* ====================================================================== */

DtInt DDclass_AddInitializedMethod (
    char        *method_name,
    DtMethodPtr  rtn)
{
    return dor_class_all_add_initialized_method(method_name, 0, DcNullPtr, rtn);
}

/* ====================================================================== */

void DDclass_AddNotify (DtPFV rtn)
{
    dor_class_add_notify (rtn);
}

/* ====================================================================== */

DtInt DDclass_AddObjectData (
    DtInt       class_id,
    DtMethodPtr crt_rtn,
    DtMethodPtr prt_rtn,
    DtMethodPtr dst_rtn)
{
    return dor_class_add_object_data (class_id, crt_rtn, prt_rtn, dst_rtn);
}

/* ====================================================================== */

DtInt DDclass_CopyMethod (
    DtInt  method_id,
    char  *method_name)
{
    return dor_class_all_copy_method(method_id, method_name, 0, DcNullPtr);
}

/* ====================================================================== */

DtMethodPtr DDclass_InqDefaultMethod (DtInt class_id)
{
    return dor_class_inq_default_method(class_id);
}

/* ====================================================================== */

DtMethodPtr DDclass_InqMethod (
    DtInt class_id,
    DtInt method_id)
{
    return dor_class_inq_method (class_id, method_id);
}

/* ====================================================================== */

DtInt DDclass_ReplaceMethod (
    char  *method_name,
    DtInt  method_id,
    DtInt  copy_method_id)
{
    return dor_class_all_replace_method (method_name, method_id, 0, DcNullPtr,
				       copy_method_id);
}

/* ====================================================================== */

void DDclass_SetMethod (
    DtInt       class_id,
    DtInt       method_id,
    DtMethodPtr rtn)
{
    dor_class_set_method (class_id, method_id, rtn);
}
