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
	drr_dynrnd_initialize_attributes
	drr_dynrnd_ambint
	drr_dynrnd_ambswi
	drr_dynrnd_bacfacculble
	drr_dynrnd_bacfacculswi
	drr_dynrnd_depcue
	drr_dynrnd_depcueswi
	drr_dynrnd_difswi
	drr_dynrnd_difclr
	drr_dynrnd_difint
	drr_dynrnd_hidsrfswi
	drr_dynrnd_inttyp
	drr_dynrnd_lintyp
	drr_dynrnd_linwid
	drr_dynrnd_localaasty
	drr_dynrnd_localaaswi
	drr_dynrnd_refswi
	drr_dynrnd_reptyp
	drr_dynrnd_shaswi
	drr_dynrnd_shdidx
	drr_dynrnd_spcclr
	drr_dynrnd_spcfct
	drr_dynrnd_spcint
	drr_dynrnd_spcswi
	drr_dynrnd_srfedgclr
	drr_dynrnd_srfshd
	drr_dynrnd_transpclr
	drr_dynrnd_transpint
	drr_dynrnd_transpswi
	drr_dynrnd_transporientclr
	drr_dynrnd_transporientexp
	drr_dynrnd_transporientint
	drr_dynrnd_transporientswi
	drr_dynrnd_smpadapt
	drr_dynrnd_smpadaptswi
	drr_dynrnd_smpfilter
	drr_dynrnd_smpjitter
	drr_dynrnd_smpjitterswi
	drr_dynrnd_smpsuper
	drr_dynrnd_smpsuperswi

  ======================================================================
 */
#include "dynrnd.h"
/*
 ======================================================================
 */
void drr_dynrnd_initialize_attributes (void)
{
    DtInt i;

    for (i=0; dre_dynrnd_attribute[i].name != 0; i++) {
	(*dre_dynrnd_attribute[i].routine)();
    }

    (*dre_dynrnd_DROM_fcns->set_att_clpvol) (DcClipNone,0,NULL);
}
/*
 ======================================================================
 */
void drr_dynrnd_ambint (void)
{
    DtReal intensity;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttAmbientIntens"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&intensity);
    (*dre_dynrnd_DROM_fcns->set_att_ambint) (intensity);
}
/*
 ======================================================================
 */
void drr_dynrnd_ambswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttAmbientSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_ambswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_bacfacculble (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttBackfaceCullable"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_bacfacculble) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_bacfacculswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttBackfaceCullSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_bacfacculswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_depcue (void)
{
    DtReal zfront, zback;
    DtReal sfront, sback;
    DtColorModel colormodel;
    DtReal color[3];
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttDepthCue"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&zfront,&zback,&sfront,&sback,&colormodel,color);
    (*dre_dynrnd_DROM_fcns->set_att_depcue) (zfront, zback, sfront, sback, 
					     colormodel, color);
}
/*
 ======================================================================
 */
void drr_dynrnd_depcueswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttDepthCueSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_depcueswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_difswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttDiffuseSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_difswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_difclr (void)
{
    DtColorModel colormodel;
    DtReal color[3];
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttDiffuseColor"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&colormodel, color);
    (*dre_dynrnd_DROM_fcns->set_att_difclr) (colormodel, color);
}
/*
 ======================================================================
 */
void drr_dynrnd_difint (void)
{
    DtReal intensity;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttDiffuseIntens"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&intensity);
    (*dre_dynrnd_DROM_fcns->set_att_difint) (intensity);
}
/*
 ======================================================================
 */
void drr_dynrnd_hidsrfswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttHiddenSurfaceSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_hidsrfswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_inttyp (void)
{
    DtInterpType interptype;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttInterpType"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&interptype);
    (*dre_dynrnd_DROM_fcns->set_att_inttyp) (interptype);
}
/*
 ======================================================================
 */
void drr_dynrnd_lintyp (void)
{
    DtLineType type;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLineType"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&type);
    (*dre_dynrnd_DROM_fcns->set_att_lintyp)(type);
}
/*
 ======================================================================
 */
void drr_dynrnd_linwid (void)
{
    DtReal width;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLineWidth"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&width);
    (*dre_dynrnd_DROM_fcns->set_att_linwid)(width);
}
/*
 ======================================================================
 */
void drr_dynrnd_localaasty (void)
{
    DtLocalAntiAliasStyle style;
    static DtMethodPtr query = DcNullPtr;

    if (dre_dynrnd_DROM_fcns->version_number < 2)
	    return;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLocalAntiAliasStyle"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&style);
    (*dre_dynrnd_DROM_fcns->set_att_localaasty) (style);
}
/*
 ======================================================================
 */
void drr_dynrnd_localaaswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (dre_dynrnd_DROM_fcns->version_number < 2)
	    return;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLocalAntiAliasSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_localaaswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_refswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttReflectionSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_refswi)(switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_reptyp (void)
{
    DtRepType reptype;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttRepType"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&reptype);
    (*dre_dynrnd_DROM_fcns->set_att_reptyp) (reptype);
}
/*
 ======================================================================
 */
void drr_dynrnd_shaswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttShadowSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_shaswi)(switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_shdidx (void)
{
    DtInt shadeindex;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttShadeIndex"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&shadeindex);
    (*dre_dynrnd_DROM_fcns->set_att_shdidx) (shadeindex);
}
/*
 ======================================================================
 */
void drr_dynrnd_spcclr (void)
{
    DtColorModel colormodel;
    DtReal color[3];
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttSpecularColor"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&colormodel, color);
    (*dre_dynrnd_DROM_fcns->set_att_spcclr) (colormodel, color);
}
/*
 ======================================================================
 */
void drr_dynrnd_spcfct (void)
{
    DtReal factor;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttSpecularFactor"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&factor);
    (*dre_dynrnd_DROM_fcns->set_att_spcfct) (factor);
}
/*
 ======================================================================
 */
void drr_dynrnd_spcint (void)
{
    DtReal intensity;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttSpecularIntens"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&intensity);
    (*dre_dynrnd_DROM_fcns->set_att_spcint) (intensity);
}
/*
 ======================================================================
 */
void drr_dynrnd_spcswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttSpecularSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_spcswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_srfedgclr (void)
{
    DtColorModel colormodel;
    DtReal color[3];
    static DtMethodPtr query = DcNullPtr;

    if (dre_dynrnd_DROM_fcns->version_number < 2)
	    return;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttSurfaceEdgeColor"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&colormodel, color);
    (*dre_dynrnd_DROM_fcns->set_att_srfedgclr) (colormodel, color);
}
/*
 ======================================================================
 */
void drr_dynrnd_srfshd (void)
{
    dot_object *callbackobj;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttSurfaceShade"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&callbackobj);
    (*dre_dynrnd_DROM_fcns->set_att_srfshd) (callbackobj);
}
/*
 ======================================================================
 */
void drr_dynrnd_transpclr (void)
{
    DtColorModel colormodel;
    DtReal color[3];
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTranspColor"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&colormodel, color);
    (*dre_dynrnd_DROM_fcns->set_att_transpclr) (colormodel, color);
}
/*
 ======================================================================
 */
void drr_dynrnd_transpint (void)
{
    DtReal value;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTranspIntens"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&value);
    (*dre_dynrnd_DROM_fcns->set_att_transpint) (value);
}
/*
 ======================================================================
 */
void drr_dynrnd_transpswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTranspSwitch"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_transpswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_transporientclr (void)
{
    DtColorModel colormodel;
    DtReal color[3];
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttTranspOrientColor"),
					DcMethodInqGlbAttVal);
    }
    (*query)(&colormodel, color);
    (*dre_dynrnd_DROM_fcns->set_att_transporientclr) (colormodel, color);
}
/*
 ======================================================================
 */
void drr_dynrnd_transporientexp (void)
{
    DtReal exponent;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttTranspOrientExp"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&exponent);
    (*dre_dynrnd_DROM_fcns->set_att_transporientexp) (exponent);
}
/*
 ======================================================================
 */
void drr_dynrnd_transporientint (void)
{
    DtReal value;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttTranspOrientIntens"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&value);
    (*dre_dynrnd_DROM_fcns->set_att_transporientint) (value);
}
/*
 ======================================================================
 */
void drr_dynrnd_transporientswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttTranspOrientSwitch"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_transporientswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_smpadapt (void)
{
    DtReal variance;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttSampleAdaptive"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&variance);
    (*dre_dynrnd_DROM_fcns->set_att_smpadapt) (variance);
}
/*
 ======================================================================
 */
void drr_dynrnd_smpadaptswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttSampleAdaptiveSwitch"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_smpadaptswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_smpfilter (void)
{
    dot_object *callbackobj;
    DtReal xwidth, ywidth;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttSampleFilter"),
				   DcMethodInqGlbAttVal);
    }
    (*query)(&callbackobj, &xwidth, &ywidth);
    (*dre_dynrnd_DROM_fcns->set_att_smpfilter) (callbackobj, xwidth, ywidth);
}
/*
 ======================================================================
 */
void drr_dynrnd_smpjitter (void)
{
    DtReal factor;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttSampleJitter"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&factor);
    (*dre_dynrnd_DROM_fcns->set_att_smpjitter) (factor);
}
/*
 ======================================================================
 */
void drr_dynrnd_smpjitterswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttSampleJitterSwitch"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_smpjitterswi) (switchvalue);
}
/*
 ======================================================================
 */
void drr_dynrnd_smpsuper (void)
{
    DtInt xsamples, ysamples;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttSampleSuper"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&xsamples, &ysamples);
    (*dre_dynrnd_DROM_fcns->set_att_smpsuper) (xsamples, ysamples);
}
/*
 ======================================================================
 */
void drr_dynrnd_smpsuperswi (void)
{
    DtSwitch switchvalue;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
        query = DDclass_InqMethod(DsInqClassId("AttSampleSuperSwitch"),
                                   DcMethodInqGlbAttVal);
    }
    (*query)(&switchvalue);
    (*dre_dynrnd_DROM_fcns->set_att_smpsuperswi) (switchvalue);
}
