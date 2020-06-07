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
	dor_varsimplymsh_initialize
	dor_varsimplymsh_create
	dor_varsimplymsh_destroy
	dor_varsimplymsh_pick
	dor_varsimplymsh_pick_lines
	dor_varsimplymsh_pick_tris
	dor_varsimplymsh_print
	dor_varsimplymsh_render_standard
	dor_varsimplymsh_update_alternate
	dor_varsimplymsh_update_lines
	dor_varsimplymsh_update_tris
	dor_varsimplymsh_create_alternate_geometry_complex_triangle
	dor_varsimplymsh_create_alternate_geom_lines
	dor_varsimplymsh_create_alternate_geometry_simple_triangle
	dor_varsimplymsh_create_alternate_geom_tris
	dor_varsimplymsh_calculate_vertex_normals
	dor_varsimplymsh_compute_bounding_volume
	dor_varsimplymsh_enable_edges_simple
	dor_varsimplymsh_polygon_normal
	dor_varsimplymsh_update
	dor_varsimplymsh_update_uv
	dor_varsimplymsh_update_uvw

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/varsplymsh.h>
#include <dore/dore_develop/private/vartrimsh.h>
#include <dore/dore_develop/private/varlinlst.h>

DtInt doe_varsimplymsh_polygonno;
DtInt doe_varsimplymsh_class_id;
/*
 ======================================================================
 */

static DtMethodEntry varsimplymsh_methods[] = {
    { DcMethodCmpBndVolume,
	    (DtMethodPtr) dor_varsimplymsh_compute_bounding_volume
    },
    { DcMethodDestroy,
	    (DtMethodPtr) dor_varsimplymsh_destroy
    },
    { DcMethodPick,
	    (DtMethodPtr) dor_varsimplymsh_pick
    },
    { DcMethodPrint,
	    (DtMethodPtr) dor_varsimplymsh_print
    },
    { DcMethodStdRenderDisplay,
	    (DtMethodPtr) dor_varsimplymsh_render_standard
    },
    { DcMethodUpdStdAltObj,
	    (DtMethodPtr) dor_varsimplymsh_update_alternate
    },
    { DcMethodUpdStdAltLineObj,
	    (DtMethodPtr) dor_varsimplymsh_update_lines
    }
};

void dor_varsimplymsh_initialize (void)
{
    doe_varsimplymsh_class_id = dor_class_add ("DoVarSimplePolygonMesh", 
				7, varsimplymsh_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_varsimplymsh_create (
    DtColorModel colormodel,
    DtInt vertexcount,
    DtReal *vertloc,
    DtReal *vertnrm,
    DtReal *vertclr,
    DtInt polygoncount,
    DtInt contours[],
    DtInt vertexlist[],
    DtShapeType shape,
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct varsimplymsh *varsimplymsh;
   DtInt polyvertexcount;
   DtInt polygonno;
   DtInt vertexno;
   DtInt i;
   dot_object *varsimplymshobject;

   if (!initialized) {
      dor_varsimplymsh_initialize();
      initialized = DcTrue;
   }

   if (shape == DcComplex) {
	DDerror (ERR_VALUE_OUT_OF_RANGE,
                   "dor_varsimplymsh_create", 
                   "shape must not be DcComplex - Null object returned");
	return((DtObject)dor_null_create(DcNullPtr));
   }

   varsimplymsh = (struct varsimplymsh *)dor_space_allocate(
						sizeof *varsimplymsh);

   varsimplymsh->colormodel = colormodel;
   varsimplymsh->polygon_count = polygoncount;

   varsimplymsh->contours =
	(DtInt *)dor_space_allocate((polygoncount)*sizeof(DtInt));

   polyvertexcount = 0;

   for (polygonno=0;polygonno<polygoncount;polygonno++) {
	varsimplymsh->contours[polygonno] = contours[polygonno];
	polyvertexcount += contours[polygonno];
   }

   varsimplymsh->vertexlist =
	(DtInt *)dor_space_allocate((polyvertexcount)*sizeof(DtInt));

   for (vertexno=0;vertexno<polyvertexcount;vertexno++) {
		varsimplymsh->vertexlist[vertexno] = vertexlist[vertexno];
   }

   varsimplymsh->vertex_count = vertexcount;

   varsimplymsh->vertex_location = (DtRealTriple *)vertloc;
   varsimplymsh->vertex_normal = (DtRealTriple *)vertnrm;
   varsimplymsh->vertex_color = (DtColorRGB *)vertclr;

   varsimplymsh->shape = shape;
   varsimplymsh->smoothflag = smoothflag;

   if (smoothflag) {
	if (vertnrm == NULL) {
		varsimplymsh->needvertnrms = DcTrue;
	} else {
		/* If we got vertex normal from the user never
                mess with them */
                varsimplymsh->smoothflag = DcFalse;
                varsimplymsh->needvertnrms = DcFalse;
	}
   } else {
   	varsimplymsh->needvertnrms = DcFalse;
   }

   varsimplymsh->alt_object_lines = NULL;
   varsimplymsh->decompose_lines = DcTrue;
   varsimplymsh->alt_linevtx = NULL;
   varsimplymsh->alt_linenrm = NULL;
   varsimplymsh->alt_lineclr = NULL;

   varsimplymsh->alt_object_tris = NULL;
   varsimplymsh->decompose_tris = DcTrue;
   varsimplymsh->alt_trilist = NULL;
   varsimplymsh->alt_tricount = 0;

   varsimplymsh->uv_count = 0;
   varsimplymsh->uv_list = (DtRealCouple **)dor_space_allocate(
                dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   for (i=0; i<dod_system_max_vertex_uv_cnt; i++) {
	varsimplymsh->uv_list[i] = NULL;
   }
   varsimplymsh->uvw_count = 0;
   varsimplymsh->uvw_list = (DtRealTriple **)dor_space_allocate(
                dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));
   for (i=0; i<dod_system_max_vertex_uvw_cnt; i++) {
	varsimplymsh->uvw_list[i] = NULL;
   }

   /* The edge_able field is used internally only for DcSurfaceOutlineEdges */
   varsimplymsh->edge_enable = NULL;

   varsimplymshobject = dor_object_create(doe_varsimplymsh_class_id,
					varsimplymsh);

   return(varsimplymshobject);
}
/*
 ======================================================================
 */

void dor_varsimplymsh_destroy (dot_object *object)
{
    struct varsimplymsh *varsimplymsh;

    varsimplymsh = (struct varsimplymsh *)(object->data);

    if (varsimplymsh->smoothflag && 
	(varsimplymsh->vertex_normal != NULL)) {
	dor_space_deallocate(varsimplymsh->vertex_normal);
    }

    if (varsimplymsh->contours != NULL) {
	dor_space_deallocate(varsimplymsh->contours);
    }

    if (varsimplymsh->vertexlist != NULL) {
	dor_space_deallocate(varsimplymsh->vertexlist);
    }

    if (varsimplymsh->alt_object_lines != NULL) {
	dor_object_check_deletion(varsimplymsh->alt_object_lines);
    }

    if (varsimplymsh->alt_object_tris != NULL) {
	dor_object_check_deletion(varsimplymsh->alt_object_tris);
    }

    if (varsimplymsh->alt_trilist != NULL) {
	dor_space_deallocate(varsimplymsh->alt_trilist);
    }

    if (varsimplymsh->alt_linevtx != NULL) {
	dor_space_deallocate(varsimplymsh->alt_linevtx);
    }

    if (varsimplymsh->alt_linenrm != NULL) {
	dor_space_deallocate(varsimplymsh->alt_linenrm);
    }

    if (varsimplymsh->alt_lineclr != NULL) {
	dor_space_deallocate(varsimplymsh->alt_lineclr);
    }

    if (varsimplymsh->edge_enable != NULL) {
	dor_space_deallocate(varsimplymsh->edge_enable);
    }

    dor_space_deallocate (varsimplymsh->uv_list);
    dor_space_deallocate (varsimplymsh->uvw_list);

    dor_space_deallocate(varsimplymsh);
}
/*
 ======================================================================
 */

void dor_varsimplymsh_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_varsimplymsh_class_id))
	    return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
	dor_varsimplymsh_pick_lines(object);
	break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
	dor_varsimplymsh_pick_tris(object);
	break;
    }
}

/*
 ======================================================================
 */

void dor_varsimplymsh_pick_lines (dot_object *object)
{
    struct varsimplymsh *varsimplymsh;

    varsimplymsh = (struct varsimplymsh *)(object->data);

    if (varsimplymsh->decompose_lines) {
   	if (varsimplymsh->alt_object_lines != DcNullPtr) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(varsimplymsh->alt_object_lines,
				  DcMethodDestroy)(varsimplymsh->alt_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    varsimplymsh->alt_object_lines = NULL;
   	} 

   	varsimplymsh->alt_object_lines =
		dor_varsimplymsh_create_alternate_geom_lines(varsimplymsh);

   	if (varsimplymsh->alt_object_lines == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_varsimplymsh_pick_lines", "");
	    return;
   	}

	varsimplymsh->decompose_lines = DcFalse;
    }

    doe_system_alternate_object_execution_flag = DcTrue;

    dor_object_get_method(varsimplymsh->alt_object_lines,
			  DcMethodPick)(varsimplymsh->alt_object_lines);

    doe_system_alternate_object_execution_flag = DcFalse;

    return;
}

/*
 ======================================================================
 */

void dor_varsimplymsh_pick_tris (dot_object *object)
{
    struct varsimplymsh *varsimplymsh;

    varsimplymsh = (struct varsimplymsh *)(object->data);

    if (varsimplymsh->decompose_tris){
	if (varsimplymsh->alt_object_tris != NULL) {
	    doe_system_alternate_object_execution_flag = DcTrue;
 
	    dor_object_get_method(varsimplymsh->alt_object_tris,
				  DcMethodDestroy)(varsimplymsh->alt_object_tris);
 
	    doe_system_alternate_object_execution_flag = DcFalse;
 
	    varsimplymsh->alt_object_tris = NULL;
        }
 
	varsimplymsh->alt_object_tris =
		dor_varsimplymsh_create_alternate_geom_tris(varsimplymsh);
 
        if (varsimplymsh->alt_object_tris == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_varsimplymsh_pick_tris", ""); 
	    return;
        }

	varsimplymsh->decompose_tris = DcFalse;
    }

    doe_system_alternate_object_execution_flag = DcTrue;

    dor_object_get_method(varsimplymsh->alt_object_tris,
			  DcMethodPick)(varsimplymsh->alt_object_tris);

    doe_system_alternate_object_execution_flag = DcFalse;

    return;
}

/*
 ======================================================================
 */

void dor_varsimplymsh_print (struct object *object)
{
    struct varsimplymsh *varsimplymsh;

    varsimplymsh = (struct varsimplymsh *)(object->data);

    if (!varsimplymsh) {
	dor_print_output ("variable simple polygon mesh is NULL");
	return;
    }

    switch (varsimplymsh->colormodel){
	case DcRGB:
	    dor_print_output("colormodel = DcRGB");
	    break;

	default:
	    dor_print_output("colormodel = UNKNOWN");
	    break;
    }

    switch (varsimplymsh->shape){
	case DcConvex:   dor_print_output ("shape = convex" );  break;
	case DcConcave:  dor_print_output ("shape = concave");  break;
	case DcComplex:  dor_print_output ("shape = complex");  break;
    }

    sprintf(dor_print_get_line(),
	    "vertex locations at [%lx]",varsimplymsh->vertex_location);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex normals at [%lx]", varsimplymsh->vertex_normal);
    dor_print_output(dor_print_get_line());
 
    sprintf(dor_print_get_line(),
	    "vertex colors at [%lx]", varsimplymsh->vertex_color);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "smooth flag = %d",varsimplymsh->smoothflag);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex count = %d",varsimplymsh->vertex_count);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "polygon count = %d",varsimplymsh->polygon_count);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

void dor_varsimplymsh_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_varsimplymsh_class_id))
	    return;

    alt = dor_varsimplymsh_update_alternate (object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
/*
 ======================================================================
 */

dot_object *dor_varsimplymsh_update_alternate (dot_object *object)
{
    DtRepType reptype;
    dot_object *alt;

    dor_global_reptyp_get_value(&reptype);
 
    switch(reptype) {
    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
	alt = dor_varsimplymsh_update_tris(object);
	break;

    case DcOutlines:
	alt = dor_varsimplymsh_update_lines(object);
	break;
    default:
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_varsimplymsh_update_alternate",
		   "unknown reptype"); 
	alt = DcNullPtr;
	break;
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_varsimplymsh_update_lines (dot_object *object)
{
    struct varsimplymsh *varsimplymsh;

    varsimplymsh = (struct varsimplymsh *)(object->data);

    if (varsimplymsh->decompose_lines) {
	if (varsimplymsh->alt_object_lines != DcNullPtr) {
	    dor_object_get_method(varsimplymsh->alt_object_lines,
				  DcMethodDestroy)(varsimplymsh->alt_object_lines);
	    varsimplymsh->alt_object_lines = DcNullPtr;
	} 

	varsimplymsh->alt_object_lines =
		dor_varsimplymsh_create_alternate_geom_lines(varsimplymsh);

	if (varsimplymsh->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_varsimplymsh_update_lines", "");
	} else {
	    varsimplymsh->decompose_lines = DcFalse;
	}
    }
    return varsimplymsh->alt_object_lines;
}

/*
 ======================================================================
 */

dot_object *dor_varsimplymsh_update_tris (dot_object *object)
{
    struct varsimplymsh *varsimplymsh;

    varsimplymsh = (struct varsimplymsh *)(object->data);

    if (varsimplymsh->needvertnrms) {
	if (varsimplymsh->vertex_normal == NULL) {
	    varsimplymsh->vertex_normal =
		    (DtRealTriple *)dor_space_allocate(
				varsimplymsh->vertex_count*(sizeof (DtRealTriple)));
	}
	dor_varsimplymsh_calculate_vertex_normals(varsimplymsh);
	varsimplymsh->needvertnrms = DcFalse;
    }

    if (varsimplymsh->decompose_tris) {
	if (varsimplymsh->alt_object_tris != NULL) {

	    dor_object_get_method(varsimplymsh->alt_object_tris,
				  DcMethodDestroy)(varsimplymsh->alt_object_tris);
	    varsimplymsh->alt_object_tris = NULL;
	}

	varsimplymsh->alt_object_tris =
		dor_varsimplymsh_create_alternate_geom_tris(varsimplymsh);

	if (varsimplymsh->alt_object_tris == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_varsimplymsh_update_tris", "");
        } else {
	    varsimplymsh->decompose_tris = DcFalse;
        }
    }

    return varsimplymsh->alt_object_tris;
}
/*
 ======================================================================
 */

void dor_varsimplymsh_create_alternate_geometry_complex_triangle (
    struct varsimplymsh *varsimplymsh,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DDerror(ERR_VALUE_OUT_OF_RANGE,
	"dor_varsimplymsh_create_alternate_geometry_complex_triangle",
	"polygon must not be DcComplex");
}
/*
 ======================================================================
 */

dot_object *dor_varsimplymsh_create_alternate_geom_lines (
    struct varsimplymsh *varsimplymsh)
{
    DtInt vertexlistloc;
    DtInt polygonno;
    DtInt polyvertexcnt;
    DtInt polyvertexno;
    DtInt firstvertex, lastvertex;
    DtInt linevcnt;
    DtInt lvtx0, lvtx1;
    DtUInt i;
    DtUInt count;
    dot_object *varlinlstobj;

    vertexlistloc = 0;
    linevcnt = 0;

    if (varsimplymsh->alt_linevtx == NULL) {
	count = 0;
	for (i=0; i<varsimplymsh->polygon_count; i++) 
		count += varsimplymsh->contours[i];

	varsimplymsh->alt_linevtx = (DtRealTriple *)dor_space_allocate(
						2*count*(sizeof (DtRealTriple)));

	if (varsimplymsh->vertex_normal != NULL) {
	    varsimplymsh->alt_linenrm = 
		    (DtRealTriple *)dor_space_allocate(
						2*count*(sizeof (DtRealTriple)));
	}

	if (varsimplymsh->vertex_color != NULL) {
	    varsimplymsh->alt_lineclr = 
		    (DtRealTriple *)dor_space_allocate(
						2*count*(sizeof (DtRealTriple)));
	}
    }

    for (polygonno=0;polygonno<varsimplymsh->polygon_count;polygonno++){

	polyvertexcnt = varsimplymsh->contours[polygonno];
	firstvertex = varsimplymsh->vertexlist[vertexlistloc];
	lastvertex = varsimplymsh->vertexlist[vertexlistloc+polyvertexcnt-1];

	for (polyvertexno=0;
	     polyvertexno<polyvertexcnt;
	     polyvertexno++) {

	    if (polyvertexno == polyvertexcnt-1) {
		lvtx0 = lastvertex;
		lvtx1 = firstvertex;
	    } else {
		lvtx0 = varsimplymsh->vertexlist[vertexlistloc++];
		lvtx1 = varsimplymsh->vertexlist[vertexlistloc];
	    }

	    varsimplymsh->alt_linevtx[linevcnt][0] = 
		    varsimplymsh->vertex_location[lvtx0][0];
	    varsimplymsh->alt_linevtx[linevcnt][1] = 
		    varsimplymsh->vertex_location[lvtx0][1];
	    varsimplymsh->alt_linevtx[linevcnt][2] = 
		    varsimplymsh->vertex_location[lvtx0][2];

	    if (varsimplymsh->vertex_normal != NULL) {
		varsimplymsh->alt_linenrm[linevcnt][0] = 
			varsimplymsh->vertex_normal[lvtx0][0];
		varsimplymsh->alt_linenrm[linevcnt][1] = 
			varsimplymsh->vertex_normal[lvtx0][1];
		varsimplymsh->alt_linenrm[linevcnt][2] = 
			varsimplymsh->vertex_normal[lvtx0][2];
	    }

	    if (varsimplymsh->vertex_color != NULL) {
		varsimplymsh->alt_lineclr[linevcnt][0] = 
			varsimplymsh->vertex_color[lvtx0][0];
		varsimplymsh->alt_lineclr[linevcnt][1] = 
			varsimplymsh->vertex_color[lvtx0][1];
		varsimplymsh->alt_lineclr[linevcnt][2] = 
			varsimplymsh->vertex_color[lvtx0][2];
	    }

	    linevcnt++;

	    varsimplymsh->alt_linevtx[linevcnt][0] = 
		    varsimplymsh->vertex_location[lvtx1][0];
	    varsimplymsh->alt_linevtx[linevcnt][1] = 
		    varsimplymsh->vertex_location[lvtx1][1];
	    varsimplymsh->alt_linevtx[linevcnt][2] = 
		    varsimplymsh->vertex_location[lvtx1][2];

	    if (varsimplymsh->vertex_normal != NULL) {
		varsimplymsh->alt_linenrm[linevcnt][0] = 
			varsimplymsh->vertex_normal[lvtx1][0];
		varsimplymsh->alt_linenrm[linevcnt][1] = 
			varsimplymsh->vertex_normal[lvtx1][1];
		varsimplymsh->alt_linenrm[linevcnt][2] = 
			varsimplymsh->vertex_normal[lvtx1][2];
	    }

	    if (varsimplymsh->vertex_color != NULL) {
		varsimplymsh->alt_lineclr[linevcnt][0] = 
			varsimplymsh->vertex_color[lvtx1][0];
		varsimplymsh->alt_lineclr[linevcnt][1] = 
			varsimplymsh->vertex_color[lvtx1][1];
		varsimplymsh->alt_lineclr[linevcnt][2] = 
			varsimplymsh->vertex_color[lvtx1][2];
	    }

	    linevcnt++;
	}

	vertexlistloc++;
    }

    varlinlstobj = dor_varlinlst_create(varsimplymsh->colormodel,
					linevcnt/2,
					(DtReal*)varsimplymsh->alt_linevtx,
					(DtReal*)varsimplymsh->alt_linenrm,
					(DtReal*)varsimplymsh->alt_lineclr);
				
    return(varlinlstobj);
}
/*
 ======================================================================
 */

void dor_varsimplymsh_create_alternate_geometry_simple_triangle (
    struct varsimplymsh *varsimplymsh,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
	DtInt trino;

	trino = varsimplymsh->alt_tricount;

	varsimplymsh->alt_trilist[trino][0] = vertex1; 
	varsimplymsh->alt_trilist[trino][1] = vertex2; 
	varsimplymsh->alt_trilist[trino][2] = vertex3; 

	varsimplymsh->edge_enable[trino] = dor_varsimplymsh_enable_edges_simple(
			varsimplymsh, vertex1,vertex2,vertex3);

#ifdef DEBUG
    printf ("edge_enable = %d\n", varsimplymsh->edge_enable[trino]);
#endif

	varsimplymsh->alt_tricount++;

}

/*
 ======================================================================
 */

dot_object *dor_varsimplymsh_create_alternate_geom_tris (
    struct varsimplymsh *varsimplymsh)
{
    dot_object *vartrimshobj;
    DtInt count;
    DtInt i;
    DtInt vertexlistloc;
    DtInt polygonno;
    DtInt vertexno;
    DtInt contourvertexcount;
    DtInt contourvertexno;

    if (varsimplymsh->alt_trilist == NULL) {
	/* Determine the total number of vertices for all polygons. */
   	count = 0;
   	for (i=0; i<varsimplymsh->polygon_count; i++)
		count += varsimplymsh->contours[i];

	varsimplymsh->alt_trilist = (DtIntTriple *)dor_space_allocate(
						count*(sizeof (DtIntTriple)));
	varsimplymsh->edge_enable = (DtInt *)dor_space_allocate(
						count*(sizeof (DtInt)));
    }

    varsimplymsh->alt_tricount = 0;

    vertexlistloc = 0;

    for (polygonno = 0; polygonno < varsimplymsh->polygon_count; polygonno++) {
	dor_gpolyd_create_polygon();
	dor_gpolyd_create_contour();
	contourvertexcount = varsimplymsh->contours[polygonno];

	for (contourvertexno=0; contourvertexno<contourvertexcount;
	     contourvertexno++) {
	    vertexno = varsimplymsh->vertexlist[vertexlistloc++];

	    dor_gpolyd_create_contour_vertex(vertexno,
				varsimplymsh->vertex_location[vertexno][0],
				varsimplymsh->vertex_location[vertexno][1],
				varsimplymsh->vertex_location[vertexno][2]);
	}

	dor_gpolyd_close_contour();
	dor_gpolyd_close_polygon();

	doe_varsimplymsh_polygonno = polygonno;
	dor_gpolyd_triangulate_polygon(varsimplymsh->shape, 
			dor_varsimplymsh_create_alternate_geometry_simple_triangle,
			dor_varsimplymsh_create_alternate_geometry_complex_triangle,
			varsimplymsh);
    }

    /* Alternate geometry is a vartrimsh (with edges). */
    vartrimshobj = dor_vartrimsh_create_with_edges(
					varsimplymsh->colormodel,
					varsimplymsh->vertex_count,
					(DtReal*)varsimplymsh->vertex_location,
					(DtReal*)varsimplymsh->vertex_normal,
					(DtReal*)varsimplymsh->vertex_color,
					varsimplymsh->alt_tricount,
					varsimplymsh->alt_trilist,
					DcFalse,
					varsimplymsh->edge_enable);

    if (vartrimshobj != DcNullObject) {

        /* copy varsimplymsh'uv and uvw's into vartrimsh */
	for (i=0; i<varsimplymsh->uv_count; i++) 
	    dor_vartrimsh_update_uv(vartrimshobj, i,
					varsimplymsh->uv_list[i]);

	for (i=0; i<varsimplymsh->uvw_count; i++) 
	    dor_vartrimsh_update_uvw(vartrimshobj, i,
					 varsimplymsh->uvw_list[i]);
    }

    return(vartrimshobj);
}

/*
 ======================================================================
 */

void dor_varsimplymsh_calculate_vertex_normals (
    struct varsimplymsh *varsimplymsh)
{
    DtFlag *vertexrefflag;
    DtInt vertexno;
    DtInt polygonno;
    DtInt vertexlistloc;
    DtInt polygonvertexno;
    DtRealTriple polygonnormal;

    /* Allocate a table of polygon reference flags, one for each vertex */
   
    vertexrefflag = (DtFlag *)dor_space_allocate(
				varsimplymsh->vertex_count*(sizeof (DtFlag)));

    /*  Zero the vertex normals and polygon reference flags for
	all vertices.  */

    for (vertexno=0;vertexno<varsimplymsh->vertex_count;vertexno++) {
	vertexrefflag[vertexno] = DcFalse;

	varsimplymsh->vertex_normal[vertexno][0] = 0.;
	varsimplymsh->vertex_normal[vertexno][1] = 0.;
	varsimplymsh->vertex_normal[vertexno][2] = 0.;
    }
   
    vertexlistloc = 0;

    for (polygonno=0;polygonno<varsimplymsh->polygon_count;polygonno++) {
	dor_varsimplymsh_polygon_normal(varsimplymsh,vertexlistloc,
			varsimplymsh->contours[polygonno],polygonnormal);

	/*  For each of the polygon's vertices, update that vertex's
	    polygon reference count and accumulated polygon normal.  This
	    also bumps the vertexlistloc value to the first vertex of the
	    next polygon.  */

	for (polygonvertexno=0;
	     polygonvertexno<varsimplymsh->contours[polygonno];
	     polygonvertexno++) {
	    vertexno = varsimplymsh->vertexlist[vertexlistloc++];

	    vertexrefflag[vertexno] = DcTrue;

	    varsimplymsh->vertex_normal[vertexno][0] += 
		    polygonnormal[0];
	    varsimplymsh->vertex_normal[vertexno][1] += 
		    polygonnormal[1];
	    varsimplymsh->vertex_normal[vertexno][2] += 
		    polygonnormal[2];
	}
    }

    /*  Rescale the vertex normals.  */

    for (vertexno=0;vertexno<varsimplymsh->vertex_count;vertexno++) {
	if (vertexrefflag[vertexno]) {
	      if (!dor_math_renormalize_vector(
	      varsimplymsh->vertex_normal[vertexno])) {
	      DDerror (ERR_ZERO_TRI_NRM_SUM,
	      "dor_varsimplymsh_calculate_vertex_normals", DcNullPtr);
	      }
	}
    }
    /*  Free the vertex reference flags.  */
    dor_space_deallocate(vertexrefflag);
}

/*
 ======================================================================
 */

void dor_varsimplymsh_compute_bounding_volume (dot_object *object)
{
    struct varsimplymsh *varsimplymsh;

    varsimplymsh = (struct varsimplymsh *)(object->data);

    dor_system_report_bounding_points
	(varsimplymsh->vertex_count, (DtReal*)varsimplymsh->vertex_location);
}

/*
 ======================================================================
 */

DtInt dor_varsimplymsh_enable_edges_simple (
    struct varsimplymsh *varsimplymsh,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
    DtInt edge_enable;
    DtInt vertexcount;
    static DtInt *vtx1_no = (DtInt *)0;
    static DtInt *vtx2_no = (DtInt *)0;
    static DtInt *vtx3_no = (DtInt *)0;
    static DtInt vtx_space = 0;
    DtInt vtx1_no_count, vtx2_no_count, vtx3_no_count;
    DtInt polygonstart;
    DtInt mshvertex;
    DtInt i,j;

#ifdef DEBUG
    printf("doe_varsimplymsh_polygonno = %d\n", doe_varsimplymsh_polygonno);
#endif

    vertexcount = varsimplymsh->contours[doe_varsimplymsh_polygonno];
    if (vertexcount > vtx_space) {
        vtx1_no = (DtInt *)dor_space_reallocate(vtx1_no, 
			vertexcount*(sizeof(DtInt)));
        vtx2_no = (DtInt *)dor_space_reallocate(vtx2_no, 
			vertexcount*(sizeof(DtInt)));
        vtx3_no = (DtInt *)dor_space_reallocate(vtx3_no, 
			vertexcount*(sizeof(DtInt)));
        vtx_space = vertexcount;
    }

    polygonstart = 0;
    for (i=0; i<doe_varsimplymsh_polygonno; i++) {
	polygonstart += varsimplymsh->contours[i];
    }
#ifdef DEBUG
    printf("polygonstart = %d\n", polygonstart);
#endif
   
    vtx1_no_count = 0; 
    vtx2_no_count = 0; 
    vtx3_no_count = 0; 
    for (i=0; i<vertexcount; i++) {
	mshvertex = varsimplymsh->vertexlist[polygonstart+i];
	if (vertex1 == mshvertex) {
	    vtx1_no[vtx1_no_count++] = i;
	}
	if (vertex2 == mshvertex) {
	    vtx2_no[vtx2_no_count++] = i;
	}
	if (vertex3 == mshvertex) {
	    vtx3_no[vtx3_no_count++] = i;
	}
    }

#ifdef DEBUG
    for (i=0; i<vtx1_no_count; i++)
	    printf("vtx1_no[%d] %d    ", i, vtx1_no[i]);
    printf("\n");
    for (i=0; i<vtx2_no_count; i++)
	    printf("vtx2_no[%d] %d    ", i, vtx2_no[i]);
    printf("\n");
    for (i=0; i<vtx3_no_count; i++)
	    printf("vtx3_no[%d] %d    ", i, vtx3_no[i]);
    printf("\n");
#endif
    edge_enable = dod_no_enabled_edges;

    for (i=0; i<vtx1_no_count; i++) {
   	for (j=0; j<vtx2_no_count; j++) {
	    if (vtx1_no[i] == ((vtx2_no[j]+1) % vertexcount) ||
		vtx2_no[j] == ((vtx1_no[i]+1) % vertexcount)) {
		edge_enable |= dod_edge1_enabled;
		break;
	    }
	}
   	for (j=0; j<vtx3_no_count; j++) {
	    if (vtx1_no[i] == ((vtx3_no[j]+1) % vertexcount) ||
		vtx3_no[j] == ((vtx1_no[i]+1) % vertexcount)) {
		edge_enable |= dod_edge3_enabled;
		break;
	    }
	}
    }
    for (i=0; i<vtx2_no_count; i++) {
   	for (j=0; j<vtx3_no_count; j++) {
	    if (vtx2_no[i] == ((vtx3_no[j]+1) % vertexcount) ||
		vtx3_no[j] == ((vtx2_no[i]+1) % vertexcount)) {
		edge_enable |= dod_edge2_enabled;
		break;
	    }
	}
    }

#ifdef DEBUG
    printf("edge_enable = %d\n", edge_enable);
#endif

    return (edge_enable);
}
/*
 ======================================================================
 */

void dor_varsimplymsh_polygon_normal (
    struct varsimplymsh *varsimplymsh,
    DtInt initialvertexlistloc,
    DtInt polygonvertexcount,
    DtRealTriple polygonnormal)

{
   DtInt vertexlistloc;
   DtInt polygonvertexno;
   DtInt vertexno;
   DtRealTriple vertices[200];

   vertexlistloc = initialvertexlistloc;

   for (polygonvertexno=0; polygonvertexno<polygonvertexcount;
	polygonvertexno++) {

	vertexno = varsimplymsh->vertexlist[vertexlistloc++];

	vertices[polygonvertexno][0] = 
			varsimplymsh->vertex_location[vertexno][0];
	vertices[polygonvertexno][1] = 
			varsimplymsh->vertex_location[vertexno][1];
	vertices[polygonvertexno][2] = 
			varsimplymsh->vertex_location[vertexno][2];
   }

   dor_math_polygon_normal_fast(
			polygonvertexcount,vertices,polygonnormal);
}
/*
 ======================================================================
 */

void dor_varsimplymsh_update (
    dot_object *object,
    DtReal *vlocs,
    DtReal *vnrms,
    DtReal *vclrs,
    DtShapeType shape,
    DtFlag decompose,
    DtFlag recompute_nrms)
{
    struct varsimplymsh *varsimplymsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) ||
	(((type = dor_object_inq_type(object)) != doe_varsimplymsh_class_id))) {
	DDerror(ERR_BAD_OBJECT, "DpUpdVarSimplePolygonMesh",DcNullPtr);
	return;
    }
#endif

    varsimplymsh = (struct varsimplymsh *)(object->data);

    varsimplymsh->sequence_number++;
   
    if (vlocs != NULL) {
	varsimplymsh->vertex_location = (DtRealTriple *)vlocs;

	if (recompute_nrms && varsimplymsh->smoothflag) 
		varsimplymsh->needvertnrms = DcTrue;
	else
		varsimplymsh->needvertnrms = DcFalse;
    }


    /* If object was created without vertex normals then you cannot
       pass them in now */
	
    if (vnrms != NULL) {
	if ((varsimplymsh->vertex_normal != NULL) && !varsimplymsh->smoothflag) 
		varsimplymsh->vertex_normal = (DtRealTriple *)vnrms;
	else
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varsimplymsh_update",
			    "vertex normals ignored");
    }	


    /* If object was created without colors then you cannot 
       pass them in now */

    if (vclrs != NULL) {
	if (varsimplymsh->vertex_color != NULL)
		varsimplymsh->vertex_color = (DtColorRGB *)vclrs;
	else 
		DDerror (ERR_VALUE_OUT_OF_RANGE,
			    "dor_varsimplymsh_update", 
			    "vertex colors ignored");
    }

    varsimplymsh->shape = shape;

    varsimplymsh->decompose_tris = decompose;

    if (!decompose && (varsimplymsh->alt_object_tris != NULL))
	    dor_vartrimsh_update(varsimplymsh->alt_object_tris,
				 vlocs, vnrms, vclrs, DcFalse);

    varsimplymsh->decompose_lines = DcTrue;
}
/*
 ======================================================================
 */

void dor_varsimplymsh_update_uv (
    dot_object *object,
    DtInt uvindex,
    DtRealCouple *uvlist)
{
    struct varsimplymsh *varsimplymsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varsimplymsh_class_id))) {
        DDerror(ERR_BAD_OBJECT, 
		   "DpUpdVarSimplePolygonMeshUV",
		   DcNullPtr);
        return;
    }   
#endif

    varsimplymsh = (struct varsimplymsh *)(object->data);

#ifdef ERRCHK
    if (uvindex < 0) {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "DpUpdVarSimplePolygonMeshUV",
		   "uv index < 0");
	return;
    } else if (uvindex > varsimplymsh->uv_count) {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "DpUpdVarSimplePolygonMeshUV",
		   "uv index too large");
	return;
    }
#endif

    varsimplymsh->sequence_number++;
    if (varsimplymsh->uv_count < uvindex+1) {
	varsimplymsh->uv_count = uvindex+1;
    }
    varsimplymsh->uv_list[uvindex] = uvlist;

    /* update the uv's of the underlying vartrimsh - if it exists */
    if (varsimplymsh->alt_object_tris != NULL) 
        dor_vartrimsh_update_uv(varsimplymsh->alt_object_tris,
                                uvindex, uvlist);
}
/*
 ======================================================================
 */

void dor_varsimplymsh_update_uvw (
    dot_object *object,
    DtInt uvwindex,
    DtRealTriple *uvwlist)
{
    struct varsimplymsh *varsimplymsh;

#ifdef ERRCHK
    DtInt type;
 
    if ((DcObjectValid != dor_object_validate(object)) || 
	(((type = dor_object_inq_type(object)) != doe_varsimplymsh_class_id))) {
        DDerror(ERR_BAD_OBJECT, 
		   "DpUpdVarSimplePolygonMeshUVW",
		   DcNullPtr);
        return;
    }   
#endif

    varsimplymsh = (struct varsimplymsh *)(object->data);

#ifdef ERRCHK
    if (uvwindex < 0) {
	DDerror(ERR_VALUE_OUT_OF_RANGE, 
		   "DpUpdVarSimplePolygonMeshUVW",
		   "uvw index < 0");
	return;
    } else if (uvwindex > varsimplymsh->uvw_count) {
	DDerror(ERR_VALUE_OUT_OF_RANGE, 
		   "DpUpdVarSimplePolygonMeshUVW",
		   "uvw index too large");
	return;
    }
#endif

    varsimplymsh->sequence_number++;
    if (varsimplymsh->uvw_count < uvwindex+1) {
	varsimplymsh->uvw_count = uvwindex+1;
    }
    varsimplymsh->uvw_list[uvwindex] = uvwlist;

    /* update the uvw's of the underlying vartrimsh - if it exists */
    if (varsimplymsh->alt_object_tris != NULL) 
        dor_vartrimsh_update_uvw(varsimplymsh->alt_object_tris,
                                 uvwindex, uvwlist);
}
