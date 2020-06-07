#ifndef DD_RAYINTTRI_H
#define DD_RAYINTTRI_H
 
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

#define dod_rayinttri_projection_type_x	 1
#define dod_rayinttri_projection_type_y	 2
#define dod_rayinttri_projection_type_z	 3

struct rayinttri {
    DtReal x1;
    DtReal y1;
    DtReal z1;
    DtReal x2;
    DtReal y2;
    DtReal z2;
    DtReal x3;
    DtReal y3;
    DtReal z3;
    DtShort projection_type;
    DtReal cosine_x;
    DtReal cosine_y;
    DtReal cosine_z;
    DtReal D;
};

    /***  Function Prototypes  ***/

void        dor_rayinttri_initialize (void);
dot_object *dor_rayinttri_create (DtRealTriple [3]);
void        dor_rayinttri_initialize (void);
dot_object *dor_rayinttri_create (DtRealTriple [3]);

#endif
