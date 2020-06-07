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
 
/* #ident "%W% %G%" */
 
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
	dor_vartrilist_initialize
	dor_vartrilist_create
	dor_vartrilist_destroy
	dor_vartrilist_print
	dor_vartrilist_update_lines
	dor_vartrilist_bounding_box
	dor_vartrilist_create_alternate_geom_lines
	dor_vartrilist_calculate_triangle_normals
	dor_vartrilist_compute_bounding_volume
	dor_vartrilist_pick
	dor_vartrilist_update
	dor_vartrilist_update_uv
	dor_vartrilist_update_uvw

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/vartrilst.h>
#include <dore/dore_develop/private/varlinlst.h>

/*
 ======================================================================
 */

DtInt doe_vartrilist_class_id;

static DtMethodEntry vartrilist_methods[] = {
    {DcMethodCmpBndVolume,(DtMethodPtr)dor_vartrilist_compute_bounding_volume},
    {DcMethodDestroy,      (DtMethodPtr)dor_vartrilist_destroy},
    {DcMethodPick,          (DtMethodPtr)dor_vartrilist_pick},
    {DcMethodPrint,          (DtMethodPtr)dor_vartrilist_print},
    {DcMethodUpdStdAltLineObj,(DtMethodPtr)dor_vartrilist_update_lines},
};

void dor_vartrilist_initialize (void)
{
    doe_vartrilist_class_id = dor_class_add("DoVarTriangleList", 5, 
					     vartrilist_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_vartrilist_create (
    DtColorModel colormodel,
    DtInt triangle_count,
    DtReal *vertices,
    DtReal *vertnormal,
    DtReal *vertcolor,
    DtReal *vertalpha)
{
   static DtFlag initialized = DcFalse;
   struct vartrilist *vartrilist;
   dot_object *vartrilistobject;
   DtInt i;
   
   if (!initialized) {
      dor_vartrilist_initialize();
      initialized = DcTrue;
   }

   vartrilist = (struct vartrilist *)dor_space_allocate(sizeof *vartrilist);
   
   vartrilist->colormodel = colormodel;
   vartrilist->triangle_count = triangle_count;
   vartrilist->vertex_location = (DtRealTriple *)vertices;
   vartrilist->vertex_normal = (DtRealTriple *)vertnormal;
   vartrilist->vertex_color = (DtColorRGB *)vertcolor;
   vartrilist->vertex_alpha = (DtReal *)vertalpha;

   vartrilist->alt_object_lines = NULL;
   vartrilist->alt_triangle_count = 0;

   vartrilist->uv_count = 0;
   vartrilist->uv_list = (DtRealCouple **)dor_space_allocate(
                dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   for (i=0; i<dod_system_max_vertex_uv_cnt; i++) {
        vartrilist->uv_list[i] = NULL;
   }

   vartrilist->uvw_count = 0;
   vartrilist->uvw_list = (DtRealTriple **)dor_space_allocate(
                dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *))
;
   for (i=0; i<dod_system_max_vertex_uvw_cnt; i++) {
        vartrilist->uvw_list[i] = NULL;
   }


   vartrilist->element_normal = (DtRealTriple *)NULL;

   vartrilist->needelemnrms = DcTrue;
   vartrilist->decompose_lines = DcTrue;

   vartrilist->sequence_number = 1;

   dor_vartrilist_bounding_box(vartrilist);

   vartrilistobject = dor_object_create(doe_vartrilist_class_id,vartrilist);
   return(vartrilistobject);
}
/*
 ======================================================================
 */

void dor_vartrilist_destroy (dot_object *object)
{
    struct vartrilist *vartrilist;

    vartrilist = (struct vartrilist *)(object->data);

    if (vartrilist->element_normal != NULL) {
	dor_space_deallocate(vartrilist->element_normal);
    }

    if (vartrilist->uv_list != NULL) {
	dor_space_deallocate(vartrilist->uv_list);
    }

    if (vartrilist->uvw_list != NULL) {
	dor_space_deallocate(vartrilist->uvw_list);
    }

    dor_space_deallocate(vartrilist);
}
/*
 ======================================================================
 */

void dor_vartrilist_print (dot_object *object)
{
    int i;
    struct vartrilist *vartrilist;

    vartrilist = (struct vartrilist *)(object->data);

    if (vartrilist == NULL) {
        dor_print_output("variable triangle list is NULL");
        return;
    }
 
    switch (vartrilist->colormodel) {
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),
	    "vertex locations at [%lx]", vartrilist->vertex_location);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex normals at [%lx]", vartrilist->vertex_normal);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex colors at [%lx]", vartrilist->vertex_color);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"vertex count = %d",
	    vartrilist->triangle_count);
    dor_print_output(dor_print_get_line());

    for (i = 0; i < vartrilist->uv_count; ++i) {
	sprintf(dor_print_get_line(),
		"uvs at [%lx]", vartrilist->uv_list[i]);
	dor_print_output(dor_print_get_line());
    }
    for (i = 0; i < vartrilist->uvw_count; ++i) {
	sprintf(dor_print_get_line(),
		"uvws at [%lx]", vartrilist->uvw_list);
	dor_print_output(dor_print_get_line());
    }
    dor_print_output(dor_print_get_line());

}
/*
 ======================================================================
 */

dot_object *dor_vartrilist_update_lines (dot_object *object)
{
    struct vartrilist *vartrilist;

    vartrilist = (struct vartrilist *)(object->data);

    if (vartrilist->decompose_lines) {
	if (vartrilist->alt_object_lines != DcNullPtr) {
	    dor_object_get_method(vartrilist->alt_object_lines,
				  DcMethodDestroy)(vartrilist->alt_object_lines);
	    vartrilist->alt_object_lines = DcNullPtr;
	} 

	vartrilist->alt_object_lines =
		dor_vartrilist_create_alternate_geom_lines(vartrilist);

	if (vartrilist->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_vartrilist_update_lines", "");
	} else {
	    vartrilist->decompose_lines = DcFalse;
	}
    }
    return vartrilist->alt_object_lines;
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_vartrilist_bounding_box (struct vartrilist *vartrilist)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;
    DtInt vertexcount;

    if (vartrilist->triangle_count == 0){
	xmin = 0.;
	ymin = 0.;
	zmin = 0.;

	xmax = 0.;
	ymax = 0.;
	zmax = 0.;
    }
    else{
	xmin = vartrilist->vertex_location[0][0];
	xmax = vartrilist->vertex_location[0][0];

	ymin = vartrilist->vertex_location[0][1];
	ymax = vartrilist->vertex_location[0][1];

	zmin = vartrilist->vertex_location[0][2];
	zmax = vartrilist->vertex_location[0][2];

	vertexcount = 3 * vartrilist->triangle_count;

	for (vertexno=1;vertexno<vertexcount;vertexno++){
	    xmin = min(xmin,vartrilist->vertex_location[vertexno][0]);
	    xmax = max(xmax,vartrilist->vertex_location[vertexno][0]);

	    ymin = min(ymin,vartrilist->vertex_location[vertexno][1]);
	    ymax = max(ymax,vartrilist->vertex_location[vertexno][1]);

	    zmin = min(zmin,vartrilist->vertex_location[vertexno][2]);
	    zmax = max(zmax,vartrilist->vertex_location[vertexno][2]);
	}
    }

    vartrilist->bounding_box_pts[0][0] = xmin;
    vartrilist->bounding_box_pts[0][1] = ymin;
    vartrilist->bounding_box_pts[0][2] = zmin;

    vartrilist->bounding_box_pts[1][0] = xmin;
    vartrilist->bounding_box_pts[1][1] = ymax;
    vartrilist->bounding_box_pts[1][2] = zmin;

    vartrilist->bounding_box_pts[2][0] = xmax;
    vartrilist->bounding_box_pts[2][1] = ymax;
    vartrilist->bounding_box_pts[2][2] = zmin;

    vartrilist->bounding_box_pts[3][0] = xmax;
    vartrilist->bounding_box_pts[3][1] = ymin;
    vartrilist->bounding_box_pts[3][2] = zmin;

    vartrilist->bounding_box_pts[4][0] = xmin;
    vartrilist->bounding_box_pts[4][1] = ymin;
    vartrilist->bounding_box_pts[4][2] = zmax;

    vartrilist->bounding_box_pts[5][0] = xmin;
    vartrilist->bounding_box_pts[5][1] = ymax;
    vartrilist->bounding_box_pts[5][2] = zmax;

    vartrilist->bounding_box_pts[6][0] = xmax;
    vartrilist->bounding_box_pts[6][1] = ymax;
    vartrilist->bounding_box_pts[6][2] = zmax;

    vartrilist->bounding_box_pts[7][0] = xmax;
    vartrilist->bounding_box_pts[7][1] = ymin;
    vartrilist->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

dot_object *dor_vartrilist_create_alternate_geom_lines (
    struct vartrilist *vartrilist)
{
    DtInt vtxcnt;
    DtInt triangleno;
    DtInt vertexno;
    DtInt lvtx0, lvtx1;
    dot_object *varlinlstobj;

    if (vartrilist->alt_linevtx == NULL) {
	vartrilist->alt_linevtx = 
		(DtRealTriple *)dor_space_allocate
			(6*(vartrilist->triangle_count)*(sizeof(DtRealTriple)));
	vartrilist->alt_triangle_count = vartrilist->triangle_count;

	if (vartrilist->vertex_normal != NULL) {
	    vartrilist->alt_linenrm =
		    (DtRealTriple *)dor_space_allocate
			(6*(vartrilist->triangle_count)*(sizeof(DtRealTriple)));
	}
	if (vartrilist->vertex_color != NULL) {
	    vartrilist->alt_lineclr =
		    (DtRealTriple *)dor_space_allocate
			(6*(vartrilist->triangle_count)*(sizeof(DtRealTriple)));
	}

    } else if (vartrilist->triangle_count > vartrilist->alt_triangle_count) {
	vartrilist->alt_linevtx = 
		(DtRealTriple *)dor_space_reallocate(vartrilist->alt_linevtx,
			6*(vartrilist->triangle_count)*(sizeof(DtRealTriple)));
	vartrilist->alt_triangle_count = vartrilist->triangle_count;

	if (vartrilist->vertex_normal != NULL) {
	    vartrilist->alt_linenrm =
		    (DtRealTriple *)dor_space_reallocate(vartrilist->alt_linenrm,
			6*(vartrilist->triangle_count)*(sizeof(DtRealTriple)));
	}
	if (vartrilist->vertex_color != NULL) {
	    vartrilist->alt_lineclr =
		    (DtRealTriple *)dor_space_reallocate(vartrilist->alt_linenrm,
			6*(vartrilist->triangle_count)*(sizeof(DtRealTriple)));
	}
    } 
	
    vtxcnt = 0;

    for (triangleno=0; triangleno<vartrilist->triangle_count; triangleno++) {
	if (!dor_check_degen_triangle_vtx_values(vartrilist->vertex_location[triangleno],
					  vartrilist->vertex_location[triangleno+1],
					  vartrilist->vertex_location[triangleno+2]))
		/* Don't draw lines for degenerate triangles */
                continue;

	for (vertexno=0; vertexno<3; vertexno++) {
	    lvtx0 = triangleno * 3 + vertexno;
	    if (vertexno == 2)
		    lvtx1 = triangleno * 3;
	    else
		    lvtx1 = lvtx0+1;

	    /* one end of the line */

	    vartrilist->alt_linevtx[vtxcnt][0] = 
		    vartrilist->vertex_location[lvtx0][0];
	    vartrilist->alt_linevtx[vtxcnt][1] = 
		    vartrilist->vertex_location[lvtx0][1];
	    vartrilist->alt_linevtx[vtxcnt][2] = 
		    vartrilist->vertex_location[lvtx0][2];

	    if (vartrilist->vertex_normal != NULL) {
		vartrilist->alt_linenrm[vtxcnt][0] = 
			vartrilist->vertex_normal[lvtx0][0];
		vartrilist->alt_linenrm[vtxcnt][1] = 
			vartrilist->vertex_normal[lvtx0][1];
		vartrilist->alt_linenrm[vtxcnt][2] = 
			vartrilist->vertex_normal[lvtx0][2];
	    }

	    if (vartrilist->vertex_color != NULL) {
		vartrilist->alt_lineclr[vtxcnt][0] = 
			vartrilist->vertex_color[lvtx0][0];
		vartrilist->alt_lineclr[vtxcnt][1] = 
			vartrilist->vertex_color[lvtx0][1];
		vartrilist->alt_lineclr[vtxcnt][2] = 
			vartrilist->vertex_color[lvtx0][2];
	    }

	    vtxcnt++;

	    /* the other end of the line */
	
	    vartrilist->alt_linevtx[vtxcnt][0] = 
		    vartrilist->vertex_location[lvtx1][0];
	    vartrilist->alt_linevtx[vtxcnt][1] = 
		    vartrilist->vertex_location[lvtx1][1];
	    vartrilist->alt_linevtx[vtxcnt][2] = 
		    vartrilist->vertex_location[lvtx1][2];

	    if (vartrilist->vertex_normal != NULL) {
		vartrilist->alt_linenrm[vtxcnt][0] = 
			vartrilist->vertex_normal[lvtx1][0];
		vartrilist->alt_linenrm[vtxcnt][1] = 
			vartrilist->vertex_normal[lvtx1][1];
		vartrilist->alt_linenrm[vtxcnt][2] = 
			vartrilist->vertex_normal[lvtx1][2];
	    }

	    if (vartrilist->vertex_color != NULL) {
		vartrilist->alt_lineclr[vtxcnt][0] = 
			vartrilist->vertex_color[lvtx1][0];
		vartrilist->alt_lineclr[vtxcnt][1] = 
			vartrilist->vertex_color[lvtx1][1];
		vartrilist->alt_lineclr[vtxcnt][2] = 
			vartrilist->vertex_color[lvtx1][2];
	    }

	    vtxcnt++;
	} 
    }

    varlinlstobj = dor_varlinlst_create(vartrilist->colormodel, vtxcnt/2,
					(DtReal*)vartrilist->alt_linevtx,
					(DtReal*)vartrilist->alt_linenrm,
					(DtReal*)vartrilist->alt_lineclr);
    return (varlinlstobj);
}
		

/*
 ======================================================================
 */

void dor_vartrilist_calculate_triangle_normals (struct vartrilist *vartrilst)
{
    DtInt triangleno;

    for (triangleno=0;triangleno<vartrilst->triangle_count;triangleno++){
	dor_math_fast_cross_cosines(vartrilst->vertex_location[triangleno*3],
				    vartrilst->vertex_location[triangleno*3+1],
				    vartrilst->vertex_location[triangleno*3+2],
				    vartrilst->element_normal[triangleno]);

	dor_math_renormalize_fast(vartrilst->element_normal[triangleno]);
    }
}

/*
 ======================================================================
 */

void dor_vartrilist_compute_bounding_volume (dot_object *object)
{
    struct vartrilist *vartrilist;

    vartrilist = (struct vartrilist *)(object->data);

    dor_system_report_bounding_points
	(8, (DtReal*)vartrilist->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_vartrilist_pick (dot_object *object)
{
    struct vartrilist *vartrilist;

    if (!dor_condex_query_pick(doe_vartrilist_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag) {
   	dor_pick_initialize_object(object);
    }

    vartrilist = (struct vartrilist *)(object->data);

    dor_pick_triangle_list(vartrilist->bounding_box_pts,
			    vartrilist->triangle_count,
			    vartrilist->vertex_location);
}
/*
 ======================================================================
 */

void dor_vartrilist_update (
    dot_object *object,
    DtInt vertexcount,
    DtReal *vlocs,
    DtReal *vnrms,
    DtReal *vclrs)
{
    struct vartrilist *vartrilist;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartrilist_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleList",DcNullPtr);
	return;
    }   
#endif

    vartrilist = (struct vartrilist *)(object->data);

    vartrilist->sequence_number++;

    if (vertexcount != 0) {
	vartrilist->triangle_count = vertexcount;
    }
   
    if (vlocs != NULL) {
	vartrilist->vertex_location = (DtRealTriple *)vlocs;

	dor_vartrilist_bounding_box(vartrilist);
	vartrilist->needelemnrms = DcTrue;
	
    }


    /* If object was created without vertex normals then you cannot
       pass them in now */
	
    if (vnrms != NULL) {
	if (vartrilist->vertex_normal != NULL) 
		vartrilist->vertex_normal = (DtRealTriple *)vnrms;
	else
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "DpUpdVarTriangleList", 
			    "vertex normals ignored");
    }	


    /* If object was created without colors then you cannot 
       pass them in now */

    if (vclrs != NULL) {
	if (vartrilist->vertex_color != NULL)
		vartrilist->vertex_color = (DtColorRGB *)vclrs;
	else 
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "DpUpdVarTriangleList", 
			    "vertex colors ignored");
    }

    vartrilist->decompose_lines = DcTrue;

}
/*
 ======================================================================
 */

void dor_vartrilist_update_uv (
    dot_object *object,
    DtInt uvindex,
    DtRealCouple *uvlist)
{
    struct vartrilist *vartrilist;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartrilist_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleListUV",DcNullPtr);
	return;
    }   
#endif

    vartrilist = (struct vartrilist *)(object->data);

#ifdef ERRCHK
    if (uvindex<0) {
          DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleListUV",
                        "uv index < 0");
          return;
    } else if (uvindex > vartrilist->uv_count) {
          DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleListUV",
                        "uv index too large");
          return;
    }
#endif

    vartrilist->sequence_number++;
    if (vartrilist->uv_count < uvindex+1) {
	vartrilist->uv_count = uvindex+1;
    }
    vartrilist->uv_list[uvindex] = uvlist;
}
/*
 ======================================================================
 */

void dor_vartrilist_update_uvw (
    dot_object *object,
    DtInt uvwindex,
    DtRealTriple *uvwlist)
{
    struct vartrilist *vartrilist;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartrilist_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleListUVW",DcNullPtr);
	return;
    }   
#endif

    vartrilist = (struct vartrilist *)(object->data);

#ifdef ERRCHK
    if (uvwindex < 0) {
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleListUVW",
                        "uvw index < 0");
        return;
    } else if (uvwindex > vartrilist->uvw_count) {
        DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleListUVW",
                        "uvw index too large");
        return;
    }
#endif

    vartrilist->sequence_number++;
    if (vartrilist->uvw_count < uvwindex+1)
	    vartrilist->uvw_count = uvwindex+1;
    vartrilist->uvw_list[uvwindex] = uvwlist;   
}
