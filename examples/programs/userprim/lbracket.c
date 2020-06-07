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
 
#include <dore/dore.h>
#include "lbracket.h"

DtInt TypeLbracket;  /* primitive class identification number */ 
 
/*
extern int LbracketDestroy(); 
extern int LbracketPrint(); 
extern int LbracketComputeBoundVol(); 
extern DtObject LbracketUpdateAlternate(); 
extern int LbracketRender(); 
extern int LbracketPick(); 
*/
 
static DtMethodEntry methods[] = { 
     	DcMethodPrint, (DtMethodPtr) LbracketPrint, 
     	DcMethodDestroy, (DtMethodPtr) LbracketDestroy, 
     	DcMethodCmpBndVolume, (DtMethodPtr) LbracketComputeBoundVol, 
     	DcMethodUpdStdAltObj, (DtMethodPtr) LbracketUpdateAlternate, 
     	DcMethodStdRenderDisplay, (DtMethodPtr) LbracketRender, 
     	DcMethodDynRender, (DtMethodPtr) LbracketRender, 
     	DcMethodGlbrndIniObjs, (DtMethodPtr) LbracketRender,
     	DcMethodPick, (DtMethodPtr) LbracketPick, 
}; 
 
/*
=============================================================================
*/
 
int LbracketInitialize (void) 
{ 
     	TypeLbracket = DeAddClass("DUoLbracket", 8, methods, DcNullPtr); 
}

/*
=============================================================================
*/

DtObject DUoLbracket (
    DtReal side1,
    DtReal side2,
    DtReal width,
    DtReal thickness) 
 
{ 
    struct Lbracket *lbracket; 
    DtObject lbracket_object; 
    DtObject LbracketCreateAlternateGeom(); 
    static DtFlag initialize = DcTrue; 

    if (initialize) { 
         LbracketInitialize(); 
         initialize = DcFalse; 
    } 

    lbracket = (struct Lbracket *)malloc(sizeof *lbracket); 

    if (side1 > 0)
    	lbracket->side1 = side1; 
    else {
	printf ("Warning - side1 <= 0 - Null object returned");
	return (DcNullObject);
    }

    if (side2 > 0)
    	lbracket->side2 = side2; 
    else {
	printf ("Warning - side2 <= 0 - Null object returned");
	return (DcNullObject);
    }

    if (width > 0)
    	lbracket->width = width; 
    else {
	printf ("Warning - width <= 0 - Null object returned");
	return (DcNullObject);
    }

    if (thickness > 0)
    	lbracket->thickness = thickness; 
    else {
	printf ("Warning - thickness <= 0 - Null object returned");
	return (DcNullObject);
    }

    lbracket->alternate_object =
    LbracketCreateAlternateGeom(lbracket); 

    lbracket_object = DeCreateObject(TypeLbracket, lbracket); 

    return(lbracket_object); 
} 
  
/*
=============================================================================
*/

int LbracketPrint (
    DtObjectStructure *object)  
{  
        struct Lbracket *lbracket;  

        lbracket = (struct Lbracket *)(object->data);  

        if (lbracket == (struct Lbracket *)0 ) { 
               printf ("L-bracket is NULL\n"); 
               return; 
        } 
        printf ("L-bracket at [%lx]:\n", lbracket); 
        printf (" side1 %f side2 %f width %f thickness %f\n", 
           		lbracket->side1, lbracket->side2, 
           		lbracket->width, lbracket->thickness); 
        printf("L-bracket end\n"); 
}  
 
/*
=============================================================================
*/

int LbracketDestroy (
    DtObjectStructure *object)  
{  
        struct Lbracket *lbracket;  

        lbracket = (struct Lbracket *)(object->data);  

        DeDeleteObject(lbracket->alternate_object);  
        free(lbracket);  
}  
 
/*
=============================================================================
*/

int LbracketComputeBoundVol (
    DtObjectStructure *object)
{
	struct Lbracket *lbracket;  
   
        lbracket = (struct Lbracket *)(object->data);

	LbracketUpdateAlternate(object);
	if (lbracket->alternate_object != DcNullObject) {
		DeExecuteAlternate(lbracket->alternate_object);
	}
}

/*
=============================================================================
*/

DtObject LbracketUpdateAlternate (
    DtObjectStructure *object) 
{ 
     	struct Lbracket *lbracket; 
     	DtObject LbracketCreateAlternateGeom();
 
     	lbracket = (struct Lbracket *)(object->data); 
 
     	if (lbracket->alternate_object == DcNullObject)
          	return (LbracketCreateAlternateGeom(lbracket)); 
     	else 
          	return (lbracket->alternate_object); 
} 

/*
=============================================================================
*/

int LbracketRender (
    DtObjectStructure *object)  
{  
        struct Lbracket *lbracket;  
                           
        if (!DeInqRenderable(TypeLbracket)){  
                return;  
        }  
                   
        lbracket = (struct Lbracket *)(object->data);  
                   
	LbracketUpdateAlternate(object);
        if (lbracket->alternate_object != DcNullObject) {
                DeExecuteAlternate(lbracket->alternate_object);
        } 
} 
  
/*
=============================================================================
*/

DtObject LbracketCreateAlternateGeom (
    struct Lbracket *lbracket)  
{  
        DtReal vtxs[14][3];  
        DtInt contours[10];  
        DtInt vlist[40];  
	int i;
        DtObject mesh_object;  

        vtxs[0][0] = 0;
        vtxs[0][1] = 0;
        vtxs[0][2] = 0;

        vtxs[1][0] = lbracket->side1;
        vtxs[1][1] = 0;
        vtxs[1][2] = 0;
         
        vtxs[2][0] = lbracket->side1;
        vtxs[2][1] = -(lbracket->thickness);
        vtxs[2][2] = 0;
         
        vtxs[3][0] = 0;
        vtxs[3][1] = -(lbracket->thickness);
        vtxs[3][2] = 0;
         
        vtxs[4][0] = -(lbracket->thickness);
        vtxs[4][1] = -(lbracket->thickness);
        vtxs[4][2] = 0;
         
        vtxs[5][0] = -(lbracket->thickness);
        vtxs[5][1] = lbracket->side2;
        vtxs[5][2] = 0;
         
        vtxs[6][0] = 0;
        vtxs[6][1] = lbracket->side2;
        vtxs[6][2] = 0;
         
        vtxs[7][0] = 0;
        vtxs[7][1] = 0;
        vtxs[7][2] = lbracket->width;

        vtxs[8][0] = lbracket->side1;
        vtxs[8][1] = 0;
        vtxs[8][2] = lbracket->width;
         
        vtxs[9][0] = lbracket->side1;
        vtxs[9][1] = -(lbracket->thickness);
        vtxs[9][2] = lbracket->width;
         
        vtxs[10][0] = 0;
        vtxs[10][1] = -(lbracket->thickness);
        vtxs[10][2] = lbracket->width;
         
        vtxs[11][0] = -(lbracket->thickness);
        vtxs[11][1] = -(lbracket->thickness);
        vtxs[11][2] = lbracket->width;
         
        vtxs[12][0] = -(lbracket->thickness);
        vtxs[12][1] = lbracket->side2;
        vtxs[12][2] = lbracket->width;
         
        vtxs[13][0] = 0;
        vtxs[13][1] = lbracket->side2;
        vtxs[13][2] = lbracket->width;

        for (i=0; i<10; i++) {
                contours[i] = 4;
        }

        vlist[0] = 0;  vlist[1] = 1;  vlist[2] = 2;  vlist[3] = 3;
        vlist[4] = 5;  vlist[5] = 6;  vlist[6] = 3;  vlist[7] = 4;
        vlist[8] = 1;  vlist[9] = 8;  vlist[10] = 9;  vlist[11] = 2;
        vlist[12] = 7;  vlist[13] = 10;  vlist[14] = 9;  vlist[15] = 8;
	vlist[16] = 10;  vlist[17] = 13;  vlist[18] = 12;  vlist[19] = 11;
        vlist[20] = 5;  vlist[21] = 12;  vlist[22] = 13;  vlist[23] = 6;
        vlist[24] = 0;  vlist[25] = 6;  vlist[26] = 13;  vlist[27] = 7;
	vlist[28] = 0;  vlist[29] = 7;  vlist[30] = 8;  vlist[31] = 1;
        vlist[32] = 4;  vlist[33] = 2;  vlist[34] = 9;  vlist[35] = 11;
	vlist[36] = 12;  vlist[37] = 5;  vlist[38] = 4;  vlist[39] = 11;

        mesh_object = DoSimplePolygonMesh(DcRGB, DcLoc, 14, (DtReal *)vtxs,
                                   10, contours, vlist, DcConvex, DcFalse);  
  
        return(mesh_object);  
} 
 
/*
=============================================================================
*/

int LbracketPick (
    DtObjectStructure *object)  
{  
	struct Lbracket *lbracket;  

        if (!DeInqPickable(TypeLbracket)) {  
                return;  
        }  
  
        lbracket = (struct Lbracket *)(object->data);
                   
        DeInitializeObjPick(object);  

	LbracketUpdateAlternate(object);
        if (lbracket->alternate_object != DcNullObject) {
                DeExecuteAlternate(lbracket->alternate_object);
        }

}  
 
/*
=============================================================================
*/
