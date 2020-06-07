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
	ddr_g4vll_drom_prisrf_create
	ddr_g4vll_drom_prisrf_print
	ddr_g4vll_drom_prisrf_delete
	ddr_g4vll_drom_prisrf_render
	ddr_g4vll_drom_prisrf_sphere_render 
	ddr_g4vll_drom_prisrf_sphere_render_firmware
	ddr_g4vll_drom_prisrf_sphere_render_software
	ddr_g4vll_drom_prisrf_cylinder_render
	ddr_g4vll_drom_prisrf_cylinder_render_firmware
	ddr_g4vll_drom_prisrf_cylinder_render_software
	ddr_g4vll_drom_prisrf_create_alternate_geometry
	ddr_g4vll_drom_prisrf_update_tristrip_alternate
	ddr_g4vll_drom_prisrf_create_alternate_geometry_cylinder
	ddr_g4vll_drom_prisrf_create_alternate_geometry_cone

  ======================================================================
 */
#include <math.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/tristrip.h>
#include "drom.h"

#define TWO_PI		(M_PI * 2.)
/*the next 2 defines patch the z-accuracy hardware problem*/
#define NOT_QUITE_ZERO	0.008 
#define NOT_QUITE_ONE	0.992
/*
  ======================================================================
 */

static DtInt classid   = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */

ddt_g4vll_drom_prisrf *ddr_g4vll_drom_prisrf_create (
    DtObjectStructure *object)
{
    ddt_g4vll_drom_prisrf *drom_prisrf;

    drom_prisrf = (ddt_g4vll_drom_prisrf *)
		DDspace_Allocate (sizeof(ddt_g4vll_drom_prisrf));

    drom_prisrf->tristrip_altobj = (DtObjectStructure *)0;

    return drom_prisrf;
}

/*
  ======================================================================
 */

void ddr_g4vll_drom_prisrf_print (
    DtObjectStructure *object,
    ddt_g4vll_drom_prisrf *data)
{
}

/*
  ======================================================================
 */

void ddr_g4vll_drom_prisrf_delete (
    DtObjectStructure *object,
    ddt_g4vll_drom_prisrf *data)
{
    if (data != (ddt_g4vll_drom_prisrf *)0) {
	if (data->tristrip_altobj != (DtObjectStructure *)0) {
	    DDspace_Deallocate(data->tristrip_altobj);
	}
    	DDspace_Deallocate(data);
    }
}

/*
  ======================================================================
 */
void ddr_g4vll_drom_prisrf_render (
    dot_object *object)
{
    static DtMethodPtr altobj_query = DcNullPtr;
    struct prisrf *prisrf;
    DtObject alt_obj;

    if (classid == -1) 
	    classid = DsInqClassId("DoPrimSurf");

    if (!DDcondex_QueryRender(classid))
	    return;

    prisrf = (struct prisrf *)(object->data);

    switch(prisrf->surface_type) {
    case DcCone:
	/* 
	 * Update and use the triangle strip alternate representation.
	 */
	alt_obj = ddr_g4vll_drom_prisrf_update_tristrip_alternate(object);

	if (alt_obj != (DtObject)0) {
	    DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	} else {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
			"ddr_g4vll_drom_prisrf_render", "");
	    return;
	}
	break;
    case DcSphere:
	ddr_g4vll_drom_prisrf_sphere_render(object);
	break;
    case DcCylinder:
	ddr_g4vll_drom_prisrf_cylinder_render(object);
	break;
    case DcBox:
    default:
	/*
	 * Update and use the standard alternate representation. 
	 */
	if (!altobj_query) {
	    altobj_query = DDobject_InqMethod(object, DcMethodUpdStdAltObj);
	}

	if (altobj_query) {
	    alt_obj = (DtObject)(*altobj_query)(object);

	    if (alt_obj != (DtObject)0) {
		DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	    } else {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "ddr_g4vll_drom_prisrf_render", "");
		return;
	    }
	}
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_prisrf_sphere_render (
    dot_object *object)
{
    static DtMethodPtr gentexuv_query = DcNullPtr;
    static DtMethodPtr gentexuvw_query = DcNullPtr;
    auto   DtSwitch    uvswitchvalue, uvwswitchvalue;
    auto   DtFlag      texture_mapping = DcFalse;

    if (!gentexuv_query)
	    gentexuv_query =  DDclass_InqMethod
		(DsInqClassId("AttCompTextureUVSwitch"), DcMethodInqGlbAttVal);
    (*gentexuv_query)(&uvswitchvalue);

    if (!gentexuvw_query)
	    gentexuvw_query =  DDclass_InqMethod
		(DsInqClassId("AttCompTextureUVWSwitch"), DcMethodInqGlbAttVal);
    (*gentexuvw_query)(&uvwswitchvalue);

    /* 
     * Use the 29050 spheres only if the user has requested them specifically.
     * The alternate representation will be used instead of the firmware if
     * any of the following conditions exist:
     *   -the spheres do not have a surface representation type
     *   -the spheres are texture mapped (texture mapping wins over transparency
     *    when both are present)
     * Note: only up to three _directional_ light sources will be applied by VLL.
     */

    if (dde_g4vll_drom_curdevdat->use_spheres &&
	(dde_g4vll_drom_data.reptype == DcSurface)) {

	if (dde_g4vll_drom_data.map_diffuse_color_switch &&
	    	((uvswitchvalue && dde_g4vll_drom_data.texture_2D_valid) ||
            	(uvwswitchvalue && dde_g4vll_drom_data.texture_3D_valid))) {
	    texture_mapping = DcTrue;
	}

    	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_standard_pass) {
	    if (texture_mapping) {
		dde_g4vll_drom_data.have_textures = DcTrue;
		return;
	    } 
	    if (dde_g4vll_drom_data.transparent_switch) {
		dde_g4vll_drom_data.have_transparency = DcTrue;
		return;
	    }
	    ddr_g4vll_drom_prisrf_sphere_render_firmware();
        } else
        if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_texture_pass) {
	    if (texture_mapping) {
	        ddr_g4vll_drom_prisrf_sphere_render_software(object);
	    }
        } else
        if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_transp_pass) {
	    if (dde_g4vll_drom_data.transparent_switch && !texture_mapping) {
	        ddr_g4vll_drom_prisrf_sphere_render_firmware();
            }
	} else
	if (dde_g4vll_drom_data.traversal_type == 
		ddd_g4vll_drom_transp_texture_pass) {
	    if ((dde_g4vll_drom_data.transparent_switch &&
                    (ddr_g4vll_dcm_inq_transp_mode() == 
		     ddd_g4vll_transp_multi_pass)) && texture_mapping) {
		ddr_g4vll_drom_prisrf_sphere_render_software(object);
	    }
        }
    } else {
	ddr_g4vll_drom_prisrf_sphere_render_software(object);
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_prisrf_sphere_render_firmware()
{
    auto   DtReal      center[1][3];
    auto   DtReal      radius[1];
    auto   DtReal      color[1][3];

    center[0][0] = 0;
    center[0][1] = 0;
    center[0][2] = 0;
    radius[0] = 1;
    color[0][0] = dde_g4vll_drom_data.diffuse_color[0];
    color[0][1] = dde_g4vll_drom_data.diffuse_color[1];
    color[0][2] = dde_g4vll_drom_data.diffuse_color[2];

    VLLset_clip_status(0);
    VLLsphere_list(1, (void *)center, (void *)radius, (void *)color, (void *)0);

    if (doe_system_timing_display_traverse_count == 1)
	doe_system_timing_firmware_spheres_drawn++;
}

/* ====================================================================== */

void ddr_g4vll_drom_prisrf_sphere_render_software(
    dot_object *object)
{
    static DtMethodPtr altobj_query = DcNullPtr;
    DtObject alt_obj;

    if (!altobj_query) {
	altobj_query = DDobject_InqMethod(object, DcMethodUpdStdAltObj);
    }

    if (altobj_query) {
	alt_obj = (DtObject)(*altobj_query)(object);

	if (alt_obj != (DtObject)0) {
	    DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	} else {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "ddr_g4vll_drom_prisrf_render", "");
	    return;
	}
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_prisrf_cylinder_render (
    dot_object *object)
{
    static DtMethodPtr gentexuv_query = DcNullPtr;
    static DtMethodPtr gentexuvw_query = DcNullPtr;
    auto   DtSwitch    uvswitchvalue, uvwswitchvalue;
    auto   DtFlag      texture_mapping = DcFalse;

    /* 
     * The same restrictions outlined above for 29050 spheres apply to 
     * 29050 cylinders.
     */

    if (!gentexuv_query)
	    gentexuv_query =  DDclass_InqMethod
		(DsInqClassId("AttCompTextureUVSwitch"), DcMethodInqGlbAttVal);
    (*gentexuv_query)(&uvswitchvalue);

    if (!gentexuvw_query)
	    gentexuvw_query =  DDclass_InqMethod
		(DsInqClassId("AttCompTextureUVWSwitch"), DcMethodInqGlbAttVal);
    (*gentexuvw_query)(&uvwswitchvalue);

#ifndef KWS1.3
    dde_g4vll_drom_curdevdat->cylinders = ddd_g4vll_cylinders_tesselated;
#endif

    if (!(dde_g4vll_drom_curdevdat->cylinders == ddd_g4vll_cylinders_tesselated) 
	&& (dde_g4vll_drom_data.reptype == DcSurface)) {

	if (dde_g4vll_drom_data.map_diffuse_color_switch &&
		((uvswitchvalue && dde_g4vll_drom_data.texture_2D_valid) ||
		(uvwswitchvalue && dde_g4vll_drom_data.texture_3D_valid))) {
	    texture_mapping = DcTrue;
	}

	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_standard_pass) {
	    if (texture_mapping) {
		dde_g4vll_drom_data.have_textures = DcTrue;
		return;
	    }
	    if (dde_g4vll_drom_data.transparent_switch &&
		    (ddr_g4vll_dcm_inq_transp_mode() ==
		    ddd_g4vll_transp_multi_pass)) {
		dde_g4vll_drom_data.have_transparency = DcTrue;
		return;
	    }
	    ddr_g4vll_drom_prisrf_cylinder_render_firmware();
	} else
	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_texture_pass) {
	    if (texture_mapping) {
		ddr_g4vll_drom_prisrf_cylinder_render_software(object);
	    }
	} else
	if (dde_g4vll_drom_data.traversal_type == ddd_g4vll_drom_transp_pass) {
	    if ((dde_g4vll_drom_data.transparent_switch &&
		(ddr_g4vll_dcm_inq_transp_mode() == ddd_g4vll_transp_multi_pass))
		&& !texture_mapping) {
		ddr_g4vll_drom_prisrf_cylinder_render_firmware();
	    }
	} else
	if (dde_g4vll_drom_data.traversal_type ==
		ddd_g4vll_drom_transp_texture_pass) {
	    if ((dde_g4vll_drom_data.transparent_switch &&
		    (ddr_g4vll_dcm_inq_transp_mode() ==
		    ddd_g4vll_transp_multi_pass)) && texture_mapping) {
		ddr_g4vll_drom_prisrf_cylinder_render_software(object);
	    }
	}
    } else {
	ddr_g4vll_drom_prisrf_cylinder_render_software(object);
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_prisrf_cylinder_render_firmware()
{
    auto   DtReal      center[2][3];
    auto   DtReal      radius[2];
    auto   DtReal      color[1][3];

    center[0][0] = 0.;
    center[0][1] = 0.;
    center[0][2] = 0.;
    center[1][0] = 0.;
    center[1][1] = 0.;
    center[1][2] = 1.;
    radius[0] = radius[1] = 1.;
    color[0][0] = dde_g4vll_drom_data.diffuse_color[0];
    color[0][1] = dde_g4vll_drom_data.diffuse_color[1];
    color[0][2] = dde_g4vll_drom_data.diffuse_color[2];

    VLLset_clip_status(0);

    if (dde_g4vll_drom_curdevdat->cylinders == 
		ddd_g4vll_cylinders_firmware_capped) {
	VLLcylinder_list(1, (void *)center, (void *)radius, (void *)color, 
		(void *)0, /* tess_num */ 0, /* capped */ 1, 
		/* default quality (3 pixel span per tri) */ 0.0);
	if (doe_system_timing_display_traverse_count == 1) {
	    doe_system_timing_firmware_cylinders_drawn++;
	}
    } else if (dde_g4vll_drom_curdevdat->cylinders == 
		ddd_g4vll_cylinders_firmware_uncapped) {
	VLLcylinder_list(1, (void *)center, (void *)radius, (void *)color, 
		(void *)0, /* tess_num */ 0, /* uncapped */ 0, 
		/* default quality (3 pixel span per tri) */ 0.0);

	if (doe_system_timing_display_traverse_count == 1) {
	    doe_system_timing_firmware_cylinders_drawn++;
	}
    }
}
/* ====================================================================== */

void ddr_g4vll_drom_prisrf_cylinder_render_software(
    dot_object *object)
{
    static DtMethodPtr altobj_query = DcNullPtr;
    DtObject alt_obj;

    if (!altobj_query) {
	altobj_query = DDobject_InqMethod(object, DcMethodUpdStdAltObj);
    }

    if (altobj_query) {
	alt_obj = (DtObject)(*altobj_query)(object);

	if (alt_obj != (DtObject)0) {
	    DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	} else {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
			"ddr_g4vll_drom_prisrf_render", "");
	    return;
	}
    }
}
/*
  ======================================================================
 */

dot_object *ddr_g4vll_drom_prisrf_update_tristrip_alternate (
    dot_object *object)
{
    struct prisrf *prisrf;
    DtFlag changed;
    dot_object *alt;
    ddt_g4vll_drom_prisrf *drom_prisrf;
    DtInt i;

    prisrf = (struct prisrf *)(object->data);

    changed = dor_prisrf_check_subdiv(prisrf);

    if (dataindex == -1) {
        for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
            if (dde_g4vll_drom_class_data[i].id == classid) {
                dataindex = dde_g4vll_drom_class_data[i].index;
                break;
            }
        }
    }
    drom_prisrf = (ddt_g4vll_drom_prisrf *)object->additional_data[dataindex];

    if ((drom_prisrf->tristrip_altobj != DcNullPtr) && !changed) {
        alt = drom_prisrf->tristrip_altobj;

    } else {
        if (drom_prisrf->tristrip_altobj != DcNullPtr) {
            DDobject_InqMethod(drom_prisrf->tristrip_altobj,
                    DcMethodDestroy)(drom_prisrf->tristrip_altobj);

            drom_prisrf->tristrip_altobj = DcNullPtr;
        }

        drom_prisrf->tristrip_altobj =
                ddr_g4vll_drom_prisrf_create_alternate_geometry(prisrf);
        alt = drom_prisrf->tristrip_altobj;

        if (alt == DcNullPtr) {
            DDerror(ERR_CANT_CREATE_ALTOBJ,
                       "ddr_g4vll_drom_prisrf_update_tristrip_alternate", "");
        }
    }
    return alt;
}
/*
  ======================================================================
 */

dot_object *ddr_g4vll_drom_prisrf_create_alternate_geometry (
    struct prisrf *prisrf)
{
    DtReal subdivlevel[1];
    DtInt subdivtype;
    DtSwitch cmptexuvswi, cmptexuvwswi;
    static DtMethodPtr routine_subdiv = DcNullPtr;
    static DtMethodPtr routine_uvswi  = DcNullPtr;
    static DtMethodPtr routine_uvwswi = DcNullPtr;

    if (!routine_subdiv) {
      routine_subdiv = DDclass_InqMethod(DsInqClassId("AttSubDivSpec"),
                                 DsInqMethodId("InqGlobalValue"));
    }
    (*routine_subdiv) (&subdivtype, subdivlevel);

    if (subdivtype != DcSubDivSegments) {
        /* old style subdivision spec */
        subdivlevel[0] = max(subdivlevel[0],0.);
        subdivlevel[0] = min(subdivlevel[0],1.);
    } else {
        subdivlevel[0] = max(subdivlevel[0],4.);
    }

    if (!routine_uvswi) {
      routine_uvswi = DDclass_InqMethod(DsInqClassId("AttCompTextureUVSwitch"),
                                 DsInqMethodId("InqGlobalValue"));
    }
    (*routine_uvswi) (&cmptexuvswi);

    if (!routine_uvwswi) {
      routine_uvwswi = DDclass_InqMethod(DsInqClassId("AttCompTextureUVWSwitch"),
                                 DsInqMethodId("InqGlobalValue"));
    }
    (*routine_uvwswi) (&cmptexuvwswi);

    switch(prisrf->surface_type){
    case DcCylinder:
        return(ddr_g4vll_drom_prisrf_create_alternate_geometry_cylinder(
               subdivtype, subdivlevel[0], cmptexuvswi, cmptexuvwswi));
        break;

    case DcCone:
        return(ddr_g4vll_drom_prisrf_create_alternate_geometry_cone(
               subdivtype, subdivlevel[0], cmptexuvswi, cmptexuvwswi));
        break;

    default:
        sprintf(DDerror_GetLine(),"Surface type %d",prisrf->surface_type);
        DDerror (ERR_VALUE_OUT_OF_RANGE,
                    "ddr_g4vll_drom_prisrf_create_alternate_geometry",
                    DDerror_GetLine());
        break;
    }

    return(DcNullPtr);
}

/*
  ======================================================================
 */

   /* The optimum alternate representation for a cylinder in this driver
    * is a triangle strip.  The strip is constructed based on the 
    * cylinder's subdivision level as follows:
    *
    *     Back Plate:   2 * subdivlevel vertices
    *     Side:         2 * (subdivlevel + 1) vertices
    *     Front Plate:  2 * subdivlevel vertices
    *
    * Cylindrical coordinates are used for parameterizing the cylinder
    * for 2D texture mapping :
    *
    *     For the cylindrical part:
    *	      u = n * 1./divisions, n = 0,..,divisions
    *	      v = -z  (note the z coordinate for the cylinder 
    *	        	is always between 0 and 1)
    *
    * 	  For the top circular plate:
    * 	      The top edge values are extended to the center point
    *
    *     For the bottom circular plate:
    *	       The bottom edge values are extended to the center point
    *
    * For 3D texture mapping, the cylinder is transformed to fit in the
    * space [0,1] in x, y, and z. The uvw are computed from these transformed
    * coordinates xyz, as: u=x,  v=1-y, and w=1-z.
    */


dot_object *ddr_g4vll_drom_prisrf_create_alternate_geometry_cylinder (
    DtInt subdivtype,
    DtReal subdivlevel,
    DtSwitch cmptexuvswi,
    DtSwitch cmptexuvwswi)
{
    static DtInt space=0;
    static DtReal *vertices;
    DtReal *vptr;
    static DtReal *circumference_vtx;
    DtInt needspace;
    DtInt divisions;
    DtInt vertexsize;
    DtReal angleincrement;
    DtReal uincrement;
    DtReal radius;
    DtInt vertex_count;
    DtInt vertex;
    DtReal x,y,z;
    DtReal nx,ny,nz;
    DtReal u;
    DtVertexType vertextype;
    dot_object *tristripobj;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    DtPFI uv_function;
    DtPFI uvw_function;
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;
    static DtMethodPtr routine_uvclbk  = DcNullPtr;
    static DtMethodPtr routine_uvwclbk = DcNullPtr;

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
        if (routine_uvclbk == DcNullPtr) {
              routine_uvclbk = DDclass_InqMethod(DsInqClassId
                              ("AttCompTextureUVCallback"),
                               DsInqMethodId("InqGlobalValue"));
        }
        (*routine_uvclbk) (&cmptexuvcbk);
        if (cmptexuvcbk != DcStdUVCallback) {
            uv_function = dor_calbak_get_function(cmptexuvcbk);
            uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);
        }
    }
    if (cmptexuvwswi) {
        vertexsize +=3;
        vertextype |= dor_system_texture_uvw_count(1);
        if (routine_uvwclbk == DcNullPtr) {
              routine_uvwclbk = DDclass_InqMethod(DsInqClassId
                              ("AttCompTextureUVWCallback"),
                               DsInqMethodId("InqGlobalValue"));
        }
        (*routine_uvwclbk) (&cmptexuvwcbk);
        if (cmptexuvwcbk != DcStdUVWCallback) {
            uvw_function = dor_calbak_get_function(cmptexuvwcbk);
            uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);
        }
    }

    needspace = 12*divisions*vertexsize;
    if (needspace > space) {
	if (space != 0) {
		vertices = (DtReal *)dor_space_reallocate(
				vertices, needspace*(sizeof (DtReal)));
		circumference_vtx = (DtReal *)dor_space_reallocate(
				circumference_vtx, (2*divisions)*(sizeof (DtReal)));
	} else {
		vertices = (DtReal *)dor_space_allocate(
				needspace*(sizeof (DtReal)));
		circumference_vtx = (DtReal *)dor_space_allocate(
				(2*divisions)*(sizeof (DtReal)));
	space = needspace;
	}
    }

    /* 
     * Generate the x/y points on the circumference of the cylinder.
     */
    angleincrement = TWO_PI / divisions;
    uincrement = 1.0 / divisions;
    radius = 1.0;

    vptr = circumference_vtx;
 
    for (vertex=0;vertex<divisions;vertex++) {
   	*vptr++ = /* radius * */ dor_math_cos(vertex*angleincrement);
   	*vptr++ = /* radius * */ dor_math_sin(vertex*angleincrement);
    }

    /*
     * Construct the triangle strip for the cylinder's back plate.
     */
    vptr = vertices;
    vertex_count = 0;

#ifdef DEBUG_2
printf ("cylinder back plate:\n");
#endif
    for (vertex=0;vertex<=divisions;vertex++) {
	/*
	 * Even vertices are on circumference; odd vertices are in the
	 * centre of the cap. (Yup, every second triangle is degenerate ...)
	 */
	if (vertex != divisions) {
	    *vptr++ = x = circumference_vtx[vertex*2];
	    *vptr++ = y = circumference_vtx[vertex*2+1];
   	    *vptr++ = z = 0.;
	} else {
	    *vptr++ = x = circumference_vtx[0];
	    *vptr++ = y = circumference_vtx[1];
   	    *vptr++ = z = 0.;
	}
#ifdef DEBUG_2
printf ("\t%g %g %g\n", x,y,z);
#endif
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = -1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = vertex*uincrement;
   		*vptr++ = NOT_QUITE_ONE;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
                *vptr++ = x/2. + .5;
                *vptr++ = .5 - y/2.;
                *vptr++ = 1.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
            }
        }

	if (vertex != divisions) {
	    *vptr++ = x = 0.;
	    *vptr++ = y = 0.;
   	    *vptr++ = z = 0.;
#ifdef DEBUG_2
printf ("\t%g %g %g\n", x,y,z);
#endif
   	    *vptr++ = nx = 0.;
   	    *vptr++ = ny = 0.;
   	    *vptr++ = nz = -1.;
	    if (cmptexuvswi) {
	        if (cmptexuvcbk == DcStdUVCallback) {
   		    *vptr++ = vertex*uincrement;
   		    *vptr++ = NOT_QUITE_ONE;
	        } else {
                    dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                            uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                    vptr += 2;
                }
	    }
            if (cmptexuvwswi) {
	        if (cmptexuvwcbk == DcStdUVWCallback) {
                    *vptr++ = .5;
                    *vptr++ = .5;
                    *vptr++ = 1.;
	        } else {
                    dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                            uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                    vptr += 3;
                }
            }

	    vertex_count+= 2;
	} else {
	    vertex_count+= 1;
	}
    }

    /*
     * Attach the side strip to the back plate, keeping the normals straight.
     * The first vertex on the side is identical to the last vertex on the 
     * back cap except that the normal is aligned with the side.
     */

#ifdef DEBUG_2
printf ("cylinder side :\n");
#endif
    for (vertex=0;vertex<=divisions;vertex++) {

        if (vertex != divisions) {
            *vptr++ = x = circumference_vtx[vertex*2];
            *vptr++ = y = circumference_vtx[vertex*2+1];
	    *vptr++ = z = 0.;
   	    u = vertex*uincrement;
        } else {
        /* avoid a crack in the model; make sure last vertex = first vertex */
            *vptr++ = x = radius;
            *vptr++ = y = 0.0;
	    *vptr++ = z = 0.;
            u = 1.0;
        }

#ifdef DEBUG_2
printf ("\t%g %g %g\n", x,y,z);
#endif
   	dor_math_normal(0.,0.,0., x, y, z, &nx, &ny, &nz);
	*vptr++ = nx;
	*vptr++ = ny;
	*vptr++ = nz;

	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
                if (vertex == divisions)
   		   *vptr++ = NOT_QUITE_ONE;
                else
   		   *vptr++ = u;
   		*vptr++ = NOT_QUITE_ONE; /* v */
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x, y, z,  nx, ny, nz, vptr, (vptr+1));
		vptr += 2;
            }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = x/2. + 0.5;
            	*vptr++ = .5 - y/2.;
            	*vptr++ = 1.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, x, y, z,  nx, ny, nz,
			vptr, (vptr+1), (vptr+2));
		vptr +=3;
            }
        }

	*vptr++ = x;
	*vptr++ = y;
   	*vptr++ = z = 1.;
#ifdef DEBUG_2
printf ("\t%g %g %g\n", x,y,z);
#endif

   	dor_math_normal(0.,0.,1., x, y, z,  &nx, &ny, &nz);
	*vptr++ = nx;
	*vptr++ = ny;
	*vptr++ = nz;

	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
                if (vertex == divisions)
   		   *vptr++ = NOT_QUITE_ONE;
                else
   		   *vptr++ = u;
   		*vptr++ = NOT_QUITE_ZERO;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x, y, z,  nx, ny, nz, vptr, (vptr+1));
		vptr += 2;
	    }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = x/2. + 0.5;
            	*vptr++ = .5 - y/2.;
            	*vptr++ = 0.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject,  x, y, z,  nx, ny, nz,  
			vptr, (vptr+1), (vptr+2));
		vptr += 3;
            }
        }

	vertex_count += 2;
    }

    /*
     * Finally, construct the triangle strip for the cylinder's front plate.
     */
#ifdef DEBUG_2
printf ("cylinder front plate :\n");
#endif
    for (vertex=0;vertex<=divisions;vertex++) {
	/*
	 * Even vertices are on circumference; odd vertices are in the
	 * centre of the cap.
	 */
	if (vertex != divisions) {
	    *vptr++ = x = circumference_vtx[vertex*2];
	    *vptr++ = y = circumference_vtx[vertex*2+1];
   	    *vptr++ = z = 1.;
	} else {
            *vptr++ = x = circumference_vtx[0];
            *vptr++ = y = circumference_vtx[1];
            *vptr++ = z = 1.;
        }
#ifdef DEBUG_2
printf ("\t%g %g %g\n", x,y,z);
#endif
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = 1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = vertex*uincrement;
   		*vptr++ = NOT_QUITE_ONE;
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

	if (vertex != divisions) {
	    *vptr++ = x = 0.,
	    *vptr++ = y = 0.,
   	    *vptr++ = z = 1.;
#ifdef DEBUG_2
printf ("\t%g %g %g\n", x,y,z);
#endif
   	    *vptr++ = nx = 0.;
   	    *vptr++ = ny = 0.;
   	    *vptr++ = nz = 1.;
	    if (cmptexuvswi) {
	        if (cmptexuvcbk == DcStdUVCallback) {
   		    *vptr++ = vertex*uincrement;
   		    *vptr++ = NOT_QUITE_ONE;
	        } else {
                    dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                            uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                    vptr += 2;
                }
	    }
            if (cmptexuvwswi) {
	        if (cmptexuvwcbk == DcStdUVWCallback) {
                    *vptr++ = .5;
                    *vptr++ = .5;
                    *vptr++ = 1.;
	        } else {
                    dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                            uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2));
                    vptr += 3;
                }
            }

	    vertex_count+= 2;
	} else {
	    vertex_count+= 1;
	}
    }

    tristripobj = dor_tristrip_create(DcRGB, vertextype, vertex_count, vertices);

    return(tristripobj);
}



/*****************************************************************************
**  A cone is subdivided into n triangles and another n triangles for the
**  bottom circular plate.
**
**  The following parameterization is used for texture mapping:
**      If you look down on the cone so the point is in the center
**      then you will see a circular cutout of the texture. 
**      There is no distortion.  
**
**          u = (x+1.)/2.
**          v = (1.-y)/2.
**
**     For the bottom circular plate, the bottom edge values of the 
**     conic part are extended to the center of the plate.
**
**     For 3D texture mapping, the cone is transformed to fit in the
**     space [0,1] in x, y, and z. The uvw are computed from these transformed
**     coordinates xyz, as: u=x,  v=1-y, and w=1-z.
*****************************************************************************/

dot_object *ddr_g4vll_drom_prisrf_create_alternate_geometry_cone (
    DtInt subdivtype,
    DtReal subdivlevel,
    DtSwitch cmptexuvswi,
    DtSwitch cmptexuvwswi)
{
    static DtInt space = 0;
    static DtReal *vertices;
    DtReal *vptr;
    static DtReal *circumference_vtx;
    DtInt needspace;
    DtInt vertexsize;
    DtInt divisions;
    DtReal angleincrement;
    DtReal uincrement;
    DtReal radius;
    DtReal mid_tri_x;
    DtReal mid_tri_y;
    DtInt vertex_count;
    DtInt vertex;
    DtReal x,y,z; 
    DtReal nx,ny,nz;
    DtVertexType vertextype;
    dot_object *tristripobj;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    DtPFI uv_function;
    DtPFI uvw_function;
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;
    static DtMethodPtr routine_uvclbk  = DcNullPtr;
    static DtMethodPtr routine_uvwclbk = DcNullPtr;

    if (subdivtype != DcSubDivSegments) 
	/* old style subdivision spec */
   	divisions = 5 + (subdivlevel*20.);
    else
	divisions = subdivlevel;

    vertexsize = 6;
    vertextype = DcLocNrm;

    /*  Are texture uv's and/or uvw's to be computed?  If so, determine whether
    **  to use the callback provided by Dore or a user-defined callback. */

    if (cmptexuvswi) {
	vertexsize += 2;
        vertextype |= dor_system_texture_uv_count(1);

	if (!routine_uvclbk) {
	    routine_uvclbk = DDclass_InqMethod
			     (   DsInqClassId ("AttCompTextureUVCallback"),
				 DsInqMethodId ("InqGlobalValue")
			     );
	}
	(*routine_uvclbk) (&cmptexuvcbk);

        if (cmptexuvcbk != DcStdUVCallback) {
            uv_function = dor_calbak_get_function(cmptexuvcbk);
            uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);
	}
    }
    if (cmptexuvwswi) {
	vertexsize += 3 ;
	vertextype |= dor_system_texture_uvw_count(1);
	if (!routine_uvwclbk) {
	    routine_uvwclbk = DDclass_InqMethod
			      (   DsInqClassId ("AttCompTextureUVWCallback"),
			          DsInqMethodId ("InqGlobalValue")
			      );
	}
	(*routine_uvwclbk) (&cmptexuvwcbk);

        if (cmptexuvwcbk != DcStdUVWCallback) {
            uvw_function = dor_calbak_get_function(cmptexuvwcbk);
            uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);
	}
    }

    needspace = 6*divisions*vertexsize;
    if (needspace > space) {
        if (space != 0) {
        	vertices = (DtReal *)dor_space_reallocate(
				vertices, needspace*(sizeof (DtReal)));
		circumference_vtx = (DtReal *)dor_space_reallocate(
				circumference_vtx, (2*divisions)*(sizeof (DtReal)));
	} else {
        	vertices = (DtReal *)dor_space_allocate(
				needspace*(sizeof (DtReal)));
		circumference_vtx = (DtReal *)dor_space_allocate(
				(2*divisions)*(sizeof (DtReal)));
        space = needspace;
	}
    }

    /*
     * Generate the x/y points on the circumference of the cone.
     */
    angleincrement = TWO_PI/divisions;
    uincrement = 1./divisions;
    radius = 1.;

    vptr = circumference_vtx;

    for (vertex=0;vertex<divisions;vertex++) {
        *vptr++ = /* radius * */ dor_math_cos (vertex * angleincrement);
        *vptr++ = /* radius * */ dor_math_sin (vertex * angleincrement);
    }

    /* Construct the triangle strip for the cone's back plate.  */

    vptr = vertices;
    vertex_count = 0;

    for (vertex=0;  vertex <= divisions;  ++vertex) {

	/* Even vertices are on circumference; odd vertices are in the
	** centre of the cap. (Yup, every second triangle is degenerate ...) */

	if (vertex != divisions) {
   	    *vptr++ = x = circumference_vtx[2 * vertex];
   	    *vptr++ = y = circumference_vtx[2 * vertex + 1];
   	    *vptr++ = z = 0.0;
	} else {
	    *vptr++ = x = circumference_vtx[0];
	    *vptr++ = y = circumference_vtx[1];
	    *vptr++ = z = 0.0;
	}
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = -1.;
	if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = vertex*uincrement;
   		*vptr++ = NOT_QUITE_ONE;
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
		dor_prisrf_compuvw_usercallback
		(   cmptexuvwcbk, uvw_function,
                    uvw_dataobject, x,y,z, nx,ny,nz, vptr,(vptr+1),(vptr+2)
		);
		vptr += 3;
	    }
	}

	if (vertex != divisions) {
	    *vptr++ = x  =  0.0;
	    *vptr++ = y  =  0.0;
	    *vptr++ = z  =  0.0;
	    *vptr++ = nx =  0.0;
	    *vptr++ = ny =  0.0;
	    *vptr++ = nz = -1.0;
	    if (cmptexuvswi) {
		if (cmptexuvcbk == DcStdUVCallback) {
		    *vptr++ = vertex * uincrement;
		    *vptr++ = NOT_QUITE_ONE;
		} else {
		    dor_prisrf_compuv_usercallback
		    (   cmptexuvcbk, uv_function, uv_dataobject, x,y,z,
			nx,ny,nz, vptr, (vptr+1)
		    );
		    vptr += 2;
		}
	    }
	    if (cmptexuvwswi) {
		if (cmptexuvwcbk == DcStdUVWCallback) {
		    *vptr++ = 0.5;
		    *vptr++ = 0.5;
		    *vptr++ = 1.0;
		} else {
		    dor_prisrf_compuvw_usercallback
		    (   cmptexuvwcbk, uvw_function, uvw_dataobject, x,y,z,
			nx,ny,nz, vptr, vptr+1, vptr+2
		    );
		    vptr += 3;
		}
	    }
	    vertex_count += 2;
	} else {
	    vertex_count += 1;
	}
    }

    /* Add the cone's "top" strip.  The first vertex in the loop is on the
    ** the circumference; the second vertex is the apex of the cone.  */

    for (vertex=0;  vertex <= divisions;  ++vertex) {
	if (vertex != divisions) {
	    *vptr++ = x = circumference_vtx[2*vertex];
	    *vptr++ = y = circumference_vtx[2*vertex + 1];
   	    *vptr++ = z = 0.0;
	} else {
	    /* Avoid a crack in the model; ensure that the last vertex is
	    ** equal to the first vertex. */
	    *vptr++ = x = radius;
	    *vptr++ = y = 0.0;
   	    *vptr++ = z = 0.0;
	}

	dor_math_normal(0.,0.,0.,  x,y,1.0,  &nx,&ny,&nz);
	*vptr++ = nx;
	*vptr++ = ny;
	*vptr++ = nz;

	if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
		*vptr++ = (x + 1.)/2.;
                *vptr++ = (1. - y)/2.;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject,  x,y,z,  nx,ny,nz,  vptr, (vptr+1));
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
                        uvw_dataobject, x,y,z,  nx,ny,nz,  vptr, (vptr+1), (vptr+2));
                vptr += 3;
	    }
	}

	*vptr++ = x = 0.;
	*vptr++ = y = 0.;
	*vptr++ = z = 1.;

	mid_tri_x = /* radius * */ dor_math_cos((vertex+0.5)*angleincrement);
	mid_tri_y = /* radius * */ dor_math_sin((vertex+0.5)*angleincrement);
	dor_math_normal(0.,0.,0.,  mid_tri_x, mid_tri_y, 1.,  &nx, &ny, &nz);
	*vptr++ = nx;
	*vptr++ = ny;
	*vptr++ = nz;

        if (cmptexuvswi) {
            if (cmptexuvcbk == DcStdUVCallback) {
		*vptr++ = .5;
		*vptr++ = .5;
	    } else {
		dor_prisrf_compuv_usercallback (cmptexuvcbk, uv_function,
                        uv_dataobject,  x,y,z,  nx,ny,nz,  vptr, (vptr+1));
		vptr += 2;
	    }
	}
        if (cmptexuvwswi) {
            if (cmptexuvwcbk == DcStdUVWCallback) {
	    	*vptr++ = 0.5;
	    	*vptr++ = 0.5;
	    	*vptr++ = 0.0;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject,  x,y,z,  nx,ny,nz, vptr,(vptr+1),(vptr+2));
                vptr += 3;
	    }
	}
   	vertex_count += 2;
    }


    tristripobj = dor_tristrip_create (DcRGB,vertextype,vertex_count,vertices);

    return tristripobj;
}
