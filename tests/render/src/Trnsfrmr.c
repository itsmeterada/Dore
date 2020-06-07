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
 

/*****************************************************************************
                TRANSFORMERS

Included in this module:

    DtInt make_transformer(comptype)
    DtObject change_transformer(transnum, transtype, transvalue,
        comptype, newobj)
    void get_transformer_matrix(transnum, usermatrix)
    void put_transformer_matrix(transnum, usermatrix)
*****************************************************************************/

#include <math.h>
#include <dore/dore.h>
#include "trnsfrmr.h"

#define X 0
#define Y 1
#define Z 2


struct transformer {
    DtMatrix4x4 M;
    DtCompType comptype;
} *T;


static DtInt transcount=0, arraysize=0;

static void transform_matrix (DtMatrix4x4, DtInt, DtReal, DtCompType);
static void load_identity    (DtMatrix4x4);
static void load_matrix      (DtMatrix4x4, DtMatrix4x4);
static void translate        (DtMatrix4x4, int, DtReal, DtCompType);
static void scale            (DtMatrix4x4, int, DtReal, DtCompType);
static void rotate           (DtMatrix4x4, int, int, DtReal, DtCompType);



/*******************************************************************************
 **********************  m a k e _ t r a n s f o r m e r  **********************
 *******************************************************************************
 * Module Name: make_transformer
 * Module Type: c
 *     Purpose:
 *      Inputs: comptype ==> specifies how the new composit matrices will
 *                 operate within the groups into which the user places the
 *                 DoTransformationMatrix objects returned from
 *         change_transformer.
 *          = DcPreConcatenate ==> the new change should appear
 *               as if it were the first one;
 *                      = DcPostConcatenate ==> appears to be applied last.
 *     Outputs: None
 * ReturnValue: Reference number for a transformer or a -1 if failed. (Used 
 *      when requesting a new DoTransformationMatrix object via
 *       change_transformer.)
 ******************************************************************************/
DtInt make_transformer (DtCompType comptype)
{
    struct transformer *newtrans;

    if((DcPreConcatenate != comptype) &&
       (DcPostConcatenate != comptype) &&
       (DcReplace != comptype)) {
        printf("make_transformer: invalid comptype %d\n", comptype);
        return(-1);
    }
    if(transcount == arraysize) {
    if(0 == arraysize) /* first time */
        T = (struct transformer *)
            malloc((arraysize+100) * sizeof(*T));
    else
        T = (struct transformer *)
            realloc(T, (arraysize+100) * sizeof(*T));
    arraysize += 100;
    if(!T) {
        printf("make_transformer: out of memory\n");
        return(-1);
    }
    }
    newtrans = &T[transcount++];
    load_identity(newtrans->M);
    newtrans->comptype = comptype;
    return(transcount-1);

}  /* End of make_transformer function */



/*******************************************************************************
 ********************  c h a n g e _ t r a n s f o r m e r  ********************
 *******************************************************************************
 * Module Name: change_transformer
 * Module Type: c
 *     Purpose: Add a transformation onto a transformer matrix (as specified
 *      bye transnum).
 *      Inputs: transnumber ==> trasformer to be modified
 *              transtype ==> must be one of the transformation types listed
 *           in /opt/examples/util/util.h
 *              transvalue ==> is the magitude of the current transformation
 *              comptype ==> specifies how the change is to be applied to the
 *                 transformer matrix.  
 *          = DcPreConcatenate ==> new change should appear as 
 *              if it were the first one
 *                      = DcPostConcatenate ==> new change appears to be
 *              applied last
 *      newobj ==> flag which specifies whether to return a Dore
 *           transform matrix object resulting from the new change
 *          = DcTrue ==> return the new transform matrix object
 *          = DcFalse ==> new transformation is accumulated, but
 *               a transform matrix object is not created
 *               The value "DcNullObject" is returned instead.
 *     Outputs: None
 * ReturnValue: The DoTransformMatrix object resulting from the transformation
 *              addition.
 ******************************************************************************/
DtObject change_transformer (
    DtInt transnum,
    DtInt transtype,
    DtReal transvalue,
    DtCompType comptype,
    DtFlag newobj)
{
    if((0 > transnum) || (transnum> transcount)) {
        printf("change_transformer: invalid transnum%d\n",
            transnum);
        return(DcNullObject);
    }
    if((0 > transtype) || (transtype > Reset)) {
        printf("change_transformer: invalid changetype %d\n",
            transtype);
        return(DcNullObject);
    }
    if((DcPreConcatenate != comptype) &&
       (DcPostConcatenate != comptype) &&
       (DcReplace != comptype)) {
        printf("change_transformer: invalid comptype %d\n", comptype);
        return(DcNullObject);
    }
    transform_matrix(T[transnum].M, transtype, transvalue, comptype);
    if(newobj)
        return((DtObject)DoTransformMatrix(
            T[transnum].M, T[transnum].comptype));
    else
        return(DcNullObject);

}  /* End of change_transformer function */



/*******************************************************************************
 **************  g e t _ t r a n s f o r m e r _ m a t r i x  ******************
 *******************************************************************************
 * Module Name: get_transformer_matrix
 * Module Type: c
 *     Purpose: Copies the transformer matrix specified by "transnum" into
 *      the user supplied matrix "usermatrix".
 *      Inputs: transnum ==> a transformer ID generated by make_transformer()
 *     Outputs: usermatrix ==> a user supplied matrix to receive the matrix
 *      portion of the transformer object.
 * ReturnValue: None
 ******************************************************************************/
void get_transformer_matrix (
    DtInt transnum,
    DtMatrix4x4 usermatrix)
{
    load_matrix(usermatrix, T[transnum].M);
}



/*******************************************************************************
 **************  p u t _ t r a n s f o r m e r _ m a t r i x  ******************
 *******************************************************************************
 * Module Name: put_transformer_matrix
 * Module Type: c
 *     Purpose: Copies the user supplied matrix "usermatrix" into the 
 *      transformer matrix specified by "transnum".
 *      Inputs: usermatrix ==> a user supplied matrix to be copied into the
 *       matrix portion of the transformer object.
 *     Outputs: transnum ==> a transformer ID generated by make_transformer()
 * ReturnValue: None
 ******************************************************************************/
void put_transformer_matrix (
    DtInt transnum,
    DtMatrix4x4 usermatrix)
{
    load_matrix(T[transnum].M, usermatrix);
}



/*******************************************************************************
 ********************  t r a n s f o r m _ m a t r i x  ************************
 *******************************************************************************
 * Module Name: transform_matrix
 * Module Type: c
 *     Purpose: Adds a transformation to a transformer matrix.
 *      Inputs: usermatrix ==> a user supplied matrix to be added to the
 *          matrix portion of the transformer object.
 *      transtype ==> must be one of the transformation types listed
 *                       in /opt/examples/util/util.h
 *              transvalue ==> is the magitude of the current transformation
 *              comptype ==> specifies how the change is to be applied to the
 *                  transformer matrix.
 *                      = DcPreConcatenate ==> new change should appear as
 *                              if it were the first one
 *                      = DcPostConcatenate ==> new change appears to be
 *                              applied last
 *     Outputs: None
 * ReturnValue: None
 ******************************************************************************/
static void transform_matrix (
    DtMatrix4x4 usermatrix,
    DtInt transtype,
    DtReal transvalue,
    DtCompType comptype)
{
    switch(transtype)
    {
        case NoTransf:
            break;
        case TransX:
            translate(usermatrix, X, transvalue, comptype);
            break;
        case TransY:
            translate(usermatrix, Y, transvalue, comptype);
            break;
        case TransZ:
            translate(usermatrix, Z, transvalue, comptype);
            break;
        case ScaleX:
            scale(usermatrix, X, transvalue, comptype);
            break;
        case ScaleY:
            scale(usermatrix, Y, transvalue, comptype);
            break;
        case ScaleZ:
            scale(usermatrix, Z, transvalue, comptype);
            break;
        case RotX:
            rotate(usermatrix, Y, Z, transvalue, comptype);
            break;
        case RotY:
            rotate(usermatrix, Z, X, transvalue, comptype);
            break;
        case RotZ:
            rotate(usermatrix, X, Y, transvalue, comptype);
            break;
        case Reset:
            load_identity(usermatrix);
            break;
        default:
            printf("transform_matrix: unknown transtype %d\n",
                transtype);
            break;
    }
}  /* End of transform_matrix function */





/*******************************************************************************
 *********  m a t r i x   m a n i p u l a t i o n   r o u t i n e s  ***********
 ******************************************************************************/

static DtMatrix4x4 identity =
    {
        {1., 0., 0., 0.},
        {0., 1., 0., 0.},
        {0., 0., 1., 0.},
        {0., 0., 0., 1.}
    };


static void load_identity (DtMatrix4x4 matrix)
{
    load_matrix (matrix, identity);
}


static void load_matrix (
    DtMatrix4x4 matrix,
    DtMatrix4x4 newmat)
{
    register DtInt row, col;

    for (row=0;  row<4;  row++)
        for (col=0;  col<4;  col++)
            matrix[row][col] = newmat[row][col];
}


/*  Pre-concatenate transformation represented by concatmatrix
with transformation represented by matrix. Since we are using
column vectors this is a matrix post-multiply!  */

static pre_concatenate (
    DtMatrix4x4 matrix,
    DtMatrix4x4 concatmatrix,
    DtMatrix4x4 result)
{
    DtMatrix4x4 tempmatrix;
    DtInt row, col;
    DtInt i;

    for (row=0;row<4;row++)
        for (col=0;col<4;col++) {
            tempmatrix[row][col] = 0.;
            for (i=0;i<4;i++)
                tempmatrix[row][col] +=
                    matrix[row][i]*concatmatrix[i][col];
        }
    load_matrix(result, tempmatrix);
}


int concatenate (
    DtMatrix4x4 onto,
    DtMatrix4x4 from,
    DtCompType compose_type)
{
    switch(compose_type) {
        case DcPreConcatenate:
            pre_concatenate(onto, from, onto);
            break;
        case DcPostConcatenate:
            pre_concatenate(from, onto, onto);
            break;
        case DcReplace:
            load_matrix(onto, from);
            break;
    }
}



static void translate (
    DtMatrix4x4 matrix,
    int axis,
    DtReal transvalue,
    DtCompType compose_type)
{
    DtMatrix4x4 tempmat;

    load_identity(tempmat);
    tempmat[axis][3] = transvalue;
    concatenate(matrix, tempmat, compose_type);
}





static void scale (
    DtMatrix4x4 matrix,
    int axis,
    DtReal scalevalue,
    DtCompType compose_type)
{
    DtMatrix4x4 tempmat;

    load_identity(tempmat);
    tempmat[axis][axis] = scalevalue;
    concatenate(matrix, tempmat, compose_type);
}





static void rotate (
    DtMatrix4x4 matrix,
    int u,
    int v,
    DtReal radians,
    DtCompType compose_type)
{
    double c, s;

    DtMatrix4x4 tempmat;

    load_identity(tempmat);
    c = cos(radians);
    s = sin(radians);
    tempmat[u][u] = c;
    tempmat[u][v] = -s;
    tempmat[v][u] = s;
    tempmat[v][v] = c;
    concatenate (matrix, tempmat, compose_type);
}
