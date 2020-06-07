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
 
/*
  ======================================================================
  Functions:
	dor_patchmatrix_initialize
	dor_patchmatrix_initialize_types
	dor_patchmatrix_create 
	dor_patchmatrix_destroy 
	dor_patchmatrix_print 

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/patchmat.h>

/*
 ======================================================================
 */

DtInt doe_patchmatrix_class_id;

static DtMethodEntry patchmatrix_methods[] = {
    { DcMethodDestroy, (DtMethodPtr) dor_patchmatrix_destroy },
    { DcMethodPrint,   (DtMethodPtr) dor_patchmatrix_print   }
};

void dor_patchmatrix_initialize (void)
{
    doe_patchmatrix_class_id = dor_class_install (DcTypeMatrix, "DoMatrix", 2,
						  patchmatrix_methods,DcNullPtr);
}
/*
 ======================================================================
 */

DtObject DcBezier3;
DtObject DcBezier4;
DtObject DcHermite4;
DtObject DcBSpline4;

static DtReal bezier3mat[9]={
	 1.0, -2.0,  1.0,
	-2.0,  2.0,  0.0,
	 1.0,  0.0,  0.0,
};
static DtReal bezier4mat[16]={/* Foley and Van Dam, p520 */
	-1.0,  3.0, -3.0,  1.0,
	 3.0, -6.0,  3.0,  0.0,
	-3.0,  3.0,  0.0,  0.0,
	 1.0,  0.0,  0.0,  0.0,
};
static DtReal bspline4mat[16]={/* Foley and Van Dam, p521 */
	-1.0/6.0, 3.0/6.0, -3.0/6.0, 1.0/6.0,
	3.0/6.0, -6.0/6.0, 3.0/6.0, 0.0,
	-3.0/6.0, 0.0, 3.0/6.0, 0.0,
	1.0/6.0, 4.0/6.0, 1.0/6.0, 0.0,
};
static DtReal hermite4mat[16]={/* Foley and Van Dam, p517 */
	 2.0, -2.0,  1.0,  1.0,
	-3.0,  3.0, -2.0, -1.0,
	 0.0,  0.0,  1.0,  0.0,
	 1.0,  0.0,  0.0,  0.0,
};

void dor_patchmatrix_initialize_types (void)
{
    DcBezier3 =dor_patchmatrix_create(3,3,bezier3mat);
    DcBezier4 =dor_patchmatrix_create(4,4,bezier4mat);
    DcHermite4=dor_patchmatrix_create(4,4,hermite4mat);
    DcBSpline4=dor_patchmatrix_create(4,4,bspline4mat);

    /* Create dummy references to these objects in the event that
     * they are dereferenced such that their reference count goes to
     * zero: this keeps them from being deleted entirely.
     */

    dor_object_add_reference(DcBezier3, DcNullObject);
    dor_object_add_reference(DcBezier4, DcNullObject);
    dor_object_add_reference(DcHermite4, DcNullObject);
    dor_object_add_reference(DcBSpline4, DcNullObject);
}
/*
 ======================================================================
 */

dot_object *dor_patchmatrix_create (
    unsigned nrow,
    unsigned ncol,
    DtReal data[])
{
    static DtFlag initialized = DcFalse;
    register int k;
    register struct matrix *m;

    if (!initialized) {
	dor_patchmatrix_initialize();
	initialized = DcTrue;
    }

    if (20<nrow || 20<ncol) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "DoMatrix", "nrow>20 or ncol>20");
	return(DcNullObject);
    }
    m = (struct matrix *)dor_space_allocate(sizeof *m);
    m->nrow=nrow; m->ncol=ncol;
    k=nrow*ncol*sizeof(*m->data);
    bcopy(data,m->data=(DtReal *)dor_space_allocate(k),k);
    return(dor_object_create(doe_patchmatrix_class_id,m));
}
/*
 ======================================================================
 */

void dor_patchmatrix_destroy (dot_object *object)
{
    struct matrix *matrix;

    matrix = (struct matrix *)(object->data);

    if (matrix->data != NULL){
	dor_space_deallocate(matrix->data);
    }

    dor_space_deallocate(matrix);
}
/*
 ======================================================================
 */

void dor_patchmatrix_print (dot_object *object)
{
    DtShort row;
    struct matrix *matrix;

    matrix = (struct matrix *)(object->data);

    if (matrix == NULL) {
	dor_print_output("bad values");
	return;
    }

    for (row=0; row < matrix->nrow; row++) {
	if (matrix->ncol == 3) {
	    sprintf(dor_print_get_line(),"%f %f %f ",
		    matrix->data[row*3],
		    matrix->data[row*3+1],
		    matrix->data[row*3+2]);
	    dor_print_output(dor_print_get_line());
	} else {
	    sprintf(dor_print_get_line(),"%f %f %f %f",
		    matrix->data[row*4],
		    matrix->data[row*4+1],
		    matrix->data[row*4+2],
		    matrix->data[row*4+3]);
	    dor_print_output(dor_print_get_line());
	}
    }
}
