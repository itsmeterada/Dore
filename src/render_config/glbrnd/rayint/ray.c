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
	DtInt drr_rayint_classify_ray (raycosine)
	DtFlag drr_rayint_intersect (ray,anyintersectswitch,intersect)
	DtFlag drr_rayint_intersect_ray_space (rayorigin,raycosine,rayslope,intersect)
	int drr_rayint_box_ray_intersect (box,rayorigin,raycosine,rayslope,rayclass, faceno,intersect)
	DtFlag drr_rayint_calculate_object_intersect (box,rayintobjectlist,ray, anyintersectswitch,intersect)

  ======================================================================
 */
#include "rayint.h"
#include <dore/internal/list.h>



/* ====================================================================== */

DtInt drr_rayint_classify_ray (DtReal raycosine[3])
{
    DtInt class;

    class = (raycosine[0] >= 0.) | ((raycosine[1] >= 0.)<<1) |
	    ((raycosine[2] >= 0.)<<2);

    return(class);
}



/* ====================================================================== */

DtFlag drr_rayint_intersect (
    struct ray *ray,
    DtFlag anyintersectswitch,
    struct intersect *intersect)
{
    DtReal rayorigin[3];
    DtShort rayclass;
    DtFlag intersectflag;
    struct rayint_box *box;
    DtInt faceno;
    DtReal intersectpoint[3];

    rayorigin[0] = ray->wcsorigin[0];
    rayorigin[1] = ray->wcsorigin[1];
    rayorigin[2] = ray->wcsorigin[2];

    /*
     * NOTE: ray->wcsslope[?] is not used if the corresponding
     * ray->wcsdirection[?] is 0.  The following checks are just
     * to avoid a divide by zero that causes some  systems to
     * get floating point exceptions.  Slope should be infinity
     * for a direction value of zero.
     */

    if (ray->wcsdirection[0] == 0.) {
	ray->wcsslope[0] = 0.;
    } else {
	ray->wcsslope[0] = 1. / ray->wcsdirection[0];
    }

    if (ray->wcsdirection[1] == 0.){
	ray->wcsslope[1] = 0.;
    } else{
	ray->wcsslope[1] = 1. / ray->wcsdirection[1];
    }

    if (ray->wcsdirection[2] == 0.){
	ray->wcsslope[2] = 0.;
    } else{
	ray->wcsslope[2] = 1. / ray->wcsdirection[2];
    }

    if (ray->current_box == NULL){
	box = drr_rayint_get_first_box(rayorigin,
				ray->wcsdirection,ray->wcsslope);

	if (box == NULL){
	    return(DcFalse);
	}
    } else{
	box = ray->current_box;
    }

    rayclass = drr_rayint_classify_ray(ray->wcsdirection);

    while (box != NULL){
	intersectflag = drr_rayint_calculate_object_intersect(
					box,box->object_list,
					ray,anyintersectswitch,
					intersect);

	if (intersectflag){
	    ray->current_box = box;

	    return(DcTrue);
	}

	drr_rayint_box_ray_intersect(box,rayorigin,ray->wcsdirection,
				     	ray->wcsslope,rayclass,
					&faceno,intersectpoint);

	rayorigin[0] = intersectpoint[0];
	rayorigin[1] = intersectpoint[1];
	rayorigin[2] = intersectpoint[2];

	box = drr_rayint_get_next_box(box,faceno,rayorigin);
    }

    return(DcFalse);
}



/* ====================================================================== */

DtFlag drr_rayint_intersect_ray_space (
    DtReal rayorigin[3],
    DtReal raycosine[3],
    DtReal rayslope[3],
    DtReal intersect[3])
{
    DtReal spcxmin = dre_rayint_data.space_x_minimum;
    DtReal spcymin = dre_rayint_data.space_y_minimum;
    DtReal spczmin = dre_rayint_data.space_z_minimum;
    DtReal spcxmax = dre_rayint_data.space_x_maximum;
    DtReal spcymax = dre_rayint_data.space_y_maximum;
    DtReal spczmax = dre_rayint_data.space_z_maximum;
    DtFlag intersectflag = DcFalse;
    DtReal inter[3];
    DtReal intersect_t;
    DtReal t;
    DtReal newintersect[3];

    if (raycosine[0] != 0.){
	/*	 Intersect against the x=xmin surface of space.  */

	t = (spcxmin-rayorigin[0]) * rayslope[0];

	if (t > 0.){
	    newintersect[1] = rayorigin[1] + t*raycosine[1];

	    if ((newintersect[1] >= spcymin) && 
			(newintersect[1] <= spcymax)){
		newintersect[2] = rayorigin[2] + t*raycosine[2];

		if ((newintersect[2] >= spczmin) && 
			(newintersect[2] <= spczmax)){
		    newintersect[0] = spcxmin;
		    intersectflag = DcTrue;
		    intersect_t = t;

		    inter[0] = newintersect[0];
		    inter[1] = newintersect[1];
		    inter[2] = newintersect[2];
		}
	    }
	}
		
	/*	 Intersect against the x=xmax surface of space.  */
	
	t = (spcxmax-rayorigin[0]) * rayslope[0];

	if (t > 0.){
	    newintersect[1] = rayorigin[1] + t*raycosine[1];

	    if ((newintersect[1] >= spcymin) && 
			(newintersect[1] <= spcymax)){
		newintersect[2] = rayorigin[2] + t*raycosine[2];

		if ((newintersect[2] >= spczmin) && 
			(newintersect[2] <= spczmax)){
		    newintersect[0] = spcxmax;

		    if (!intersectflag){
			intersectflag = DcTrue;

			intersect_t = t;

			inter[0] = newintersect[0];
			inter[1] = newintersect[1];
			inter[2] = newintersect[2];

		    } else{
			if (t < intersect_t){
			    inter[0] = newintersect[0];
			    inter[1] = newintersect[1];
			    inter[2] = newintersect[2];
			}

			intersect[0] = inter[0];
			intersect[1] = inter[1];
			intersect[2] = inter[2];

			return(DcTrue);
		    }
		}
	    }
	}
    }

    if (raycosine[1] != 0.){
	/*	 Intersect against the y=ymin surface of space.  */

	t = (spcymin-rayorigin[1]) * rayslope[1];

	if (t > 0.){
	    newintersect[0] = rayorigin[0] + t*raycosine[0];

	    if ((newintersect[0] >= spcxmin) && 
			(newintersect[0] <= spcxmax)){
		newintersect[2] = rayorigin[2] + t*raycosine[2];

		if ((newintersect[2] >= spczmin) && 
			(newintersect[2] <= spczmax)){
		    newintersect[1] = spcymin;

		    if (!intersectflag){
			intersectflag = DcTrue;

			intersect_t = t;

			inter[0] = newintersect[0];
			inter[1] = newintersect[1];
			inter[2] = newintersect[2];

		    } else {
			if (t < intersect_t){
			    inter[0] = newintersect[0];
			    inter[1] = newintersect[1];
			    inter[2] = newintersect[2];
			}

			intersect[0] = inter[0];
			intersect[1] = inter[1];
			intersect[2] = inter[2];

			return(DcTrue);
		    }
		}
	    }
	}
	
	/*	 Intersect against the y=ymax surface of space.  */
	
	t = (spcymax-rayorigin[1]) * rayslope[1];

	if (t > 0.){
	    newintersect[0] = rayorigin[0] + t*raycosine[0];

	    if ((newintersect[0] >= spcxmin) && 
			(newintersect[0] <= spcxmax)){
		newintersect[2] = rayorigin[2] + t*raycosine[2];

		if ((newintersect[2] >= spczmin) && 
			(newintersect[2] <= spczmax)){
		    newintersect[1] = spcymax;

		    if (!intersectflag){
			intersectflag = DcTrue;

			intersect_t = t;

			inter[0] = newintersect[0];
			inter[1] = newintersect[1];
			inter[2] = newintersect[2];

		    } else{
			if (t < intersect_t){
			    inter[0] = newintersect[0];
			    inter[1] = newintersect[1];
			    inter[2] = newintersect[2];
			}

			intersect[0] = inter[0];
			intersect[1] = inter[1];
			intersect[2] = inter[2];

			return(DcTrue);
		    }
		}
	    }
	}
    }

    if (raycosine[2] != 0.){
	/*	 Intersect against the z=zmin surface of space.  */

	t = (spczmin-rayorigin[2]) * rayslope[2];

	if (t > 0.){
	    newintersect[0] = rayorigin[0] + t*raycosine[0];

	    if ((newintersect[0] >= spcxmin) && 
			(newintersect[0] <= spcxmax)){
		newintersect[1] = rayorigin[1] + t*raycosine[1];

		if ((newintersect[1] >= spcymin) && 
			(newintersect[1] <= spcymax)){
		    newintersect[2] = spczmin;

		    if (intersectflag == 0){
			intersectflag = DcTrue;

			intersect_t = t;

			inter[0] = newintersect[0];
			inter[1] = newintersect[1];
			inter[2] = newintersect[2];
}
		    else{
			if (t < intersect_t){
			    inter[0] = newintersect[0];
			    inter[1] = newintersect[1];
			    inter[2] = newintersect[2];
			}

			intersect[0] = inter[0];
			intersect[1] = inter[1];
			intersect[2] = inter[2];

			return(DcTrue);
		    }
		}
	    }
	}
	
	/*	 Intersect against the z=zmax surface of space.  */
	
	t = (spczmax-rayorigin[2]) * rayslope[2];

	if (t > 0.){
	    newintersect[0] = rayorigin[0] + t*raycosine[0];

	    if ((newintersect[0] >= spcxmin) && 
			(newintersect[0] <= spcxmax)){
		newintersect[1] = rayorigin[1] + t*raycosine[1];

		if ((newintersect[1] >= spcymin) && 
			(newintersect[1] <= spcymax)){
		    newintersect[2] = spczmax;

		    if (!intersectflag){
			intersectflag = DcTrue;

			intersect_t = t;

			inter[0] = newintersect[0];
			inter[1] = newintersect[1];
			inter[2] = newintersect[2];

		    } else{
			if (t < intersect_t){
			    inter[0] = newintersect[0];
			    inter[1] = newintersect[1];
			    inter[2] = newintersect[2];
			}

			intersect[0] = inter[0];
			intersect[1] = inter[1];
			intersect[2] = inter[2];

			return(DcTrue);
		    }
		}
	    }
	}
    }

    if (intersectflag){
	intersect[0] = inter[0];
	intersect[1] = inter[1];
	intersect[2] = inter[2];
    }

    return(intersectflag);
}



/* ====================================================================== */

void drr_rayint_box_ray_intersect (
    struct rayint_box *box,
    DtReal rayorigin[3],
    DtReal raycosine[3],
    DtReal rayslope[3],
    DtInt rayclass,
    DtInt *faceno,
    DtReal intersect[3])
{
    DtReal xmin = box->xmin;
    DtReal ymin = box->ymin;
    DtReal zmin = box->zmin;
    DtReal xmax = box->xmax;
    DtReal ymax = box->ymax;
    DtReal zmax = box->zmax;
    DtReal rayoriginx = rayorigin[0];
    DtReal rayoriginy = rayorigin[1];
    DtReal rayoriginz = rayorigin[2];
    DtReal raycosinex = raycosine[0];
    DtReal raycosiney = raycosine[1];
    DtReal raycosinez = raycosine[2];
    DtReal rayslopex = rayslope[0];
    DtReal rayslopey = rayslope[1];
    DtReal rayslopez = rayslope[2];
    DtReal intersectx, intersecty, intersectz;
    DtReal t;

    switch(rayclass){
	/*	 xcosine < 0., ycosine < 0., zcosine < 0.  */

    case 0:
	/* Intersect against the x=xmin surface of the box.  */

	if (raycosinex != 0.){
	    t = (xmin-rayoriginx) * rayslopex;

	    intersecty = rayoriginy + t*raycosiney;

	    if ((intersecty >= ymin) && (intersecty <= ymax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 0;

		    intersectx = xmin;

		    break;
		}
	    }
	}
		
	/*  Intersect against the y=ymin surface of the box.  */
	
	if (raycosiney != 0.){
	    t = (ymin-rayoriginy) * rayslopey;

	    intersectx = rayoriginx + t*raycosinex;

	    if ((intersectx >= xmin) && (intersectx <= xmax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 4;

		    intersecty = ymin;

		    break;
		}
	    }
	}
	
	/*  If we get here, then the ray must intersect 
	    the zmin surface of the box.  */
	
	/*  Intersect against the z=zmin surface of the box.  */
	
	if (raycosinez != 0.){
	    t = (zmin-rayoriginz) * rayslopez;

	    *faceno = 2;

	    intersectx = rayoriginx + t*raycosinex;
	    intersecty = rayoriginy + t*raycosiney;
	    intersectz = zmin;

	    break;
	}
	
	break;
	
	/*  xcosine >= 0., ycosine < 0., zcosine < 0.  */

    case 1:
	/*  Intersect against the x=xmax surface of the box.  */

	if (raycosinex != 0.){
	    t = (xmax-rayoriginx) * rayslopex;

	    intersecty = rayoriginy + t*raycosiney;

	    if ((intersecty >= ymin) && (intersecty <= ymax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 1;

		    intersectx = xmax;

		    break;
		}
	    }
	}
		
	/*  Intersect against the y=ymin surface of the box.  */
	
	if (raycosiney != 0.){
	    t = (ymin-rayoriginy) * rayslopey;

	    intersectx = rayoriginx + t*raycosinex;

	    if ((intersectx >= xmin) && (intersectx <= xmax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 4;

		    intersecty = ymin;

		    break;
		}
	    }
	}
	
	/*  If we get here, then the ray must intersect 
	    the zmin surface of the box.  */
	
	/*  Intersect against the z=zmin surface of the box.  */
	
	if (raycosinez != 0.){
	    t = (zmin-rayoriginz) * rayslopez;

	    *faceno = 2;

	    intersectx = rayoriginx + t*raycosinex;
	    intersecty = rayoriginy + t*raycosiney;
	    intersectz = zmin;

	    break;
	}
	
	break;
	
	/*  xcosine < 0., ycosine >=0., zcosine < 0.  */

    case 2:
	/*  Intersect against the x=xmin surface of the box.  */

	if (raycosinex != 0.){
	    t = (xmin-rayoriginx) * rayslopex;

	    intersecty = rayoriginy + t*raycosiney;

	    if ((intersecty >= ymin) && (intersecty <= ymax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 0;

		    intersectx = xmin;

		    break;
		}
	    }
	}
		
	/*  Intersect against the y=ymax surface of the box.  */
	
	if (raycosiney != 0.){
	    t = (ymax-rayoriginy) * rayslopey;

	    intersectx = rayoriginx + t*raycosinex;

	    if ((intersectx >= xmin) && (intersectx <= xmax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 5;

		    intersecty = ymax;

		    break;
		}
	    }
	}
	
	/*  If we get here, then the ray must intersect the 
	    zmin surface of the box.  */
 
	/*  Intersect against the z=zmin surface of the box.  */
	
	if (raycosinez != 0.){
	    t = (zmin-rayoriginz) * rayslopez;

	    *faceno = 2;

	    intersectx = rayoriginx + t*raycosinex;
	    intersecty = rayoriginy + t*raycosiney;
	    intersectz = zmin;

	    break;
	}
	
	break;
	
	/*  xcosine >= 0., ycosine >= 0., zcosine < 0.  */

    case 3:
	/*  Intersect against the x=xmax surface of the box.  */

	if (raycosinex != 0.){
	    t = (xmax-rayoriginx) * rayslopex;

	    intersecty = rayoriginy + t*raycosiney;

	    if ((intersecty >= ymin) && (intersecty <= ymax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 1;

		    intersectx = xmax;

		    break;
		}
	    }
	}
		
	/*  Intersect against the y=ymax surface of the box.  */
	
	if (raycosiney != 0.){
	    t = (ymax-rayoriginy) * rayslopey;

	    intersectx = rayoriginx + t*raycosinex;

	    if ((intersectx >= xmin) && (intersectx <= xmax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 5;

		    intersecty = ymax;

		    break;
		}
	    }
	}
	
	/*  If we get here, then the ray must intersect the 
	    zmin surface of the box.  */
	
	/*  Intersect against the z=zmin surface of the box.  */
	
	if (raycosinez != 0.){
	    t = (zmin-rayoriginz) * rayslopez;

	    *faceno = 2;

	    intersectx = rayoriginx + t*raycosinex;
	    intersecty = rayoriginy + t*raycosiney;
	    intersectz = zmin;

	    break;
	}
	
	break;
	
	/*  xcosine < 0., ycosine < 0., zcosine >= 0.  */

    case 4:
	/*  Intersect against the x=xmin surface of the box.  */

	if (raycosinex != 0.){
	    t = (xmin-rayoriginx) * rayslopex;

	    intersecty = rayoriginy + t*raycosiney;

	    if ((intersecty >= ymin) && (intersecty <= ymax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 0;

		    intersectx = xmin;

		    break;
		}
	    }
	}
		
	/*  Intersect against the y=ymin surface of the box.  */
	
	if (raycosiney != 0.){
	    t = (ymin-rayoriginy) * rayslopey;

	    intersectx = rayoriginx + t*raycosinex;

	    if ((intersectx >= xmin) && (intersectx <= xmax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 4;

		    intersecty = ymin;

		    break;
		}
	    }
	}
	
	/*  If we get here, then the ray must intersect the 
	    zmax surface of the box.  */
	
	/*  Intersect against the z=zmax surface of the box.  */
	
	if (raycosinez != 0.){
	    t = (zmax-rayoriginz) * rayslopez;

	    *faceno = 3;

	    intersectx = rayoriginx + t*raycosinex;
	    intersecty = rayoriginy + t*raycosiney;
	    intersectz = zmax;

	    break;
	}
	
	break;
	
	/*  xcosine >= 0., ycosine < 0., zcosine >= 0.  */

    case 5:
	/*  Intersect against the x=xmax surface of the box.  */

	if (raycosinex != 0.){
	    t = (xmax-rayoriginx) * rayslopex;

	    intersecty = rayoriginy + t*raycosiney;

	    if ((intersecty >= ymin) && (intersecty <= ymax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 1;

		    intersectx = xmax;

		    break;
		}
	    }
	}
		
	/*  Intersect against the y=ymin surface of the box.  */
	
	if (raycosiney != 0.){
	    t = (ymin-rayoriginy) * rayslopey;

	    intersectx = rayoriginx + t*raycosinex;

	    if ((intersectx >= xmin) && (intersectx <= xmax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 4;

		    intersecty = ymin;

		    break;
		}
	    }
	}
	
	/*  If we get here, then the ray must intersect the 
	    zmax surface of the box.  */
	
	/*  Intersect against the z=zmax surface of the box.  */
	
	if (raycosinez != 0.){
	    t = (zmax-rayoriginz) * rayslopez;

	    *faceno = 3;

	    intersectx = rayoriginx + t*raycosinex;
	    intersecty = rayoriginy + t*raycosiney;
	    intersectz = zmax;

	    break;
	}
	
	break;
	
	/*  xcosine < 0., ycosine >= 0., zcosine >= 0.  */

    case 6:
	/*  Intersect against the x=xmin surface of the box.  */

	if (raycosinex != 0.){
	    t = (xmin-rayoriginx) * rayslopex;

	    intersecty = rayoriginy + t*raycosiney;

	    if ((intersecty >= ymin) && (intersecty <= ymax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 0;

		    intersectx = xmin;

		    break;
		}
	    }
	}
		
	/*  Intersect against the y=ymax surface of the box.  */
	
	if (raycosiney != 0.){
	    t = (ymax-rayoriginy) * rayslopey;

	    intersectx = rayoriginx + t*raycosinex;

	    if ((intersectx >= xmin) && (intersectx <= xmax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 5;

		    intersecty = ymax;

		    break;
		}
	    }
	}
	
	/*  If we get here, then the ray must intersect the 
 	    zmax surface of the box.  */
	
	/*  Intersect against the z=zmax surface of the box.  */
	
	if (raycosinez != 0.){
	    t = (zmax-rayoriginz) * rayslopez;

	    *faceno = 3;

	    intersectx = rayoriginx + t*raycosinex;
	    intersecty = rayoriginy + t*raycosiney;
	    intersectz = zmax;

	    break;
	}
	
	break;
	
	/*  xcosine >= 0., ycosine >= 0., zcosine >= 0.  */

    case 7:
	/*  Intersect against the x=xmax surface of the box.  */

	if (raycosinex != 0.){
	    t = (xmax-rayoriginx) * rayslopex;

	    intersecty = rayoriginy + t*raycosiney;

	    if ((intersecty >= ymin) && (intersecty <= ymax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 1;

		    intersectx = xmax;

		    break;
		}
	    }
	}
		
	/*  Intersect against the y=ymax surface of the box.  */
	
	if (raycosiney != 0.){
	    t = (ymax-rayoriginy) * rayslopey;

	    intersectx = rayoriginx + t*raycosinex;

	    if ((intersectx >= xmin) && (intersectx <= xmax)){
		intersectz = rayoriginz + t*raycosinez;

		if ((intersectz >= zmin) && (intersectz <= zmax)){
		    *faceno = 5;

		    intersecty = ymax;

		    break;
		}
	    }
	}
	
	/*  If we get here, then the ray must intersect the 
	    zmax surface of the box.  */
	
	/*  Intersect against the z=zmax surface of the box.  */
	
	if (raycosinez != 0.){
	    t = (zmax-rayoriginz) * rayslopez;

	    *faceno = 3;

	    intersectx = rayoriginx + t*raycosinex;
	    intersecty = rayoriginy + t*raycosiney;
	    intersectz = zmax;

	    break;
	}
	
	break;

    default:
	sprintf (DDerror_GetLine(), "rayclass %d",
		 rayclass);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "drr_rayint_box_ray_intersect", 
		    DDerror_GetLine());

	intersectx = 0.;
	intersecty = 0.;
	intersectz = 0.;

	break;
    }

    intersect[0] = intersectx;
    intersect[1] = intersecty;
    intersect[2] = intersectz;
}



/* ====================================================================== */

DtFlag drr_rayint_calculate_object_intersect (
    struct rayint_box *box,
    struct list       *rayintobjectlist,
    struct ray        *ray,
    DtFlag             anyintersectswitch,
    struct intersect  *intersect)
{
    DtReal boxxmin = box->xmin;
    DtReal boxymin = box->ymin;
    DtReal boxzmin = box->zmin;
    DtReal boxxmax = box->xmax;
    DtReal boxymax = box->ymax;
    DtReal boxzmax = box->zmax;
    struct list_node *rayintobjectlistnode;
    struct rayint_object *rayintobject;
    struct rayint_object_modifier *rayintobjectmodifier;
    struct ray lcsraystruct;
    struct ray *lcsray = &lcsraystruct;
    DtFlag intersectflag;
    DtReal length;
    DtMethodPtr rayintmethod;
    DtInt firstintersectflag = DcFalse;
    struct intersect closest_intersect;
    DtReal closest_distance;
    DtReal distance[3];
    DtReal intersectdistance;

    if (rayintobjectlist == NULL)
	    return(DcFalse);

    /*  Loop through all the rayint objects, checking for the
	closest one which intersects the ray.  Note that we 
	cannot use the data_get utilities here, since this 
	routine may be called recursively, or may call another 
	routine that needs to look through the ray object list.  */

    for (rayintobjectlistnode =
	 rayintobjectlist->list_header_node->list_node_next;
	 rayintobjectlistnode != rayintobjectlist->list_header_node;
	 rayintobjectlistnode = rayintobjectlistnode->list_node_next){
	rayintobject = (struct rayint_object *)rayintobjectlistnode->
		list_node_data;

	/*
	  if (rayintobject == ray->origin_rayint_object)
	  continue;
	  */

	rayintobjectmodifier = rayintobject->modifier;

	if (rayintobjectmodifier->wcstolcsmat != NULL){
	    /*  Transform the ray's location and direction
		cosines from wcs to the object's lcs.  */

	    dor_math_pnttrns_fast (ray->wcsorigin,
			  *(rayintobjectmodifier->wcstolcsmat),
			  lcsray->wcsorigin);

	    dor_math_vectrns_fast (ray->wcsdirection,
			  *(rayintobjectmodifier->wcstolcsmat),
			  lcsray->wcsdirection);

	    /*  Renormalize the direction cosines of the
		transformed ray to take into account possible
		scaling effects.  */

	    length = 
		(lcsray->wcsdirection[0])*(lcsray->wcsdirection[0]) +
		(lcsray->wcsdirection[1])*(lcsray->wcsdirection[1]) +
		(lcsray->wcsdirection[2])*(lcsray->wcsdirection[2]);

	    if (length == 0.){
		DDerror (ERR_ZERO_LEN_VEC,
			    "drr_rayint_calculate_object_intersection", 
			    DcNullPtr);

		lcsray->wcsdirection[0] = 0.;
		lcsray->wcsdirection[1] = 0.;
		lcsray->wcsdirection[2] = 0.;
	    }

	    length = 1. / dor_math_sqrt(length);

	    lcsray->wcsdirection[0] *= length;
	    lcsray->wcsdirection[1] *= length;
	    lcsray->wcsdirection[2] *= length;

	    /*  Find the intersection point between the ray
		and the object.  The generic primitive object
		ray intersection function only determines if
		there is an intersection and if so, sets the
		intersection point and surface normal at
		intersection point portions of the intersect
		structure.  It is not responsible for setting
		the rayintobject portion of the intersect
		structure, that must be done here.  */

	    rayintmethod = DDobject_InqMethod(rayintobject->object,
				       dre_glbrnd_data.Rayint_method);
	    intersectflag = (DtFlag)(*rayintmethod)
				(rayintobject->object,lcsray,intersect);

	} else{
	    /*  Find the intersection point between the ray
		and the object.  The generic primitive object
		ray intersection function only determines if
		there is an intersection and if so, sets the
		intersection point and surface normal at
		intersection point portions of the intersect
		structure.  It is not responsible for setting
		the rayintobject portion of the intersect
		structure, that must be done here.  */

	    rayintmethod = DDobject_InqMethod(rayintobject->object,
				 	dre_glbrnd_data.Rayint_method);
	    intersectflag = (DtFlag)(*rayintmethod)
				(rayintobject->object,ray,intersect);
	}

	if (!intersectflag)
		continue;

	if (rayintobjectmodifier->lcstowcsmat != NULL){
	    /*  Transform the point of intersection back to
		the world coordinate system.  */

	    dor_math_pnttrns_fast(intersect->lcsloc,
				*(rayintobjectmodifier->lcstowcsmat),
				intersect->wcsloc);

	} else{
	    intersect->wcsloc[0] = intersect->lcsloc[0];
	    intersect->wcsloc[1] = intersect->lcsloc[1];
	    intersect->wcsloc[2] = intersect->lcsloc[2];
	}

	/*  Check to make sure that the ray-object intersection
	    point in the world coordinate system is actually within
	    the world coordinate system box which we are
	    interrogating.  If not, skip this intersection point
	    for this box.  */

	if (!((intersect->wcsloc[0] >= boxxmin) &&
	      (intersect->wcsloc[0] <= boxxmax) &&
	      (intersect->wcsloc[1] >= boxymin) &&
	      (intersect->wcsloc[1] <= boxymax) &&
	      (intersect->wcsloc[2] >= boxzmin) &&
	      (intersect->wcsloc[2] <= boxzmax))){
	    continue;
	}

	/*  Compute the squared distance between the ray origin 
	    and the wcs intersection point.  */

	distance[0] = ray->wcsorigin[0] - intersect->wcsloc[0];
	distance[1] = ray->wcsorigin[1] - intersect->wcsloc[1];
	distance[2] = ray->wcsorigin[2] - intersect->wcsloc[2];

	intersectdistance = distance[0]*distance[0] + 
				distance[1]*distance[1] +
				distance[2]*distance[2];

	/*  If the distance from the origin of the ray to the
	    point of intersection is less than a certain amount,
	    then we have probably found an intersection with the
	    originating object, and should disregard it.  This
	    check is more generally correct than just rejecting
	    intersections based on the rayint_object tag, since a
	    rayint_object could generate more than one intersection
	    with itself (ie. rays transmitted through a sphere
	    object), or you can get cases where a spatial point
	    lies on two objects (ie. on the boundary between two
	    triangles), and a shadow ray is cast back and fourth
	    between the two objects at the same point forever.
	    This test, based on intersection distance, takes care
	    of the latter problem, and does not cause the former.
	    */

	if (intersectdistance < .00001)
		continue;

	/*  If the anyintersectswitch is on, then we merely
	    want to determine the presence of any intersecting
	    object, and we do not care where and what it is.  Thus,
	    since we have just found an intersecting object, just
	    set the intersect flag and return.  */

	if (anyintersectswitch)
		return(DcTrue);

	/*  If there was previously no intersection found for
	    this ray in this box, then remember this intersection
	    in the closest_intersect structure.  Note that we
	    remember the location of the intersection point in the
	    world coordinate system, not the local coordinate
	    system value returned by the primitive object ray
	    intersection function.  All other information in the
	    intersect structure is retained exactly as returned by
	    the primitive object's ray intersection method.  */

	if (!firstintersectflag){
	    /* Copy all values from intersect to closest_intersect. */

	    closest_intersect = *intersect;

	    closest_intersect.rayint_object = rayintobject;

	    closest_distance = intersectdistance;

	    firstintersectflag = DcTrue;
	}

	/*  If there exists a previously found intersection of
	    the ray with a primitive object in this box, then test
	    to see if the new intersection is closer than the
	    previous one, and if so, remember the new one.  */

	else if (intersectdistance < closest_distance){
	    /* Copy all values from intersect to closest_intersect. */

	    closest_intersect = *intersect;

	    closest_intersect.rayint_object = rayintobject;

	    closest_distance = intersectdistance;
	}
    }

    if (!firstintersectflag){
	intersect->rayint_object = NULL;

	return(DcFalse);
    } else{
	/*  Copy all values from closest_intersect to intersect.  */

	*intersect = closest_intersect;

	return(DcTrue);
    }
}
