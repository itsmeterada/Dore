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
	dor_patch_initialize
	dor_patch_bezinv
	dor_patch_bezconv
	dor_patch_create
	dor_patch_destroy
	dor_patch_print
	dor_patch_update_alternate
	dor_patch_create_alternate_geometry
	dor_patch_create_alternate_geometry_clr_triangle
	dor_patch_create_alternate_geometry_triangle
	dor_patch_check_subdiv
	dor_patch_check_triangle
	dor_patch_compute_bounding_volume
	dor_patch_get_center
	dor_patch_matmul
	dor_patch_mattrn
	dor_patch_pick
	dor_patch_render_standard
	dor_patch_subdivide_adaptive
	dor_patch_subdivide_fixed
	dor_patch_subdivide_control_along_u
	dump_patch
	dor_patch_subdivide_control_along_v
	dor_patch_subdivide_along_u
	dor_patch_subdivide_along_u_and_v
	dor_patch_subdivide_along_v
	dor_patch_tessellate
	dor_patch_tessellate_patch
	dor_patch_test_linear
	dor_patch_test_u_boundaries
	dor_patch_test_v_boundaries

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/patch.h>
#include <dore/dore_develop/private/trilst.h>
#include <math.h>

/*
 ======================================================================
 */

DtInt doe_patch_class_id;

#define dod_patch_type_undefined                0
#define dod_patch_type_bezpat                   1

#define dod_patch_tessellatetype_nonmesh        0
#define dod_patch_tessellatetype_mesh           1

static DtMethodEntry patch_methods[] = {
   {DcMethodCmpBndVolume,     (DtMethodPtr) dor_patch_compute_bounding_volume},
   {DcMethodDestroy,          (DtMethodPtr) dor_patch_destroy},
   {DcMethodPick,             (DtMethodPtr) dor_patch_pick},
   {DcMethodPrint,            (DtMethodPtr) dor_patch_print},
   {DcMethodUpdStdAltObj,     (DtMethodPtr) dor_patch_update_alternate},
   {DcMethodStdRenderDisplay, (DtMethodPtr) dor_patch_render_standard}
};

void dor_patch_initialize (void)
{
    doe_patch_class_id = dor_class_install (DcTypePatch, "DoPatch", 6,
					    patch_methods, DcNullPtr);
}
/*
 ======================================================================
 */

/* Construct the inverse of the bezier matrix of order n.  First construct the 
   bezier matrix of order n, then invert it.  The bezier matrix of order n is 
   an upside-down Pascal's triangle of binomial coefficients with alternating 
   signs, and the rows multiplied by row leader.  For 3==n, the bezier matrix is:
	-1.0,  3.0, -3.0,  1.0,		row is multiple of 1
	 3.0, -6.0,  3.0,  0.0,		row is multiple of 3
	-3.0,  3.0,  0.0,  0.0,		row is multiple of 3
	 1.0,  0.0,  0.0,  0.0,		row is multiple of 1
	  |
	binomial coefficients with alternating sign
   Note that the matrix is upper anti-triangular,
   i.e., all entries below the anti-diagonal are 0.0.
*/
void dor_patch_bezinv (
    DtReal *result,
    int n)
{
    register int k,j;
    register DtReal *q,*p,*work;

    work=(DtReal *)dor_space_allocate(n*n*sizeof(*work));
    /* clear work space and result */
    for (q=result, p=work, k=n*n; --k>=0; ) {
	*q++=0.0; *p++=0.0;
    }
    /* Pascal's triangle */
    for (k=n; --k>=0; ) {	/* each row, from bottom to top */
	p=work+k*n;
	for (j=n-k; --j>=0; ++p) { /* from left to anti-diagonal */
	    if (0==j)
		    p[0]=1.0;
	    else
		    p[0]=p[n]+p[n-1];
	}
    }
    /* row multiple and alternating sign */
    for (q=work, k=n; --k>=0; ) { /* by rows from bottom to top */
	int odd=1&(1^(n-k)); DtReal m;
	p=work+k*n; m= *q++;
	for (j=n-k; --j>=0; odd^=1) { /* from left to anti-diag */
	    if (odd)
		    *p++*= -m;
	    else
		    *p++*=  m;
	}
    }
    /* now invert into result: solve anti-triangular system */
    for (q=result, k=n; --k>=0; ) {
	/* rows -- result: top to bot;  work: bot to top */
	p=work+k*n;
	/* row begins with k zeroes */
	for (j=k; --j>=0; ) *q++=0.0;
	/* and continues with solution of n-k unknowns */
	for (j=n-k; --j>=0; ) {
	    register int i; register DtReal t,*tp,*tr;
	    for (t=0.0, tp=p, tr=result+n-j-1, i=n-k-1; --i>=0; ) {
		t+= *tp++ * *tr; tr+=n;
	    }
	    if ((1+j+k)==n) {	/* on diagonal; identity has 1.0 */
		*q++=(1.0-t)/ *tp;
	    } else {		/* off diagonal; identity has 0.0 */
		*q++=    -t / *tp;
	    }
	}
    }
    dor_space_deallocate(work);
}

/*
 ======================================================================
 */

/* convert to Bezier:
   L * V * Rt  ==  B * (Binv * L * V * Rt * Btinv) * Bt
   Note that B == Bt; i.e., Bezier matrix is symmetric.
   Also note that the above equation is component-wise in V,
   i.e., must be evaluated with separate tensors for the x,y,z components
   of the vertices.  [Otherwise the dimensions don't match!]
*/

void dor_patch_bezconv (
    struct matrix *lmp,
    DtNPoint3 *vp,
    struct matrix *rmp)
{
    int nrow=lmp->nrow, nr2=nrow*nrow;
    int ncol=rmp->nrow, nc2=ncol*ncol;
    int nel=nrow*ncol, nsq;
    register int k;
    DtReal *br,*rtbt,*bl,*ten0,*ten1,*bezi;

    if (nrow>ncol)
	    nsq=nrow*nrow;
    else
	    nsq=ncol*ncol;

    k=nc2+nc2+nr2+nel+nel+nsq;
    br  =(DtReal*)dor_space_allocate(k*sizeof(DtReal));
    rtbt=nc2+br;
    bl  =nc2+rtbt;
    ten0=nr2+bl;
    ten1=nel+ten0;
    bezi=nel+ten1;

    dor_patch_bezinv(bezi,ncol);
    dor_patch_matmul(br,bezi,rmp->data,ncol,ncol,ncol);
    dor_patch_mattrn(rtbt,br,ncol,ncol);
    if (nrow!=ncol)
	    dor_patch_bezinv(bezi,nrow);
    dor_patch_matmul(bl,bezi,lmp->data,nrow,nrow,nrow);
    for (k=3; --k>=0; ) {	/* each component */
	register int j; register DtReal *tp; register DtNPoint3 *tvp;
	for (tp=ten0,tvp=(DtNPoint3*)(k+(DtReal*)vp),j=nel; --j>=0; ) {
	    *tp++=tvp++->x;
	}
	dor_patch_matmul(ten1,ten0,rtbt,nrow,ncol,ncol);
	dor_patch_matmul(ten0,bl,  ten1,nrow,nrow,ncol);
	for (tp=ten0,tvp=(DtNPoint3*)(k+(DtReal*)vp),j=nel; --j>=0; ) {
	    tvp++->x= *tp++;
	}
    }
    dor_space_deallocate(br);
}

/*
 ======================================================================
 */

dot_object *dor_patch_create  (
    DtColorModel colormodel,
    DtVertexType vertextype,
    dot_object *lmatrix,
    DtNPoint3 vertices[],
    dot_object *rmatrix)
{
    static DtFlag initialized = DcFalse;
    struct patch *new;
    DtInt matrix_class_id = dor_class_inq_id("DoMatrix");
    struct matrix *lmp=(struct matrix *)lmatrix->data;
    struct matrix *rmp=(struct matrix *)rmatrix->data;

    if (!initialized) {
	dor_patch_initialize();
	initialized = DcTrue;
    }

    if ((vertextype != DcLocClr) &&
	(vertextype != DcLoc)) {
	DDerror (ERR_BAD_VERTEX_TYPE,
		    "DoPatch", DcNullPtr);

	return ((DtObject)dor_null_create(DcNullPtr));
    }

    if ((DcObjectValid != dor_object_validate(lmatrix)) ||
	(DcObjectValid != dor_object_validate(rmatrix)) ||
	(dor_object_inq_type(lmatrix) != matrix_class_id) ||
	(dor_object_inq_type(rmatrix) != matrix_class_id)) {
	DDerror (ERR_INVALID_MATRIX_OBJECT,
		    "DoPatch", DcNullPtr);
	return ((DtObject)dor_null_create(DcNullPtr));
    }

    lmp=(struct matrix *)lmatrix->data;
    rmp=(struct matrix *)rmatrix->data;

    if (lmp->nrow!=lmp->ncol ||
	rmp->nrow!=rmp->ncol) {
	DDerror (ERR_NON_SQUARE_MATRIX,
		    "DoPatch", DcNullPtr);
	return ((DtObject)dor_null_create(DcNullPtr));
    }


    new=(struct patch *)dor_space_allocate(sizeof(*new));
    new->patch_type=dod_patch_type_bezpat;
    dor_global_subdivspc_get_value (&new->subdivtype, &new->subdivlvl[0]);
    new->edge0flat=DcFalse;
    new->edge1flat=DcFalse;
    new->edge2flat=DcFalse;
    new->edge3flat=DcFalse;
    new->corner00flat=DcFalse;
    new->corner03flat=DcFalse;
    new->corner30flat=DcFalse;
    new->corner33flat=DcFalse;
    new->has_colors=DcFalse;
    new->colormodel=colormodel;
    new->vertextype=vertextype;
    new->colors=NULL;
    new->points=(struct patch_control *)dor_space_allocate(sizeof(*new->points));

    if (vertextype==DcLocClr) {
	register int k;
	register DtNPoint3 *pc,*p,*c;
	new->has_colors=DcTrue;
	new->colors=(struct patch_control *)dor_space_allocate(sizeof(*new->colors));
	pc=vertices;
	p= &new->points->point[0][0];
	c= &new->colors->point[0][0];
	for (k=16; --k>=0; ) {
	    *p++= *pc++; *c++= *pc++;
	}
    } else if (vertextype==DcLoc) {
	bcopy(vertices,
	      &new->points->point[0][0],
	      sizeof(new->points->point));
    }

    new->lmatrix=lmatrix;
    new->rmatrix=rmatrix;
    if (((dot_object *)DcBezier4)!=lmatrix || 
	((dot_object *)DcBezier4)!=rmatrix) {

	dor_patch_bezconv(lmp,&new->points->point[0][0],rmp);

	if (new->has_colors) {
	    dor_patch_bezconv(lmp,&new->colors->point[0][0],rmp);
	}
    }

    new->alternate_object = NULL;

    return(dor_object_create(doe_patch_class_id, new));
}

/*
 ======================================================================
 */

void dor_patch_destroy (dot_object *object)
{
    struct patch *patch;

    patch = (struct patch *)(object->data);

    if (patch->points != NULL){
	dor_space_deallocate(patch->points);
    }

    if (patch->colors != NULL){
	dor_space_deallocate(patch->colors);
    }

    if (patch->alternate_object != NULL){
	dor_object_check_deletion(patch->alternate_object);
    }

    dor_space_deallocate(patch);
}
/*
 ======================================================================
 */

void dor_patch_print (dot_object *object)
{
    struct patch *ptch;
  
    ptch = (struct patch *)object->data;
  
    if (ptch == NULL){
	dor_print_output("patch is NULL");
	return;  
    }
}
/*
 ======================================================================
 */

dot_object *dor_patch_update_alternate (dot_object *object)
{
    struct patch *patch;
    DtFlag changed;
    dot_object *alt;

    patch = (struct patch *)(object->data);

    changed = dor_patch_check_subdiv(patch);

    if ((patch->alternate_object != DcNullPtr) && !changed) {
	alt = patch->alternate_object;
    } else {
	if (patch->alternate_object != DcNullPtr) {
	    dor_object_get_method(patch->alternate_object,
				  DcMethodDestroy)(patch->alternate_object);

	    patch->alternate_object = DcNullPtr;
	}

	patch->alternate_object = dor_patch_create_alternate_geometry(patch);
	alt = patch->alternate_object;

	if (patch->alternate_object == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_patch_update_alternate", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_patch_create_alternate_geometry (struct patch *patch)
{
    dot_object *trilstobj;

    if (patch->vertextype == DcLocClr) {
	trilstobj = dor_trilst_open(DcRGB,DcLocNrmClr,500);
	dor_patch_tessellate (patch,
			      (DtShort) dod_patch_tessellatetype_mesh,
			      dor_patch_create_alternate_geometry_clr_triangle,
			      trilstobj);
    } else {
	trilstobj = dor_trilst_open(DcRGB,DcLocNrm,500);
	dor_patch_tessellate(patch,
			     (DtShort) dod_patch_tessellatetype_mesh,
			     dor_patch_create_alternate_geometry_triangle,
			     trilstobj);
    }

    dor_trilst_close (trilstobj);

    return(trilstobj);
}
/*
 ======================================================================
 */

void dor_patch_create_alternate_geometry_clr_triangle (
    dot_object *trilstobj,
    DtNPoint3 *vtx1,
    DtNPoint3 *vtx2,
    DtNPoint3 *vtx3,
    DtNPoint3 *vtxnrm1,
    DtNPoint3 *vtxnrm2,
    DtNPoint3 *vtxnrm3,
    DtNPoint3 *clr1,
    DtNPoint3 *clr2,
    DtNPoint3 *clr3)
{
	DtReal vertices[3][9];

	if (!dor_patch_check_triangle(vtx1,vtx2,vtx3))
		return;

	vertices[0][0] = vtx1->x;
	vertices[0][1] = vtx1->y;
	vertices[0][2] = vtx1->z;
	vertices[1][0] = vtx2->x;
	vertices[1][1] = vtx2->y;
	vertices[1][2] = vtx2->z;
	vertices[2][0] = vtx3->x;
	vertices[2][1] = vtx3->y;
	vertices[2][2] = vtx3->z;

	dor_math_renormalize_fast ((DtReal*)vtxnrm1);
	dor_math_renormalize_fast ((DtReal*)vtxnrm2);
	dor_math_renormalize_fast ((DtReal*)vtxnrm3);

	vertices[0][3] = -vtxnrm1->x;
	vertices[0][4] = -vtxnrm1->y;
	vertices[0][5] = -vtxnrm1->z;
	vertices[1][3] = -vtxnrm2->x;
	vertices[1][4] = -vtxnrm2->y;
	vertices[1][5] = -vtxnrm2->z;
	vertices[2][3] = -vtxnrm3->x;
	vertices[2][4] = -vtxnrm3->y;
	vertices[2][5] = -vtxnrm3->z;

	vertices[0][6] = clr1->x;
	vertices[0][7] = clr1->y;
	vertices[0][8] = clr1->z;
	vertices[1][6] = clr2->x;
	vertices[1][7] = clr2->y;
	vertices[1][8] = clr2->z;
	vertices[2][6] = clr3->x;
	vertices[2][7] = clr3->y;
	vertices[2][8] = clr3->z;

	dor_trilst_add_triangle (trilstobj, (DtReal*)vertices);
	}
/*
 ======================================================================
 */

void dor_patch_create_alternate_geometry_triangle  (
    dot_object *trilstobj,
    DtNPoint3 *vtx1,
    DtNPoint3 *vtx2,
    DtNPoint3 *vtx3,
    DtNPoint3 *vtxnrm1,
    DtNPoint3 *vtxnrm2,
    DtNPoint3 *vtxnrm3)
{
	DtReal vertices[3][6];

	if (!dor_patch_check_triangle(vtx1,vtx2,vtx3))
		return;

	vertices[0][0] = vtx1->x;
	vertices[0][1] = vtx1->y;
	vertices[0][2] = vtx1->z;
	vertices[1][0] = vtx2->x;
	vertices[1][1] = vtx2->y;
	vertices[1][2] = vtx2->z;
	vertices[2][0] = vtx3->x;
	vertices[2][1] = vtx3->y;
	vertices[2][2] = vtx3->z;

	dor_math_renormalize_fast ((DtReal*)vtxnrm1);
	dor_math_renormalize_fast ((DtReal*)vtxnrm2);
	dor_math_renormalize_fast ((DtReal*)vtxnrm3);

	vertices[0][3] = -vtxnrm1->x;
	vertices[0][4] = -vtxnrm1->y;
	vertices[0][5] = -vtxnrm1->z;
	vertices[1][3] = -vtxnrm2->x;
	vertices[1][4] = -vtxnrm2->y;
	vertices[1][5] = -vtxnrm2->z;
	vertices[2][3] = -vtxnrm3->x;
	vertices[2][4] = -vtxnrm3->y;
	vertices[2][5] = -vtxnrm3->z;

	dor_trilst_add_triangle (trilstobj, (DtReal*)vertices);
}
/*
 ======================================================================
 */

DtFlag dor_patch_check_subdiv (struct patch *patch)
{
    DtFlag returnflag;
    DtInt subdivtype;
    DtReal subdivlvl[5];

    returnflag = DcFalse;

    dor_global_subdivspc_get_value(&subdivtype, subdivlvl);

    if (patch->subdivtype != subdivtype) {
	patch->subdivtype = subdivtype;
	returnflag = DcTrue;
    }

    if (patch->subdivlvl[0] != subdivlvl[0]) {
	patch->subdivlvl[0] = subdivlvl[0];
	returnflag = DcTrue;
    }

    return (returnflag);
}
/*
 ======================================================================
 */

DtFlag dor_patch_check_triangle (
    register DtNPoint3 *vertex1,
    register DtNPoint3 *vertex2,
    register DtNPoint3 *vertex3)
{
    if (vertex1->x == vertex2->x && vertex1->y == vertex2->y &&
	vertex1->z == vertex2->z)
	    return(DcFalse);

    if (vertex1->x == vertex3->x && vertex1->y == vertex3->y &&
	vertex1->z == vertex3->z)
	    return(DcFalse);

    if (vertex2->x == vertex3->x && vertex2->y == vertex3->y &&
	vertex2->z == vertex3->z)
	    return(DcFalse);

    return(DcTrue);
}
/*
 ======================================================================
 */

void dor_patch_compute_bounding_volume (dot_object *object)
{
    struct patch *patch;

    patch = (struct patch *)(object->data);

    if (patch->alternate_object != DcNullPtr) {
   	dor_object_get_method(patch->alternate_object,
			      DcMethodCmpBndVolume) (patch->alternate_object);

   	return;
    } else {
   	patch->alternate_object = 
		dor_patch_create_alternate_geometry(patch);

   	if (patch->alternate_object == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_patch_compute_bounding_volume", "");
	    return;
   	}

   	dor_object_get_method(patch->alternate_object,
			      DcMethodCmpBndVolume) (patch->alternate_object);

   	return;
    }
}
/*
 ======================================================================
 */

void dor_patch_get_center (
    dot_object *object,
    register DtNPoint3 *center)
{
    register int k;
    register DtNPoint3 *dp=
	    &((struct patch *)(object->data))->points->point[0][0];

    center->x = 0.;
    center->y = 0.;
    center->z = 0.;

    for (k=16; --k>=0; ++dp) {
	center->x+=dp->x;
	center->y+=dp->y;
	center->z+=dp->z;
    }

    center->x /= 16.;
    center->y /= 16.;
    center->z /= 16.;
}
/*
 ======================================================================
 */

/* c[d1 by d3] = a[d1 by d2] +.* b[d2 by d3]  */
void dor_patch_matmul (
    DtReal *c,
    DtReal *a,
    DtReal *b,
    int d1,
    int d2,
    int d3)
{
    register int row,col,k;
    DtReal t;
    register DtReal *cp,*ap,*bp;
    int d23=d2*d3;
    cp=c;
    for (row=0; row<d1; a+=d2, ++row) {
	bp=b;
	for (col=0; col<d3; bp+=1, ++col) {
	    ap=a;
	    t=0.0;
	    for (k=0; k<d2; ++k) {
		t+= *ap * *bp;
		ap+=1; bp+=d3;
	    }
	    *cp++=t;
	    bp-=d23;
	}
    }
}

/* b[d2 by d1] = a'[d1 by d2] */
void dor_patch_mattrn (
    DtReal *b,
    DtReal *a,
    int d1,
    int d2)
{
    register int row,col;
    register DtReal *bp;
    for (row=0; row<d1; ++b, ++row) {
	bp=b;
	for (col=0; col<d2; ++col) {
	    *bp= *a;
	    ++a; bp+=d1;
	}
    }
}
/*
 ======================================================================
 */

void dor_patch_pick (dot_object *object)
{
    struct patch *patch;
    DtFlag changed;

    if (!dor_condex_query_pick(doe_patch_class_id))
	    return;

    patch = (struct patch *)(object->data);

    changed = dor_patch_check_subdiv(patch);
    dor_pick_initialize_object(object);

    if ((patch->alternate_object != DcNullPtr) && !changed) {
	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(patch->alternate_object,
			      DcMethodPick)(patch->alternate_object);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    } else {
	if (patch->alternate_object != NULL) {
	    dor_object_get_method(patch->alternate_object,
				  DcMethodDestroy) (patch->alternate_object);

	    patch->alternate_object = NULL;
	}

	patch->alternate_object = dor_patch_create_alternate_geometry(patch);

	if (patch->alternate_object == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_patch_pick", "");
	    return;
	}

	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(patch->alternate_object,
			      DcMethodPick) (patch->alternate_object);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    }
}
/*
 ======================================================================
 */

void dor_patch_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_patch_class_id))
	    return;

    alt = dor_patch_update_alternate(object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
/*
 ======================================================================
 */

void dor_patch_subdivide_adaptive (
    struct patch *patch,
    DtShort tessellatetype,
    DtPFV action,
    DtPtr actionparm)
{
	struct patch patch1, patch2;
	struct patch_control points1,points2;
	struct patch_control colors1,colors2;

	patch1.points= &points1; patch1.colors= &colors1;
	patch2.points= &points2; patch2.colors= &colors2;

	/*  Test the u parameter boundary curves for flatness.  If either curve
	is non-flat, then subdivision in the u direction will be performed.  If
	a boundary tests flat, then it is marked as such, so that if subdivision
	in its direction is later performed, its midpoint for the new patches will
	be computed by linear interpolation, rather than by midpoint subdivision.
	This prevents the "cracking" problem.  */

	if (!dor_patch_test_u_boundaries(patch)) {
		dor_patch_subdivide_along_u(patch,&patch1,&patch2);

		dor_patch_subdivide_adaptive(&patch1, tessellatetype, action,
					     actionparm);
		dor_patch_subdivide_adaptive(&patch2, tessellatetype, action,
					     actionparm);
		return;
	}

	/*  Test the v parameter boundary curves for flatness.  If either curve
	is non-flat, then subdivision in the v direction will be performed.  If
	a boundary tests flat, then it is marked as such, so that if subdivision
	in its direction is later performed, its midpoint for the new patches will
	be computed by linear interpolation, rather than by midpoint subdivision.
	This prevents the "cracking" problem.  */

	if (!dor_patch_test_v_boundaries(patch)) {
		dor_patch_subdivide_along_v(patch,&patch1,&patch2);

		dor_patch_subdivide_adaptive(&patch1, tessellatetype, action,
					     actionparm);
		dor_patch_subdivide_adaptive(&patch2, tessellatetype, action,
					     actionparm);
		return;
	}

	/*  The patch has passed all of its flatness tests, go render it.  */

	dor_patch_tessellate_patch (patch,tessellatetype,action,actionparm);
}
/*
 ======================================================================
 */

void dor_patch_subdivide_fixed (
    struct patch *patch,
    DtInt divisionsremaining,
    DtShort tessellatetype,
    DtPFV action,
    DtPtr actionparm)
{
	struct patch lulvpatch, lurvpatch, rulvpatch, rurvpatch;
	struct patch_control lulvpoint,lurvpoint,rulvpoint,rurvpoint;
	struct patch_control lulvcolor,lurvcolor,rulvcolor,rurvcolor;

	if (--divisionsremaining <= 0) {
		dor_patch_tessellate_patch(patch,tessellatetype,action,actionparm);

		return;
	}

	lulvpatch.points= &lulvpoint; lulvpatch.colors= &lulvcolor;
	lurvpatch.points= &lurvpoint; lurvpatch.colors= &lurvcolor;
	rulvpatch.points= &rulvpoint; rulvpatch.colors= &rulvcolor;
	rurvpatch.points= &rurvpoint; rurvpatch.colors= &rurvcolor;

	dor_patch_subdivide_along_u_and_v(patch,&lulvpatch,&lurvpatch,
		&rulvpatch,&rurvpatch);

	dor_patch_subdivide_fixed(&lulvpatch,divisionsremaining,
		tessellatetype,action,actionparm);
	dor_patch_subdivide_fixed(&lurvpatch,divisionsremaining,
		tessellatetype,action,actionparm);
	dor_patch_subdivide_fixed(&rulvpatch,divisionsremaining,
		tessellatetype,action,actionparm);
	dor_patch_subdivide_fixed(&rurvpatch,divisionsremaining,
		tessellatetype,action,actionparm);
}
/*
 ======================================================================
 */

void dor_patch_subdivide_control_along_u (
    struct patch *pch,
    struct patch_control *op,
    struct patch_control *lp,
    struct patch_control *rp)
{
    register int c,i,j;
    register DtNPoint3
	    (*orig)[4]=op->point,
    (*lu)  [4]=lp->point,
    (*ru)  [4]=rp->point;

    if (pch->corner00flat) lp->corner00=op->corner00;
    if (pch->corner03flat) lp->corner03=op->corner03;
    if (pch->corner30flat) rp->corner30=op->corner30;
    if (pch->corner33flat) rp->corner33=op->corner33;
    if (pch->edge0flat) {
	lp->corner30.x=0.5*(op->corner00.x + op->corner30.x);
	lp->corner30.y=0.5*(op->corner00.y + op->corner30.y);
	lp->corner30.z=0.5*(op->corner00.z + op->corner30.z);
	rp->corner00=lp->corner30;
    }
    if (pch->edge1flat) {
	lp->corner33.x=0.5*(op->corner03.x + op->corner33.x);
	lp->corner33.y=0.5*(op->corner03.y + op->corner33.y);
	lp->corner33.z=0.5*(op->corner03.z + op->corner33.z);
	rp->corner03=lp->corner33;
    }
    bcopy(orig,lu,4*sizeof(*lu));
    for (c=0; c<4; ++c) ru[3][c]=lu[3][c];
    for (i=1; i<4; ++i) {
	for (c=0; c<4; ++c) {
	    for (j=4; --j>=i; ) {
		lu[j][c].x = 0.5*(lu[j-1][c].x+lu[j][c].x);
		lu[j][c].y = 0.5*(lu[j-1][c].y+lu[j][c].y);
		lu[j][c].z = 0.5*(lu[j-1][c].z+lu[j][c].z);
	    }
	    ru[3-i][c] = lu[3][c];
	}
    }
}

/*
 ======================================================================
 */

#ifdef NOTDEF
void dump_patch (
    char *msg,
    struct patch *pch,
    register DtNPoint3 *p)
{
    register int k;
    printf("\n\n%s\npatch at %lx  type %x  matrix %x\n",
	   msg,pch,pch->patch_type,pch->matrix);
    printf("  flat edges %x%x%x%x  flat corners %x%x%x%x\n",
	   pch->edge0flat,pch->edge1flat,pch->edge2flat,pch->edge3flat,
	   pch->corner00flat,pch->corner03flat,pch->corner30flat,pch->corner33flat);
    printf("corners\n");
    for (k=4; --k>=0; ) printf("  (%g %g %g)\n",*p++);
    printf("control points\n");
    for (k=16; --k>=0; ) {
	printf("  (%g %g %g)\n",*p++);
	if (0==(3&k)) printf("\n");
    }
}
#endif NOTDEF
/*
 ======================================================================
 */

void dor_patch_subdivide_control_along_v (
    struct patch *pch,
    struct patch_control *op,
    struct patch_control *lp,
    struct patch_control *rp)
{
    register int r,i,j;
    DtNPoint3 (*orig)[4]=op->point, (*lv)[4]=lp->point, (*rv)[4]=rp->point;

    if (pch->corner00flat) lp->corner00=op->corner00;
    if (pch->corner30flat) lp->corner30=op->corner30;
    if (pch->corner03flat) rp->corner03=op->corner03;
    if (pch->corner33flat) rp->corner33=op->corner33;
    if (pch->edge2flat) {
	lp->corner03.x=0.5*(op->corner00.x + op->corner03.x);
	lp->corner03.y=0.5*(op->corner00.y + op->corner03.y);
	lp->corner03.z=0.5*(op->corner00.z + op->corner03.z);
	rp->corner00=lp->corner03;
    }
    if (pch->edge3flat) {
	lp->corner33.x=0.5*(op->corner30.x + op->corner33.x);
	lp->corner33.y=0.5*(op->corner30.y + op->corner33.y);
	lp->corner33.z=0.5*(op->corner30.z + op->corner33.z);
	rp->corner30=lp->corner33;
    }
    bcopy(orig,lv,4*sizeof(*lv)); for (r=0; r<4; ++r) rv[r][3]=lv[r][3];
    for (i=1; i<4; ++i) {
	for (r=0; r<4; ++r) {
	    register DtNPoint3 *lp=lv[r];
	    for (j=4; --j>=i; ) {
		lp[j].x = 0.5*(lp[j-1].x+lp[j].x);
		lp[j].y = 0.5*(lp[j-1].y+lp[j].y);
		lp[j].z = 0.5*(lp[j-1].z+lp[j].z);
	    }
	    rv[r][3-i] = lp[3];
	}
    }
}
/*
 ======================================================================
 */

void dor_patch_subdivide_along_u (
    register struct patch *orig,
    register struct patch *lu,
    register struct patch *ru)
{
    lu->patch_type=orig->patch_type;     ru->patch_type=orig->patch_type;
    lu->subdivtype=orig->subdivtype;     ru->subdivtype=orig->subdivtype;
    lu->subdivlvl[0]=orig->subdivlvl[0];     
    ru->subdivlvl[0]=orig->subdivlvl[0];
    lu->lmatrix=orig->lmatrix;           ru->lmatrix=orig->lmatrix;
    lu->rmatrix=orig->rmatrix;           ru->rmatrix=orig->rmatrix;
    lu->has_colors=orig->has_colors;     ru->has_colors=orig->has_colors;

    lu->edge0flat=orig->edge0flat;       ru->edge0flat=orig->edge0flat;
    lu->edge1flat=orig->edge1flat;       ru->edge1flat=orig->edge1flat;

    lu->edge2flat=orig->edge2flat;       ru->edge3flat=orig->edge3flat;
    lu->edge3flat=DcFalse;               ru->edge2flat=DcFalse;

    lu->corner00flat=orig->corner00flat; lu->corner30flat=orig->edge0flat;
    lu->corner03flat=orig->corner03flat; lu->corner33flat=orig->edge1flat;
    ru->corner30flat=orig->corner30flat; ru->corner00flat=orig->edge0flat;
    ru->corner33flat=orig->corner33flat; ru->corner03flat=orig->edge1flat;
	
    if (orig->has_colors) {
	dor_patch_subdivide_control_along_u(orig,orig->colors,
					    lu->colors,ru->colors);
    }

    dor_patch_subdivide_control_along_u(orig,orig->points,
					lu->points,ru->points);
}
/*
 ======================================================================
 */

void dor_patch_subdivide_along_u_and_v (
    struct patch *orig,
    struct patch *lulv,
    struct patch *lurv,
    struct patch *rulv,
    struct patch *rurv)
{
    struct patch_control pointslu,pointsru;
    struct patch_control colorslu,colorsru;
    struct patch templu, tempru;

    templu.points= &pointslu; templu.colors= &colorslu;
    tempru.points= &pointsru; tempru.colors= &colorsru;

    dor_patch_subdivide_along_u(orig,&templu,&tempru);
    dor_patch_subdivide_along_v(&templu,lulv,lurv);
    dor_patch_subdivide_along_v(&tempru,rulv,rurv);
}
/*
 ======================================================================
 */

void dor_patch_subdivide_along_v (
    register struct patch *orig,
    register struct patch *lv,
    register struct patch *rv)
{
    lv->patch_type=orig->patch_type;     rv->patch_type=orig->patch_type;
    lv->subdivtype=orig->subdivtype;     rv->subdivtype=orig->subdivtype;
    lv->subdivlvl[0]=orig->subdivlvl[0];     
    rv->subdivlvl[0]=orig->subdivlvl[0];
    lv->lmatrix=orig->lmatrix;           rv->lmatrix=orig->lmatrix;
    lv->rmatrix=orig->rmatrix;           rv->rmatrix=orig->rmatrix;
    lv->has_colors=orig->has_colors;     rv->has_colors=orig->has_colors;

    lv->edge2flat=orig->edge2flat;       rv->edge2flat=orig->edge2flat;
    lv->edge3flat=orig->edge3flat;       rv->edge3flat=orig->edge3flat;

    lv->edge0flat=orig->edge0flat;       rv->edge1flat=orig->edge1flat;
    lv->edge1flat=DcFalse;               rv->edge0flat=DcFalse;

    lv->corner00flat=orig->corner00flat; lv->corner03flat=orig->edge2flat;
    lv->corner30flat=orig->corner30flat; lv->corner33flat=orig->edge3flat;
    rv->corner03flat=orig->corner03flat; rv->corner00flat=orig->edge2flat;
    rv->corner33flat=orig->corner33flat; rv->corner30flat=orig->edge3flat;

    if (orig->has_colors) {
	dor_patch_subdivide_control_along_v(orig,orig->colors,
					    lv->colors,rv->colors);
    }

    dor_patch_subdivide_control_along_v(orig,orig->points,
					lv->points,rv->points);
}
/*
 ======================================================================
 */

void dor_patch_tessellate (
    register struct patch *patch,
    DtShort tessellatetype,
    DtPFV action,
    DtPtr actionparm)
{
   DtInt lo, hi;
   DtReal mid;
   DtReal logvalue;

   if (patch->subdivlvl[0]<=0.0) {
   	DDerror (ERR_ZERO_SUBDIVSPEC,
   		    "dor_patch_tessellate", DcNullPtr);
   }

   patch->edge0flat = DcFalse;
   patch->edge1flat = DcFalse;
   patch->edge2flat = DcFalse;
   patch->edge3flat = DcFalse;
   patch->corner00flat = DcFalse;
   patch->corner03flat = DcFalse;
   patch->corner30flat = DcFalse;
   patch->corner33flat = DcFalse;

   switch(patch->subdivtype) {
   	int k;
   case DcSubDivSegments:
   	/* Will use fixed subdivision with k that results in 
   	   the number of segments closest to specified level. */ 
   	if (0>=(k=patch->subdivlvl[0])) {
   		k=3;
   	} else {
   		logvalue = dor_math_log(patch->subdivlvl[0])/M_LN2;
   		lo = dor_math_floor(logvalue);
   		if ((logvalue - lo) < .000001) {
   			k = lo+1;
   		} else {
   			hi = dor_math_ceil(logvalue);
			mid = (dor_math_pow(2.,(DtReal)lo) + 
				dor_math_pow(2.,(DtReal)hi)) / 2.;
   			if (patch->subdivlvl[0] > mid)
   				k = hi+1;
   			else
   				k = lo+1;
   		}
   	}
   	dor_patch_subdivide_fixed(patch,k,
   		tessellatetype,action,actionparm);
   	break;
   case DcSubDivFixed:
   	if (0>=(k=patch->subdivlvl[0]))
   		k=3;
   	dor_patch_subdivide_fixed(patch,k,
   		tessellatetype,action,actionparm);
   	break;
   case DcSubDivAbsolute:
   case DcSubDivRelative:
   	if (patch->subdivlvl[0]<=0.0)
   		patch->subdivlvl[0]=0.03;
   	dor_patch_subdivide_adaptive(patch,
   		tessellatetype,action,actionparm);
   	break;
   }
}
/*
 ======================================================================
 */

/*  This routine tessellates a single patch, without further subdividing it.  
    In this algorithm, the patch is broken up into two triangles, whose vertices 
    are taken from the corner points of the Bezier patch control tensors.  
    Triangles are used because they are always planar.  */

void dor_patch_tessellate_patch (
    register struct patch *patch,
    DtShort tessellatetype,
    DtPFV action,
    DtPtr actionparm)
{
   register struct patch_control *pp=patch->points;
   register struct patch_control *pc=patch->colors;
   DtNPoint3 *vertex00,*vertex30,*vertex33,*vertex03;
   DtNPoint3 vertex00cosine;
   DtNPoint3 vertex30cosine;
   DtNPoint3 vertex33cosine;
   DtNPoint3 vertex03cosine;
   DtNPoint3 *clr00, *clr30, *clr33, *clr03;
   
   /*  Assign the vertex locations and calculate the vertex cosines.   */

   if (patch->corner00flat) {
	vertex00 = &pp->corner00;
   	if (patch->has_colors)
		clr00 = &pc->corner00;
   } else {	
	vertex00 = &pp->point[0][0];
   	if (patch->has_colors)
		clr00 = &pc->point[0][0];
   }


   if (tessellatetype == dod_patch_tessellatetype_mesh) {
	dor_math_cross_cosines(
		pp->point[0][1].x,pp->point[0][1].y,pp->point[0][1].z,
		pp->point[0][0].x,pp->point[0][0].y,pp->point[0][0].z,
		pp->point[1][0].x,pp->point[1][0].y,pp->point[1][0].z,
		&vertex00cosine.x,&vertex00cosine.y,&vertex00cosine.z);
   }

   if (patch->corner30flat) {
	vertex30 = &pp->corner30;
   	if (patch->has_colors)
		clr30 = &pc->corner30;
   } else {	
	vertex30 = &pp->point[3][0];
   	if (patch->has_colors)
		clr30 = &pc->point[3][0];
   }


   if (tessellatetype == dod_patch_tessellatetype_mesh) {
	dor_math_cross_cosines(
		pp->point[2][0].x,pp->point[2][0].y,pp->point[2][0].z,
		pp->point[3][0].x,pp->point[3][0].y,pp->point[3][0].z,
		pp->point[3][1].x,pp->point[3][1].y,pp->point[3][1].z,
		&vertex30cosine.x,&vertex30cosine.y,&vertex30cosine.z);
   }

   if (patch->corner33flat) {
	vertex33 = &pp->corner33;
   	if (patch->has_colors)
		clr33 = &pc->corner33;
   } else {	
	vertex33 = &pp->point[3][3];
   	if (patch->has_colors)
		clr33 = &pc->point[3][3];
   }


   if (tessellatetype == dod_patch_tessellatetype_mesh) {
	dor_math_cross_cosines(
		pp->point[3][2].x,pp->point[3][2].y,pp->point[3][2].z,
		pp->point[3][3].x,pp->point[3][3].y,pp->point[3][3].z,
		pp->point[2][3].x,pp->point[2][3].y,pp->point[2][3].z,
		&vertex33cosine.x,&vertex33cosine.y,&vertex33cosine.z);
   }
   
   if (patch->corner03flat) {
	vertex03 = &pp->corner03;
   	if (patch->has_colors)
		clr03 = &pc->corner03;
   } else {	
	vertex03 = &pp->point[0][3];
   	if (patch->has_colors)
		clr03 = &pc->point[0][3];
   }


   if (tessellatetype == dod_patch_tessellatetype_mesh) {
	dor_math_cross_cosines(
		pp->point[1][3].x,pp->point[1][3].y,pp->point[1][3].z,
		pp->point[0][3].x,pp->point[0][3].y,pp->point[0][3].z,
		pp->point[0][2].x,pp->point[0][2].y,pp->point[0][2].z,
		&vertex03cosine.x,&vertex03cosine.y,&vertex03cosine.z);
   }

   if (tessellatetype == dod_patch_tessellatetype_mesh) {
	if (patch->has_colors) {
		(*action)(actionparm,
			vertex00,vertex03,vertex33,
			&vertex00cosine,&vertex03cosine,&vertex33cosine,
			clr00,clr03,clr33);
		(*action)(actionparm,
			vertex00,vertex33,vertex30,
			&vertex00cosine,&vertex33cosine,&vertex30cosine,
			clr00,clr33,clr30);
	} else {
		(*action)(actionparm,
			vertex00,vertex03,vertex33,
			&vertex00cosine,&vertex03cosine,&vertex33cosine);
		(*action)(actionparm,
			vertex00,vertex33,vertex30,
			&vertex00cosine,&vertex33cosine,&vertex30cosine);
	}
   } else {
	if (patch->has_colors) {
		(*action)(actionparm,
			vertex00,vertex03,vertex33,
			clr00,clr03,clr33);
		(*action)(actionparm,
			vertex00,vertex33,vertex30,
			clr00,clr33,clr30);
	} else {
		(*action)(actionparm,
			vertex00,vertex03,vertex33);
		(*action)(actionparm,
			vertex00,vertex33,vertex30);
	}
   }
}

/*
 ======================================================================
 */

/*  Test if the linear sequence of control vertices is flat.  */

DtFlag dor_patch_test_linear  (
    register DtNPoint3 points[4],
    DtInt subdivtype,
    DtReal *subdivlvl)
{
    DtReal pntlnedst;
    DtReal d=subdivlvl[0];

    if (DcSubDivRelative==subdivtype)
	    d*=dor_math_distance(points[0].x,points[0].y,points[0].z,
				 points[3].x,points[3].y,points[3].z);

    /*  Compute the distance between the second point in the curve and
	the line drawn between the endpoints of the curve.  */

    pntlnedst = dor_math_point_line_distance(points[1].x,points[1].y,points[1].z,
					     points[0].x,points[0].y,points[0].z,
					     points[3].x,points[3].y,points[3].z);

    if (dor_math_fabs(pntlnedst) > d) return(DcFalse);

    /*  Compute the distance between the third point in the curve and
	the line drawn between the endpoints of the curve.  */

    pntlnedst = dor_math_point_line_distance(points[2].x,points[2].y,points[2].z,
					     points[0].x,points[0].y,points[0].z,
					     points[3].x,points[3].y,points[3].z);

    if (dor_math_fabs(pntlnedst) > d) return(DcFalse);

    return(DcTrue);
}
/*
 ======================================================================
 */

/*  Test the patch for flatness of its u parameter boundaries.  If it is flat 
    in both of its boundaries, return true, otherwise return false.  In addition, 
    as part of "cracking" prevention, mark boundaries which are flat.  If a 
    boundary has already been marked as flat, do not test it again.  */

DtFlag dor_patch_test_u_boundaries (struct patch *patch)
{
    DtNPoint3 points[4];

    if (!patch->edge0flat) {
	points[0] = patch->points->point[0][0];
	points[1] = patch->points->point[1][0];
	points[2] = patch->points->point[2][0];
	points[3] = patch->points->point[3][0];

	patch->edge0flat = dor_patch_test_linear
		(points,patch->subdivtype,patch->subdivlvl);
	if (patch->edge0flat) {
	    if (patch->has_colors) {
		if (!patch->corner00flat)
			patch->colors->corner00=patch->colors->point[0][0];
		if (!patch->corner30flat)
			patch->colors->corner30=patch->colors->point[3][0];
	    }
	    if (!patch->corner00flat) {
		patch->corner00flat=DcTrue;
		patch->points->corner00=patch->points->point[0][0];
	    }
	    if (!patch->corner30flat) {
		patch->corner30flat=DcTrue;
		patch->points->corner30=patch->points->point[3][0];
	    }
	}
    }

    if (!patch->edge1flat) {
	points[0] = patch->points->point[0][3];
	points[1] = patch->points->point[1][3];
	points[2] = patch->points->point[2][3];
	points[3] = patch->points->point[3][3];

	patch->edge1flat = dor_patch_test_linear
			(points,patch->subdivtype, patch->subdivlvl);
	if (patch->edge1flat) { 
	    if (patch->has_colors) {
		if (!patch->corner03flat)
			patch->colors->corner03=patch->colors->point[0][3];
		if (!patch->corner33flat)
			patch->colors->corner33=patch->colors->point[3][3];
	    }
	    if (!patch->corner03flat) {
		patch->corner03flat=DcTrue;
		patch->points->corner03=patch->points->point[0][3];
	    }
	    if (!patch->corner33flat) {
		patch->corner33flat=DcTrue;
		patch->points->corner33=patch->points->point[3][3];
	    }
	}
    }

    /* return DcTrue only if both edges and the two interior curves
       are flat */

    if (patch->edge0flat && patch->edge1flat) {
	points[0] = patch->points->point[0][1];
	points[1] = patch->points->point[1][1];
	points[2] = patch->points->point[2][1];
	points[3] = patch->points->point[3][1];

	if (dor_patch_test_linear(points,patch->subdivtype, patch->subdivlvl)) {
	    points[0] = patch->points->point[0][2];
	    points[1] = patch->points->point[1][2];
	    points[2] = patch->points->point[2][2];
	    points[3] = patch->points->point[3][2];

	    if (dor_patch_test_linear(points,patch->subdivtype, patch->subdivlvl)) {
		return(DcTrue);
	    }
	}
    }
    return (DcFalse);
}
/*
 ======================================================================
 */

/*  Test the patch for flatness of its v parameter boundaries.  If it is flat 
    in both of its boundaries, return true, otherwise return false.  In addition, 
    as part of "cracking" prevention, mark boundaries which are flat.  If a 
    boundary has already been marked as flat, do not test it again.  */

DtFlag dor_patch_test_v_boundaries (register struct patch *patch)
{
    DtNPoint3 points[4];

    if (!patch->edge2flat) {
	points[0] = patch->points->point[0][0];
	points[1] = patch->points->point[0][1];
	points[2] = patch->points->point[0][2];
	points[3] = patch->points->point[0][3];

	patch->edge2flat = dor_patch_test_linear
			(points,patch->subdivtype,patch->subdivlvl);
	if (patch->edge2flat) {
	    if (patch->has_colors) {
		if (!patch->corner00flat)
			patch->colors->corner00=patch->colors->point[0][0];
		if (!patch->corner03flat)
			patch->colors->corner03=patch->colors->point[0][3];
	    }
	    if (!patch->corner00flat) {
		patch->corner00flat=DcTrue;
		patch->points->corner00=patch->points->point[0][0];
	    }
	    if (!patch->corner03flat) {
		patch->corner03flat=DcTrue;
		patch->points->corner03=patch->points->point[0][3];
	    }
	}
    }

    if (!patch->edge3flat) {
	points[0] = patch->points->point[3][0];
	points[1] = patch->points->point[3][1];
	points[2] = patch->points->point[3][2];
	points[3] = patch->points->point[3][3];

	patch->edge3flat = dor_patch_test_linear
			(points,patch->subdivtype,patch->subdivlvl);
	if (patch->edge3flat) {
	    if (patch->has_colors) {
		if (!patch->corner30flat)
			patch->colors->corner30=patch->colors->point[3][0];
		if (!patch->corner33flat)
			patch->colors->corner33=patch->colors->point[3][3];
	    }
	    if (!patch->corner30flat) {
		patch->corner30flat=DcTrue;
		patch->points->corner30=patch->points->point[3][0];
	    }
	    if (!patch->corner33flat) {
		patch->corner33flat=DcTrue;
		patch->points->corner33=patch->points->point[3][3];
	    }
	}
    }

    /* Return DcTrue only if both edges and the two interior curves are flat */

    if (patch->edge2flat && patch->edge3flat) {
	points[0] = patch->points->point[1][0];
	points[1] = patch->points->point[1][1];
	points[2] = patch->points->point[1][2];
	points[3] = patch->points->point[1][3];

	if (dor_patch_test_linear(points,patch->subdivtype, patch->subdivlvl)) {
	    points[0] = patch->points->point[2][0];
	    points[1] = patch->points->point[2][1];
	    points[2] = patch->points->point[2][2];
	    points[3] = patch->points->point[2][3];

	    if (dor_patch_test_linear(points,patch->subdivtype, patch->subdivlvl)) {
		return(DcTrue);
	    }
	}
    }

    return (DcFalse);
}
