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


/*****************************************************************************
**  Delete the current object name.
*****************************************************************************/

void dssond_ (object)
    DtObject *object;
{
    dor_object_set_name_string (*object, DcNullPtr, DcTrue);
}



/*****************************************************************************
**  Set the object name to the given integer value.
*****************************************************************************/

void dssoni_ (object, object_name, replace)
    DtObject *object;
    DtInt     object_name;
    int      *replace;
{
    dor_object_set_name_integer (*object, object_name, (DtFlag)*replace);
}



/*****************************************************************************
**  Set the object name to the given integer value.
*****************************************************************************/

void dssons_ (object, object_name, n, replace, fstrlen)
    DtObject *object;
    DtPtr     object_name;
    DtInt    *n;
    int      *replace;
    int       fstrlen;
{
    static int   n1;
    static char *s1;

    n1 = fstr (&s1, n1, object_name, *n);

    dor_object_set_name_string (*object, s1, (DtFlag) *replace);
}
