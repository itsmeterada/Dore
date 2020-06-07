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

DtObject dors_ (width, height, depth, type, typestring, tlen, data, delcallback)
    DtInt *width;
    DtInt *height;
    DtInt *depth;
    int *type;
    char *typestring;
    DtInt *tlen;
    DtPtr data;
    DtObject *delcallback;
{
    static int   ntypestr;
    static char *stypestr;

    ntypestr = fstr(&stypestr,ntypestr, typestring, *tlen);

    return(dor_rstr_create(*width, *height, *depth, (DtRasterType)*type, 
				    stypestr, data, *delcallback));
}
