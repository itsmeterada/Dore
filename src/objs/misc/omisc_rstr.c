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
	dor_rstr_initialize
	dor_rstr_delete_data
	dor_rstr_init_delete_callback
	dor_rstr_create
	dor_rstr_destroy
	dor_rstr_print
	dor_rstr_inq
	dor_rstr_update

  ======================================================================
 */
#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/rstr.h>

DtInt doe_rstr_class_id = -1;
DtFlag doe_rstr_initialized = DcFalse;

/* ====================================================================== */

static DtMethodEntry rstr_methods[] = {
    { DcMethodPrint,   (DtMethodPtr) dor_rstr_print },
    { DcMethodDestroy, (DtMethodPtr) dor_rstr_destroy },
};

DtObject DcDeleteData = DcNullObject;



/* ====================================================================== */

void dor_rstr_initialize (void)
{
    doe_rstr_class_id = dor_class_add ("DoRaster", 2, rstr_methods,DcNullPtr);
}



/* ====================================================================== */

static void dor_rstr_delete_data (
    Dt32Bits dataval,
    DtPtr data)
{
    if (data) dor_space_deallocate (data);
}



/* ====================================================================== */

void dor_rstr_init_delete_callback (void)
{
    if (DcDeleteData == DcNullObject) {
	DcDeleteData = dor_calbak_create ((DtPFI) dor_rstr_delete_data,
					  dor_datval_create((Dt32Bits)0),
					  DcFalse);

	/* Create dummy reference to this object in the event that it
	 * is dereferenced such that its reference count goes to zero:
	 * this keeps it from being deleted entirely.
	 */

	dor_object_add_reference(DcDeleteData, DcNullObject);
    }
}

/* ====================================================================== */

dot_object *dor_rstr_create (
    DtInt width,
    DtInt height,
    DtInt depth,
    DtRasterType type,
    DtPtr typestring,
    DtPtr data,
    DtObject delcallback)
{
   struct rstr *rstr;

   if (!doe_rstr_initialized) {
      dor_rstr_initialize();
      doe_rstr_initialized = DcTrue;
   }

   rstr = (struct rstr *)dor_space_allocate(sizeof *rstr);
   if (rstr == DcNullPtr) {
      DDerror (ERR_CANT_ALLOC_MEM, "DoRaster", DcNullPtr);
      return ((dot_object *)dor_null_create (DcNullPtr));
   }

   rstr->width = width;
   rstr->height = height;
   rstr->depth = depth;
   rstr->type = type;
   if (typestring != (DtPtr *)0) {
      rstr->typestring = dor_space_allocate (strlen(typestring)+1);
      if (!rstr->typestring) {
	 DDerror (ERR_CANT_ALLOC_MEM, "DoRaster", DcNullPtr);
	 dor_space_deallocate (rstr);
	 return ((dot_object *)dor_null_create(DcNullPtr));
      }
      strcpy(rstr->typestring, typestring);
   } else {
      rstr->typestring = 0;
   }
   rstr->data = data;
   rstr->sequence_number = 1;

   if ((delcallback != DcNullObject) &&
       ((DcObjectValid != dor_object_validate(delcallback)) ||
	(dor_object_inq_type (delcallback) != 
	 dor_class_inq_id("DoCallback")))) {
      DDerror (ERR_INVALID_CALLBACK, "DoRaster", DcNullPtr);
      if (rstr->typestring != DcNullPtr)
	    dor_space_deallocate (rstr->typestring);
      dor_space_deallocate (rstr);
      return ((dot_object *)dor_null_create(DcNullPtr));
   }
   rstr->delcallback = delcallback;

   return (dor_object_create(doe_rstr_class_id,rstr));
}

/* ====================================================================== */

void dor_rstr_destroy (dot_object *object)
{
    struct rstr *rstr;
    DtPFI function;
    dot_object *dataobject;
    DtPtr datptr;
    Dt32Bits datval;
    DtInt type;
    DtInt dataptr_class_id = dor_class_inq_id("DoDataPtr");
    DtInt dataval_class_id = dor_class_inq_id("DoDataVal");

    rstr = (struct rstr *) object->data;

    if (rstr->typestring) {
	dor_space_deallocate (rstr->typestring);
    }

    /*
     * call the user-defined function to delete the raster data
     */

    if (rstr->delcallback != DcNullPtr) {
	function = dor_calbak_get_function(rstr->delcallback);
	dataobject = dor_calbak_get_dataobject(rstr->delcallback);
	if (dataobject == DcNullObject) {
	    datval = 0;
	    if (dor_calbak_get_pass_by_ref(rstr->delcallback)) {
		(*function)(&datval, &(rstr->data));
	    } else {
		(*function)(datval, rstr->data);
	    }
	} else if ((DcObjectValid == dor_object_validate(dataobject)) &&
		   (((type = dor_object_inq_type(dataobject)) == dataptr_class_id) ||
		    (type == dataval_class_id))) {
	    if (type == dataptr_class_id) {
		datptr = dor_datptr_get_value(dataobject);
		if (dor_calbak_get_pass_by_ref(rstr->delcallback)) {
		    (*function)(&datptr, &(rstr->data));
		} else {
		    (*function)(datptr, rstr->data);
		}
	    } else if (type == dataval_class_id) {
		datval = dor_datval_get_value(dataobject);
		if (dor_calbak_get_pass_by_ref(rstr->delcallback)) {
		    (*function)(&datval, &(rstr->data));
		} else {
		    (*function)(datval, rstr->data);
		}
	    }
	} else {
	    DDerror(ERR_INVALID_DATAOBJECT, 
			 "dor_rstr_destroy", DcNullPtr);
	}
    }

    dor_space_deallocate (rstr);
}

/* ====================================================================== */

void dor_rstr_print (dot_object *object)
{
    struct rstr *rstr;

    rstr = (struct rstr *)object->data;

    if (rstr == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "width = %d",rstr->width);
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(),
	    "height = %d",rstr->height);
    dor_print_output(dor_print_get_line());
    sprintf(dor_print_get_line(),
	    "depth = %d",rstr->depth);
    dor_print_output(dor_print_get_line());

    switch (rstr->type) {
    case DcRasterRGB:
	dor_print_output("type = DcRasterRGB");
	break;
    case DcRasterRGBA:
	dor_print_output("type = DcRasterRGBA");
	break;
    case DcRasterRGBAZ:
	dor_print_output("type = DcRasterRGBAZ");
	break;
    case DcRasterRGBZ:
	dor_print_output("type = DcRasterRGBZ");
	break;
    case DcRasterA:
	dor_print_output("type = DcRasterA");
	break;
    case DcRasterABGR:
	dor_print_output("type = DcRasterABGR");
	break;
    case DcRasterZ:
	dor_print_output("type = DcRasterZ");
	break;
    case DcRasterSpecial:
	sprintf(dor_print_get_line(),
		"type = DcRasterSpecial '%s'",rstr->typestring);
	dor_print_output(dor_print_get_line());
	break;
    default:
	sprintf(dor_print_get_line(),
		"type = <UNKNOWN> %d",rstr->type);
	dor_print_output(dor_print_get_line());
	break;
    }
}

/* ====================================================================== */

void dor_rstr_inq (
    dot_object *object,
    DtInt *width,
    DtInt *height,
    DtInt *depth,
    DtRasterType *type,
    char **typestring,
    DtPtr *data)
{
    struct rstr *rstr;

#ifdef ERRCHK
    if (!dor_object_confirm_type(object,doe_rstr_class_id))	{
	DDerror(ERR_INVALID_RASTER, "DsInqRaster",DcNullPtr);
	return;
    }
#endif

    rstr = (struct rstr *) object->data;

    *width = rstr->width;
    *height = rstr->height;
    *depth = rstr->depth;
    *type = rstr->type;
    *typestring = rstr->typestring;
    *data = rstr->data;
}

/* ====================================================================== */

void dor_rstr_update (dot_object *object)
{
    struct rstr *rstr;

    rstr = (struct rstr *) object->data;

    rstr->sequence_number ++;
}
