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
#include <dore/dore_develop/private/nurbsurf.h>

DtObject donrbs_ (colormodel, ctrlpointtype, uv_area, order_u, n_knot_u,
		  knot_u, order_v, n_knot_v, knot_v, n_ctrl_u, n_ctrl_v,
		  ctrl_vertices)
    DtColorModel *colormodel;
    DtCtrlPointType *ctrlpointtype;
    DtNArea uv_area;
    DtInt *order_u;
    DtInt *n_knot_u;
    DtReal knot_u[];
    DtInt *order_v;
    DtInt *n_knot_v;
    DtReal knot_v[];
    DtInt *n_ctrl_u;
    DtInt *n_ctrl_v;
    DtReal ctrl_vertices[];
{
    return dor_nurbsurf_create
	   (   *colormodel, *ctrlpointtype, uv_area, *order_u, *n_knot_u,
	       knot_u, *order_v, *n_knot_v, knot_v, *n_ctrl_u, *n_ctrl_v,
	       ctrl_vertices
	   );
}
