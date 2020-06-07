#ifndef DD_TORUS_H
#define DD_TORUS_H

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

struct torus {
    DtReal R;
    DtReal r;
    DtInt subdivtype;
    DtReal subdivparms[1];
    DtObjectStructure *alternate_object;
};

    /***  Function Prototypes  ***/

void        dor_torus_initialize (void);
dot_object *dor_torus_create (DtReal, DtReal);
void        dor_torus_destroy (dot_object *);
void        dor_torus_print (dot_object *);
dot_object *dor_torus_update_alternate (dot_object *);
DtFlag      dor_torus_check_subdiv (struct torus *);
void        dor_torus_compute_bounding_volume (dot_object *);
void        dor_torus_pick (dot_object *);
void        dor_torus_render_standard (dot_object *);
dot_object *dor_torus_create_alternate_geometry (struct torus *);
DtReal      dor_torus_ucomp (DtReal, DtReal);
DtReal      dor_torus_vcomp (DtReal, DtReal);
void        dor_torus_compuv_usercallback
		    (DtObject, DtPFI, DtObject, DtReal, DtReal, DtReal,
		     DtReal, DtReal, DtReal, DtReal *, DtReal *);
void        dor_torus_compuvw_usercallback
		    (DtObject, DtPFI, DtObject, DtReal, DtReal, DtReal,
		     DtReal, DtReal, DtReal, DtReal *, DtReal *, DtReal *);

#endif
