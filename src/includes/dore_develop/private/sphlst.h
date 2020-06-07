#ifndef DD_SPHLST_H
#define DD_SPHLST_H
 
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
 
struct sphlst {
    DtInt sphere_count;
    DtReal *locations;
    DtReal *radii;
    DtReal *colors;
    DtInt subdivtype;
    DtReal subdivparms[1];
    DtObjectStructure *alternate_object;
};

    /***  Function Prototypes  ***/

void        dor_sphlst_initialize (void);
dot_object *dor_sphlst_create (DtColorModel, DtInt, DtReal*, DtReal*, DtReal*);
void        dor_sphlst_destroy (dot_object *);
void        dor_sphlst_print (dot_object *);
dot_object *dor_sphlst_update_alternate (dot_object *);
void        dor_sphlst_pick (dot_object *);
void        dor_sphlst_render_standard (dot_object *);
dot_object *dor_sphlst_create_alternate_geometry (struct sphlst *);
void        dor_sphlst_gen_octahedra (void);
void        dor_sphlst_sphere_point (DtReal, DtReal, DtInt);
void        dor_sphlst_gen_sphere (DtInt);
DtFlag      dor_sphlst_check_subdiv (struct sphlst *);
void        dor_sphlst_compute_bounding_volume (dot_object *);

#endif
