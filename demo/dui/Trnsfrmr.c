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
#include "Trnsfrmr.h"

struct transformer {
	DtMatrix4x4 M;
	DtCompType comptype;
} *T;


static DtInt transcount=0, arraysize=0;



/*******************************************************************************
 **********************  M a k e T r a n s f o r m e r  ************************
 *******************************************************************************
 * Module Name: MakeTransformer
 * Module Type: c
 *     Purpose: 
 *      Inputs: comptype ==> specifies how the new composit matrices will
 *		   operate within the groups into which the user places the 
 *		   DoTransformationMatrix objects returned from 
 *		   ChangeTransformer: DcPreConcatenate meant that the new 
 *		   change should appear as if it were the first one; 
 *		   DcPostConcatenate appears to be applied last.
 *     Outputs: None
 * ReturnValue: Reference number for a transformer or a -1 if failed.
 ******************************************************************************/
DtInt MakeTransformer(comptype)
DtCompType comptype;
{
	struct transformer *newtrans;

#ifdef ERRCHK
	if((DcPreConcatenate != comptype) &&
	   (DcPostConcatenate != comptype) &&
	   (DcReplace != comptype))
		{
		printf("MakeTransformer: invalid comptype %d\n", comptype);
		return(-1);
		}
#endif
	if(transcount == arraysize)
		{
		arraysize += 100;
		T = (struct transformer *)
			dor_space_reallocate( T, arraysize * sizeof(*T));
		}
	newtrans = &T[transcount++];
	dor_matrix_load_identity(newtrans->M);
	newtrans->comptype = comptype;
	return(transcount-1);

}  /* End of MakeTransformer function */




/*******************************************************************************
 ********************  C h a n g e T r a n s f o r m e r  **********************
 *******************************************************************************
 * Module Name: ChangeTransformer
 * Module Type: c
 *     Purpose: Add a transformation onto a transformer matrix.
 *      Inputs: transnumber ==>
 *		transtype ==>
 *		transvalue ==>
 *		comptype ==> specifies how the change is to be applied to the
 *		   transformer matrix.  Either DcPreConcatenate or 
 *		   DcPostConcatenate.
 *     Outputs: None
 * ReturnValue: The DoTransformMatrix object resulting from the transformation
 *		addition.
 ******************************************************************************/
DtObject ChangeTransformer(transnumber, transtype, transvalue, comptype)
DtInt transnumber, transtype;
DtReal transvalue;
DtCompType comptype;
{
	DtObject dor_tfmmat_create();

#ifdef ERRCHK
	if((0 > transnumber) || (transnumber > transcount))
		{
		printf("ChangeTransformer: invalid transnumber %d\n",
			transnumber);
		return(DcNullObject);
		}
	if((0 > transtype) || (transtype >= 10))
		{
		printf("ChangeTransformer: invalid changetype %d\n",
			transtype);
		return(DcNullObject);
		}
	if((DcPreConcatenate != comptype) &&
	   (DcPostConcatenate != comptype) &&
	   (DcReplace != comptype))
		{
		printf("ChangeTransformer: invalid comptype %d\n", comptype);
		return(DcNullObject);
		}
#endif
	transform_matrix(T[transnumber].M, transtype, transvalue, comptype);
	return(dor_tfmmat_create(T[transnumber].M, T[transnumber].comptype));

}  /* End of ChangeTransformer function */



/*******************************************************************************
 ********************  U p d a t e T r a n s f o r m e r  **********************
 *******************************************************************************
 * Module Name: UpdateTransformer
 * Module Type: c
 *     Purpose: Same as ChangeTransformer, but don't create a new 
 *		transformation object.
 *      Inputs:
 *     Outputs:
 * ReturnValue:
 ******************************************************************************/
void UpdateTransformer(transnumber, transtype, transvalue, comptype)
DtInt transnumber, transtype;
DtReal transvalue;
DtCompType comptype;
{
#ifdef ERRCHK
	if((0 > transnumber) || (transnumber > transcount))
		{
		printf("ChangeTransformer: invalid transnumber %d\n",
			transnumber);
		return;
		}
	if((0 > transtype) || (transtype >= 10))
		{
		printf("ChangeTransformer: invalid changetype %d\n",
			transtype);
		return;
		}
	if((DcPreConcatenate != comptype) &&
	   (DcPostConcatenate != comptype) &&
	   (DcReplace != comptype))
		{
		printf("ChangeTransformer: invalid comptype %d\n", comptype);
		return;
		}
#endif
	transform_matrix(T[transnumber].M, transtype, transvalue, comptype);

}  /* End of UpdateTransformer function */



/*******************************************************************************
 ********************* t r a n s f o r m _ m a t r i x  ************************
 *******************************************************************************
 * Module Name: transform_matrix
 * Module Type: c
 *     Purpose: Concatenates a transformation onto a matrix.
 *      Inputs:
 *     Outputs:
 * ReturnValue:
 ******************************************************************************/
static transform_matrix(matrix, transtype, value, comptype)
DtMatrix4x4 matrix;
DtInt transtype;
DtReal value;
DtCompType comptype;
{

switch(transtype)
		{
		case DcTransX:
			dor_matrix_translate(matrix, value, 0., 0., comptype);
			break;
		case DcTransY:
			dor_matrix_translate(matrix, 0., value, 0., comptype);
			break;
		case DcTransZ:
			dor_matrix_translate(matrix, 0., 0., value, comptype);
			break;
		case DcScaleX:
			dor_matrix_scale(matrix, value, 1., 1., comptype);
			break;
		case DcScaleY:
			dor_matrix_scale(matrix, 1., value, 1., comptype);
			break;
		case DcScaleZ:
			dor_matrix_scale(matrix, 1., 1., value, comptype);
			break;
		case DcRotX:
			dor_matrix_rotate_x(matrix, value, comptype);
			break;
		case DcRotY:
			dor_matrix_rotate_y(matrix, value, comptype);
			break;
		case DcRotZ:
			dor_matrix_rotate_z(matrix, value, comptype);
			break;
		case DcReset:
			dor_matrix_load_identity(matrix);
			break;
		}

}  /* End of transform_matrix function */



/*******************************************************************************
 *****************  I n q T r a n s f o r m e r M a t r i x  *******************
 *******************************************************************************
 * Module Name: InqTransformerMatrix
 * Module Type: c
 *     Purpose: Return the matrix from a transformer.
 *      Inputs:
 *     Outputs:
 * ReturnValue:
 ******************************************************************************/
DtReal *InqTransformerMatrix(mid)
DtInt mid;
{
	if (mid<0 || mid>=transcount)
		return ((DtReal *)0);

	return((DtReal *)T[mid].M);

}  /* End of InqTransformerMatrix function */

