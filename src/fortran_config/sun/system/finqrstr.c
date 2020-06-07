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
#include <dore/internal/dogen.h>

void dsqr_ (raster, width, height, depth, type, typestring, tlen, data, fstrlen)
    DtObject *raster;
    DtInt *width;
    DtInt *height;
    DtInt *depth;
    /* DtRasterType */ DtInt *type;
    DtPtr *typestring;
    DtInt *tlen;
    DtPtr *data;
    int fstrlen;
{
    char *t;
    int i;

    dor_rstr_inq (*raster, width, height, depth, (DtRasterType*)type, &t, data);

    if (fstrlen < (i = strlen(t))) {
	*tlen = 0;
    } else {
	strncpy (typestring, t, i);
	*tlen = i;
    }
}

