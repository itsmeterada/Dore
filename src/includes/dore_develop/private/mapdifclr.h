#ifndef DD_MAPDIFCLR_H
#define DD_MAPDIFCLR_H

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
 
struct mapdifclr_data
{   DtMapOperator operator;
    DtObject mapping;
    DtObject raster;
};

    /***  Function Prototypes  ***/

void        dor_mapdifclr_initialize  (void);
dot_object *dor_mapdifclr_create  (DtMapOperator, DtObject, DtObject);
void        dor_mapdifclr_destroy  (dot_object *);
void        dor_mapdifclr_print  (dot_object *);
void        dor_mapdifclr_execute  (dot_object *);

#endif
