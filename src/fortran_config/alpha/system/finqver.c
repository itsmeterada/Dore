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
#include <dore/internal/system.h>

void dsqver_ (version, length, fstrlen)
    DtPtr *version;
    DtInt *length;
    int fstrlen;
{
    int i;
    char *v;

    dor_system_inq_version ((DtPtr*)(&v));

    if (fstrlen < (i = strlen(v))) {
        strncpy ((char*)(version), v, fstrlen);
        *length = fstrlen;
        return;
    }
    strncpy ((char*)(version), v, i);
    *length = i;
    v = (char *)version;
    for (;i<fstrlen;i++) {
        v[i] = ' ';
    }
}
