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
	drr_glbrnd_rayintmshtri_rayint
	drr_glbrnd_rayintmshtri_world_boundary
	drr_glbrnd_rayintmshtri_overlap_spacebox
	drr_glbrnd_rayintmshtri_userdata
	drr_glbrnd_rayintmshtri_wcsloc
	drr_glbrnd_rayintmshtri_wcsnrm
	drr_glbrnd_rayintmshtri_bounding_box
	drr_glbrnd_rayintmshtri_rayintersect_containment_test
	drr_glbrnd_rayintmshtri_intersect_edge_box
	drr_glbrnd_rayintmshtri_intersect_edge_triangle
	drr_glbrnd_rayintmshtri_intersect_box_triangle
	drr_glbrnd_rayintmshtri_intersect_triangle_box

  ======================================================================
 */
#include "../glbrnd.h"
#include <dore/dore_develop/private/rayintmtr.h>

/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayintmshtri_rayint (
    dot_object *object,
    struct ray *ray,
    struct intersect *intersect)
{
    struct rayintmshtri *mshtri = (struct rayintmshtri *)object->data;
    DtReal mshtricosinex = mshtri->cosine_x;
    DtReal mshtricosiney = mshtri->cosine_y;
    DtReal mshtricosinez = mshtri->cosine_z;
    DtReal D = mshtri->D;
    DtReal rayxdir = ray->wcsdirection[0];
    DtReal rayydir = ray->wcsdirection[1];
    DtReal rayzdir = ray->wcsdirection[2];
    DtReal rayx0 = ray->wcsorigin[0];
    DtReal rayy0 = ray->wcsorigin[1];
    DtReal rayz0 = ray->wcsorigin[2];
    DtReal temp1;
    DtReal temp2;
    DtReal t;

    temp1 = mshtricosinex*rayxdir + mshtricosiney*rayydir +
	    	mshtricosinez*rayzdir;

    temp2 = mshtricosinex*rayx0 + mshtricosiney*rayy0 + 
		mshtricosinez*rayz0 + D;

    t = temp1 * temp2;

    if ((t > 0.) || (temp1 == 0.)) {
	return(DcFalse);
    }

    intersect->t = t = -temp2 / temp1;

    intersect->lcsloc[0] = rayx0 + t*rayxdir;
    intersect->lcsloc[1] = rayy0 + t*rayydir;
    intersect->lcsloc[2] = rayz0 + t*rayzdir;

    if (!drr_glbrnd_rayintmshtri_rayintersect_containment_test(mshtri,
					       intersect->lcsloc)) {
	return(DcFalse);
    }

    intersect->lcsnrm_available = DcTrue;

    return(DcTrue);
}
/*
 ======================================================================
 */

void drr_glbrnd_rayintmshtri_world_boundary (
    dot_object *object,
    DtMatrix4x4 *lcstowcsmat,
    DtReal *xmin,
    DtReal *ymin,
    DtReal *zmin,
    DtReal *xmax,
    DtReal *ymax,
    DtReal *zmax)
{
    struct rayintmshtri *mshtri;

    if (lcstowcsmat != NULL) {
   	DDerror (ERR_NON_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayintmshtri_world_boundary", DcNullPtr);
   	return;
    }

    mshtri = (struct rayintmshtri *)object->data;

    drr_glbrnd_rayintmshtri_bounding_box(mshtri,xmin,ymin,zmin,xmax,ymax,zmax);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayintmshtri_overlap_spacebox (
    dot_object *object,
    DtMatrix4x4 *lcstowcsmat,
    DtReal boxminmax[6],
    struct bsp_instance *bspinstance)
{
   struct rayintmshtri *mshtri;
   DtReal bndboxxmin, bndboxymin, bndboxzmin;
   DtReal bndboxxmax, bndboxymax, bndboxzmax;
   DtReal boxxmin = boxminmax[0];
   DtReal boxymin = boxminmax[1];
   DtReal boxzmin = boxminmax[2];
   DtReal boxxmax = boxminmax[3];
   DtReal boxymax = boxminmax[4];
   DtReal boxzmax = boxminmax[5];

   mshtri = (struct rayintmshtri *)object->data;

   drr_glbrnd_rayintmshtri_bounding_box(mshtri,&bndboxxmin,&bndboxymin,
   	&bndboxzmin,&bndboxxmax,&bndboxymax,&bndboxzmax);

   /*  First, check to see if the bounding box is completely contained within
   the bsp box.  */

   if ((bndboxxmin>=boxxmin) &&
   	(bndboxymin>=boxymin) &&
   	(bndboxzmin>=boxzmin) &&
   	(bndboxxmax<=boxxmax) &&
   	(bndboxymax<=boxymax) &&
   	(bndboxzmax<=boxzmax)) {
   	return(DcTrue);
   }

   /*  Next, check to see if the bounding box is completely outside of
   the bsp box.  */

   if ((bndboxxmin>boxxmax) ||
   	(bndboxymin>boxymax) ||
   	(bndboxzmin>boxzmax) ||
   	(bndboxxmax<boxxmin) ||
   	(bndboxymax<boxymin) ||
   	(bndboxzmax<boxzmin)) {
   	return(DcFalse);
   }

   if (drr_glbrnd_rayintmshtri_intersect_triangle_box(mshtri,boxminmax)) {
   	return(DcTrue);
   }

   if (drr_glbrnd_rayintmshtri_intersect_box_triangle(boxminmax,mshtri)) {
   	return(DcTrue);
   }

   return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayintmshtri_userdata (dot_object *object, struct ray *ray, struct intersect *intersect, DtReal userdata[3])
{
    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayintmshtri_wcsloc (dot_object *object, struct ray *ray, struct intersect *intersect, DtReal wcsloc[3])
{
    struct rayint_object_modifier *rayintobjectmodifier;

    rayintobjectmodifier = intersect->rayint_object->modifier;

    if (rayintobjectmodifier->lcstowcsmat != NULL) {
   	DDerror (ERR_NON_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayintmshtri_wcsloc", DcNullPtr);

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

DtFlag drr_glbrnd_rayintmshtri_wcsnrm (dot_object *object, struct ray *ray, struct intersect *intersect, DtReal wcsnrm[3])
{
#ifdef debug
    struct rayint_object_modifier *rayintobjectmodifier;
#endif
    struct rayintmshtri *mshtri = (struct rayintmshtri *)object->data;
    DtReal initialnormx = mshtri->initialnormx;
    DtReal initialnormy = mshtri->initialnormy;
    DtReal initialnormz = mshtri->initialnormz;
    DtReal dnormxdval1 = mshtri->dnormxdval1;
    DtReal dnormxdval2 = mshtri->dnormxdval2;
    DtReal dnormydval1 = mshtri->dnormydval1;
    DtReal dnormydval2 = mshtri->dnormydval2;
    DtReal dnormzdval1 = mshtri->dnormzdval1;
    DtReal dnormzdval2 = mshtri->dnormzdval2;
    DtReal intersectx = intersect->lcsloc[0];
    DtReal intersecty = intersect->lcsloc[1];
    DtReal intersectz = intersect->lcsloc[2];
    DtReal length;
    DtReal dotproduct;

#ifdef debug
    rayintobjectmodifier = intersect->rayint_object->modifier;

    if (rayintobjectmodifier->lcstowcsmat != NULL) {
   	DDerror (ERR_NON_NULL_LCSTOWCS,
   		    "drr_glbrnd_rayintmshtri_wcsnrm", 
		    DcNullPtr);

   	return(DcFalse);
    }
#endif

    /*
      mshtri = (struct rayintmshtri *)object->data;
      */

    switch (mshtri->projection_type) {
    case dod_rayintmshtri_projection_type_x:
	wcsnrm[0] = initialnormx + dnormxdval1*intersecty +
			dnormxdval2*intersectz;
	wcsnrm[1] = initialnormy + dnormydval1*intersecty +
			dnormydval2*intersectz;
	wcsnrm[2] = initialnormz + dnormzdval1*intersecty +
			dnormzdval2*intersectz;
	break;

    case dod_rayintmshtri_projection_type_y:
	wcsnrm[0] = initialnormx + dnormxdval1*intersectx +
			dnormxdval2*intersectz;
	wcsnrm[1] = initialnormy + dnormydval1*intersectx +
			dnormydval2*intersectz;
	wcsnrm[2] = initialnormz + dnormzdval1*intersectx +
			dnormzdval2*intersectz;
	break;

    case dod_rayintmshtri_projection_type_z:
	wcsnrm[0] = initialnormx + dnormxdval1*intersectx +
			dnormxdval2*intersecty;
	wcsnrm[1] = initialnormy + dnormydval1*intersectx +
			dnormydval2*intersecty;
	wcsnrm[2] = initialnormz + dnormzdval1*intersectx +
			dnormzdval2*intersecty;
	break;
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

    /*  Since the normal at the intersection point was previously 
	computed by interpolating across the rayint triangle, it must 
	be renormalized at this time.  */

    length = (wcsnrm[0])*(wcsnrm[0]) +
	    	(wcsnrm[1])*(wcsnrm[1]) +
		(wcsnrm[2])*(wcsnrm[2]);

    if (length == 0.) {
   	DDerror (ERR_ZERO_LEN_VEC,
   		    "drr_glbrnd_rayintmshtri_wcsnrm", 
		    DcNullPtr);

   	wcsnrm[0] = 0.;
   	wcsnrm[1] = 0.;
   	wcsnrm[2] = 0.;
    } else {
   	length = 1. / dor_math_fast_sqrt(length);

   	wcsnrm[0] *= length;
   	wcsnrm[1] *= length;
   	wcsnrm[2] *= length;
    }

    return(DcTrue);
}
/*
 ======================================================================
 */

void drr_glbrnd_rayintmshtri_bounding_box (struct rayintmshtri *mshtri, DtReal *boxxmin, DtReal *boxymin, DtReal *boxzmin, DtReal *boxxmax, DtReal *boxymax, DtReal *boxzmax)
{
    DtReal xmin, ymin, zmin;
    DtReal xmax, ymax, zmax;

    /*  Use the first vertex of the mshtriangle as the beginning
	min and max values.  */

    xmin = mshtri->x1;
    ymin = mshtri->y1;
    zmin = mshtri->z1;
    xmax = mshtri->x1;
    ymax = mshtri->y1;
    zmax = mshtri->z1;

    xmin = min(xmin,mshtri->x2);
    ymin = min(ymin,mshtri->y2);
    zmin = min(zmin,mshtri->z2);
    xmax = max(xmax,mshtri->x2);
    ymax = max(ymax,mshtri->y2);
    zmax = max(zmax,mshtri->z2);

    xmin = min(xmin,mshtri->x3);
    ymin = min(ymin,mshtri->y3);
    zmin = min(zmin,mshtri->z3);
    xmax = max(xmax,mshtri->x3);
    ymax = max(ymax,mshtri->y3);
    zmax = max(zmax,mshtri->z3);

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

DtFlag drr_glbrnd_rayintmshtri_rayintersect_containment_test (struct rayintmshtri *mshtri, DtReal intersect[3])
{
    DtReal x1 = mshtri->x1;
    DtReal y1 = mshtri->y1;
    DtReal z1 = mshtri->z1;
    DtReal x2 = mshtri->x2;
    DtReal y2 = mshtri->y2;
    DtReal z2 = mshtri->z2;
    DtReal x3 = mshtri->x3;
    DtReal y3 = mshtri->y3;
    DtReal z3 = mshtri->z3;
    DtReal dx, dy, dx1, dy1;
    DtInt sideflag, newsideflag;

    switch (mshtri->projection_type) {
    case dod_rayintmshtri_projection_type_z:

	dx = x2 - mshtri->x1;
	dy = y2 - mshtri->y1;
	dx1 = intersect[0] - x1;
	dy1 = intersect[1] - y1;
	sideflag = ((dx*dy1-dy*dx1) > 0.);

	dx = x3 - mshtri->x2;
	dy = y3 - mshtri->y2;
	dx1 = intersect[0] - x2;
	dy1 = intersect[1] - y2;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	dx = x1 - mshtri->x3;
	dy = y1 - mshtri->y3;
	dx1 = intersect[0] - x3;
	dy1 = intersect[1] - y3;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	return(DcTrue);
	break;

    case dod_rayintmshtri_projection_type_y:

	dx = x2 - mshtri->x1;
	dy = z2 - mshtri->z1;
	dx1 = intersect[0] - x1;
	dy1 = intersect[2] - z1;
	sideflag = ((dx*dy1-dy*dx1) > 0.);

	dx = x3 - mshtri->x2;
	dy = z3 - mshtri->z2;
	dx1 = intersect[0] - x2;
	dy1 = intersect[2] - z2;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	dx = x1 - mshtri->x3;
	dy = z1 - mshtri->z3;
	dx1 = intersect[0] - x3;
	dy1 = intersect[2] - z3;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	return(DcTrue);
	break;

    case dod_rayintmshtri_projection_type_x:

	dx = y2 - mshtri->y1;
	dy = z2 - mshtri->z1;
	dx1 = intersect[1] - y1;
	dy1 = intersect[2] - z1;
	sideflag = ((dx*dy1-dy*dx1) > 0.);

	dx = y3 - mshtri->y2;
	dy = z3 - mshtri->z2;
	dx1 = intersect[1] - y2;
	dy1 = intersect[2] - z2;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	dx = y1 - mshtri->y3;
	dy = z1 - mshtri->z3;
	dx1 = intersect[1] - y3;
	dy1 = intersect[2] - z3;
	newsideflag = ((dx*dy1-dy*dx1) > 0.);
	if (newsideflag != sideflag)
		return(DcFalse);

	return(DcTrue);
	break;
    }

    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayintmshtri_intersect_edge_box (
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
      
/*    Test for intersection against the x=boxxmax surface of the box.	 */
   
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



/* ====================================================================== */

DtFlag drr_glbrnd_rayintmshtri_intersect_edge_triangle (
    DtReal edgex1,
    DtReal edgey1,
    DtReal edgez1,
    DtReal edgexdir,
    DtReal edgeydir,
    DtReal edgezdir,
    DtReal edgelength,
    struct rayintmshtri *mshtri)
{
    DtReal tricosinex, tricosiney, tricosinez, D;
    DtReal temp1;
    DtReal t;
    DtReal intersect[3];

    tricosinex = mshtri->cosine_x;
    tricosiney = mshtri->cosine_y;
    tricosinez = mshtri->cosine_z;
    D = mshtri->D;

    temp1 = tricosinex*edgexdir + 
		tricosiney*edgeydir + 
		tricosinez*edgezdir;

    if (temp1 == 0.)
  	return(DcFalse);

    t = (tricosinex*edgex1 + 
		tricosiney*edgey1 + 
		tricosinez*edgez1 + D) / temp1;

    if (t > 0.)
	return(DcFalse);

    if (-t > edgelength)
	return(DcFalse);

    intersect[0] = edgex1 - t*edgexdir;
    intersect[1] = edgey1 - t*edgeydir;
    intersect[2] = edgez1 - t*edgezdir;

    if (!drr_glbrnd_rayintmshtri_rayintersect_containment_test(
						mshtri,intersect))
	return(DcFalse);

    return(DcTrue);
}



/* ====================================================================== */

DtFlag drr_glbrnd_rayintmshtri_intersect_box_triangle (
    DtReal boxminmax[6],
    struct rayintmshtri *mshtri)
{
    DtReal boxsize;

    boxsize = boxminmax[3] - boxminmax[0];

    /*  Intersect the front face.  */

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[0],boxminmax[1],boxminmax[2],
				0.,1.,0.,boxsize,mshtri))
 	return(DcTrue);

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[0],boxminmax[4],boxminmax[2],
				1.,0.,0.,boxsize,mshtri))
	return(DcTrue);

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[3],boxminmax[4],boxminmax[2],
				0.,-1.,0.,boxsize,mshtri))
	return(DcTrue);

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[3],boxminmax[1],boxminmax[2],
				-1.,0.,0.,boxsize,mshtri))
	return(DcTrue);

    /*  Intersect the back face.  */

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[0],boxminmax[1],boxminmax[5],
				0.,1.,0.,boxsize,mshtri))
	return(DcTrue);

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[0],boxminmax[4],boxminmax[5],
				1.,0.,0.,boxsize,mshtri))
	return(DcTrue);

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[3],boxminmax[4],boxminmax[5],
				0.,-1.,0.,boxsize,mshtri))
	return(DcTrue);

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[3],boxminmax[1],boxminmax[5],
				-1.,0.,0.,boxsize,mshtri))
	return(DcTrue);

    /*  Intersect the unintersected edges of the left face.  */

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[0],boxminmax[4],boxminmax[2],
				0.,0.,1.,boxsize,mshtri))
	return(DcTrue);

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[0],boxminmax[1],boxminmax[5],
				0.,0.,-1.,boxsize,mshtri))
	return(DcTrue);

    /*  Intersect the unintersected edges of the right face.  */

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[3],boxminmax[4],boxminmax[2],
				0.,0.,1.,boxsize,mshtri))
	return(DcTrue);

    if (drr_glbrnd_rayintmshtri_intersect_edge_triangle(
				boxminmax[3],boxminmax[1],boxminmax[5],
				0.,0.,-1.,boxsize,mshtri))
  	return(DcTrue);

    return(DcFalse);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_rayintmshtri_intersect_triangle_box (
    struct rayintmshtri *mshtri,
    DtReal boxminmax[6])
{
    DtReal edgex0, edgey0, edgez0;
    DtReal edgex1, edgey1, edgez1;
    DtReal edgexdir, edgeydir, edgezdir;
    DtReal edgelength;
    DtReal oneoveredgelength;

    edgex0 = mshtri->x1;
    edgey0 = mshtri->y1;
    edgez0 = mshtri->z1;
    edgex1 = mshtri->x2;
    edgey1 = mshtri->y2;
    edgez1 = mshtri->z2;
    edgelength = dor_math_distance(edgex0,edgey0,edgez0,
					edgex1,edgey1,edgez1);
    oneoveredgelength = 1. / edgelength;
    edgexdir = (edgex1-edgex0) * oneoveredgelength;
    edgeydir = (edgey1-edgey0) * oneoveredgelength;
    edgezdir = (edgez1-edgez0) * oneoveredgelength;

    if (drr_glbrnd_rayintmshtri_intersect_edge_box(
				edgex0,edgey0,edgez0,
			   	edgexdir,edgeydir,edgezdir,
				edgelength,boxminmax))
	return(DcTrue);

    edgex0 = mshtri->x2;
    edgey0 = mshtri->y2;
    edgez0 = mshtri->z2;
    edgex1 = mshtri->x3;
    edgey1 = mshtri->y3;
    edgez1 = mshtri->z3;
    edgelength = dor_math_distance(edgex0,edgey0,edgez0,
					edgex1,edgey1,edgez1);
    oneoveredgelength = 1. / edgelength;
    edgexdir = (edgex1-edgex0) * oneoveredgelength;
    edgeydir = (edgey1-edgey0) * oneoveredgelength;
    edgezdir = (edgez1-edgez0) * oneoveredgelength;

    if (drr_glbrnd_rayintmshtri_intersect_edge_box(
					edgex0,edgey0,edgez0,
					edgexdir,edgeydir,edgezdir,
					edgelength,boxminmax))
	return(DcTrue);

    edgex0 = mshtri->x3;
    edgey0 = mshtri->y3;
    edgez0 = mshtri->z3;
    edgex1 = mshtri->x1;
    edgey1 = mshtri->y1;
    edgez1 = mshtri->z1;
    edgelength = dor_math_distance(edgex0,edgey0,edgez0,
					edgex1,edgey1,edgez1);
    oneoveredgelength = 1. / edgelength;
    edgexdir = (edgex1-edgex0) * oneoveredgelength;
    edgeydir = (edgey1-edgey0) * oneoveredgelength;
    edgezdir = (edgez1-edgez0) * oneoveredgelength;

    if (drr_glbrnd_rayintmshtri_intersect_edge_box(
					edgex0,edgey0,edgez0,
					edgexdir,edgeydir,edgezdir,
					edgelength,boxminmax))
	return(DcTrue);

    return(DcFalse);
}
