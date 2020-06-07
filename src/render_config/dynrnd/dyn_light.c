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
	drr_dynrnd_light
	drr_dynrnd_lgtatn
	drr_dynrnd_lgtclr
	drr_dynrnd_lgtint
	drr_dynrnd_lgtspdang
	drr_dynrnd_lgtspdexp
	drr_dynrnd_lgtswi
	drr_dynrnd_lgttyp

  ======================================================================
 */
#include "dynrnd.h"



/* ====================================================================== */

void drr_dynrnd_light (dot_object *object)
{
    (*dre_dynrnd_DROM_fcns->light)(object);
}

/* ====================================================================== */

void drr_dynrnd_lgtatn (void)
{
    DtReal c1;
    DtReal c2;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLightAttenuation"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&c1, &c2);
    (*dre_dynrnd_DROM_fcns->set_att_lgtatn)(c1, c2);
}

/* ====================================================================== */

void drr_dynrnd_lgtclr (void)
{
    DtColorModel colormodel;
    DtReal color[3];
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLightColor"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&colormodel, color);
    (*dre_dynrnd_DROM_fcns->set_att_lgtclr) (colormodel, color);
}

/* ====================================================================== */

void drr_dynrnd_lgtint (void)
{
    DtReal intensity;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLightIntens"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&intensity);
    (*dre_dynrnd_DROM_fcns->set_att_lgtint) (intensity);
}

/* ====================================================================== */

void drr_dynrnd_lgtspdang (void)
{
    DtReal total;
    DtReal delta;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLightSpreadAngles"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&total, &delta);
    (*dre_dynrnd_DROM_fcns->set_att_lgtspdang)(total, delta);
}

/* ====================================================================== */

void drr_dynrnd_lgtspdexp (void)
{
    DtReal exponent;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLightSpreadExp"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&exponent);
    (*dre_dynrnd_DROM_fcns->set_att_lgtspdexp)(exponent);
}

/* ====================================================================== */

void drr_dynrnd_lgtswi (void)
{
    DtInt count;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLightSwitch"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&count, dre_dynrnd_data.light_switch, 
	     dre_dynrnd_data.light_switch_count);
    if (count > dre_dynrnd_data.light_switch_count) {
	dre_dynrnd_data.light_switch =
		DDspace_Reallocate (dre_dynrnd_data.light_switch,
				    count*sizeof(char*));
	dre_dynrnd_data.light_switch_count = count;

	(*query)(&count, dre_dynrnd_data.light_switch, 
		 dre_dynrnd_data.light_switch_count);
    }


    (*dre_dynrnd_DROM_fcns->set_att_lgtswi)
	(count, (DtObject*) dre_dynrnd_data.light_switch);
}

/* ====================================================================== */

void drr_dynrnd_lgttyp (void)
{
    dot_object *type;
    static DtMethodPtr query = DcNullPtr;

    if (query == DcNullPtr) {
	query = DDclass_InqMethod(DsInqClassId("AttLightType"),
				  DcMethodInqGlbAttVal);
    }
    (*query)(&type);
    (*dre_dynrnd_DROM_fcns->set_att_lgttyp) (type);
}
