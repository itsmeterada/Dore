#ifndef DD_PRISRF_H
#define DD_PRISRF_H
 
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

struct prisrf {
    DtSurface surface_type;
    DtInt subdivtype;
    DtReal subdivparms[1];
    DtObjectStructure *alternate_object;
};

    /***  Function Prototypes  ***/

void        dor_prisrf_initialize (void);
dot_object *dor_prisrf_create (DtSurface);
void        dor_prisrf_destroy (dot_object *);
void        dor_prisrf_print (dot_object *);
dot_object *dor_prisrf_update_alternate (dot_object *);
dot_object *dor_prisrf_create_alternate_geometry (struct prisrf *);
dot_object *dor_prisrf_create_alternate_geometry_box
		    (DtInt, DtReal, DtSwitch, DtSwitch);
dot_object *dor_prisrf_create_alternate_geometry_cone
		    (DtInt, DtReal, DtSwitch, DtSwitch);
dot_object *dor_prisrf_create_alternate_geometry_cylinder
		    (DtInt, DtReal, DtSwitch, DtSwitch);
dot_object *dor_prisrf_create_alternate_geometry_sphere
		    (DtInt, DtReal, DtSwitch, DtSwitch);
void        dor_prisrf_sphere_triangles
		    (DtInt, DtInt, DtInt, DtInt, DtInt, DtInt);
DtReal      dor_prisrf_sphere_ucomp (DtReal, DtReal);
DtReal      dor_prisrf_sphere_vcomp (DtReal);
DtFlag      dor_prisrf_check_subdiv (struct prisrf *);
void        dor_prisrf_compuv_usercallback
		    (DtObject, DtPFI, DtObject, DtReal, DtReal, DtReal,
		     DtReal, DtReal, DtReal, DtReal *, DtReal *);
void        dor_prisrf_compuvw_usercallback
		    (DtObject, DtPFI, DtObject, DtReal, DtReal, DtReal,
		     DtReal, DtReal, DtReal, DtReal *, DtReal *, DtReal *);
void        dor_prisrf_compute_bounding_volume (dot_object *);
void        dor_prisrf_pick (dot_object *);
void        dor_prisrf_render_standard (dot_object *);

#endif
