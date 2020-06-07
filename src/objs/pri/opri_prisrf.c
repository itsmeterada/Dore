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
	dor_prisrf_initialize
	dor_prisrf_create
	dor_prisrf_destroy
	dor_prisrf_print
	dor_prisrf_update_alternate
	dor_prisrf_create_alternate_geometry
	dor_prisrf_create_alternate_geometry_box
	dor_prisrf_create_alternate_geometry_cone
	dor_prisrf_create_alternate_geometry_cylinder
	dor_prisrf_create_alternate_geometry_sphere
	dor_prisrf_sphere_triangles
	dor_prisrf_sphere_ucomp
	dor_prisrf_sphere_vcomp
	dor_prisrf_check_subdiv
	dor_prisrf_compuv_usercallback
	dor_prisrf_compute_bounding_volume
	dor_prisrf_pick
	dor_prisrf_render_standard

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/prisrf.h>
#include <dore/dore_develop/private/trilst.h>
#include <dore/dore_develop/private/trimsh.h>
#include <math.h>

#define TWO_PI		(M_PI * 2.)

DtInt doe_prisrf_class_id;
DtFlag doe_prisrf_initialized = DcFalse;

/*
 ======================================================================
 */

static DtMethodEntry prisrf_methods[] = {
    {DcMethodCmpBndVolume,    (DtMethodPtr)dor_prisrf_compute_bounding_volume },
    {DcMethodDestroy,         (DtMethodPtr)dor_prisrf_destroy },
    {DcMethodPick,            (DtMethodPtr)dor_prisrf_pick },
    {DcMethodPrint,           (DtMethodPtr)dor_prisrf_print },
    {DcMethodUpdStdAltObj,    (DtMethodPtr)dor_prisrf_update_alternate },
    {DcMethodStdRenderDisplay,(DtMethodPtr)dor_prisrf_render_standard }
};

void dor_prisrf_initialize (void)
{
    doe_prisrf_class_id
	= dor_class_install (DcTypePrimitiveSurface, "DoPrimSurf", 6,
			     prisrf_methods, DcNullPtr);

    doe_prisrf_initialized = DcTrue;
}
/*
 ======================================================================
 */

dot_object *dor_prisrf_create (DtSurface surfacetype)
{
    struct prisrf *prisrf;
    dot_object *prisrfobject;

    if (!doe_prisrf_initialized) {
      	dor_prisrf_initialize();
    }

    prisrf = (struct prisrf *)dor_space_allocate(sizeof *prisrf);

    prisrf->surface_type = surfacetype;

    prisrf->alternate_object = DcNullPtr;

    prisrfobject = dor_object_create(doe_prisrf_class_id,prisrf);

    return(prisrfobject);
}
/*
 ======================================================================
 */

void dor_prisrf_destroy (dot_object *object)
{
    struct prisrf *prisrf;

    prisrf = (struct prisrf *)(object->data);

    if (prisrf->alternate_object != NULL){
	dor_object_check_deletion(prisrf->alternate_object);
    }

    dor_space_deallocate(prisrf);
}
/*
 ======================================================================
 */

void dor_prisrf_print (dot_object *object)
{
    struct prisrf *prisrf;

    prisrf = (struct prisrf *)object->data;

    if (prisrf == NULL){
	dor_print_output("bad values");
	return;
    }

    switch (prisrf->surface_type){
    case DcSphere:
	dor_print_output("type = DcSphere");
	break;

    case DcCylinder:
	dor_print_output("type = DcCylinder");
	break;

    case DcBox:
	dor_print_output("type = DcBox");
	break;

    case DcCone:
	dor_print_output("type = DcCone");
	break;

    default:
	dor_print_output("type = UNKNOWN");
	break;
    }
}
/*
 ======================================================================
 */

dot_object *dor_prisrf_update_alternate (dot_object *object)
{
    struct prisrf *prisrf;
    DtFlag changed;
    dot_object *alt;

    prisrf = (struct prisrf *)(object->data);

    changed = dor_prisrf_check_subdiv(prisrf);

    if ((prisrf->alternate_object != DcNullPtr) && !changed) {
	alt = prisrf->alternate_object;

    } else {
	if (prisrf->alternate_object != DcNullPtr) {
	    dor_object_get_method(prisrf->alternate_object,
				  DcMethodDestroy)(prisrf->alternate_object);

	    prisrf->alternate_object = DcNullPtr;
	}

	prisrf->alternate_object =
		dor_prisrf_create_alternate_geometry(prisrf);
	alt = prisrf->alternate_object;

	if (prisrf->alternate_object == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_prisrf_update_alternate", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_prisrf_create_alternate_geometry (struct prisrf *prisrf)
{
    DtReal subdivlevel[1];
    DtInt subdivtype;
    DtSwitch cmptexuvswi, cmptexuvwswi;

    dor_global_subdivspc_get_value(&subdivtype, subdivlevel);
    if (subdivtype != DcSubDivSegments) {
	/* old style subdivision spec */
	subdivlevel[0] = max(subdivlevel[0],0.);
	subdivlevel[0] = min(subdivlevel[0],1.);
    } else {
	subdivlevel[0] = max(subdivlevel[0],4.);
    }

    dor_global_cmptexuvswi_get_value(&cmptexuvswi);
    dor_global_cmptexuvwswi_get_value(&cmptexuvwswi);

    switch(prisrf->surface_type){
    case DcSphere:
	return(dor_prisrf_create_alternate_geometry_sphere(subdivtype,
		subdivlevel[0], cmptexuvswi, cmptexuvwswi));
	break;

    case DcCylinder:
	return(dor_prisrf_create_alternate_geometry_cylinder(subdivtype,
		subdivlevel[0], cmptexuvswi, cmptexuvwswi));
	break;

    case DcBox:
	return(dor_prisrf_create_alternate_geometry_box(subdivtype,
		subdivlevel[0], cmptexuvswi, cmptexuvwswi));
	break;

    case DcCone:
	return(dor_prisrf_create_alternate_geometry_cone(subdivtype,
		subdivlevel[0], cmptexuvswi, cmptexuvwswi));
	break;

    default:
	sprintf(DDerror_GetLine(),"Surface type %d",prisrf->surface_type);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "dor_prisrf_create_alternate_geometry",
		    DDerror_GetLine());
	break;
    }

    return(DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_prisrf_create_alternate_geometry_box (
    DtInt subdivtype,
    DtReal subdivlevel,
    DtSwitch cmptexuvswi,
    DtSwitch cmptexuvwswi)
{
    DtInt vertex, vertexsize;
    DtVertexType vertextype;
    DtReal *trilst, *vertices;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    DtPFI uv_function;
    DtPFI uvw_function;
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;
    dot_object *trilstobj;

    static DtReal trilstvtxs[36][3] = {
   	{ 0.0, 0.0, 0.0 },  { 0.0, 1.0, 0.0 },  { 1.0, 1.0, 0.0 },
   	{ 0.0, 0.0, 0.0 },  { 1.0, 1.0, 0.0 },  { 1.0, 0.0, 0.0 },
   	{ 0.0, 0.0, 1.0 },  { 1.0, 0.0, 1.0 },  { 1.0, 1.0, 1.0 },
   	{ 0.0, 0.0, 1.0 },  { 1.0, 1.0, 1.0 },  { 0.0, 1.0, 1.0 },
   	{ 0.0, 1.0, 0.0 },  { 0.0, 1.0, 1.0 },  { 1.0, 1.0, 1.0 },
   	{ 0.0, 1.0, 0.0 },  { 1.0, 1.0, 1.0 },  { 1.0, 1.0, 0.0 },
   	{ 0.0, 0.0, 0.0 },  { 1.0, 0.0, 0.0 },  { 1.0, 0.0, 1.0 },
   	{ 0.0, 0.0, 0.0 },  { 1.0, 0.0, 1.0 },  { 0.0, 0.0, 1.0 },
   	{ 0.0, 0.0, 0.0 },  { 0.0, 0.0, 1.0 },  { 0.0, 1.0, 1.0 },
   	{ 0.0, 0.0, 0.0 },  { 0.0, 1.0, 1.0 },  { 0.0, 1.0, 0.0 },
   	{ 1.0, 0.0, 0.0 },  { 1.0, 1.0, 0.0 },  { 1.0, 1.0, 1.0 },
   	{ 1.0, 0.0, 0.0 },  { 1.0, 1.0, 1.0 },  { 1.0, 0.0, 1.0 }
    };

    static DtReal uv[36][2] = {
	/* comments assume z up,  and y going in*/
	/*bottom*/
	{ 0.0, 1.0 },   { 0.0, 0.0 },   { 1.0, 0.0 },
	{ 0.0, 1.0 },   { 1.0, 0.0 },   { 1.0, 1.0 },

	/*top*/
        { 0.0, 1.0 },   { 1.0, 1.0 },   { 1.0, 0.0 },
        { 0.0, 1.0 },   { 1.0, 0.0 },   { 0.0, 0.0 },

        /*back*/
        { 1.0, 1.0 },   { 1.0, 0.0 },   { 0.0, 0.0 },
        { 1.0, 1.0 },   { 0.0, 0.0 },   { 0.0, 1.0 },

        /*front*/
        { 0.0, 1.0 },   { 1.0, 1.0 },   { 1.0, 0.0 },
        { 0.0, 1.0 },   { 1.0, 0.0 },   { 0.0, 0.0 },

        /*left*/
        { 1.0, 1.0 },   { 1.0, 0.0 },   { 0.0, 0.0 },
        { 1.0, 1.0 },   { 0.0, 0.0 },   { 0.0, 1.0 },

        /*right*/
        { 0.0, 1.0 },   { 1.0, 1.0 },   { 1.0, 0.0 },
        { 0.0, 1.0 },   { 1.0, 0.0 },   { 0.0, 0.0 }
    };

    /* use trilstvtxs values for uvw */
    static DtReal uvw[36][3] = {
   	{ 0.0, 1.0, 1.0 },  { 0.0, 0.0, 1.0 },  { 1.0, 0.0, 1.0 },
   	{ 0.0, 1.0, 1.0 },  { 1.0, 0.0, 1.0 },  { 1.0, 1.0, 1.0 },
   	{ 0.0, 1.0, 0.0 },  { 1.0, 1.0, 0.0 },  { 1.0, 0.0, 0.0 },
   	{ 0.0, 1.0, 0.0 },  { 1.0, 0.0, 0.0 },  { 0.0, 0.0, 0.0 },
   	{ 0.0, 0.0, 1.0 },  { 0.0, 0.0, 0.0 },  { 1.0, 0.0, 0.0 },
   	{ 0.0, 0.0, 1.0 },  { 1.0, 0.0, 0.0 },  { 1.0, 0.0, 1.0 },
   	{ 0.0, 1.0, 1.0 },  { 1.0, 1.0, 1.0 },  { 1.0, 1.0, 0.0 },
   	{ 0.0, 1.0, 1.0 },  { 1.0, 1.0, 0.0 },  { 0.0, 1.0, 0.0 },
   	{ 0.0, 1.0, 1.0 },  { 0.0, 1.0, 0.0 },  { 0.0, 0.0, 0.0 },
   	{ 0.0, 1.0, 1.0 },  { 0.0, 0.0, 0.0 },  { 0.0, 0.0, 1.0 },
   	{ 1.0, 1.0, 1.0 },  { 1.0, 0.0, 1.0 },  { 1.0, 0.0, 0.0 },
   	{ 1.0, 1.0, 1.0 },  { 1.0, 0.0, 0.0 },  { 1.0, 1.0, 0.0 }
    };


    static DtReal vtxnrms[36][3] = {
	/* we need to supply these to user-defined DoCompTextureUVCallback */
	/* bottom */
	{ 0.0,  0.0, -1.0 },  { 0.0,  0.0, -1.0 },  { 0.0,  0.0, -1.0 },
	{ 0.0,  0.0, -1.0 },  { 0.0,  0.0, -1.0 },  { 0.0,  0.0, -1.0 },

	/* top */
	{ 0.0,  0.0,  1.0 },  { 0.0,  0.0,  1.0 },  { 0.0,  0.0,  1.0 },
	{ 0.0,  0.0,  1.0 },  { 0.0,  0.0,  1.0 },  { 0.0,  0.0,  1.0 },

	/* back */
	{ 0.0,  1.0,  0.0 },  { 0.0,  1.0,  0.0 },  { 0.0,  1.0,  0.0 },
	{ 0.0,  1.0,  0.0 },  { 0.0,  1.0,  0.0 },  { 0.0,  1.0,  0.0 },

	/* front */
	{ 0.0, -1.0,  0.0 },  { 0.0, -1.0,  0.0 },  { 0.0, -1.0,  0.0 },
	{ 0.0, -1.0,  0.0 },  { 0.0, -1.0,  0.0 },  { 0.0, -1.0,  0.0 },

	/* left */
	{-1.0,  0.0,  0.0 },  {-1.0,  0.0,  0.0 },  {-1.0,  0.0,  0.0 },
	{-1.0,  0.0,  0.0 },  {-1.0,  0.0,  0.0 },  {-1.0,  0.0,  0.0 },

	/* right */
	{ 1.0,  0.0,  0.0 },  { 1.0,  0.0,  0.0 },  { 1.0,  0.0,  0.0 },
	{ 1.0,  0.0,  0.0 },  { 1.0,  0.0,  0.0 },  { 1.0,  0.0,  0.0 }
    };


    if (!cmptexuvswi  && !cmptexuvwswi)  {
   	trilstobj = dor_trilst_create (DcRGB, DcLoc, 12, (DtReal*)trilstvtxs);
    } else {

        /*  Are texture uvs and/or uvws to be computed?  If so, determine whether
        to use the "callback" provided by Dore or a user-defined callback. */

        if (cmptexuvswi) {
    	    dor_global_cmptexuvcbk_get_value(&cmptexuvcbk);
	    if (cmptexuvcbk != DcStdUVCallback) {
                uv_function = dor_calbak_get_function(cmptexuvcbk);
                uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);
	        /* call user-defined callback for every vertex in box */
	        for (vertex=0; vertex<36; vertex++) {
	    	    dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
			    uv_dataobject, trilstvtxs[vertex][0],
			    trilstvtxs[vertex][1], trilstvtxs[vertex][2],
			    vtxnrms[vertex][0], vtxnrms[vertex][1],
			    vtxnrms[vertex][2],
			    &(uv[vertex][0]), &(uv[vertex][1]));
	        }
	    }
        }
        if (cmptexuvwswi) {
    	    dor_global_cmptexuvwcbk_get_value(&cmptexuvwcbk);
	    if (cmptexuvwcbk != DcStdUVWCallback) {
                uvw_function = dor_calbak_get_function(cmptexuvwcbk);
                uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);
	        /* call user-defined callback for every vertex in box */
	        for (vertex=0; vertex<36; vertex++) {
	    	    dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
			    uvw_dataobject, trilstvtxs[vertex][0],
			    trilstvtxs[vertex][1], trilstvtxs[vertex][2],
			    vtxnrms[vertex][0], vtxnrms[vertex][1],
			    vtxnrms[vertex][2],
			    &(uvw[vertex][0]), &(uvw[vertex][1]), &(uvw[vertex][2]));
	        }
	    }
        }

	/* figure out vertex type based on whether uv/uvw are used */
	vertextype = DcLoc;
	vertexsize = 3;
	if (cmptexuvswi) {
		vertextype |= dor_system_texture_uv_count(1);
		vertexsize += 2;
	}
	if (cmptexuvwswi) {
		vertextype |= dor_system_texture_uvw_count(1);
		vertexsize += 3;
	}

        /* store locations, uv's and/or uvw's in same array */
	trilst = (DtReal *) dor_space_allocate(36*vertexsize*(sizeof(DtReal)));

	vertices = trilst;

        for (vertex=0; vertex<36; vertex++) {
    	    *trilst++ = trilstvtxs[vertex][0];
    	    *trilst++ = trilstvtxs[vertex][1];
    	    *trilst++ = trilstvtxs[vertex][2];
	    if (cmptexuvswi) {
	        *trilst++ = uv[vertex][0];
	        *trilst++ = uv[vertex][1];
	    }
	    if (cmptexuvwswi) {
	        *trilst++ = uvw[vertex][0];
	        *trilst++ = uvw[vertex][1];
	        *trilst++ = uvw[vertex][2];
	    }
        }
   	trilstobj = dor_trilst_create(DcRGB, vertextype, 12, vertices);
    }

    return(trilstobj);
}

/*
 ======================================================================
 */

/*  A cone is subdivided into n triangles and a another n
    triangle for the bottom circular plate.

    The following parameterization is used for texture mapping:
	If you look down on the cone so the point is in the center
	then you will see a circular cutout of the texture.
	There is no distortion.

	     u = (x+1.)/2.
	     v = (1.-y)/2.

	For the bottom circular plate, the bottom edge values of the
	conic part are extended to the center of the plate.

       For 3D texture mapping, the cone is transformed to fit in the
       space [0,1] in x, y, and z. The uvw are computed from these transformed
       coordinates xyz, as: u=x,  v=1-y, and w=1-z.

*/


dot_object *dor_prisrf_create_alternate_geometry_cone (
    DtInt subdivtype,
    DtReal subdivlevel,
    DtSwitch cmptexuvswi,
    DtSwitch cmptexuvwswi)
{
    static DtInt space = 0;
    static DtReal *vertices;
    DtReal *vptr;
    DtInt needspace;
    DtInt vertexsize;
    DtInt divisions;
    DtReal angleincrement;
    DtReal radius;
    DtInt tricount;
    DtInt vertex;
    DtReal x,y,z;
    DtReal nx,ny,nz;
    DtReal tmp[6];
    DtVertexType vertextype;
    dot_object *trilstobj;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    DtPFI uv_function;
    DtPFI uvw_function;
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;

    if (subdivtype != DcSubDivSegments)
	/* old style subdivision spec */
   	divisions = 5 + (subdivlevel*20.);
    else
	divisions = subdivlevel;

    vertexsize = 6;
    vertextype = DcLocNrm;
    /*  Are texture uvs and/or uvws to be computed?  If so, determine whether
        to use the "callback" provided by Dore or a user-defined callback. */
    if (cmptexuvswi) {
	vertexsize += 2;
        vertextype |= dor_system_texture_uv_count(1);
	dor_global_cmptexuvcbk_get_value(&cmptexuvcbk);
        if (cmptexuvcbk != DcStdUVCallback) {
            uv_function = dor_calbak_get_function(cmptexuvcbk);
            uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);
	}
    }
    if (cmptexuvwswi) {
	vertexsize += 3 ;
        vertextype |= dor_system_texture_uvw_count(1);
	dor_global_cmptexuvwcbk_get_value(&cmptexuvwcbk);
        if (cmptexuvwcbk != DcStdUVWCallback) {
            uvw_function = dor_calbak_get_function(cmptexuvwcbk);
            uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);
	}
    }

    needspace = 6*divisions*vertexsize;
    if (needspace > space) {
        if (space != 0)
        	vertices = (DtReal *)dor_space_reallocate(
				vertices, needspace*(sizeof (DtReal)));
	else
        	vertices = (DtReal *)dor_space_allocate(
				needspace*(sizeof (DtReal)));
        space = needspace;
    }

    angleincrement = TWO_PI/divisions;
    radius = 1.;

    vptr = vertices;
    tricount = 0;

    /*  Make the back circular plate, notice the backwards angular
        direction, used to get the normal correct.  */

    for (vertex=0;vertex<divisions;vertex++) {
	tmp[0] = radius * dor_math_cos(-vertex*angleincrement);
   	tmp[1] = radius * dor_math_sin(-vertex*angleincrement);
   	x = *vptr++ = tmp[0];
   	y = *vptr++ = tmp[1];
   	z = *vptr++ = 0.;
   	nx = *vptr++ = 0.;
   	ny = *vptr++ = 0.;
   	nz = *vptr++ = -1.;
	if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = (tmp[0] + 1.)/2.;
   		*vptr++ = (1. - tmp[1])/2.;
	    } else {
		dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
		vptr += 2;
	    }
	}
        if (cmptexuvwswi) {
            if (cmptexuvwcbk == DcStdUVWCallback) {
	        *vptr++ = x/2.+0.5;
	        *vptr++ = .5 - y/2.;
	        *vptr++ = 1.0;
	    } else {
		dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
		vptr += 3;
	    }
	}

	if (vertex == (divisions-1)) {
        /* avoid a crack in the model; make sure last vertex = first vertex */
	    tmp[0] = radius;
	    tmp[1] = 0.0;
	} else {
   	    tmp[0] = radius * dor_math_cos(-(vertex+1)*angleincrement);
   	    tmp[1] = radius * dor_math_sin(-(vertex+1)*angleincrement);
	}
   	x = *vptr++ = tmp[0];
   	y = *vptr++ = tmp[1];
   	z = *vptr++ = 0.;
   	nx = *vptr++ = 0.;
   	ny = *vptr++ = 0.;
   	nz = *vptr++ = -1.;
	if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = (tmp[0] + 1.)/2.;
   		*vptr++ = (1. - tmp[1])/2.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
	    }
	}
        if (cmptexuvwswi) {
            if (cmptexuvwcbk == DcStdUVWCallback) {
	        *vptr++ = x/2.+0.5;
	        *vptr++ = .5 - y/2.;
	        *vptr++ = 1.0;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
	    }
	}

   	tmp[0] = radius * dor_math_cos(-(vertex+.5)*angleincrement);
   	tmp[1] = radius * dor_math_sin(-(vertex+.5)*angleincrement);
   	x = *vptr++ = 0.;
   	y = *vptr++ = 0.;
   	z = *vptr++ = 0.;
   	nx = *vptr++ = 0.;
   	ny = *vptr++ = 0.;
   	nz = *vptr++ = -1.;
	if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = (tmp[0] + 1.)/2.;
   		*vptr++ = (1. - tmp[1])/2.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
	    }
	}
	/* middle of the "bottom" of the texture cube */
        if (cmptexuvwswi) {
            if (cmptexuvwcbk == DcStdUVWCallback) {
	    	*vptr++ = 0.5;
	    	*vptr++ = 0.5;
	    	*vptr++ = 1.0;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
	    }
	}
   	tricount++;
    }

    /*  Add the side polygons.  */

    for (vertex=0;vertex<divisions;vertex++) {
	tmp[0] = radius * dor_math_cos(vertex*angleincrement);
	tmp[1] = radius * dor_math_sin(vertex*angleincrement);
   	dor_math_normal(0., 0., 0., tmp[0], tmp[1], 1.,
			&tmp[3], &tmp[4], &tmp[5]);
	*vptr++ = tmp[0];
	*vptr++ = tmp[1];
   	*vptr++ = tmp[2] = 0.;
	*vptr++ = tmp[3];
	*vptr++ = tmp[4];
	*vptr++ = tmp[5];
	if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
		*vptr++ = (tmp[0] + 1.)/2.;
                *vptr++ = (1. - tmp[1])/2.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, tmp[0],tmp[1],tmp[2],
			tmp[3],tmp[4],tmp[5], vptr, (vptr+1));
                vptr += 2;
	    }
	}
        if (cmptexuvwswi) {
            if (cmptexuvwcbk == DcStdUVWCallback) {
	    	*vptr++ = tmp[0]/2.+0.5;
	    	*vptr++ = .5 - tmp[1]/2.;
	    	*vptr++ = 1.0;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, tmp[0],tmp[1],tmp[2],
			tmp[3],tmp[4],tmp[5], vptr,(vptr+1),(vptr+2));
                vptr += 3;
	    }
	}

	if (vertex == (divisions-1)) {
	    /* avoid a crack in the model; make sure last vertex = first vertex */
	    tmp[0] = radius;
	    tmp[1] = 0.0;
	} else {
   	    tmp[0] = radius * dor_math_cos((vertex+1)*angleincrement);
   	    tmp[1] = radius * dor_math_sin((vertex+1)*angleincrement);
	}
   	dor_math_normal(0., 0., 0., tmp[0], tmp[1], 1.,
			&tmp[3], &tmp[4], &tmp[5]);
	*vptr++ = tmp[0];
	*vptr++ = tmp[1];
   	*vptr++ = tmp[2] = 0.;
	*vptr++ = tmp[3];
	*vptr++ = tmp[4];
	*vptr++ = tmp[5];
	if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
		*vptr++ = (tmp[0] + 1.)/2.;
                *vptr++ = (1. - tmp[1])/2.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, tmp[0],tmp[1],tmp[2],
			tmp[3],tmp[4],tmp[5], vptr, (vptr+1));
                vptr += 2;
	    }
	}
        if (cmptexuvwswi) {
            if (cmptexuvwcbk == DcStdUVWCallback) {
	    	*vptr++ = tmp[0]/2.+0.5;
	    	*vptr++ = .5 - tmp[1]/2.;
	    	*vptr++ = 1.0;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, tmp[0],tmp[1],tmp[2],
			tmp[3],tmp[4],tmp[5], vptr,(vptr+1),(vptr+2));
                vptr += 3;
	    }
	}

   	*vptr++ = x = 0.;
   	*vptr++ = y = 0.;
   	*vptr++ = z = 1.;
   	tmp[0] = radius * dor_math_cos((vertex+0.5)*angleincrement);
   	tmp[1] = radius * dor_math_sin((vertex+0.5)*angleincrement);
   	dor_math_normal(0., 0., 0., tmp[0], tmp[1], 1.,
			&tmp[3], &tmp[4], &tmp[5]);
	*vptr++ = tmp[3];
	*vptr++ = tmp[4];
	*vptr++ = tmp[5];
	if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = .5;
   		*vptr++ = .5;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x, y, z,
			tmp[3],tmp[4],tmp[5], vptr, (vptr+1));
                vptr += 2;
	    }
	}
	/* apex in middle of the "top" of the texture cube */
        if (cmptexuvwswi) {
            if (cmptexuvwcbk == DcStdUVWCallback) {
	    	*vptr++ = 0.5;
	    	*vptr++ = 0.5;
	    	*vptr++ = 0.0;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x, y, z,
			tmp[3],tmp[4],tmp[5], vptr,(vptr+1),(vptr+2));
                vptr += 3;
	    }
	}
   	tricount++;
    }


    trilstobj = dor_trilst_create(DcRGB,vertextype,tricount,vertices);

    return(trilstobj);
}
/*
 ======================================================================
 */

   /* A cylinder is subdivided into n strips (of 2 triangles each)
	and a top and bottom circular plate (each made of n triangles).

      Cylindrical coordinates are used for parameterizing the
	cylinder for texture mapping :

      	For the cylindrical part:
		u = n * 1./divsions, n = 0,..,divsions
		v = -z  (note the z coordinate for the cylinder
			is always between 0 and 1)

      	For the top circular plate:
		The top edge values are extended to the center point

      	For the bottom circular plate:
		The bottom edge values are extended to the center point

       For 3D texture mapping, the cylinder is transformed to fit in the
       space [0,1] in x, y, and z. The uvw are computed from these transformed
       coordinates xyz, as: u=x,  v=1-y, and w=1-z.

   */


dot_object *dor_prisrf_create_alternate_geometry_cylinder (
    DtInt subdivtype,
    DtReal subdivlevel,
    DtSwitch cmptexuvswi,
    DtSwitch cmptexuvwswi)
{
    static DtInt space=0;
    static DtReal *vertices;
    DtReal *vptr;
    DtInt needspace;
    DtInt vertexsize;
    DtInt divisions;
    DtReal angleincrement;
    DtReal uincrement;
    DtReal radius;
    DtInt tricount;
    DtInt vertex;
    DtReal x,y,z;
    DtReal nx,ny,nz;
    DtReal tempvtxs[4][11];
    DtReal lowx, lowy;
    DtReal highx, highy;
    DtReal lowu, highu;
    DtVertexType vertextype;
    dot_object *trilstobj;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    DtPFI uv_function;
    DtPFI uvw_function;
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;

    if (subdivtype != DcSubDivSegments)
	/* old style subdiv spec */
   	divisions = 5 + (subdivlevel*20.);
    else
	divisions = subdivlevel;

    vertexsize = 6;
    vertextype = DcLocNrm;
    if (cmptexuvswi) {
	vertexsize +=2;
        vertextype |= dor_system_texture_uv_count(1);
	dor_global_cmptexuvcbk_get_value(&cmptexuvcbk);
        if (cmptexuvcbk != DcStdUVCallback) {
            uv_function = dor_calbak_get_function(cmptexuvcbk);
            uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);
        }
    }
    if (cmptexuvwswi) {
        vertexsize +=3;
        vertextype |= dor_system_texture_uvw_count(1);
	dor_global_cmptexuvwcbk_get_value(&cmptexuvwcbk);
        if (cmptexuvwcbk != DcStdUVWCallback) {
            uvw_function = dor_calbak_get_function(cmptexuvwcbk);
            uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);
        }
    }

    needspace = 12*divisions*vertexsize;
    if (needspace > space) {
	if (space != 0)
		vertices = (DtReal *)dor_space_reallocate(
				vertices, needspace*(sizeof (DtReal)));
	else
		vertices = (DtReal *)dor_space_allocate(
				needspace*(sizeof (DtReal)));
	space = needspace;
    }

    /*  Make the back circular plate, notice the backwards angular
        direction, used to get the normal correct.  */

    angleincrement = 2.*M_PI/divisions;
    uincrement = 1./divisions;
    radius = 1.;

    vptr = vertices;
    tricount = 0;

    for (vertex=0;vertex<divisions;vertex++) {
   	*vptr++ = x = radius * dor_math_cos(-vertex*angleincrement);
   	*vptr++ = y = radius * dor_math_sin(-vertex*angleincrement);
   	*vptr++ = z = 0.;
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = -1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = 1. - (vertex*uincrement);
   		*vptr++ = 1.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
	if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = x/2.+.5;
            	*vptr++ = .5 - y/2.;
            	*vptr++ = 1.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
            }
        }

	if (vertex == (divisions-1)) {
	/* avoid a crack in the model; make sure last vertex = first vertex */
	    *vptr++ = x = radius;
	    *vptr++ = y = 0.0;
	} else {
   	    *vptr++ = x = radius * dor_math_cos(-(vertex+1)*angleincrement);
   	    *vptr++ = y = radius * dor_math_sin(-(vertex+1)*angleincrement);
	}
   	*vptr++ = z = 0.;
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = -1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = 1. - ((vertex+1)*uincrement);
   		*vptr++ = 1.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
	if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = x/2.+0.5;
            	*vptr++ = .5 - y/2.;
            	*vptr++ = 1.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
            }
        }

   	*vptr++ = x = 0.;
   	*vptr++ = y = 0.;
   	*vptr++ = z = 0.;
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = -1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = 1. - ((vertex+.5)*uincrement);
   		*vptr++ = 1.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
	if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = 0.5;
            	*vptr++ = 0.5;
            	*vptr++ = 1.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
            }
        }

	tricount++;
    }

    /*  Add the front circular plate to the polygonal object definition.
        Notice the frontwards angular direction, used to get the normal
        correct.  */

    for (vertex=0;vertex<divisions;vertex++) {
   	*vptr++ = x = radius * dor_math_cos(vertex*angleincrement);
   	*vptr++ = y = radius * dor_math_sin(vertex*angleincrement);
   	*vptr++ = z = 1.;
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = 1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = vertex * uincrement;
   		*vptr++ = 0.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
	if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = x/2.+0.5;
            	*vptr++ = .5 - y/2.;
            	*vptr++ = 0.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
            }
        }

	if (vertex == (divisions-1)) {
	/* avoid a crack in the model; make sure last vertex = first vertex */
	    *vptr++ = x = radius;
	    *vptr++ = y = 0.0;
	} else {
   	    *vptr++ = x = radius * dor_math_cos((vertex+1)*angleincrement);
   	    *vptr++ = y = radius * dor_math_sin((vertex+1)*angleincrement);
	}
   	*vptr++ = z = 1.;
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = 1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = (vertex+1) * uincrement;
   		*vptr++ = 0.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
	if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = x/2.+0.5;
            	*vptr++ = .5 - y/2.;
            	*vptr++ = 0.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
            }
        }

   	*vptr++ = x = 0.;
   	*vptr++ = y = 0.;
   	*vptr++ = z = 1.;
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = 1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = (vertex + .5) * uincrement;
   		*vptr++ = 0.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
	if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            *vptr++ = 0.5;
            *vptr++ = 0.5;
            *vptr++ = 0.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
            }
        }
	tricount++;
    }

    /*  Add the side polygons, keeping the normals straight.  */

    for (vertex=0;vertex<divisions;vertex++) {

   	lowx = radius * dor_math_cos(vertex*angleincrement);
   	lowy = radius * dor_math_sin(vertex*angleincrement);
   	lowu = vertex*uincrement;

	if (vertex == (divisions-1)) {
	/* avoid a crack in the model; make sure last vertex = first vertex */
   	    highx = radius;
   	    highy = 0.0;
   	    highu = 1.0;
	} else {
   	    highx = radius * dor_math_cos((vertex+1)*angleincrement);
   	    highy = radius * dor_math_sin((vertex+1)*angleincrement);
   	    highu = (vertex+1)*uincrement;
	}

   	tempvtxs[0][0] = highx;
   	tempvtxs[0][1] = highy;
   	tempvtxs[0][2] = 0.;
   	dor_math_normal(0.,0.,0.,tempvtxs[0][0],tempvtxs[0][1],tempvtxs[0][2],
   		&(tempvtxs[0][3]),&(tempvtxs[0][4]),&(tempvtxs[0][5]));
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		tempvtxs[0][6] = highu;
   		tempvtxs[0][7] = 1.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, tempvtxs[0][0], tempvtxs[0][1],
			tempvtxs[0][2], tempvtxs[0][3], tempvtxs[0][4],
			tempvtxs[0][5], &(tempvtxs[0][6]), &(tempvtxs[0][7]));
            }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	tempvtxs[0][8] = highx/2. + 0.5;
            	tempvtxs[0][9] = .5 - highy/2.;
            	tempvtxs[0][10] = 1.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, tempvtxs[0][0], tempvtxs[0][1],
			tempvtxs[0][2], tempvtxs[0][3], tempvtxs[0][4],
			tempvtxs[0][5], &(tempvtxs[0][8]), &(tempvtxs[0][9]),
			&(tempvtxs[0][10]));
            }
        }

   	tempvtxs[1][0] = highx;
   	tempvtxs[1][1] = highy;
   	tempvtxs[1][2] = 1.;
   	dor_math_normal(0.,0.,1.,tempvtxs[1][0],tempvtxs[1][1],tempvtxs[1][2],
   		&(tempvtxs[1][3]),&(tempvtxs[1][4]),&(tempvtxs[1][5]));
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		tempvtxs[1][6] = highu;
   		tempvtxs[1][7] = 0.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, tempvtxs[1][0], tempvtxs[1][1],
			tempvtxs[1][2], tempvtxs[1][3], tempvtxs[1][4],
			tempvtxs[1][5], &(tempvtxs[1][6]), &(tempvtxs[1][7]));
	    }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	tempvtxs[1][8] = highx/2. + 0.5;
            	tempvtxs[1][9] = .5 - highy/2.;
            	tempvtxs[1][10] = 0.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, tempvtxs[1][0], tempvtxs[1][1],
			tempvtxs[1][2], tempvtxs[1][3], tempvtxs[1][4],
			tempvtxs[1][5], &(tempvtxs[1][8]), &(tempvtxs[1][9]),
                        &(tempvtxs[1][10]));
	    }
        }

   	tempvtxs[2][0] = lowx;
   	tempvtxs[2][1] = lowy;
   	tempvtxs[2][2] = 1.;
   	dor_math_normal(0.,0.,1.,tempvtxs[2][0],tempvtxs[2][1],tempvtxs[2][2],
   		&(tempvtxs[2][3]),&(tempvtxs[2][4]),&(tempvtxs[2][5]));
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		tempvtxs[2][6] = lowu;
   		tempvtxs[2][7] = 0.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, tempvtxs[2][0], tempvtxs[2][1],
			tempvtxs[2][2], tempvtxs[2][3], tempvtxs[2][4],
			tempvtxs[2][5], &(tempvtxs[2][6]), &(tempvtxs[2][7]));
	    }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	tempvtxs[2][8] = lowx/2. + 0.5;
            	tempvtxs[2][9] = .5 - lowy/2.;
            	tempvtxs[2][10] = 0.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, tempvtxs[2][0], tempvtxs[2][1],
			tempvtxs[2][2], tempvtxs[2][3], tempvtxs[2][4],
			tempvtxs[2][5], &(tempvtxs[2][8]), &(tempvtxs[2][9]),
			&(tempvtxs[2][10]));
	    }
        }

   	tempvtxs[3][0] = lowx;
   	tempvtxs[3][1] = lowy;
   	tempvtxs[3][2] = 0.;
   	dor_math_normal(0.,0.,0.,tempvtxs[3][0],tempvtxs[3][1],tempvtxs[3][2],
   		&(tempvtxs[3][3]),&(tempvtxs[3][4]),&(tempvtxs[3][5]));
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		tempvtxs[3][6] = lowu;
   		tempvtxs[3][7] = 1.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, tempvtxs[3][0], tempvtxs[3][1],
			tempvtxs[3][2], tempvtxs[3][3], tempvtxs[3][4],
			tempvtxs[3][5], &(tempvtxs[3][6]), &(tempvtxs[3][7]));
	    }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	tempvtxs[3][8] = lowx/2. + 0.5;
            	tempvtxs[3][9] = .5 - lowy/2.;
            	tempvtxs[3][10] = 1.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, tempvtxs[3][0], tempvtxs[3][1],
			tempvtxs[3][2], tempvtxs[3][3], tempvtxs[3][4],
			tempvtxs[3][5], &(tempvtxs[3][8]), &(tempvtxs[3][9]),
			&(tempvtxs[3][10]));
	    }
        }

   	*vptr++ = tempvtxs[0][0];
   	*vptr++ = tempvtxs[0][1];
   	*vptr++ = tempvtxs[0][2];
   	*vptr++ = tempvtxs[0][3];
   	*vptr++ = tempvtxs[0][4];
   	*vptr++ = tempvtxs[0][5];
	if (cmptexuvswi) {
   		*vptr++ = tempvtxs[0][6];
   		*vptr++ = tempvtxs[0][7];
	}
	if (cmptexuvwswi) {
   		*vptr++ = tempvtxs[0][8];
   		*vptr++ = tempvtxs[0][9];
   		*vptr++ = tempvtxs[0][10];
	}

   	*vptr++ = tempvtxs[1][0];
   	*vptr++ = tempvtxs[1][1];
   	*vptr++ = tempvtxs[1][2];
   	*vptr++ = tempvtxs[1][3];
   	*vptr++ = tempvtxs[1][4];
   	*vptr++ = tempvtxs[1][5];
	if (cmptexuvswi) {
   		*vptr++ = tempvtxs[1][6];
   		*vptr++ = tempvtxs[1][7];
	}
	if (cmptexuvwswi) {
   		*vptr++ = tempvtxs[1][8];
   		*vptr++ = tempvtxs[1][9];
   		*vptr++ = tempvtxs[1][10];
	}

   	*vptr++ = tempvtxs[2][0];
   	*vptr++ = tempvtxs[2][1];
   	*vptr++ = tempvtxs[2][2];
   	*vptr++ = tempvtxs[2][3];
   	*vptr++ = tempvtxs[2][4];
   	*vptr++ = tempvtxs[2][5];
	if (cmptexuvswi) {
   		*vptr++ = tempvtxs[2][6];
   		*vptr++ = tempvtxs[2][7];
	}
	if (cmptexuvwswi) {
   		*vptr++ = tempvtxs[2][8];
   		*vptr++ = tempvtxs[2][9];
   		*vptr++ = tempvtxs[2][10];
	}

   	*vptr++ = tempvtxs[0][0];
   	*vptr++ = tempvtxs[0][1];
   	*vptr++ = tempvtxs[0][2];
   	*vptr++ = tempvtxs[0][3];
   	*vptr++ = tempvtxs[0][4];
   	*vptr++ = tempvtxs[0][5];
	if (cmptexuvswi) {
   		*vptr++ = tempvtxs[0][6];
   		*vptr++ = tempvtxs[0][7];
	}
	if (cmptexuvwswi) {
   		*vptr++ = tempvtxs[0][8];
   		*vptr++ = tempvtxs[0][9];
   		*vptr++ = tempvtxs[0][10];
	}

   	*vptr++ = tempvtxs[2][0];
   	*vptr++ = tempvtxs[2][1];
   	*vptr++ = tempvtxs[2][2];
   	*vptr++ = tempvtxs[2][3];
   	*vptr++ = tempvtxs[2][4];
   	*vptr++ = tempvtxs[2][5];
	if (cmptexuvswi) {
   		*vptr++ = tempvtxs[2][6];
   		*vptr++ = tempvtxs[2][7];
	}
	if (cmptexuvwswi) {
   		*vptr++ = tempvtxs[2][8];
   		*vptr++ = tempvtxs[2][9];
   		*vptr++ = tempvtxs[2][10];
	}

   	*vptr++ = tempvtxs[3][0];
   	*vptr++ = tempvtxs[3][1];
   	*vptr++ = tempvtxs[3][2];
   	*vptr++ = tempvtxs[3][3];
   	*vptr++ = tempvtxs[3][4];
   	*vptr++ = tempvtxs[3][5];
	if (cmptexuvswi) {
   		*vptr++ = tempvtxs[3][6];
   		*vptr++ = tempvtxs[3][7];
	}
	if (cmptexuvwswi) {
   		*vptr++ = tempvtxs[3][8];
   		*vptr++ = tempvtxs[3][9];
   		*vptr++ = tempvtxs[3][10];
	}

	tricount += 2;
    }

    trilstobj = dor_trilst_create(DcRGB, vertextype, tricount,vertices);

    return(trilstobj);

}

/*
 ======================================================================
 */

/*
	One octant of a geodesic sphere is calculated and
	then reflected to get the other 7 octants

	Spherical coordinates are used for parameterizing the
	sphere for texture mapping:
	(note that x,y,z are always between 0 and 1 for a standard
	Dore sphere)

	    atan2(y,x)
	u = ---------- + .5	(atan2 returns a value in the
	       2*PI		  range (-PI,PI] and we want a
				  value between 0 and 1)


	     arccos(z)
	v =  ---------
	      	PI

       For 3D texture mapping, the sphere is transformed to fit in the
       space [0,1] in x, y, and z. The uvw are computed from these transformed
       coordinates xyz, as: u=x,  v=1-y, and w=1-z.
 */

static DtIntTriple *tris;

dot_object *dor_prisrf_create_alternate_geometry_sphere (
    DtInt subdivtype,
    DtReal subdivlevel,
    DtSwitch cmptexuvswi,
    DtSwitch cmptexuvwswi)
{
    static DtInt space = 0;
    static DtReal *sphvertices;
    static DtNPoint3 **grid; /* grid of vertices for one octant */

    DtReal *vptr;
    DtInt row;
    DtInt octsize;
    DtInt octcnt;
    DtInt vtx;
    DtInt vertexsize;
    DtReal x,y,z;
    DtReal nx,ny,nz;
    DtReal u;
    DtReal saveu[16];
    DtInt gridsize;
    DtInt needspace;
    DtUInt i, j;
    DtInt divs;  /* should be a DtUInt but for a sun compiler bug */
    DtReal incr;
    DtVertexType vertextype;
    static DtInt finest=0;
    static DtNPoint3 norm0, norm1;
    dot_object *trimshobj;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    DtPFI uv_function;
    DtPFI uvw_function;
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;

    if (subdivtype != DcSubDivSegments)
	/* old style subdivsion spec */
	divs = 2 + (subdivlevel*10.);
    else
	divs = subdivlevel/4.;

    row = divs+1;
    gridsize = row*row;

    vertexsize = 6;
    vertextype = DcLocNrm;
    if (cmptexuvswi) {
	vertexsize += 2;
        vertextype |= dor_system_texture_uv_count(1);
	dor_global_cmptexuvcbk_get_value(&cmptexuvcbk);
        if (cmptexuvcbk != DcStdUVCallback) {
            uv_function = dor_calbak_get_function(cmptexuvcbk);
            uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);
        }
    }
    if (cmptexuvwswi) {
        vertexsize += 3;
        vertextype |= dor_system_texture_uvw_count(1);
	dor_global_cmptexuvwcbk_get_value(&cmptexuvwcbk);
        if (cmptexuvwcbk != DcStdUVWCallback) {
            uvw_function = dor_calbak_get_function(cmptexuvwcbk);
            uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);
        }
    }

    needspace = gridsize*8*vertexsize;
    if (needspace > space) {
	if (space != 0) {
        	tris = (DtIntTriple *)dor_space_reallocate(
				tris, 8*divs*divs*(sizeof (DtIntTriple)));
        	sphvertices = (DtReal *)dor_space_reallocate(
				sphvertices, needspace*(sizeof (DtReal)));
	} else {
        	tris = (DtIntTriple *)dor_space_allocate(
				8*divs*divs*(sizeof (DtIntTriple)));
        	sphvertices = (DtReal *)dor_space_allocate(
				needspace*(sizeof (DtReal)));
	}
        space = needspace;
    }
    /* initialize array to zero (since not all array positions are filled) */
    for (i=0; i<space; i++)
	sphvertices[i]  = 0.0;

    if(divs > finest) /* need more space */ {
	if(finest != 0) {
		for(i=0; i<finest+1; i++) {
			dor_space_deallocate(grid[i]);
		}
		dor_space_deallocate(grid);
	}
	finest = divs;
	grid = (DtNPoint3 **) dor_space_allocate( (row)  * sizeof(DtNPoint3 *));
	for(i=0; i<divs+1; i++) {
		grid[i] = (DtNPoint3 *) dor_space_allocate(
			(i+1) * sizeof(DtNPoint3));
	}
    }

    /* Do first octant */

    /* initialize corner points of triangular grid */
    grid[0][0].x = 0.;
    grid[0][0].y = 0.;
    grid[0][0].z = 1.;
    grid[divs][0].x = 1.;
    grid[divs][0].y = 0.;
    grid[divs][0].z = 0.;
    grid[divs][divs].x = 0.;
    grid[divs][divs].y = 1.;
    grid[divs][divs].z = 0.;

    /* compute points along edges of a great triangle */
    for(i=1, incr=M_PI_2/divs; i<=divs/2; i++)
    {
	grid[i][0].x    = grid[divs-i][0].z =
	grid[i][i].y    = grid[divs-i][divs-i].z =
	grid[divs][i].y = grid[divs][divs-i].x =
		dor_math_sin(i * incr);

	grid[i][0].y    = grid[divs-i][0].y =
	grid[i][i].x    = grid[divs-i][divs-i].x =
	grid[divs][i].z = grid[divs][divs-i].z =
		0.0;

	grid[i][0].z    = grid[divs-i][0].x =
	grid[i][i].z    = grid[divs-i][divs-i].y =
	grid[divs][i].x = grid[divs][divs-i].y =
		dor_math_cos(i * incr);
    }

    /* compute interior points of great triangle */
    for(i=2; i<divs; i++)
    {
	incr = M_PI_2 / i;
	/* get normal of plane 0 */
	dor_math_crossproduct(
		grid[i][0].x, grid[i][0].y, grid[i][0].z,
		grid[i][i].x, grid[i][i].y, grid[i][i].z,
		&norm0.x, &norm0.y, &norm0.z);

	/* compute points along a great circle within great triangle */
		for(j=1; j<=i/2; j++)
    {
		dor_math_crossproduct(
			dor_math_cos(j * incr), /* pt on equator */
			dor_math_sin(j * incr),
			0.0,
			0.0, 0.0, 1.0,      /* north pole */
			&norm1.x, &norm1.y, &norm1.z);
		/* get vector of new grid point */
		dor_math_crossproduct(
			norm0.x, norm0.y, norm0.z,
			norm1.x, norm1.y, norm1.z,
			&grid[i][j].x, &grid[i][j].y, &grid[i][j].z);
		/* normalize to get point */
		dor_math_renormalize(
			&grid[i][j].x, &grid[i][j].y, &grid[i][j].z);

		if(2*j != i){
		    grid[i][i-j].x = grid[i][j].y;
		    grid[i][i-j].y = grid[i][j].x;
		    grid[i][i-j].z = grid[i][j].z;
		}
	}
    }

    /* Set up triangles for first octant */

    octcnt = 0;
    for(i=1; i<=divs; i++) {

        for(j=1; j<=i; j++) {

                tris[octcnt][0] = i*row+j-1;
                tris[octcnt][1] = i*row+j;
                tris[octcnt][2] = (i-1)*row+j-1;
                octcnt++;

                if(j != i) /* not end of row */ {
                        tris[octcnt][0] = (i-1)*row+j;
                        tris[octcnt][1] = (i-1)*row+j-1;
                        tris[octcnt][2] = i*row+j;
                        octcnt++;
                }
        }
    }
    octsize = octcnt;

    /* Reflect octant and build vertex list */

    vptr = sphvertices;
    for(i=0; i<=divs; i++) {

	for(j=0; j<=i; j++) {

		vtx = i*row+j;

		vptr = sphvertices + vtx*vertexsize;
		*vptr++ = x = grid[i][j].x;
		*vptr++ = y = grid[i][j].y;
		*vptr++ = z = grid[i][j].z;
		*vptr++ = nx = grid[i][j].x;
		*vptr++ = ny = grid[i][j].y;
		*vptr++ = nz = grid[i][j].z;
		if (cmptexuvswi) {
		    if (cmptexuvcbk == DcStdUVCallback) {
			*vptr++ = u = dor_prisrf_sphere_ucomp(y, x);
			*vptr++ = dor_prisrf_sphere_vcomp(z);
			if (i==1) {
			    saveu[j] = u;
			}
		    } else {
			dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                                uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                                vptr += 2;
                    }
		}
                if (cmptexuvwswi) {
		    if (cmptexuvwcbk == DcStdUVWCallback) {
                    	*vptr++ = x/2.+0.5;
                    	*vptr++ = .5 - y/2.;
                    	*vptr++ = .5 - z/2.;
		    } else {
			dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                                uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                                vptr += 3;
                    }
                 }

		vptr = sphvertices + (gridsize+vtx)*vertexsize;
		*vptr++ = x = -grid[i][j].x;
                *vptr++ = y = grid[i][j].y;
                *vptr++ = z = grid[i][j].z;
		*vptr++ = nx = -grid[i][j].x;
                *vptr++ = ny = grid[i][j].y;
                *vptr++ = nz = grid[i][j].z;
		if (cmptexuvswi) {
		    if (cmptexuvcbk == DcStdUVCallback) {
			*vptr++ = u = dor_prisrf_sphere_ucomp(y, x);
			*vptr++ = dor_prisrf_sphere_vcomp(z);
			if (i==1) {
			    saveu[2+j] = u;
			}
		    } else {
			dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        	uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                		vptr += 2;
            	    }
		}
                if (cmptexuvwswi) {
		    if (cmptexuvwcbk == DcStdUVWCallback) {
                    	*vptr++ = x/2.+0.5;
                    	*vptr++ = .5 - y/2.;
                    	*vptr++ = .5 - z/2.;
		    } else {
			dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        	uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                		vptr += 3;
            	    }
                 }

		vptr = sphvertices + (2*gridsize+vtx)*vertexsize;
                *vptr++ = x = grid[i][j].x;
                *vptr++ = y = -grid[i][j].y;
                *vptr++ = z = grid[i][j].z;
                *vptr++ = nx = grid[i][j].x;
                *vptr++ = ny = -grid[i][j].y;
                *vptr++ = nz = grid[i][j].z;
		if (cmptexuvswi) {
		    if (cmptexuvcbk == DcStdUVCallback) {
			*vptr++ = u = dor_prisrf_sphere_ucomp(y, x);
			*vptr++ = dor_prisrf_sphere_vcomp(z);
			if (i==1) {
			    saveu[4+j] = u;
			}
                    } else {
                        dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                                uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                                vptr += 2;
                    }
		}
                if (cmptexuvwswi) {
		    if (cmptexuvwcbk == DcStdUVWCallback) {
                    *vptr++ = x/2.+0.5;
                    *vptr++ = .5 - y/2.;
                    *vptr++ = .5 - z/2.;
                    } else {
                        dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                                uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                                vptr += 3;
                    }
                 }

		vptr = sphvertices + (3*gridsize+vtx)*vertexsize;
                *vptr++ = x = grid[i][j].x;
                *vptr++ = y = grid[i][j].y;
                *vptr++ = z = -grid[i][j].z;
                *vptr++ = nx = grid[i][j].x;
                *vptr++ = ny = grid[i][j].y;
                *vptr++ = nz = -grid[i][j].z;
		if (cmptexuvswi) {
                    if (cmptexuvcbk == DcStdUVCallback) {
			*vptr++ = u = dor_prisrf_sphere_ucomp(y, x);
			*vptr++ = dor_prisrf_sphere_vcomp(z);
			if (i==1) {
			    saveu[6+j] = u;
			}
                    } else {
                        dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                                uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                                vptr += 2;
                    }
		}
                if (cmptexuvwswi) {
                    if (cmptexuvwcbk == DcStdUVWCallback) {
                    	*vptr++ = x/2.+0.5;
                    	*vptr++ = .5 - y/2.;
                    	*vptr++ = .5 - z/2.;
                    } else {
                        dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                                uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                                vptr += 3;
                    }
                 }

		vptr = sphvertices + (4*gridsize+vtx)*vertexsize;
                *vptr++ = x = -grid[i][j].x;
                *vptr++ = y = -grid[i][j].y;
                *vptr++ = z = grid[i][j].z;
                *vptr++ = nx = -grid[i][j].x;
                *vptr++ = ny = -grid[i][j].y;
                *vptr++ = nz = grid[i][j].z;
		if (cmptexuvswi) {
                    if (cmptexuvcbk == DcStdUVCallback) {
			if (grid[i][j].y == 0)
				*vptr++ = u = 0.;
			else
				*vptr++ = u = dor_prisrf_sphere_ucomp(y, x);
			*vptr++ = dor_prisrf_sphere_vcomp(z);
			if (i==1) {
			    saveu[8+j] = u;
			}
		    } else {
			dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                                uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                                vptr += 2;
                    }
		}
                if (cmptexuvwswi) {
                    if (cmptexuvwcbk == DcStdUVWCallback) {
                    	*vptr++ = x/2.+0.5;
                    	*vptr++ = .5 - y/2.;
                    	*vptr++ = .5 - z/2.;
		    } else {
			dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                                uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                                vptr += 3;
                    }
                 }

		vptr = sphvertices + (5*gridsize+vtx)*vertexsize;
                *vptr++ = x = -grid[i][j].x;
                *vptr++ = y = grid[i][j].y;
                *vptr++ = z = -grid[i][j].z;
                *vptr++ = nx = -grid[i][j].x;
                *vptr++ = ny = grid[i][j].y;
                *vptr++ = nz = -grid[i][j].z;
		if (cmptexuvswi) {
                    if (cmptexuvcbk == DcStdUVCallback) {
			*vptr++ = u = dor_prisrf_sphere_ucomp(y, x);
			*vptr++ = dor_prisrf_sphere_vcomp(z);
			if (i==1) {
			    saveu[10+j] = u;
			}
                    } else {
                        dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                                uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                                vptr += 2;
		    }
		}
                if (cmptexuvwswi) {
                    if (cmptexuvwcbk == DcStdUVWCallback) {
                    	*vptr++ = x/2.+0.5;
                    	*vptr++ = .5 - y/2.;
                    	*vptr++ = .5 - z/2.;
                    } else {
                        dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                                uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                                vptr += 3;
		    }
                 }

		vptr = sphvertices + (6*gridsize+vtx)*vertexsize;
                *vptr++ = x = grid[i][j].x;
                *vptr++ = y = -grid[i][j].y;
                *vptr++ = z = -grid[i][j].z;
                *vptr++ = nx = grid[i][j].x;
                *vptr++ = ny = -grid[i][j].y;
                *vptr++ = nz = -grid[i][j].z;
		if (cmptexuvswi) {
                    if (cmptexuvcbk == DcStdUVCallback) {
			*vptr++ = u = dor_prisrf_sphere_ucomp(y, x);
			*vptr++ = dor_prisrf_sphere_vcomp(z);
			if (i==1) {
			    saveu[12+j] = u;
			}
                    } else {
                        dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                                uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                                vptr += 2;
                    }
		}
                if (cmptexuvwswi) {
                    if (cmptexuvwcbk == DcStdUVWCallback) {
                    	*vptr++ = x/2.+0.5;
                    	*vptr++ = .5 - y/2.;
                    	*vptr++ = .5 - z/2.;
                    } else {
                        dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                                uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                                vptr += 3;
                    }
                 }

		vptr = sphvertices + (7*gridsize+vtx)*vertexsize;
                *vptr++ = x = -grid[i][j].x;
                *vptr++ = y = -grid[i][j].y;
                *vptr++ = z = -grid[i][j].z;
                *vptr++ = nx = -grid[i][j].x;
                *vptr++ = ny = -grid[i][j].y;
                *vptr++ = nz = -grid[i][j].z;
		if (cmptexuvswi) {
                    if (cmptexuvcbk == DcStdUVCallback) {
			if (grid[i][j].y == 0)
				*vptr++ = u = 0.;
			else
				*vptr++ = u = dor_prisrf_sphere_ucomp(y, x);
			*vptr++ = dor_prisrf_sphere_vcomp(z);
			if (i==1) {
			    saveu[14+j] = u;
			}
		    } else {
                        dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                                uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                                vptr += 2;
                    }
		}
                if (cmptexuvwswi) {
                    if (cmptexuvwcbk == DcStdUVWCallback) {
                    	*vptr++ = x/2.+0.5;
                    	*vptr++ = .5 - y/2.;
                    	*vptr++ = .5 - z/2.;
		    } else {
                        dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                                uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                                vptr += 3;
                    }
                 }

	}
    }

    /* Four triangles come together to make the top and bottom
	of the sphere.  For the standard uv callback reset u values
	for the apex point of each of these triangles to be half
	way between u values at the two other vertices */

    if (cmptexuvswi && cmptexuvcbk == DcStdUVCallback) {
	for (i=0; i<8; i++) {
	    vptr = sphvertices + i*gridsize*vertexsize + 6;
	   *vptr = (saveu[2*i]+saveu[2*i+1])/2.;
	}
    }

    /* Set up triangles for all other octants */
    octcnt=0;
    for(i=1; i<=divs; i++) {

	for(j=1; j<=i; j++) {

		dor_prisrf_sphere_triangles(octcnt,octsize, gridsize,
			i*row+j-1,i*row+j,(i-1)*row+j-1);
		octcnt++;

		if(j != i) /* not end of row */ {
			dor_prisrf_sphere_triangles(
				octcnt,octsize, gridsize,
				(i-1)*row+j,(i-1)*row+j-1,i*row+j);
			octcnt++;
		}
	}
    }

    trimshobj = dor_trimsh_create(DcRGB, vertextype,
				8*gridsize,sphvertices,octsize*8,tris,DcTrue);

    return(trimshobj);

}

void dor_prisrf_sphere_triangles (
    DtInt cnt,
    DtInt octsize,
    DtInt gridsize,
    DtInt v0,
    DtInt v1,
    DtInt v2)
{
    tris[octsize+cnt][0] = gridsize+v1;
    tris[octsize+cnt][1] = gridsize+v0;
    tris[octsize+cnt][2] = gridsize+v2;

    tris[2*octsize+cnt][0] = 2*gridsize+v1;
    tris[2*octsize+cnt][1] = 2*gridsize+v0;
    tris[2*octsize+cnt][2] = 2*gridsize+v2;

    tris[3*octsize+cnt][0] = 3*gridsize+v1;
    tris[3*octsize+cnt][1] = 3*gridsize+v0;
    tris[3*octsize+cnt][2] = 3*gridsize+v2;

    tris[4*octsize+cnt][0] = 4*gridsize+v0;
    tris[4*octsize+cnt][1] = 4*gridsize+v1;
    tris[4*octsize+cnt][2] = 4*gridsize+v2;

    tris[5*octsize+cnt][0] = 5*gridsize+v0;
    tris[5*octsize+cnt][1] = 5*gridsize+v1;
    tris[5*octsize+cnt][2] = 5*gridsize+v2;

    tris[6*octsize+cnt][0] = 6*gridsize+v0;
    tris[6*octsize+cnt][1] = 6*gridsize+v1;
    tris[6*octsize+cnt][2] = 6*gridsize+v2;

    tris[7*octsize+cnt][0] = 7*gridsize+v1;
    tris[7*octsize+cnt][1] = 7*gridsize+v0;
    tris[7*octsize+cnt][2] = 7*gridsize+v2;
}

DtReal dor_prisrf_sphere_ucomp (
    DtReal y,
    DtReal x)
{
    if ((fabs((double)x) < 0.0000001) && (fabs((double)y) < 0.0000001))
	    return (0.5);

    return (dor_math_atan2(y,x)/TWO_PI + .5);
}


DtReal dor_prisrf_sphere_vcomp (DtReal z)
{
    return (dor_math_acos(z)/M_PI);
}
/*
 ======================================================================
 */

DtFlag dor_prisrf_check_subdiv (struct prisrf *prisrf)
{
    DtFlag returnflag;
    DtInt subdivtype;
    DtReal subdivparms[1];

    returnflag = DcFalse;

    dor_global_subdivspc_get_value(&subdivtype, subdivparms);

    if (prisrf->subdivtype != subdivtype) {
	prisrf->subdivtype = subdivtype;
	returnflag = DcTrue;
    }

    if (prisrf->subdivparms[0] != subdivparms[0]) {
	prisrf->subdivparms[0] = subdivparms[0];
	returnflag = DcTrue;
    }

    return (returnflag);
}
/*
 ======================================================================
 */

void dor_prisrf_compuv_usercallback (
    DtObject cmptexuvcbk,
    DtPFI function,
    DtObject dataobject,
    DtReal x,
    DtReal y,
    DtReal z,
    DtReal nx,
    DtReal ny,
    DtReal nz,
    DtReal *u,
    DtReal *v)
{
    DtPtr datptr;
    Dt32Bits datval;
    DtInt type;
    DtInt dataptr_class_id = dor_class_inq_id("DoDataPtr");
    DtInt dataval_class_id = dor_class_inq_id("DoDataVal");

    if (dataobject == DcNullObject) {
        datval = 0;
        if (dor_calbak_get_pass_by_ref(cmptexuvcbk)){
            (*function)(&datval, &x,&y,&z,  &nx,&ny,&nz,  u, v);
        } else {
            (*function)(datval, x,y,z,  nx,ny,nz,  u, v);
        }
    } else if ((DcObjectValid == dor_object_validate(dataobject)) &&
              (((type = dor_object_inq_type(dataobject)) == dataptr_class_id) ||

              (type == dataval_class_id))) {
        if (type == dataptr_class_id) {
            datptr = dor_datptr_get_value(dataobject);
            if (dor_calbak_get_pass_by_ref(cmptexuvcbk)) {
                (*function)(&datptr, &x,&y,&z,  &nx,&ny,&nz,  u, v);
            } else {
                (*function)(datptr, x,y,z,  nx,ny,nz,  u, v);
            }
        } else if (type == dataval_class_id) {
            datval = dor_datval_get_value(dataobject);
            if (dor_calbak_get_pass_by_ref(cmptexuvcbk)) {
                (*function)(&datval, &x,&y,&z,  &nx,&ny,&nz,  u, v);
            } else {
                (*function)(datval, x,y,z,  nx,ny,nz,  u, v);
            }
        }
    }
}
/*
 ======================================================================
 */

void dor_prisrf_compuvw_usercallback (
    DtObject cmptexuvwcbk,
    DtPFI function,
    DtObject dataobject,
    DtReal x,
    DtReal y,
    DtReal z,
    DtReal nx,
    DtReal ny,
    DtReal nz,
    DtReal *u,
    DtReal *v,
    DtReal *w)
{
    DtPtr datptr;
    Dt32Bits datval;
    DtInt type;
    DtInt dataptr_class_id = dor_class_inq_id("DoDataPtr");
    DtInt dataval_class_id = dor_class_inq_id("DoDataVal");

    if (dataobject == DcNullObject) {
        datval = 0;
        if (dor_calbak_get_pass_by_ref(cmptexuvwcbk)){
            (*function)(&datval, &x,&y,&z,  &nx,&ny,&nz,  u,v,w);
        } else {
            (*function)(datval, x,y,z,  nx,ny,nz,  u,v,w);
        }
    } else if ((DcObjectValid == dor_object_validate(dataobject)) &&
              (((type = dor_object_inq_type(dataobject)) == dataptr_class_id) ||

              (type == dataval_class_id))) {
        if (type == dataptr_class_id) {
            datptr = dor_datptr_get_value(dataobject);
            if (dor_calbak_get_pass_by_ref(cmptexuvwcbk)) {
                (*function)(&datptr, &x,&y,&z,  &nx,&ny,&nz,  u,v,w);
            } else {
                (*function)(datptr, x,y,z,  nx,ny,nz,  u,v,w);
            }
        } else if (type == dataval_class_id) {
            datval = dor_datval_get_value(dataobject);
            if (dor_calbak_get_pass_by_ref(cmptexuvwcbk)) {
                (*function)(&datval, &x,&y,&z,  &nx,&ny,&nz,  u,v,w);
            } else {
                (*function)(datval, x,y,z,  nx,ny,nz,  u,v,w);
            }
        }
    }
}

/*
 ======================================================================
 */

void dor_prisrf_compute_bounding_volume (dot_object *prisrfobject)
{
    struct prisrf *prisrf;
    static DtReal bv_sphere_pts[24] = /* a bounding box */
	    {   1., 1., -1.,
	        1.,-1., -1.,
	       -1., 1., -1.,
	       -1.,-1., -1.,
	        1., 1., 1.,
	        1.,-1., 1.,
	       -1., 1., 1.,
	       -1.,-1., 1. };
    static DtReal bv_cylinder_pts[24] =	/* a bounding box */
	    {   1., 1., 0.,
	        1.,-1., 0.,
	       -1., 1., 0.,
	       -1.,-1., 0.,
	        1., 1., 1.,
	        1.,-1., 1.,
	       -1., 1., 1.,
	       -1.,-1., 1. };
    static DtReal bv_box_pts[24] = /* a bounding box */
	    {   0., 0., 0.,
	        0., 0., 1.,
	        0., 1., 0.,
	        0., 1., 1.,
	        1., 0., 0.,
	        1., 0., 1.,
	        1., 1., 0.,
	        1., 1., 1. };
    static DtReal bv_cone_pts[15] = /* a bounding pyramid */
	    {   1., 1., 0.,
	        1.,-1., 0.,
	       -1., 1., 0.,
	       -1.,-1., 0.,
	        0., 0., 1. };

    prisrf = (struct prisrf *)prisrfobject->data;

    switch(prisrf->surface_type){
    case DcSphere:
	dor_system_report_bounding_points(8, bv_sphere_pts);
	break;
    case DcCylinder:
	dor_system_report_bounding_points(8, bv_cylinder_pts);
	break;
    case DcBox:
	dor_system_report_bounding_points(8, bv_box_pts);
	break;
    case DcCone:
	dor_system_report_bounding_points(5, bv_cone_pts);
	break;
    }
}
/*
 ======================================================================
 */

void dor_prisrf_pick (dot_object *object)
{
    struct prisrf *prisrf;
    DtFlag changed;

    if (!dor_condex_query_pick(doe_prisrf_class_id))
	    return;

    prisrf = (struct prisrf *)(object->data);
    changed = dor_prisrf_check_subdiv(prisrf);

    dor_pick_initialize_object(object);

    if ((prisrf->alternate_object != DcNullPtr) && !changed) {
	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(prisrf->alternate_object,
			      DcMethodPick)(prisrf->alternate_object);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    } else {
	if (prisrf->alternate_object != NULL) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(prisrf->alternate_object,
				  DcMethodDestroy)(prisrf->alternate_object);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    prisrf->alternate_object = NULL;
	}

	prisrf->alternate_object = dor_prisrf_create_alternate_geometry(prisrf);

	if (prisrf->alternate_object == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_prisrf_pick", "");
	    return;
	}

	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(prisrf->alternate_object,
			      DcMethodPick)(prisrf->alternate_object);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    }
}
/*
 ======================================================================
 */

void dor_prisrf_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_prisrf_class_id))
	    return;

    alt = dor_prisrf_update_alternate (object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
