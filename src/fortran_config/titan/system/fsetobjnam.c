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

void DSSOND (object)
    DtObject *object;
{
    dor_object_set_name (*object, DcNameNone, "", DcTrue);
}

void DSSONI (object, object_name, replace)
    DtObject *object;
    DtInt object_name;
    /* DtFlag */int *replace;
{
    dor_object_set_name
	(*object, DcNameInteger, object_name, (DtFlag)*replace);
}

void DSSONS (object, object_name, n, replace)
    DtObject *object;
    DtPtr object_name;
    DtInt *n;
    /* DtFlag */int *replace;
{
    static int n1;
    static char *s1;

    n1 = fstr (&s1, n1, object_name, *n);
    dor_object_set_name (*object, DcNameString, s1, (DtFlag)*replace);
}
