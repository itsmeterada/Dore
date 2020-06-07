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
 

extern DtInt TypeLbracket;

struct Lbracket {  
        DtReal side1;  
        DtReal side2;  
        DtReal width;  
        DtReal thickness;  
        DtObject alternate_object;  
 
};  

/* prototype declarations */

int LbracketInitialize  (void);
DtObject DUoLbracket  (DtReal, DtReal, DtReal, DtReal);
int LbracketPrint  (DtObjectStructure *);
int LbracketDestroy  (DtObjectStructure *);
int LbracketComputeBoundVol  (DtObjectStructure *);
DtObject LbracketUpdateAlternate  (DtObjectStructure *);
int LbracketRender  (DtObjectStructure *);
DtObject LbracketCreateAlternateGeom  (struct Lbracket *);
int LbracketPick  (DtObjectStructure *);
int main  (int, char *[]);
