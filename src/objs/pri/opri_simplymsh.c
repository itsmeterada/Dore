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
	int dor_simplymsh_initialize 
	dot_object * dor_simplymsh_create 
	int dor_simplymsh_destroy
	int dor_simplymsh_print 
	dot_object * dor_simplymsh_update_alternate
	dot_object * dor_simplymsh_update_lines 
	dot_object * dor_simplymsh_update_tris 
	int dor_simplymsh_create_alternate_geometry_complex_triangle 
	dot_object * dor_simplymsh_create_alternate_geom_lines
	dot_object * dor_simplymsh_create_alternate_geom_tris 
	int dor_simplymsh_calculate_vertex_normals 
	int dor_simplymsh_compute_bounding_volume 
	int dor_simplymsh_enable_edges_complex 
	DtFlag dor_simplymsh_check_complex_edge 
	int dor_simplymsh_enable_edges_simple 
	int dor_simplymsh_pick 
	int dor_simplymsh_pick_lines 
	int dor_simplymsh_pick_tris 
	int dor_simplymsh_polygon_normal 
	int dor_simplymsh_render_standard 

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/gpolyd.h>
#include <dore/internal/system.h>
#include <dore/dore_develop/private/simplymsh.h>
#include <dore/dore_develop/private/linlst.h>
#include <dore/dore_develop/private/trilst.h>

/*
 ======================================================================
 */

DtInt doe_simplymsh_class_id;

static DtMethodEntry simplymsh_methods[] = {
    {DcMethodCmpBndVolume, (DtMethodPtr)dor_simplymsh_compute_bounding_volume},
    {DcMethodDestroy,       (DtMethodPtr)dor_simplymsh_destroy},
    {DcMethodPick,           (DtMethodPtr)dor_simplymsh_pick},
    {DcMethodPrint,           (DtMethodPtr)dor_simplymsh_print},
    {DcMethodUpdStdAltObj,    (DtMethodPtr)dor_simplymsh_update_alternate},
    {DcMethodUpdStdAltLineObj,(DtMethodPtr)dor_simplymsh_update_lines},
    {DcMethodStdRenderDisplay,(DtMethodPtr)dor_simplymsh_render_standard}
};

void dor_simplymsh_initialize (void)
{
    doe_simplymsh_class_id = dor_class_add ("DoSimplePolygonMesh", 7,
					    simplymsh_methods, DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_simplymsh_create (
    DtColorModel colormodel,
    DtVertexType vertextype,
    DtInt vertexcount,
    DtReal *vertices,
    DtInt polygoncount,
    DtInt contours[],
    DtInt vertexlist[],
    DtShapeType shape,
    DtFlag smoothflag)
{
   static DtFlag initialized = DcFalse;
   struct simplymsh *simplymsh;
   DtInt polyvertexcount;
   DtInt polygonno;
   DtReal *vertexptr;
   DtInt vertexno;
   DtVertexType basevtxtype;
   DtInt uv, uvw;
   dot_object *simplymshobject;

   if (!initialized) {
     	 dor_simplymsh_initialize();
      	initialized = DcTrue;
   }

   simplymsh = (struct simplymsh *)dor_space_allocate(sizeof *simplymsh);

   simplymsh->colormodel = colormodel;

   basevtxtype = dod_system_inq_vertex_basetype(vertextype);
   simplymsh->vertextype = basevtxtype;
   simplymsh->fullvtxtype = vertextype;

   simplymsh->polygon_count = polygoncount;

   simplymsh->contours =
   	(DtInt *)dor_space_allocate((polygoncount)*sizeof(DtInt));

   polyvertexcount = 0;

   for (polygonno=0;polygonno<polygoncount;polygonno++) {
   	simplymsh->contours[polygonno] = contours[polygonno];
   	polyvertexcount += contours[polygonno];
   }

   simplymsh->vertexlist = (DtInt *)dor_space_allocate(
				(polyvertexcount)*sizeof(DtInt));

   for (vertexno=0;vertexno<polyvertexcount;vertexno++) {
   	simplymsh->vertexlist[vertexno] = vertexlist[vertexno];
   }

   simplymsh->vertex_count = vertexcount;

   simplymsh->vertex_location = (DtRealTriple *)dor_space_allocate(
   				vertexcount*(sizeof (DtRealTriple)));

   simplymsh->vertex_normal = (DtRealTriple *)NULL;

   simplymsh->vertex_color = (DtColorRGB *)NULL;

   simplymsh->vertex_alpha = (DtReal *)NULL;


   /* Texture coordinates */

   simplymsh->uv_list = (DtRealCouple **)dor_space_allocate(
                dod_system_max_vertex_uv_cnt*sizeof (DtRealCouple *));
   simplymsh->uvw_list = (DtRealTriple **)dor_space_allocate(
                dod_system_max_vertex_uvw_cnt*sizeof (DtRealTriple *));

   for (uv=0; uv < dod_system_max_vertex_uv_cnt; uv++) {
        simplymsh->uv_list[uv] = NULL;
   }
   for (uvw=0; uvw < dod_system_max_vertex_uvw_cnt; uvw++) {
        simplymsh->uvw_list[uvw] = NULL;
   }

   simplymsh->uv_count = dod_system_inq_vertex_uv_cnt(vertextype);
   simplymsh->uvw_count = dod_system_inq_vertex_uvw_cnt(vertextype);

   for (uv=0; uv< simplymsh->uv_count; uv++) {
        simplymsh->uv_list[uv] = (DtRealCouple *)dor_space_allocate(
                                vertexcount*(sizeof (DtRealCouple)));
   }

   for (uvw=0; uvw< simplymsh->uvw_count; uvw++) {
        simplymsh->uvw_list[uvw] = (DtRealTriple *)dor_space_allocate(
                                vertexcount*(sizeof (DtRealTriple)));
   }

   /**/

    vertexptr = vertices;

    if (basevtxtype & DcLocNrm)
        simplymsh->vertex_normal = (DtRealTriple *)
            dor_space_allocate(vertexcount*(sizeof (DtRealTriple)));

    if (basevtxtype & DcLocClr)
        simplymsh->vertex_color = (DtColorRGB *)
            dor_space_allocate(vertexcount*(sizeof (DtColorRGB)));

    if (basevtxtype & DcLocAlpha)
        simplymsh->vertex_alpha = (DtReal *)
            dor_space_allocate(vertexcount*(sizeof (DtReal)));

    for (vertexno=0; vertexno<vertexcount; vertexno++) {
        simplymsh->vertex_location[vertexno][0] = *vertexptr++;
        simplymsh->vertex_location[vertexno][1] = *vertexptr++;
        simplymsh->vertex_location[vertexno][2] = *vertexptr++;

        if (basevtxtype & DcLocNrm) {
            simplymsh->vertex_normal[vertexno][0] = *vertexptr++;
            simplymsh->vertex_normal[vertexno][1] = *vertexptr++;
            simplymsh->vertex_normal[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocClr) {
            simplymsh->vertex_color[vertexno][0] = *vertexptr++;
            simplymsh->vertex_color[vertexno][1] = *vertexptr++;
            simplymsh->vertex_color[vertexno][2] = *vertexptr++;
        }

        if (basevtxtype & DcLocAlpha) {
            simplymsh->vertex_alpha[vertexno] = *vertexptr++;
        }

        for (uv=0; uv < simplymsh->uv_count; uv++) {
            simplymsh->uv_list[uv][vertexno][0] = *vertexptr++;
            simplymsh->uv_list[uv][vertexno][1] = *vertexptr++;
        }

        for (uvw=0; uvw < simplymsh->uvw_count; uvw++) {
            simplymsh->uvw_list[uvw][vertexno][0] = *vertexptr++;
            simplymsh->uvw_list[uvw][vertexno][1] = *vertexptr++;
            simplymsh->uvw_list[uvw][vertexno][2] = *vertexptr++;
        }
    }

   simplymsh->shape = shape;

   if (smoothflag) {
	if (!(basevtxtype & DcLocNrm)) {

   		simplymsh->vertex_normal = (DtRealTriple *)dor_space_allocate(
   					vertexcount*(sizeof (DtRealTriple)));

   		dor_simplymsh_calculate_vertex_normals(simplymsh);

                /* add normals to the vertextype */
  		simplymsh->vertextype |= DcLocNrm;
		simplymsh->fullvtxtype |= DcLocNrm;
   	}
   }

   simplymsh->smoothflag = smoothflag;

   simplymsh->alt_object_lines = NULL;
   simplymsh->alt_object_tris = NULL;

   simplymshobject = dor_object_create(doe_simplymsh_class_id,simplymsh);

   return(simplymshobject);
}
/*
 ======================================================================
 */

void dor_simplymsh_destroy (dot_object *object)
{
    int i;
    struct simplymsh *simplymsh;

    simplymsh = (struct simplymsh *)(object->data);

    if (simplymsh->vertex_location != NULL) {
	dor_space_deallocate(simplymsh->vertex_location);
    }

    if (simplymsh->vertex_normal != NULL) {
	dor_space_deallocate(simplymsh->vertex_normal);
    }

    if (simplymsh->vertex_color != NULL) {
	dor_space_deallocate(simplymsh->vertex_color);
    }

    if (simplymsh->vertex_alpha != NULL) {
	dor_space_deallocate(simplymsh->vertex_alpha);
    }

    for (i=0; i< simplymsh->uv_count; i++) {
	dor_space_deallocate(simplymsh->uv_list[i]);
    }
    dor_space_deallocate(simplymsh->uv_list);

    for (i=0; i< simplymsh->uvw_count; i++) {
	dor_space_deallocate(simplymsh->uvw_list[i]);
    }
    dor_space_deallocate(simplymsh->uvw_list);

    if (simplymsh->contours != NULL) {
	dor_space_deallocate(simplymsh->contours);
    }

    if (simplymsh->vertexlist != NULL) {
	dor_space_deallocate(simplymsh->vertexlist);
    }

    if (simplymsh->alt_object_lines != NULL) {
	dor_object_check_deletion(simplymsh->alt_object_lines);
    }

    if (simplymsh->alt_object_tris != NULL) {
	dor_object_check_deletion(simplymsh->alt_object_tris);
    }

    dor_space_deallocate(simplymsh);
}
/*
 ======================================================================
 */

void dor_simplymsh_print (struct object *object)
{
    struct simplymsh *simplymsh;

    simplymsh = (struct simplymsh *)(object->data);

    if (simplymsh == NULL){
	dor_print_output("simple polygon mesh is NULL");
	return;
    }

    switch (simplymsh->colormodel){
    case DcRGB:
	dor_print_output("colormodel = DcRGB");
	break;

    default:
	dor_print_output("colormodel = UNKNOWN");
	break;
    }

    dor_utilpri_print_vertextype(simplymsh->vertextype);

    switch (simplymsh->shape){
    case DcConvex:
        dor_print_output("shape = convex");
        break;

    case DcConcave:
        dor_print_output("shape = concave");
        break;

    case DcComplex:
        dor_print_output("shape = complex");
        break;
    }

    sprintf(dor_print_get_line(),
	    "smooth flag = %d",simplymsh->smoothflag);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "vertex count = %d",simplymsh->vertex_count);
    dor_print_output(dor_print_get_line());

    sprintf(dor_print_get_line(),
	    "polygon count = %d",simplymsh->polygon_count);
    dor_print_output(dor_print_get_line());

    if (simplymsh->uv_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uv's per vertex = %d", simplymsh->uv_count);
	dor_print_output(dor_print_get_line());
    }

    if (simplymsh->uvw_count > 0) {
	sprintf (dor_print_get_line(), 
		 "uvw's per vertex = %d", simplymsh->uvw_count);
	dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

dot_object *dor_simplymsh_update_alternate (dot_object *object)
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
	alt = dor_simplymsh_update_tris(object);
	break;

    case DcOutlines:
	alt = dor_simplymsh_update_lines(object);
	break;
    default:
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_simplymsh_update_alternate",
		   "unknown reptype");
	alt = DcNullPtr;
	break;
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_simplymsh_update_lines (dot_object *object)
{
    struct simplymsh *simplymsh;
    dot_object *alt;

    simplymsh = (struct simplymsh *)(object->data);

    if (simplymsh->alt_object_lines != DcNullPtr) {
	alt = simplymsh->alt_object_lines;

    } else {
	simplymsh->alt_object_lines =
		dor_simplymsh_create_alternate_geom_lines(simplymsh);
	alt = simplymsh->alt_object_lines;

	if (simplymsh->alt_object_lines == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_simplymsh_update_lines", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

dot_object *dor_simplymsh_update_tris (dot_object *object)
{
    struct simplymsh *simplymsh;
    dot_object *alt;

    simplymsh = (struct simplymsh *)(object->data);

    if (simplymsh->alt_object_tris != DcNullPtr) {
	alt = simplymsh->alt_object_tris;

    } else {
	simplymsh->alt_object_tris =
		dor_simplymsh_create_alternate_geom_tris(simplymsh);
	alt = simplymsh->alt_object_tris;

	if (simplymsh->alt_object_tris == DcNullPtr) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_simplymsh_update_tris", "");
	}
    }
    return alt;
}
/*
 ======================================================================
 */

#define MAXVTXS_CT (27 + dod_system_max_vertex_uv_cnt*6 + \
                        dod_system_max_vertex_uvw_cnt*9)

void dor_simplymsh_create_alternate_geometry_complex_triangle (
    struct simplymsh *simplymsh,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
   DtVertexType vertextype;
   DtReal *vtxptr;
   DtReal vertices[MAXVTXS_CT];
   DtInt vertex1, vertex2;
   DtReal weight1, weight2;
   DtInt uv, uvw;
   DtInt edge_enable;

   edge_enable = dor_simplymsh_enable_edges_complex(simplymsh,
			gpolydvtx1,gpolydvtx2,gpolydvtx3);

   vertextype = simplymsh->vertextype;

   vtxptr = vertices;

   *vtxptr++ = gpolydvtx1->coord[0];
   *vtxptr++ = gpolydvtx1->coord[1];
   *vtxptr++ = gpolydvtx1->coord[2];

   vertex1 = gpolydvtx1->vertex1;

   if (gpolydvtx1->interpolated) {
       vertex2 = gpolydvtx1->vertex2;

       weight1 = gpolydvtx1->weight1;
       weight2 = gpolydvtx1->weight2;

       if (vertextype & DcLocNrm) {
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][0] +
                       weight2*simplymsh->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][1] +
                       weight2*simplymsh->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][2] +
                       weight2*simplymsh->vertex_normal[vertex2][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][0] +
                       weight2*simplymsh->vertex_color[vertex2][0];
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][1] +
                       weight2*simplymsh->vertex_color[vertex2][1];
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][2] +
                       weight2*simplymsh->vertex_color[vertex2][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*simplymsh->vertex_alpha[vertex1] +
                       weight2*simplymsh->vertex_alpha[vertex2];
       }

       for (uv=0; uv < simplymsh->uv_count; uv++) {
           *vtxptr++ = weight1*simplymsh->uv_list[uv][vertex1][0] +
                       weight2*simplymsh->uv_list[uv][vertex2][0];
           *vtxptr++ = weight1*simplymsh->uv_list[uv][vertex1][1] +
                       weight2*simplymsh->uv_list[uv][vertex2][1];
       }
       for (uvw=0; uvw < simplymsh->uvw_count; uvw++) {
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][0] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][0];
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][1] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][1];
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][2] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][2];
       }

   } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = simplymsh->vertex_normal[vertex1][0];
           *vtxptr++ = simplymsh->vertex_normal[vertex1][1];
           *vtxptr++ = simplymsh->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = simplymsh->vertex_color[vertex1][0];
           *vtxptr++ = simplymsh->vertex_color[vertex1][1];
           *vtxptr++ = simplymsh->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = simplymsh->vertex_alpha[vertex1];
       }

       for (uv=0; uv < simplymsh->uv_count; uv++) {
           *vtxptr++ = simplymsh->uv_list[uv][vertex1][0];
           *vtxptr++ = simplymsh->uv_list[uv][vertex1][1];
       }
       for (uvw=0; uvw < simplymsh->uvw_count; uvw++) {
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][0];
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][1];
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][2];
       }
   }

   *vtxptr++ = gpolydvtx2->coord[0];
   *vtxptr++ = gpolydvtx2->coord[1];
   *vtxptr++ = gpolydvtx2->coord[2];

   vertex1 = gpolydvtx2->vertex1;

   if (gpolydvtx2->interpolated) {
       vertex2 = gpolydvtx2->vertex2;

       weight1 = gpolydvtx2->weight1;
       weight2 = gpolydvtx2->weight2;

       if (vertextype & DcLocNrm) {
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][0] +
                       weight2*simplymsh->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][1] +
                       weight2*simplymsh->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][2] +
                       weight2*simplymsh->vertex_normal[vertex2][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][0] +
                       weight2*simplymsh->vertex_color[vertex2][0];
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][1] +
                       weight2*simplymsh->vertex_color[vertex2][1];
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][2] +
                       weight2*simplymsh->vertex_color[vertex2][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*simplymsh->vertex_alpha[vertex1] +
                       weight2*simplymsh->vertex_alpha[vertex2];
       }

       for (uv=0; uv < simplymsh->uv_count; uv++) {
           *vtxptr++ = weight1*simplymsh->uv_list[uv][vertex1][0] +
                       weight2*simplymsh->uv_list[uv][vertex2][0];
           *vtxptr++ = weight1*simplymsh->uv_list[uv][vertex1][1] +
                       weight2*simplymsh->uv_list[uv][vertex2][1];
       }
       for (uvw=0; uvw < simplymsh->uvw_count; uvw++) {
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][0] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][0];
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][1] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][1];
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][2] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][2];
       }

   } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = simplymsh->vertex_normal[vertex1][0];
           *vtxptr++ = simplymsh->vertex_normal[vertex1][1];
           *vtxptr++ = simplymsh->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = simplymsh->vertex_color[vertex1][0];
           *vtxptr++ = simplymsh->vertex_color[vertex1][1];
           *vtxptr++ = simplymsh->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = simplymsh->vertex_alpha[vertex1];
       }

       for (uv=0; uv < simplymsh->uv_count; uv++) {
           *vtxptr++ = simplymsh->uv_list[uv][vertex1][0];
           *vtxptr++ = simplymsh->uv_list[uv][vertex1][1];
       }
       for (uvw=0; uvw < simplymsh->uvw_count; uvw++) {
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][0];
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][1];
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][2];
       }
   }

   *vtxptr++ = gpolydvtx3->coord[0];
   *vtxptr++ = gpolydvtx3->coord[1];
   *vtxptr++ = gpolydvtx3->coord[2];

   vertex1 = gpolydvtx3->vertex1;

   if (gpolydvtx3->interpolated) {
       vertex2 = gpolydvtx3->vertex2;

       weight1 = gpolydvtx3->weight1;
       weight2 = gpolydvtx3->weight2;

       if (vertextype & DcLocNrm) {
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][0] +
                       weight2*simplymsh->vertex_normal[vertex2][0];
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][1] +
                       weight2*simplymsh->vertex_normal[vertex2][1];
           *vtxptr++ = weight1*simplymsh->vertex_normal[vertex1][2] +
                       weight2*simplymsh->vertex_normal[vertex2][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][0] +
                       weight2*simplymsh->vertex_color[vertex2][0];
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][1] +
                       weight2*simplymsh->vertex_color[vertex2][1];
           *vtxptr++ = weight1*simplymsh->vertex_color[vertex1][2] +
                       weight2*simplymsh->vertex_color[vertex2][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = weight1*simplymsh->vertex_alpha[vertex1] +
                       weight2*simplymsh->vertex_alpha[vertex2];
       }

       for (uv=0; uv < simplymsh->uv_count; uv++) {
           *vtxptr++ = weight1*simplymsh->uv_list[uv][vertex1][0] +
                       weight2*simplymsh->uv_list[uv][vertex2][0];
           *vtxptr++ = weight1*simplymsh->uv_list[uv][vertex1][1] +
                       weight2*simplymsh->uv_list[uv][vertex2][1];
       }
       for (uvw=0; uvw < simplymsh->uvw_count; uvw++) {
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][0] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][0];
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][1] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][1];
           *vtxptr++ = weight1*simplymsh->uvw_list[uvw][vertex1][2] +
                       weight2*simplymsh->uvw_list[uvw][vertex2][2];
       }

   } else {
       if (vertextype & DcLocNrm) {
           *vtxptr++ = simplymsh->vertex_normal[vertex1][0];
           *vtxptr++ = simplymsh->vertex_normal[vertex1][1];
           *vtxptr++ = simplymsh->vertex_normal[vertex1][2];
       }
       if (vertextype & DcLocClr) {
           *vtxptr++ = simplymsh->vertex_color[vertex1][0];
           *vtxptr++ = simplymsh->vertex_color[vertex1][1];
           *vtxptr++ = simplymsh->vertex_color[vertex1][2];
       }
       if (vertextype & DcLocAlpha) {
           *vtxptr++ = simplymsh->vertex_alpha[vertex1];
       }

       for (uv=0; uv < simplymsh->uv_count; uv++) {
           *vtxptr++ = simplymsh->uv_list[uv][vertex1][0];
           *vtxptr++ = simplymsh->uv_list[uv][vertex1][1];
       }
       for (uvw=0; uvw < simplymsh->uvw_count; uvw++) {
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][0];
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][1];
           *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][2];
       }
   }

   dor_trilst_add_triangle_with_edges(simplymsh->alt_object_tris,
                                      vertices,edge_enable);

}

/*
 ======================================================================
 */

#define MAXVTXS_L (18 + dod_system_max_vertex_uv_cnt*4 + \
                        dod_system_max_vertex_uvw_cnt*6)

dot_object *dor_simplymsh_create_alternate_geom_lines (
    struct simplymsh *simplymsh)
{
    DtInt vertexlistloc;
    DtInt polygonno;
    DtInt polyvertexcnt;
    DtInt polyvertexno;
    DtInt firstvertex, lastvertex;
    DtInt lvtx0, lvtx1;
    DtUInt i;
    DtUInt uv, uvw;
    DtReal vtxs[MAXVTXS_L];
    dot_object *linlstobj;
    DtInt sum;

    for (i=0,sum=0;i<simplymsh->polygon_count;i++) {
	sum += simplymsh->contours[i];
    }

    linlstobj = dor_linlst_open(DcRGB,simplymsh->fullvtxtype,sum);

    simplymsh->alt_object_lines = linlstobj;

    vertexlistloc = 0;

    for (polygonno=0;polygonno<simplymsh->polygon_count;polygonno++){

	polyvertexcnt = simplymsh->contours[polygonno];
	firstvertex = simplymsh->vertexlist[vertexlistloc];
	lastvertex = simplymsh->vertexlist[vertexlistloc+polyvertexcnt-1];

	for (polyvertexno=0;
	     polyvertexno<polyvertexcnt;
	     polyvertexno++) {

	    if (polyvertexno == polyvertexcnt-1) {
		lvtx0 = lastvertex;
		lvtx1 = firstvertex;
	    } else {
		lvtx0 = simplymsh->vertexlist[vertexlistloc++];
		lvtx1 = simplymsh->vertexlist[vertexlistloc];
	    }

	    i = 0;

	    /* One end of a line */

	    vtxs[i++] = simplymsh->vertex_location[lvtx0][0];
	    vtxs[i++] = simplymsh->vertex_location[lvtx0][1];
	    vtxs[i++] = simplymsh->vertex_location[lvtx0][2];

	    if (simplymsh->vertextype & DcLocNrm) {
		vtxs[i++] = simplymsh->vertex_normal[lvtx0][0];
		vtxs[i++] = simplymsh->vertex_normal[lvtx0][1];
		vtxs[i++] = simplymsh->vertex_normal[lvtx0][2];
	    }

	    if (simplymsh->vertextype & DcLocClr) {
		vtxs[i++] = simplymsh->vertex_color[lvtx0][0];
		vtxs[i++] = simplymsh->vertex_color[lvtx0][1];
		vtxs[i++] = simplymsh->vertex_color[lvtx0][2];
	    }

	    if (simplymsh->vertextype & DcLocAlpha) {
		vtxs[i++] = simplymsh->vertex_alpha[lvtx0];
	    }

	    for (uv=0; uv<simplymsh->uv_count; uv++) {
		vtxs[i++] = simplymsh->uv_list[uv][lvtx0][0]; 
		vtxs[i++] = simplymsh->uv_list[uv][lvtx0][1]; 
	    }

	    for (uvw=0; uvw<simplymsh->uvw_count; uvw++) {
		vtxs[i++] = simplymsh->uvw_list[uvw][lvtx0][0]; 
		vtxs[i++] = simplymsh->uvw_list[uvw][lvtx0][1]; 
		vtxs[i++] = simplymsh->uvw_list[uvw][lvtx0][2]; 
	    }

	    /* The other end of the line */

	    vtxs[i++] = simplymsh->vertex_location[lvtx1][0];
	    vtxs[i++] = simplymsh->vertex_location[lvtx1][1];
	    vtxs[i++] = simplymsh->vertex_location[lvtx1][2];

	    if (simplymsh->vertextype & DcLocNrm) {
		vtxs[i++] = simplymsh->vertex_normal[lvtx1][0];
		vtxs[i++] = simplymsh->vertex_normal[lvtx1][1];
		vtxs[i++] = simplymsh->vertex_normal[lvtx1][2];
	    }

	    if (simplymsh->vertextype & DcLocClr) {
		vtxs[i++] = simplymsh->vertex_color[lvtx1][0];
		vtxs[i++] = simplymsh->vertex_color[lvtx1][1];
		vtxs[i++] = simplymsh->vertex_color[lvtx1][2];
	    }

	    if (simplymsh->vertextype & DcLocAlpha) {
		vtxs[i++] = simplymsh->vertex_alpha[lvtx1];
	    }

	    for (uv=0; uv<simplymsh->uv_count; uv++) {
		vtxs[i++] = simplymsh->uv_list[uv][lvtx1][0]; 
		vtxs[i++] = simplymsh->uv_list[uv][lvtx1][1]; 
	    }
	
	    for (uvw=0; uvw<simplymsh->uvw_count; uvw++) {
		vtxs[i++] = simplymsh->uvw_list[uvw][lvtx1][0]; 
		vtxs[i++] = simplymsh->uvw_list[uvw][lvtx1][1]; 
		vtxs[i++] = simplymsh->uvw_list[uvw][lvtx1][2]; 
	    }

	    /* Add the line to the list */

	    dor_linlst_add_line(linlstobj,vtxs);
	}

	vertexlistloc++;
    }

    dor_linlst_close(linlstobj);

    return(linlstobj);
}
/*
 ======================================================================
 */

#define MAXVTXS_ST (27 + dod_system_max_vertex_uv_cnt*6 + \
			dod_system_max_vertex_uvw_cnt*9)

void dor_simplymsh_create_alternate_geometry_simple_triangle (
    struct simplymsh *simplymsh,
    DtInt vertex1,
    DtInt vertex2,
    DtInt vertex3)
{
   DtVertexType vertextype;
   DtReal *vtxptr;
   DtReal vertices[MAXVTXS_ST];
   DtInt uv, uvw;
   DtInt edge_enable;

#ifdef DEBUG
printf("dor_simplymsh_create_alternate_geometry_simple_triangle %d %d %d\n",
	vertex1, vertex2, vertex3);
#endif

    edge_enable =
        dor_simplymsh_enable_edges_simple(simplymsh,vertex1,vertex2,vertex3);

    vertextype = simplymsh->vertextype;

    vtxptr = vertices;

    /* First vertex */

    *vtxptr++ = simplymsh->vertex_location[vertex1][0];
    *vtxptr++ = simplymsh->vertex_location[vertex1][1];
    *vtxptr++ = simplymsh->vertex_location[vertex1][2];

    if (vertextype & DcLocNrm) {
       *vtxptr++ = simplymsh->vertex_normal[vertex1][0];
       *vtxptr++ = simplymsh->vertex_normal[vertex1][1];
       *vtxptr++ = simplymsh->vertex_normal[vertex1][2];
    }

    if (vertextype & DcLocClr) {
       *vtxptr++ = simplymsh->vertex_color[vertex1][0];
       *vtxptr++ = simplymsh->vertex_color[vertex1][1];
       *vtxptr++ = simplymsh->vertex_color[vertex1][2];
    }

    if (vertextype & DcLocAlpha) {
       *vtxptr++ = simplymsh->vertex_alpha[vertex1];
    }

    for (uv=0; uv<simplymsh->uv_count; uv++) {
        *vtxptr++ = simplymsh->uv_list[uv][vertex1][0]; 
        *vtxptr++ = simplymsh->uv_list[uv][vertex1][1]; 
    }

    for (uvw=0; uvw<simplymsh->uvw_count; uvw++) {
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][0]; 
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][1]; 
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex1][2]; 
    }

    /* Second vertex */

    *vtxptr++ = simplymsh->vertex_location[vertex2][0];
    *vtxptr++ = simplymsh->vertex_location[vertex2][1];
    *vtxptr++ = simplymsh->vertex_location[vertex2][2];

    if (vertextype & DcLocNrm) {
       *vtxptr++ = simplymsh->vertex_normal[vertex2][0];
       *vtxptr++ = simplymsh->vertex_normal[vertex2][1];
       *vtxptr++ = simplymsh->vertex_normal[vertex2][2];
    }

    if (vertextype & DcLocClr) {
       *vtxptr++ = simplymsh->vertex_color[vertex2][0];
       *vtxptr++ = simplymsh->vertex_color[vertex2][1];
       *vtxptr++ = simplymsh->vertex_color[vertex2][2];
    }

    if (vertextype & DcLocAlpha) {
       *vtxptr++ = simplymsh->vertex_alpha[vertex2];
    }

    for (uv=0; uv<simplymsh->uv_count; uv++) {
        *vtxptr++ = simplymsh->uv_list[uv][vertex2][0]; 
        *vtxptr++ = simplymsh->uv_list[uv][vertex2][1]; 
    }

    for (uvw=0; uvw<simplymsh->uvw_count; uvw++) {
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex2][0]; 
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex2][1]; 
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex2][2]; 
    }

    /* Third vertex */

    *vtxptr++ = simplymsh->vertex_location[vertex3][0];
    *vtxptr++ = simplymsh->vertex_location[vertex3][1];
    *vtxptr++ = simplymsh->vertex_location[vertex3][2];

    if (vertextype & DcLocNrm) {
       *vtxptr++ = simplymsh->vertex_normal[vertex3][0];
       *vtxptr++ = simplymsh->vertex_normal[vertex3][1];
       *vtxptr++ = simplymsh->vertex_normal[vertex3][2];
    }

    if (vertextype & DcLocClr) {
       *vtxptr++ = simplymsh->vertex_color[vertex3][0];
       *vtxptr++ = simplymsh->vertex_color[vertex3][1];
       *vtxptr++ = simplymsh->vertex_color[vertex3][2];
    }

    if (vertextype & DcLocAlpha) {
       *vtxptr++ = simplymsh->vertex_alpha[vertex3];
    }

    for (uv=0; uv<simplymsh->uv_count; uv++) {
        *vtxptr++ = simplymsh->uv_list[uv][vertex3][0]; 
        *vtxptr++ = simplymsh->uv_list[uv][vertex3][1]; 
    }

    for (uvw=0; uvw<simplymsh->uvw_count; uvw++) {
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex3][0]; 
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex3][1]; 
        *vtxptr++ = simplymsh->uvw_list[uvw][vertex3][2]; 
    }

    /* Add triangle to list */

    dor_trilst_add_triangle_with_edges(simplymsh->alt_object_tris,
                                       vertices,edge_enable);

}
/*
 ======================================================================
 */

DtInt doe_simplymsh_polygonno;

dot_object *dor_simplymsh_create_alternate_geom_tris (
    struct simplymsh *simplymsh)
{
    dot_object *trilstobj;
    DtInt vertexlistloc;
    DtInt polygonno;
    DtInt vertexno;
    DtInt contourvertexcount;
    DtInt contourvertexno;
    DtInt i,sum;

    for (i=0,sum=0;i<simplymsh->polygon_count;i++) {
      	sum += simplymsh->contours[i];
    }

    trilstobj = dor_trilst_open(DcRGB,simplymsh->fullvtxtype,sum);

    simplymsh->alt_object_tris = trilstobj;

    vertexlistloc = 0;

#ifdef DEBUG
printf("polygon_count %d\n", simplymsh->polygon_count);
#endif
    for (polygonno=0;polygonno<simplymsh->polygon_count;polygonno++) {
   	dor_gpolyd_create_polygon();

   	dor_gpolyd_create_contour();
   
   	contourvertexcount = simplymsh->contours[polygonno];

#ifdef DEBUG
printf("contourvertexcount%d\n", contourvertexcount);
#endif
   	for (contourvertexno=0;
   	     contourvertexno<contourvertexcount;
   	     contourvertexno++) {
	    vertexno = simplymsh->vertexlist[vertexlistloc++];

	    dor_gpolyd_create_contour_vertex(vertexno,
			     simplymsh->vertex_location[vertexno][0],
			     simplymsh->vertex_location[vertexno][1],
			     simplymsh->vertex_location[vertexno][2]);
   	}
   
   	dor_gpolyd_close_contour();

   	dor_gpolyd_close_polygon();

	doe_simplymsh_polygonno = polygonno;
   	dor_gpolyd_triangulate_polygon(simplymsh->shape, 
		 dor_simplymsh_create_alternate_geometry_simple_triangle,
		 dor_simplymsh_create_alternate_geometry_complex_triangle,
		 simplymsh);
    }

    dor_trilst_close(trilstobj);

    return(trilstobj);
}
/*
 ======================================================================
 */

void dor_simplymsh_calculate_vertex_normals (struct simplymsh *simplymsh)
{
    DtFlag *vertexrefflag;
    DtInt vertexno;
    DtInt polygonno;
    DtInt vertexlistloc;
    DtInt polygonvertexno;
    DtRealTriple polygonnormal;

    /*  Allocate a table of polygon reference flags, one for each vertex.  */

    vertexrefflag = (DtFlag *)dor_space_allocate
			(simplymsh->vertex_count*(sizeof (DtFlag)));

    /*  Zero the vertex normals and polygon reference flags for
	all vertices.  */

    for (vertexno=0;vertexno<simplymsh->vertex_count;vertexno++){
	vertexrefflag[vertexno] = DcFalse;

	simplymsh->vertex_normal[vertexno][0] = 0.;
	simplymsh->vertex_normal[vertexno][1] = 0.;
	simplymsh->vertex_normal[vertexno][2] = 0.;
    }

    vertexlistloc = 0;

    for (polygonno=0;polygonno<simplymsh->polygon_count;polygonno++){
	dor_simplymsh_polygon_normal(simplymsh,vertexlistloc,
				     	simplymsh->contours[polygonno],
					polygonnormal);

	/*  For each of the polygon's vertices, update that vertex's
	    polygon reference count and accumulated polygon normal.  This
	    also bumps the vertexlistloc value to the first vertex of the
	    next polygon.  */

	for (polygonvertexno=0;
	     polygonvertexno<simplymsh->contours[polygonno];
	     polygonvertexno++){
	    vertexno = simplymsh->vertexlist[vertexlistloc++];

	    vertexrefflag[vertexno] = DcTrue;

	    simplymsh->vertex_normal[vertexno][0] += polygonnormal[0];
	    simplymsh->vertex_normal[vertexno][1] += polygonnormal[1];
	    simplymsh->vertex_normal[vertexno][2] += polygonnormal[2];
	}
    }

    /*  Rescale the vertex normals.  */

    for (vertexno=0;vertexno<simplymsh->vertex_count;vertexno++){
	if (vertexrefflag[vertexno]){
	    if (!dor_math_renormalize_vector(simplymsh->vertex_normal[vertexno])){
		DDerror (ERR_ZERO_TRI_NRM_SUM,
			 "dor_simplymsh_calculate_vertex_normals", 
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

void dor_simplymsh_compute_bounding_volume (dot_object *object)
{
    struct simplymsh *simplymsh;

    simplymsh = (struct simplymsh *)(object->data);

    if (simplymsh->alt_object_tris != DcNullPtr) {
	dor_object_get_method(simplymsh->alt_object_tris,
		      DcMethodCmpBndVolume)(simplymsh->alt_object_tris);
    } else {
	dor_system_report_bounding_points
	    (simplymsh->vertex_count, (DtReal*) simplymsh->vertex_location);
    }
}
/*
 ======================================================================
 */

DtInt dor_simplymsh_enable_edges_complex (
    struct simplymsh *simplymsh,
    dot_gpolyd_interpolated_vertex *gpolydvtx1,
    dot_gpolyd_interpolated_vertex *gpolydvtx2,
    dot_gpolyd_interpolated_vertex *gpolydvtx3)
{
    DtInt edge_enable;

#ifdef DEBUG
printf("dor_simplymsh_enable_edges_complex\n");
#endif

    edge_enable = dod_no_enabled_edges;
    if (dor_simplymsh_check_complex_edge(simplymsh, gpolydvtx1, gpolydvtx2))
	    edge_enable |= dod_edge1_enabled;
    if (dor_simplymsh_check_complex_edge(simplymsh, gpolydvtx2, gpolydvtx3))
	    edge_enable |= dod_edge2_enabled;
    if (dor_simplymsh_check_complex_edge(simplymsh, gpolydvtx3, gpolydvtx1))
	    edge_enable |= dod_edge3_enabled;
#ifdef DEBUG
    printf("edge_enable %d \n", edge_enable);
#endif
  
    return (edge_enable);
}


DtFlag dor_simplymsh_check_complex_edge (
    struct simplymsh *simplymsh,
    dot_gpolyd_interpolated_vertex *vtx1,
    dot_gpolyd_interpolated_vertex *vtx2)
{
    DtInt vertexcount;
    static DtInt *vtx1_no = (DtInt *)0;
    static DtInt *vtx2_no = (DtInt *)0;
    static DtInt vtx_space = 0;
    DtInt vtx1_no_count, vtx2_no_count;
    DtInt polygonstart;
    DtInt mshvertex;
    DtInt i,j;

    if (vtx1->interpolated && vtx2->interpolated) {
#ifdef DEBUG
	printf("both interpolated\n");
#endif
	if ((vtx1->vertex1 == vtx2->vertex1 ||
             vtx1->vertex1 == vtx2->vertex2) &&
            (vtx1->vertex2 == vtx2->vertex1 ||
             vtx1->vertex2 == vtx2->vertex2))
		return (DcTrue);

    } else if (vtx1->interpolated) {
#ifdef DEBUG
	printf("vtx1 interpolated\n");
#endif
	if (vtx2->vertex1 == vtx1->vertex1 ||
            vtx2->vertex1 == vtx1->vertex2)
		return (DcTrue);

    } else if (vtx2->interpolated) {
#ifdef DEBUG
	printf("vtx2 interpolated\n");
#endif
	if (vtx1->vertex1 == vtx2->vertex1 ||
            vtx1->vertex1 == vtx2->vertex2)
		return (DcTrue);

    } else {
	vertexcount = simplymsh->contours[doe_simplymsh_polygonno];

	if (vertexcount > vtx_space) {
	    vtx1_no = (DtInt *)dor_space_reallocate
			(vtx1_no, vertexcount*(sizeof(DtInt)));
	    vtx2_no = (DtInt *)dor_space_reallocate
			(vtx2_no, vertexcount*(sizeof(DtInt)));
	    vtx_space = vertexcount;
	}
	polygonstart = 0;

	for (i=0; i<doe_simplymsh_polygonno; i++) {
	    polygonstart += simplymsh->contours[i];
	}
#ifdef DEBUG
	printf("vertexcount %d  polygonstart %d\n", vertexcount , polygonstart);
#endif

  	vtx1_no_count = 0;
  	vtx2_no_count = 0;
	for (i=0; i<vertexcount; i++) {
	    mshvertex = simplymsh->vertexlist[polygonstart+i];
	    if (vtx1->vertex1 == mshvertex) {
		vtx1_no[vtx1_no_count++] = i;
	    }
	    if (vtx2->vertex1 == mshvertex) {
		vtx2_no[vtx2_no_count++] = i;
	    }
	}

	for (i=0; i<vtx1_no_count; i++) {
	    for (j=0; j<vtx2_no_count; j++) {
		if (vtx1_no[i] == ((vtx2_no[j]+1) % vertexcount) ||
		    vtx2_no[j] == ((vtx1_no[i]+1) % vertexcount)) {
		    return (DcTrue);
		}
	    }
	}
    }

    return (DcFalse);
}
/*
 ======================================================================
 */

DtInt dor_simplymsh_enable_edges_simple (
    struct simplymsh *simplymsh,
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
    printf("doe_simplymsh_polygonno = %d\n", doe_simplymsh_polygonno);
#endif

    vertexcount = simplymsh->contours[doe_simplymsh_polygonno];
    if (vertexcount > vtx_space) {
        vtx1_no = (DtInt *)dor_space_reallocate(vtx1_no,vertexcount*(sizeof(DtInt)));
        vtx2_no = (DtInt *)dor_space_reallocate(vtx2_no,vertexcount*(sizeof(DtInt)));
        vtx3_no = (DtInt *)dor_space_reallocate(vtx3_no,vertexcount*(sizeof(DtInt)));
        vtx_space = vertexcount;
    }

    polygonstart = 0;
    for (i=0; i<doe_simplymsh_polygonno; i++) {
	polygonstart += simplymsh->contours[i];
    }
#ifdef DEBUG
    printf("polygonstart = %d\n", polygonstart);
#endif
   
    vtx1_no_count = 0; 
    vtx2_no_count = 0; 
    vtx3_no_count = 0; 
    for (i=0; i<vertexcount; i++) {
	mshvertex = simplymsh->vertexlist[polygonstart+i];
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

void dor_simplymsh_pick (dot_object *object)
{
    DtRepType reptype;

    if (!dor_condex_query_pick(doe_simplymsh_class_id))
	    return;

    dor_pick_initialize_object(object);

    dor_global_reptyp_get_value(&reptype);

    switch(reptype) {
    case DcOutlines:
	dor_simplymsh_pick_lines(object);
	break;

    case DcSurface:
    case DcSurfaceWireframeEdges:
    case DcSurfaceOutlineEdges:
    case DcWireframe:
    case DcPoints:
	dor_simplymsh_pick_tris(object);
	break;
    default:
	DDerror(ERR_VALUE_OUT_OF_RANGE,
		   "dor_simplymsh_pick",
		   "unknown reptype");
    }
}
/*
 ======================================================================
 */

void dor_simplymsh_pick_lines (dot_object *object)
{
    struct simplymsh *simplymsh;

    simplymsh = (struct simplymsh *)(object->data);

    if (simplymsh->alt_object_lines != DcNullPtr) {
   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(simplymsh->alt_object_lines,
			      DcMethodPick)(simplymsh->alt_object_lines);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    } else {
   	simplymsh->alt_object_lines =
		dor_simplymsh_create_alternate_geom_lines(simplymsh);

   	if (simplymsh->alt_object_lines == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_simplymsh_pick_lines", "");
	    return;
   	}

   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(simplymsh->alt_object_lines,
			      DcMethodPick)(simplymsh->alt_object_lines);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    }
}
/*
 ======================================================================
 */

void dor_simplymsh_pick_tris (dot_object *object)
{
    struct simplymsh *simplymsh;

    simplymsh = (struct simplymsh *)(object->data);

    if (simplymsh->alt_object_tris != DcNullPtr) {
   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(simplymsh->alt_object_tris,
			      DcMethodPick)(simplymsh->alt_object_tris);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    } else {
   	simplymsh->alt_object_tris =
		dor_simplymsh_create_alternate_geom_tris(simplymsh);

   	if (simplymsh->alt_object_tris == NULL) {
	    DDerror(ERR_CANT_CREATE_ALTOBJ,
		       "dor_simplymsh_pick_tris", "");
	    return;
   	}

   	doe_system_alternate_object_execution_flag = DcTrue;

   	dor_object_get_method(simplymsh->alt_object_tris,
			      DcMethodPick)(simplymsh->alt_object_tris);

   	doe_system_alternate_object_execution_flag = DcFalse;

   	return;
    }
}
/*
 ======================================================================
 */

void dor_simplymsh_polygon_normal (
    struct simplymsh *simplymsh,
    DtInt initialvertexlistloc,
    DtInt polygonvertexcount,
    DtRealTriple polygonnormal)
{
    DtInt vertexlistloc;
    DtInt polygonvertexno;
    DtInt vertexno;
    DtRealTriple vertices[200];

    vertexlistloc = initialvertexlistloc;

    for (polygonvertexno=0; polygonvertexno<polygonvertexcount; polygonvertexno++) {
	vertexno = simplymsh->vertexlist[vertexlistloc++];

	vertices[polygonvertexno][0] = simplymsh->vertex_location[vertexno][0];
	vertices[polygonvertexno][1] = simplymsh->vertex_location[vertexno][1];
	vertices[polygonvertexno][2] = simplymsh->vertex_location[vertexno][2];
    }

    dor_math_polygon_normal_fast(polygonvertexcount,vertices,polygonnormal);
}
/*
 ======================================================================
 */

void dor_simplymsh_render_standard (dot_object *object)
{
    dot_object *alt;

    if (!dor_condex_query_render(doe_simplymsh_class_id))
	    return;

    alt = dor_simplymsh_update_alternate (object);
    if (alt != DcNullPtr) {
	dor_object_get_current_method(alt) (alt);
    }
}


