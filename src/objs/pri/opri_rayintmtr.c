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
 
/*
  ======================================================================
  Functions:
	dor_rayintmshtri_initialize
	dor_rayintmshtri_create
	dor_rayintmshtri_z_values

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/rayintmtr.h>

DtInt doe_rayintmshtri_class_id = -1;
DtFlag doe_rayintmshtri_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry rayintmshtri_methods[] = {
    { 0, (DtMethodPtr)(0) }
};

void dor_rayintmshtri_initialize (void)
{
    doe_rayintmshtri_class_id = dor_class_install (DcTypeRayintmshtri, 
			"Rayintmshtri", 0, rayintmshtri_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_rayintmshtri_create (
    DtRealTriple loc[3],
    DtRealTriple nrm[3])
{
    struct rayintmshtri *mshtri;
    dot_object *rayintmshtriobj;

    if (!doe_rayintmshtri_initialized) {
	dor_rayintmshtri_initialize();
	doe_rayintmshtri_initialized = DcTrue;
    }

    mshtri = (struct rayintmshtri *)dor_space_allocate(sizeof *mshtri);

    mshtri->x1 = loc[0][0];
    mshtri->y1 = loc[0][1];
    mshtri->z1 = loc[0][2];

    mshtri->x2 = loc[1][0];
    mshtri->y2 = loc[1][1];
    mshtri->z2 = loc[1][2];

    mshtri->x3 = loc[2][0];
    mshtri->y3 = loc[2][1];
    mshtri->z3 = loc[2][2];

    dor_math_plane_equation(loc[0][0],loc[0][1],loc[0][2],
			    loc[1][0],loc[1][1],loc[1][2],
			    loc[2][0],loc[2][1],loc[2][2],
			    &(mshtri->cosine_x),&(mshtri->cosine_y),
			    &(mshtri->cosine_z),&(mshtri->D));

    if ((dor_math_fabs(mshtri->cosine_z) >=
	 dor_math_fabs(mshtri->cosine_x)) &&
	(dor_math_fabs(mshtri->cosine_z) >=
	 dor_math_fabs(mshtri->cosine_y))){
	mshtri->projection_type = dod_rayintmshtri_projection_type_z;
    }
    else if ((dor_math_fabs(mshtri->cosine_y) >=
	      dor_math_fabs(mshtri->cosine_x)) &&
	     (dor_math_fabs(mshtri->cosine_y) >=
	      dor_math_fabs(mshtri->cosine_z))){
	mshtri->projection_type = dod_rayintmshtri_projection_type_y;
    }
    else{
	mshtri->projection_type = dod_rayintmshtri_projection_type_x;
    }

    switch(mshtri->projection_type){
    case dod_rayintmshtri_projection_type_x:
	dor_rayintmshtri_z_values(loc[0][1],loc[0][2],nrm[0][0],
				  loc[1][1],loc[1][2],nrm[1][0],
				  loc[2][1],loc[2][2],nrm[2][0],
				  &(mshtri->initialnormx),
				  &(mshtri->dnormxdval2),
				  &(mshtri->dnormxdval1));

	dor_rayintmshtri_z_values(loc[0][1],loc[0][2],nrm[0][1],
				  loc[1][1],loc[1][2],nrm[1][1],
				  loc[2][1],loc[2][2],nrm[2][1],
				  &(mshtri->initialnormy),
				  &(mshtri->dnormydval2),
				  &(mshtri->dnormydval1));

	dor_rayintmshtri_z_values(loc[0][1],loc[0][2],nrm[0][2],
				  loc[1][1],loc[1][2],nrm[1][2],
				  loc[2][1],loc[2][2],nrm[2][2],
				  &(mshtri->initialnormz),
				  &(mshtri->dnormzdval2),
				  &(mshtri->dnormzdval1));
	break;

    case dod_rayintmshtri_projection_type_y:
	dor_rayintmshtri_z_values(loc[0][0],loc[0][2],nrm[0][0],
				  loc[1][0],loc[1][2],nrm[1][0],
				  loc[2][0],loc[2][2],nrm[2][0],
				  &(mshtri->initialnormx),
				  &(mshtri->dnormxdval2),
				  &(mshtri->dnormxdval1));

	dor_rayintmshtri_z_values(loc[0][0],loc[0][2],nrm[0][1],
				  loc[1][0],loc[1][2],nrm[1][1],
				  loc[2][0],loc[2][2],nrm[2][1],
				  &(mshtri->initialnormy),
				  &(mshtri->dnormydval2),
				  &(mshtri->dnormydval1));

	dor_rayintmshtri_z_values(loc[0][0],loc[0][2],nrm[0][2],
				  loc[1][0],loc[1][2],nrm[1][2],
				  loc[2][0],loc[2][2],nrm[2][2],
				  &(mshtri->initialnormz),
				  &(mshtri->dnormzdval2),
				  &(mshtri->dnormzdval1));
	break;

    case dod_rayintmshtri_projection_type_z:
	dor_rayintmshtri_z_values(loc[0][0],loc[0][1],nrm[0][0],
				  loc[1][0],loc[1][1],nrm[1][0],
				  loc[2][0],loc[2][1],nrm[2][0],
				  &(mshtri->initialnormx),
				  &(mshtri->dnormxdval2),
				  &(mshtri->dnormxdval1));

	dor_rayintmshtri_z_values(loc[0][0],loc[0][1],nrm[0][1],
				  loc[1][0],loc[1][1],nrm[1][1],
				  loc[2][0],loc[2][1],nrm[2][1],
				  &(mshtri->initialnormy),
				  &(mshtri->dnormydval2),
				  &(mshtri->dnormydval1));

	dor_rayintmshtri_z_values(loc[0][0],loc[0][1],nrm[0][2],
				  loc[1][0],loc[1][1],nrm[1][2],
				  loc[2][0],loc[2][1],nrm[2][2],
				  &(mshtri->initialnormz),
				  &(mshtri->dnormzdval2),
				  &(mshtri->dnormzdval1));
	break;

    default:
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_rayintmshtri_create",
		    "projection type");
	return((dot_object *)NULL);
	break;
    }

    rayintmshtriobj = dor_object_create(doe_rayintmshtri_class_id,mshtri);

    return(rayintmshtriobj);
}
/*
 ======================================================================
 */

void dor_rayintmshtri_z_values (
    DtReal x1,
    DtReal y1,
    DtReal z1,
    DtReal x2,
    DtReal y2,
    DtReal z2,
    DtReal x3,
    DtReal y3,
    DtReal z3,
    DtReal *initialz,
    DtReal *dzdy,
    DtReal *dzdx)
{
    DtReal a, b, c, d;
    DtReal oneoverc;

    dor_math_plane_equation(x1,y1,z1,x2,y2,z2,x3,y3,z3,&a,&b,&c,&d);

    oneoverc = 1 / c;

    *initialz = (-a*(0.)-b*(0.)-d) * oneoverc;

    *dzdy = -b * oneoverc;

    *dzdx = -a * oneoverc;
}
