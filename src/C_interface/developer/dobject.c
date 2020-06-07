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

void DDobject_AddReference (
    DtObject object,
    DtObject referencing_object)
{
    dor_object_add_reference (object, referencing_object);
}



/* ====================================================================== */

void DDobject_DeleteReference (
    DtObject object,
    DtObject referencing_object)
{
    dor_object_delete_reference (object, referencing_object);
}



/* ====================================================================== */

DtMethodPtr DDobject_InqCurrentMethod (DtObject object)
{
    return dor_object_get_current_method(object);
}



/* ====================================================================== */

void DDobject_Delete (DtObject object)
{
    dor_object_check_deletion (object);
}



/* ====================================================================== */

DtMethodPtr DDobject_InqDefaultMethod (DtObject object)
{
    return dor_object_inq_default_method (object);
}



/* ====================================================================== */

DtMethodPtr DDobject_InqMethod (
    DtObject object,
    DtInt    method_id)
{
    return dor_object_get_method (object, method_id);
}
