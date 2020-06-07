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
	dor_text_initialize
	dor_text_create
	dor_text_destroy
	dor_text_print
	dor_text_update_alternate
	dor_text_update_lines
	dor_text_update_triangles
	dor_text_create_alternate_geometry_complex_triangle
	dor_text_create_alternate_geometry_line_polygon
	dor_text_create_alternate_geometry_line_polyline
	dor_text_create_alternate_geometry_lines
	dor_text_create_alternate_geometry_simple_triangle
	dor_text_create_alternate_geometry_triangle_error
	dor_text_create_alternate_geometry_triangle_polygon
	dor_text_create_alternate_geometry_triangles
	dor_text_check_attributes
	dor_text_calculate_text_plane
	dor_text_compute_bounding_volume
	dor_text_cmpbndvol_point_callback
	dor_text_enable_edges_complex
	dor_text_check_complex_edge
	dor_text_enable_edges_simple
	dor_text_pick
	dor_text_pick_lines
	dor_text_pick_triangles
	dor_text_render_standard

  ======================================================================
 */

#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/system.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/text.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trilst.h>

/*
 ======================================================================
 */

DtInt doe_text_class_id;

static DtMethodEntry text_methods[] = {
    { DcMethodCmpBndVolume,     (DtMethodPtr) dor_text_compute_bounding_volume},
    { DcMethodDestroy,          (DtMethodPtr) dor_text_destroy},
    { DcMethodPick,             (DtMethodPtr) dor_text_pick},
    { DcMethodPrint,            (DtMethodPtr) dor_text_print},
    { DcMethodUpdStdAltObj,     (DtMethodPtr) dor_text_update_alternate},
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_text_render_standard},
};

void dor_text_initialize (void)
{
    doe_text_class_id = dor_class_install (DcTypeText, "DoText", 6, 
					   text_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_text_create (
    DtPoint3  position,
    DtVector3 u,
    DtVector3 v,
    char     *string)
{
    static DtFlag initialized = DcFalse;
    struct text *text;
    dot_object *textobject;
    dot_txtaln txtaln;
    dot_txtupvec txtupvec;

    if (!initialized) {
	dor_text_initialize();
	initialized = DcTrue;
    }

    text = (struct text *)dor_space_allocate(sizeof *text);

    text->position[0] = position[0];
    text->position[1] = position[1];
    text->position[2] = position[2];

    text->u[0] = u[0];
    text->u[1] = u[1];
    text->u[2] = u[2];

    text->v[0] = v[0];
    text->v[1] = v[1];
    text->v[2] = v[2];

    text->string = dor_space_allocate (strlen(string)+1);
    strcpy (text->string, string);

    dor_global_txtaln_get_value(&txtaln);
    text->halign = txtaln.halign;
    text->valign = txtaln.valign;

    dor_global_txtexp_get_value(&text->textexp);

    dor_global_txtfnt_get_value(&text->font);

    dor_global_txthgt_get_value(&text->textheight);

    dor_global_txtpat_get_value(&text->textpath);

    dor_global_txtprc_get_value(&text->precision);

    dor_global_txtspc_get_value(&text->textspace);

    dor_global_txtupvec_get_value(&txtupvec);
    text->xup = txtupvec.xup;
    text->yup = txtupvec.yup;

    dor_text_calculate_text_plane(position,u,v,text->planematrix);

    text->alternate_object_lines = DcNullPtr;
    text->alternate_object_triangles = DcNullPtr;

    textobject = dor_object_create(doe_text_class_id,text);

    return(textobject);
}
/*
 ======================================================================
 */

void dor_text_destroy (dot_object *object)
{
    struct text *text;

    text = (struct text *)(object->data);

    if (text->string != NULL){
	dor_space_deallocate(text->string);
    }

    if (text->alternate_object_lines != NULL){
	dor_object_check_deletion(text->alternate_object_lines);
    }

    if (text->alternate_object_triangles != NULL){
	dor_object_check_deletion(text->alternate_object_triangles);
    }

    dor_space_deallocate(text);
}
/*
 ======================================================================
 */

void dor_text_print (dot_object *textobject)
{
    struct text *text;

    text = (struct text *)(textobject->data);

    if (text == NULL){
	dor_print_output("text is NULL");
	return;
    }

    sprintf(dor_print_get_line(), "position (%f, %f, %f)", 
	    text->position[0], text->position[1], text->position[2]) ;
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(), "direction 1 (%f, %f, %f)", 
	    text->u[0], text->u[1], text->u[2]) ;
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(), "direction 2 (%f, %f, %f)", 
	    text->v[0], text->v[1], text->v[2]) ;
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(), "string = '%s'", text->string) ;
    dor_print_output(dor_print_get_line());
}
/*
 ======================================================================
 */

dot_object *dor_text_update_alternate (dot_object *object)
{
    DtRepType reptype;
    dot_object *alt;

    dor_global_reptyp_get_value(&reptype);

    switch (reptype) {
    case DcPoints:
    case DcWireframe:
    case DcOutlines:
	alt = dor_text_update_lines(object);
	break;
    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
	alt = dor_text_update_triangles(object);
	break;
    default:
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_text_update_alternate",
		   "unknown reptype");
	alt = DcNullPtr;
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_text_update_lines (dot_object *object)
{
    struct text *text;
    DtFlag changed;
    dot_object *alt;
   
    text = (struct text *)(object->data);

    changed = dor_text_check_attributes(text);

    if ((text->alternate_object_lines != DcNullPtr) && !changed) {
	alt = text->alternate_object_lines;

    } else {
	if (text->alternate_object_lines != DcNullPtr) {
	    dor_object_get_method(text->alternate_object_lines,
				  DcMethodDestroy) (text->alternate_object_lines);

	    text->alternate_object_lines = DcNullPtr;
	}

	dor_text_create_alternate_geometry_lines(text);
	alt = text->alternate_object_lines;

	if (text->alternate_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_text_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_text_update_triangles (dot_object *object)
{
    struct text *text;
    DtInt fonttype;
    dot_font_state ts;
    DtFlag changed;
    dot_object *alt;

    text = (struct text *)(object->data);

    changed = dor_text_check_attributes(text);

    ts.font_index = text->font;
    fonttype = dor_font_get_type(&ts);

    if (fonttype == dod_font_type_polygonal) {
	if ((text->alternate_object_triangles != DcNullPtr) && 
	    !changed) {
	    alt = text->alternate_object_triangles;

	} else {
	    if (text->alternate_object_triangles != DcNullPtr) {
		dor_object_get_method(text->alternate_object_triangles,
				      DcMethodDestroy) (text->alternate_object_triangles);

		text->alternate_object_triangles = DcNullPtr;
	    }

	    dor_text_create_alternate_geometry_triangles(text);
	    alt = text->alternate_object_triangles;

	    if (text->alternate_object_triangles == DcNullPtr) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_text_update_triangles", "");
	    }
	}
    } else {
	if ((text->alternate_object_lines != DcNullPtr) && !changed){
	    alt = text->alternate_object_lines;

	} else {
	    if (text->alternate_object_lines != DcNullPtr) {
		dor_object_get_method(text->alternate_object_lines,
				      DcMethodDestroy) (text->alternate_object_lines);

		text->alternate_object_lines = DcNullPtr;
	    }	

	    dor_text_create_alternate_geometry_lines(text);
	    alt = text->alternate_object_lines;

	    if (text->alternate_object_lines == DcNullPtr) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_text_update_triangles", "");
	    }
	}
    }
    return alt;
}
/*
 ======================================================================
 */

void dor_text_create_alternate_geometry_complex_triangle (
    struct text_decomp_data *decompdata,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
   DtReal *vtxptr;
   DtReal vertices[9];
   DtInt edge_enable;

   edge_enable = dor_text_enable_edges_complex(gpolydvtx1, gpolydvtx2, gpolydvtx3);

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

   dor_trilst_add_triangle_with_edges(decompdata->alternate_object_triangles,
				      vertices,edge_enable);
}

/*
 ======================================================================
 */

void dor_text_create_alternate_geometry_line_polygon (
    struct text *text,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtInt vertexno;
    DtRealTriple wcsvertices[1000];

    for (vertexno=0;vertexno<vertexcount;vertexno++){
	dor_math_pnttrns_fast(vertices[vertexno],text->planematrix,
			      wcsvertices[vertexno]);
    }

    /*  Since the polygonal description does not include the original point
	replicated, add it as the last vertex of the connected line list.  */

    dor_math_pnttrns_fast(vertices[0],text->planematrix,
			  wcsvertices[vertexcount]);

    for (vertexno=0;vertexno<vertexcount;vertexno++){
	dor_linlst_add_line(text->alternate_object_lines,wcsvertices[vertexno]);
    }
}
/*
 ======================================================================
 */

void dor_text_create_alternate_geometry_line_polyline (
    struct text *text,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtInt vertexno;
    DtRealTriple wcsvertices[1000];

    for (vertexno=0;vertexno<vertexcount;vertexno++){
	dor_math_pnttrns_fast(vertices[vertexno],text->planematrix,
			      wcsvertices[vertexno]);
    }

    for (vertexno=0;vertexno<(vertexcount-1);vertexno++){
	dor_linlst_add_line(text->alternate_object_lines,wcsvertices[vertexno]);
    }
}
/*
 ======================================================================
 */

void dor_text_create_alternate_geometry_lines (struct text *text)
{
    dot_object *linlstobj;
    dot_font_state ts;

    /* set up text state */

    ts.string = text->string;
    ts.length = strlen(ts.string);
    ts.position.x = 0.;
    ts.position.y = 0.;

    ts.height = text->textheight;

    ts.charexp = text->textexp;

    ts.path = text->textpath;

    ts.xup = text->xup;
    ts.yup = text->yup;

    ts.spacing = text->textspace;

    ts.halign = text->halign;
    ts.valign = text->valign;

    ts.font_index = text->font;

    ts.user_data = (char *) text;

    ts.polyline = dor_text_create_alternate_geometry_line_polyline;
    ts.polygon = dor_text_create_alternate_geometry_line_polygon;

    linlstobj = dor_linlst_open(DcRGB,DcLoc,ts.length*50);
    text->alternate_object_lines = linlstobj;

    dor_font_draw_text(&ts);

    dor_linlst_close(linlstobj);
}
/*
 ======================================================================
 */

void dor_text_create_alternate_geometry_simple_triangle (
    struct text_decomp_data *decompdata,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
   DtReal *vtxptr;
   DtReal vertices[9];
   DtInt edge_enable;

   edge_enable = dor_text_enable_edges_simple(vertex1, vertex2, vertex3);

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

void dor_text_create_alternate_geometry_triangle_error (
    struct text *text,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DDerror(ERR_VALUE_OUT_OF_RANGE,
	       "dor_text_create_alternate_geometry_triangle_error",
	       "font is polyline");
}
/*
 ======================================================================
 */

DtInt doe_text_vertexcount;

void dor_text_create_alternate_geometry_triangle_polygon (
    struct text *text,
    DtInt vertexcount,
    DtRealTriple *vertices)
{
    DtInt vertexno;
    struct text_decomp_data decompdata;

    decompdata.wcsvertices = (DtRealTriple *)dor_space_allocate(2000*sizeof(DtRealTriple));
    decompdata.alternate_object_triangles = text->alternate_object_triangles;

    for (vertexno=0;vertexno<vertexcount;vertexno++) {
	dor_math_pnttrns_fast(vertices[vertexno],text->planematrix,
			      decompdata.wcsvertices[vertexno]);
    }
   
    dor_gpolyd_create_polygon();
    dor_gpolyd_create_contour();

    for (vertexno=0;vertexno<vertexcount;vertexno++) {
	dor_gpolyd_create_contour_vertex(vertexno,
					 decompdata.wcsvertices[vertexno][0],
					 decompdata.wcsvertices[vertexno][1],
					 decompdata.wcsvertices[vertexno][2]);
    }

    dor_gpolyd_close_contour();
    dor_gpolyd_close_polygon();

    doe_text_vertexcount = vertexcount;
    dor_gpolyd_triangulate_polygon(DcComplex,
			dor_text_create_alternate_geometry_simple_triangle,
			dor_text_create_alternate_geometry_complex_triangle,
			&decompdata);

    dor_space_deallocate(decompdata.wcsvertices);
}
/*
 ======================================================================
 */

void dor_text_create_alternate_geometry_triangles (struct text *text)
{
    dot_object *trilstobj;
    dot_font_state ts;

    /* set up text state */

    ts.string = text->string;
    ts.length = strlen(ts.string);
    ts.position.x = 0.;
    ts.position.y = 0.;

    ts.height = text->textheight;

    ts.charexp = text->textexp;

    ts.path = text->textpath;

    ts.xup = text->xup;
    ts.yup = text->yup;

    ts.spacing = text->textspace;

    ts.halign = text->halign;
    ts.valign = text->valign;

    ts.font_index = text->font;

    ts.user_data = (char *) text;

    ts.polyline = dor_text_create_alternate_geometry_triangle_error;
    ts.polygon = dor_text_create_alternate_geometry_triangle_polygon;

    trilstobj = dor_trilst_open(DcRGB,DcLoc,ts.length*300);
    text->alternate_object_triangles = trilstobj;

    dor_font_draw_text(&ts);

    dor_trilst_close(trilstobj);
}
/*
 ======================================================================
 */

DtFlag dor_text_check_attributes (struct text *text)
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
    if (text->halign != txtaln.halign) {
	text->halign = txtaln.halign;
	returnflag = DcTrue;
    }
    if (text->valign != txtaln.valign) {
	text->valign = txtaln.valign;
	returnflag = DcTrue;
    }

    dor_global_txtexp_get_value(&textexp);
    if (text->textexp != textexp) {
	text->textexp = textexp;
	returnflag = DcTrue;
    }

    dor_global_txtfnt_get_value(&font);
    if (text->font != font) {
	text->font = font;
	returnflag = DcTrue;
    }

    dor_global_txthgt_get_value(&textheight);
    if (text->textheight != textheight) {
	text->textheight = textheight;
	returnflag = DcTrue;
    }

    dor_global_txtpat_get_value(&textpath);
    if (text->textpath != textpath) {
	text->textpath = textpath;
	returnflag = DcTrue;
    }

    dor_global_txtprc_get_value(&precision);
    if (text->precision != precision) {
	text->precision = precision;
	returnflag = DcTrue;
    }

    dor_global_txtspc_get_value(&textspace);
    if (text->textspace != textspace) {
	text->textspace = textspace;
	returnflag = DcTrue;
    }

    dor_global_txtupvec_get_value(&txtupvec);
    if (text->xup != txtupvec.xup) {
	text->xup = txtupvec.xup;
	returnflag = DcTrue;
    }
    if (text->yup != txtupvec.yup) {
	text->yup = txtupvec.yup;
	returnflag = DcTrue;
    }

    return (returnflag);
} 
/*
 ======================================================================
 */

/*  Compute matrix that maps text from text plane into modeling space.  */

void dor_text_calculate_text_plane (
    DtPoint3  position,
    DtVector3 u,
    DtVector3 v,
    DtMatrix4x4 matrix)
{
    DtReal x[3], y[3], z[3];

    dor_math_crossproduct(u[0],u[1],u[2],v[0],v[1],v[2],&z[0],&z[1],&z[2]);

    if (!dor_math_renormalize_fast(z)) {
    /* degenerate text directions */

	x[0] = 1;
	x[1] = 0;
	x[2] = 0;

	y[0] = 0;
	y[1] = 1;
	y[2] = 0;

	z[0] = 0;
	z[1] = 0;
	z[2] = 1;
    }
    else {
	dor_math_crossproduct(v[0],v[1],v[2],z[0],z[1],z[2],&x[0],&x[1],&x[2]);

   	dor_math_renormalize_fast(x);

      	dor_math_crossproduct(z[0],z[1],z[2],x[0],x[1],x[2],&y[0],&y[1],&y[2]);

   	dor_math_renormalize_fast(y);
    }

    /* compute matrix */

    matrix[0][0] = x[0];
    matrix[1][0] = x[1];
    matrix[2][0] = x[2];
    matrix[3][0] = 0.; 

    matrix[0][1] = y[0];
    matrix[1][1] = y[1];
    matrix[2][1] = y[2];
    matrix[3][1] = 0.;

    matrix[0][2] = z[0];
    matrix[1][2] = z[1];
    matrix[2][2] = z[2];
    matrix[3][2] = 0.;

    matrix[0][3] = position[0];
    matrix[1][3] = position[1];
    matrix[2][3] = position[2];
    matrix[3][3] = 1.;

}
/*
 ======================================================================
 */

void dor_text_compute_bounding_volume (dot_object *object)
{
    struct text *text;
    dot_font_state ts;
    dot_txtaln txtaln;
    dot_txtupvec txtupvec;

    text = (struct text *)(object->data);

    ts.string = text->string;

    ts.length = strlen(ts.string);

    ts.position.x = 0.;
    ts.position.y = 0.;

    dor_global_txthgt_get_value(&ts.height);

    dor_global_txtexp_get_value(&ts.charexp);

    dor_global_txtpat_get_value(&ts.path);

    dor_global_txtupvec_get_value(&txtupvec);
    ts.xup = txtupvec.xup;
    ts.yup = txtupvec.yup;

    dor_global_txtspc_get_value(&ts.spacing);

    dor_global_txtaln_get_value(&txtaln);
    ts.halign = txtaln.halign;
    ts.valign = txtaln.valign;
 
    dor_global_txtfnt_get_value(&ts.font_index);

    ts.polyline = dor_text_cmpbndvol_point_callback;
    ts.polygon = dor_text_cmpbndvol_point_callback;
    ts.user_data = (char *)text;
	
    dor_font_draw_text(&ts);
}

void dor_text_cmpbndvol_point_callback (
    struct text *text,
    DtInt vertexcount,
    DtRealTriple *points)
{
    DtInt vertexno;
    DtRealTriple wcsvertices[1000];

    for (vertexno=0;vertexno<vertexcount;vertexno++){
	dor_math_pnttrns_fast(points[vertexno],text->planematrix,                        wcsvertices[vertexno]);
    }

    dor_system_report_bounding_points (vertexcount, (DtReal*)wcsvertices);
}
/*
 ======================================================================
 */

DtInt dor_text_enable_edges_complex (
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DtInt edge_enable;

    edge_enable = dod_no_enabled_edges;
    if (dor_text_check_complex_edge(gpolydvtx1, gpolydvtx2))
	    edge_enable |= dod_edge1_enabled;
    if (dor_text_check_complex_edge(gpolydvtx2, gpolydvtx3))
	    edge_enable |= dod_edge2_enabled;
    if (dor_text_check_complex_edge(gpolydvtx3, gpolydvtx1))
	    edge_enable |= dod_edge3_enabled;

    return (edge_enable);

}

DtFlag dor_text_check_complex_edge (
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

    } else if (vtx1->vertex1 == ((vtx2->vertex1+1)%doe_text_vertexcount) ||
	       vtx2->vertex1 == ((vtx1->vertex1+1)%doe_text_vertexcount)) {
        return (DcTrue);
    }

    return (DcFalse);
}
/*
 ======================================================================
 */

DtInt dor_text_enable_edges_simple (
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
    DtInt edge_enable;

    edge_enable = dod_no_enabled_edges;
    if (vertex1 == ((vertex2+1)%doe_text_vertexcount) ||
	vertex2 == ((vertex1+1)%doe_text_vertexcount)) {
        edge_enable |= dod_edge1_enabled;
    }
    if (vertex2 == ((vertex3+1)%doe_text_vertexcount) ||
	vertex3 == ((vertex2+1)%doe_text_vertexcount)) {
        edge_enable |= dod_edge2_enabled;
    }
    if (vertex3 == ((vertex1+1)%doe_text_vertexcount) ||
	vertex1 == ((vertex3+1)%doe_text_vertexcount)) {
        edge_enable |= dod_edge3_enabled;
    }

    return (edge_enable);
}
/*
 ======================================================================
 */

void dor_text_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_text_class_id))
	    return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    if ((reptype == DcPoints) || 
	(reptype == DcWireframe) ||
	(reptype == DcOutlines)) {
   	dor_text_pick_lines(object);

    } else if ((reptype == DcSurface) ||
	       (reptype == DcSurfaceWireframeEdges) ||
	       (reptype == DcSurfaceOutlineEdges)) {
   	dor_text_pick_triangles(object);

    } else {
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_text_pick",
		   "unknown reptype");
    }
}
/*
 ======================================================================
 */

void dor_text_pick_lines (dot_object *object)
{
    struct text *text;
    DtFlag changed;

    text = (struct text *)(object->data);

    changed = dor_text_check_attributes(text);

    if ((text->alternate_object_lines != DcNullPtr) && !changed) {
	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(text->alternate_object_lines,
			      DcMethodPick) (text->alternate_object_lines);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    } else {
	if (text->alternate_object_lines != NULL) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(text->alternate_object_lines,
				  DcMethodDestroy) (text->alternate_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    text->alternate_object_lines = NULL;
	}

	dor_text_create_alternate_geometry_lines(text);

	if (text->alternate_object_lines == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_text_pick_lines", "");
	    return;
	}

	doe_system_alternate_object_execution_flag = DcTrue;

	dor_object_get_method(text->alternate_object_lines,
			      DcMethodPick) (text->alternate_object_lines);

	doe_system_alternate_object_execution_flag = DcFalse;

	return;
    }
}
/*
 ======================================================================
 */

void dor_text_pick_triangles (dot_object *object)
{
    struct text *text;
    DtInt fonttype;
    dot_font_state ts;
    DtFlag changed;

    text = (struct text *)(object->data);

    changed = dor_text_check_attributes(text);

    ts.font_index = text->font;
    fonttype = dor_font_get_type(&ts);

    if (fonttype == dod_font_type_polygonal) {
	if ((text->alternate_object_triangles != DcNullPtr) && !changed) {
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(text->alternate_object_triangles,
				  DcMethodPick) (text->alternate_object_triangles);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    return;
	} else {
	    if (text->alternate_object_triangles != NULL) {
		doe_system_alternate_object_execution_flag = DcTrue;

		dor_object_get_method(text->alternate_object_triangles,
				DcMethodDestroy) (text->alternate_object_triangles);

		doe_system_alternate_object_execution_flag = DcFalse;

		text->alternate_object_triangles = NULL;
	    }

	    dor_text_create_alternate_geometry_triangles(text);

	    if (text->alternate_object_triangles == NULL) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_text_pick_triangles", "");
		return;
	    }

	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(text->alternate_object_triangles,
				  DcMethodPick) (text->alternate_object_triangles);

	    doe_system_alternate_object_execution_flag = DcFalse;
	}
    } else {
	if ((text->alternate_object_lines != DcNullPtr) && !changed){
	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(text->alternate_object_lines,
				  DcMethodPick) (text->alternate_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;

	    return;
	} else {
	    if (text->alternate_object_lines != NULL) {
		doe_system_alternate_object_execution_flag = DcTrue;

		dor_object_get_method(text->alternate_object_lines,
				      DcMethodDestroy) (text->alternate_object_lines);

		doe_system_alternate_object_execution_flag = DcFalse;

		text->alternate_object_lines = NULL;
	    }

	    dor_text_create_alternate_geometry_lines(text);

	    if (text->alternate_object_lines == NULL) {
		DDerror(ERR_CANT_CREATE_ALTOBJ,
			   "dor_text_pick_triangles", "");
		return;
	    }

	    doe_system_alternate_object_execution_flag = DcTrue;

	    dor_object_get_method(text->alternate_object_lines,
				  DcMethodPick) (text->alternate_object_lines);

	    doe_system_alternate_object_execution_flag = DcFalse;
	}
    }

    return;
}
/*
 ======================================================================
 */

void dor_text_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_text_class_id))
	    return;

    alt = dor_text_update_alternate (object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}
