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
	void drr_dynrnd_lcstowcs (matrix, exe_type, pop_cnt)
	void drr_dynrnd_lokatfrm (object)
	void drr_dynrnd_rotate (object)
	void drr_dynrnd_scale (object)
	void drr_dynrnd_shear (object)
	void drr_dynrnd_tfmmat (object)
	void drr_dynrnd_transl (object)
	void drr_dynrnd_get_wcstofcsmat (wcstofcsmat)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/dore_develop/private/lokatfrm.h>
#include <dore/dore_develop/private/rotate.h>
#include <dore/dore_develop/private/scale.h>
#include <dore/dore_develop/private/shear.h>
#include <dore/dore_develop/private/tfmmat.h>
#include <dore/dore_develop/private/transl.h>
#include "dynrnd.h"

/* ====================================================================== */

void drr_dynrnd_lcstowcs (
    DtMatrix4x4 matrix,
    DtGlobalCall exe_type,
    DtInt pop_cnt)
{
    DtInt i;

    switch (exe_type) {
	case DcGlobalCallPush:
	    (*dre_dynrnd_DROM_fcns->update_lcstowcsmat_push) ();
	    break;

	case DcGlobalCallPop:
	    for (i=0; i<pop_cnt; i++) {
		(*dre_dynrnd_DROM_fcns->update_lcstowcsmat_pop) ();
	    }
	    break;
	case DcGlobalCallGetValue:
	    (*dre_dynrnd_DROM_fcns->get_lcstowcsmat) (matrix);
	    break;

	case DcGlobalCallSetValue:
	    (*dre_dynrnd_DROM_fcns->update_lcstowcsmat_tfmmat)
		(matrix, DcReplace);
	    break;
	
	default:
	    break;
    }
}
/*
 ======================================================================
 */
void drr_dynrnd_lokatfrm (
    dot_object *object)
{
    struct lokatfrm_data *lokatfrmdat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoLookAtFrom");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    lokatfrmdat = (struct lokatfrm_data *)object->data;

    dor_global_lcstowcsmat_notify_change ();

    (*dre_dynrnd_DROM_fcns->update_lcstowcsmat_lokatfrm) (
						lokatfrmdat->at,
						lokatfrmdat->from,
						lokatfrmdat->up,
						DcPreConcatenate);
}
/*
 ======================================================================
 */
void drr_dynrnd_rotate (
    dot_object *object)
{
    struct rotate_data *rotdat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoRotate");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    rotdat = (struct rotate_data *)object->data;

    dor_global_lcstowcsmat_notify_change ();

    (*dre_dynrnd_DROM_fcns->update_lcstowcsmat_rotate) (
						rotdat->axis, 
						rotdat->radians, 
						DcPreConcatenate);
}
/*
 ======================================================================
 */
void drr_dynrnd_scale (
    dot_object *object)
{
    struct scale_data *scldat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoScale");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    scldat = (struct scale_data *)object->data;

    dor_global_lcstowcsmat_notify_change ();

    (*dre_dynrnd_DROM_fcns->update_lcstowcsmat_scale) (
					scldat->scalevalue[0],
					scldat->scalevalue[1],
					scldat->scalevalue[2],
					DcPreConcatenate);
}
/*
 ======================================================================
 */
void drr_dynrnd_shear (
    dot_object *object)
{
    struct shear_data *shrdat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoShear");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    shrdat = (struct shear_data *)object->data;

    dor_global_lcstowcsmat_notify_change ();

    (*dre_dynrnd_DROM_fcns->update_lcstowcsmat_shear) (
					shrdat->plane,
					shrdat->firstdirectionshearvalue,
					shrdat->seconddirectionshearvalue,
					DcPreConcatenate);
}
/*
 ======================================================================
 */
void drr_dynrnd_tfmmat (
    dot_object *object)
{
    struct tfmmat_data *tfmmatdat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoTransformMatrix");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    tfmmatdat = (struct tfmmat_data *)object->data;

    dor_global_lcstowcsmat_notify_change ();

    (*dre_dynrnd_DROM_fcns->update_lcstowcsmat_tfmmat) (
					tfmmatdat->tfmmat, 
					tfmmatdat->comptype);
}

				      
/*
 ======================================================================
 */
void drr_dynrnd_transl (
    dot_object *object)
{
    struct transl_data *transldat;
    static DtInt class_id = -1;

    if (class_id == -1) {
	class_id = DsInqClassId("DoTranslate");
    }

    if (!DDcondex_QueryExecute(class_id))
	    return;

    transldat = (struct transl_data *)object->data;

    dor_global_lcstowcsmat_notify_change ();

    (*dre_dynrnd_DROM_fcns->update_lcstowcsmat_transl) (
					transldat->delta[0],
					transldat->delta[1],
					transldat->delta[2],
					DcPreConcatenate);
}
/*
 ======================================================================
 */
void drr_dynrnd_get_wcstofcsmat (
    DtMatrix4x4 wcstofcsmat)
{
    (*dre_dynrnd_DROM_fcns->get_wcstofcsmat) (wcstofcsmat);
}
/*
 ======================================================================
 */
