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
 
#include <string.h>
#include "../fortran.h"


/*****************************************************************************
**  This routine enquires the object's name type.
*****************************************************************************/

DtNameType dsqont_ (object)
    DtObject *object;
{
    return dor_object_inq_name_type (*object);
}



/*****************************************************************************
**  Get the object's integer name.
*****************************************************************************/

DtInt dsqoni_ (object)
    DtObject *object;
{
    return dor_object_inq_name_integer (*object);
}



/*****************************************************************************
**  Get the object's string name.
*****************************************************************************/

void dsqons_ (object, fname, length, fstrlen)
    DtObject *object;
    char  **fname;
    DtInt *length;
    int fstrlen;
{
    register int   i;
    register char *name;

    name = dor_object_inq_name_string (*object);

    if (!name) {
	*length = 0;
	return;
    }

    if (fstrlen < (i = strlen(name))) {
	*length = 0;
	return;
    }

    strncpy (*fname, name, i);
    *length = i;
}
