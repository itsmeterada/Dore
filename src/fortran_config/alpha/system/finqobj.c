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
 
#include "../fortran.h"



/*****************************************************************************
**  This routine queries the object via integer name.
*****************************************************************************/

DtObject dsqoi_ (object_name, object_type)
    DtInt *object_name;
    DtInt *object_type;
{
    return dor_object_inq_via_integer (*object_name, *object_type);
}



/*****************************************************************************
**  This routine queries the object via string name.
*****************************************************************************/

DtObject dsqos_ (object_name, n, object_type, fstrlen)
    char *object_name;
    DtInt *n;
    DtInt *object_type;
    int fstrlen;
{
    static char *string;
    static int   sn;

    sn = fstr (&string, sn, object_name, *n);

    return dor_object_inq_via_string (string, *object_type);
}
