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
	dor_vartristrip_initialize
	dor_vartristrip_create
	dor_vartristrip_destroy
	dor_vartristrip_print
	dor_vartristrip_update_lines
	dor_vartristrip_bounding_box
	dor_vartristrip_create_alternate_geom_lines
	dor_vartristrip_calculate_triangle_normals
	dor_vartristrip_compute_bounding_volume
	dor_vartristrip_pick
	dor_vartristrip_update
	dor_vartristrip_update_uv
	dor_vartristrip_update_uvw

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/vartristrp.h>
#include <dore/dore_develop/private/varlinlst.h>

/*
 ======================================================================
 */

DtInt doe_vartristrip_class_id;

static DtMethodEntry vartristrip_methods[] = {
    {DcMethodCmpBndVolume,(DtMethodPtr)dor_vartristrip_compute_bounding_volume},
    {DcMethodDestroy,      (DtMethodPtr)dor_vartristrip_destroy},
    {DcMethodPick,          (DtMethodPtr)dor_vartristrip_pick},
    {DcMethodPrint,          (DtMethodPtr)dor_vartristrip_print},
    {DcMethodUpdStdAltLineObj,(DtMethodPtr)dor_vartristrip_update_lines},
};

void dor_vartristrip_initialize (void)
{
    doe_vartristrip_class_id = dor_class_add("DoVarTriangleStrip", 5, 
					     vartristrip_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_vartristrip_create (
    DtColorModel colormodel,
    DtInt vertexcount,
    DtReal *vertices,
    DtReal *vertnormal,
    DtReal *vertcolor)
{
   static DtFlag initialized = DcFalse;
   struct vartristrip *vartristrip;
   dot_object *vartristripobject;
   DtInt i;
   
   if (!initialized) {
      dor_vartristrip_initialize();
      initialized = DcTrue;
   }

   vartristrip = (struct vartristrip *)dor_space_allocate(sizeof *vartristrip);
   
   vartristrip->colormodel = colormodel;
   vartristrip->vertex_count = vertexcount;
   vartristrip->vertex_location = (DtRealTriple *)vertices;
   vartristrip->vertex_normal = (DtRealTriple *)vertnormal;
   vartristrip->vertex_color = (DtColorRGB *)vertcolor;

   vartristrip->alt_object_lines = NULL;
   vartristrip->alt_linevtx = (DtRealTriple *)NULL;
   vartristrip->alt_linenrm = (DtRealTriple *)NULL;
   vartristrip->alt_lineclr = (DtColorRGB *)NULL;
   vartristrip->alt_vertexcount = 0;

   vartristrip->uv_count = 0;
   vartristrip->uv_list = (DtRealCouple **)dor_space_allocate(
                dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   for (i=0; i<dod_system_max_vertex_uv_cnt; i++) {
        vartristrip->uv_list[i] = NULL;
   }

   vartristrip->uvw_count = 0;
   vartristrip->uvw_list = (DtRealTriple **)dor_space_allocate(
                dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));
   for (i=0; i<dod_system_max_vertex_uvw_cnt; i++) {
        vartristrip->uvw_list[i] = NULL;
   }


   vartristrip->element_normal = (DtRealTriple *)NULL;
   vartristrip->triangle_vertexlist = NULL;
   vartristrip->compiled_triangle_vertexlist = NULL;

   vartristrip->needelemnrms = DcTrue;
   vartristrip->decompose_lines = DcTrue;

   vartristrip->sequence_number = 1;

   dor_vartristrip_bounding_box(vartristrip);

   vartristripobject = dor_object_create(doe_vartristrip_class_id,vartristrip);
   return(vartristripobject);
}
/*
 ======================================================================
 */

void dor_vartristrip_destroy (dot_object *object)
{
    struct vartristrip *vartristrip;

    vartristrip = (struct vartristrip *)(object->data);

    if (vartristrip->element_normal != NULL) {
	dor_space_deallocate(vartristrip->element_normal);
    }

    if (vartristrip->triangle_vertexlist != NULL) {
	dor_space_deallocate(vartristrip->triangle_vertexlist);
    }

    if (vartristrip->compiled_triangle_vertexlist != NULL) {
	dor_space_deallocate(vartristrip->compiled_triangle_vertexlist);
    }

    dor_space_deallocate (vartristrip->uv_list);
    dor_space_deallocate (vartristrip->uvw_list);

    dor_space_deallocate(vartristrip);
}
/*
 ======================================================================
 */

void dor_vartristrip_print (dot_object *object)
{
    struct vartristrip *vartristrip;

    vartristrip = (struct vartristrip *)(object->data);

    if (vartristrip == NULL) {
        dor_print_output("variable triangle strip is NULL");
        return;
    }
 
    switch (vartristrip->colormodel) {
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),
	    "vertex locations at [%lx]", vartristrip->vertex_location);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex normals at [%lx]", vartristrip->vertex_normal);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex colors at [%lx]", vartristrip->vertex_color);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"vertex count = %d",
	    vartristrip->vertex_count);
    dor_print_output(dor_print_get_line());

}
/*
 ======================================================================
 */

dot_object *dor_vartristrip_update_lines (dot_object *object)
{
    struct vartristrip *vartristrip;

    vartristrip = (struct vartristrip *)(object->data);

    if (vartristrip->decompose_lines) {
	if (vartristrip->alt_object_lines != DcNullPtr) {
	    dor_object_get_method(vartristrip->alt_object_lines,
				  DcMethodDestroy)(vartristrip->alt_object_lines);
	    vartristrip->alt_object_lines = DcNullPtr;
	} 

	vartristrip->alt_object_lines =
		dor_vartristrip_create_alternate_geom_lines(vartristrip);

	if (vartristrip->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_vartristrip_update_lines", "");
	} else {
	    vartristrip->decompose_lines = DcFalse;
	}
    }
    return vartristrip->alt_object_lines;
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_vartristrip_bounding_box (struct vartristrip *vartristrip)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    xmin = vartristrip->vertex_location[0][0];
    xmax = vartristrip->vertex_location[0][0];

    ymin = vartristrip->vertex_location[0][1];
    ymax = vartristrip->vertex_location[0][1];

    zmin = vartristrip->vertex_location[0][2];
    zmax = vartristrip->vertex_location[0][2];

    vertexcount = vartristrip->vertex_count;

PRAGMA(IVDEP)
PRAGMA(VBEST)
    for (vertexno=1;vertexno<vertexcount;vertexno++){
	xmin = min(xmin,vartristrip->vertex_location[vertexno][0]);
	xmax = max(xmax,vartristrip->vertex_location[vertexno][0]);

	ymin = min(ymin,vartristrip->vertex_location[vertexno][1]);
	ymax = max(ymax,vartristrip->vertex_location[vertexno][1]);

	zmin = min(zmin,vartristrip->vertex_location[vertexno][2]);
	zmax = max(zmax,vartristrip->vertex_location[vertexno][2]);
    }

    vartristrip->bounding_box_pts[0][0] = xmin;
    vartristrip->bounding_box_pts[0][1] = ymin;
    vartristrip->bounding_box_pts[0][2] = zmin;

    vartristrip->bounding_box_pts[1][0] = xmin;
    vartristrip->bounding_box_pts[1][1] = ymax;
    vartristrip->bounding_box_pts[1][2] = zmin;

    vartristrip->bounding_box_pts[2][0] = xmax;
    vartristrip->bounding_box_pts[2][1] = ymax;
    vartristrip->bounding_box_pts[2][2] = zmin;

    vartristrip->bounding_box_pts[3][0] = xmax;
    vartristrip->bounding_box_pts[3][1] = ymin;
    vartristrip->bounding_box_pts[3][2] = zmin;

    vartristrip->bounding_box_pts[4][0] = xmin;
    vartristrip->bounding_box_pts[4][1] = ymin;
    vartristrip->bounding_box_pts[4][2] = zmax;

    vartristrip->bounding_box_pts[5][0] = xmin;
    vartristrip->bounding_box_pts[5][1] = ymax;
    vartristrip->bounding_box_pts[5][2] = zmax;

    vartristrip->bounding_box_pts[6][0] = xmax;
    vartristrip->bounding_box_pts[6][1] = ymax;
    vartristrip->bounding_box_pts[6][2] = zmax;

    vartristrip->bounding_box_pts[7][0] = xmax;
    vartristrip->bounding_box_pts[7][1] = ymin;
    vartristrip->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

dot_object *dor_vartristrip_create_alternate_geom_lines (
    struct vartristrip *vartristrip)
{
    DtInt vtxcnt;
    DtInt basevtxno;
    DtInt vertexno;
    DtInt lvtx0, lvtx1;
    dot_object *varlinlstobj;

    if (vartristrip->alt_linevtx == NULL) {
	vartristrip->alt_linevtx = 
		(DtRealTriple *)dor_space_allocate
			(6*(vartristrip->vertex_count-2)*(sizeof(DtRealTriple)));
	vartristrip->alt_vertexcount = vartristrip->vertex_count;

	if (vartristrip->vertex_normal != NULL) {
	    vartristrip->alt_linenrm =
		    (DtRealTriple *)dor_space_allocate
			(6*(vartristrip->vertex_count-2)*(sizeof(DtRealTriple)));
	}
	if (vartristrip->vertex_color != NULL) {
	    vartristrip->alt_lineclr =
		    (DtRealTriple *)dor_space_allocate
			(6*(vartristrip->vertex_count-2)*(sizeof(DtRealTriple)));
	}

    } else if (vartristrip->vertex_count > vartristrip->alt_vertexcount) {
	vartristrip->alt_linevtx = 
		(DtRealTriple *)dor_space_reallocate(vartristrip->alt_linevtx,
			6*(vartristrip->vertex_count-2)*(sizeof(DtRealTriple)));
	vartristrip->alt_vertexcount = vartristrip->vertex_count;

	if (vartristrip->vertex_normal != NULL) {
	    vartristrip->alt_linenrm =
		    (DtRealTriple *)dor_space_reallocate(vartristrip->alt_linenrm,
			6*(vartristrip->vertex_count-2)*(sizeof(DtRealTriple)));
	}
	if (vartristrip->vertex_color != NULL) {
	    vartristrip->alt_lineclr =
		    (DtRealTriple *)dor_space_reallocate(vartristrip->alt_linenrm,
			6*(vartristrip->vertex_count-2)*(sizeof(DtRealTriple)));
	}
    } 
	
    vtxcnt = 0;

    for (basevtxno=0; basevtxno<vartristrip->vertex_count-2; basevtxno++) {
	if (!dor_check_degen_triangle_vtx_values(vartristrip->vertex_location[basevtxno],
					  vartristrip->vertex_location[basevtxno+1],
					  vartristrip->vertex_location[basevtxno+2]))
		/* Don't draw lines for degenerate triangles */
                continue;

	for (vertexno=0; vertexno<3; vertexno++) {
	    lvtx0 = basevtxno+vertexno;
	    if (vertexno == 2)
		    lvtx1 = basevtxno;
	    else
		    lvtx1 = lvtx0+1;

	    /* one end of the line */

	    vartristrip->alt_linevtx[vtxcnt][0] = 
		    vartristrip->vertex_location[lvtx0][0];
	    vartristrip->alt_linevtx[vtxcnt][1] = 
		    vartristrip->vertex_location[lvtx0][1];
	    vartristrip->alt_linevtx[vtxcnt][2] = 
		    vartristrip->vertex_location[lvtx0][2];

	    if (vartristrip->vertex_normal != NULL) {
		vartristrip->alt_linenrm[vtxcnt][0] = 
			vartristrip->vertex_normal[lvtx0][0];
		vartristrip->alt_linenrm[vtxcnt][1] = 
			vartristrip->vertex_normal[lvtx0][1];
		vartristrip->alt_linenrm[vtxcnt][2] = 
			vartristrip->vertex_normal[lvtx0][2];
	    }

	    if (vartristrip->vertex_color != NULL) {
		vartristrip->alt_lineclr[vtxcnt][0] = 
			vartristrip->vertex_color[lvtx0][0];
		vartristrip->alt_lineclr[vtxcnt][1] = 
			vartristrip->vertex_color[lvtx0][1];
		vartristrip->alt_lineclr[vtxcnt][2] = 
			vartristrip->vertex_color[lvtx0][2];
	    }

	    vtxcnt++;

	    /* the other end of the line */
	
	    vartristrip->alt_linevtx[vtxcnt][0] = 
		    vartristrip->vertex_location[lvtx1][0];
	    vartristrip->alt_linevtx[vtxcnt][1] = 
		    vartristrip->vertex_location[lvtx1][1];
	    vartristrip->alt_linevtx[vtxcnt][2] = 
		    vartristrip->vertex_location[lvtx1][2];

	    if (vartristrip->vertex_normal != NULL) {
		vartristrip->alt_linenrm[vtxcnt][0] = 
			vartristrip->vertex_normal[lvtx1][0];
		vartristrip->alt_linenrm[vtxcnt][1] = 
			vartristrip->vertex_normal[lvtx1][1];
		vartristrip->alt_linenrm[vtxcnt][2] = 
			vartristrip->vertex_normal[lvtx1][2];
	    }

	    if (vartristrip->vertex_color != NULL) {
		vartristrip->alt_lineclr[vtxcnt][0] = 
			vartristrip->vertex_color[lvtx1][0];
		vartristrip->alt_lineclr[vtxcnt][1] = 
			vartristrip->vertex_color[lvtx1][1];
		vartristrip->alt_lineclr[vtxcnt][2] = 
			vartristrip->vertex_color[lvtx1][2];
	    }

	    vtxcnt++;
	} 
    }

    varlinlstobj = dor_varlinlst_create(vartristrip->colormodel, vtxcnt/2,
					(DtReal*)vartristrip->alt_linevtx,
					(DtReal*)vartristrip->alt_linenrm,
					(DtReal*)vartristrip->alt_lineclr);
    return (varlinlstobj);
}
			

/*
 ======================================================================
 */

void dor_vartristrip_calculate_triangle_normals (
    struct vartristrip *vartristrip)
{
    DtInt basevtx;
    DtInt odd;

    odd = DcTrue;
    for (basevtx=0;basevtx<(vartristrip->vertex_count-2);basevtx++){

 	if (odd) {
	    odd = DcFalse;
	    dor_math_fast_cross_cosines(vartristrip->vertex_location[basevtx],
					vartristrip->vertex_location[basevtx+1],
					vartristrip->vertex_location[basevtx+2],
					vartristrip->element_normal[basevtx]);
	} else {
	    odd = DcTrue;
	    dor_math_fast_cross_cosines(vartristrip->vertex_location[basevtx],
					vartristrip->vertex_location[basevtx+2],
					vartristrip->vertex_location[basevtx+1],
					vartristrip->element_normal[basevtx]);
	}

   	dor_math_renormalize_fast(vartristrip->element_normal[basevtx]);
    }
}
/*
 ======================================================================
 */

void dor_vartristrip_compute_bounding_volume (dot_object *object)
{
    struct vartristrip *vartristrip;

    vartristrip = (struct vartristrip *)(object->data);

    dor_system_report_bounding_points
	(8, (DtReal*)vartristrip->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_vartristrip_pick (dot_object *object)
{
    struct vartristrip *vartristrip;

    if (!dor_condex_query_pick(doe_vartristrip_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag) {
   	dor_pick_initialize_object(object);
    }

    vartristrip = (struct vartristrip *)(object->data);

    dor_pick_triangle_strip(vartristrip->bounding_box_pts,
			    vartristrip->vertex_count,
			    vartristrip->vertex_location);
}
/*
 ======================================================================
 */

void dor_vartristrip_update (
    dot_object *object,
    DtInt vertexcount,
    DtReal *vlocs,
    DtReal *vnrms,
    DtReal *vclrs)
{
    struct vartristrip *vartristrip;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartristrip_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleStrip",DcNullPtr);
	return;
    }   
#endif

    vartristrip = (struct vartristrip *)(object->data);

    vartristrip->sequence_number++;

    if (vertexcount != 0) {
	vartristrip->vertex_count = vertexcount;
    }
   
    if (vlocs != NULL) {
	vartristrip->vertex_location = (DtRealTriple *)vlocs;

	dor_vartristrip_bounding_box(vartristrip);
	vartristrip->needelemnrms = DcTrue;
	
    }


    /* If object was created without vertex normals then you cannot
       pass them in now */
	
    if (vnrms != NULL) {
	if (vartristrip->vertex_normal != NULL) 
		vartristrip->vertex_normal = (DtRealTriple *)vnrms;
	else
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "DpUpdVarTriangleStrip", 
			    "vertex normals ignored");
    }	


    /* If object was created without colors then you cannot 
       pass them in now */

    if (vclrs != NULL) {
	if (vartristrip->vertex_color != NULL)
		vartristrip->vertex_color = (DtColorRGB *)vclrs;
	else 
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "DpUpdVarTriangleStrip", 
			    "vertex colors ignored");
    }

    vartristrip->decompose_lines = DcTrue;

}
/*
 ======================================================================
 */

void dor_vartristrip_update_uv (
    dot_object *object,
    DtInt uvindex,
    DtRealCouple *uvlist)
{
    struct vartristrip *vartristrip;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartristrip_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleStripUV",DcNullPtr);
	return;
    }   
#endif

    vartristrip = (struct vartristrip *)(object->data);

#ifdef ERRCHK
    if (uvindex<0) {
          DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleStripUV",
                        "uv index < 0");
          return;
    } else if (uvindex > vartristrip->uv_count) {
          DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleStripUV",
                        "uv index too large");
          return;
    }
#endif

    vartristrip->sequence_number++;
    if (vartristrip->uv_count < uvindex+1) {
	vartristrip->uv_count = uvindex+1;
    }
    vartristrip->uv_list[uvindex] = uvlist;
}
/*
 ======================================================================
 */

void dor_vartristrip_update_uvw (
    dot_object *object,
    DtInt uvwindex,
    DtRealTriple *uvwlist)
{
    struct vartristrip *vartristrip;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartristrip_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleStripUVW",DcNullPtr);
	return;
    }   
#endif

    vartristrip = (struct vartristrip *)(object->data);

#ifdef ERRCHK
    if (uvwindex < 0) {
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleStripUVW",
                        "uvw index < 0");
        return;
    } else if (uvwindex > vartristrip->uvw_count) {
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleStripUVW",
                        "uvw index too large");
        return;
    }
#endif

    vartristrip->sequence_number++;
    if (vartristrip->uvw_count < uvwindex+1)
	    vartristrip->uvw_count = uvwindex+1;
    vartristrip->uvw_list[uvwindex] = uvwlist;   
}
