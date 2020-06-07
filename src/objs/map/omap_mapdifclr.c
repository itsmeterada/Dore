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
	int dor_mapdifclr_initialize ()
	dot_object * dor_mapdifclr_create (operator, mapping, raster)
	int dor_mapdifclr_destroy (object)
	int dor_mapdifclr_print (object)
	int dor_mapdifclr_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/mapdifclr.h>
#include <dore/dore_develop/private/stdtexatt.h>

DtInt doe_mapdifclr_class_id = -1;
DtFlag doe_mapdifclr_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry mapdifclr_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_mapdifclr_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_mapdifclr_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_mapdifclr_execute },
};

void dor_mapdifclr_initialize (void)
{
    doe_mapdifclr_class_id = dor_class_add ("DoTextureMapDiffuseColor", 3,
					    mapdifclr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_mapdifclr_create (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster)
{
    struct mapdifclr_data *mapdifclrdata;

    if (!doe_mapdifclr_initialized) {
	dor_mapdifclr_initialize();
	doe_mapdifclr_initialized = DcTrue;
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(raster,dor_class_inq_id("DoRaster")) &&
	!dor_object_confirm_type(raster,dor_class_inq_id("DoFileRaster"))){

        DDerror(ERR_BAD_OBJECT, "DoTextureMapDiffuseColor",
		     "raster must be DoRaster or DoFileRaster object");
        return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif

    mapdifclrdata = (struct mapdifclr_data *)dor_space_allocate(sizeof *mapdifclrdata);

    mapdifclrdata->operator = operator;
    mapdifclrdata->mapping = mapping;
    mapdifclrdata->raster = raster;

    return (dor_object_create(doe_mapdifclr_class_id,mapdifclrdata));
}
/*
 ======================================================================
 */

void dor_mapdifclr_destroy (
    dot_object *object)
{
    struct mapdifclr_data *mapdifclr;

    mapdifclr = (struct mapdifclr_data *)(object->data);

    dor_space_deallocate(mapdifclr);
}
/*
 ======================================================================
 */

void dor_mapdifclr_print (
    dot_object *object)
{
    struct mapdifclr_data *mapdifclr;

    mapdifclr = (struct mapdifclr_data *)object->data;

    if (mapdifclr == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(mapdifclr->operator) {
    case DcMapReplace:
	dor_print_output ("operator = DcMapReplace");
	break;
    case DcMapAdd:
	dor_print_output ("operator = DcMapAdd");
	break;
    default:
	sprintf(dor_print_get_line(),"operator = <UNKNOWN TYPE> %d",mapdifclr->operator);
	dor_print_output(dor_print_get_line());
	break;
    }
    if (mapdifclr->mapping == DcStdTableLookup) {
	dor_print_output ("mapping = DcStdTableLookup");
    } else if (mapdifclr->mapping == DcStd3dTableLookup) {
	dor_print_output ("mapping = DcStd3dTableLookup");
    } else if (mapdifclr->mapping == DcStdBumpMap) {
	dor_print_output ("mapping = DcStdBumpMap");
    } else if (mapdifclr->mapping == DcStdSphereEnvironMap) {
	dor_print_output ("mapping = DcStdSphereEnvironMap");
    } else if (mapdifclr->mapping == DcStdCubeEnvironMap) {
	dor_print_output ("mapping = DcStdCubeEnvironMap");
    } else if (mapdifclr->mapping == Dc2PartMap) {
	dor_print_output ("mapping = Dc2PartMap");
    } else {
	sprintf(dor_print_get_line(),"mapping = 0x%lx",mapdifclr->mapping);
	dor_print_output(dor_print_get_line());
	dor_object_print (mapdifclr->mapping);
    }

    sprintf(dor_print_get_line(),"raster = 0x%lx",mapdifclr->raster);
    dor_print_output(dor_print_get_line());
    dor_object_print (mapdifclr->raster);
}
/*
 ======================================================================
 */

void dor_mapdifclr_execute (
    dot_object *object)
{
    struct mapdifclr_data *mapdifclrdat;
    dot_stdtexatt attributes;
    static DtMethodPtr aamode_query, op_query, intsrf_query;
    static DtMethodPtr uvidx_query, uvextend_query, uvmatrix_query;
    static DtMethodPtr uvwidx_query, uvwextend_query, uvwmatrix_query;
    static DtFlag initialized = DcFalse;

    if (!dor_condex_query_execute(doe_mapdifclr_class_id))
	    return;

    mapdifclrdat = (struct mapdifclr_data *)object->data;

    if (!initialized) {
	initialized = DcTrue;

	aamode_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureAntiAlias"), DcMethodInqGlbAttVal);
	op_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureOp"), DcMethodInqGlbAttVal);
	intsrf_query = dor_class_inq_method (
		dor_class_inq_id ("AttTextureIntermediateSurf"), 
		DcMethodInqGlbAttVal);
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
    (*intsrf_query)(&attributes.surface_object, &attributes.projection_type);
    (*uvidx_query)(&attributes.uv.idx);
    (*uvextend_query)(&attributes.uv.extend[0], &attributes.uv.extend[1]);
    (*uvmatrix_query)(attributes.uv.matrix);
    (*uvwidx_query)(&attributes.uvw.idx);
    (*uvwextend_query)(&attributes.uvw.extend[0], &attributes.uvw.extend[1],
		       &attributes.uvw.extend[2]);
    (*uvwmatrix_query)(attributes.uvw.matrix);

    dor_global_mapdifclr_set_value(mapdifclrdat->operator, mapdifclrdat->mapping,
				   mapdifclrdat->raster,
				   &attributes, sizeof(dot_stdtexatt));
}
