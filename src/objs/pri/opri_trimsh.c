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
	dor_trimsh_initialize
	dor_trimsh_create
	dor_trimsh_destroy
	dor_trimsh_print
	dor_trimsh_update_lines
	dor_trimsh_bounding_box
	dor_trimsh_create_alternate_lines
	dor_trimsh_calculate_triangle_normals
	dor_trimsh_calculate_vertex_normals
	dor_trimsh_compute_bounding_volume
	dor_trimsh_pick

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/trimsh.h>
#include <dore/dore_develop/private/linlst.h>

/*
 ======================================================================
 */

DtInt doe_trimsh_class_id;

static DtMethodEntry trimsh_methods[] = {
    { DcMethodCmpBndVolume, (DtMethodPtr)dor_trimsh_compute_bounding_volume },
    { DcMethodDestroy, (DtMethodPtr)dor_trimsh_destroy },
    { DcMethodPick, (DtMethodPtr)dor_trimsh_pick },
    { DcMethodPrint, (DtMethodPtr)dor_trimsh_print },
    { DcMethodUpdStdAltLineObj, (DtMethodPtr)dor_trimsh_update_lines }
};

void dor_trimsh_initialize (void)
{
    doe_trimsh_class_id = dor_class_install (DcTypeTriangleMesh, "DoTriangleMesh", 
					     5, trimsh_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_trimsh_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt vertexcount,
    DtReal vertices[3],
    DtInt trianglecount,
    DtInt trivtxlst[][3],
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct trimsh *trimsh;
   DtInt vertexno;
   DtReal *vertexptr;
   trimsh_vertexlist *trimshvtxlstptr, *trivtxlstptr;
   DtInt vertex1, vertex2, vertex3;
   DtInt triangleno;
   dot_object *trimshobject;
   DtVertexType basevtxtype;
   DtInt uv;
   DtInt uvw;

   if (!initialized) {
	dor_trimsh_initialize();
	initialized = DcTrue;
   }

   trimsh = (struct trimsh *)dor_space_allocate(sizeof *trimsh);

   trimsh->alt_object_lines = (DtObjectStructure *)0;

   trimsh->colormodel = colormodel;

   basevtxtype = dod_system_inq_vertex_basetype(vertextype);
   trimsh->vertextype = basevtxtype;

   trimsh->vertex_count = vertexcount;
   
   trimsh->vertex_location = (DtRealTriple *)dor_space_allocate(
			   vertexcount*(sizeof (DtRealTriple)));

   trimsh->vertex_normal = (DtRealTriple *)NULL;

   trimsh->vertex_color = (DtColorRGB *)NULL;

   trimsh->vertex_alpha = (DtReal *)NULL;


   /* texture coordinates */

   trimsh->uv_list = (DtRealCouple **)dor_space_allocate(
		dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   trimsh->uvw_list = (DtRealTriple **)dor_space_allocate(
		dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

   for (uv = 0; uv < dod_system_max_vertex_uv_cnt; uv++) {
	trimsh->uv_list[uv] = NULL;
   }
   for (uvw = 0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
	trimsh->uvw_list[uvw] = NULL;
   }

   trimsh->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
   trimsh->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

   for (uv=0; uv< trimsh->uv_count; uv++) {
	trimsh->uv_list[uv] = (DtRealCouple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealCouple)));
   }

   for (uvw=0; uvw< trimsh->uvw_count; uvw++) {
	trimsh->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealTriple)));
   }

   /**/

   vertexptr = vertices;

    if (basevtxtype & DcLocNrm)
        trimsh->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        trimsh->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        trimsh->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

    for (vertexno=0; vertexno<vertexcount; vertexno++) {
        trimsh->vertex_location[vertexno][0] = *vertexptr++;
        trimsh->vertex_location[vertexno][1] = *vertexptr++;
        trimsh->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            trimsh->vertex_normal[vertexno][0] = *vertexptr++;
            trimsh->vertex_normal[vertexno][1] = *vertexptr++;
            trimsh->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            trimsh->vertex_color[vertexno][0] = *vertexptr++;
            trimsh->vertex_color[vertexno][1] = *vertexptr++;
            trimsh->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            trimsh->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < trimsh->uv_count; uv++) {
            trimsh->uv_list[uv][vertexno][0] = *vertexptr++;
            trimsh->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < trimsh->uvw_count; uvw++) {
            trimsh->uvw_list[uvw][vertexno][0] = *vertexptr++;
            trimsh->uvw_list[uvw][vertexno][1] = *vertexptr++;
            trimsh->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }

   trimsh->triangle_vertexlist = (trimsh_vertexlist *)dor_space_allocate(
			trianglecount*(sizeof (trimsh_vertexlist)));

   trivtxlstptr = trivtxlst;
   trimshvtxlstptr = trimsh->triangle_vertexlist;
   
   trimsh->triangle_count = 0;

   for (triangleno=0;triangleno<trianglecount;triangleno++) {
	vertex1 = (*trivtxlstptr)[0];
	vertex2 = (*trivtxlstptr)[1];
	vertex3 = (*trivtxlstptr)[2];

	trivtxlstptr++;

	if ((vertex1 < 0) || (vertex1 >= vertexcount) ||
	    (vertex2 < 0) || (vertex2 >= vertexcount) ||
	    (vertex3 < 0) || (vertex3 >= vertexcount)) {
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_trimsh_create", 
			    "vertex index < 0 or > vertexcount");
		continue;
	}

	/* Check if same vertex is used more than once in same triangle.
	   Add the triangle to the primitive's list if it is not
	   degenerate.  */

	if (dor_check_degen_triangle_vtx_values(
	    &(trimsh->vertex_location[vertex1][0]),
	    &(trimsh->vertex_location[vertex2][0]),
	    &(trimsh->vertex_location[vertex3][0]))) {

	    (*trimshvtxlstptr)[0] = vertex1;
	    (*trimshvtxlstptr)[1] = vertex2;
	    (*trimshvtxlstptr)[2] = vertex3;

	    trimsh->triangle_count++;

	    trimshvtxlstptr++;
	}
   }

   /*  Since we will use the element normals to compute the vertex
  	 normals, compute the element normals first.  */

   trimsh->element_normal = (DtRealTriple *)dor_space_allocate(
			trianglecount*(sizeof (DtRealTriple)));

   dor_trimsh_calculate_triangle_normals(trimsh);

   if (smoothflag) {

        if (!(basevtxtype & DcLocNrm)) {

		trimsh->vertex_normal = 
			(DtRealTriple *)dor_space_allocate(
				vertexcount*(sizeof (DtRealTriple)));

		dor_trimsh_calculate_vertex_normals(trimsh);
	}
   }

   trimsh->smoothflag = smoothflag;

   dor_trimsh_bounding_box(trimsh);
   
   trimsh->compiled_triangle_vertexlist = NULL;

   trimshobject = dor_object_create(doe_trimsh_class_id,trimsh);

   return(trimshobject);
}
/*
 ======================================================================
 */

void dor_trimsh_destroy (dot_object *object)
{
    struct trimsh *trimsh;
    int i;

    trimsh = (struct trimsh *)(object->data);

    if (trimsh->vertex_location != NULL) {
	dor_space_deallocate(trimsh->vertex_location);
    }

    if (trimsh->vertex_normal != NULL) {
	dor_space_deallocate(trimsh->vertex_normal);
    }

    if (trimsh->vertex_color != NULL) {
	dor_space_deallocate(trimsh->vertex_color);
    }

    if (trimsh->vertex_alpha != NULL) {
	dor_space_deallocate(trimsh->vertex_alpha);
    }

    if (trimsh->element_normal != NULL) {
	dor_space_deallocate(trimsh->element_normal);
    }

    for (i=0; i< trimsh->uv_count; i++) {
	dor_space_deallocate(trimsh->uv_list[i]);
    }
    dor_space_deallocate(trimsh->uv_list);

    for (i=0; i< trimsh->uvw_count; i++) {
	dor_space_deallocate(trimsh->uvw_list[i]);
    }
    dor_space_deallocate(trimsh->uvw_list);

    if (trimsh->triangle_vertexlist != NULL){
	dor_space_deallocate(trimsh->triangle_vertexlist);
    }

    if (trimsh->compiled_triangle_vertexlist != NULL){
	dor_space_deallocate(trimsh->compiled_triangle_vertexlist);
    }

    if (trimsh->alt_object_lines != NULL) {
	dor_object_check_deletion(trimsh->alt_object_lines);
    }

    dor_space_deallocate(trimsh);
}
/*
 ======================================================================
 */

void dor_trimsh_print (dot_object *object)
{
    struct trimsh *trimsh;

    trimsh = (struct trimsh *)(object->data);

    if (trimsh == NULL){
	dor_print_output("triangle mesh is NULL");
	return;
    }
 
    switch (trimsh->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(trimsh->vertextype);

    sprintf(dor_print_get_line(),"smooth flag = %d",trimsh->smoothflag);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"vertex count = %d",trimsh->vertex_count);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"triangle count = %d",trimsh->triangle_count);
    dor_print_output(dor_print_get_line());

    if (trimsh->uv_count > 0) {
	sprintf (dor_print_get_line(), "uv's per vertex = %d", trimsh->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (trimsh->uvw_count > 0) {
	sprintf (dor_print_get_line(), "uvw's per vertex = %d", trimsh->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

dot_object *dor_trimsh_update_lines (dot_object *object)
{
    struct trimsh *trimsh;
    dot_object *alt;

    trimsh = (struct trimsh *)(object->data);

    if (trimsh->alt_object_lines != DcNullPtr) {
	alt = trimsh->alt_object_lines;

    } else {
	trimsh->alt_object_lines = 
		dor_trimsh_create_alternate_lines(trimsh);
	alt = trimsh->alt_object_lines;

	if (trimsh->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_trimsh_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_trimsh_bounding_box (struct trimsh *trimsh)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt i, vertexno;
    DtInt trino;

    if (trimsh->triangle_count == 0) {
        xmin = 0.;
        ymin = 0.;
        zmin = 0.;

        xmax = 0.;
        ymax = 0.;
        zmax = 0.;
    } else {

        vertexno = trimsh->triangle_vertexlist[0][0];

        xmin = trimsh->vertex_location[vertexno][0];
        xmax = trimsh->vertex_location[vertexno][0];

        ymin = trimsh->vertex_location[vertexno][1];
        ymax = trimsh->vertex_location[vertexno][1];

        zmin = trimsh->vertex_location[vertexno][2];
        zmax = trimsh->vertex_location[vertexno][2];

PRAGMA(IVDEP)
PRAGMA(VBEST)

        /* use only the vertices that are referenced in the mesh */

        for (trino=0;trino<trimsh->triangle_count;trino++){
            for (i=0; i<3; i++) {
                vertexno = trimsh->triangle_vertexlist[trino][i];

	        xmin = min(xmin,trimsh->vertex_location[vertexno][0]);
	        xmax = max(xmax,trimsh->vertex_location[vertexno][0]);

	        ymin = min(ymin,trimsh->vertex_location[vertexno][1]);
	        ymax = max(ymax,trimsh->vertex_location[vertexno][1]);

	        zmin = min(zmin,trimsh->vertex_location[vertexno][2]);
	        zmax = max(zmax,trimsh->vertex_location[vertexno][2]);
            }
        }
    }

    trimsh->bounding_box_pts[0][0] = xmin;
    trimsh->bounding_box_pts[0][1] = ymin;
    trimsh->bounding_box_pts[0][2] = zmin;

    trimsh->bounding_box_pts[1][0] = xmin;
    trimsh->bounding_box_pts[1][1] = ymax;
    trimsh->bounding_box_pts[1][2] = zmin;

    trimsh->bounding_box_pts[2][0] = xmax;
    trimsh->bounding_box_pts[2][1] = ymax;
    trimsh->bounding_box_pts[2][2] = zmin;

    trimsh->bounding_box_pts[3][0] = xmax;
    trimsh->bounding_box_pts[3][1] = ymin;
    trimsh->bounding_box_pts[3][2] = zmin;

    trimsh->bounding_box_pts[4][0] = xmin;
    trimsh->bounding_box_pts[4][1] = ymin;
    trimsh->bounding_box_pts[4][2] = zmax;

    trimsh->bounding_box_pts[5][0] = xmin;
    trimsh->bounding_box_pts[5][1] = ymax;
    trimsh->bounding_box_pts[5][2] = zmax;

    trimsh->bounding_box_pts[6][0] = xmax;
    trimsh->bounding_box_pts[6][1] = ymax;
    trimsh->bounding_box_pts[6][2] = zmax;

    trimsh->bounding_box_pts[7][0] = xmax;
    trimsh->bounding_box_pts[7][1] = ymin;
    trimsh->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

#define MAXVTXS (18 + dod_system_max_vertex_uv_cnt*4 + \
			dod_system_max_vertex_uvw_cnt*6)

dot_object *dor_trimsh_create_alternate_lines (struct trimsh *trimsh)
{
    DtReal vtxs[MAXVTXS];
    DtVertexType vertextype;
    DtInt trino;
    DtInt lvtx0, lvtx1;
    DtUInt i;
    DtUInt uv, uvw;
    DtInt vertexno;
    dot_object *linlstobj;

    vertextype = trimsh->vertextype;
    if (trimsh->smoothflag) {
         trimsh->vertextype |= DcLocNrm;
    }

    linlstobj = dor_linlst_open(trimsh->colormodel, 
				vertextype | 
			   	dor_system_texture_uv_count(trimsh->uv_count) |
				dor_system_texture_uvw_count(trimsh->uvw_count),
				3*trimsh->triangle_count);

    for (trino=0; trino<trimsh->triangle_count; trino++) {
	for (vertexno=0; vertexno<3; vertexno++){

	    lvtx0 = trimsh->triangle_vertexlist[trino][vertexno];
	    if (vertexno==2) 
		    lvtx1 = trimsh->triangle_vertexlist[trino][0];
	    else
		    lvtx1 = trimsh->triangle_vertexlist[trino][vertexno+1];

	    i = 0;

	    /* One end of a line */

	    vtxs[i++] = trimsh->vertex_location[lvtx0][0];
	    vtxs[i++] = trimsh->vertex_location[lvtx0][1];
	    vtxs[i++] = trimsh->vertex_location[lvtx0][2];

	    if (trimsh->vertextype & DcLocNrm) {
		vtxs[i++] = trimsh->vertex_normal[lvtx0][0];
		vtxs[i++] = trimsh->vertex_normal[lvtx0][1];
		vtxs[i++] = trimsh->vertex_normal[lvtx0][2];
	    }

	    if (trimsh->vertextype & DcLocClr) {
		vtxs[i++] = trimsh->vertex_color[lvtx0][0];
		vtxs[i++] = trimsh->vertex_color[lvtx0][1];
		vtxs[i++] = trimsh->vertex_color[lvtx0][2];
	    }
	
	    if (trimsh->vertextype & DcLocAlpha) {
		vtxs[i++] = trimsh->vertex_alpha[lvtx0];
	    }
	
	    for (uv=0; uv<trimsh->uv_count; uv++) {
		vtxs[i++] = trimsh->uv_list[uv][lvtx0][0]; 
		vtxs[i++] = trimsh->uv_list[uv][lvtx0][1]; 
	    }
	
	    for (uvw=0; uvw<trimsh->uvw_count; uvw++) {
		vtxs[i++] = trimsh->uvw_list[uvw][lvtx0][0]; 
		vtxs[i++] = trimsh->uvw_list[uvw][lvtx0][1]; 
		vtxs[i++] = trimsh->uvw_list[uvw][lvtx0][2]; 
	    }
	
	    /* The other end of the line */
	
	    vtxs[i++] = trimsh->vertex_location[lvtx1][0]; 
	    vtxs[i++] = trimsh->vertex_location[lvtx1][1]; 
	    vtxs[i++] = trimsh->vertex_location[lvtx1][2]; 
	 
	    if (trimsh->vertextype & DcLocNrm) {
		vtxs[i++] = trimsh->vertex_normal[lvtx1][0];
		vtxs[i++] = trimsh->vertex_normal[lvtx1][1];
		vtxs[i++] = trimsh->vertex_normal[lvtx1][2];
	    } 
	 
	    if (trimsh->vertextype & DcLocClr) {
		vtxs[i++] = trimsh->vertex_color[lvtx1][0];
		vtxs[i++] = trimsh->vertex_color[lvtx1][1];
		vtxs[i++] = trimsh->vertex_color[lvtx1][2];
	    }

	    if (trimsh->vertextype & DcLocAlpha) {
		vtxs[i++] = trimsh->vertex_alpha[lvtx1];
            }
	
	    for (uv=0; uv<trimsh->uv_count; uv++) {
		vtxs[i++] = trimsh->uv_list[uv][lvtx1][0]; 
		vtxs[i++] = trimsh->uv_list[uv][lvtx1][1]; 
	    }
	
	    for (uvw=0; uvw<trimsh->uvw_count; uvw++) {
		vtxs[i++] = trimsh->uvw_list[uvw][lvtx1][0]; 
		vtxs[i++] = trimsh->uvw_list[uvw][lvtx1][1]; 
		vtxs[i++] = trimsh->uvw_list[uvw][lvtx1][2]; 
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

void dor_trimsh_calculate_triangle_normals (struct trimsh *trimsh)
{
    DtInt triangleno;
    DtInt vertex1, vertex2, vertex3;

    for (triangleno=0;triangleno<trimsh->triangle_count;triangleno++){
	vertex1 = trimsh->triangle_vertexlist[triangleno][0];
	vertex2 = trimsh->triangle_vertexlist[triangleno][1];
	vertex3 = trimsh->triangle_vertexlist[triangleno][2];

	dor_math_fast_cross_cosines(trimsh->vertex_location[vertex1],
				    trimsh->vertex_location[vertex2],
				    trimsh->vertex_location[vertex3],
				    trimsh->element_normal[triangleno]);

	dor_math_renormalize_fast(trimsh->element_normal[triangleno]);
    }
}
/*
 ======================================================================
 */

void dor_trimsh_calculate_vertex_normals (struct trimsh *trimsh)
{
    DtFlag *vertexrefflag;
    DtInt vertexno;
    DtInt triangleno;
    DtInt trianglevertexno;

    /* Allocate a reference flag table for each vertex.  The reference
    ** flag tells us if the vertex belongs to any triangle, and therefore
    ** whether its direction vector was computed and should be renormalized. */

    vertexrefflag = (DtFlag *)dor_space_allocate
			(trimsh->vertex_count*(sizeof (DtFlag)));

    /* Zero the vertex normals and triangle reference flags for
    ** all vertices.  */

PRAGMA(ASIS)
    for (vertexno=0;vertexno<trimsh->vertex_count;vertexno++) {
   	vertexrefflag[vertexno] = DcFalse;

   	trimsh->vertex_normal[vertexno][0] = 0.;
   	trimsh->vertex_normal[vertexno][1] = 0.;
   	trimsh->vertex_normal[vertexno][2] = 0.;
    }

    /*  For each of the triangle's three vertices, update that vertex's
	triangle reference count and accumulated triangle normal.  */

PRAGMA(ASIS)
    for (triangleno=0;triangleno<trimsh->triangle_count;triangleno++) {

   	for (trianglevertexno=0;trianglevertexno<3;trianglevertexno++) {

	    vertexno = trimsh->triangle_vertexlist[triangleno][trianglevertexno];

	    vertexrefflag[vertexno] = DcTrue;

	    trimsh->vertex_normal[vertexno][0] +=
		    trimsh->element_normal[triangleno][0];
	    trimsh->vertex_normal[vertexno][1] +=
		    trimsh->element_normal[triangleno][1];
	    trimsh->vertex_normal[vertexno][2] +=
		    trimsh->element_normal[triangleno][2];
   	}
    }

    /*  Rescale the vertex normals.  */

PRAGMA(ASIS)
    for (vertexno=0;vertexno<trimsh->vertex_count;vertexno++) {

   	if (vertexrefflag[vertexno]) {
	    if (!dor_math_renormalize_vector(trimsh->vertex_normal[vertexno])) {
		DDerror (ERR_ZERO_TRI_NRM_SUM,
			    "dor_trimsh_calculate_vertex_normals", 
			    DcNullPtr);
	    }
   	}
    }

    /*  Free the vertex reference flags.  */

    dor_space_deallocate(vertexrefflag);
}
/*
 ======================================================================
 */

void dor_trimsh_compute_bounding_volume (dot_object *object)
{
    struct trimsh *trimsh;

    trimsh = (struct trimsh *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*) trimsh->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_trimsh_pick (dot_object *object)
{
    struct trimsh *trimsh;

    if (!dor_condex_query_pick(doe_trimsh_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag){
	dor_pick_initialize_object(object);
    }

    trimsh = (struct trimsh *)(object->data);

    dor_pick_triangle_mesh(trimsh->bounding_box_pts,
			   trimsh->triangle_count,
			   trimsh->vertex_count,
			   trimsh->vertex_location,
			   trimsh->triangle_vertexlist);
}
