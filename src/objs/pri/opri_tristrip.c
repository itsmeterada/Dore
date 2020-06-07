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
	int dor_tristrip_initialize 
	dot_object * dor_tristrip_create 
	int dor_tristrip_destroy 
	int dor_tristrip_print 
	dot_object * dor_tristrip_update_alternate_lines 
	int dor_tristrip_bounding_box 
	dot_object * dor_tristrip_create_alternate_lines 
	int dor_tristrip_calculate_triangle_normals 
	int dor_tristrip_compute_bounding_volume 
	int dor_tristrip_pick 

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/tristrip.h>
#include <dore/dore_develop/private/linlst.h>

DtInt doe_tristrip_class_id;
DtFlag doe_tristrip_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry tristrip_methods[] = {
    { DcMethodCmpBndVolume,
	    (DtMethodPtr) dor_tristrip_compute_bounding_volume
    },
    { DcMethodDestroy,
	    (DtMethodPtr) dor_tristrip_destroy
    },
    { DcMethodPick,
	    (DtMethodPtr) dor_tristrip_pick
    },
    { DcMethodPrint,
	    (DtMethodPtr) dor_tristrip_print
    },
    { DcMethodUpdStdAltLineObj,
	    (DtMethodPtr) dor_tristrip_update_alternate_lines
    }
};

void dor_tristrip_initialize (void)
{
    doe_tristrip_class_id = dor_class_add ("DoTriangleStrip", 5, 
					   tristrip_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_tristrip_create  (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt vertexcount,
    DtReal vertices[])
{
    struct tristrip *tristrip;
    DtReal *vertexptr;
    DtInt vertexno;
    DtVertexType basevtxtype;
    DtInt uv;
    DtInt uvw;
    dot_object *tristripobject;
   
    if (!doe_tristrip_initialized) {
      	dor_tristrip_initialize();
   	doe_tristrip_initialized = DcTrue;
    }

    tristrip = (struct tristrip *)dor_space_allocate(sizeof *tristrip);

    tristrip->colormodel = colormodel;

    tristrip->vertex_count = vertexcount;

    basevtxtype = dod_system_inq_vertex_basetype(vertextype);
    tristrip->vertextype = basevtxtype;

    /*  Vertices always have locations.  */

    tristrip->vertex_location = (DtRealTriple *)dor_space_allocate
			(vertexcount*(sizeof (DtRealTriple)));

    /* texture coordinates */

    tristrip->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
    tristrip->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

    if (tristrip->uv_count > 0) {
   	tristrip->uv_list = (DtRealCouple **)dor_space_allocate
			(tristrip->uv_count*sizeof (DtRealCouple *));
   	for (uv=0; uv< tristrip->uv_count; uv++) {
	    tristrip->uv_list[uv] = 
		    (DtRealCouple *) dor_space_allocate
			    (vertexcount*(sizeof (DtRealCouple)));
   
	}
    } else {
	tristrip->uv_list = NULL;
    }

    if (tristrip->uvw_count > 0) {
   	tristrip->uvw_list = (DtRealTriple **)dor_space_allocate
			(tristrip->uvw_count*sizeof (DtRealTriple *));

   	for (uvw=0; uvw< tristrip->uvw_count; uvw++) {
	    tristrip->uvw_list[uvw] = 
		    (DtRealTriple *)dor_space_allocate
			(vertexcount*(sizeof (DtRealTriple)));
	}
    } else {
	tristrip->uvw_list = NULL;
    }

    tristrip->vertex_normal = (DtRealTriple *)NULL;
    tristrip->vertex_color = (DtColorRGB *)NULL;
    tristrip->vertex_alpha = (DtReal *)NULL;

    /**/

    vertexptr = vertices;

    if (basevtxtype & DcLocNrm)
        tristrip->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        tristrip->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        tristrip->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

PRAGMA(ASIS)
    for (vertexno=0;vertexno<vertexcount;vertexno++) {
        tristrip->vertex_location[vertexno][0] = *vertexptr++;
        tristrip->vertex_location[vertexno][1] = *vertexptr++;
        tristrip->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            tristrip->vertex_normal[vertexno][0] = *vertexptr++;
            tristrip->vertex_normal[vertexno][1] = *vertexptr++;
            tristrip->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            tristrip->vertex_color[vertexno][0] = *vertexptr++;
            tristrip->vertex_color[vertexno][1] = *vertexptr++;
            tristrip->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            tristrip->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < tristrip->uv_count; uv++) {
            tristrip->uv_list[uv][vertexno][0] = *vertexptr++;
            tristrip->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < tristrip->uvw_count; uvw++) {
            tristrip->uvw_list[uvw][vertexno][0] = *vertexptr++;
            tristrip->uvw_list[uvw][vertexno][1] = *vertexptr++;
            tristrip->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }

    tristrip->alt_object_lines = NULL;

    dor_tristrip_bounding_box(tristrip);

    /*  Compute element normals for all triangles in the list.  */

    tristrip->element_normal = (DtRealTriple *)dor_space_allocate
			((vertexcount-2)*(sizeof (DtRealTriple)));
    dor_tristrip_calculate_triangle_normals(tristrip);

    tristrip->triangle_vertexlist = NULL;
    tristrip->compiled_triangle_vertexlist = NULL;

    tristripobject = dor_object_create(doe_tristrip_class_id,tristrip);

    return(tristripobject);
}
/*
 ======================================================================
 */

void dor_tristrip_destroy (dot_object *object)
{
    struct tristrip *tristrip;
    int i;

    tristrip = (struct tristrip *)(object->data);

    if (tristrip->vertex_location != NULL) {
   	dor_space_deallocate(tristrip->vertex_location);
    }

    if (tristrip->vertex_normal != NULL) {
   	dor_space_deallocate(tristrip->vertex_normal);
    }

    if (tristrip->vertex_color != NULL) {
   	dor_space_deallocate(tristrip->vertex_color);
    }

    if (tristrip->vertex_alpha != NULL) {
   	dor_space_deallocate(tristrip->vertex_alpha);
    }

    if (tristrip->uv_list != NULL) {
   	for (i=0; i< tristrip->uv_count; i++) 
              	dor_space_deallocate(tristrip->uv_list[i]);
   	dor_space_deallocate(tristrip->uv_list);
    }

    if (tristrip->uvw_list != NULL) {
	for (i=0; i< tristrip->uvw_count; i++) 
              	dor_space_deallocate(tristrip->uvw_list[i]);
   	dor_space_deallocate(tristrip->uvw_list);
    }

    if (tristrip->element_normal != NULL) {
   	dor_space_deallocate(tristrip->element_normal);
    }

    if (tristrip->triangle_vertexlist != NULL) {
   	dor_space_deallocate(tristrip->triangle_vertexlist);
    }

    if (tristrip->compiled_triangle_vertexlist != NULL) {
   	dor_space_deallocate(tristrip->compiled_triangle_vertexlist);
    }

    dor_space_deallocate(tristrip);
}
/*
 ======================================================================
 */

void dor_tristrip_print (dot_object *object)
{
    struct tristrip *tristrip;

    tristrip = (struct tristrip *)(object->data);

    if (tristrip == NULL) {
        dor_print_output("triangle list is NULL");
        return;
    }
 
    switch (tristrip->colormodel) {
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;

    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(tristrip->vertextype);

    sprintf(dor_print_get_line(),
	    "vertex count = %d",tristrip->vertex_count);
    dor_print_output(dor_print_get_line());

    if (tristrip->uv_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uv's per vertex = %d", tristrip->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (tristrip->uvw_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uvw's per vertex = %d", tristrip->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

dot_object *dor_tristrip_update_alternate_lines (dot_object *object)
{
    struct tristrip *tristrip;
    dot_object *alt;

    tristrip = (struct tristrip *)(object->data);

    if (tristrip->alt_object_lines != DcNullPtr) {
	alt = tristrip->alt_object_lines;

    } else {
	tristrip->alt_object_lines = 
		dor_tristrip_create_alternate_lines(tristrip);
	alt = tristrip->alt_object_lines;

	if (tristrip->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_tristrip_update_alternate_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_tristrip_bounding_box (struct tristrip *tristrip)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt vertexno;

    if (tristrip->vertex_count == 0) {
   	xmin = 0.;
   	ymin = 0.;
   	zmin = 0.;

   	xmax = 0.;
   	ymax = 0.;
   	zmax = 0.;
    } else {
   	xmin = tristrip->vertex_location[0][0];
   	xmax = tristrip->vertex_location[0][0];

   	ymin = tristrip->vertex_location[0][1];
   	ymax = tristrip->vertex_location[0][1];

   	zmin = tristrip->vertex_location[0][2];
   	zmax = tristrip->vertex_location[0][2];

   	for (vertexno=1;vertexno<tristrip->vertex_count;vertexno++) {
	    xmin = min(xmin,tristrip->vertex_location[vertexno][0]);
	    xmax = max(xmax,tristrip->vertex_location[vertexno][0]);

	    ymin = min(ymin,tristrip->vertex_location[vertexno][1]);
	    ymax = max(ymax,tristrip->vertex_location[vertexno][1]);

	    zmin = min(zmin,tristrip->vertex_location[vertexno][2]);
	    zmax = max(zmax,tristrip->vertex_location[vertexno][2]);
   	}
    }

    tristrip->bounding_box_pts[0][0] = xmin;
    tristrip->bounding_box_pts[0][1] = ymin;
    tristrip->bounding_box_pts[0][2] = zmin;

    tristrip->bounding_box_pts[1][0] = xmin;
    tristrip->bounding_box_pts[1][1] = ymax;
    tristrip->bounding_box_pts[1][2] = zmin;

    tristrip->bounding_box_pts[2][0] = xmax;
    tristrip->bounding_box_pts[2][1] = ymax;
    tristrip->bounding_box_pts[2][2] = zmin;

    tristrip->bounding_box_pts[3][0] = xmax;
    tristrip->bounding_box_pts[3][1] = ymin;
    tristrip->bounding_box_pts[3][2] = zmin;

    tristrip->bounding_box_pts[4][0] = xmin;
    tristrip->bounding_box_pts[4][1] = ymin;
    tristrip->bounding_box_pts[4][2] = zmax;

    tristrip->bounding_box_pts[5][0] = xmin;
    tristrip->bounding_box_pts[5][1] = ymax;
    tristrip->bounding_box_pts[5][2] = zmax;

    tristrip->bounding_box_pts[6][0] = xmax;
    tristrip->bounding_box_pts[6][1] = ymax;
    tristrip->bounding_box_pts[6][2] = zmax;

    tristrip->bounding_box_pts[7][0] = xmax;
    tristrip->bounding_box_pts[7][1] = ymin;
    tristrip->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

#define MAXVTXS (18 + dod_system_max_vertex_uv_cnt*4 + \
			dod_system_max_vertex_uvw_cnt*6)

dot_object *dor_tristrip_create_alternate_lines (struct tristrip *tristrip)
{
    DtReal vtxs[MAXVTXS];
    DtInt lvtx0, lvtx1;
    DtUInt i;
    DtUInt uv, uvw;
    DtInt basevtxno;
    DtInt vertexno;
    dot_object *linlstobj;

    linlstobj = dor_linlst_open(tristrip->colormodel, 
				tristrip->vertextype |
				dor_system_texture_uv_count(tristrip->uv_count) |
				dor_system_texture_uvw_count(tristrip->uvw_count),
				3*tristrip->vertex_count);

    for (basevtxno=0; basevtxno<tristrip->vertex_count-2; basevtxno++) {

	if (!dor_check_degen_triangle_vtx_values(tristrip->vertex_location[basevtxno],
					 tristrip->vertex_location[basevtxno+1],
					 tristrip->vertex_location[basevtxno+2]))
		/* Don't draw lines for degenerate triangles */
                continue;

	for (vertexno=0; vertexno<3; vertexno++){

	    lvtx0 = basevtxno+vertexno;
	    if (vertexno==2) 
		    lvtx1 = basevtxno;
	    else
		    lvtx1 = basevtxno+vertexno+1;

	    i = 0;

	    /* One end of a line */

	    vtxs[i++] = tristrip->vertex_location[lvtx0][0];
	    vtxs[i++] = tristrip->vertex_location[lvtx0][1];
	    vtxs[i++] = tristrip->vertex_location[lvtx0][2];

	    if (tristrip->vertextype & DcLocNrm) {
		vtxs[i++] = tristrip->vertex_normal[lvtx0][0];
		vtxs[i++] = tristrip->vertex_normal[lvtx0][1];
		vtxs[i++] = tristrip->vertex_normal[lvtx0][2];
	    }

	    if (tristrip->vertextype & DcLocClr) {
		vtxs[i++] = tristrip->vertex_color[lvtx0][0];
		vtxs[i++] = tristrip->vertex_color[lvtx0][1];
		vtxs[i++] = tristrip->vertex_color[lvtx0][2];
	    }
	
	    if (tristrip->vertextype & DcLocAlpha) {
		vtxs[i++] = tristrip->vertex_alpha[lvtx0];
	    }
	
	    for (uv=0; uv<tristrip->uv_count; uv++) {
		vtxs[i++] = tristrip->uv_list[uv][lvtx0][0]; 
		vtxs[i++] = tristrip->uv_list[uv][lvtx0][1]; 
	    }
	
	    for (uvw=0; uvw<tristrip->uvw_count; uvw++) {
		vtxs[i++] = tristrip->uvw_list[uvw][lvtx0][0]; 
		vtxs[i++] = tristrip->uvw_list[uvw][lvtx0][1]; 
		vtxs[i++] = tristrip->uvw_list[uvw][lvtx0][2]; 
	    }
	
	    /* The other end of the line */
	
	    vtxs[i++] = tristrip->vertex_location[lvtx1][0]; 
	    vtxs[i++] = tristrip->vertex_location[lvtx1][1]; 
	    vtxs[i++] = tristrip->vertex_location[lvtx1][2]; 
	 
	    if (tristrip->vertextype & DcLocNrm) {
		vtxs[i++] = tristrip->vertex_normal[lvtx1][0];
		vtxs[i++] = tristrip->vertex_normal[lvtx1][1];
		vtxs[i++] = tristrip->vertex_normal[lvtx1][2];
	    } 
	 
	    if (tristrip->vertextype & DcLocClr) {
		vtxs[i++] = tristrip->vertex_color[lvtx1][0];
		vtxs[i++] = tristrip->vertex_color[lvtx1][1];
		vtxs[i++] = tristrip->vertex_color[lvtx1][2];
	    }
	
	    if (tristrip->vertextype & DcLocAlpha) {
		vtxs[i++] = tristrip->vertex_alpha[lvtx1];
	    }
	
	    for (uv=0; uv<tristrip->uv_count; uv++) {
		vtxs[i++] = tristrip->uv_list[uv][lvtx1][0]; 
		vtxs[i++] = tristrip->uv_list[uv][lvtx1][1]; 
	    }
	
	    for (uvw=0; uvw<tristrip->uvw_count; uvw++) {
		vtxs[i++] = tristrip->uvw_list[uvw][lvtx1][0]; 
		vtxs[i++] = tristrip->uvw_list[uvw][lvtx1][1]; 
		vtxs[i++] = tristrip->uvw_list[uvw][lvtx1][2]; 
	    }
	
	    /* Add the line to the list */
   		
	    dor_linlst_add_line(linlstobj,vtxs);
	
   	}
    }
    dor_linlst_close(linlstobj);
    return(linlstobj);
}
/*
 ======================================================================
 */

void dor_tristrip_calculate_triangle_normals (struct tristrip *tristrip)
{
    DtInt basevtx;
    DtInt odd;

    odd = DcTrue;
    for (basevtx=0;basevtx<(tristrip->vertex_count-2);basevtx++){

 	if (odd) {
	    odd = DcFalse;
	    dor_math_fast_cross_cosines(tristrip->vertex_location[basevtx],
					tristrip->vertex_location[basevtx+1],
					tristrip->vertex_location[basevtx+2],
					tristrip->element_normal[basevtx]);
	} else {
	    odd = DcTrue;
	    dor_math_fast_cross_cosines(tristrip->vertex_location[basevtx],
					tristrip->vertex_location[basevtx+2],
					tristrip->vertex_location[basevtx+1],
					tristrip->element_normal[basevtx]);
	}

   	dor_math_renormalize_fast(tristrip->element_normal[basevtx]);
    }
}
/*
 ======================================================================
 */

void dor_tristrip_compute_bounding_volume (dot_object *object)
{
    struct tristrip *tristrip;

    tristrip = (struct tristrip *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)tristrip->bounding_box_pts);
}
/*
 ======================================================================
 */

void dor_tristrip_pick (dot_object *object)
{
    struct tristrip *tristrip;

    if (!dor_condex_query_pick(doe_tristrip_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag) {
   	dor_pick_initialize_object(object);
    }

    tristrip = (struct tristrip *)(object->data);

    dor_pick_triangle_strip(tristrip->bounding_box_pts,
			    tristrip->vertex_count,
			    tristrip->vertex_location);
}
