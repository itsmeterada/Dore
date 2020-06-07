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
	dor_anntxt_initialize
	dor_anntxt_create
	dor_anntxt_destroy
	dor_anntxt_print
	dor_anntxt_update_alternate
	dor_anntxt_update_lines
	dor_anntxt_update_triangles
	dor_anntxt_create_alternate_geometry_line_polygon
	dor_anntxt_create_alternate_geometry_line_polyline
	dor_anntxt_create_alternate_geometry_lines
	dor_anntxt_create_alternate_geometry_simple_triangle
	dor_anntxt_create_alternate_geometry_complex_triangle
	dor_anntxt_create_alternate_geometry_triangle_error
	dor_anntxt_create_alternate_geometry_triangle_polygon
	dor_anntxt_create_alternate_geometry_triangles
	dor_anntxt_check_attributes
	dor_anntxt_compute_bounding_volume
	dor_anntxt_enable_edges_complex
	dor_anntxt_check_complex_edge
	dor_anntxt_enable_edges_simple
	dor_anntxt_pick
	dor_anntxt_pick_lines
	dor_anntxt_pick_triangles

  ======================================================================
 */
#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/font.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/anntxt.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trilst.h>

/*
 ======================================================================
 */

DtInt doe_anntxt_class_id;

static DtMethodEntry anntxt_methods[] =
{   { DcMethodCmpBndVolume, (DtMethodPtr) dor_anntxt_compute_bounding_volume },
    { DcMethodDestroy,      (DtMethodPtr) dor_anntxt_destroy },
    { DcMethodPick,         (DtMethodPtr) dor_anntxt_pick },
    { DcMethodPrint,        (DtMethodPtr) dor_anntxt_print },
    { DcMethodUpdStdAltObj, (DtMethodPtr) dor_anntxt_update_alternate }
};

void dor_anntxt_initialize (void)
{
    doe_anntxt_class_id = dor_class_install (DcTypeAnnotationText, 
				"DoAnnoText", 5, anntxt_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_anntxt_create (
    DtPoint3 position,
    char *string)
{
    static DtFlag initialized = DcFalse;
    struct anntxt *anntxt;
    dot_txtaln txtaln;
    dot_txtupvec txtupvec;
    dot_object *anntxtobject;

    if (!initialized) {
	dor_anntxt_initialize();
	initialized = DcTrue;
    }

    anntxt = (struct anntxt *)dor_space_allocate(sizeof *anntxt);

    anntxt->position[0] = position[0];
    anntxt->position[1] = position[1];
    anntxt->position[2] = position[2];

    anntxt->string = dor_space_allocate (strlen(string)+1);
    strcpy (anntxt->string, string);

    dor_global_txtaln_get_value(&txtaln);
    anntxt->halign = txtaln.halign;
    anntxt->valign = txtaln.valign;

    dor_global_txtexp_get_value(&anntxt->textexp);
 
    dor_global_txtfnt_get_value(&anntxt->font);

    dor_global_txthgt_get_value(&anntxt->textheight);

    dor_global_txtpat_get_value(&anntxt->textpath);

    dor_global_txtprc_get_value(&anntxt->precision);

    dor_global_txtspc_get_value(&anntxt->textspace);

    dor_global_txtupvec_get_value(&txtupvec);
    anntxt->xup = txtupvec.xup;
    anntxt->yup = txtupvec.yup;

    anntxt->alternate_object_lines = DcNullPtr;
    anntxt->alternate_object_triangles = DcNullPtr;

    anntxtobject = dor_object_create(doe_anntxt_class_id,anntxt);

    return(anntxtobject);
}
/*
 ======================================================================
 */

void dor_anntxt_destroy (dot_object *object)
{
    struct anntxt *anntxt;

    anntxt = (struct anntxt *)(object->data);

    if (anntxt->string != NULL){
	dor_space_deallocate(anntxt->string);
    }

    if (anntxt->alternate_object_lines != NULL){
	dor_object_check_deletion(anntxt->alternate_object_lines);
    }

    if (anntxt->alternate_object_triangles != NULL){
	dor_object_check_deletion(anntxt->alternate_object_triangles);
    }

    dor_space_deallocate(anntxt);
}
/*
 ======================================================================
 */

void dor_anntxt_print (dot_object *object)
{
    struct anntxt *anntxt;

    anntxt = (struct anntxt *)(object->data);

    if (anntxt == NULL){
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),"position = (%lf,%lf,%lf)", 
	    anntxt->position[0],anntxt->position[1],anntxt->position[2]);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),"string = '%s'",anntxt->string);
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

dot_object *dor_anntxt_update_alternate (dot_object *object)
{
    DtRepType reptype;
    dot_object *alt_obj;

    dor_global_reptyp_get_value(&reptype);

    if ((reptype == DcPoints) || 
	(reptype == DcWireframe) ||
	(reptype == DcOutlines)) {
	alt_obj = dor_anntxt_update_lines(object);

    } else if ((reptype == DcSurface) ||
	       (reptype == DcSurfaceWireframeEdges) ||
	       (reptype == DcSurfaceOutlineEdges)) {
	alt_obj = dor_anntxt_update_triangles(object);

    } else {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_anntxt_pick",
		   "unknown reptype");
        alt_obj = DcNullPtr;
    }
    return alt_obj;
}
/*
 ======================================================================
 */

dot_object *dor_anntxt_update_lines (dot_object *object)
{
    struct anntxt *anntxt;
    DtFlag changed;
    dot_object *alt_obj;

    anntxt = (struct anntxt *)(object->data);

    changed = dor_anntxt_check_attributes(anntxt);

    if ((anntxt->alternate_object_lines != DcNullPtr) && !changed) {
	alt_obj = anntxt->alternate_object_lines;
    } else {
	if (anntxt->alternate_object_lines != NULL) {
	    dor_object_get_method (anntxt->alternate_object_lines,
		DcMethodDestroy) (anntxt->alternate_object_lines, 0);
	    anntxt->alternate_object_lines = NULL;
	}

	dor_anntxt_create_alternate_geometry_lines(anntxt);
	alt_obj = anntxt->alternate_object_lines;

	if (anntxt->alternate_object_lines == NULL) {
	    DDerror (ERR_CANT_CREATE_ALTOBJ, "dor_anntxt_update_lines", "");
	}
    }
    return alt_obj;
}

/* ====================================================================== */

dot_object *dor_anntxt_update_triangles (dot_object *object)
{
    struct anntxt *anntxt;
    DtFlag changed;
    DtInt fonttype;
    dot_font_state ts;
    dot_object *alt_obj;

    anntxt = (struct anntxt *)(object->data);
   
    changed = dor_anntxt_check_attributes(anntxt);
   
    ts.font_index = anntxt->font;
    fonttype = dor_font_get_type(&ts);

    alt_obj = DcNullPtr;

    if (fonttype == dod_font_type_polygonal) {
	if ((anntxt->alternate_object_triangles != DcNullPtr) && 
	    !changed){
	    alt_obj = anntxt->alternate_object_triangles;
	} else {
	    if (anntxt->alternate_object_triangles != NULL) {
		dor_object_get_method (anntxt->alternate_object_triangles,
		    DcMethodDestroy) (anntxt->alternate_object_triangles);

		anntxt->alternate_object_triangles = NULL;
	    }

	    dor_anntxt_create_alternate_geometry_triangles(anntxt);

	    alt_obj = anntxt->alternate_object_triangles;

	    if (anntxt->alternate_object_triangles == NULL) {
		DDerror
		    (ERR_CANT_CREATE_ALTOBJ, "dor_anntxt_update_triangles", "");
	    }
	}
    } else {
	if ((anntxt->alternate_object_lines != DcNullPtr) && !changed){
	    alt_obj = anntxt->alternate_object_lines;
	} else {
	    if (anntxt->alternate_object_lines != NULL) {
		dor_object_get_method(anntxt->alternate_object_lines,
			DcMethodDestroy)(anntxt->alternate_object_lines);

		anntxt->alternate_object_lines = NULL;
	    } 

	    dor_anntxt_create_alternate_geometry_lines(anntxt);
	    alt_obj = anntxt->alternate_object_lines;

	    if (anntxt->alternate_object_lines == NULL) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_anntxt_update_triangles", "");
	    }
	}
    }
    return alt_obj;
}
/*
 ======================================================================
 */

void dor_anntxt_create_alternate_geometry_line_polygon (
    struct anntxt *anntxt,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtInt vertexno;
    DtRealTriple lastline[2];

    for (vertexno=0;vertexno<(vertexcount-1);vertexno++){
	dor_linlst_add_line(anntxt->alternate_object_lines,vertices[vertexno]);
    }

    /*  Since the polygonal description does not include the original point
	replicated, add it as the last vertex of the connected line list.  */

    lastline[0][0] = vertices[vertexcount-1][0];
    lastline[0][1] = vertices[vertexcount-1][1];
    lastline[0][2] = vertices[vertexcount-1][2];

    lastline[1][0] = vertices[0][0];
    lastline[1][1] = vertices[0][1];
    lastline[1][2] = vertices[0][2];

    dor_linlst_add_line (anntxt->alternate_object_lines, (DtReal*)lastline);
}
/*
 ======================================================================
 */

void dor_anntxt_create_alternate_geometry_line_polyline (
    struct anntxt *anntxt,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtInt vertexno;

    for (vertexno=0;vertexno<(vertexcount-1);vertexno++){
	dor_linlst_add_line(anntxt->alternate_object_lines,vertices[vertexno]);
    }
}
/*
 ======================================================================
 */

void dor_anntxt_create_alternate_geometry_lines (struct anntxt *anntxt)
{
    dot_object *linlstobj;
    dot_font_state ts;

    /* set up anntxt state */

    ts.string = anntxt->string;
    ts.length = strlen(ts.string);
    ts.position.x = 0.;
    ts.position.y = 0.;

    ts.height = anntxt->textheight;

    ts.charexp = anntxt->textexp;

    ts.path = anntxt->textpath;

    ts.xup = anntxt->xup;
    ts.yup = anntxt->yup;

    ts.spacing = anntxt->textspace;

    ts.halign = anntxt->halign;
    ts.valign = anntxt->valign;

    ts.font_index = anntxt->font;

    ts.user_data = (char *)anntxt;

    ts.polyline = dor_anntxt_create_alternate_geometry_line_polyline;
    ts.polygon = dor_anntxt_create_alternate_geometry_line_polygon;

    linlstobj = dor_linlst_open (DcRGB, DcLoc, ts.length*50);
    anntxt->alternate_object_lines = linlstobj;

    dor_font_draw_text(&ts);

    dor_linlst_close(linlstobj);
}
/*
 ======================================================================
 */

void dor_anntxt_create_alternate_geometry_simple_triangle (
    struct anntxt_decomp_data *decompdata,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
   DtReal *vtxptr;
   DtReal vertices[9];
   DtInt edge_enable;

   edge_enable = dor_anntxt_enable_edges_simple(vertex1, vertex2, vertex3);

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

   dor_trilst_add_triangle_with_edges(decompdata->alternate_object_triangles,
		vertices,edge_enable);
}
/*
 ======================================================================
 */
void dor_anntxt_create_alternate_geometry_complex_triangle (
    struct anntxt_decomp_data *decompdata,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
   DtReal *vtxptr;
   DtReal vertices[9];
   DtInt edge_enable;

   edge_enable = dor_anntxt_enable_edges_complex(gpolydvtx1, gpolydvtx2, 
				gpolydvtx3);

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

   dor_trilst_add_triangle_with_edges
		(decompdata->alternate_object_triangles,vertices,edge_enable);
}
/*
 ======================================================================
 */

void dor_anntxt_create_alternate_geometry_triangle_error (
    struct anntxt *anntxt,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DDerror(ERR_VALUE_OUT_OF_RANGE,
	       "dor_anntxt_create_alternate_geometry_triangle_error",
	       "font is polyline");
}
/*
 ======================================================================
 */

DtInt doe_anntxt_vertexcount;

void dor_anntxt_create_alternate_geometry_triangle_polygon (
    struct anntxt *anntxt,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtInt vertexno;
    struct anntxt_decomp_data decompdata;

    decompdata.wcsvertices = 
	(DtRealTriple *)dor_space_allocate(2000*sizeof(DtRealTriple));
    decompdata.alternate_object_triangles = 
			anntxt->alternate_object_triangles;

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

    doe_anntxt_vertexcount = vertexcount;
    dor_gpolyd_triangulate_polygon
    (   DcComplex,
	dor_anntxt_create_alternate_geometry_simple_triangle,
	dor_anntxt_create_alternate_geometry_complex_triangle,
	&decompdata
    );

    dor_space_deallocate(decompdata.wcsvertices);
}

/* ====================================================================== */

void dor_anntxt_create_alternate_geometry_triangles (struct anntxt *anntxt)
{
    dot_object *trilstobj;
    dot_font_state ts;

    /* set up anntxt state */

    ts.string = anntxt->string;
    ts.length = strlen(ts.string);
    ts.position.x = 0.;
    ts.position.y = 0.;

    ts.string = anntxt->string;
    ts.length = strlen(ts.string);
    ts.position.x = 0.;
    ts.position.y = 0.;
 
    ts.height = anntxt->textheight;
 
    ts.charexp = anntxt->textexp;
 
    ts.path = anntxt->textpath;
 
    ts.xup = anntxt->xup;
    ts.yup = anntxt->yup;
 
    ts.spacing = anntxt->textspace;
 
    ts.halign = anntxt->halign;
    ts.valign = anntxt->valign;
 
    ts.font_index = anntxt->font;

    ts.user_data = (char *) anntxt;

    ts.polyline = dor_anntxt_create_alternate_geometry_triangle_error;
    ts.polygon = dor_anntxt_create_alternate_geometry_triangle_polygon;

    trilstobj = dor_trilst_open (DcRGB, DcLoc, ts.length * 300);
    anntxt->alternate_object_triangles = trilstobj;

    dor_font_draw_text(&ts);

    dor_trilst_close(trilstobj);
}
/*
 ======================================================================
 */

DtFlag dor_anntxt_check_attributes (struct anntxt *anntxt)
{
    DtFlag returnflag;
    dot_txtaln txtaln;
    DtReal textexp;
    DtFont font;
    DtReal textheight;
    DtTextPath textpath;
    DtTextPrecision precision;
    DtReal textspace;
    dot_txtupvec txtupvec;

    returnflag = DcFalse;

    dor_global_txtaln_get_value(&txtaln);
    if (anntxt->halign != txtaln.halign) {
	anntxt->halign = txtaln.halign;
	returnflag = DcTrue;
    }
    if (anntxt->valign != txtaln.valign) {
	anntxt->valign = txtaln.valign;
	returnflag = DcTrue;
    }

    dor_global_txtexp_get_value(&textexp);
    if (anntxt->textexp != textexp) {
	anntxt->textexp = textexp;
	returnflag = DcTrue;
    }

    dor_global_txtfnt_get_value(&font);
    if (anntxt->font != font) {
	anntxt->font = font;
	returnflag = DcTrue;
    }

    dor_global_txthgt_get_value(&textheight);
    if (anntxt->textheight != textheight) {
	anntxt->textheight = textheight;
	returnflag = DcTrue;
    }

    dor_global_txtpat_get_value(&textpath);
    if (anntxt->textpath != textpath) {
	anntxt->textpath = textpath;
	returnflag = DcTrue;
    }

    dor_global_txtprc_get_value(&precision);
    if (anntxt->precision != precision) {
	anntxt->precision = precision;
	returnflag = DcTrue;
    }

    dor_global_txtspc_get_value(&textspace);
    if (anntxt->textspace != textspace) {
	anntxt->textspace = textspace;
	returnflag = DcTrue;
    }

    dor_global_txtupvec_get_value(&txtupvec);
    if (anntxt->xup != txtupvec.xup) {
	anntxt->xup = txtupvec.xup;
	returnflag = DcTrue;
    }
    if (anntxt->yup != txtupvec.yup) {
	anntxt->yup = txtupvec.yup;
	returnflag = DcTrue;
    }

    return (returnflag);
} 
/*
 ======================================================================
 */

void dor_anntxt_compute_bounding_volume (dot_object *object)
{
    struct anntxt *anntxt;

    anntxt = (struct anntxt *)(object->data);

    dor_system_report_bounding_points(1,anntxt->position);
}
/*
 ======================================================================
 */

DtInt dor_anntxt_enable_edges_complex (
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DtInt edge_enable;

    edge_enable = dod_no_enabled_edges;
    if (dor_anntxt_check_complex_edge(gpolydvtx1, gpolydvtx2))
	    edge_enable |= dod_edge1_enabled;
    if (dor_anntxt_check_complex_edge(gpolydvtx2, gpolydvtx3))
	    edge_enable |= dod_edge2_enabled;
    if (dor_anntxt_check_complex_edge(gpolydvtx3, gpolydvtx1))
	    edge_enable |= dod_edge3_enabled;

    return (edge_enable);

}

DtFlag dor_anntxt_check_complex_edge (
    dot_gpolyd_interpolated_vertex *vtx1,
    dot_gpolyd_interpolated_vertex *vtx2)
{
    if (vtx1->interpolated && vtx2->interpolated) {
        if ((vtx1->vertex1 == vtx2->vertex1 ||
             vtx1->vertex1 == vtx2->vertex2) &&
            (vtx1->vertex2 == vtx2->vertex1 ||
             vtx1->vertex2 == vtx2->vertex2))
		return (DcTrue);

    } else if (vtx1->interpolated) {
        if (vtx2->vertex1 == vtx1->vertex1 ||
            vtx2->vertex1 == vtx1->vertex2)
		return (DcTrue);

    } else if (vtx2->interpolated) {
        if (vtx1->vertex1 == vtx2->vertex1 ||
            vtx1->vertex1 == vtx2->vertex2)
		return (DcTrue);

    } else if (vtx1->vertex1 == ((vtx2->vertex1+1)%doe_anntxt_vertexcount) ||
	       vtx2->vertex1 == ((vtx1->vertex1+1)%doe_anntxt_vertexcount)) {
        return (DcTrue);
    }

    return (DcFalse);
}
/*
 ======================================================================
 */

DtInt dor_anntxt_enable_edges_simple (
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
    DtInt edge_enable;

    edge_enable = dod_no_enabled_edges;
    if (vertex1 == ((vertex2+1)%doe_anntxt_vertexcount) ||
	vertex2 == ((vertex1+1)%doe_anntxt_vertexcount)) {
        edge_enable |= dod_edge1_enabled;
    }
    if (vertex2 == ((vertex3+1)%doe_anntxt_vertexcount) ||
	vertex3 == ((vertex2+1)%doe_anntxt_vertexcount)) {
        edge_enable |= dod_edge2_enabled;
    }
    if (vertex3 == ((vertex1+1)%doe_anntxt_vertexcount) ||
	vertex1 == ((vertex3+1)%doe_anntxt_vertexcount)) {
        edge_enable |= dod_edge3_enabled;
    }

    return (edge_enable);
}
/*
 ======================================================================
 */

void dor_anntxt_pick (dot_object *object)
{
    struct anntxt *anntxt;
    DtMatrix4x4 tfmmatrix;
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_anntxt_class_id)) {
   	return;
    }

    anntxt = (struct anntxt *)(object->data);

    dor_pick_initialize_object(object);

    if (dor_pick_transform_clip_z_point(anntxt->position,tfmmatrix)) {
   	return;
    }

    dor_pick_push_lcstofcsmat(tfmmatrix);

    dor_global_reptyp_get_value(&reptype);

    if ((reptype == DcPoints) || 
	(reptype == DcWireframe) ||
	(reptype == DcOutlines)) {
   	dor_anntxt_pick_lines(object);

    } else if ((reptype == DcSurface) ||
	       (reptype == DcSurfaceWireframeEdges) ||
	       (reptype == DcSurfaceOutlineEdges)) {
   	dor_anntxt_pick_triangles(object);

    } else {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_anntxt_pick",
		   "unknown reptype"); }

    dor_pick_pop_lcstofcsmat();
}
/*
 ======================================================================
 */

void dor_anntxt_pick_lines (dot_object *object)
{
    struct anntxt *anntxt;
    DtFlag changed;

    anntxt = (struct anntxt *)(object->data);

    changed = dor_anntxt_check_attributes(anntxt);

    if ((anntxt->alternate_object_lines != DcNullPtr) && !changed) {
	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(anntxt->alternate_object_lines,
			      DcMethodPick) (anntxt->alternate_object_lines);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    } else {
	if (anntxt->alternate_object_lines != NULL) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(anntxt->alternate_object_lines,
			DcMethodDestroy)(anntxt->alternate_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    anntxt->alternate_object_lines = NULL;
        }

	dor_anntxt_create_alternate_geometry_lines(anntxt);

	if (anntxt->alternate_object_lines == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_anntxt_pick_lines", "");
	    return;
	}

	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(anntxt->alternate_object_lines,
			      DcMethodPick) (anntxt->alternate_object_lines);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    }
}
/*
 ======================================================================
 */

void dor_anntxt_pick_triangles (dot_object *object)
{
    struct anntxt *anntxt;
    DtInt fonttype;
    dot_font_state ts;
    DtFlag changed;

    anntxt = (struct anntxt *)(object->data);

    changed = dor_anntxt_check_attributes(anntxt);

    ts.font_index = anntxt->font;
    fonttype = dor_font_get_type(&ts);

    if (fonttype == dod_font_type_polygonal) {
	if ((anntxt->alternate_object_triangles != DcNullPtr) && !changed) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(anntxt->alternate_object_triangles,
			DcMethodPick)(anntxt->alternate_object_triangles);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    return;
	} else {
	    if (anntxt->alternate_object_triangles != NULL) {
		doe_system_alternate_object_execution_flag = DcTrue;

		dor_object_get_method(anntxt->alternate_object_triangles,
			DcMethodDestroy) (anntxt->alternate_object_triangles);

		doe_system_alternate_object_execution_flag = DcFalse;

		anntxt->alternate_object_triangles = NULL;
	    }

	    dor_anntxt_create_alternate_geometry_triangles(anntxt);

	    if (anntxt->alternate_object_triangles == NULL) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_anntxt_pick_triangles", "");
		return;
	    }

	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(anntxt->alternate_object_triangles,
			DcMethodPick)(anntxt->alternate_object_triangles);

	    doe_system_alternate_object_execution_flag = DcFalse;
	}
    } else {
	if ((anntxt->alternate_object_lines != DcNullPtr) && !changed){
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(anntxt->alternate_object_lines,
			DcMethodPick)(anntxt->alternate_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    return;
	} else {
	    if (anntxt->alternate_object_lines != NULL) {
		doe_system_alternate_object_execution_flag = DcTrue;

		dor_object_get_method(anntxt->alternate_object_lines,                              DcMethodDestroy) (anntxt->alternate_object_lines);

		doe_system_alternate_object_execution_flag = DcFalse;

		anntxt->alternate_object_lines = NULL;
	    }

	    dor_anntxt_create_alternate_geometry_lines(anntxt);

	    if (anntxt->alternate_object_lines == NULL) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_anntxt_pick_triangles", "");
		return;
	    }

	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(anntxt->alternate_object_lines,
			DcMethodPick)(anntxt->alternate_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;
	}
    }

    return;
}
