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
	int dor_filerstr_initialize ()
	dot_object * dor_filerstr_create (filename, type)
	int dor_filerstr_destroy (object)
	int dor_filerstr_print (object)

  ======================================================================
 */
#include <string.h>
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/filerstr.h>

DtInt doe_filerstr_class_id = -1;
DtFlag doe_filerstr_initialized = DcFalse;
/*
 ======================================================================
 */

static DtMethodEntry filerstr_methods[] = {
    { DcMethodPrint,   (DtMethodPtr) dor_filerstr_print },
    { DcMethodDestroy, (DtMethodPtr) dor_filerstr_destroy },
};

void dor_filerstr_initialize (void)
{
    doe_filerstr_class_id = dor_class_add ("DoFileRaster", 2,
					   filerstr_methods,DcNullPtr);
}
/*
 ======================================================================
 */

dot_object *dor_filerstr_create (char *filename, char *type)
{
    struct filerstr *filerstr;

    if (!doe_filerstr_initialized) {
	dor_filerstr_initialize();
	doe_filerstr_initialized = DcTrue;
    }

    filerstr = (struct filerstr *)dor_space_allocate(sizeof *filerstr);

    if (filename == (DtPtr)0) {
	DDerror (ERR_VALUE_OUT_OF_RANGE, "DoFileRaster", "Null filename");
	return ((dot_object *)dor_null_create(DcNullPtr));
    } 

    filerstr->filename = dor_space_allocate (1 + strlen(filename));
    if (!filerstr->filename) {
	DDerror (ERR_CANT_ALLOC_MEM, "DoFileRaster", DcNullPtr);
	return((dot_object *)dor_null_create(DcNullPtr));
    }
    strcpy(filerstr->filename, filename);

    if (type != (DtPtr)0) {
	filerstr->type = dor_space_allocate(strlen(type)+1);
	if (!filerstr->type) {
	    DDerror (ERR_CANT_ALLOC_MEM, "DoFileRaster", DcNullPtr);
	    dor_space_deallocate(filerstr->filename);
	    dor_space_deallocate(filerstr);
	    return ((dot_object *)dor_null_create(DcNullPtr));
	}
	strcpy(filerstr->type, type);
    } else {
	filerstr->type = DcNullPtr;
    }
      
    return (dor_object_create(doe_filerstr_class_id,filerstr));
}
/*
 ======================================================================
 */

void dor_filerstr_destroy (dot_object *object)
{
    struct filerstr *filerstr;

    filerstr = (struct filerstr *) object->data;

    dor_space_deallocate (filerstr->filename);

    if (filerstr->type != (DtPtr)0) {
	dor_space_deallocate (filerstr->type);
    }

    dor_space_deallocate (filerstr);
}
/*
 ======================================================================
 */

void dor_filerstr_print (dot_object *object)
{
    struct filerstr *filerstr;

    filerstr = (struct filerstr *)object->data;

    if (filerstr == NULL) {
	dor_print_output("bad values");
	return;
    }

    sprintf(dor_print_get_line(),
	    "filename = %s",filerstr->filename);
    dor_print_output(dor_print_get_line());
    if (filerstr->type != (DtPtr)0) {
	sprintf(dor_print_get_line(),
		"type = %s",filerstr->type);
	dor_print_output(dor_print_get_line());
    }
}
