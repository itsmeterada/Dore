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

DtObject DOSPM (colormodel, vertextype, vertexcount, vertices, polygoncount,
	        contours, vertexlist, shape, smoothflag)
    /* DtColorModel */int *colormodel;
    /* DtVertexType */int *vertextype;
    DtInt *vertexcount;
    DtReal vertices[];
    DtInt *polygoncount;
    DtInt contours[];
    DtInt vertexlist[];
    /* DtShapeType */int *shape;
    /* DtFlag */int *smoothflag;
{
    return(dor_simplymsh_create((DtColorModel)*colormodel,
			    (DtVertexType)*vertextype,
			    *vertexcount, vertices,
			    *polygoncount, contours, 
			    vertexlist, (DtShapeType)*shape, 
			    (DtFlag)*smoothflag));
}
