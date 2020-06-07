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
	ddr_g4vll_drom_quadmsh_create
	ddr_g4vll_drom_quadmsh_print
	ddr_g4vll_drom_quadmsh_delete
	ddr_g4vll_drom_quadmsh_render
	ddr_g4vll_drom_quadmsh_create_alternate_quadlst

  ====================================================================== */

#include "drom.h"
#include <dore/internal/system.h>
#include <dore/dore_develop/private/quadlst.h>
#include <dore/dore_develop/private/quadmsh.h>

/* ====================================================================== */

static DtInt classid = -1;
static DtInt dataindex = -1;


/* ====================================================================== */

ddt_g4vll_drom_quadmsh *ddr_g4vll_drom_quadmsh_create (
    DtObjectStructure *object)
{
    struct quadmsh *quadmsh;
    ddt_g4vll_drom_quadmsh *drom_quadmsh;
    int i;

    quadmsh = (struct quadmsh *)(object->data);

    drom_quadmsh =  (ddt_g4vll_drom_quadmsh *)
	    DDspace_Allocate(sizeof(ddt_g4vll_drom_quadmsh));

    if (quadmsh->uv_count > 0) {
        drom_quadmsh->bboxuv = (ddt_g4vll_drom_bboxuv *)DDspace_Allocate
                (quadmsh->uv_count *sizeof(ddt_g4vll_drom_bboxuv));

        for (i=0; i< quadmsh->uv_count; i++) {
            drom_quadmsh->bboxuv[i].initialized = DcFalse;
        }
    } else {
        drom_quadmsh->bboxuv = (ddt_g4vll_drom_bboxuv *)0;
    }

    if (quadmsh->uvw_count > 0) {
        drom_quadmsh->bboxuvw = (ddt_g4vll_drom_bboxuvw *)DDspace_Allocate
                (quadmsh->uvw_count *sizeof(ddt_g4vll_drom_bboxuvw));

        for (i=0; i< quadmsh->uvw_count; i++) {
            drom_quadmsh->bboxuvw[i].initialized = DcFalse;
        }
    } else {
        drom_quadmsh->bboxuvw = (ddt_g4vll_drom_bboxuvw *)0;
    }

    drom_quadmsh->altobj_quadlst = (DtObjectStructure *)0;

    return drom_quadmsh;
}

/* ====================================================================== */

void ddr_g4vll_drom_quadmsh_print (
    DtObject object,
    DtInt *data)
{
    return;
}

/* ====================================================================== */

void ddr_g4vll_drom_quadmsh_delete (
    DtObject object,
    ddt_g4vll_drom_quadmsh *data)
{
    if (data != (ddt_g4vll_drom_quadmsh *)0) {
        if (data->bboxuv != (ddt_g4vll_drom_bboxuv *)0) {
            DDspace_Deallocate(data->bboxuv);
        }
        if (data->bboxuvw != (ddt_g4vll_drom_bboxuvw *)0) {
            DDspace_Deallocate(data->bboxuvw);
        }
        if (data->altobj_quadlst != (DtObjectStructure *)0) {
            DeDeleteObject(data->altobj_quadlst);
        }

	DDspace_Deallocate(data);
    }
}

/* ====================================================================== */

void ddr_g4vll_drom_quadmsh_render (
    dot_object *object)
{
    ddt_g4vll_drom_quadmsh *drom_quadmsh;
    DtInt i;

    if (classid == -1)  classid = DsInqClassId("DoQuadMesh");

    /* Determine index into quad mesh's additional data */
    if (dataindex == -1) {
	for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
	    if (dde_g4vll_drom_class_data[i].id == classid) {
		dataindex = dde_g4vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    if (!DDcondex_QueryRender(classid))
	    return;

    drom_quadmsh = (ddt_g4vll_drom_quadmsh *)object->additional_data[dataindex];

    /*
     * The quad mesh's alternate object is a quad list.
     */
    if (drom_quadmsh->altobj_quadlst == (DtObject)0) {
	drom_quadmsh->altobj_quadlst = 
		ddr_g4vll_drom_quadmsh_create_alternate_quadlst(object);
    }

    if (drom_quadmsh->altobj_quadlst != (DtObject)0) {
    	DDobject_InqCurrentMethod(drom_quadmsh->altobj_quadlst)
           	(drom_quadmsh->altobj_quadlst);
    }
}

/* ====================================================================== */

dot_object *ddr_g4vll_drom_quadmsh_create_alternate_quadlst (
    dot_object *object)
{
    struct quadmsh 		*quadmsh;
    DtVertexType		vertextype;
    DtReal			*vertices, *vptr;
    quadmsh_vertexlist 		*curquadptr;
    DtInt 			quadno, curquad;
    DtInt 			vertexsize;
    DtInt			index;
    DtInt			uv, uvw;
    dot_object			*quadlstobj;

    quadmsh = (struct quadmsh *)(object->data);

    vertextype = quadmsh->vertextype;

    if (quadmsh->smoothflag)
        vertextype |= DcLocNrm;

    vertexsize = 3;
    if (vertextype & DcLocNrm)
        vertexsize += 3;
    if (vertextype & DcLocClr)
        vertexsize += 3;
    if (vertextype & DcLocAlpha)
        vertexsize += 3;

    vertexsize += (quadmsh->uv_count * 2 + quadmsh->uvw_count * 3);

    vertices = (DtReal *)dor_space_allocate(quadmsh->quad_count * 4 * 
			vertexsize*sizeof(DtReal));

    vptr = vertices;
    curquadptr = quadmsh->quad_vertexlist;

    for (quadno = 0; quadno < quadmsh->quad_count; quadno++) {
    	for (index = 0; index < 4; index++) {
	    curquad = (*curquadptr)[index];

	    *vptr++ = quadmsh->vertex_location[curquad][0];
	    *vptr++ = quadmsh->vertex_location[curquad][1];
	    *vptr++ = quadmsh->vertex_location[curquad][2];

	    if (vertextype & DcLocNrm) {
	    	*vptr++ = quadmsh->vertex_normal[curquad][0];
	    	*vptr++ = quadmsh->vertex_normal[curquad][1];
	    	*vptr++ = quadmsh->vertex_normal[curquad][2];
	    }

	    if (vertextype & DcLocClr) {
	    	*vptr++ = quadmsh->vertex_color[curquad][0];
	    	*vptr++ = quadmsh->vertex_color[curquad][1];
	    	*vptr++ = quadmsh->vertex_color[curquad][2];
	    }

	    if (vertextype & DcLocAlpha) {
	    	*vptr++ = quadmsh->vertex_alpha[curquad];
	    }

	    for (uv=0; uv<quadmsh->uv_count; uv++) {
		*vptr++ = quadmsh->uv_list[uv][curquad][0];
		*vptr++ = quadmsh->uv_list[uv][curquad][1];
	    }
	    for (uvw=0; uvw<quadmsh->uvw_count; uvw++) {
		*vptr++ = quadmsh->uvw_list[uvw][curquad][0];
		*vptr++ = quadmsh->uvw_list[uvw][curquad][1];
		*vptr++ = quadmsh->uvw_list[uvw][curquad][2];
	    }
    	}
	curquadptr++;
    }

    quadlstobj = dor_quadlst_create(quadmsh->colormodel, 
		(vertextype | dor_system_texture_uv_count(quadmsh->uv_count) |
		dor_system_texture_uvw_count(quadmsh->uvw_count)),
		quadmsh->quad_count, vertices);

    dor_space_deallocate(vertices);
    return(quadlstobj);
}
