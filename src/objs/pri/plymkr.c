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
	dor_plymkr_initialize
	dor_plymkr_create
	dor_plymkr_destroy
	dor_plymkr_print
	dor_plymkr_update_alternate
	dor_plymkr_create_alternate_geometry_complex_triangle
	dor_plymkr_create_alternate_geometry_line_polygon
	dor_plymkr_create_alternate_geometry_line_polyline
	dor_plymkr_create_alternate_geometry_lines
	dor_plymkr_create_alternate_geometry_simple_triangle
	dor_plymkr_create_alternate_geometry_triangle_error
	dor_plymkr_create_alternate_geometry_triangle_polygon
	dor_plymkr_create_alternate_geometry_triangles
	dor_plymkr_check_attributes
	dor_plymkr_compute_bounding_volume
	dor_plymkr_pick
	dor_plymkr_pick_lines
	dor_plymkr_pick_triangles
	dor_plymkr_update_lines
	dor_plymkr_update_triangles

  ======================================================================
 */
#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/plymkr.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trilst.h>

/*
 ======================================================================
 */

DtInt doe_plymkr_class_id;

static DtMethodEntry plymkr_methods[] = {
    { DcMethodCmpBndVolume, (DtMethodPtr) dor_plymkr_compute_bounding_volume },
    { DcMethodDestroy,      (DtMethodPtr) dor_plymkr_destroy },
    { DcMethodPick,         (DtMethodPtr) dor_plymkr_pick },
    { DcMethodPrint,        (DtMethodPtr) dor_plymkr_print },
    { DcMethodUpdStdAltObj, (DtMethodPtr) dor_plymkr_update_alternate }
};

void dor_plymkr_initialize (void)
{
    doe_plymkr_class_id = dor_class_install (DcTypePolymarker, "DoPolymarker", 
			5, plymkr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_plymkr_create (
    DtInt markercount,
    DtPoint3 *position)
{
    static DtFlag initialized = DcFalse;
    struct plymkr *plymkr;
    DtInt markerno;
    dot_object *plymkrobject;
    dot_txtupvec txtupvec;

    if (!initialized) {
	dor_plymkr_initialize();
	initialized = DcTrue;
    }

    plymkr = (struct plymkr *)dor_space_allocate(sizeof *plymkr);

    if (markercount > 0) {
	plymkr->position = 
		(DtRealTriple *)dor_space_allocate(markercount*(sizeof (DtRealTriple)));

	for (markerno=0;markerno<markercount;markerno++) {
	    plymkr->position[markerno][0] = position[markerno][0];
	    plymkr->position[markerno][1] = position[markerno][1];
	    plymkr->position[markerno][2] = position[markerno][2];
	}

	plymkr->count = markercount;
    } else {
	plymkr->position = NULL;
	plymkr->count = 0;
    }

    dor_global_mkrgly_get_value(&plymkr->glyph); 
    dor_global_mkrscl_get_value(&plymkr->scale);
    dor_global_mkrfnt_get_value(&plymkr->font);

    dor_global_txtexp_get_value(&plymkr->textexp);
    dor_global_txtupvec_get_value(&txtupvec);
    plymkr->xup = txtupvec.xup;
    plymkr->yup = txtupvec.yup;

    plymkr->alternate_object_lines = DcNullPtr;
    plymkr->alternate_object_triangles = DcNullPtr;

    plymkrobject = dor_object_create(doe_plymkr_class_id,plymkr);

    return(plymkrobject);
}
/*
 ======================================================================
 */

void dor_plymkr_destroy (dot_object *object)
{
    struct plymkr *plymkr;

    plymkr = (struct plymkr *)(object->data);

    if (plymkr->position != NULL){
	dor_space_deallocate(plymkr->position);
    }

    if (plymkr->alternate_object_lines != NULL){
	dor_object_check_deletion(plymkr->alternate_object_lines);
    }

    if (plymkr->alternate_object_triangles != NULL){
	dor_object_check_deletion(plymkr->alternate_object_triangles);
    }

    dor_space_deallocate(plymkr);
}
/*
 ======================================================================
 */

void dor_plymkr_print (dot_object *object)
{
    struct plymkr *plymkr;

    plymkr = (struct plymkr *)(object->data);

    if (plymkr == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"marker count = %d",plymkr->count);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

dot_object *dor_plymkr_update_alternate (dot_object *object)
{	
    DtRepType reptype;
    dot_object *alt;

    dor_global_reptyp_get_value(&reptype);

    if ((reptype == DcPoints) || 
	(reptype == DcWireframe) ||
	(reptype == DcOutlines)) {
        alt = dor_plymkr_update_lines(object);

    } else if ((reptype == DcSurface) ||
	       (reptype == DcSurfaceWireframeEdges) ||
	       (reptype == DcSurfaceOutlineEdges)) {
        alt = dor_plymkr_update_triangles(object);

    } else {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_plymkr_update_alternate", 
		   "unknown reptype");
        alt = DcNullPtr;
    }
    return alt;
}
/*
 ======================================================================
 */

void dor_plymkr_create_alternate_geometry_complex_triangle (
    struct plymkr_decomp_data *decompdata,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DtReal *vtxptr;
    DtReal vertices[9];

    vtxptr = vertices;

    *vtxptr++ = gpolydvtx1->coord[0];
    *vtxptr++ = gpolydvtx1->coord[1];
    *vtxptr++ = gpolydvtx1->coord[2];

    *vtxptr++ = gpolydvtx2->coord[0];
    *vtxptr++ = gpolydvtx2->coord[1];
    *vtxptr++ = gpolydvtx2->coord[2];

    *vtxptr++ = gpolydvtx3->coord[0];
    *vtxptr++ = gpolydvtx3->coord[1];
    *vtxptr++ = gpolydvtx3->coord[2];

    dor_trilst_add_triangle(decompdata->alternate_object_triangles,vertices);
}
/*
 ======================================================================
 */

void dor_plymkr_create_alternate_geometry_line_polygon(
    struct plymkr *plymkr,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtInt vertexno;
    DtRealTriple lastline[2];

    for (vertexno=0;vertexno<(vertexcount-1);vertexno++){
	dor_linlst_add_line(plymkr->alternate_object_lines,vertices[vertexno]);
    }

    /*  Since the polygonal description does not include the original point
	replicated, add it as the last vertex of the connected line list.  */

    lastline[0][0] = vertices[vertexcount-1][0];
    lastline[0][1] = vertices[vertexcount-1][1];
    lastline[0][2] = vertices[vertexcount-1][2];

    lastline[1][0] = vertices[0][0];
    lastline[1][1] = vertices[0][1];
    lastline[1][2] = vertices[0][2];

    dor_linlst_add_line (plymkr->alternate_object_lines, (DtReal*)lastline);
}
/*
 ======================================================================
 */

void dor_plymkr_create_alternate_geometry_line_polyline (
    struct plymkr *plymkr,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtInt vertexno;

    for (vertexno=0;vertexno<(vertexcount-1);vertexno++){
	dor_linlst_add_line(plymkr->alternate_object_lines,vertices[vertexno]);
    }
}
/*
 ======================================================================
 */

void dor_plymkr_create_alternate_geometry_lines (struct plymkr *plymkr)
{
    dot_object *linlstobj;
    dot_font_state ts;
    char glyphstring[2];

    /* set up plymkr state */

    glyphstring[0] = plymkr->glyph;
    glyphstring[1] = 0;
    ts.string = glyphstring;
    ts.length = strlen (ts.string);
    ts.position.x = 0.;
    ts.position.y = 0.;

    ts.height = plymkr->scale;
    ts.font_index = plymkr->font;

    ts.charexp = plymkr->textexp;
    ts.xup = plymkr->xup;
    ts.yup = plymkr->yup;
    dor_global_txtspc_get_value(&ts.spacing);
    dor_global_txtpat_get_value(&ts.path);

    ts.halign = DcTextHAlignCenter;
    ts.valign = DcTextVAlignHalf;


    ts.user_data = (char *)plymkr;

    ts.polyline = dor_plymkr_create_alternate_geometry_line_polyline;
    ts.polygon = dor_plymkr_create_alternate_geometry_line_polygon;

    linlstobj = dor_linlst_open(DcRGB,DcLoc,50);
    plymkr->alternate_object_lines = linlstobj;

    dor_font_draw_text(&ts);

    dor_linlst_close(linlstobj);
}
/*
 ======================================================================
 */

void dor_plymkr_create_alternate_geometry_simple_triangle (
    struct plymkr_decomp_data *decompdata,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
    DtReal *vtxptr;
    DtReal vertices[9];

    vtxptr = vertices;

    *vtxptr++ = decompdata->wcsvertices[vertex1][0];
    *vtxptr++ = decompdata->wcsvertices[vertex1][1];
    *vtxptr++ = decompdata->wcsvertices[vertex1][2];

    *vtxptr++ = decompdata->wcsvertices[vertex2][0];
    *vtxptr++ = decompdata->wcsvertices[vertex2][1];
    *vtxptr++ = decompdata->wcsvertices[vertex2][2];

    *vtxptr++ = decompdata->wcsvertices[vertex3][0];
    *vtxptr++ = decompdata->wcsvertices[vertex3][1];
    *vtxptr++ = decompdata->wcsvertices[vertex3][2];

    dor_trilst_add_triangle(decompdata->alternate_object_triangles,vertices);
}
/*
 ======================================================================
 */

void dor_plymkr_create_alternate_geometry_triangle_error (
    struct plymkr *plymkr,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DDerror(ERR_VALUE_OUT_OF_RANGE,
	       "dor_plymkr_create_alternate_geometry_triangle_error",
	       "font is polyline");
}
/*
 ======================================================================
 */

void dor_plymkr_create_alternate_geometry_triangle_polygon (
    struct plymkr *plymkr,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
	DtInt vertexno;
	struct plymkr_decomp_data decompdata;

	decompdata.wcsvertices = (DtRealTriple *)dor_space_allocate
			(2000*sizeof(DtRealTriple));
	decompdata.alternate_object_triangles = plymkr->alternate_object_triangles;

	dor_gpolyd_create_polygon();
	dor_gpolyd_create_contour();
	
	for (vertexno=0;vertexno<vertexcount;vertexno++) {
		dor_gpolyd_create_contour_vertex(vertexno,
			vertices[vertexno][0],
			vertices[vertexno][1],
			vertices[vertexno][2]);
	}
	
	dor_gpolyd_close_contour();
	dor_gpolyd_close_polygon();

	dor_gpolyd_triangulate_polygon(DcComplex,
		dor_plymkr_create_alternate_geometry_simple_triangle,
		dor_plymkr_create_alternate_geometry_complex_triangle,
		&decompdata);

	dor_space_deallocate(decompdata.wcsvertices);
}
/*
 ======================================================================
 */

void dor_plymkr_create_alternate_geometry_triangles (struct plymkr *plymkr)
{
    dot_object *trilstobj;
    dot_font_state ts;
    char glyphstring[2];

    /* set up plymkr state */

    glyphstring[0] = plymkr->glyph;
    glyphstring[1] = 0;
    ts.string = glyphstring;
    ts.length = strlen (ts.string);
    ts.position.x = 0.;
    ts.position.y = 0.;

    ts.height = plymkr->scale;
    ts.font_index = plymkr->font;

    ts.charexp = plymkr->textexp;
    ts.xup = plymkr->xup;
    ts.yup = plymkr->yup;
    dor_global_txtspc_get_value(&ts.spacing);
    dor_global_txtpat_get_value(&ts.path);

    ts.halign = DcTextHAlignCenter;
    ts.valign = DcTextVAlignHalf;

    ts.user_data = (char *) plymkr;

    ts.polyline = dor_plymkr_create_alternate_geometry_triangle_error;
    ts.polygon = dor_plymkr_create_alternate_geometry_triangle_polygon;

    trilstobj = dor_trilst_open (DcRGB, DcLoc, 300);
    plymkr->alternate_object_triangles = trilstobj;

    dor_font_draw_text(&ts);

    dor_trilst_close(trilstobj);
}
/*
 ======================================================================
 */

DtFlag dor_plymkr_check_attributes (struct plymkr *plymkr)
{
    DtFlag returnflag;
    DtInt glyph;
    DtReal scale;
    DtFont font;
    DtReal textexp;
    dot_txtupvec txtupvec;

    returnflag = DcFalse;

    dor_global_mkrgly_get_value(&glyph);
    if (plymkr->glyph != glyph) {
	plymkr->glyph = glyph;
	returnflag = DcTrue;
    }

    dor_global_mkrscl_get_value(&scale);
    if (plymkr->scale != scale) {
	plymkr->scale = scale;
	returnflag = DcTrue;
    }

    dor_global_mkrfnt_get_value(&font);
    if (plymkr->font != font) {
	plymkr->font = font;
	returnflag = DcTrue;
    }

    dor_global_txtexp_get_value(&textexp);
    if (plymkr->textexp != textexp) {
	plymkr->textexp = textexp;
	returnflag = DcTrue;
    }

    dor_global_txtupvec_get_value(&txtupvec);
    if (plymkr->xup != txtupvec.xup) {
	plymkr->xup = txtupvec.xup;
	returnflag = DcTrue;
    }
    if (plymkr->yup != txtupvec.yup) {
	plymkr->yup = txtupvec.yup;
	returnflag = DcTrue;
    }

    return (returnflag);
} 
/*
 ======================================================================
 */

void dor_plymkr_compute_bounding_volume (dot_object *object)
{
    struct plymkr *plymkr;

    plymkr = (struct plymkr *)(object->data);

    dor_system_report_bounding_points (plymkr->count,(DtReal*)plymkr->position);
}
/*
 ======================================================================
 */

void dor_plymkr_pick (dot_object *object)
{
    struct plymkr *plymkr;
    DtMatrix4x4 tfmmatrix;
    DtRepType reptype;
    DtInt markerno;

    if (!dor_condex_query_pick(doe_plymkr_class_id)) {
   	return;
    }

    plymkr = (struct plymkr *)(object->data);

    dor_global_reptyp_get_value(&reptype);

    dor_pick_initialize_object(object);

    for (markerno=0;markerno<plymkr->count;markerno++) {
   	if (dor_pick_transform_clip_z_point(plymkr->position[markerno],
					    tfmmatrix)) {
	    continue;
   	}

   	dor_pick_push_lcstofcsmat(tfmmatrix);

   	if ((reptype == DcPoints) || 
   	    (reptype == DcWireframe) ||
   	    (reptype == DcOutlines)) {
	    dor_plymkr_pick_lines(object);

   	} else if ((reptype == DcSurface) ||
   		   (reptype == DcSurfaceWireframeEdges) ||
   		   (reptype == DcSurfaceOutlineEdges)) {
	    dor_plymkr_pick_triangles(object);

   	} else {
	    DDerror(ERR_VALUE_OUT_OF_RANGE,
		       "dor_plymkr_pick", 
		       "unknown reptype");			
   	}

   	dor_pick_pop_lcstofcsmat();
    }
}
/*
 ======================================================================
 */

void dor_plymkr_pick_lines (dot_object *object)
{
    struct plymkr *plymkr;
    DtFlag changed;

    plymkr = (struct plymkr *)(object->data);

    changed = dor_plymkr_check_attributes(plymkr);

    if ((plymkr->alternate_object_lines != DcNullPtr) && !changed){
	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(plymkr->alternate_object_lines,
			      DcMethodPick)(plymkr->alternate_object_lines);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    } else {
	if (plymkr->alternate_object_lines != NULL) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(plymkr->alternate_object_lines,
				  DcMethodDestroy)(plymkr->alternate_object_lines);
 
	    doe_system_alternate_object_execution_flag = DcFalse;

	    plymkr->alternate_object_lines = NULL;
        }

	dor_plymkr_create_alternate_geometry_lines(plymkr);

	if (plymkr->alternate_object_lines == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_plymkr_pick_lines", "");
	    return;
	}

	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(plymkr->alternate_object_lines,
			      DcMethodPick) (plymkr->alternate_object_lines);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    }
}
/*
 ======================================================================
 */

void dor_plymkr_pick_triangles (dot_object *object)
{
    struct plymkr *plymkr;
    DtInt fonttype;
    dot_font_state ts;
    DtFlag changed;

    plymkr = (struct plymkr *)(object->data);

    changed = dor_plymkr_check_attributes(plymkr);

    ts.font_index = plymkr->font;
    fonttype = dor_font_get_type(&ts);

    if (fonttype == dod_font_type_polygonal) {
	if ((plymkr->alternate_object_triangles != DcNullPtr) && !changed) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(plymkr->alternate_object_triangles,
				  DcMethodPick)(plymkr->alternate_object_triangles);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    return;
	} else {
	    if (plymkr->alternate_object_triangles != NULL) {
		doe_system_alternate_object_execution_flag = DcTrue;

		dor_object_get_method(plymkr->alternate_object_triangles,
				      DcMethodDestroy)(plymkr->alternate_object_triangles);

		doe_system_alternate_object_execution_flag = DcFalse;

		plymkr->alternate_object_triangles = NULL;
	    }

	    dor_plymkr_create_alternate_geometry_triangles(plymkr);

	    if (plymkr->alternate_object_triangles == NULL) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_plymkr_pick_triangles", "");
		return;
	    }

	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(plymkr->alternate_object_triangles,
				  DcMethodPick)(plymkr->alternate_object_triangles);

	    doe_system_alternate_object_execution_flag = DcFalse;
	}
    } else {
	if ((plymkr->alternate_object_lines != DcNullPtr) && !changed) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(plymkr->alternate_object_lines,
				  DcMethodPick)(plymkr->alternate_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    return;
	} else {
	    if (plymkr->alternate_object_lines != NULL) {
		doe_system_alternate_object_execution_flag = DcTrue;

		dor_object_get_method(plymkr->alternate_object_lines,
				      DcMethodDestroy)(plymkr->alternate_object_lines);

		doe_system_alternate_object_execution_flag = DcFalse;

		plymkr->alternate_object_lines = NULL;
	    }

	    dor_plymkr_create_alternate_geometry_lines(plymkr);

	    if (plymkr->alternate_object_lines == NULL) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_plymkr_pick_triangles", "");
		return;
	    }

	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(plymkr->alternate_object_lines,
				  DcMethodPick)(plymkr->alternate_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;
	}
    }

    return;
}
/*
 ======================================================================
 */

dot_object *dor_plymkr_update_lines (dot_object *object)
{
    dot_object *alt;
    struct plymkr *plymkr;
    DtFlag changed;
   
    plymkr = (struct plymkr *)(object->data);

    changed = dor_plymkr_check_attributes(plymkr);

    if ((plymkr->alternate_object_lines != DcNullPtr) && !changed) {
	alt = plymkr->alternate_object_lines;

    } else {
	if (plymkr->alternate_object_lines != DcNullPtr) {
	    dor_object_get_method(plymkr->alternate_object_lines,
				  DcMethodDestroy)(plymkr->alternate_object_lines);
	    plymkr->alternate_object_lines = DcNullPtr;
	}

	dor_plymkr_create_alternate_geometry_lines(plymkr);
	alt = plymkr->alternate_object_lines;

	if (plymkr->alternate_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_plymkr_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_plymkr_update_triangles (dot_object *object)
{
    dot_object *alt;
    struct plymkr *plymkr;
    DtInt fonttype;
    dot_font_state ts;
    DtFlag changed;

    plymkr = (struct plymkr *)(object->data);

    changed = dor_plymkr_check_attributes(plymkr);

    ts.font_index = plymkr->font;
    fonttype = dor_font_get_type(&ts);

    if (fonttype == dod_font_type_polygonal) {
	if ((plymkr->alternate_object_triangles != DcNullPtr) && 
	    !changed) { 
	    alt = plymkr->alternate_object_triangles;

	} else {
	    if (plymkr->alternate_object_triangles != DcNullPtr) {
		dor_object_get_method(plymkr->alternate_object_triangles,
				      DcMethodDestroy) (plymkr->alternate_object_triangles);

		plymkr->alternate_object_triangles = DcNullPtr;
	    }

	    dor_plymkr_create_alternate_geometry_triangles(plymkr);
	    alt = plymkr->alternate_object_triangles;

	    if (plymkr->alternate_object_triangles == DcNullPtr) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_plymkr_update_triangles", "");
	    }
	}
    } else {
	if ((plymkr->alternate_object_lines != DcNullPtr) && 
	    !changed) {
	    alt = plymkr->alternate_object_lines;

	} else {
	    if (plymkr->alternate_object_lines != DcNullPtr) {
		dor_object_get_method(plymkr->alternate_object_lines,
				      DcMethodDestroy)(plymkr->alternate_object_lines); 
		plymkr->alternate_object_lines = DcNullPtr;
	    } 

	    dor_plymkr_create_alternate_geometry_lines(plymkr);
	    alt = plymkr->alternate_object_lines;

	    if (plymkr->alternate_object_lines == DcNullPtr) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_plymkr_update_triangles", "");
	    }
	}
    }

    return alt;
}
