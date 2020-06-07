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
 

/* transformation types */
#define NoTransf 0
#define TransX   1
#define TransY   2
#define TransZ   3
#define ScaleX   4
#define ScaleY   5
#define ScaleZ   6
#define RotX     7
#define RotY     8
#define RotZ     9
#define Reset   10

/* integer or floating point max */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/* integer or pointer swap */
#define SWAP(a, b) { (a) ^= (b); (b) ^= (a); (a) ^= (b); }


/****** function prototypes: ******/

/* transformers */

DtInt	 make_transformer 	(DtCompType);
DtObject change_transformer 	(DtInt, DtInt, DtReal, DtCompType, DtFlag);
void	 get_transformer_matrix	(DtInt, DtMatrix4x4);
void	 put_transformer_matrix	(DtInt, DtMatrix4x4);
