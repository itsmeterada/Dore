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
	DtFlag drr_glbrnd_rayintsph_rayint (object,ray,intersect)
	int drr_glbrnd_rayintsph_world_boundary (object,lcstowcsmat,
	    xmin,ymin,zmin, xmax,ymax,zmax)
	DtFlag drr_glbrnd_rayintsph_overlap_spacebox (object,lcstowcsmat, boxminmax)
	DtFlag drr_glbrnd_rayintsph_userdata (object,ray,intersect,userdata)
	DtFlag drr_glbrnd_rayintsph_wcsloc (object,ray,intersect,wcsloc)
	DtFlag drr_glbrnd_rayintsph_wcsnrm (object,ray,intersect,wcsnrm)
	int drr_glbrnd_rayintsph_bounding_box(rayintsph,boxxmin,boxymin,boxzmin, 
	    boxxmax,boxymax,boxzmax)
	DtFlag drr_glbrnd_rayintsph_intersect_box(boxxmin1,boxymin1,boxzmin1, 
	    boxxmax1,boxymax1,boxzmax1, boxxmin2,boxymin2,boxzmin2, 
	    boxxmax2,boxymax2,boxzmax2)
	DtFlag drr_glbrnd_rayintsph_intersect_edge_box(edgex0,edgey0,edgez0, 
	    edgex1,edgey1,edgez1, boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax)

  ======================================================================
 */
#include "../glbrnd.h"
#include "dore/dore_develop/private/rayintsph.h"



/* ====================================================================== */

DtFlag drr_glbrnd_rayintsph_rayint (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect)
{
    struct rayintsph *rayintsph;
    DtReal rayx0, rayy0, rayz0;
    DtReal rayxdir, rayydir, rayzdir;
    DtReal rayx0rel, rayy0rel, rayz0rel;
    DtFlag intersectflag;
    DtReal xinter, yinter, zinter;
    DtReal tinter;
    DtReal xnorm, ynorm, znorm;
    DtReal a, b, c, temp1;
    DtReal sqrttemp1;
    DtReal t;
    DtReal intersect_t;
    DtReal newxinter, newyinter, newzinter;
    DtReal centerx, centery, centerz;
    DtReal radius;

    rayintsph = (struct rayintsph *)(object->data);

    radius = rayintsph->radius;
    centerx = rayintsph->location[0];
    centery = rayintsph->location[1];
    centerz = rayintsph->location[2];

    rayx0 = ray->wcsorigin[0];
    rayy0 = ray->wcsorigin[1];
    rayz0 = ray->wcsorigin[2];

    rayxdir = ray->wcsdirection[0];
    rayydir = ray->wcsdirection[1];
    rayzdir = ray->wcsdirection[2];
 
    /* 
     * Translate the ray origin so that it is relative to the sphere center.
     */
    rayx0rel = rayx0 - centerx;
    rayy0rel = rayy0 - centery;
    rayz0rel = rayz0 - centerz;

    intersectflag = DcFalse;

    a = rayxdir*rayxdir + rayydir*rayydir + rayzdir*rayzdir;
    b = 2. * (rayx0rel*rayxdir+rayy0rel*rayydir+rayz0rel*rayzdir);
    c = rayx0rel*rayx0rel + rayy0rel*rayy0rel + rayz0rel*rayz0rel - radius*radius;

    temp1 = b*b - 4.*a*c;

    /*
     * If temp1 is negative, the ray misses the sphere.  Otherwise,
     * the smaller, positive real root is the closest intersection 
     * distance on the ray.
     */
    if ((temp1 > 0.) && (a != 0.)) {

   	sqrttemp1 = dor_math_sqrt(temp1);

   	t = (-b+sqrttemp1)/ (2.*a);

   	if (t > .000001) {
	    xinter = newxinter = rayx0 + t*rayxdir;
	    yinter = newyinter = rayy0 + t*rayydir;
	    zinter = newzinter = rayz0 + t*rayzdir;

	    intersectflag = DcTrue;

	    intersect_t = tinter = t;

	    xnorm = (newxinter - centerx) / radius;
	    ynorm = (newyinter - centery) / radius;
	    znorm = (newzinter - centerz) / radius;
   	}

   	t = (-b-sqrttemp1)/ (2.*a);

   	if (t > .000001) {
	    newxinter = rayx0 + t*rayxdir;
	    newyinter = rayy0 + t*rayydir;
	    newzinter = rayz0 + t*rayzdir;

	    if (!intersectflag) {
		intersectflag = DcTrue;

		xinter = newxinter;
		yinter = newyinter;
		zinter = newzinter;

		intersect_t = tinter = t;

		xnorm = (newxinter - centerx) / radius;
		ynorm = (newyinter - centery) / radius;
		znorm = (newzinter - centerz) / radius;
	    } else {
		if (t < intersect_t) {
		    xinter = newxinter;
		    yinter = newyinter;
		    zinter = newzinter;

		    tinter = t;

		    xnorm = (newxinter - centerx) / radius;
		    ynorm = (newyinter - centery) / radius;
		    znorm = (newzinter - centerz) / radius;
		}
	    }
   	}
    }

    if (!intersectflag) {

    } else {
   	intersect->lcsloc[0] = xinter;
   	intersect->lcsloc[1] = yinter;
   	intersect->lcsloc[2] = zinter;

   	intersect->t = tinter;

   	intersect->lcsnrm_available = DcTrue;

   	intersect->lcsnrm[0] = xnorm;
   	intersect->lcsnrm[1] = ynorm;
   	intersect->lcsnrm[2] = znorm;
    }

    return(intersectflag);
}
/*
 ======================================================================
 */

void drr_glbrnd_rayintsph_world_boundary (
    dot_object *object,
    DtMatrix4x4 lcstowcsmat,
    DtReal *xmin,
    DtReal *ymin,
    DtReal *zmin,
    DtReal *xmax,
    DtReal *ymax,
    DtReal *zmax)
{
    struct rayintsph *rayintsph;

    if (lcstowcsmat != NULL) {
    	DDerror (ERR_NULL_LCSTOWCS, "drr_glbrnd_rayintsph_world_boundary", 
		    DcNullPtr);
    	return;
    }

    rayintsph = (struct rayintsph *)(object->data);
    
    drr_glbrnd_rayintsph_bounding_box (rayintsph,xmin,ymin,zmin,xmax,ymax,zmax);
}



/* ====================================================================== */

DtFlag drr_glbrnd_rayintsph_overlap_spacebox (
    dot_object *object,
    DtMatrix4x4 *lcstowcsmat,
    DtReal boxminmax[6])
{
    struct rayintsph *rayintsph;
    DtReal bndboxxmin, bndboxymin, bndboxzmin;
    DtReal bndboxxmax, bndboxymax, bndboxzmax;
    DtReal spaceboxxmin = boxminmax[0];
    DtReal spaceboxymin = boxminmax[1];
    DtReal spaceboxzmin = boxminmax[2];
    DtReal spaceboxxmax = boxminmax[3];
    DtReal spaceboxymax = boxminmax[4];
    DtReal spaceboxzmax = boxminmax[5];

    if (lcstowcsmat != NULL) {
   	DDerror (ERR_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayintsph_overlap_spacebox", DcNullPtr);
   	return(DcFalse);
    }

    rayintsph = (struct rayintsph *)(object->data);

    drr_glbrnd_rayintsph_bounding_box(rayintsph, &bndboxxmin,&bndboxymin,&bndboxzmin,
		&bndboxxmax,&bndboxymax,&bndboxzmax);

    /*
     * First, check to see if the bounding box is completely contained 
     * within the space box.  
     */
    if ((bndboxxmin>=spaceboxxmin) && (bndboxymin>=spaceboxymin) &&
        (bndboxzmin>=spaceboxzmin) &&
        (bndboxxmax<=spaceboxxmax) && (bndboxymax<=spaceboxymax) &&
        (bndboxzmax<=spaceboxzmax)) {

        return(DcTrue);
    }

    /*  
     * Next, check to see if the bounding box is completely outside of
     * the space box.  
     */
    if ((bndboxxmin>spaceboxxmax) || (bndboxymin>spaceboxymax) ||
        (bndboxzmin>spaceboxzmax) ||
        (bndboxxmax<spaceboxxmin) || (bndboxymax<spaceboxymin) ||
        (bndboxzmax<spaceboxzmin)) {

        return(DcFalse);
    }

    /* 
     * Next, check to see if the space box is completely inside the
     * aligned wcs bounding box.
     */
    if ((spaceboxxmin>=bndboxxmin) && (spaceboxymin>=bndboxymin) &&
        (spaceboxzmin>=bndboxzmin) &&
        (spaceboxxmax<=bndboxxmax) && (spaceboxymax<=bndboxymax) &&
        (spaceboxzmax<=bndboxzmax)) {
        return(DcTrue);
    }

    /*
     * Check for overlap between space box and bounding box.
     */
    if (drr_glbrnd_rayintsph_intersect_box(
	    bndboxxmin,bndboxymin,bndboxzmin, bndboxxmax,bndboxymax,bndboxzmax, 
	    spaceboxxmin,spaceboxymin,spaceboxzmin, 
	    spaceboxxmax,spaceboxymax,spaceboxzmax)) {
	return(DcTrue);
    }

    if (drr_glbrnd_rayintsph_intersect_box(
	    spaceboxxmin,spaceboxymin,spaceboxzmin, 
	    spaceboxxmax,spaceboxymax,spaceboxzmax,
	    bndboxxmin,bndboxymin,bndboxzmin, bndboxxmax,bndboxymax,bndboxzmax)) { 
	return(DcTrue);
    }

    return(DcFalse);
}



/* ====================================================================== */

DtFlag drr_glbrnd_rayintsph_userdata (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect,
    DtReal userdata[3])
{
    struct rayintsph *rayintsph;

    rayintsph = (struct rayintsph *)(object->data);

    userdata[0] = rayintsph->color[0];
    userdata[1] = rayintsph->color[1];
    userdata[2] = rayintsph->color[2];

    return(DcTrue);
}



/* ====================================================================== */

DtFlag drr_glbrnd_rayintsph_wcsloc (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect,
    DtReal wcsloc[3])
{
    struct rayint_object_modifier *rayintobjectmodifier;

    rayintobjectmodifier = intersect->rayint_object->modifier;

    if (rayintobjectmodifier->lcstowcsmat != NULL) {

   	DDerror (ERR_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayintsph_wcsloc", DcNullPtr);

   	return(DcFalse);
    } else {
	wcsloc[0] = intersect->lcsloc[0];
	wcsloc[1] = intersect->lcsloc[1];
	wcsloc[2] = intersect->lcsloc[2];
    }

    return(DcTrue);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayintsph_wcsnrm (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect,
    DtReal wcsnrm[3])
{
    struct rayint_object_modifier *rayintobjectmodifier;
    DtReal dotproduct;

    rayintobjectmodifier = intersect->rayint_object->modifier;

    if (rayintobjectmodifier->lcstowcsmat != NULL) {

   	DDerror (ERR_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayintsph_wcsnrm", DcNullPtr);
   	return(DcFalse);

    } else {

	wcsnrm[0] = intersect->lcsnrm[0];
	wcsnrm[1] = intersect->lcsnrm[1];
	wcsnrm[2] = intersect->lcsnrm[2];
    }

    /*  
     * Find the dot product of the ray direction and the surface
     * normal.  If this value is positive, then the ray is viewing the
     * back part of the surface, and its intersection normal value
     * must be reversed for the subsequent shading calculation.  
     */
    dotproduct = ray->wcsdirection[0]*wcsnrm[0] + ray->wcsdirection[1]*wcsnrm[1] +
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
void drr_glbrnd_rayintsph_bounding_box (
    struct rayintsph *rayintsph,
    DtReal *boxxmin,
    DtReal *boxymin,
    DtReal *boxzmin,
    DtReal *boxxmax,
    DtReal *boxymax,
    DtReal *boxzmax)
{
    DtReal       radius;
    DtRealTriple location;

    radius = rayintsph->radius;
    location[0] = rayintsph->location[0];
    location[1] = rayintsph->location[1];
    location[2] = rayintsph->location[2];

    *boxxmin = location[0] - radius;
    *boxymin = location[1] - radius;
    *boxzmin = location[2] - radius;
    *boxxmax = location[0] + radius;
    *boxymax = location[1] + radius;
    *boxzmax = location[2] + radius;
}
/*
 ======================================================================
 */
DtFlag drr_glbrnd_rayintsph_intersect_box ( 
    DtReal boxxmin1,
    DtReal boxymin1,
    DtReal boxzmin1,
    DtReal boxxmax1,
    DtReal boxymax1,
    DtReal boxzmax1,
    DtReal boxxmin2,
    DtReal boxymin2,
    DtReal boxzmin2,
    DtReal boxxmax2,
    DtReal boxymax2,
    DtReal boxzmax2)
{
    /* Intersect the front face of the first box.  */

    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmin1,boxymin1,boxzmin1, boxxmin1,boxymax1,boxzmin1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmin1,boxymax1,boxzmin1, boxxmax1,boxymax1,boxzmin1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmax1,boxymax1,boxzmin1, boxxmax1,boxymin1,boxzmin1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmax1,boxymin1,boxzmin1, boxxmin1,boxymin1,boxzmin1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    /* 
     * Intersect the back face of the first box.
     */
    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmin1,boxymin1,boxzmax1, boxxmin1,boxymax1,boxzmax1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmin1,boxymax1,boxzmax1, boxxmax1,boxymax1,boxzmax1,
        boxxmin2,boxymin2,boxzmin2,boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmax1,boxymax1,boxzmax1, boxxmax1,boxymin1,boxzmax1,
        boxxmin2,boxymin2,boxzmin2,boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    /*
     * Intersect the unintersected edges of the left face.
     */
    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmin1,boxymax1,boxzmin1, boxxmin1,boxymax1,boxzmax1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmin1,boxymin1,boxzmax1, boxxmin1,boxymin1,boxzmin1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    /*
     * Intersect the unintersected edges of the right face.
     */
    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmax1,boxymax1,boxzmin1, boxxmax1,boxymax1,boxzmax1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    if (drr_glbrnd_rayintsph_intersect_edge_box(
        boxxmax1,boxymin1,boxzmax1, boxxmax1,boxymin1,boxzmin1,
        boxxmin2,boxymin2,boxzmin2, boxxmax2,boxymax2,boxzmax2))
        return(DcTrue);

    return(DcFalse);
}
/*
 ======================================================================
 */
DtFlag drr_glbrnd_rayintsph_intersect_edge_box (
    DtReal edgex0,
    DtReal edgey0,
    DtReal edgez0,
    DtReal edgex1,
    DtReal edgey1,
    DtReal edgez1,
    DtReal boxxmin,
    DtReal boxymin,
    DtReal boxzmin,
    DtReal boxxmax,
    DtReal boxymax,
    DtReal boxzmax)
{
    DtReal edgexdir, edgeydir, edgezdir;
    DtReal edgelength;

    dor_math_normal(edgex0,edgey0,edgez0, edgex1,edgey1,edgez1,
        &edgexdir,&edgeydir,&edgezdir);

    edgelength = dor_math_distance(edgex0,edgey0,edgez0,
        edgex1,edgey1,edgez1);

    return(drr_glbrnd_prisrf_intersect_edge_box(
        edgex0,edgey0,edgez0,
        edgexdir,edgeydir,edgezdir,edgelength,
        boxxmin,boxymin,boxzmin,boxxmax,boxymax,boxzmax));
}
