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
	dor_vartrimsh_initialize 
	dor_vartrimsh_create
	dor_vartrimsh_create_with_edges
	dor_vartrimsh_destroy 
	dor_vartrimsh_print 
	dor_vartrimsh_update_lines 
	dor_vartrimsh_bounding_box 
	dor_vartrimsh_create_alternate_geom_lines
	dor_vartrimsh_calculate_triangle_normals
	dor_vartrimsh_calculate_vertex_normals 
	dor_vartrimsh_compute_bounding_volume 
	dor_vartrimsh_pick 
	dor_vartrimsh_update 
	dor_vartrimsh_update_uv 
	dor_vartrimsh_update_uvw

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/vartrimsh.h>
#include <dore/dore_develop/private/varlinlst.h>

/*
 ======================================================================
 */

DtInt doe_vartrimsh_class_id;

static DtMethodEntry vartrimsh_methods[] = {
    {DcMethodCmpBndVolume, (DtMethodPtr)dor_vartrimsh_compute_bounding_volume},
    {DcMethodDestroy,       (DtMethodPtr)dor_vartrimsh_destroy},
    {DcMethodPick,           (DtMethodPtr)dor_vartrimsh_pick},
    {DcMethodPrint,           (DtMethodPtr)dor_vartrimsh_print},
    {DcMethodUpdStdAltLineObj, (DtMethodPtr)dor_vartrimsh_update_lines}
};

void dor_vartrimsh_initialize (void)
{
    doe_vartrimsh_class_id = dor_class_install (DcTypeVarTriangleMesh, 
				"DoVarTriangleMesh", 5, vartrimsh_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_vartrimsh_create (
    DtColorModel colormodel,
    DtInt vertexcount,
    DtReal *vertices,
    DtReal *vertnormal,
    DtReal *vertcolor,
    DtInt trianglecount,
    DtInt trivtxlst[][3],
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct vartrimsh *vartrimsh;
   vartrimsh_vertexlist *vartrimsh_vlstptr;
   vartrimsh_vertexlist *tri_vlstptr;
   DtInt vertex1, vertex2, vertex3;
   DtInt triangleno;
   DtInt i;
   dot_object *vartrimshobject;
   
   if (!initialized) {
      dor_vartrimsh_initialize();
      initialized = DcTrue;
   }

   vartrimsh = (struct vartrimsh *)dor_space_allocate(sizeof *vartrimsh);
   
   vartrimsh->colormodel = colormodel;
   vartrimsh->vertex_count = vertexcount;
   vartrimsh->vertex_location = (DtRealTriple *)vertices;
   vartrimsh->vertex_normal = (DtRealTriple *)vertnormal;
   vartrimsh->vertex_color = (DtColorRGB *)vertcolor;
   vartrimsh->triangle_count = trianglecount;
   vartrimsh->smoothflag = smoothflag;
   vartrimsh->needelemnrms = DcTrue;
   vartrimsh->compiled_triangle_vertexlist = NULL;
   vartrimsh->element_normal = (DtRealTriple *)NULL;
   vartrimsh->alt_object_lines = NULL;
   vartrimsh->alt_linevtx = (DtRealTriple *)NULL;
   vartrimsh->alt_linenrm = (DtRealTriple *)NULL;
   vartrimsh->alt_lineclr = (DtColorRGB *)NULL;
   vartrimsh->decompose_lines = DcTrue;

   vartrimsh->uv_count = 0;
   vartrimsh->uv_list = (DtRealCouple **)dor_space_allocate(
                dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   for (i=0; i<dod_system_max_vertex_uv_cnt; i++) {
	vartrimsh->uv_list[i] = NULL;
   }

   vartrimsh->uvw_count = 0;
   vartrimsh->uvw_list = (DtRealTriple **)dor_space_allocate(
                dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));
   for (i=0; i<dod_system_max_vertex_uvw_cnt; i++) {
	vartrimsh->uvw_list[i] = NULL;
   }

   vartrimsh->triangle_vertexlist = 
		(vartrimsh_vertexlist *)dor_space_allocate(
		trianglecount*(sizeof (vartrimsh_vertexlist)));

   tri_vlstptr = trivtxlst;
   vartrimsh_vlstptr = vartrimsh->triangle_vertexlist;

   for (triangleno=0;triangleno<trianglecount;triangleno++) {
	vertex1 = (*tri_vlstptr)[0];
	vertex2 = (*tri_vlstptr)[1];
	vertex3 = (*tri_vlstptr)[2];

	tri_vlstptr++;

	if ((vertex1 < 0) || (vertex1 >= vertexcount) ||
		(vertex2 < 0) || (vertex2 >= vertexcount) ||
		(vertex3 < 0) || (vertex3 >= vertexcount)) {
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_vartrimsh_create", 
			    "vertex index > vertexcount");

		continue;
	}

	(*vartrimsh_vlstptr)[0] = vertex1;
	(*vartrimsh_vlstptr)[1] = vertex2;
	(*vartrimsh_vlstptr)[2] = vertex3;

	vartrimsh_vlstptr++;
   }

   if (smoothflag) {
	if (vertnormal == NULL) {
		vartrimsh->needvertnrms = DcTrue;		
	} else {
		/* If we got vertex normal from the user never
		mess with them */
		vartrimsh->smoothflag = DcFalse;
		vartrimsh->needvertnrms = DcFalse;
	}
   } else {
	vartrimsh->needvertnrms = DcFalse;
   }

   vartrimsh->sequence_number = 1;

   /* Edge enable field is currently only for varsimplymsh. */
   vartrimsh->edge_enable = NULL;

   dor_vartrimsh_bounding_box(vartrimsh);

   vartrimshobject = dor_object_create(doe_vartrimsh_class_id,vartrimsh);
   return(vartrimshobject);
}

/*
 ======================================================================
 */
dot_object *dor_vartrimsh_create_with_edges (
    DtColorModel colormodel,
    DtInt vertex_count,
    DtReal *vertex_location,
    DtReal *vertex_normal,
    DtReal *vertex_color,
    DtInt tricount,
    DtIntTriple *trilist,
    DtFlag smoothflag,
    DtInt *edges)
{
   struct vartrimsh *vartrimsh;
   dot_object *vartrimshobject;

   vartrimshobject = dor_vartrimsh_create(colormodel, vertex_count,
                        vertex_location, vertex_normal, vertex_color,
                        tricount, trilist, DcFalse);

   vartrimsh = (struct vartrimsh *)(vartrimshobject->data);
   vartrimsh->edge_enable = edges;

   return(vartrimshobject);
}

/*
 ======================================================================
 */

void dor_vartrimsh_destroy (dot_object *object)
{
    struct vartrimsh *vartrimsh;

    vartrimsh = (struct vartrimsh *)(object->data);

    if (vartrimsh->smoothflag && (vartrimsh->vertex_normal != NULL)) {
	dor_space_deallocate(vartrimsh->vertex_normal);
    }

    if (vartrimsh->element_normal != NULL) {
	dor_space_deallocate(vartrimsh->element_normal);
    }

    if (vartrimsh->triangle_vertexlist != NULL) {
	dor_space_deallocate(vartrimsh->triangle_vertexlist);
    }

    if (vartrimsh->compiled_triangle_vertexlist != NULL) {
	dor_space_deallocate(vartrimsh->compiled_triangle_vertexlist);
    }

    dor_space_deallocate (vartrimsh->uv_list);
    dor_space_deallocate (vartrimsh->uvw_list);

    dor_space_deallocate(vartrimsh);
}
/*
 ======================================================================
 */

void dor_vartrimsh_print (dot_object *object)
{
    struct vartrimsh *vartrimsh;

    vartrimsh = (struct vartrimsh *)(object->data);

    if (vartrimsh == NULL){
	dor_print_output("variable triangle mesh is NULL");
	return;
    }
 
    switch (vartrimsh->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;
 
    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    sprintf(dor_print_get_line(),
	    "vertex locations at [%lx]", vartrimsh->vertex_location);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex normals at [%lx]", vartrimsh->vertex_normal);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex colors at [%lx]", vartrimsh->vertex_color);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"smooth flag = %d",vartrimsh->smoothflag);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"vertex count = %d",vartrimsh->vertex_count);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"triangle count = %d",vartrimsh->triangle_count);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

dot_object *dor_vartrimsh_update_lines (dot_object *object)
{
    struct vartrimsh *vartrimsh;

    vartrimsh = (struct vartrimsh *)(object->data);

    if (vartrimsh->decompose_lines) {
	if (vartrimsh->alt_object_lines != DcNullPtr) {
	    dor_object_get_method(vartrimsh->alt_object_lines,
				  DcMethodDestroy)(vartrimsh->alt_object_lines);
	    vartrimsh->alt_object_lines = DcNullPtr;
	} 

	vartrimsh->alt_object_lines =
		dor_vartrimsh_create_alternate_geom_lines(vartrimsh);

	if (vartrimsh->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_vartrimsh_update_lines", "");
	} else {
	    vartrimsh->decompose_lines = DcFalse;
	}
    }
    return vartrimsh->alt_object_lines;
}
/*
 ======================================================================
 */

PRAGMA(OPT_LEVEL 2)

void dor_vartrimsh_bounding_box (struct vartrimsh *vartrimsh)
{
    DtReal xmin, ymin, zmin, xmax, ymax, zmax;
    DtInt i, trino, vertexno;

    vertexno = vartrimsh->triangle_vertexlist[0][0];

    xmin = vartrimsh->vertex_location[vertexno][0];
    xmax = vartrimsh->vertex_location[vertexno][0];

    ymin = vartrimsh->vertex_location[vertexno][1];
    ymax = vartrimsh->vertex_location[vertexno][1];

    zmin = vartrimsh->vertex_location[vertexno][2];
    zmax = vartrimsh->vertex_location[vertexno][2];

PRAGMA(IVDEP)
PRAGMA(VBEST)

    /* use only the vertices that are referenced in the mesh */

    for (trino=0;trino<vartrimsh->triangle_count;trino++){
        for (i=0; i<3; i++) {
            vertexno = vartrimsh->triangle_vertexlist[trino][i];

            xmin = min(xmin,vartrimsh->vertex_location[vertexno][0]);
            xmax = max(xmax,vartrimsh->vertex_location[vertexno][0]);

            ymin = min(ymin,vartrimsh->vertex_location[vertexno][1]);
            ymax = max(ymax,vartrimsh->vertex_location[vertexno][1]);

            zmin = min(zmin,vartrimsh->vertex_location[vertexno][2]);
            zmax = max(zmax,vartrimsh->vertex_location[vertexno][2]);
        }
    }

    vartrimsh->bounding_box_pts[0][0] = xmin;
    vartrimsh->bounding_box_pts[0][1] = ymin;
    vartrimsh->bounding_box_pts[0][2] = zmin;

    vartrimsh->bounding_box_pts[1][0] = xmin;
    vartrimsh->bounding_box_pts[1][1] = ymax;
    vartrimsh->bounding_box_pts[1][2] = zmin;

    vartrimsh->bounding_box_pts[2][0] = xmax;
    vartrimsh->bounding_box_pts[2][1] = ymax;
    vartrimsh->bounding_box_pts[2][2] = zmin;

    vartrimsh->bounding_box_pts[3][0] = xmax;
    vartrimsh->bounding_box_pts[3][1] = ymin;
    vartrimsh->bounding_box_pts[3][2] = zmin;

    vartrimsh->bounding_box_pts[4][0] = xmin;
    vartrimsh->bounding_box_pts[4][1] = ymin;
    vartrimsh->bounding_box_pts[4][2] = zmax;

    vartrimsh->bounding_box_pts[5][0] = xmin;
    vartrimsh->bounding_box_pts[5][1] = ymax;
    vartrimsh->bounding_box_pts[5][2] = zmax;

    vartrimsh->bounding_box_pts[6][0] = xmax;
    vartrimsh->bounding_box_pts[6][1] = ymax;
    vartrimsh->bounding_box_pts[6][2] = zmax;

    vartrimsh->bounding_box_pts[7][0] = xmax;
    vartrimsh->bounding_box_pts[7][1] = ymin;
    vartrimsh->bounding_box_pts[7][2] = zmax;
}
/*
 ======================================================================
 */

dot_object *dor_vartrimsh_create_alternate_geom_lines (
    struct vartrimsh *vartrimsh)
{
    DtInt trino;
    DtInt vtxno;
    DtInt vtxcnt;
    DtInt vertex0, vertex1, vertex2;
    DtInt lvtx0, lvtx1;
    dot_object *varlinlstobj;

    vtxcnt = 0;

    if (vartrimsh->alt_linevtx == NULL) {

	vartrimsh->alt_linevtx = (DtRealTriple *)dor_space_allocate
		(6*vartrimsh->triangle_count*(sizeof (DtRealTriple)));

	if (vartrimsh->vertex_normal != NULL) {
	    vartrimsh->alt_linenrm = 
		    (DtRealTriple *)dor_space_allocate
		(6*vartrimsh->triangle_count*(sizeof (DtRealTriple)));
	}

	if (vartrimsh->vertex_color != NULL) {
	    vartrimsh->alt_lineclr = 
		    (DtRealTriple *)dor_space_allocate
			    (6*vartrimsh->triangle_count* (sizeof (DtRealTriple)));
	}
    }

    for (trino=0; trino<vartrimsh->triangle_count; trino++){

	/* Do not include degenerate triangles */
	vertex0 = vartrimsh->triangle_vertexlist[trino][0];
	vertex1 = vartrimsh->triangle_vertexlist[trino][1];
	vertex2 = vartrimsh->triangle_vertexlist[trino][2];

	if (!dor_check_degen_triangle_vtx_values(vartrimsh->vertex_location[vertex0],
					  vartrimsh->vertex_location[vertex1],
					  vartrimsh->vertex_location[vertex2])) 
		continue;

	/**/

	for (vtxno=0; vtxno<3; vtxno++) {

	    lvtx0 = vartrimsh->triangle_vertexlist[trino][vtxno];
	    if (vtxno==2)
		    lvtx1 = vartrimsh->triangle_vertexlist[trino][0];
	    else
		    lvtx1 = vartrimsh->triangle_vertexlist[trino][vtxno+1];

	    vartrimsh->alt_linevtx[vtxcnt][0] = 
		    vartrimsh->vertex_location[lvtx0][0];
	    vartrimsh->alt_linevtx[vtxcnt][1] = 
		    vartrimsh->vertex_location[lvtx0][1];
	    vartrimsh->alt_linevtx[vtxcnt][2] = 
		    vartrimsh->vertex_location[lvtx0][2];

	    if (vartrimsh->vertex_normal != NULL) {
		vartrimsh->alt_linenrm[vtxcnt][0] = 
			vartrimsh->vertex_normal[lvtx0][0];
		vartrimsh->alt_linenrm[vtxcnt][1] = 
			vartrimsh->vertex_normal[lvtx0][1];
		vartrimsh->alt_linenrm[vtxcnt][2] = 
			vartrimsh->vertex_normal[lvtx0][2];
	    }

	    if (vartrimsh->vertex_color != NULL) {
		vartrimsh->alt_lineclr[vtxcnt][0] = 
			vartrimsh->vertex_color[lvtx0][0];
		vartrimsh->alt_lineclr[vtxcnt][1] = 
			vartrimsh->vertex_color[lvtx0][1];
		vartrimsh->alt_lineclr[vtxcnt][2] = 
			vartrimsh->vertex_color[lvtx0][2];
	    }

	    vtxcnt++;

	    vartrimsh->alt_linevtx[vtxcnt][0] = 
		    vartrimsh->vertex_location[lvtx1][0];
	    vartrimsh->alt_linevtx[vtxcnt][1] = 
		    vartrimsh->vertex_location[lvtx1][1];
	    vartrimsh->alt_linevtx[vtxcnt][2] = 
		    vartrimsh->vertex_location[lvtx1][2];

	    if (vartrimsh->vertex_normal != NULL) {
		vartrimsh->alt_linenrm[vtxcnt][0] = 
			vartrimsh->vertex_normal[lvtx1][0];
		vartrimsh->alt_linenrm[vtxcnt][1] = 
			vartrimsh->vertex_normal[lvtx1][1];
		vartrimsh->alt_linenrm[vtxcnt][2] = 
			vartrimsh->vertex_normal[lvtx1][2];
	    }

	    if (vartrimsh->vertex_color != NULL) {
		vartrimsh->alt_lineclr[vtxcnt][0] = 
			vartrimsh->vertex_color[lvtx1][0];
		vartrimsh->alt_lineclr[vtxcnt][1] = 
			vartrimsh->vertex_color[lvtx1][1];
		vartrimsh->alt_lineclr[vtxcnt][2] = 
			vartrimsh->vertex_color[lvtx1][2];
	    }

	    vtxcnt++;
	}
    }

    varlinlstobj = dor_varlinlst_create(vartrimsh->colormodel, vtxcnt/2,
					(DtReal*)vartrimsh->alt_linevtx,
					(DtReal*)vartrimsh->alt_linenrm,
					(DtReal*)vartrimsh->alt_lineclr);
				
    return(varlinlstobj);
}
/*
 ======================================================================
 */

void dor_vartrimsh_calculate_triangle_normals (struct vartrimsh *vartrimsh)
{
    DtInt triangleno;
    DtInt vertex1, vertex2, vertex3;

    for (triangleno=0;triangleno<vartrimsh->triangle_count;triangleno++) {
	vertex1 = vartrimsh->triangle_vertexlist[triangleno][0];
	vertex2 = vartrimsh->triangle_vertexlist[triangleno][1];
	vertex3 = vartrimsh->triangle_vertexlist[triangleno][2];

	dor_math_fast_cross_cosines(vartrimsh->vertex_location[vertex1],
				    vartrimsh->vertex_location[vertex2],
				    vartrimsh->vertex_location[vertex3],
				    vartrimsh->element_normal[triangleno]);

	dor_math_renormalize_fast(vartrimsh->element_normal[triangleno]);
    }
}
/*
 ======================================================================
 */

void dor_vartrimsh_calculate_vertex_normals (struct vartrimsh *vartrimsh)
{
    DtFlag *vertexrefflag;
    DtInt vertexno;
    DtInt triangleno;
    DtInt trianglevertexno;

    /* Allocate a reference flag table for each vertex.  The reference
       flag tells us if the vertex belongs to any triangle, and 
       therefore whether its direction vector was computed and should 
       be renormalized.  */

    vertexrefflag = (DtFlag *)dor_space_allocate
			(vartrimsh->vertex_count*(sizeof (DtFlag)));

    /*  Zero the vertex normals and triangle reference flags for
	all vertices.  */

PRAGMA(ASIS)
    for (vertexno=0;vertexno<vartrimsh->vertex_count;vertexno++) {
	vertexrefflag[vertexno] = DcFalse;

	vartrimsh->vertex_normal[vertexno][0] = 0.;
	vartrimsh->vertex_normal[vertexno][1] = 0.;
	vartrimsh->vertex_normal[vertexno][2] = 0.;
    }

    /*  For each of the triangle's three vertices, update that vertex's
	triangle reference count and accumulated triangle normal.  */

PRAGMA(ASIS)
    for (triangleno=0;triangleno<vartrimsh->triangle_count;triangleno++) {
	for (trianglevertexno=0;trianglevertexno<3;trianglevertexno++) {
	    vertexno = 
		    vartrimsh->triangle_vertexlist[triangleno][trianglevertexno];

	    vertexrefflag[vertexno] = DcTrue;

	    vartrimsh->vertex_normal[vertexno][0] +=
		    vartrimsh->element_normal[triangleno][0];
	    vartrimsh->vertex_normal[vertexno][1] +=
		    vartrimsh->element_normal[triangleno][1];
	    vartrimsh->vertex_normal[vertexno][2] +=
		    vartrimsh->element_normal[triangleno][2];
	}
    }

    /*  Rescale the vertex normals.  */

PRAGMA(ASIS)
    for (vertexno=0;vertexno<vartrimsh->vertex_count;vertexno++) {
	if (vertexrefflag[vertexno]) {
	    if (!dor_math_renormalize_vector(vartrimsh->vertex_normal[vertexno])) {
		DDerror (ERR_ZERO_TRI_NRM_SUM,
			    "dor_vartrimsh_calculate_vertex_normals", 
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

void dor_vartrimsh_compute_bounding_volume (dot_object *object)
{
    struct vartrimsh *vartrimsh;

    vartrimsh = (struct vartrimsh *)(object->data);

    dor_system_report_bounding_points (8, (DtReal*)vartrimsh->bounding_box_pts);
}

/*
 ======================================================================
 */

void dor_vartrimsh_pick (dot_object *object)
{
    struct vartrimsh *vartrimsh;

    if (!dor_condex_query_pick(doe_vartrimsh_class_id))
	    return;

    if (!doe_system_alternate_object_execution_flag){
	dor_pick_initialize_object(object);
    }

    vartrimsh = (struct vartrimsh *)(object->data);

    dor_pick_triangle_mesh(vartrimsh->bounding_box_pts,
			   vartrimsh->triangle_count,
			   vartrimsh->vertex_count,
			   vartrimsh->vertex_location,
			   vartrimsh->triangle_vertexlist);
}
/*
 ======================================================================
 */

void dor_vartrimsh_update (
    dot_object *object,
    DtReal *vlocs,
    DtReal *vnrms,
    DtReal *vclrs,
    DtFlag recompute_nrms)
{
    struct vartrimsh *vartrimsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartrimsh_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleMesh",DcNullPtr);
	return;
    }   
#endif

    vartrimsh = (struct vartrimsh *)(object->data);

    vartrimsh->sequence_number++;
   
    if (vlocs != NULL) {
	vartrimsh->vertex_location = (DtRealTriple *)vlocs;

	dor_vartrimsh_bounding_box(vartrimsh);

	if (recompute_nrms) {
	    vartrimsh->needelemnrms = DcTrue;
	    if (vartrimsh->smoothflag) 
		    vartrimsh->needvertnrms = DcTrue;
	} else {
	    vartrimsh->needelemnrms = DcFalse;
	    vartrimsh->needvertnrms = DcFalse;
	}
	
    }


    /* If object was created without vertex normals then you cannot
       pass them in now */
	
    if (vnrms != NULL) {
	if ((vartrimsh->vertex_normal != NULL) && !vartrimsh->smoothflag) 
		vartrimsh->vertex_normal = (DtRealTriple *)vnrms;
	else
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_vartrimsh_update", 
			    "vertex normals ignored");
    }	


    /* If object was created without colors then you cannot 
       pass them in now */

    if (vclrs != NULL) {
	if (vartrimsh->vertex_color != NULL)
		vartrimsh->vertex_color = (DtColorRGB *)vclrs;
	else 
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_vartrimsh_update", 
			    "vertex colors ignored");
    }

    vartrimsh->decompose_lines = DcTrue;

}
/*
 ======================================================================
 */

void dor_vartrimsh_update_uv (
    dot_object *object,
    DtInt uvindex,
    DtRealCouple *uvlist)
{
    struct vartrimsh *vartrimsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartrimsh_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleMeshUV",DcNullPtr);
	return;
    }   
#endif

    vartrimsh = (struct vartrimsh *)(object->data);

#ifdef ERRCHK
    if (uvindex<0) {
          DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleMeshUV",
                        "uv index < 0");
          return;
    } else if (uvindex > vartrimsh->uv_count) {
          DDerror(ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleMeshUV",
                        "uv index too large");
          return;
    }
#endif

    vartrimsh->sequence_number++;
    if (vartrimsh->uv_count < uvindex+1) {
	vartrimsh->uv_count = uvindex+1;
    }
    vartrimsh->uv_list[uvindex] = uvlist;
}
/*
 ======================================================================
 */

void dor_vartrimsh_update_uvw (
    dot_object *object,
    DtInt uvwindex,
    DtRealTriple *uvwlist)
{
    struct vartrimsh *vartrimsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_vartrimsh_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarTriangleMeshUVW",DcNullPtr);
	return;
    }   
#endif

    vartrimsh = (struct vartrimsh *)(object->data);

#ifdef ERRCHK
    if (uvwindex<0) {
          DDerror (ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleMeshUVW",
                        "uvw index < 0");

          return;
    } else if (uvwindex > vartrimsh->uvw_count) {
          DDerror (ERR_VALUE_OUT_OF_RANGE,
                        "DpUpdVarTriangleMeshUVW",
                        "uvw index too large");
          return;
    }

#endif

    vartrimsh->sequence_number++;
    if (vartrimsh->uvw_count < uvwindex+1)
	    vartrimsh->uvw_count = uvwindex+1;
    vartrimsh->uvw_list[uvwindex] = uvwlist;   
}
