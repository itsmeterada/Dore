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
	drr_glbrnd_prisrf_initialize_objects
	drr_glbrnd_prisrf_rayint
	drr_glbrnd_prisrf_world_boundary
	drr_glbrnd_prisrf_overlap_spacebox
	drr_glbrnd_prisrf_userdata
	drr_glbrnd_prisrf_wcsloc
	drr_glbrnd_prisrf_wcsnrm
	drr_glbrnd_prisrf_bound_lcsbox
	drr_glbrnd_prisrf_calculate_box_limits
	drr_glbrnd_prisrf_intersect_edge_box

  ======================================================================
 */
#include "../../glbrnd.h"
#include <dore/internal/matrix.h>
#include <dore/dore_develop/private/prisrf.h>

/*
 ======================================================================
 */

void drr_glbrnd_prisrf_initialize_objects (dot_object *object)
{

    drr_glbrnd_initialize_object_modifier(DcTrue);

    drr_glbrnd_initialize_object(object);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_rayint (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect)
{
    struct prisrf *prisrf;

    prisrf = (struct prisrf *)(object->data);

    switch(prisrf->surface_type) {
    case DcSphere:
	return(drr_glbrnd_prisrf_rayint_sphere(prisrf,ray,intersect));
	break;

    case DcBox:
	return(drr_glbrnd_prisrf_rayint_box(prisrf,ray,intersect));
	break;

    case DcCylinder:
	return(drr_glbrnd_prisrf_rayint_cylinder(prisrf,ray,intersect));
	break;

    case DcCone:
	return(drr_glbrnd_prisrf_rayint_cone(prisrf,ray,intersect));
	break;

    default:
	sprintf(DDerror_GetLine(),
		"type %d", prisrf->surface_type);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "drr_glbrnd_prisrf_rayint", 
		    DDerror_GetLine());
	return(DcFalse);
	break;
    }

    return(DcFalse);
}
/*
 ======================================================================
 */

void drr_glbrnd_prisrf_world_boundary (
    dot_object *object,
    DtMatrix4x4 lcstowcsmat,
    DtReal *xmin,
    DtReal *ymin,
    DtReal *zmin,
    DtReal *xmax,
    DtReal *ymax,
    DtReal *zmax)
{
    struct prisrf *prisrf;

    if (lcstowcsmat == NULL) {
    	DDerror (ERR_NULL_LCSTOWCS,
    		    "drr_glbrnd_prisrf_world_boundary", 
		    DcNullPtr);
    	return;
    }

    prisrf = (struct prisrf *)(object->data);

    switch(prisrf->surface_type) {
    	case DcSphere:
    	    drr_glbrnd_prisrf_world_boundary_sphere(lcstowcsmat,
    				xmin,ymin,zmin,xmax,ymax,zmax);
    	    break;

    	case DcBox:
    	    drr_glbrnd_prisrf_world_boundary_box(lcstowcsmat,
    				xmin,ymin,zmin,xmax,ymax,zmax);
    	    break;

    	case DcCylinder:
    	    drr_glbrnd_prisrf_world_boundary_cylinder(lcstowcsmat,
    				xmin,ymin,zmin,xmax,ymax,zmax);
    	    break;

    	case DcCone:
    	    drr_glbrnd_prisrf_world_boundary_cone(lcstowcsmat,
    				xmin,ymin,zmin,xmax,ymax,zmax);
    	    break;
    	default:
    	    sprintf(DDerror_GetLine(),
    		    "prisrf type %d",
    		    prisrf->surface_type);
    	    DDerror (ERR_VALUE_OUT_OF_RANGE,
    		    "drr_glbrnd_prisrf_world_boundary", 
    		    DDerror_GetLine());
    	    break;
    }
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_overlap_spacebox (
    dot_object *object,
    DtMatrix4x4 *lcstowcsmat,
    DtReal boxminmax[6])
{
    struct prisrf *prisrf;
    DtFlag overlap;

    if (lcstowcsmat == NULL) {
   	DDerror (ERR_NULL_LCSTOWCS,
   		    "drr_glbrnd_prisrf_overlap_spacebox", DcNullPtr);
   	return(DcFalse);
    }

    prisrf = (struct prisrf *)(object->data);

    switch(prisrf->surface_type) {
    case DcSphere:
	overlap = drr_glbrnd_prisrf_overlap_spacebox_sphere
		      (*lcstowcsmat, boxminmax);
	break;

    case DcBox:
	overlap = drr_glbrnd_prisrf_overlap_spacebox_box
		      (*lcstowcsmat, boxminmax);
	break;

    case DcCylinder:
	overlap = drr_glbrnd_prisrf_overlap_spacebox_cylinder
		      (*lcstowcsmat, boxminmax);
	break;

    case DcCone:
	overlap = drr_glbrnd_prisrf_overlap_spacebox_cone
		      (*lcstowcsmat, boxminmax);
	break;

    default:
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "drr_glbrnd_prisrf_overlap_spacebox", 
		    DcNullPtr);
	return(DcFalse);
	break;
    }

    return(overlap);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_userdata (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect,
    DtReal userdata[3])
{
    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_wcsloc (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect,
    DtReal wcsloc[3])
{
    struct rayint_object_modifier *rayintobjectmodifier;

    rayintobjectmodifier = intersect->rayint_object->modifier;

    if (rayintobjectmodifier->lcstowcsmat != NULL) {
   	/*  Transform the point of intersection into the world
	    coordinate system.  */

   	dor_math_pnttrns(intersect->lcsloc[0],intersect->lcsloc[1],intersect->lcsloc[2],
			 *(rayintobjectmodifier->lcstowcsmat),
			 &(wcsloc[0]),&(wcsloc[1]),&(wcsloc[2]));
    } else {
   	DDerror (ERR_NULL_LCSTOWCS,
   		    "drr_glbrnd_prisrf_wcsloc", DcNullPtr);

   	return(DcFalse);
    }

    return(DcTrue);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_wcsnrm (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect,
    DtReal wcsnrm[3])
{
    struct rayint_object_modifier *rayintobjectmodifier;
    DtReal dotproduct;

    rayintobjectmodifier = intersect->rayint_object->modifier;

    if (rayintobjectmodifier->lcstowcsmat != NULL) {
   	/*  Convert the directions cosines of the
	    surface normal at the point of intersection
	    from the object's lcs to the wcs.  */

   	dor_math_vectrns_fast(intersect->lcsnrm,
			      *(rayintobjectmodifier->lcstowcsmat),wcsnrm);

   	/*  Renormalize the direction cosines of the
	    transformed normal to take into account possible
	    scaling effects.  */

   	dor_math_renormalize_fast(wcsnrm);
    } else {
   	DDerror (ERR_NULL_LCSTOWCS,
   		    "drr_glbrnd_prisrf_wcsnrm", DcNullPtr);

   	return(DcFalse);
    }

    /*  Find the dot product of the ray direction and the surface
	normal.  If this value is positive, then the ray is viewing the
	back part of the surface, and its intersection normal value
	must be reversed for the subsequent shading calculation.  */

    dotproduct = ray->wcsdirection[0]*wcsnrm[0] + 
	    ray->wcsdirection[1]*wcsnrm[1] +
		    ray->wcsdirection[2]*wcsnrm[2];

    if (dotproduct > 0.) {
   	wcsnrm[0] = -wcsnrm[0];
   	wcsnrm[1] = -wcsnrm[1];
   	wcsnrm[2] = -wcsnrm[2];
    }

    return(DcTrue);
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void drr_glbrnd_prisrf_bound_lcsbox (
    DtMatrix4x4 lcstowcsmat,
    DtReal  boxxmin,
    DtReal  boxymin,
    DtReal  boxzmin,
    DtReal  boxxmax,
    DtReal  boxymax,
    DtReal  boxzmax,
    DtReal *wcsxmin,
    DtReal *wcsymin,
    DtReal *wcszmin,
    DtReal *wcsxmax,
    DtReal *wcsymax,
    DtReal *wcszmax)
{
    DtReal lcspts[8][3];
    DtReal wcspts[8][3];
    DtInt pointno;
    DtInt row, column;
    DtReal newpoint[8][3];

    lcspts[0][0] = boxxmin;
    lcspts[0][1] = boxymin;
    lcspts[0][2] = boxzmin;

    lcspts[1][0] = boxxmin;
    lcspts[1][1] = boxymin;
    lcspts[1][2] = boxzmax;

    lcspts[2][0] = boxxmin;
    lcspts[2][1] = boxymax;
    lcspts[2][2] = boxzmin;

    lcspts[3][0] = boxxmin;
    lcspts[3][1] = boxymax;
    lcspts[3][2] = boxzmax;

    lcspts[4][0] = boxxmax;
    lcspts[4][1] = boxymin;
    lcspts[4][2] = boxzmin;

    lcspts[5][0] = boxxmax;
    lcspts[5][1] = boxymin;
    lcspts[5][2] = boxzmax;

    lcspts[6][0] = boxxmax;
    lcspts[6][1] = boxymax;
    lcspts[6][2] = boxzmin;

    lcspts[7][0] = boxxmax;
    lcspts[7][1] = boxymax;
    lcspts[7][2] = boxzmax;

PRAGMA(IVDEP)
PRAGMA(VBEST)
    for (pointno=0;pointno<8;pointno++) {
    	for (row=0;row<3;row++) {
    	    newpoint[pointno][row] = 0.;

    	    for (column=0;column<3;column++) {
    		newpoint[pointno][row] += lcstowcsmat[row][column]*
    					lcspts[pointno][column];
    	    }

    	    newpoint[pointno][row] += lcstowcsmat[row][3];

    	    wcspts[pointno][row] = newpoint[pointno][row];
    	}
    }

    *wcsxmin = wcspts[0][0];
    *wcsymin = wcspts[0][1];
    *wcszmin = wcspts[0][2];
    *wcsxmax = wcspts[0][0];
    *wcsymax = wcspts[0][1];
    *wcszmax = wcspts[0][2];

    for (pointno=1;pointno<8;pointno++) {
    	*wcsxmin = min(*wcsxmin,wcspts[pointno][0]);
    	*wcsxmax = max(*wcsxmax,wcspts[pointno][0]);
    	*wcsymin = min(*wcsymin,wcspts[pointno][1]);
    	*wcsymax = max(*wcsymax,wcspts[pointno][1]);
    	*wcszmin = min(*wcszmin,wcspts[pointno][2]);
    	*wcszmax = max(*wcszmax,wcspts[pointno][2]);
    }
}
/*
 ======================================================================
 */

void drr_glbrnd_prisrf_calculate_box_limits (
    DtMatrix4x4 lcstowcsmat,
    DtReal boxxmin,
    DtReal boxymin,
    DtReal boxzmin,
    DtReal boxxmax,
    DtReal boxymax,
    DtReal boxzmax,
    DtReal *wcsxmin,
    DtReal *wcsymin,
    DtReal *wcszmin,
    DtReal *wcsxmax,
    DtReal *wcsymax,
    DtReal *wcszmax)
{
    DtReal newx, newy, newz;
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;

    dor_math_pnttrns(boxxmin,boxymin,boxzmin,lcstowcsmat,&newx,&newy,&newz);

    xmin = newx;
    ymin = newy;
    zmin = newz;
    xmax = newx;
    ymax = newy;
    zmax = newz;

    dor_math_pnttrns(boxxmax,boxymin,boxzmin,lcstowcsmat,&newx,&newy,&newz);

    xmin = min(xmin,newx);
    ymin = min(ymin,newy);
    zmin = min(zmin,newz);
    xmax = max(xmax,newx);
    ymax = max(ymax,newy);
    zmax = max(zmax,newz);

    dor_math_pnttrns(boxxmax,boxymax,boxzmin,lcstowcsmat,&newx,&newy,&newz);

    xmin = min(xmin,newx);
    ymin = min(ymin,newy);
    zmin = min(zmin,newz);
    xmax = max(xmax,newx);
    ymax = max(ymax,newy);
    zmax = max(zmax,newz);

    dor_math_pnttrns(boxxmin,boxymax,boxzmin,lcstowcsmat,&newx,&newy,&newz);

    xmin = min(xmin,newx);
    ymin = min(ymin,newy);
    zmin = min(zmin,newz);
    xmax = max(xmax,newx);
    ymax = max(ymax,newy);
    zmax = max(zmax,newz);

    dor_math_pnttrns(boxxmin,boxymin,boxzmax,lcstowcsmat,&newx,&newy,&newz);

    xmin = min(xmin,newx);
    ymin = min(ymin,newy);
    zmin = min(zmin,newz);
    xmax = max(xmax,newx);
    ymax = max(ymax,newy);
    zmax = max(zmax,newz);

    dor_math_pnttrns(boxxmax,boxymin,boxzmax,lcstowcsmat,&newx,&newy,&newz);

    xmin = min(xmin,newx);
    ymin = min(ymin,newy);
    zmin = min(zmin,newz);
    xmax = max(xmax,newx);
    ymax = max(ymax,newy);
    zmax = max(zmax,newz);

    dor_math_pnttrns(boxxmax,boxymax,boxzmax,lcstowcsmat,&newx,&newy,&newz);

    xmin = min(xmin,newx);
    ymin = min(ymin,newy);
    zmin = min(zmin,newz);
    xmax = max(xmax,newx);
    ymax = max(ymax,newy);
    zmax = max(zmax,newz);

    dor_math_pnttrns(boxxmin,boxymax,boxzmax,lcstowcsmat,&newx,&newy,&newz);

    xmin = min(xmin,newx);
    ymin = min(ymin,newy);
    zmin = min(zmin,newz);
    xmax = max(xmax,newx);
    ymax = max(ymax,newy);
    zmax = max(zmax,newz);

    *wcsxmin = xmin;
    *wcsymin = ymin;
    *wcszmin = zmin;
    *wcsxmax = xmax;
    *wcsymax = ymax;
    *wcszmax = zmax;
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_intersect_edge_box (
    DtReal edgex0,
    DtReal edgey0,
    DtReal edgez0,
    DtReal edgexdir,
    DtReal edgeydir,
    DtReal edgezdir,
    DtReal edgelength,
    DtReal boxxmin,
    DtReal boxymin,
    DtReal boxzmin,
    DtReal boxxmax,
    DtReal boxymax,
    DtReal boxzmax)
{
    DtReal t;
    DtReal intersectx, intersecty, intersectz;

/*
    printf("intersecting edge (%f,%f,%f) in direction (%f,%f,%f), and length %f\n",(double)edgex0,(double)edgey0,(double)edgez0,(double)edgexdir,(double)edgeydir,(double)edgezdir,(double)edgelength);
    printf("with box (%f,%f,%f) (%f,%f,%f)\n",(double)boxxmin,(double)boxymin,(double)boxzmin,(double)boxxmax,(double)boxymax,(double)boxzmax);
*/

    if (edgexdir != 0.) {
    /* Test for intersection against the x=boxxmin surface of the box.  */

    	t = (boxxmin-edgex0) / edgexdir;

    	if (t > 0. && t <= edgelength) {
    	    intersecty = edgey0 + t*edgeydir;
    	    if ((intersecty >= boxymin) && (intersecty <= boxymax)) {
    	    	intersectz = edgez0 + t*edgezdir;
    	    	if ((intersectz >= boxzmin) && (intersectz <= boxzmax)) {
    	    	    intersectx = boxxmin;
    	    	    return(1);
    	    	}
    	    }
    	}
    	
  /*  Test for intersection against the x=boxxmax surface of the box.  */
    
    	t = (boxxmax-edgex0) / edgexdir;
    	if (t > 0. && t <= edgelength) {

    	    intersecty = edgey0 + t*edgeydir;
    	    if ((intersecty >= boxymin) && (intersecty <= boxymax)) {

    	    	intersectz = edgez0 + t*edgezdir;
    	    	if ((intersectz >= boxzmin) && (intersectz <= boxzmax)) {
    	    	    intersectx = boxxmax;
    	    	    return(1);
    	    	}
    	    }
    	}
    }

    if (edgeydir != 0.) {

    /*  Test for intersection against the y=boxymin surface of the box.  */

    	t = (boxymin-edgey0) / edgeydir;
    	if (t > 0. && t <= edgelength) {

    	    intersectx = edgex0 + t*edgexdir;
    	    if ((intersectx >= boxxmin) && (intersectx <= boxxmax)) {

    	    	intersectz = edgez0 + t*edgezdir;
    	    	if ((intersectz >= boxzmin) && (intersectz <= boxzmax)) {
    	    	    intersecty = boxymin;
    	    	    return(DcTrue);
    	    	}
    	    }
    	}
    
    /*  Test for intersection against the y=boxymax surface of the box.  */
    
    	t = (boxymax-edgey0) / edgeydir;
    	if (t > 0. && t <= edgelength) {

    	    intersectx = edgex0 + t*edgexdir;
    	    if ((intersectx >= boxxmin) && (intersectx <= boxxmax)) {

    	    	intersectz = edgez0 + t*edgezdir;
    	    	if ((intersectz >= boxzmin) && (intersectz <= boxzmax)) {
    	    	    intersecty = boxymax;
    	    	    return(DcTrue);
    	    	}
    	    }
    	}
    }

    if (edgezdir != 0.) {

    /*  Test for intersection against the z=boxzmin surface of the box.  */

    	t = (boxzmin-edgez0)/edgezdir;
    	if (t > 0. && t <= edgelength) {

    	    intersectx = edgex0 + t*edgexdir;
    	    if ((intersectx >= boxxmin) && (intersectx <= boxxmax)) {

    	    	intersecty = edgey0 + t*edgeydir;
    	    	if ((intersecty >= boxymin) && (intersecty <= boxymax)) {
    	    	    intersectz = boxzmin;
    	    	    return(DcTrue);
    	    	}
    	    }
    	}
    
    /*  Test for intersection against the z=boxzmax surface of the box.  */
    
    	t = (boxzmax-edgez0) / edgezdir;
    	if (t > 0. && t <= edgelength) {

    	    intersectx = edgex0 + t*edgexdir;
    	    if ((intersectx >= boxxmin) && (intersectx <= boxxmax)) {

    	    	intersecty = edgey0 + t*edgeydir;
    	    	if ((intersecty >= boxymin) && (intersecty <= boxymax)) {
    	    	    intersectz = boxzmax;
    	    	    return(DcTrue);
    	    	}
    	    }
    	}
    }

    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_intersect_lcsbox_box (
    DtReal lcsboxxmin,
    DtReal lcsboxymin,
    DtReal lcsboxzmin,
    DtReal lcsboxxmax,
    DtReal lcsboxymax,
    DtReal lcsboxzmax,
    DtMatrix4x4 lcstowcsmat,
    DtReal boxxmin,
    DtReal boxymin,
    DtReal boxzmin,
    DtReal boxxmax,
    DtReal boxymax,
    DtReal boxzmax)
{
    /*   Intersect the front face of the lcs box.			*/

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmin,lcsboxymin,lcsboxzmin,
    	lcsboxxmin,lcsboxymax,lcsboxzmin,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmin,lcsboxymax,lcsboxzmin,
    	lcsboxxmax,lcsboxymax,lcsboxzmin,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmax,lcsboxymax,lcsboxzmin,
    	lcsboxxmax,lcsboxymin,lcsboxzmin,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmax,lcsboxymin,lcsboxzmin,
    	lcsboxxmin,lcsboxymin,lcsboxzmin,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

/*   Intersect the back face of the lcs box.			*/

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmin,lcsboxymin,lcsboxzmax,
    	lcsboxxmin,lcsboxymax,lcsboxzmax,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmin,lcsboxymax,lcsboxzmax,
    	lcsboxxmax,lcsboxymax,lcsboxzmax,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmax,lcsboxymax,lcsboxzmax,
    	lcsboxxmax,lcsboxymin,lcsboxzmax,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmax,lcsboxymin,lcsboxzmax,
    	lcsboxxmin,lcsboxymin,lcsboxzmax,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

/*   Intersect the unintersected edges of the left face.		*/

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmin,lcsboxymax,lcsboxzmin,
    	lcsboxxmin,lcsboxymax,lcsboxzmax,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmin,lcsboxymin,lcsboxzmax,
    	lcsboxxmin,lcsboxymin,lcsboxzmin,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

/*   Intersect the unintersected edges of the right face.		*/

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmax,lcsboxymax,lcsboxzmin,
    	lcsboxxmax,lcsboxymax,lcsboxzmax,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    if (drr_glbrnd_prisrf_intersect_lcsedge_box(
	lcsboxxmax,lcsboxymin,lcsboxzmax,
    	lcsboxxmax,lcsboxymin,lcsboxzmin,lcstowcsmat,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax))
    	return(DcTrue);

    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_intersect_lcsedge_box (
    DtReal lcsedgex0,
    DtReal lcsedgey0,
    DtReal lcsedgez0,
    DtReal lcsedgex1,
    DtReal lcsedgey1,
    DtReal lcsedgez1,
    DtMatrix4x4 lcstowcsmat,
    DtReal boxxmin,
    DtReal boxymin,
    DtReal boxzmin,
    DtReal boxxmax,
    DtReal boxymax,
    DtReal boxzmax)
{
    DtReal wcsedgex0, wcsedgey0, wcsedgez0;
    DtReal wcsedgex1, wcsedgey1, wcsedgez1;
    DtReal wcsedgexdir, wcsedgeydir, wcsedgezdir;
    DtReal wcsedgelength;

    dor_math_pnttrns(lcsedgex0,lcsedgey0,lcsedgez0,
    	lcstowcsmat,&wcsedgex0,&wcsedgey0,&wcsedgez0);

    dor_math_pnttrns(lcsedgex1,lcsedgey1,lcsedgez1,
    	lcstowcsmat,&wcsedgex1,&wcsedgey1,&wcsedgez1);

    dor_math_normal(wcsedgex0,wcsedgey0,wcsedgez0,
    	wcsedgex1,wcsedgey1,wcsedgez1,
    	&wcsedgexdir,&wcsedgeydir,&wcsedgezdir);

    wcsedgelength = dor_math_distance(wcsedgex0,wcsedgey0,wcsedgez0,
    	wcsedgex1,wcsedgey1,wcsedgez1);

    return(drr_glbrnd_prisrf_intersect_edge_box(
	wcsedgex0,wcsedgey0,wcsedgez0,
    	wcsedgexdir,wcsedgeydir,wcsedgezdir,wcsedgelength,
    	boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax));
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_overlap_lcsbox_spacebox (
    DtReal lcsboxxmin,
    DtReal lcsboxymin,
    DtReal lcsboxzmin,
    DtReal lcsboxxmax,
    DtReal lcsboxymax,
    DtReal lcsboxzmax,
    DtMatrix4x4 lcstowcsmat,
    DtReal boxminmax[6])
{
    DtReal wcsbndboxxmin, wcsbndboxymin, wcsbndboxzmin;
    DtReal wcsbndboxxmax, wcsbndboxymax, wcsbndboxzmax;
    DtReal spcboxxmin = boxminmax[0];
    DtReal spcboxymin = boxminmax[1];
    DtReal spcboxzmin = boxminmax[2];
    DtReal spcboxxmax = boxminmax[3];
    DtReal spcboxymax = boxminmax[4];
    DtReal spcboxzmax = boxminmax[5];
    DtMatrix4x4 wcstolcsmat;

/* Transform the lcs box into wcs and calculate its aligned bounding
    box in wcs.								*/

    drr_glbrnd_prisrf_bound_lcsbox(lcstowcsmat,
    	lcsboxxmin,lcsboxymin,lcsboxzmin,lcsboxxmax,lcsboxymax,lcsboxzmax,
    	&wcsbndboxxmin,&wcsbndboxymin,&wcsbndboxzmin,
    	&wcsbndboxxmax,&wcsbndboxymax,&wcsbndboxzmax);

/* First, check to see if the aligned wcs bounding box of the data
    is completely contained within the space box.			*/

    if ((wcsbndboxxmin>=spcboxxmin) && (wcsbndboxymin>=spcboxymin) &&
    	(wcsbndboxzmin>=spcboxzmin) &&
    	(wcsbndboxxmax<=spcboxxmax) && (wcsbndboxymax<=spcboxymax) &&
    	(wcsbndboxzmax<=spcboxzmax)) {
    	return(DcTrue);
    }

/* Next, check to see if the aligned wcs bounding box of the data
    is completely outside of the space box.				*/

    if ((wcsbndboxxmin>spcboxxmax) || (wcsbndboxymin>spcboxymax) ||
    	(wcsbndboxzmin>spcboxzmax) ||
    	(wcsbndboxxmax<spcboxxmin) || (wcsbndboxymax<spcboxymin) ||
    	(wcsbndboxzmax<spcboxzmin)) {
    	return(DcFalse);
    }

/* Next, check to see if the space box is completely inside the aligned
    wcs bounding box.							*/

    if ((spcboxxmin>=wcsbndboxxmin) && (spcboxymin>=wcsbndboxymin) &&
    	(spcboxzmin>=wcsbndboxzmin) &&
    	(spcboxxmax<=wcsbndboxxmax) && (spcboxymax<=wcsbndboxymax) &&
    	(spcboxzmax<=wcsbndboxzmax)) {
    	return(DcTrue);
    }

    if (drr_glbrnd_prisrf_intersect_lcsbox_box(
    	lcsboxxmin,lcsboxymin,lcsboxzmin,lcsboxxmax,lcsboxymax,lcsboxzmax,
    	lcstowcsmat,
    	spcboxxmin,spcboxymin,spcboxzmin,spcboxxmax,spcboxymax,spcboxzmax)) {
    	return(DcTrue);
    }

    dor_matrix_load(wcstolcsmat,lcstowcsmat);
    dor_matrix_invert(wcstolcsmat);

    if (drr_glbrnd_prisrf_intersect_lcsbox_box(
    	spcboxxmin,spcboxymin,spcboxzmin,spcboxxmax,spcboxymax,spcboxzmax,
    	wcstolcsmat,
    	lcsboxxmin,lcsboxymin,lcsboxzmin,lcsboxxmax,lcsboxymax,lcsboxzmax)){
        return(DcTrue);
    }

    return(DcFalse);
}
