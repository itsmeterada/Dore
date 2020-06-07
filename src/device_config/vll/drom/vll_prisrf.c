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
        ddt_vll_drom_prisrf * ddr_vll_drom_prisrf_create
        int ddr_vll_drom_prisrf_print (object, data)
        int ddr_vll_drom_prisrf_delete (object, data)
	void ddr_vll_drom_prisrf_render (object)
	DtInt ddr_vll_drom_render_prisrf_sphere (prisrf)
	dot_object *ddr_vll_drom_prisrf_create_alternate_geometry(prisrf)
	dot_object *ddr_vll_drom_prisrf_update_quad_alternate(object)
	dot_object *ddr_vll_drom_prisrf_create_alternate_geometry_cylinder
                          (subdivtype, subdivlevel, cmptexuvswi, cmptexuvwswi)
	dot_object *ddr_vll_drom_prisrf_create_alternate_geometry_box
	                  (subdivtype, subdivlevel, cmptexuvswi, cmptexuawvswi)

  ======================================================================
 */
#include <math.h>
#include "drom.h"
#include "dore/internal/dogen.h"
#include "dore/dore_develop/develop.h"
#include "dore/dore_develop/private/prisrf.h"

/*
 ======================================================================
 */

typedef struct {
    DtObjectStructure *quad_altobj;
    DtInt quad_subdivtype;
    DtReal quad_subdivlevel[1];
} ddt_vll_drom_prisrf;

static DtRealTriple bbox[] = {
   -1.,-1.,-1., 1.,-1.,-1., 1.,1.,-1., -1.,1.,-1.,
   -1.,1.,1., 1.,1.,1., 1.,-1.,1., -1.,-1.,1.,
};
static DtInt classid = -1;
static DtInt dataindex = -1;

/*
  ======================================================================
 */

ddt_vll_drom_prisrf *
ddr_vll_drom_prisrf_create(object)
        DtObjectStructure *object;
{
    ddt_vll_drom_prisrf *dynout_prisrf;

    dynout_prisrf =  (ddt_vll_drom_prisrf *)
            DDspace_Allocate(sizeof(ddt_vll_drom_prisrf));

    dynout_prisrf->quad_altobj = (DtObjectStructure *)0;

    dynout_prisrf->quad_subdivtype = -1;

    return dynout_prisrf;
}
/*
  ======================================================================
 */

ddr_vll_drom_prisrf_print(object, data)
        DtObjectStructure *object;
        ddt_vll_drom_prisrf *data;
{
}

/*
  ======================================================================
 */

ddr_vll_drom_prisrf_delete(object, data)
        DtObjectStructure *object;
        ddt_vll_drom_prisrf *data;
{
    if (data->quad_altobj != (DtObjectStructure *)0) {
        DeDeleteObject(data->quad_altobj);
    }
    DDspace_Deallocate(data);
}

/*
  ======================================================================
 */
void
ddr_vll_drom_prisrf_render(object)
	dot_object *object;
{
    static DtPFI altobj_query = (DtPFI)0;
    struct prisrf *prisrf;
    dot_object *ddr_vll_drom_prisrf_update_quad_alternate();
    DtObject alt_obj;

    if (classid == -1) 
	    classid = DsInqClassId("DoPrimSurf");

    if (!DDcondex_QueryRender(classid))
	    return;

    prisrf = (struct prisrf *)(object->data);

    switch(prisrf->surface_type) {
    case DcSphere:
	if (dde_vll_drom_curdevdat->use_spheres) {
	    if (ddr_vll_drom_render_prisrf_sphere(prisrf)) {
		/* this routine will return 1 if it did the sphere otherwise
		 * use the alternate object
		 */
		break;
	    }
	} 
	/* if the device does not want to use hardware spheres then
	 * use standard alternate representation (note: no "break" here)
	 */

    case DcCone:
	/* update and use standard alternate representation (triangles) */

	if (altobj_query == (DtPFI)0) {
	    altobj_query = DDobject_InqMethod(object, DcMethodUpdStdAltObj);
	}

	if (altobj_query != (DtPFI)0) {
	    alt_obj = (DtObject)(*altobj_query)(object);

	    if (alt_obj != (DtObject)0) {
	    	DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	    } else {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "ddr_vll_drom_prisrf_render", "");
	        return;
	    }
	}
        break;

    case DcBox:
    case DcCylinder:
    default:
	/* update and use quadrilateral alternate representation  */

        alt_obj = ddr_vll_drom_prisrf_update_quad_alternate(object);
        
	if (alt_obj != (DtObject)0) {
	    DDobject_InqCurrentMethod(alt_obj) (alt_obj);
	 } else {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
                    "ddr_vll_drom_prisrf_render", "");
            return;
         }
        break;
    }
}
/*
  ======================================================================
 */
DtInt
ddr_vll_drom_render_prisrf_sphere(prisrf)
	struct prisrf *prisrf;
{
    DtReal center[1][3];
    DtReal radius[1];
    DtReal color[1][3];
    static DtPFI gentexuv_query = (DtPFI)DcNullPtr;
    static DtPFI gentexuvw_query = (DtPFI)DcNullPtr;
    DtSwitch uvswitchvalue, uvwswitchvalue;

    if (gentexuv_query == (DtPFI)DcNullPtr)
            gentexuv_query =
               DDclass_InqMethod (DsInqClassId("AttCompTextureUVSwitch"),
                                                 DcMethodInqGlbAttVal);
    (*gentexuv_query)(&uvswitchvalue);

    if (gentexuvw_query == (DtPFI)DcNullPtr)
            gentexuvw_query =
               DDclass_InqMethod (DsInqClassId("AttCompTextureUVWSwitch"),
                                                 DcMethodInqGlbAttVal);
    (*gentexuvw_query)(&uvwswitchvalue);

    /* 
     * The user has specified that this object should be able to be texture mapped
     * so use the tesselated spheres not the hardware sphere
     */
    if (((uvswitchvalue && dde_vll_drom_data.texture_2D_valid) ||
        (uvwswitchvalue && dde_vll_drom_data.texture_3D_valid)) &&
        dde_vll_drom_data.map_diffuse_color)
	    return 0;

    /*
     * The representation type is not a surface so use the tesselated spheres
     */
    if (dde_vll_drom_data.reptype != DcSurface) 
	    return 0;

    /*
     * if the view has texture maps then we can't use HW spheres 
     */
    if (dde_vll_drom_curviwdat->have_textures)
	    return 0;


    /*
     * We are in the transparent pass so use the tesselated spheres
     */
    if (dde_vll_drom_data.traversal_type ==  ddd_vll_drom_transp_pass)
	    return 0;



    if (!dde_vll_drom_data.have_spheres &&
	!dde_vll_drom_curviwdat->have_spheres) {
	/*
	 * This is the first sphere seen and didn't think there were any spheres
	 * so abort the traversal and indicate that we have spheres
	 */
	dde_vll_drom_data.have_spheres = DcTrue;
	DsExecutionAbort();
	return 1;
    }

    dde_vll_drom_data.have_spheres = DcTrue;
    center[0][0] = 0;
    center[0][1] = 0;
    center[0][2] = 0;
    radius[0] = 1;
    color[0][0] = dde_vll_drom_data.diffuse_color[0];
    color[0][1] = dde_vll_drom_data.diffuse_color[1];
    color[0][2] = dde_vll_drom_data.diffuse_color[2];

    VLLsphere_list(1, (void *)bbox, (void *)center, (void *)radius, 
		   (void *)color, (void *)0);

    return 1;
}
/*
  ======================================================================
 */

dot_object *ddr_vll_drom_prisrf_update_quad_alternate(object)
dot_object *object;
{
    struct prisrf *prisrf;
    DtFlag changed;
    dot_object *ddr_vll_drom_prisrf_create_alternate_geometry();
    DtFlag dor_prisrf_check_subdiv();
    dot_object *alt;
    ddt_vll_drom_prisrf *dynout_prisrf;
    DtInt i, quad_subdivtype;
    DtReal quad_subdivlevel[1];
    DtPFI routine_subdiv = DcNullPtr;

    prisrf = (struct prisrf *)(object->data);

    if (dataindex == -1) {
        for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
            if (dde_vll_drom_class_data[i].id == classid) {
                dataindex = dde_vll_drom_class_data[i].index;
                break;
            }
        }
    }
    dynout_prisrf = (ddt_vll_drom_prisrf *)object->additional_data[dataindex];

    /* check if subdivision spec has changed since last update of the quad
       alternate representation. Note that the subdivision level of the stored
       quad and triangle alt. representations may differ */

    if (routine_subdiv == (DtPFI)DcNullPtr) {
      routine_subdiv = DDclass_InqMethod(DsInqClassId("AttSubDivSpec"),
                                 DsInqMethodId("InqGlobalValue"));
    }
    (*routine_subdiv) (&quad_subdivtype, quad_subdivlevel);

    if (dynout_prisrf->quad_subdivtype != quad_subdivtype) {
         dynout_prisrf->quad_subdivtype = quad_subdivtype;
         changed = DcTrue;
    }

    if (dynout_prisrf->quad_subdivlevel[0] != quad_subdivlevel[0]) {
         dynout_prisrf->quad_subdivlevel[0] = quad_subdivlevel[0];
         changed = DcTrue;
    }

    if ((dynout_prisrf->quad_altobj != dod_object_null) && !changed) {
	alt = dynout_prisrf->quad_altobj;

    } else {
	if (dynout_prisrf->quad_altobj != dod_object_null) {
	    DDobject_InqMethod(dynout_prisrf->quad_altobj,
                    DcMethodDestroy)(dynout_prisrf->quad_altobj);

	    dynout_prisrf->quad_altobj = dod_object_null;
	}

	dynout_prisrf->quad_altobj = 
		ddr_vll_drom_prisrf_create_alternate_geometry(prisrf);
	alt = dynout_prisrf->quad_altobj;

	if (dynout_prisrf->quad_altobj == dod_object_null) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "ddr_vll_drom_prisrf_update_quad_alternate", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *ddr_vll_drom_prisrf_create_alternate_geometry(prisrf)
struct prisrf *prisrf;
{
    DtReal subdivlevel[1];
    DtInt subdivtype;
    DtSwitch cmptexuvswi, cmptexuvwswi;
    static DtPFI routine_subdiv = (DtPFI)DcNullPtr;
    static DtPFI routine_uvswi = (DtPFI)DcNullPtr;
    static DtPFI routine_uvwswi = (DtPFI)DcNullPtr;
    dot_object *ddr_vll_drom_prisrf_create_alternate_geometry_cylinder();
    dot_object *ddr_vll_drom_prisrf_create_alternate_geometry_box();

    if (routine_subdiv == (DtPFI)DcNullPtr) {
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

    if (routine_uvswi == (DtPFI)DcNullPtr) {
      routine_uvswi = DDclass_InqMethod(DsInqClassId("AttCompTextureUVSwitch"),
                                 DsInqMethodId("InqGlobalValue"));
    }
    (*routine_uvswi) (&cmptexuvswi);

    if (routine_uvwswi == (DtPFI)DcNullPtr) {
      routine_uvwswi = DDclass_InqMethod(DsInqClassId("AttCompTextureUVWSwitch"),
                                 DsInqMethodId("InqGlobalValue"));
    }
    (*routine_uvwswi) (&cmptexuvwswi);

    switch(prisrf->surface_type){
    case DcCylinder:
	return(ddr_vll_drom_prisrf_create_alternate_geometry_cylinder(
               subdivtype, subdivlevel[0], cmptexuvswi, cmptexuvwswi));
	break;

    case DcBox:
	return(ddr_vll_drom_prisrf_create_alternate_geometry_box(
               subdivtype, subdivlevel[0], cmptexuvswi, cmptexuvwswi));
	break;

    default:
	sprintf(DDerror_GetLine(),"Surface type %d",prisrf->surface_type);
	DDerror (ERR_VALUE_OUT_OF_RANGE,
		    "ddr_vll_drom_prisrf_create_alternate_geometry",
		    DDerror_GetLine());
	break;
    }
  
    return(dod_object_null);
}
/*
 ======================================================================
 */

dot_object *ddr_vll_drom_prisrf_create_alternate_geometry_box(subdivtype,
            subdivlevel, cmptexuvswi, cmptexuvwswi)
DtInt subdivtype;
DtReal subdivlevel;
DtSwitch cmptexuvswi;
DtSwitch cmptexuvwswi;
{
    DtInt vertex;
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    dot_pfi uv_function;
    dot_pfi uvw_function;
    dot_pfi dor_calbak_get_function();
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;
    dot_object *dor_calbak_get_dataobject();
    dot_object *quadlstobj;
    dot_object *dor_quadlst_create();
    static DtPFI routine_uvclbk = (DtPFI)DcNullPtr;
    static DtPFI routine_uvwclbk = (DtPFI)DcNullPtr;
    static DtReal *quadlst, *vertices;
    DtVertexType vertextype;
    DtInt vertexsize;

    static DtReal quadlstvtxs[24][3] = { 
  	0.,0.,0.,  0.,1.,0.,  1.,1.,0.,  1.,0.,0.,
   	0.,0.,1.,  1.,0.,1.,  1.,1.,1.,  0.,1.,1.,
   	0.,1.,0.,  0.,1.,1.,  1.,1.,1.,  1.,1.,0.,
   	0.,0.,0.,  1.,0.,0.,  1.,0.,1.,  0.,0.,1.,
   	0.,0.,0.,  0.,0.,1.,  0.,1.,1.,  0.,1.,0.,
   	1.,0.,0.,  1.,1.,0.,  1.,1.,1.,  1.,0.,1.
    };

    static DtReal uv[24][2] = { 
	/* comments assume z up, and y going in*/
	/*bottom*/
   	0.,1.,  0.,0.,  1.,0.,  1.,1.,

	/*top*/
	0.,1.,  1.,1.,  1.,0.,  0.,0.,

	/*back*/
	1.,1.,  1.,0.,  0.,0.,  0.,1.,

	/*front*/
	0.,1.,  1.,1.,  1.,0.,  0.,0.,

	/*left*/
	1.,1.,  1.,0.,  0.,0.,  0.,1.,

	/*right*/
	0.,1.,  1.,1.,  1.,0.,  0.,0.
    };

    static DtReal uvw[24][3] = { 
  	0.,1.,1.,  0.,0.,1.,  1.,0.,1.,  1.,1.,1.,
   	0.,1.,0.,  1.,1.,0.,  1.,0.,0.,  0.,0.,0.,
   	0.,0.,1.,  0.,0.,0.,  1.,0.,0.,  1.,0.,1.,
   	0.,1.,1.,  1.,1.,1.,  1.,1.,0.,  0.,1.,0.,
   	0.,1.,1.,  0.,1.,0.,  0.,0.,0.,  0.,0.,1.,
   	1.,1.,1.,  1.,0.,1.,  1.,0.,0.,  1.,1.,0.
    };

    static DtReal vtxnrms[24][3] = {
	/* we need to supply these to user-defined DoCompTextureUV/WCallbacks */
	/* bottom */	0.,0.,-1.,  0.,0.,-1.,  0.,0.,-1.,  0.,0.,-1.,

	/* top */	0.,0.,1.,   0.,0.,1.,   0.,0.,1.,  0.,0.,1.,

	/* back */	0.,1.,0.,   0.,1.,0.,   0.,1.,0.,  0.,1.,0.,

	/* front */	0.,-1.,0.,  0.,-1.,0.,  0.,-1.,0.,  0.,-1.,0.,

	/* left */	-1.,0.,0.,  -1.,0.,0.,  -1.,0.,0.,  -1.,0.,0.,

	/* right */	1.,0.,0.,   1.,0.,0.,   1.,0.,0.,  1.,0.,0.
    };

    if (!cmptexuvswi  && !cmptexuvwswi)  {
        quadlstobj = dor_quadlst_create(DcRGB,DcLoc,6,quadlstvtxs);
    } else {

        /*  Are texture uvs and/or uvws to be computed?  If so, determine whether
        to use the "callback" provided by Dore or a user-defined callback. */

        if (cmptexuvswi) {
            if (routine_uvclbk == (DtPFI)DcNullPtr) {
                  routine_uvclbk = DDclass_InqMethod(DsInqClassId
                              ("AttCompTextureUVCallback"),
                               DsInqMethodId("InqGlobalValue"));
            }
            (*routine_uvclbk) (&cmptexuvcbk);
	    if (cmptexuvcbk != DcStdUVCallback) {
                uv_function = dor_calbak_get_function(cmptexuvcbk);
                uv_dataobject = dor_calbak_get_dataobject(cmptexuvcbk);

	        /* call user-defined callback for every vertex in box */

	        for (vertex=0; vertex<24; vertex++) {
	    	    dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function, 
			    uv_dataobject, quadlstvtxs[vertex][0], 
			    quadlstvtxs[vertex][1],
			    quadlstvtxs[vertex][2], 
			    vtxnrms[vertex][0], vtxnrms[vertex][1],
			    vtxnrms[vertex][2],
			    &(uv[vertex][0]), &(uv[vertex][1]));
	        }
	    }
        }
        if (cmptexuvwswi) {
            if (routine_uvwclbk == (DtPFI)DcNullPtr) {
                  routine_uvwclbk = DDclass_InqMethod(DsInqClassId
                              ("AttCompTextureUVWCallback"),
                               DsInqMethodId("InqGlobalValue"));
            }
            (*routine_uvwclbk) (&cmptexuvwcbk);
	    if (cmptexuvwcbk != DcStdUVWCallback) {
                uvw_function = dor_calbak_get_function(cmptexuvwcbk);
                uvw_dataobject = dor_calbak_get_dataobject(cmptexuvwcbk);

	        /* call user-defined callback for every vertex in box */

	        for (vertex=0; vertex<24; vertex++) {
	    	    dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function, 
			    uvw_dataobject, quadlstvtxs[vertex][0], 
			    quadlstvtxs[vertex][1],
			    quadlstvtxs[vertex][2], 
			    vtxnrms[vertex][0], vtxnrms[vertex][1],
			    vtxnrms[vertex][2],
			    &(uvw[vertex][0]), &(uvw[vertex][1]), &(uvw[vertex][2]));
	        }
	    }
        }

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
        vertices = (DtReal *) dor_space_allocate(24*vertexsize*(sizeof(DtReal)));

	quadlst = vertices;

        for (vertex=0; vertex<24; vertex++) {
            *quadlst++ = quadlstvtxs[vertex][0];
            *quadlst++ = quadlstvtxs[vertex][1];
            *quadlst++ = quadlstvtxs[vertex][2];
            if (cmptexuvswi) {
                *quadlst++ = uv[vertex][0];
                *quadlst++ = uv[vertex][1];
            }
            if (cmptexuvwswi) {
                *quadlst++ = uvw[vertex][0];
                *quadlst++ = uvw[vertex][1];
                *quadlst++ = uvw[vertex][2];
            }
        }
    quadlstobj = dor_quadlst_create(DcRGB, vertextype, 6, vertices);
    }

    return(quadlstobj);
}

/*
 ======================================================================
 */

   /* A cylinder is subdivided into n quads and a top and bottom circular
      plate, each made of n "triangles". For each of thes triangles, the
      last vertex is duplicated and entered as a quad in the quad list 

      Cylindrical coordinates are used for parameterizing the 
	cylinder for 2D texture mapping :

      	For the cylindrical part:
		u = n * 1./divisions, n = 0,..,divisions
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


dot_object *ddr_vll_drom_prisrf_create_alternate_geometry_cylinder(
                    subdivtype, subdivlevel,cmptexuvswi,cmptexuvwswi)
DtInt subdivtype;
DtReal subdivlevel;
DtSwitch cmptexuvswi;
DtSwitch cmptexuvwswi;

{
    static DtInt space=0;
    static DtReal *vertices;
    DtReal *vptr;
    DtInt needspace;
    DtInt divisions;
    DtInt vertexsize;
    DtReal angleincrement;
    DtReal uincrement;
    DtReal radius;
    DtInt quadcount;
    DtInt vertex;
    DtReal x,y,z;
    DtReal nx,ny,nz;
    DtReal tempvtxs[4][11];
    DtReal dor_math_cos();
    DtReal dor_math_sin();
    DtReal lowx, lowy;
    DtReal highx, highy;
    DtReal lowu, highu;
    DtVertexType vertextype;
    dot_object *quadlstobj;
    dot_object *dor_quadlst_create();
    dot_object *cmptexuvcbk;
    dot_object *cmptexuvwcbk;
    dot_pfi uv_function;
    dot_pfi uvw_function;
    dot_pfi dor_calbak_get_function();
    dot_object *uv_dataobject;
    dot_object *uvw_dataobject;
    dot_object *dor_calbak_get_dataobject();
    static DtPFI routine_uvclbk = DcNullPtr;
    static DtPFI routine_uvwclbk = DcNullPtr;
    static DtReal not_quite_zero = 0.008; /*to patch z-accuracy hardware problem*/
    static DtReal not_quite_one  = 0.992;

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

    angleincrement = 2.* M_PI /divisions;
    uincrement = 1./divisions;
    radius = 1.;

    vptr = vertices;
    quadcount = 0;
 
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
   		*vptr++ = not_quite_one;
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
   		*vptr++ = not_quite_one;
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

   	*vptr++ = x = 0.;
   	*vptr++ = y = 0.;
   	*vptr++ = z = 0.;
   	*vptr++ = nx = 0.;
   	*vptr++ = ny = 0.;
   	*vptr++ = nz = -1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = 1. - ((vertex+.5)*uincrement);
   		*vptr++ = not_quite_one;
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

	/* duplicate last vertex to make a quad out of the triangle */

   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = -1;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = 1. - ((vertex+.5)*uincrement);
   		*vptr++ = not_quite_one;
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

	quadcount++;
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
   		*vptr++ = not_quite_zero;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = x/2.+ .5;
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
   		*vptr++ = not_quite_zero;
	    } else {
                dor_prisrf_compuv_usercallback(cmptexuvcbk, uv_function,
                        uv_dataobject, x,y,z, nx,ny,nz, vptr, (vptr+1));
                vptr += 2;
            }
	}
        if (cmptexuvwswi) {
	    if (cmptexuvwcbk == DcStdUVWCallback) {
            	*vptr++ = x/2.+ .5;
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
   		*vptr++ = not_quite_zero;
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

	/* duplicate last triangle vertex to make quad */

   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 1.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 1.;
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
   		*vptr++ = (vertex + .5) * uincrement;
   		*vptr++ = not_quite_zero;
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

	quadcount++;
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
                if (vertex == (divisions-1))
   		   tempvtxs[0][6] = not_quite_one;
                else
   		   tempvtxs[0][6] = highu;
   		tempvtxs[0][7] = not_quite_one;
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
                        uvw_dataobject, tempvtxs[0][0], tempvtxs[0][1], tempvtxs[0][2], 
			tempvtxs[0][3], tempvtxs[0][4], tempvtxs[0][5], 
			&(tempvtxs[0][8]), &(tempvtxs[0][9]), &(tempvtxs[0][10]));
            }
        }

   	tempvtxs[1][0] = highx;
   	tempvtxs[1][1] = highy;
   	tempvtxs[1][2] = 1.;
   	dor_math_normal(0.,0.,1.,tempvtxs[1][0],tempvtxs[1][1],tempvtxs[1][2],
   		&(tempvtxs[1][3]),&(tempvtxs[1][4]),&(tempvtxs[1][5]));
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
                if (vertex == (divisions-1))
   		   tempvtxs[1][6] = not_quite_one;
                else
   		   tempvtxs[1][6] = highu;
   		tempvtxs[1][7] = not_quite_zero;
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
                        uvw_dataobject, tempvtxs[1][0], tempvtxs[1][1], tempvtxs[1][2], 
			tempvtxs[1][3], tempvtxs[1][4], tempvtxs[1][5], 
			&(tempvtxs[1][8]), &(tempvtxs[1][9]), &(tempvtxs[1][10]));
            }
        }

   	tempvtxs[2][0] = lowx;
   	tempvtxs[2][1] = lowy;
   	tempvtxs[2][2] = 1.;
   	dor_math_normal(0.,0.,1.,tempvtxs[2][0],tempvtxs[2][1],tempvtxs[2][2],
   		&(tempvtxs[2][3]),&(tempvtxs[2][4]),&(tempvtxs[2][5]));
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
                if (vertex == 0)
   		   tempvtxs[2][6] = not_quite_zero;
                else
   		   tempvtxs[2][6] = lowu;
   		tempvtxs[2][7] = not_quite_zero;
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
                        uvw_dataobject, tempvtxs[2][0], tempvtxs[2][1], tempvtxs[2][2], 
			tempvtxs[2][3], tempvtxs[2][4], tempvtxs[2][5], 
			&(tempvtxs[2][8]), &(tempvtxs[2][9]), &(tempvtxs[2][10]));
	    }
        }

   	tempvtxs[3][0] = lowx;
   	tempvtxs[3][1] = lowy;
   	tempvtxs[3][2] = 0.;
   	dor_math_normal(0.,0.,0.,tempvtxs[3][0],tempvtxs[3][1],tempvtxs[3][2],
   		&(tempvtxs[3][3]),&(tempvtxs[3][4]),&(tempvtxs[3][5]));
	if (cmptexuvswi) {
	    if (cmptexuvcbk == DcStdUVCallback) {
                if (vertex == 0)
   		   tempvtxs[3][6] = not_quite_zero;
                else
   		   tempvtxs[3][6] = lowu;
   		tempvtxs[3][7] = not_quite_one;
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
            	tempvtxs[3][9] = 0.5 - lowy/2.;
            	tempvtxs[3][10] = 1.;
	    } else {
                dor_prisrf_compuvw_usercallback(cmptexuvwcbk, uvw_function,
                        uvw_dataobject, tempvtxs[3][0], tempvtxs[3][1], tempvtxs[3][2], 
			tempvtxs[3][3], tempvtxs[3][4], tempvtxs[3][5], 
			&(tempvtxs[3][8]), &(tempvtxs[3][9]), &(tempvtxs[3][10]));
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

	quadcount ++;
    }

    quadlstobj = dor_quadlst_create(DcRGB, vertextype, quadcount,vertices);

    return(quadlstobj);

}

/*
 ======================================================================
 */

