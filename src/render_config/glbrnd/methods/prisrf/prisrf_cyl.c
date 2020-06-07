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
	DtFlag drr_glbrnd_prisrf_rayint_cylinder (prisrf,ray,intersect)
	int drr_glbrnd_prisrf_world_boundary_cylinder (lcstowcsmat,xmin,ymin,zmin,xmax,ymax,zmax)
	DtFlag drr_glbrnd_prisrf_overlap_spacebox_cylinder (lcstowcsmat,boxminmax)

  ======================================================================
 */
#include "../../glbrnd.h"
#include "dore/dore_develop/private/prisrf.h"

/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_rayint_cylinder (
    struct prisrf *prisrf,
    struct ray *ray,
    struct intersect *intersect)
{
    DtReal rayx0 = ray->wcsorigin[0];
    DtReal rayy0 = ray->wcsorigin[1];
    DtReal rayz0 = ray->wcsorigin[2];
    DtReal rayxdir = ray->wcsdirection[0];
    DtReal rayydir = ray->wcsdirection[1];
    DtReal rayzdir = ray->wcsdirection[2];
    DtFlag intersectflag;
    DtReal oneover2a;
    DtReal rayzslope;
    DtReal xinter, yinter, zinter;
    DtReal tinter;
    DtReal xnorm, ynorm, znorm;
    DtReal a, b, c, temp1;
    DtReal t;
    DtReal intersect_t;
    DtReal sqrttemp1;
    DtReal newxinter, newyinter, newzinter;

    intersectflag = DcFalse;

    a = rayxdir*rayxdir + rayydir*rayydir;
    b = 2. * (rayx0*rayxdir+rayy0*rayydir);
    c = rayx0*rayx0 + rayy0*rayy0 - 1.;

    temp1 = b*b - 4.*a*c;

    if ((temp1 >= 0.) && (a != 0.)) {
   	sqrttemp1 = dor_math_sqrt(temp1);

   	oneover2a = .5 / a;

   	t = (-b+sqrttemp1) * oneover2a;

   	if (t > .000001) {
	    newzinter = rayz0 + t*rayzdir;

	    if ((newzinter >= 0.) && (newzinter <= 1.)) {
		newxinter = rayx0 + t*rayxdir;
		newyinter = rayy0 + t*rayydir;

		intersectflag = DcTrue;

		xinter = newxinter;
		yinter = newyinter;
		zinter = newzinter;

		tinter = t;

		xnorm = newxinter;
		ynorm = newyinter;
		znorm = 0.;

		intersect_t = t;
	    }
   	}

   	t = (-b-sqrttemp1) * oneover2a;

   	if (t > .000001) {
	    newzinter = rayz0 + t*rayzdir;

	    if ((newzinter >= 0.) && (newzinter <= 1.)) {
		newxinter = rayx0 + t*rayxdir;
		newyinter = rayy0 + t*rayydir;

		if (intersectflag == DcFalse) {
		    intersectflag = DcTrue;

		    xinter = newxinter;
		    yinter = newyinter;
		    zinter = newzinter;

		    tinter = t;

		    xnorm = newxinter;
		    ynorm = newyinter;
		    znorm = 0.;

		    intersect_t = t;
		} else {
		    if (t < intersect_t) {
			xinter = newxinter;
			yinter = newyinter;
			zinter = newzinter;

			tinter = t;

			xnorm = newxinter;
			ynorm = newyinter;
			znorm = 0.;
		    }

		    intersect->lcsloc[0] = xinter;
		    intersect->lcsloc[1] = yinter;
		    intersect->lcsloc[2] = zinter;

		    intersect->t = tinter;

		    intersect->lcsnrm_available = DcTrue;

		    intersect->lcsnrm[0] = xnorm;
		    intersect->lcsnrm[1] = ynorm;
		    intersect->lcsnrm[2] = znorm;

		    return(DcTrue);
		}
	    }
   	}
    }

    if (rayzdir != 0.) {
   	/* intersect against the z=0 surface of the cyl  */

   	rayzslope = 1. / rayzdir;

   	t = -rayz0 * rayzslope;

   	if (t > .000001) {
	    newxinter = rayx0 + t*rayxdir;
	    newyinter = rayy0 + t*rayydir;

	    if ((newxinter*newxinter+newyinter*newyinter) <= 1.) {
		newzinter = 0.;

		if (intersectflag == DcFalse) {
		    intersectflag = DcTrue;

		    xinter = newxinter;
		    yinter = newyinter;
		    zinter = newzinter;

		    tinter = t;

		    xnorm = 0.;
		    ynorm = 0.;
		    znorm = -1.;

		    intersect_t = t;
		} else {
		    if (t < intersect_t) {
			xinter = newxinter;
			yinter = newyinter;
			zinter = newzinter;

			tinter = t;

			xnorm = 0.;
			ynorm = 0.;
			znorm = -1.;
		    }

		    intersect->lcsloc[0] = xinter;
		    intersect->lcsloc[1] = yinter;
		    intersect->lcsloc[2] = zinter;

		    intersect->t = tinter;

		    intersect->lcsnrm_available = DcTrue;

		    intersect->lcsnrm[0] = xnorm;
		    intersect->lcsnrm[1] = ynorm;
		    intersect->lcsnrm[2] = znorm;

		    return(DcTrue);
		}
	    }
   	}
   
   	/* intersect against the z=1 surface of the cyl */
   
   	t = (1.-rayz0) * rayzslope;

   	if (t > .000001) {
	    newxinter = rayx0 + t*rayxdir;
	    newyinter = rayy0 + t*rayydir;

	    if ((newxinter*newxinter+newyinter*newyinter) <= 1.) {
		newzinter = 1.;

		if (intersectflag == DcFalse) {
		    intersectflag = DcTrue;

		    xinter = newxinter;
		    yinter = newyinter;
		    zinter = newzinter;

		    tinter = t;

		    xnorm = 0.;
		    ynorm = 0.;
		    znorm = 1.;

		    intersect_t = t;
		} else {
		    if (t < intersect_t) {
			xinter = newxinter;
			yinter = newyinter;
			zinter = newzinter;

			tinter = t;

			xnorm = 0.;
			ynorm = 0.;
			znorm = 1.;
		    }

		    intersect->lcsloc[0] = xinter;
		    intersect->lcsloc[1] = yinter;
		    intersect->lcsloc[2] = zinter;

		    intersect->t = tinter;

		    intersect->lcsnrm_available = DcTrue;

		    intersect->lcsnrm[0] = xnorm;
		    intersect->lcsnrm[1] = ynorm;
		    intersect->lcsnrm[2] = znorm;

		    return(DcTrue);
		}
	    }
   	}
    }

    if (intersectflag) {
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

void drr_glbrnd_prisrf_world_boundary_cylinder (
    DtMatrix4x4 lcstowcsmat,
    DtReal *xmin,
    DtReal *ymin,
    DtReal *zmin,
    DtReal *xmax,
    DtReal *ymax,
    DtReal *zmax)
{
    drr_glbrnd_prisrf_bound_lcsbox(lcstowcsmat,-1.,-1.,0.,1.,1.,1.,
				   xmin,ymin,zmin,xmax,ymax,zmax);
}
/*
 ======================================================================
 */

DtFlag drr_glbrnd_prisrf_overlap_spacebox_cylinder (
    DtMatrix4x4 lcstowcsmat,
    DtReal boxminmax[6])
{
    return(drr_glbrnd_prisrf_overlap_lcsbox_spacebox(-1.,-1.,0.,1.,1.,1.,lcstowcsmat,
						     boxminmax));
}
