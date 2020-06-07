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
	int dor_lgttyp_initialize ()
	int dor_lgttyp_initialize_types ()
	static dor_light_ambient_handler ()
	static dor_light_infinite_handler ()
	static dor_light_point_handler ()
	static dor_light_point_attn_handler ()
	static dor_light_spot_handler ()
	static dor_light_spot_attn_handler ()
	dot_object * dor_lgttyp_create (lgtobj)
	int dor_lgttyp_destroy (object)
	int dor_lgttyp_print (object)
	int dor_lgttyp_execute (object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/internal/glbatt.h>
#include <dore/internal/studio.h>

struct lgttyp_data {
    DtObject lgtobj;
};
/*
 ======================================================================
 */

DtInt doe_lgttyp_class_id;

static DtMethodEntry lgttyp_methods[] = {
    { DcMethodPrint,            (DtMethodPtr) dor_lgttyp_print },
    { DcMethodDestroy,          (DtMethodPtr) dor_lgttyp_destroy },
    { DcMethodStdRenderStudio,  (DtMethodPtr) dor_lgttyp_execute },
    { DcMethodStdRenderDisplay, (DtMethodPtr) dor_lgttyp_execute },
};

void dor_lgttyp_initialize (void)
{
    doe_lgttyp_class_id = dor_class_install (DcTypeLightType, "DoLightType", 4,
					     lgttyp_methods,DcNullPtr);
}
/*
 ======================================================================
 */

DtObject DcLightAmbient;
DtObject DcLightInfinite;
DtObject DcLightPoint;
DtObject DcLightPointAttn;
DtObject DcLightSpot;
DtObject DcLightSpotAttn;

/*  Initialize the light callback objects provided with standard Dore.  */

static void dor_light_ambient_handler	(void) { return; }
static void dor_light_infinite_handler	(void) { return; }
static void dor_light_point_handler	(void) { return; }
static void dor_light_point_attn_handler(void) { return; }
static void dor_light_spot_handler	(void) { return; }
static void dor_light_spot_attn_handler	(void) { return; }

void dor_lgttyp_initialize_types (void)
{
    dot_object *datavalobj;

    datavalobj = dor_datval_create((Dt32Bits)0);

    DcLightAmbient = dor_calbak_create ((DtPFI) dor_light_ambient_handler,
					datavalobj, DcFalse);

    DcLightInfinite = dor_calbak_create ((DtPFI) dor_light_infinite_handler,
					 datavalobj, DcFalse);

    DcLightPoint = dor_calbak_create ((DtPFI) dor_light_point_handler,
				      datavalobj, DcFalse);

    DcLightPointAttn = dor_calbak_create ((DtPFI) dor_light_point_attn_handler,
					  datavalobj, DcFalse);

    DcLightSpot = dor_calbak_create ((DtPFI) dor_light_spot_handler,
				     datavalobj, DcFalse);

    DcLightSpotAttn = dor_calbak_create ((DtPFI) dor_light_spot_attn_handler,
					 datavalobj, DcFalse);

    /* Create dummy references to these objects in the event that
     * they are dereferenced such that their reference count goes to
     * zero: this keeps them from being deleted entirely.
     */

    dor_object_add_reference (DcLightAmbient,   DcNullObject);
    dor_object_add_reference (DcLightInfinite,  DcNullObject);
    dor_object_add_reference (DcLightPoint,     DcNullObject);
    dor_object_add_reference (DcLightPointAttn, DcNullObject);
    dor_object_add_reference (DcLightSpot,      DcNullObject);
    dor_object_add_reference (DcLightSpotAttn,  DcNullObject);

}

/*
 ======================================================================
 */

dot_object *dor_lgttyp_create (DtObject lgtobj)
{
    static DtFlag initialized = DcFalse;
    struct lgttyp_data *lgttypdata;
    dot_object *lgttypobject;

    if (!initialized) {
	dor_lgttyp_initialize();
	initialized = DcTrue;
    }

    lgttypdata = (struct lgttyp_data *)dor_space_allocate(sizeof *lgttypdata);

    lgttypdata->lgtobj = lgtobj;

    lgttypobject = dor_object_create(doe_lgttyp_class_id,lgttypdata);

    return(lgttypobject);
}
/*
 ======================================================================
 */

void dor_lgttyp_destroy (dot_object *object)
{
    struct lgttyp *lgttyp;

    lgttyp = (struct lgttyp *)(object->data);

    dor_space_deallocate(lgttyp);
}
/*
 ======================================================================
 */

void dor_lgttyp_print (dot_object *object)
{
    struct lgttyp_data *lgttyp;

    lgttyp = (struct lgttyp_data *)object->data;

    if (lgttyp == NULL) {
        dor_print_output("bad values");
        return;

    } else if (lgttyp->lgtobj == DcLightAmbient) {
	dor_print_output("DcLightAmbient");

    } else if (lgttyp->lgtobj == DcLightInfinite) {
	dor_print_output("DcLightInfinite");

    } else if (lgttyp->lgtobj == DcLightPoint) {
	dor_print_output("DcLightPoint");

    } else if (lgttyp->lgtobj == DcLightPointAttn) {
	dor_print_output("DcLightPointAttn");

    } else if (lgttyp->lgtobj == DcLightSpot) {
	dor_print_output("DcLightSpot");

    } else if (lgttyp->lgtobj == DcLightSpotAttn) {
	dor_print_output("DcLightSpotAttn");

    } else {
	sprintf(dor_print_get_line(),"light object = [%lx]:",lgttyp->lgtobj);
        dor_print_output(dor_print_get_line());
    }
}
/*
 ======================================================================
 */

void dor_lgttyp_execute (dot_object *object)
{
    struct lgttyp_data *lgttypdat;

    if (!dor_condex_query_execute(doe_lgttyp_class_id))
	    return;

    lgttypdat = (struct lgttyp_data *)object->data;

    dor_global_lgttyp_set_value(lgttypdat->lgtobj);
}
