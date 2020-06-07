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

DtInt dsfrsr_(filename, flen, width, height, depth, type, data)
    char *filename;
    DtInt *flen;
    DtInt *width;
    DtInt *height;
    DtInt *depth;
    /*DtRasterType*/ int *type;
    DtPtr *data;
{
    static char *sfilename;
    static DtInt nfilename;

    nfilename = fstr (&sfilename, nfilename, filename, flen);
    
    return dor_system_filerstr_read
	   (sfilename, width, height, depth, (DtRasterType*)(type), data);
}

