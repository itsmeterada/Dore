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

/* ======================================================================== */

DtObject DSQOI (object_name, object_type)
    DtInt *object_name;
    DtInt *object_type;
{
    return dor_object_inq_handle(DcNameInteger, *object_name, *object_type);
}

/* ======================================================================== */

DtObject DSQOS (object_name, n, object_type)
    DtPtr *object_name;
    DtInt *n;
    DtInt *object_type;
{
    static char *s;
    static int sn;

    sn = fstr(&s, sn, object_name, *n);
    return dor_object_inq_handle(DcNameString, s, *object_type);
}
