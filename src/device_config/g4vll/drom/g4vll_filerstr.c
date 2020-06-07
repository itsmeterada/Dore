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
	DtPtr ddr_g4vll_drom_filerstr_create (object)
	void ddr_g4vll_drom_filerstr_print (object, data)
	void ddr_g4vll_drom_filerstr_delete (object, data)
	void ddr_g4vll_drom_filerstr_update (object)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/filerstr.h"
/*
  ======================================================================
 */
DtPtr ddr_g4vll_drom_filerstr_create (
    DtObject object)
{
    return (DcNullPtr);
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_filerstr_print (
    DtObject object,
    DtObjectStructure *data)
{
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_filerstr_delete (
    DtObject object,
    DtObjectStructure *data)
{
    if (data)
	DeDeleteObject(data);
}
/*
  ======================================================================
 */
void ddr_g4vll_drom_filerstr_update (
    DtObjectStructure *object)
{
    struct filerstr *filerstr;
    static DtInt dataindex = -1;
    static DtInt classid = -1;
    DtObjectStructure *alt_obj;
    DtInt width, height, depth;
    DtRasterType type;
    DtPtr data;
    DtInt i;

    if (classid == -1) {
	classid = DsInqClassId("DoFileRaster");
    }

    if (dataindex == -1) {
	for (i=0; dde_g4vll_drom_class_data[i].name != 0; i++) {
	    if (dde_g4vll_drom_class_data[i].id == classid) {
		dataindex = dde_g4vll_drom_class_data[i].index;
		break;
	    }
	}
    }

    filerstr = (struct filerstr *)(object->data);

    alt_obj = (DtObjectStructure *)object->additional_data[dataindex];

    if (alt_obj == DcNullPtr) {
	/*
	 * create a memory raster if possible
	 */

	if (filerstr->type == DcNullPtr) {
	    if (DsFileRasterRead (filerstr->filename, &width, &height, &depth, 
				  &type, &data) != -1) {
		alt_obj = (DtObjectStructure *)
			DoRaster (width, height, depth, type, DcNullPtr, data,
				  DcDeleteData);

		object->additional_data[dataindex] = (DtPtr)alt_obj;
	    }
	}
    }
}
/*
  ======================================================================
 */
