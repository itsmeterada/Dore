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
	int dor_mapbmp_initialize ()
	dot_object * dor_mapbmp_create (operator, mapping, raster)
	int dor_mapbmp_destroy (object)
	int dor_mapbmp_print (object)
	int dor_mapbmp_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/mapbmp.h>
#include <dore/dore_develop/private/stdtexatt.h>

DtInt doe_mapbmp_class_id = -1;
DtFlag doe_mapbmp_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry mapbmp_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_mapbmp_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_mapbmp_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_mapbmp_execute },
};

void dor_mapbmp_initialize (void)
{
    doe_mapbmp_class_id = dor_class_add ("DoTextureMapBump", 3,
					 mapbmp_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_mapbmp_create (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster)
{
    struct mapbmp_data *mapbmpdata;

    if (!doe_mapbmp_initialized) {
	dor_mapbmp_initialize();
	doe_mapbmp_initialized = DcTrue;
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(raster,dor_class_inq_id("DoRaster")) &&
	!dor_object_confirm_type(raster,dor_class_inq_id("DoFileRaster"))){

	DDerror(ERR_BAD_OBJECT, "DoTextureMapBump",
		     "raster must be DoRaster or DoFileRaster object");
        return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif

    mapbmpdata = (struct mapbmp_data *)dor_space_allocate(sizeof *mapbmpdata);

    mapbmpdata->operator = operator;
    mapbmpdata->mapping = mapping;
    mapbmpdata->raster = raster;

    return (dor_object_create(doe_mapbmp_class_id,mapbmpdata));
}
/*
 ======================================================================
 */

void dor_mapbmp_destroy (
    dot_object *object)
{
    struct mapbmp_data *mapbmp;

    mapbmp = (struct mapbmp_data *)(object->data);

    dor_space_deallocate(mapbmp);
}
/*
 ======================================================================
 */

void dor_mapbmp_print (
    dot_object *object)
{
    struct mapbmp_data *mapbmp;

    mapbmp = (struct mapbmp_data *)object->data;

    if (mapbmp == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(mapbmp->operator) {
    case DcMapReplace:
	dor_print_output ("operator = DcMapReplace");
	break;
    case DcMapAdd:
	dor_print_output ("operator = DcMapAdd");
	break;
    default:
	sprintf(dor_print_get_line(),"operator = <UNKNOWN TYPE> %d",mapbmp->operator);
	dor_print_output(dor_print_get_line());
	break;
    }
    if (mapbmp->mapping == DcStdTableLookup) {
	dor_print_output ("mapping = DcStdTableLookup");
    } else if (mapbmp->mapping == DcStd3dTableLookup) {
	dor_print_output ("mapping = DcStd3dTableLookup");
    } else if (mapbmp->mapping == DcStdBumpMap) {
	dor_print_output ("mapping = DcStdBumpMap");
    } else if (mapbmp->mapping == DcStdSphereEnvironMap) {
	dor_print_output ("mapping = DcStdSphereEnvironMap");
    } else if (mapbmp->mapping == DcStdCubeEnvironMap) {
	dor_print_output ("mapping = DcStdCubeEnvironMap");
    } else {
	sprintf(dor_print_get_line(),"mapping = 0x%lx",mapbmp->mapping);
	dor_print_output(dor_print_get_line());
	dor_object_print (mapbmp->mapping);
    }

    sprintf(dor_print_get_line(),"raster = 0x%lx",mapbmp->raster);
    dor_print_output(dor_print_get_line());
    dor_object_print (mapbmp->raster);
}
/*
 ======================================================================
 */

void dor_mapbmp_execute (
    dot_object *object)
{
    struct mapbmp_data *mapbmpdat;
    dot_stdtexatt attributes;
    static DtMethodPtr aamode_query, op_query;
    static DtMethodPtr uvidx_query, uvextend_query, uvmatrix_query;
    static DtMethodPtr uvwidx_query, uvwextend_query, uvwmatrix_query;
    static DtFlag initialized = DcFalse;

    if (!dor_condex_query_execute(doe_mapbmp_class_id))
	    return;

    mapbmpdat = (struct mapbmp_data *)object->data;

    if (!initialized) {
	initialized = DcTrue;

	aamode_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureAntiAlias"), DcMethodInqGlbAttVal);
	op_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureOp"), DcMethodInqGlbAttVal);
	uvidx_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureUVIndex"), DcMethodInqGlbAttVal);
	uvextend_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureExtendUV"), DcMethodInqGlbAttVal);
	uvmatrix_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureMatrixUV"), DcMethodInqGlbAttVal);
	uvwidx_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureUVWIndex"), DcMethodInqGlbAttVal);
	uvwextend_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureExtendUVW"), DcMethodInqGlbAttVal);
	uvwmatrix_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureMatrixUVW"), DcMethodInqGlbAttVal);
    }

    (*aamode_query) (&attributes.aamode);
    (*op_query)(&attributes.operator);
    (*uvidx_query)(&attributes.uv.idx);
    (*uvextend_query)(&attributes.uv.extend[0], &attributes.uv.extend[1]);
    (*uvmatrix_query)(attributes.uv.matrix);
    (*uvwidx_query)(&attributes.uvw.idx);
    (*uvwextend_query)(&attributes.uvw.extend[0], &attributes.uvw.extend[1],
		       &attributes.uvw.extend[2]);
    (*uvwmatrix_query)(attributes.uvw.matrix);

    dor_global_mapbmp_set_value(mapbmpdat->operator, mapbmpdat->mapping,
				mapbmpdat->raster,
				&attributes, sizeof(dot_stdtexatt));
}
