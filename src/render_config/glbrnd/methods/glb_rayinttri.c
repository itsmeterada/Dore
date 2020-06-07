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
	DtFlag drr_glbrnd_rayinttri_rayint (object,ray,intersect)
	int drr_glbrnd_rayinttri_world_boundary (object,lcstowcsmat, xmin,ymin,zmin,xmax,ymax,zmax)
	DtFlag drr_glbrnd_rayinttri_overlap_spacebox (object,lcstowcsmat,boxminmax)
	DtFlag drr_glbrnd_rayinttri_userdata (object,ray,intersect,userdata)
	DtFlag drr_glbrnd_rayinttri_wcsloc (object,ray,intersect,wcsloc)
	DtFlag drr_glbrnd_rayinttri_wcsnrm (object,ray,intersect,wcsnrm)
	int drr_glbrnd_rayinttri_bounding_box (tri,boxxmin,boxymin,boxzmin, boxxmax,boxymax,boxzmax)
	DtFlag drr_glbrnd_rayinttri_rayintersect_containment_test (tri, intersectx,intersecty,intersectz)
	DtFlag drr_glbrnd_rayinttri_intersect_edge_box (edgex0,edgey0,edgez0, edgexdir,edgeydir,edgezdir, edgelength,boxminmax)
	DtFlag drr_glbrnd_rayinttri_intersect_edge_triangle (edgex1,edgey1,edgez1, edgexdir,edgeydir,edgezdir, edgelength,tri)
	DtFlag drr_glbrnd_rayinttri_intersect_spacebox_triangle (boxminmax, tri)
	DtFlag drr_glbrnd_rayinttri_intersect_triangle_spacebox (tri, boxminmax)

  ======================================================================
 */
#include "../glbrnd.h"
#include "dore/dore_develop/private/rayinttri.h"

/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayinttri_rayint (dot_object *object, struct ray *ray, struct intersect *intersect)
{
    struct rayinttri *tri;
    DtReal tricosinex, tricosiney, tricosinez, D;
    DtReal rayxdir, rayydir, rayzdir;
    DtReal temp1;
    DtReal rayx0, rayy0, rayz0;
    DtReal t;

    tri = (struct rayinttri *)object->data;

    tricosinex = tri->cosine_x;
    tricosiney = tri->cosine_y;
    tricosinez = tri->cosine_z;
    D = tri->D;

    rayxdir = ray->wcsdirection[0];
    rayydir = ray->wcsdirection[1];
    rayzdir = ray->wcsdirection[2];

    temp1 = tricosinex*rayxdir + tricosiney*rayydir + tricosinez*rayzdir;

    if (temp1 == 0.) {
   	return(DcFalse);
    }

    rayx0 = ray->wcsorigin[0];
    rayy0 = ray->wcsorigin[1];
    rayz0 = ray->wcsorigin[2];

    t = (tricosinex*rayx0 + tricosiney*rayy0 + tricosinez*rayz0 + D) /
	    temp1;

    if (t > 0.) {
   	return(DcFalse);
    }

    intersect->lcsloc[0] = rayx0 - t*rayxdir;
    intersect->lcsloc[1] = rayy0 - t*rayydir;
    intersect->lcsloc[2] = rayz0 - t*rayzdir;

    if (!drr_glbrnd_rayinttri_rayintersect_containment_test(tri,
	  intersect->lcsloc[0],intersect->lcsloc[1],intersect->lcsloc[2])) {
   	return(DcFalse);
    }

    intersect->t = t;

    intersect->lcsnrm[0] = tricosinex;
    intersect->lcsnrm[1] = tricosiney;
    intersect->lcsnrm[2] = tricosinez;

    intersect->lcsnrm_available = DcTrue;

    return(DcTrue);
}
/*
 ======================================================================
 */

void drr_glbrnd_rayinttri_world_boundary (
    dot_object *object,
    DtMatrix4x4 *lcstowcsmat,
    DtReal *xmin,
    DtReal *ymin,
    DtReal *zmin,
    DtReal *xmax,
    DtReal *ymax,
    DtReal *zmax)
{
    struct rayinttri *tri;

    if (lcstowcsmat != NULL) {
   	DDerror (ERR_NON_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayinttri_world_boundary", 
		    DcNullPtr);
   	return;
    }

    tri = (struct rayinttri *)object->data;

    drr_glbrnd_rayinttri_bounding_box(tri,xmin,ymin,zmin,xmax,ymax,zmax);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayinttri_overlap_spacebox (dot_object *object, DtMatrix4x4 *lcstowcsmat, DtReal boxminmax[6])
{
    struct rayinttri *tri;
    DtReal bndboxxmin, bndboxymin, bndboxzmin;
    DtReal bndboxxmax, bndboxymax, bndboxzmax;
    DtReal spaceboxxmin = boxminmax[0];
    DtReal spaceboxymin = boxminmax[1];
    DtReal spaceboxzmin = boxminmax[2];
    DtReal spaceboxxmax = boxminmax[3];
    DtReal spaceboxymax = boxminmax[4];
    DtReal spaceboxzmax = boxminmax[5];

    if (lcstowcsmat != NULL) {
   	DDerror (ERR_NON_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayinttri_spacebox_overlap", 
		    DcNullPtr);
   	return(DcFalse);
    }

    tri = (struct rayinttri *)object->data;

    drr_glbrnd_rayinttri_bounding_box(tri,&bndboxxmin,&bndboxymin,&bndboxzmin,
					&bndboxxmax,&bndboxymax,&bndboxzmax);

    /*  First, check to see if the bounding box is completely 
	contained within the space box.  */

    if ((bndboxxmin>=spaceboxxmin) && (bndboxymin>=spaceboxymin) &&
   	(bndboxzmin>=spaceboxzmin) &&
   	(bndboxxmax<=spaceboxxmax) && (bndboxymax<=spaceboxymax) &&
   	(bndboxzmax<=spaceboxzmax)) {

   	return(DcTrue);
    }

    /*  Next, check to see if the bounding box is completely outside of
	the space box.  */

    if ((bndboxxmin>spaceboxxmax) || (bndboxymin>spaceboxymax) ||
   	(bndboxzmin>spaceboxzmax) ||
   	(bndboxxmax<spaceboxxmin) || (bndboxymax<spaceboxymin) ||
   	(bndboxzmax<spaceboxzmin)) {
   	return(DcFalse);
    }

    if (drr_glbrnd_rayinttri_intersect_triangle_spacebox(tri, boxminmax)) {
   	return(DcTrue);
    }

    if (drr_glbrnd_rayinttri_intersect_spacebox_triangle(boxminmax, tri)) {
   	return(DcTrue);
    }

    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayinttri_userdata (dot_object *object, struct ray *ray, struct intersect *intersect, DtReal userdata[3])
{
    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayinttri_wcsloc (dot_object *object, struct ray *ray, struct intersect *intersect, DtReal wcsloc[3])
{
    struct rayint_object_modifier *rayintobjectmodifier;

    rayintobjectmodifier = intersect->rayint_object->modifier;

    if (rayintobjectmodifier->lcstowcsmat != NULL) {
   	DDerror (ERR_NON_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayinttri_wcsloc", DcNullPtr);

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

DtFlag drr_glbrnd_rayinttri_wcsnrm (dot_object *object, struct ray *ray, struct intersect *intersect, DtReal wcsnrm[3])
{
    struct rayint_object_modifier *rayintobjectmodifier;

    rayintobjectmodifier = intersect->rayint_object->modifier;

    if (rayintobjectmodifier->lcstowcsmat != NULL) {
	DDerror (ERR_NON_NULL_LCSTOWCS,
		    "drr_glbrnd_rayinttri_wcsnrm", DcNullPtr);

	return(DcFalse);
    } else {
	wcsnrm[0] = intersect->lcsnrm[0];
	wcsnrm[1] = intersect->lcsnrm[1];
	wcsnrm[2] = intersect->lcsnrm[2];
    }

    /*  Find the dot product of the ray direction and the surface
	normal.  If this value is positive, then the ray is viewing the
	back part of the surface, and its intersection normal value
	must be reversed for the subsequent shading calculation.  */

    if (dor_math_dotproduct(ray->wcsdirection[0],
			    ray->wcsdirection[1],
			    ray->wcsdirection[2],
			    wcsnrm[0],wcsnrm[1],wcsnrm[2]) > 0.) {
	wcsnrm[0] = -wcsnrm[0];
	wcsnrm[1] = -wcsnrm[1];
	wcsnrm[2] = -wcsnrm[2];
    }

    return(DcTrue);
}
/*
 ======================================================================
 */

void drr_glbrnd_rayinttri_bounding_box (
    struct rayinttri *tri,
    DtReal *boxxmin,
    DtReal *boxymin,
    DtReal *boxzmin,
    DtReal *boxxmax,
    DtReal *boxymax,
    DtReal *boxzmax)
{
    DtReal xmin, ymin, zmin;
    DtReal xmax, ymax, zmax;

    /*  Use the first vertex of the triangle as the beginning
	min and max values.  */

    xmin = tri->x1;
    ymin = tri->y1;
    zmin = tri->z1;
    xmax = tri->x1;
    ymax = tri->y1;
    zmax = tri->z1;

    xmin = min(xmin,tri->x2);
    ymin = min(ymin,tri->y2);
    zmin = min(zmin,tri->z2);
    xmax = max(xmax,tri->x2);
    ymax = max(ymax,tri->y2);
    zmax = max(zmax,tri->z2);

    xmin = min(xmin,tri->x3);
    ymin = min(ymin,tri->y3);
    zmin = min(zmin,tri->z3);
    xmax = max(xmax,tri->x3);
    ymax = max(ymax,tri->y3);
    zmax = max(zmax,tri->z3);

    *boxxmin = xmin;
    *boxymin = ymin;
    *boxzmin = zmin;
    *boxxmax = xmax;
    *boxymax = ymax;
    *boxzmax = zmax;
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayinttri_rayintersect_containment_test (
    struct rayinttri *tri,
    DtReal intersectx,
    DtReal intersecty,
    DtReal intersectz)
{
    register DtReal dx, dy, dx1, dy1;
    DtFlag sideflag, newsideflag;

    switch (tri->projection_type){
    case dod_rayinttri_projection_type_z:

	dx = tri->x2 - tri->x1;
	dy = tri->y2 - tri->y1;
	dx1 = intersectx - tri->x1;
	dy1 = intersecty - tri->y1;
	sideflag = ((dx*dy1-dy*dx1) > 0.);

	dx = tri->x3 - tri->x2;
	dy = tri->y3 - tri->y2;
	dx1 = intersectx - tri->x2;
	dy1 = intersecty - tri->y2;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	dx = tri->x1 - tri->x3;
	dy = tri->y1 - tri->y3;
	dx1 = intersectx - tri->x3;
	dy1 = intersecty - tri->y3;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	return(DcTrue);
	break;

    case dod_rayinttri_projection_type_y:

	dx = tri->x2 - tri->x1;
	dy = tri->z2 - tri->z1;
	dx1 = intersectx - tri->x1;
	dy1 = intersectz - tri->z1;
	sideflag = ((dx*dy1-dy*dx1) > 0.);

	dx = tri->x3 - tri->x2;
	dy = tri->z3 - tri->z2;
	dx1 = intersectx - tri->x2;
	dy1 = intersectz - tri->z2;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	dx = tri->x1 - tri->x3;
	dy = tri->z1 - tri->z3;
	dx1 = intersectx - tri->x3;
	dy1 = intersectz - tri->z3;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	return(DcTrue);
	break;

    case dod_rayinttri_projection_type_x:

	dx = tri->y2 - tri->y1;
	dy = tri->z2 - tri->z1;
	dx1 = intersecty - tri->y1;
	dy1 = intersectz - tri->z1;
	sideflag = ((dx*dy1-dy*dx1) > 0.);

	dx = tri->y3 - tri->y2;
	dy = tri->z3 - tri->z2;
	dx1 = intersecty - tri->y2;
	dy1 = intersectz - tri->z2;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	dx = tri->y1 - tri->y3;
	dy = tri->z1 - tri->z3;
	dx1 = intersecty - tri->y3;
	dy1 = intersectz - tri->z3;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	return(DcTrue);
	break;
    }

    return (DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayinttri_intersect_edge_box (
    DtReal edgex0,
    DtReal edgey0,
    DtReal edgez0,
    DtReal edgexdir,
    DtReal edgeydir,
    DtReal edgezdir,
    DtReal edgelength,
    DtReal boxminmax[6])
{
    DtReal boxxmin = boxminmax[0];
    DtReal boxymin = boxminmax[1];
    DtReal boxzmin = boxminmax[2];
    DtReal boxxmax = boxminmax[3];
    DtReal boxymax = boxminmax[4];
    DtReal boxzmax = boxminmax[5];
    DtReal t;
    DtReal intersectx, intersecty, intersectz;

    if (edgexdir != 0.) {
  /*  Test for intersection against the x=boxxmin surface of the box.  */

       t = (boxxmin-edgex0) / edgexdir;

       if (t > 0. && t <= edgelength) {
          intersecty = edgey0 + t*edgeydir;

          if ((intersecty >= boxymin) && (intersecty <= boxymax)) {
             intersectz = edgez0 + t*edgezdir;

             if ((intersectz >= boxzmin) && (intersectz <= boxzmax)) {
                intersectx = boxxmin;
                return(DcTrue);
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
                return(DcTrue);
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

DtFlag drr_glbrnd_rayinttri_intersect_edge_triangle (
    DtReal edgex1,
    DtReal edgey1,
    DtReal edgez1,
    DtReal edgexdir,
    DtReal edgeydir,
    DtReal edgezdir,
    DtReal edgelength,
    struct rayinttri *tri)
{
    DtReal tricosinex, tricosiney, tricosinez, D;
    DtReal temp1;
    DtReal t;
    DtReal intersectx, intersecty, intersectz;

    tricosinex = tri->cosine_x;
    tricosiney = tri->cosine_y;
    tricosinez = tri->cosine_z;
    D = tri->D;

    temp1 = tricosinex*edgexdir + tricosiney*edgeydir + tricosinez*edgezdir;

    if (temp1 == 0.)
	return(DcFalse);

    t = (tricosinex*edgex1 + tricosiney*edgey1 + tricosinez*edgez1 + D) / temp1;

    if (t > 0.)
	return(DcFalse);

    if (-t > edgelength)
	return(DcFalse);

    intersectx = edgex1 - t*edgexdir;
    intersecty = edgey1 - t*edgeydir;
    intersectz = edgez1 - t*edgezdir;

    if (!drr_glbrnd_rayinttri_rayintersect_containment_test(tri,
					    intersectx,intersecty,intersectz))
	return(DcFalse);

    return(DcTrue);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayinttri_intersect_spacebox_triangle (DtReal boxminmax[6], struct rayinttri *tri)
{
    DtReal spaceboxxmin = boxminmax[0];
    DtReal spaceboxymin = boxminmax[1];
    DtReal spaceboxzmin = boxminmax[2];
    DtReal spaceboxxmax = boxminmax[3];
    DtReal spaceboxymax = boxminmax[4];
    DtReal spaceboxzmax = boxminmax[5];
    DtReal spaceboxsize;

    spaceboxsize = spaceboxxmax - spaceboxxmin;

    /*  Intersect the front face.  */

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmin,spaceboxymin,spaceboxzmin,
			     0.,1.,0.,spaceboxsize,tri))
        return(DcTrue);

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmin,spaceboxymax,spaceboxzmin,
			     1.,0.,0.,spaceboxsize,tri))
	return(DcTrue);

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmax,spaceboxymax,spaceboxzmin,
			     0.,-1.,0.,spaceboxsize,tri))
	return(DcTrue);

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmax,spaceboxymin,spaceboxzmin,
			     -1.,0.,0.,spaceboxsize,tri))
	return(DcTrue);

    /*  Intersect the back face.  */

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmin,spaceboxymin,spaceboxzmax,
			     0.,1.,0.,spaceboxsize,tri))
	return(DcTrue);

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmin,spaceboxymax,spaceboxzmax,
			     1.,0.,0.,spaceboxsize,tri))
	return(DcTrue);

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmax,spaceboxymax,spaceboxzmax,
			     0.,-1.,0.,spaceboxsize,tri))
	return(DcTrue);

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmax,spaceboxymin,spaceboxzmax,
			     -1.,0.,0.,spaceboxsize,tri))
	return(DcTrue);

    /*  Intersect the unintersected edges of the left face.  */

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmin,spaceboxymax,spaceboxzmin,
			     0.,0.,1.,spaceboxsize,tri))
	return(DcTrue);

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmin,spaceboxymin,spaceboxzmax,
			     0.,0.,-1.,spaceboxsize,tri))
	return(DcTrue);

    /*  Intersect the unintersected edges of the right face.  */

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmax,spaceboxymax,spaceboxzmin,
			     0.,0.,1.,spaceboxsize,tri))
	return(DcTrue);

    if (drr_glbrnd_rayinttri_intersect_edge_triangle(
			     spaceboxxmax,spaceboxymin,spaceboxzmax,
			     0.,0.,-1.,spaceboxsize,tri))
	return(DcTrue);

    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayinttri_intersect_triangle_spacebox (struct rayinttri *tri, DtReal boxminmax[6])
{
    DtReal edgex0, edgey0, edgez0;
    DtReal edgex1, edgey1, edgez1;
    DtReal edgexdir, edgeydir, edgezdir;
    DtReal edgelength;

    edgex0 = tri->x1;
    edgey0 = tri->y1;
    edgez0 = tri->z1;
    edgex1 = tri->x2;
    edgey1 = tri->y2;
    edgez1 = tri->z2;
    edgelength = dor_math_distance(edgex0,edgey0,edgez0,edgex1,edgey1,edgez1);
    edgexdir = (edgex1-edgex0) / edgelength;
    edgeydir = (edgey1-edgey0) / edgelength;
    edgezdir = (edgez1-edgez0) / edgelength;

    if (drr_glbrnd_rayinttri_intersect_edge_box(edgex0,edgey0,edgez0,
					edgexdir,edgeydir,edgezdir,
					edgelength,boxminmax))
	return(DcTrue);

    edgex0 = tri->x2;
    edgey0 = tri->y2;
    edgez0 = tri->z2;
    edgex1 = tri->x3;
    edgey1 = tri->y3;
    edgez1 = tri->z3;
    edgelength = dor_math_distance(edgex0,edgey0,edgez0,
				edgex1,edgey1,edgez1);
    edgexdir = (edgex1-edgex0) / edgelength;
    edgeydir = (edgey1-edgey0) / edgelength;
    edgezdir = (edgez1-edgez0) / edgelength;

    if (drr_glbrnd_rayinttri_intersect_edge_box(edgex0,edgey0,edgez0,
					edgexdir,edgeydir,edgezdir,
					edgelength,boxminmax))
	return(DcTrue);

    edgex0 = tri->x3;
    edgey0 = tri->y3;
    edgez0 = tri->z3;
    edgex1 = tri->x1;
    edgey1 = tri->y1;
    edgez1 = tri->z1;
    edgelength = dor_math_distance(edgex0,edgey0,edgez0,
				   edgex1,edgey1,edgez1);
    edgexdir = (edgex1-edgex0) / edgelength;
    edgeydir = (edgey1-edgey0) / edgelength;
    edgezdir = (edgez1-edgez0) / edgelength;

    if (drr_glbrnd_rayinttri_intersect_edge_box(edgex0,edgey0,edgez0,
					edgexdir,edgeydir,edgezdir,
					edgelength,boxminmax))
	return(DcTrue);

    return(DcFalse);
}
