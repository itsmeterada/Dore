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
	int drr_glbshd_shade_raytraced (rayintobject,ray,intersect,color)
	int drr_glbshd_shade (color)
	int drr_glbshd_shade_light (lightno,color)
	int drr_glbshd_reflection_component (color)
	int drr_glbshd_transmitted_component (color)
	DtInt drr_glbshd_cast_shadow (lightvector,lightcolor)
	DtFlag drr_glbshd_transmit_shadow (ray,intersect,color)
	int drr_glbshd_h_normal (viwdir,srftolgtnrm,hnrm)

  ======================================================================
 */
#include "glbshd.h"

/*
 ======================================================================
 */

void drr_glbshd_shade_raytraced (
    struct rayint_object *rayintobject,
    struct ray *ray,
    struct intersect *intersect,
    DtColorRGB color)
{
    struct glbshd_object_data *prevobjectdata;
    struct glbshd_surface_data *prevsurfacedata;
    struct glbshd_surface_data newsurfacedata;
    DtMethodPtr executemethod;

    /*  Stack the object and surface data pointers, the object 
	data pointer pointing to the data structure associated 
	with the object being shaded, and the surface data pointer 
	pointing to the new surface data structure allocated on the 
	local stack.  This stacking is performed so that recursive 
	shading will work, the shading subsystem uses these two
	pointers to determine all of its working data.  */

    prevobjectdata = dre_glbshd_object_data;
    dre_glbshd_object_data = (struct glbshd_object_data *)
				rayintobject->modifier->shade_data;

    prevsurfacedata = dre_glbshd_surface_data;
    dre_glbshd_surface_data = &newsurfacedata;

    /*  Fill in element specific data in the surface point data
	structure by requesting the information from the 
	rayintobject.  Set the flags in the surface point data 
	structure to indicate whether or not the element specific 
	data was available from the rayintobject.  */

    dre_glbshd_surface_data->ray = ray;
    dre_glbshd_surface_data->intersect = intersect;

    executemethod = DDobject_InqMethod (rayintobject->object,
				dre_glbrnd_data.UsrDat_method);
    dre_glbshd_surface_data->userdata_available = 
		(DtFlag)(*executemethod)
		(rayintobject->object,ray,intersect,
		     dre_glbshd_surface_data->userdata);

    executemethod = DDobject_InqMethod(rayintobject->object,
				dre_glbrnd_data.Wcsnrm_method);
    dre_glbshd_surface_data->wcsnrm_available = 
		(DtFlag)(*executemethod)
		(rayintobject->object,ray,intersect,
		     dre_glbshd_surface_data->wcsnrm);

    /*
      executemethod = DDobject_InqMethod(rayintobject->object,
      dre_glbrnd_data.Wcsloc_method);
      dre_glbshd_surface_data->wcsloc_available = (DtFlag)
      (*executemethod)(rayintobject->object,ray,intersect,
      dre_glbshd_surface_data->wcsloc);
      */

    /*  The wcs location of the intersection is always computed 
	by the intersection routine.  */

    dre_glbshd_surface_data->wcsloc[0] = intersect->wcsloc[0];
    dre_glbshd_surface_data->wcsloc[1] = intersect->wcsloc[1];
    dre_glbshd_surface_data->wcsloc[2] = intersect->wcsloc[2];
    dre_glbshd_surface_data->wcsloc_available = DcTrue;

    drr_glbshd_shade(color);

    /*  Unstack the shader data structure pointers.  */

    dre_glbshd_object_data = prevobjectdata;

    dre_glbshd_surface_data = prevsurfacedata;
}
/*
 ======================================================================
 */

void drr_glbshd_shade (DtColorRGB color)
{
    struct glbshd_object_data dynamicobjectdata;
    struct glbshd_object_data *storedobjectdata;
    DtInt lightno;

    /*  Copy the object data to the dynamic data structure so 
	that this data can be modified during the shading process 
	without affecting the values stored with the object.  */

    dynamicobjectdata = *dre_glbshd_object_data;
    storedobjectdata = dre_glbshd_object_data;
    dre_glbshd_object_data = &dynamicobjectdata;

    color[0] = 0.;
    color[1] = 0.;
    color[2] = 0.;

    if (dre_glbshd_object_data->ambswi){
	if (dre_glbshd_surface_data->userdata_available){
	    color[0] += dre_glbshd_surface_data->userdata[0] *
		    dre_glbshd_light_data.ambient_reflectance[0] *
			    dre_glbshd_object_data->ambint;
	    color[1] += dre_glbshd_surface_data->userdata[1] * 
		    dre_glbshd_light_data.ambient_reflectance[1] *
			    dre_glbshd_object_data->ambint;
	    color[2] += dre_glbshd_surface_data->userdata[2] *
		    dre_glbshd_light_data.ambient_reflectance[2] *
			    dre_glbshd_object_data->ambint;
	} else {
	    color[0] += dre_glbshd_object_data->difclr[0] * 
		    dre_glbshd_light_data.ambient_reflectance[0] *
			    dre_glbshd_object_data->ambint;
	    color[1] += dre_glbshd_object_data->difclr[1] *
		    dre_glbshd_light_data.ambient_reflectance[1] *
			    dre_glbshd_object_data->ambint;
	    color[2] += dre_glbshd_object_data->difclr[2] *
		    dre_glbshd_light_data.ambient_reflectance[2] *
			    dre_glbshd_object_data->ambint;
	}
    }

    if (dre_glbshd_object_data->difswi || 
		dre_glbshd_object_data->spcswi){
	for (lightno=0;lightno<dre_glbshd_static_data.lightcount;lightno++){
	    drr_glbshd_shade_light(lightno,color);
	}
    }

    if (dre_glbshd_object_data->transpswi){
	drr_glbshd_transmitted_component(color);
    }

    if (dre_glbshd_object_data->reflectswi){
	drr_glbshd_reflection_component(color);
    }

    color[0] = min(color[0],.99);
    color[1] = min(color[1],.99);
    color[2] = min(color[2],.99);

    /*  Restore the pointer to the stored object data.  */

    dre_glbshd_object_data = storedobjectdata;
}
/*
 ======================================================================
 */

void drr_glbshd_shade_light (
    DtInt lightno,
    DtColorRGB color)
{
    DtReal diffuseint;
    DtInt shadowtype;
    DtColorRGB lightint;
    DtReal RdotL;
    DtReal specularint;
    DtReal angspreadfactor;

    diffuseint = dor_math_dotproduct(
			dre_glbshd_surface_data->wcsnrm[0],
			dre_glbshd_surface_data->wcsnrm[1],
			dre_glbshd_surface_data->wcsnrm[2],
			dre_glbshd_light_data.cosine[lightno][0],
			dre_glbshd_light_data.cosine[lightno][1],
			dre_glbshd_light_data.cosine[lightno][2]);

    /*  If the dot product of the surface normal and the light
	direction is less than zero, then the light does not 
	contribute to the shading of the surface point, so return.  */

    if (diffuseint <= 0.)
	    return;

    diffuseint *= dre_glbshd_object_data->difint;

    /*  Initial light int (rgb) is taken directly from the light 
	source data.  The reason we convert a single int into an 
	rgb int here is that the color balance of the light might 
	be altered by being transmitted through translucent materials.  
	Also, we will use colored lights in the future anyway.  */

    lightint[0] = dre_glbshd_light_data.intens[lightno];
    lightint[1] = dre_glbshd_light_data.intens[lightno];
    lightint[2] = dre_glbshd_light_data.intens[lightno];


    /*  Seek out shadows.  */

    if (dre_glbshd_object_data->shadowswi == 1){
	shadowtype = drr_glbshd_cast_shadow(
			    &dre_glbshd_light_data.cosine[lightno][0],
			    lightint);

	/*  If a "hard" shadow was encountered, then just give up
	    for this light source.  If not, then either no shadow 
	    at all was encounteed, in which case the lightint will 
	    have been unmodified, or a "soft" shadow was encounted, 
	    in which case the lightint was modified accordingly.  */

	if (shadowtype == 2)
		return;
    }

    if (dre_glbshd_object_data->difswi){
	if (dre_glbshd_surface_data->userdata_available){
	    color[0] += dre_glbshd_surface_data->userdata[0] *
		    dre_glbshd_object_data->difclr[0] * diffuseint *
			    dre_glbshd_light_data.color[lightno][0] *
				    lightint[0];
	    color[1] += dre_glbshd_surface_data->userdata[1] *
		    dre_glbshd_object_data->difclr[1] * diffuseint *
			    dre_glbshd_light_data.color[lightno][1] *
				    lightint[1];
	    color[2] += dre_glbshd_surface_data->userdata[2] *
		    dre_glbshd_object_data->difclr[2] * diffuseint *
			    dre_glbshd_light_data.color[lightno][2] *
				    lightint[2];
	} else {
	    color[0] += dre_glbshd_object_data->difclr[0] *
		    diffuseint *
			    dre_glbshd_light_data.color[lightno][0] *
				    lightint[0];
	    color[1] += dre_glbshd_object_data->difclr[1] *
		    diffuseint *
			    dre_glbshd_light_data.color[lightno][1] *
				    lightint[1];
	    color[2] += dre_glbshd_object_data->difclr[2] *
		    diffuseint *
			    dre_glbshd_light_data.color[lightno][2] *
				    lightint[2];
	}
    }

    if (!dre_glbshd_object_data->spcswi)
	    return;

    /*  Compute the h direction based on the viewer (incoming ray) 
	direction and the direction to the infinitely distant light 
	source.  */

    drr_glbshd_h_normal(dre_glbshd_surface_data->ray->wcsdirection,
			dre_glbshd_light_data.cosine[lightno],
			dre_glbshd_light_data.h[lightno]);

    RdotL = dor_math_dotproduct(dre_glbshd_light_data.h[lightno][0],
				dre_glbshd_light_data.h[lightno][1],
				dre_glbshd_light_data.h[lightno][2],
				dre_glbshd_surface_data->wcsnrm[0],
				dre_glbshd_surface_data->wcsnrm[1],
				dre_glbshd_surface_data->wcsnrm[2]);

    if (RdotL < 0.){
	return;
    } else {
	angspreadfactor = dor_math_pow(RdotL,
				       dre_glbshd_object_data->spcfct);

	specularint = angspreadfactor * dre_glbshd_object_data->spcint;
    }

    if (dre_glbshd_surface_data->userdata_available){
	color[0] += dre_glbshd_surface_data->userdata[0] *
		dre_glbshd_object_data->spcclr[0] * specularint *
			dre_glbshd_light_data.color[lightno][0] *
				lightint[0];
	color[1] += dre_glbshd_surface_data->userdata[1] *
		dre_glbshd_object_data->spcclr[1] * specularint *
			dre_glbshd_light_data.color[lightno][1] *
				lightint[1];
	color[2] += dre_glbshd_surface_data->userdata[2] *
		dre_glbshd_object_data->spcclr[2] * specularint *
			dre_glbshd_light_data.color[lightno][2] *
				lightint[2];
    } else {
	color[0] += dre_glbshd_object_data->spcclr[0] *
		specularint *
			dre_glbshd_light_data.color[lightno][0] *
				lightint[0];
	color[1] += dre_glbshd_object_data->spcclr[1] *
		specularint *
			dre_glbshd_light_data.color[lightno][1] *
				lightint[1];
	color[2] += dre_glbshd_object_data->spcclr[2] *
		specularint *
			dre_glbshd_light_data.color[lightno][2] *
				lightint[2];
    }
}
/*
 ======================================================================
 */

void drr_glbshd_reflection_component (DtColorRGB color)
{
    struct ray reflectedray;
    DtReal indir[3];
    DtReal wcsnrm[3];
    DtReal indotnorm;
    DtReal refred, refgrn, refblu;
    DtFlag renderflag;

    if (dre_glbshd_static_data.raylvl > dre_glbshd_static_data.maxraylvl){
	return;
    }

    /*
      reflectedray.origin_rayint_object = dre_glbshd_surface_data->
      intersect->rayint_object;
      */

    reflectedray.current_box = 
		dre_glbshd_surface_data->ray->current_box;

    /*  Initialize the reflected ray created on the stack.  
	This will be the mirror ray from the surface point being 
	shaded in the mirror direction from the incoming viewer ray. */

    reflectedray.wcsorigin[0] = dre_glbshd_surface_data->wcsloc[0];
    reflectedray.wcsorigin[1] = dre_glbshd_surface_data->wcsloc[1];
    reflectedray.wcsorigin[2] = dre_glbshd_surface_data->wcsloc[2];

    /*  Calculate the direction cosines for the reflected ray.  */

    indir[0] = dre_glbshd_surface_data->ray->wcsdirection[0];
    indir[1] = dre_glbshd_surface_data->ray->wcsdirection[1];
    indir[2] = dre_glbshd_surface_data->ray->wcsdirection[2];

    wcsnrm[0] = dre_glbshd_surface_data->wcsnrm[0];
    wcsnrm[1] = dre_glbshd_surface_data->wcsnrm[1];
    wcsnrm[2] = dre_glbshd_surface_data->wcsnrm[2];

    indotnorm = dor_math_dotproduct(-indir[0],-indir[1],-indir[2],
				    wcsnrm[0],wcsnrm[1],wcsnrm[2]);

    if (indotnorm == 0.){
	reflectedray.wcsdirection[0] = indir[0];
	reflectedray.wcsdirection[1] = indir[1];
	reflectedray.wcsdirection[2] = indir[2];
    } else {
	reflectedray.wcsdirection[0] = 
				2.*indotnorm*wcsnrm[0] + indir[0];
	reflectedray.wcsdirection[1] = 
				2.*indotnorm*wcsnrm[1] + indir[1];
	reflectedray.wcsdirection[2] = 
				2.*indotnorm*wcsnrm[2] + indir[2];
    }

    dor_math_renormalize_fast(reflectedray.wcsdirection);

    dre_glbshd_static_data.raylvl++;

    renderflag = drr_recrnd_render_ray(&reflectedray,
					&refred,&refgrn,&refblu);

    dre_glbshd_static_data.raylvl--;

    if (renderflag){
	color[0] += dre_glbshd_object_data->spcclr[0] * refred *
			dre_glbshd_object_data->spcint;
	color[1] += dre_glbshd_object_data->spcclr[1] * refgrn *
			dre_glbshd_object_data->spcint;
	color[2] += dre_glbshd_object_data->spcclr[2] * refblu *
			dre_glbshd_object_data->spcint;
    }
}
/*
 ======================================================================
 */

void drr_glbshd_transmitted_component (DtColorRGB color)
{
    struct ray transmittedray;
    DtReal trnred, trngrn, trnblu;
    DtFlag renderflag;

    if (dre_glbshd_static_data.raylvl > 
		dre_glbshd_static_data.maxraylvl){
	return;
    }

    /*
      transmittedray.origin_rayint_object = 
		dre_glbshd_surface_data->intersect->rayint_object;
      */

    transmittedray.current_box = 
		dre_glbshd_surface_data->ray->current_box;

    transmittedray.wcsorigin[0] = dre_glbshd_surface_data->wcsloc[0];
    transmittedray.wcsorigin[1] = dre_glbshd_surface_data->wcsloc[1];
    transmittedray.wcsorigin[2] = dre_glbshd_surface_data->wcsloc[2];

    transmittedray.wcsdirection[0] =
	    dre_glbshd_surface_data->ray->wcsdirection[0];
    transmittedray.wcsdirection[1] =
	    dre_glbshd_surface_data->ray->wcsdirection[1];
    transmittedray.wcsdirection[2] =
	    dre_glbshd_surface_data->ray->wcsdirection[2];

    dre_glbshd_static_data.raylvl++;

    renderflag = drr_recrnd_render_ray(&transmittedray,
					&trnred,&trngrn,&trnblu);

    dre_glbshd_static_data.raylvl--;

    if (renderflag){
	color[0] += dre_glbshd_object_data->transpclr[0] * 
			trnred * dre_glbshd_object_data->transpint;
	color[1] += dre_glbshd_object_data->transpclr[1] * 
			trngrn * dre_glbshd_object_data->transpint;
	color[2] += dre_glbshd_object_data->transpclr[2] * 
			trnblu * dre_glbshd_object_data->transpint;
    } else {
	color[0] += dre_glbshd_object_data->transpclr[0] *
			dre_glbshd_object_data->transpint *
			dre_glbshd_static_data.background_color[0];
	color[1] += dre_glbshd_object_data->transpclr[1] *
			dre_glbshd_object_data->transpint *
			dre_glbshd_static_data.background_color[1];
	color[2] += dre_glbshd_object_data->transpclr[2] * 
			dre_glbshd_object_data->transpint *
			dre_glbshd_static_data.background_color[2];
    }
}
/*
 ======================================================================
 */

DtInt drr_glbshd_cast_shadow (
    DtReal lightvector[3],
    DtColorRGB lightcolor)
{
    struct ray ray;
    struct intersect intersect;
    DtFlag intersectflag;
    DtFlag transmitflag;

    ray.wcsorigin[0] = dre_glbshd_surface_data->wcsloc[0];
    ray.wcsorigin[1] = dre_glbshd_surface_data->wcsloc[1];
    ray.wcsorigin[2] = dre_glbshd_surface_data->wcsloc[2];

    ray.wcsdirection[0] = lightvector[0];
    ray.wcsdirection[1] = lightvector[1];
    ray.wcsdirection[2] = lightvector[2];

    /*
    ray.origin_rayint_object = 
		dre_glbshd_surface_data->intersect->rayint_object;
    */

    ray.current_box = dre_glbshd_surface_data->ray->current_box;

    intersectflag = drr_rayint_intersect(&ray,0,&intersect);

    /*  If there was no initial intersection with a surface, then
	there is no shadow, hard or soft, so just return a flag
	indicating no shadow was found.  */

    if (!intersectflag){
	return(0);
    }

    /*  A surface was intersected by the shadow ray.  Ask the
	surface to transmit the shadow ray as far towards the 
	light source as possible.  This transmission will occur 
	recursively, affecting the light source color passed in 
	on the way, until either the light is reached, in which 
	case the routine will return DcTrue, or until a 
	non-transmitting surface is reached, in which case it 
	will return DcFalse.  */

    transmitflag = drr_glbshd_transmit_shadow(&ray,&intersect,lightcolor);

    if (transmitflag)
	    return(1);
    else
	    return(2);
}
/*
 ======================================================================
 */

    /*  A shadow ray has intersected this surface.  If the surface
	is opaque, then the shadow ray is "stopped" at this surface,
	and we return a flag indicating non-transmission.  If the
	surface is translucent, then we continue to trace the ray in
	its path to to the light, by calling ourselves recursively.  
	If the recursive call indicates that the shadow ray was 
	stopped, then return the stopped flag.  Otherwise, tint the 
	shadow color by the transmittance of this surface, and return 
	a "transmitted" flag.  */

DtFlag drr_glbshd_transmit_shadow (
    struct ray *ray,
    struct intersect *intersect,
    DtReal color[3])
{
    struct glbshd_object_data *objectdata;
    DtFlag intersectflag;
    struct ray shadowray;
    struct intersect shadowintersect;
    DtFlag transmitflag;
    /*
      DtPFI executemethod;
    */

    objectdata = (struct glbshd_object_data *)
		 intersect->rayint_object->modifier->shade_data;

    if (!objectdata->transpswi){
	/* Indicate that this surface has "stopped" the shadow ray. */

	return(DcFalse);
    }

    /*
    shadowray.origin_rayint_object = intersect->rayint_object;
    */

    shadowray.current_box = ray->current_box;

    /*  Ask the object for the world coordinate system location of 
	the intersection point.  */

    /*
      executemethod = DDobject_InqMethod(
			intersect->rayint_object->object,
      			dre_glbrnd_data.Wcsloc_method);
      (*executemethod)(intersect->rayint_object->object,
			ray,intersect,shadowray.wcsorigin);
    */

    /*  The wcs location of the intersection is always computed by
	the intersection routine.  */

    shadowray.wcsorigin[0] = intersect->wcsloc[0];
    shadowray.wcsorigin[1] = intersect->wcsloc[1];
    shadowray.wcsorigin[2] = intersect->wcsloc[2];

    shadowray.wcsdirection[0] = ray->wcsdirection[0];
    shadowray.wcsdirection[1] = ray->wcsdirection[1];
    shadowray.wcsdirection[2] = ray->wcsdirection[2];

    intersectflag = drr_rayint_intersect(&shadowray,0,&shadowintersect);

    if (intersectflag){
	transmitflag = drr_glbshd_transmit_shadow(&shadowray,
						&shadowintersect,
						color);

	if (!transmitflag){
	    return(DcFalse);
	}
    }

    color[0] = color[0] * 
		objectdata->transpclr[0] * 
		objectdata->transpint;
    color[1] = color[1] * 
		objectdata->transpclr[1] * 
		objectdata->transpint;
    color[2] = color[2] * 
		objectdata->transpclr[2] * 
		objectdata->transpint;

    /* Indicate that the shadow was transmitted 
	through this surface.  */

    return(DcTrue);
}
/*
 ======================================================================
 */

void drr_glbshd_h_normal (
    DtReal viwdir[3],
    DtReal srftolgtnrm[3],
    DtReal hnrm[3])
{
    /*
      hnrm[0] = srftolgtnrm[0] + viwdir[0];
      hnrm[1] = srftolgtnrm[1] + viwdir[1];
      hnrm[2] = srftolgtnrm[2] + viwdir[2];
      */

    hnrm[0] = srftolgtnrm[0] - viwdir[0];
    hnrm[1] = srftolgtnrm[1] - viwdir[1];
    hnrm[2] = srftolgtnrm[2] - viwdir[2];

    dor_math_renormalize_fast(hnrm);
}
