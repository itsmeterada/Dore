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
	int dor_maptrnint_initialize ()
	dot_object * dor_maptrnint_create (operator, mapping, raster)
	int dor_maptrnint_destroy (object)
	int dor_maptrnint_print (object)
	int dor_maptrnint_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/maptrnint.h>
#include <dore/dore_develop/private/stdtexatt.h>

DtInt doe_maptrnint_class_id = -1;
DtFlag doe_maptrnint_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry maptrnint_methods[] = {
    { DcMethodDestroy,          (DtMethodPtr) dor_maptrnint_destroy },
    { DcMethodPrint,            (DtMethodPtr) dor_maptrnint_print },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_maptrnint_execute },
};

void dor_maptrnint_initialize (void)
{
    doe_maptrnint_class_id = dor_class_add ("DoTextureMapTranspIntens", 3,
					    maptrnint_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_maptrnint_create (
    DtMapOperator operator,
    DtObject mapping,
    DtObject raster)
{
    struct maptrnint_data *maptrnintdata;

    if (!doe_maptrnint_initialized) {
	dor_maptrnint_initialize();
	doe_maptrnint_initialized = DcTrue;
    }

#ifdef ERRCHK
    if (!dor_object_confirm_type(raster,dor_class_inq_id("DoRaster")) &&
	!dor_object_confirm_type(raster,dor_class_inq_id("DoFileRaster"))){

        DDerror(ERR_BAD_OBJECT, "DoTextureMapTranspIntens",
		     "raster must be DoRaster or DoFileRaster object");
        return ((dot_object *)dor_null_create(DcNullPtr));
    }
#endif

    maptrnintdata = (struct maptrnint_data *)dor_space_allocate(sizeof *maptrnintdata);

    maptrnintdata->operator = operator;
    maptrnintdata->mapping = mapping;
    maptrnintdata->raster = raster;

    return (dor_object_create(doe_maptrnint_class_id,maptrnintdata));
}
/*
 ======================================================================
 */

void dor_maptrnint_destroy (
    dot_object *object)
{
    struct maptrnint_data *maptrnint;

    maptrnint = (struct maptrnint_data *)(object->data);

    dor_space_deallocate(maptrnint);
}
/*
 ======================================================================
 */

void dor_maptrnint_print (
    dot_object *object)
{
    struct maptrnint_data *maptrnint;

    maptrnint = (struct maptrnint_data *)object->data;

    if (maptrnint == NULL) {
	dor_print_output("bad values");
	return;
    }

    switch(maptrnint->operator) {
    case DcMapReplace:
	dor_print_output ("operator = DcMapReplace");
	break;
    case DcMapAdd:
	dor_print_output ("operator = DcMapAdd");
	break;
    default:
	sprintf(dor_print_get_line(),"operator = <UNKNOWN TYPE> %d",maptrnint->operator);
	dor_print_output(dor_print_get_line());
	break;
    }
    if (maptrnint->mapping == DcStdTableLookup) {
	dor_print_output ("mapping = DcStdTableLookup");
    } else if (maptrnint->mapping == DcStd3dTableLookup) {
	dor_print_output ("mapping = DcStd3dTableLookup");
    } else if (maptrnint->mapping == DcStdBumpMap) {
	dor_print_output ("mapping = DcStdBumpMap");
    } else if (maptrnint->mapping == DcStdSphereEnvironMap) {
	dor_print_output ("mapping = DcStdSphereEnvironMap");
    } else if (maptrnint->mapping == DcStdCubeEnvironMap) {
	dor_print_output ("mapping = DcStdCubeEnvironMap");
    } else {
	sprintf(dor_print_get_line(),"mapping = 0x%lx",maptrnint->mapping);
	dor_print_output(dor_print_get_line());
	dor_object_print (maptrnint->mapping);
    }

    sprintf(dor_print_get_line(),"raster = 0x%lx",maptrnint->raster);
    dor_print_output(dor_print_get_line());
    dor_object_print (maptrnint->raster);
}
/*
 ======================================================================
 */

void dor_maptrnint_execute (
    dot_object *object)
{
    struct maptrnint_data *maptrnintdat;
    dot_stdtexatt attributes;
    static DtMethodPtr aamode_query, op_query;
    static DtMethodPtr uvidx_query, uvextend_query, uvmatrix_query;
    static DtMethodPtr uvwidx_query, uvwextend_query, uvwmatrix_query;
    static DtFlag initialized = DcFalse;

    if (!dor_condex_query_execute(doe_maptrnint_class_id))
	    return;

    maptrnintdat = (struct maptrnint_data *)object->data;

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

    dor_global_maptrnint_set_value(maptrnintdat->operator, maptrnintdat->mapping,
				   maptrnintdat->raster,
				   &attributes, sizeof(dot_stdtexatt));
}
