#ifndef DD_ZBUFFER_H
#define DD_ZBUFFER_H
 
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

    /***  Function Prototypes  ***/
 
void ddr_zbuffer_SetBuffer    (DtInt, DtInt, DtUShort *,
				    void (*)(DtInt, DtInt, DtColorRGB));
void ddr_zbuffer_Clear        (void);
void ddr_zbuffer_ClearRect    (int, int, int, int);
void ddr_zbuffer_DrawPoint    (DtInt*, DtColorRGB);
void ddr_zbuffer_DrawLine     (DtInt*, DtInt*, DtColorRGB, DtColorRGB);
void ddr_zbuffer_DrawTriangle (DtInt*, DtInt*, DtInt*, DtColorRGB,
				     DtColorRGB, DtColorRGB);

#endif
