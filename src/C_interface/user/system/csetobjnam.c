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
#include <dore/internal/system.h>



/*****************************************************************************
**  This function is now obsolete and has been replaced with
**  DsSetObjNameInteger and DsSetObjNameString.
*****************************************************************************/

void DsSetObjName (
    DtObject   object,
    DtNameType name_type,
    DtPtr      object_name,
    DtFlag     replace)
{
    DDerror
    (   ERR_FUNCTION_CHANGED,
	"DsSetObjName",
	"DsSetObjNameInteger or DsSetObjNameString"
    );
}
