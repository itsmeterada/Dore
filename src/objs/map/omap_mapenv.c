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
	int dor_mapenv_initialize ()
	dot_object * dor_mapenv_create (operator, mapping, raster)
	int dor_mapenv_destroy (object)
	int dor_mapenv_print (object)
	int dor_mapenv_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/mapenv.h>
#include <dore/dore_develop/private/stdtexatt.h>

DtInt doe_mapenv_class_id = -1;
DtFlag doe_mapenv_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry mapenv_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_mapenv_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_mapenv_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_mapenv_execute },
};

void dor_mapenv_initialize (void)
{
    doe_mapenv_class_id = dor_class_add ("DoTextureMapEnviron", 3,
					 mapenv_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_mapenv_create (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster)
{
    struct mapenv_data *mapenvdata;

    if (!doe_mapenv_initialized) {
	dor_mapenv_initialize();
	doe_mapenv_initialized = DcTrue;
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(raster,dor_class_inq_id("DoRaster")) &&
	!dor_object_confirm_type(raster,dor_class_inq_id("DoFileRaster"))){

        DDerror(ERR_BAD_OBJECT, "DoTextureMapEnviron",
		     "raster must be DoRaster or DoFileRaster object");
        return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif

    mapenvdata = (struct mapenv_data *)dor_space_allocate(sizeof *mapenvdata);

    mapenvdata->operator = operator;
    mapenvdata->mapping = mapping;
    mapenvdata->raster = raster;

    return (dor_object_create(doe_mapenv_class_id,mapenvdata));
}
/*
 ======================================================================
 */

void dor_mapenv_destroy (
    dot_object *object)
{
    struct mapenv_data *mapenv;

    mapenv = (struct mapenv_data *)(object->data);

    dor_space_deallocate(mapenv);
}
/*
 ======================================================================
 */

void dor_mapenv_print (
    dot_object *object)
{
    struct mapenv_data *mapenv;

    mapenv = (struct mapenv_data *)object->data;

    if (mapenv == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(mapenv->operator) {
    case DcMapReplace:
	dor_print_output ("operator = DcMapReplace");
	break;
    case DcMapAdd:
	dor_print_output ("operator = DcMapAdd");
	break;
    default:
	sprintf(dor_print_get_line(),"operator = <UNKNOWN TYPE> %d",mapenv->operator);
	dor_print_output(dor_print_get_line());
	break;
    }
    if (mapenv->mapping == DcStdTableLookup) {
	dor_print_output ("mapping = DcStdTableLookup");
    } else if (mapenv->mapping == DcStd3dTableLookup) {
	dor_print_output ("mapping = DcStd3dTableLookup");
    } else if (mapenv->mapping == DcStdBumpMap) {
	dor_print_output ("mapping = DcStdBumpMap");
    } else if (mapenv->mapping == DcStdSphereEnvironMap) {
	dor_print_output ("mapping = DcStdSphereEnvironMap");
    } else if (mapenv->mapping == DcStdCubeEnvironMap) {
	dor_print_output ("mapping = DcStdCubeEnvironMap");
    } else {
	sprintf(dor_print_get_line(),"mapping = 0x%lx",mapenv->mapping);
	dor_print_output(dor_print_get_line());
	dor_object_print (mapenv->mapping);
    }

    sprintf(dor_print_get_line(),"raster = 0x%lx",mapenv->raster);
    dor_print_output(dor_print_get_line());
    dor_object_print (mapenv->raster);
}
/*
 ======================================================================
 */

void dor_mapenv_execute (
    dot_object *object)
{
    struct mapenv_data *mapenvdat;
    dot_stdtexatt attributes;
    static DtMethodPtr aamode_query, op_query;
    static DtMethodPtr uvidx_query, uvextend_query, uvmatrix_query;
    static DtMethodPtr uvwidx_query, uvwextend_query, uvwmatrix_query;
    static DtFlag initialized = DcFalse;

    if (!dor_condex_query_execute(doe_mapenv_class_id))
	    return;

    mapenvdat = (struct mapenv_data *)object->data;

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

    dor_global_mapenv_set_value(mapenvdat->operator, mapenvdat->mapping,
				mapenvdat->raster,
				&attributes, sizeof(dot_stdtexatt));
}
