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
	DtPtr ddr_x11_XFDI_drom_prisrf_create (object)
	int ddr_x11_XFDI_drom_prisrf_print (object, data)
	int ddr_x11_XFDI_drom_prisrf_delete (object, data)
	int ddr_x11_XFDI_drom_prisrf_render (object)
	int ddr_x11_XFDI_drom_render_prisrf_box ()
	DtInt ddr_x11_XFDI_drom_render_prisrf_sphere (prisrf)
	dot_object * ddr_x11_XFDI_drom_render_prisrf_create_alternate (prisrf)
	dot_object * ddr_x11_XFDI_drom_render_prisrf_create_alternate_box (subdivtype, subdivlevel, genuv)
	dot_object * ddr_x11_XFDI_drom_render_prisrf_create_alternate_cone (subdivtype, subdivlevel,genuv)
	dot_object * ddr_x11_XFDI_drom_render_prisrf_create_alternate_cyl (subdivtype, subdivlevel,genuv)
	dot_object * ddr_x11_XFDI_drom_render_prisrf_create_alternate_sphere (subdivtype, subdivlevel, genuv)
	int ddr_x11_XFDI_drom_sphere_triangles (cnt,octsize,gridsize,v0,v1,v2)
	DtReal ddr_x11_XFDI_drom_render_sphere_ucomp (y,x)
	DtReal ddr_x11_XFDI_drom_render_sphere_vcomp (z)

  ======================================================================
 */
#include "../drom.h"
#include "dore/dore_develop/private/prisrf.h"
#include <math.h>

typedef struct {
	DtObjectStructure *alt_obj;
	DtInt subdivtype;
	DtReal subdivlevel[1];
} ddt_x11_XFDI_drom_prisrf_data;
/*
 ======================================================================
 */
DtPtr
ddr_x11_XFDI_drom_prisrf_create(object)
     DtObject object;
{
    static DtPFI subdivspc_query;
    ddt_x11_XFDI_drom_prisrf_data *data;

    if ((data = (ddt_x11_XFDI_drom_prisrf_data *)
	 DDspace_Allocate(sizeof(ddt_x11_XFDI_drom_prisrf_data)))
	== (ddt_x11_XFDI_drom_prisrf_data *)0) {
	DDerror (ERR_CANT_ALLOC_MEM_SIZE,
		    "ddr_x11_XFDI_drom_prisrf_create",
		    "sizeof (ddt_x11_XFDI_drom_prisrf_data)");
	return DcNullPtr;
    }

    data->alt_obj = DcNullPtr;

    if (subdivspc_query == (DtPFI)DcNullPtr)
	    subdivspc_query =  DDclass_InqMethod(DsInqClassId("AttSubDivSpec"), 
						  DcMethodInqGlbAttVal);
    (*subdivspc_query)(&data->subdivtype, data->subdivlevel);

    return (DtPtr)data;
}
/*
 ======================================================================
 */

ddr_x11_XFDI_drom_prisrf_print(object, data)
     DtObject object;
     ddt_x11_XFDI_drom_prisrf_data *data;
{
}
/*
 ======================================================================
 */

ddr_x11_XFDI_drom_prisrf_delete(object, data)
     DtObject object;
     ddt_x11_XFDI_drom_prisrf_data *data;
{
    if (data->alt_obj != NULL)
	    DeDeleteObject(data->alt_obj);
    DDspace_Deallocate(data);
}

     
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_prisrf_render(object)
dot_object *object;

{
    struct prisrf *prisrf;
    static DtInt dataindex = -1;
    static DtInt classid = -1;
    static DtPFI subdivspc_query;
    DtReal subdivlevel[1];
    DtInt subdivtype;
    DtFlag changed;
    DtObjectStructure *ddr_x11_XFDI_drom_render_prisrf_create_alternate();
    ddt_x11_XFDI_drom_prisrf_data *data;
    DtInt i;

    classid = DsInqClassId("DoPrimSurf");

    if (!DDcondex_QueryRender(classid))
	    return;

    prisrf = (struct prisrf *)(object->data);

    switch(prisrf->surface_type) {
    case DcBox:
        ddr_x11_XFDI_drom_render_prisrf_box();
	break;

    case DcSphere:
	if (dde_x11_XFDI_drom_curdevdat->use_spheres) {
	    if (ddr_x11_XFDI_drom_render_prisrf_sphere(prisrf)) {
		/* this routine will return 1 if it did the sphere otherwise
		 * use the alternate object
		 */
		break;
	    }
	} 
	/* if the device does not want to use hardware spheres then
	 * fall through to the default
	 */
    case DcCylinder:
    case DcCone:

   	if (dataindex == -1) {
	    for (i=0; dde_x11_XFDI_drom_class_data[i].name != 0; i++) {
		if (dde_x11_XFDI_drom_class_data[i].id == classid) {
		    dataindex = dde_x11_XFDI_drom_class_data[i].index;
		    break;
		}
	    }
   	}

	/* has subdivision spec changed ? */

	if (subdivspc_query == (DtPFI)DcNullPtr)
		subdivspc_query =  DDclass_InqMethod (DsInqClassId("AttSubDivSpec"),
						       DcMethodInqGlbAttVal);
  	(*subdivspc_query)(&subdivtype, subdivlevel);

	if (prisrf->subdivtype != subdivtype) {
	    prisrf->subdivtype = subdivtype;
	    changed = DcTrue;
	}

	if (prisrf->subdivparms[0] != subdivlevel[0]) {
	    prisrf->subdivparms[0] = subdivlevel[0];
	    changed = DcTrue;
	}

	data = (ddt_x11_XFDI_drom_prisrf_data *)
		object->additional_data[dataindex];
   	if ((data->alt_obj != dod_object_null) && !changed) {
	    DDobject_InqCurrentMethod(data->alt_obj) (data->alt_obj);

	    return;

   	} else {
	    if (data->alt_obj != dod_object_null) {
		DDobject_InqMethod(data->alt_obj, DcMethodDestroy)
			(data->alt_obj);
		data->alt_obj = dod_object_null;
	    }

	    data->alt_obj =
		    ddr_x11_XFDI_drom_render_prisrf_create_alternate(prisrf);

	    if (data->alt_obj == dod_object_null) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "ddr_x11_XFDI_drom_prisrf_render", "");
		return;
	    }

	    DDobject_InqCurrentMethod(data->alt_obj) (data->alt_obj);

	    return;
   	}
	break;

    default:
	sprintf(DDerror_GetLine(),"Surface type %d",prisrf->surface_type);
        DDerror (ERR_VALUE_OUT_OF_RANGE,
                    "ddr_x11_XFDI_drom_prisrf_render",
		    DDerror_GetLine());
        break;

    }
}
/*
 ======================================================================
 */

static DtReal point_verts[] = {
   0.,0.,0., 1.,0.,0., 1.,1.,0., 0.,1.,0.,
   0.,1.,1., 1.,1.,1., 1.,0.,1., 0.,0.,1.,
};

static DtReal line_verts[] = {
   0.,0.,0., 1.,0.,0.,   1.,0.,0., 1.,1.,0.,
   1.,1.,0., 0.,1.,0.,   0.,1.,0., 0.,0.,0.,
   0.,0.,0., 0.,0.,1.,   1.,0.,0., 1.,0.,1.,
   1.,1.,0., 1.,1.,1.,   0.,1.,0., 0.,1.,1.,
   0.,0.,1., 1.,0.,1.,   1.,0.,1., 1.,1.,1.,
   1.,1.,1., 0.,1.,1.,   0.,1.,1., 0.,0.,1.,
};

static DtReal quad_verts[] = {
   0.,0.,0.,   0.,1.,0.,   0.,1.,1.,   0.,0.,1.,
   0.,1.,0.,   1.,1.,0.,   1.,1.,1.,   0.,1.,1.,
   0.,1.,1.,   1.,1.,1.,   1.,0.,1.,   0.,0.,1.,
   1.,0.,1.,   1.,0.,0.,   0.,0.,0.,   0.,0.,1.,
   1.,0.,1.,   1.,1.,1.,   1.,1.,0.,   1.,0.,0.,
   0.,0.,0.,   1.,0.,0.,   1.,1.,0.,   0.,1.,0.,
};

static DtReal quad_norms[] = {
   -1., 0.,0.,   0.,1.,0.,   0.,0., 1.,
    0.,-1.,0.,   1.,0.,0.,   0.,0.,-1.,
};

static DtReal tristrip_verts[] = {
   1.,0.,1.,   1.,0.,1.,   1.,1.,1.,   0.,0.,1.,   0.,1.,1.,   0.,1.,1.,
   0.,1.,1.,   0.,0.,1.,   0.,1.,0.,   0.,0.,0.,   0.,0.,0.,   0.,0.,0.,
   0.,1.,0.,   1.,0.,0.,   1.,1.,0.,   1.,1.,0.,   1.,1.,0.,   1.,0.,0.,
   1.,1.,1.,   1.,0.,1.,   1.,0.,1.,   1.,0.,0.,   0.,0.,1.,   0.,0.,0.,
   0.,0.,0.,   0.,1.,0.,   0.,1.,0.,   1.,1.,0.,   0.,1.,1.,   1.,1.,1.,
};

static DtReal tristrip_norms[] = {
  0., 0., 1.,  0.,0., 1.,  0., 0., 1.,  0., 0., 1.,  0., 0., 1.,  0., 0., 1.,
 -1., 0., 0., -1.,0., 0., -1., 0., 0., -1., 0., 0., -1., 0., 0.,  0., 0.,-1.,
  0., 0.,-1.,  0.,0.,-1.,  0., 0.,-1.,  0., 0.,-1.,  1., 0., 0.,  1., 0., 0.,
  1., 0., 0.,  1.,0., 0.,  0.,-1., 0.,  0.,-1., 0.,  0.,-1., 0.,  0.,-1., 0.,
  0.,-1., 0.,  0.,1., 0.,  0., 1., 0.,  0., 1., 0.,  0., 1., 0.,  0., 1., 0.,
};

static DtReal tristrip_uv[] = {
   1.,0.,   1.,0.,   1.,1.,   0.,0.,   0.,1.,   0.,1.,
   0.,1.,   1.,1.,   0.,0.,   1.,0.,   1.,0.,   0.,1.,
   0.,0.,   1.,1.,   1.,0.,   1.,0.,   1.,0.,   0.,0.,
   1.,1.,   0.,1.,   1.,1.,   1.,0.,   0.,1.,   0.,0.,
   0.,0.,   0.,1.,   0.,1.,   1.,1.,   0.,0.,   1.,0.,
};

static DtReal *tristrip_uvs[] = {
   tristrip_uv,
};

ddr_x11_XFDI_drom_render_prisrf_box()
{
    static DtPFI genuv_query;
    DtSwitch genuv;
    DtInt uv_count;

    if (dde_x11_XFDI_drom_data.reptype == DcPoints) {
	XFDIPolyMarker(dde_x11_XFDI_drom_curdevdat->display,
		       dde_x11_XFDI_drom_curdevdat->gc,
		       point_verts,8);
    } else if (dde_x11_XFDI_drom_data.reptype == DcWireframe ||
	       dde_x11_XFDI_drom_data.reptype == DcOutlines) {
	XFDIDisjointPolyLine(dde_x11_XFDI_drom_curdevdat->display,
			     dde_x11_XFDI_drom_curdevdat->gc,
			     line_verts,24);
    } else {
	if (genuv_query == (DtPFI)DcNullPtr) 
		genuv_query =  DDclass_InqMethod (DsInqClassId("AttGenerateTextureUV"),
						   DcMethodInqGlbAttVal);
	(*genuv_query)(&genuv);

	uv_count  = genuv  ? 1 : 0;

	ddr_x11_XFDI_drom_output_tristrip(point_verts, 
					   30,
					   tristrip_verts,
					   tristrip_norms,
					   (unsigned long *)0,
					   uv_count,  tristrip_uvs, 
					   0, (DtRealTriple **)0);
    }
}
/*
 ======================================================================
 */

DtInt
ddr_x11_XFDI_drom_render_prisrf_sphere(prisrf)
struct prisrf *prisrf;
{
    DtReal center[1][3];
    DtReal radius[1];
    unsigned long color[1];
    static DtPFI gentexuv_query = (DtPFI)DcNullPtr;
    DtSwitch switchvalue;

    if (gentexuv_query == (DtPFI)DcNullPtr)
	    gentexuv_query =  DDclass_InqMethod (DsInqClassId("AttGenerateTextureUV"),
						  DcMethodInqGlbAttVal);
    (*gentexuv_query)(&switchvalue);

    /* 
     * The user has specified that this object should be able to be texture mapped
     * so use the tesselated spheres not the hardware sphere
     */
    if (switchvalue && 
	(dde_x11_XFDI_drom_data.texture_3D_valid ||
	 dde_x11_XFDI_drom_data.texture_2D_valid) &&
	dde_x11_XFDI_drom_data.map_diffuse_color)
	    return 0;

    /*
     * The representation type is not a surface so use the tesselated spheres
     */
    if ((dde_x11_XFDI_drom_data.reptype != DcSurface) &&
	(dde_x11_XFDI_drom_data.reptype != DcSurfaceWireframeEdges) &&
	(dde_x11_XFDI_drom_data.reptype != DcSurfaceOutlineEdges))
	    return 0;

    if (!dde_x11_XFDI_drom_data.spheres_required &&
	!dde_x11_XFDI_drom_curviwdat->have_spheres) {
	/*
	 * This is the first sphere seen and didn't think there were any spheres
	 * so abort the traversal and indicate that we have spheres
	 */
	dde_x11_XFDI_drom_data.spheres_required = DcTrue;
	DsExecutionAbort();
	return 1;
    }

    dde_x11_XFDI_drom_data.spheres_required = DcTrue;
    center[0][0] = 0;
    center[0][1] = 0;
    center[0][2] = 0;
    radius[0] = 1;
    color[0] = XFDIQuerySurfaceColor(dde_x11_XFDI_drom_curdevdat->display,
				     dde_x11_XFDI_drom_curdevdat->gc);

    XFDIPolysphereColors(dde_x11_XFDI_drom_curdevdat->display,
			 dde_x11_XFDI_drom_curdevdat->gc,
			 center, radius, color, 1);

    return 1;
}
/*
 ======================================================================
 */

 dot_object * ddr_x11_XFDI_drom_render_prisrf_create_alternate(prisrf)
	struct prisrf *prisrf;
{
    static DtPFI subdivspc_query;
    static DtPFI genuv_query;
    DtReal subdivlevel[1];
    DtInt subdivtype;
    DtSwitch genuv;
    dot_object *ddr_x11_XFDI_drom_render_prisrf_create_alternate_sphere();
    dot_object *ddr_x11_XFDI_drom_render_prisrf_create_alternate_cyl();
    dot_object *ddr_x11_XFDI_drom_render_prisrf_create_alternate_box();
    dot_object *ddr_x11_XFDI_drom_render_prisrf_create_alternate_cone();

    if (subdivspc_query == (DtPFI)DcNullPtr) 
	    subdivspc_query =  DDclass_InqMethod (DsInqClassId("AttSubDivSpec"),
						   DcMethodInqGlbAttVal);
    (*subdivspc_query)(&subdivtype, subdivlevel);

    if (subdivtype != DcSubDivSegments) {
	/* old style subdivision spec */
	subdivlevel[0] = max(subdivlevel[0],0.);
	subdivlevel[0] = min(subdivlevel[0],1.);
    } else {
	subdivlevel[0] = max(subdivlevel[0],4.);
    }

    if (genuv_query == (DtPFI)DcNullPtr) 
	    genuv_query =  DDclass_InqMethod (DsInqClassId("AttGenerateTextureUV"),
					       DcMethodInqGlbAttVal);
    (*genuv_query)(&genuv);

    switch(prisrf->surface_type){
    case DcSphere:
	return(ddr_x11_XFDI_drom_render_prisrf_create_alternate_sphere
	       (subdivtype, subdivlevel[0],genuv));
	break;

    case DcCylinder:
	return(ddr_x11_XFDI_drom_render_prisrf_create_alternate_cyl
	       (subdivtype, subdivlevel[0],genuv));
	break;

    case DcBox:
	return(ddr_x11_XFDI_drom_render_prisrf_create_alternate_box
	       (subdivtype, subdivlevel[0],genuv));
	break;

    case DcCone:
	return(ddr_x11_XFDI_drom_render_prisrf_create_alternate_cone
	       (subdivtype, subdivlevel[0],genuv));
	break;

    default:
	break;
    }
  
    return(dod_object_null);
}
/*
 ======================================================================
 */

dot_object *ddr_x11_XFDI_drom_render_prisrf_create_alternate_box(subdivtype, 
								 subdivlevel, genuv)
     DtInt subdivtype;
     DtReal subdivlevel;
     DtSwitch genuv;

{

    static DtReal trilstvtxs_withuv[36][5] = { 
	/* comments assume z up, and y going in*/
	/*bottom*/
	0.,0.,0.,0.,1.,   0.,1.,0.,0.,0.,   1.,1.,0.,1.,0., 
        0.,0.,0.,0.,1.,   1.,1.,0.,1.,0.,   1.,0.,0.,1.,1.,

	/*top*/
        0.,0.,1.,0.,0.,   1.,0.,1.,1.,0.,   1.,1.,1.,1.,1.,
        0.,0.,1.,0.,0.,   1.,1.,1.,1.,1.,   0.,1.,1.,0.,1.,

	/*back*/
        0.,1.,0.,0.,1.,  0.,1.,1.,0.,0.,   1.,1.,1.,1.,0.,
        0.,1.,0.,0.,1.,  1.,1.,1.,1.,0.,   1.,1.,0.,1.,1.,

	/*front*/
        0.,0.,0.,0.,0.,   1.,0.,0.,1.,0.,  1.,0.,1.,1.,1.,
        0.,0.,0.,0.,0.,   1.,0.,1.,1.,1.,  0.,0.,1.,0.,1.,

	/*left*/
        0.,0.,0.,1.,0.,   0.,0.,1.,1.,1.,   0.,1.,1.,0.,1.,
        0.,0.,0.,1.,0.,   0.,1.,1.,0.,1.,  0.,1.,0.,0.,0.,

	/*right*/
        1.,0.,0.,0.,0.,  1.,1.,0.,1.,0.,   1.,1.,1.,1.,1.,
        1.,0.,0.,0.,0.,  1.,1.,1.,1.,1.,   1.,0.,1.,0.,1.
	    };


    static DtReal strip1[] = {
	0.,0.,1.,
	0.,0.,0.,
	0.,1.,1.,
	0.,1.,0.,
	1.,1.,1.,
	1.,1.,0.,
	1.,0.,1.,
	1.,0.,0.,
	0.,0.,1.,
	0.,0.,0.,
    };

    static DtReal strip2[] = {
	0.,0.,0.,
	0.,1.,0.,
	1.,0.,0.,
	1.,1.,0
	    };

    static DtReal strip3[] = {
	0.,0.,1.,
	0.,1.,1.,
	1.,0.,1.,
	1.,1.,1
	    };
	

    /* can't get image on each side with this tristrip approach - each
       side will have to be one tristrip */
    static DtReal strip1_withuv[] = {
	0.,0.,1.,
	0.,0.,0.,
	0.,1.,1.,
	0.,1.,0.,
	1.,1.,1.,
	1.,1.,0.,
	1.,0.,1.,
	1.,0.,0.,
	0.,0.,1.,
	0.,0.,0.,
    };

    static DtReal strip2_withuv[] = {
	0.,0.,0.,
	0.,1.,0.,
	1.,0.,0.,
	1.,1.,0
	    };

    static DtReal strip3_withuv[] = {
	0.,0.,1.,
	0.,1.,1.,
	1.,0.,1.,
	1.,1.,1
	    };
	

    DtObject tristripobj;

    /*
      if (genuv == DcOn)
      
      tristripobj = DoGroup(DcTrue)
      DgAddObj(DoTriangleStrip(DcRGB, DcLoc|DsTextureUVCount(1), 
      10, strip1_withuv);
      DgAddObj(DoTriangleStrip(DcRGB, DcLoc|DsTextureUVCount(1),
      4, strip2_withuv);
      DgAddObj(DoTriangleStrip(DcRGB, DcLoc|DsTextureUVCount(1), 
      4, strip3_withuv);
      return((dot_object *)tristripobj);
   else {
*/
   	tristripobj = DoGroup(DcTrue);
		DgAddObj(DoTriangleStrip(DcRGB, DcLoc, 10, strip1));
		DgAddObj(DoTriangleStrip(DcRGB, DcLoc, 4, strip2));
		DgAddObj(DoTriangleStrip(DcRGB, DcLoc, 4, strip3));
   	return((dot_object *)tristripobj);
/*
   }
*/
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
	     v = (y+1.)/2.

	For the bottom circular plate, the bottom edge values of the 
	conic part are extended to the center of the plate.
*/


dot_object *ddr_x11_XFDI_drom_render_prisrf_create_alternate_cone(subdivtype,
								  subdivlevel,genuv)
DtInt subdivtype;
DtReal subdivlevel;
DtSwitch genuv;

{
    static DtInt space = 0;
    static DtReal *vertices;
    DtReal *vptr;
    DtInt needspace;
    DtInt vtxsize;
    DtInt divisions;
    DtReal angleincrement;
    DtReal radius;
    DtInt tricount;
    DtInt vertex;
    DtReal tmp[6];
    DtReal dor_math_cos();
    DtReal dor_math_sin();
    DtObject trilstobj;

    if (subdivtype != DcSubDivSegments) 
	    /* old style subdivision spec */
	    divisions = 5 + (subdivlevel*20.);
    else
	    divisions = subdivlevel;

    if (genuv == DcOn)
	    vtxsize = 8;
    else 
	    vtxsize = 6;

    needspace = 6*divisions*vtxsize;
    if (needspace > space) {
        if (space != 0)
                DDspace_Deallocate(vertices);
        vertices = (DtReal *)DDspace_Allocate(needspace*(sizeof (DtReal)));
        space = needspace;
    }

    angleincrement = 2.*3.14159/divisions;
    radius = 1.;

    vptr = vertices;
    tricount = 0;

    /*  Make the back circular plate, notice the backwards angular
	direction, used to get the normal correct.  */

    for (vertex=0;vertex<divisions;vertex++) {
	tmp[0] = radius * dor_math_cos(-vertex*angleincrement);
   	tmp[1] = radius * dor_math_sin(-vertex*angleincrement);
   	*vptr++ = tmp[0];
   	*vptr++ = tmp[1];
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = -1.;
	if (genuv == DcOn) {
	    *vptr++ = (tmp[0] + 1.)/2.;
	    *vptr++ = (tmp[1] + 1.)/2.;
	}

   	tmp[0] = radius * dor_math_cos(-(vertex+1)*angleincrement);
   	tmp[1] = radius * dor_math_sin(-(vertex+1)*angleincrement);
   	*vptr++ = tmp[0];
   	*vptr++ = tmp[1];
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = -1.;
	if (genuv == DcOn) {
	    *vptr++ = (tmp[0] + 1.)/2.;
	    *vptr++ = (tmp[1] + 1.)/2.;
	}

   	tmp[0] = radius * dor_math_cos(-(vertex+.5)*angleincrement);
   	tmp[1] = radius * dor_math_sin(-(vertex+.5)*angleincrement);
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = -1.;
	if (genuv == DcOn) {
	    *vptr++ = (tmp[0] + 1.)/2.;
	    *vptr++ = (tmp[1] + 1.)/2.;
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
   	*vptr++ = 0.;
	*vptr++ = tmp[3];
	*vptr++ = tmp[4];
	*vptr++ = tmp[5];
	if (genuv == DcOn) {
	    *vptr++ = (tmp[0]+1.)/2.;
	    *vptr++ = (tmp[1]+1.)/2.;
	}

   	tmp[0] = radius * dor_math_cos((vertex+1)*angleincrement);
   	tmp[1] = radius * dor_math_sin((vertex+1)*angleincrement);
   	dor_math_normal(0., 0., 0., tmp[0], tmp[1], 1.,
			&tmp[3], &tmp[4], &tmp[5]);
	*vptr++ = tmp[0];
	*vptr++ = tmp[1];
   	*vptr++ = 0.;
	*vptr++ = tmp[3];
	*vptr++ = tmp[4];
	*vptr++ = tmp[5];
	if (genuv == DcOn) {
	    *vptr++ = (tmp[0]+1.)/2.;
	    *vptr++ = (tmp[1]+1.)/2.;
	}

   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 1.;
   	tmp[0] = radius * dor_math_cos((vertex+0.5)*angleincrement);
   	tmp[1] = radius * dor_math_sin((vertex+0.5)*angleincrement);
   	dor_math_normal(0., 0., 0., tmp[0], tmp[1], 1.,
			&tmp[3], &tmp[4], &tmp[5]);
	*vptr++ = tmp[3];
	*vptr++ = tmp[4];
	*vptr++ = tmp[5];
	if (genuv == DcOn) {
	    *vptr++ = .5;
	    *vptr++ = .5;
	}

   	tricount++;
   	
    }

    if (genuv) 
	    trilstobj = DoTriangleList(DcRGB, DcLocNrm|DsTextureUVCount(1),
				       tricount,vertices);
    else
	    trilstobj = DoTriangleList(DcRGB,DcLocNrm,tricount,vertices);

    return((dot_object *)trilstobj);
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
		v = z  (note the z coordinate for the cylinder 
			is always between 0 and 1)

      	For the top circular plate:
		The top edge values are extended to the center point
		
      	For the bottom circular plate:
		The bottom edge values are extended to the center point
		
   */


dot_object *ddr_x11_XFDI_drom_render_prisrf_create_alternate_cyl(subdivtype,
								 subdivlevel,genuv)
     DtInt subdivtype;
     DtReal subdivlevel;
     DtSwitch genuv;

{
    static DtInt space=0;
    static DtReal *cylverts;
    static DtReal *cap1verts;
    static DtReal *cap2verts;
    DtReal *vptr;
    DtInt cylspace;
    DtInt capspace;
    DtInt vtxsize;
    DtInt divisions;
    DtReal angleincrement;
    DtReal uincrement;
    DtReal radius;
    DtInt tricount;
    DtInt vertex;
    DtReal tmpvtx[3];
    DtInt cylvertcount;
    DtInt capvertcount;
    DtInt odd;
    DtReal x, y, z1, z2;
    DtReal u;
    DtReal dor_math_cos();
    DtReal dor_math_sin();
    DtObject tristripobj;

    if (subdivtype != DcSubDivSegments)
	    /* old style subdiv spec */
	    divisions = 5 + (subdivlevel*20.);
    else
	    divisions = subdivlevel;

    if (genuv == DcOn)
	    vtxsize = 8;
    else
	    vtxsize = 6;

    cylspace = (4*divisions)*vtxsize;
    capspace = (4*divisions)*vtxsize;
    if (cylspace > space) {
	if (space != 0) {
	    DDspace_Deallocate(cylverts);
	    DDspace_Deallocate(cap1verts);
	    DDspace_Deallocate(cap2verts);
	}
	cylverts = (DtReal *)DDspace_Allocate(cylspace*(sizeof (DtReal)));
	cap1verts = (DtReal *)DDspace_Allocate(capspace*(sizeof (DtReal)));
	cap2verts = (DtReal *)DDspace_Allocate(capspace*(sizeof (DtReal)));
	space = cylspace;
    }

    /*  Make the back circular plate, notice the backwards angular
	direction, used to get the normal correct.  */

    angleincrement = 2.*3.14159/divisions;
    uincrement = 1./divisions;
    radius = 1.;

    vptr = cap1verts;
    capvertcount = 0;

    for (vertex=0;vertex<divisions;vertex++) {
	if (vertex != 0) {
	    /* 
	     * add an additional degenerate triangle to get the order 
	     * right for backface culling
	     */
	    *vptr++ = radius * dor_math_cos(-vertex*angleincrement);
	    *vptr++ = radius * dor_math_sin(-vertex*angleincrement);
	    *vptr++ = 0.;
	    *vptr++ = 0.;
	    *vptr++ = 0.;
	    *vptr++ = -1.;
	    if (genuv == DcOn) {
		*vptr++ = -vertex * uincrement;
		*vptr++ = 0.;
	    }
	    capvertcount+=1;
	}

   	*vptr++ = radius * dor_math_cos(-vertex*angleincrement);
   	*vptr++ = radius * dor_math_sin(-vertex*angleincrement);
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = -1.;
	if (genuv == DcOn) {
	    *vptr++ = -vertex * uincrement;
	    *vptr++ = 0.;
	}

   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = -1.;
	if (genuv == DcOn) {
	    *vptr++ = -(vertex+.5) * uincrement;
	    *vptr++ = 0.;
	} 

   	*vptr++ = radius * dor_math_cos(-(vertex+1)*angleincrement);
   	*vptr++ = radius * dor_math_sin(-(vertex+1)*angleincrement);
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = -1.;
	if (genuv == DcOn) {
	    *vptr++ = -(vertex+1) * uincrement;
	    *vptr++ = 0.;
	}
 
	capvertcount+=3;
    }

    /*  Add the front circular plate to the polygonal object definition.
	Notice the frontwards angular direction, used to get the normal
	correct.  */

    vptr = cap2verts;

    for (vertex=0;vertex<divisions;vertex++) {
	if (vertex != 0) {
	    /* 
	     * add an additional degenerate triangle to get the order 
	     * right for backface culling
	     */
	    *vptr++ = radius * dor_math_cos(vertex*angleincrement);
	    *vptr++ = radius * dor_math_sin(vertex*angleincrement);
	    *vptr++ = 1.;
	    *vptr++ = 0.;
	    *vptr++ = 0.;
	    *vptr++ = -1.;
	    if (genuv == DcOn) {
		*vptr++ = -vertex * uincrement;
		*vptr++ = 0.;
	    }
	}

   	*vptr++ = radius * dor_math_cos(vertex*angleincrement);
   	*vptr++ = radius * dor_math_sin(vertex*angleincrement);
   	*vptr++ = 1.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 1.;
	if (genuv == DcOn) {
	    *vptr++ = vertex * uincrement;
	    *vptr++ = 1.;
	}

   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 1.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 1.;
	if (genuv == DcOn) {
	    *vptr++ = (vertex + .5) * uincrement;
	    *vptr++ = 1.;
	}

   	*vptr++ = radius * dor_math_cos((vertex+1)*angleincrement);
   	*vptr++ = radius * dor_math_sin((vertex+1)*angleincrement);
   	*vptr++ = 1.;
   	*vptr++ = 0.;
   	*vptr++ = 0.;
   	*vptr++ = 1.;
	if (genuv == DcOn) {
	    *vptr++ = (vertex+1) * uincrement;
	    *vptr++ = 1.;
	}
    }

    /*  Add the side polygons, keeping the normals straight.  */

    vptr = cylverts;
    cylvertcount = 0;

    for (vertex=0;vertex<divisions+1;vertex++) {
   	x = radius * dor_math_cos(vertex*angleincrement);
   	y = radius * dor_math_sin(vertex*angleincrement);
   	u = vertex*uincrement;

   	*vptr++ = x;
   	*vptr++ = y;
   	*vptr++ = 0.;
   	dor_math_normal(0.,0.,0., x, y, 0.,
   			&(tmpvtx[0]),&(tmpvtx[1]),&(tmpvtx[2]));
        
   	*vptr++ = tmpvtx[0];
   	*vptr++ = tmpvtx[1];
   	*vptr++ = tmpvtx[2];
	if (genuv == DcOn) {
	    *vptr++ = u;
	    *vptr++ = 0.;
	}

   	*vptr++ = x;
   	*vptr++ = y;
   	*vptr++ = 1.;
   	dor_math_normal(0.,0.,1., x, y, 1.,
   			&(tmpvtx[0]),&(tmpvtx[1]),&(tmpvtx[2]));
        
   	*vptr++ = tmpvtx[0];
   	*vptr++ = tmpvtx[1];
   	*vptr++ = tmpvtx[2];
	if (genuv == DcOn) {
	    *vptr++ = u;
	    *vptr++ = 1.;
	}

	cylvertcount +=2;
    }


    tristripobj = DoGroup(DcTrue);

    if (genuv == DcOn) {
	DgAddObj(DoTriangleStrip(DcRGB, DcLocNrm|DsTextureUVCount(1),
				 cylvertcount, cylverts));
	DgAddObj(DoTriangleStrip(DcRGB, DcLocNrm|DsTextureUVCount(1),
				 capvertcount, cap1verts));
	DgAddObj(DoTriangleStrip(DcRGB, DcLocNrm|DsTextureUVCount(1),
				 capvertcount, cap2verts));
    } else {
	DgAddObj(DoTriangleStrip(DcRGB, DcLocNrm, cylvertcount, cylverts));
	DgAddObj(DoTriangleStrip(DcRGB, DcLocNrm, capvertcount, cap1verts));
	DgAddObj(DoTriangleStrip(DcRGB, DcLocNrm, capvertcount, cap2verts));
    }

    return((dot_object *)tristripobj);

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
	v = 1. - ---------
	      	     PI

 */

#define TWOPI		(M_PI * 2.)

static DtIntTriple *tris;

dot_object *ddr_x11_XFDI_drom_render_prisrf_create_alternate_sphere(subdivtype,
						    subdivlevel, genuv)
     DtInt subdivtype;
     DtReal subdivlevel;
     DtSwitch genuv;
{

    static DtInt space = 0;
    static DtReal *sphvertices;
    static DtNPoint3 **grid;	/* grid of vertices for one octant */

    DtReal *vptr;
    DtInt row;
    DtInt octsize;
    DtInt octcnt;
    DtInt vtx;
    DtInt vtxsize;
    DtInt gridsize;
    DtInt needspace;
    DtUInt i, j;
    DtInt divs;			/* should be a DtUInt but for a sun compiler bug */
    DtReal incr;
    DtReal dor_math_cos(), dor_math_sin();
    DtReal ddr_x11_XFDI_drom_render_sphere_ucomp(), 
    ddr_x11_XFDI_drom_render_sphere_vcomp();
    DtPtr DDspace_Allocate();
    static DtInt finest=0;
    static DtNPoint3 norm0, norm1;
    DtObject trimshobj;

    if (subdivtype != DcSubDivSegments) 
	    /* old style subdivsion spec */
	    divs = 2 + (subdivlevel*10.);
    else
	    divs = subdivlevel/4.;

    row = divs+1;
    gridsize = row*row;

    if (genuv == DcOn)
	    vtxsize = 8;
    else
	    vtxsize = 6;

    needspace = gridsize*8*vtxsize;
    if (needspace > space) {
	if (space != 0) {
	    dor_space_deallocate(tris);
	    dor_space_deallocate(sphvertices);
	}
        tris = (DtIntTriple *)dor_space_allocate
		(8*divs*divs*(sizeof (DtIntTriple)));
        sphvertices = (DtReal *)dor_space_allocate
		(needspace*(sizeof (DtReal)));
        space = needspace;
    }

    if(divs > finest)		/* need more space */ {
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
    for(i=1, incr=M_PI_2/divs; i<=divs/2; i++) {
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
    for(i=2; i<divs; i++) {
	incr = M_PI_2 / i;
	/* get normal of plane 0 */
	dor_math_crossproduct(
			      grid[i][0].x, grid[i][0].y, grid[i][0].z,
			      grid[i][i].x, grid[i][i].y, grid[i][i].z,
			      &norm0.x, &norm0.y, &norm0.z);

	/* compute points along a great circle within great triangle */
	for(j=1; j<=i/2; j++) {
	    dor_math_crossproduct(
				  dor_math_cos(j * incr), /* pt on equator */
				  dor_math_sin(j * incr),
				  0.0,
				  0.0, 0.0, 1.0, /* north pole */
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

	    if(j != i)		/* not end of row */ {
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

	    vptr = sphvertices + vtx*vtxsize;
	    *vptr++ = grid[i][j].x;
	    *vptr++ = grid[i][j].y;
	    *vptr++ = grid[i][j].z;
	    *vptr++ = grid[i][j].x;
	    *vptr++ = grid[i][j].y;
	    *vptr++ = grid[i][j].z;
	    if (genuv == DcOn) {
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_ucomp(grid[i][j].y, grid[i][j].x);
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_vcomp(grid[i][j].z);
	    }
		

	    vptr = sphvertices + (gridsize+vtx)*vtxsize;
	    *vptr++ = -grid[i][j].x;
	    *vptr++ = grid[i][j].y;
	    *vptr++ = grid[i][j].z;
	    *vptr++ = -grid[i][j].x;
	    *vptr++ = grid[i][j].y;
	    *vptr++ = grid[i][j].z;
	    if (genuv == DcOn) {
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_ucomp(grid[i][j].y, -grid[i][j].x);
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_vcomp(grid[i][j].z);
	    }

	    vptr = sphvertices + (2*gridsize+vtx)*vtxsize;
	    *vptr++ = grid[i][j].x;
	    *vptr++ = -grid[i][j].y;
	    *vptr++ = grid[i][j].z;
	    *vptr++ = grid[i][j].x;
	    *vptr++ = -grid[i][j].y;
	    *vptr++ = grid[i][j].z;
	    if (genuv == DcOn) {
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_ucomp(-grid[i][j].y, grid[i][j].x);
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_vcomp(grid[i][j].z);
	    }
 
	    vptr = sphvertices + (3*gridsize+vtx)*vtxsize;
	    *vptr++ = grid[i][j].x;
	    *vptr++ = grid[i][j].y;
	    *vptr++ = -grid[i][j].z;
	    *vptr++ = grid[i][j].x;
	    *vptr++ = grid[i][j].y;
	    *vptr++ = -grid[i][j].z;
	    if (genuv == DcOn) {
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_ucomp(grid[i][j].y, grid[i][j].x);
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_vcomp(-grid[i][j].z);
	    }

	    vptr = sphvertices + (4*gridsize+vtx)*vtxsize;
	    *vptr++ = -grid[i][j].x;
	    *vptr++ = -grid[i][j].y;
	    *vptr++ = grid[i][j].z;
	    *vptr++ = -grid[i][j].x;
	    *vptr++ = -grid[i][j].y;
	    *vptr++ = grid[i][j].z;
	    if (genuv == DcOn) {
		if (grid[i][j].y == 0)
			*vptr++ = 0.;
		else
			*vptr++ = ddr_x11_XFDI_drom_render_sphere_ucomp(
									-grid[i][j].y, -grid[i][j].x);
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_vcomp(grid[i][j].z);
	    }
 
	    vptr = sphvertices + (5*gridsize+vtx)*vtxsize;
	    *vptr++ = -grid[i][j].x;
	    *vptr++ = grid[i][j].y;
	    *vptr++ = -grid[i][j].z;
	    *vptr++ = -grid[i][j].x;
	    *vptr++ = grid[i][j].y;
	    *vptr++ = -grid[i][j].z;
	    if (genuv == DcOn) {
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_ucomp(grid[i][j].y, -grid[i][j].x);
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_vcomp(-grid[i][j].z);
	    }
 
	    vptr = sphvertices + (6*gridsize+vtx)*vtxsize;
	    *vptr++ = grid[i][j].x;
	    *vptr++ = -grid[i][j].y;
	    *vptr++ = -grid[i][j].z;
	    *vptr++ = grid[i][j].x;
	    *vptr++ = -grid[i][j].y;
	    *vptr++ = -grid[i][j].z;
	    if (genuv == DcOn) {
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_ucomp(-grid[i][j].y, grid[i][j].x);
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_vcomp(-grid[i][j].z);
	    }
 
	    vptr = sphvertices + (7*gridsize+vtx)*vtxsize;
	    *vptr++ = -grid[i][j].x;
	    *vptr++ = -grid[i][j].y;
	    *vptr++ = -grid[i][j].z;
	    *vptr++ = -grid[i][j].x;
	    *vptr++ = -grid[i][j].y;
	    *vptr++ = -grid[i][j].z;
	    if (genuv == DcOn) {
		if (grid[i][j].y == 0)
			*vptr++ = 0.;
		else
			*vptr++ = ddr_x11_XFDI_drom_render_sphere_ucomp(
									-grid[i][j].y, -grid[i][j].x);
		*vptr++ = ddr_x11_XFDI_drom_render_sphere_vcomp(-grid[i][j].z);
	    }
	}
    }
	

    /* Set up triangles for all other octants */
    octcnt=0;
    for(i=1; i<=divs; i++) {

	for(j=1; j<=i; j++) {

	    ddr_x11_XFDI_drom_sphere_triangles(octcnt,octsize, gridsize,
					       i*row+j-1,i*row+j,(i-1)*row+j-1);
	    octcnt++;

	    if(j != i)		/* not end of row */ {
		ddr_x11_XFDI_drom_sphere_triangles(
						   octcnt,octsize, gridsize, 
						   (i-1)*row+j,(i-1)*row+j-1,i*row+j);	
		octcnt++;
	    }
	}
    }

    if (genuv == DcOn)
	    trimshobj = DoTriangleMesh(DcRGB,
				       DcLocNrm|dor_system_texture_uv_count(1),
				       8*row*row,sphvertices,octsize*8,tris,DcTrue);
    else
	    trimshobj = DoTriangleMesh(DcRGB,DcLocNrm, 8*row*row,
				       sphvertices,octsize*8,tris,DcTrue);
    return((dot_object *)trimshobj);

}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_sphere_triangles(cnt,octsize,gridsize,v0,v1,v2)
DtInt cnt;
DtInt octsize;
DtInt gridsize;
DtInt v0,v1,v2;
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
/*
 ======================================================================
 */

DtReal ddr_x11_XFDI_drom_render_sphere_ucomp(y,x)
DtReal y,x;
{
    DtReal dor_math_atan2();

    if (x==0 && y==0)
	    return (0.);

    return (dor_math_atan2(y,x)/TWOPI + .5);
}

/*
 ======================================================================
 */

DtReal ddr_x11_XFDI_drom_render_sphere_vcomp(z)
DtReal z;
{
    DtReal dor_math_acos();

    return (1.-dor_math_acos(z)/M_PI);
}
