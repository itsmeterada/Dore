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
	ddr_gl_drom_rstr_create
	ddr_gl_drom_rstr_print
	ddr_gl_drom_rstr_delete
	ddr_gl_drom_rstr_update

  ======================================================================
 */
#include "../gl_driver.h"
#include <dore/dore_develop/private/rstr.h>



/* ======================================================================== */

static DtInt rstr_dataindex = -1;
static DtInt rstr_classid = -1;



/* ======================================================================== */

ddt_gl_drom_rstr *ddr_gl_drom_rstr_create (DtObject object)
{
    ddt_gl_drom_rstr *dynout_rstr;

    dynout_rstr = DDspace_Allocate (sizeof(ddt_gl_drom_rstr));

    dynout_rstr->bgd.data = 0;
    dynout_rstr->bgd.z = 0;
    dynout_rstr->bgd.sequence_number = -1;

    dynout_rstr->tex.sequence_number = -1;
    dynout_rstr->tex.id = -1;
    dynout_rstr->tex.data = 0;
    return dynout_rstr;
}



/* ======================================================================== */

void ddr_gl_drom_rstr_print (
    DtObject object,
    ddt_gl_drom_rstr *data)
{
}



/* ======================================================================== */

void ddr_gl_drom_rstr_delete (
    DtObject object,
    ddt_gl_drom_rstr *data)
{
    if  (data)
    {   if (data->bgd.data) DDspace_Deallocate (data->bgd.data);
	if (data->bgd.z)    DDspace_Deallocate (data->bgd.z);
	if (data->tex.data) DDspace_Deallocate (data->tex.data);
	DDspace_Deallocate (data);
    }
}



/* ======================================================================== */

void ddr_gl_drom_rstr_update (DtObjectStructure *object)
{
    struct rstr *rstr;
    ddt_gl_drom_rstr *dynout_rstr;
    int i;
    int w, h, skip, have_z, have_alpha;
    Dt32Bits *d1;
    Dt32Bits *d2;
    Dt32Bits *dz;

    if (rstr_classid == -1) {
	rstr_classid = DsInqClassId("DoRaster");
    }

    if (rstr_dataindex == -1) {
	for (i=0; dde_gl_drom_class_data[i].name != 0; i++) {
	    if (dde_gl_drom_class_data[i].id == rstr_classid) {
		rstr_dataindex = dde_gl_drom_class_data[i].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(object->data);
    dynout_rstr = (ddt_gl_drom_rstr *)object->additional_data[rstr_dataindex];

    if ((dynout_rstr->bgd.sequence_number != rstr->sequence_number) ||
	(dynout_rstr->bgd.data == 0)) {

	switch (rstr->type) {
	    case DcRasterRGB:    skip = 3; have_alpha = 0; have_z = 0; break;
	    case DcRasterRGBA:   skip = 4; have_alpha = 1; have_z = 0; break;
	    case DcRasterRGBZ:   skip = 7; have_alpha = 0; have_z = 3; break;
	    case DcRasterRGBAZ:  skip = 8; have_alpha = 1; have_z = 4; break;
	    case DcRasterABGR:   skip = 4; have_alpha = 1; have_z = 0; break;
	    default:
		return;
	}
      
	if (dynout_rstr->bgd.data) DDspace_Deallocate (dynout_rstr->bgd.data);
	if (dynout_rstr->bgd.z)    DDspace_Deallocate (dynout_rstr->bgd.z);
	dynout_rstr->bgd.data = 0;
	dynout_rstr->bgd.z = 0;

	if (have_z) {
	    dynout_rstr->bgd.z = DDspace_Allocate
				 (rstr->width*rstr->height * sizeof(Dt32Bits));
	    if (!dynout_rstr->bgd.z) return;
	}
	dynout_rstr->bgd.data = DDspace_Allocate
				(rstr->width * rstr->height * sizeof(Dt32Bits));
	if (!dynout_rstr->bgd.data) return;

	d1 = (Dt32Bits*) rstr->data;
	d2 = ((Dt32Bits*)(dynout_rstr->bgd.data))
	   + (rstr->width * (rstr->height - 1));
	if (have_z)
	    dz = ((Dt32Bits*)(dynout_rstr->bgd.z))
	       + (rstr->width * (rstr->height - 1));

	if (rstr->type == DcRasterABGR) {
	    h = rstr->height;
	    while (h--) {
		for (w=rstr->width; w--;) {
		    if (have_z) {
			dz[0] = d1[have_z+0];
			dz[1] = d1[have_z+1];
			dz[2] = d1[have_z+2];
			dz[3] = d1[have_z+3];
			dz += 4;
		    }
	       
		    d2[0] = have_alpha ? d1[0] : 0xFF;
		    d2[1] = d1[1];
		    d2[2] = d1[2];
		    d2[3] = d1[3];

		    d1 += skip;
		    d2 += 4;
		}
		d2 -= 2 * rstr->width;
		dz -= 2 * rstr->width;
	    }
	} else {
	    h = rstr->height;
	    while (h--) {
		for (w=rstr->width; w--;) {
		    if (have_z) {
			dz[0] = d1[have_z+0];
			dz[1] = d1[have_z+1];
			dz[2] = d1[have_z+2];
			dz[3] = d1[have_z+3];
			dz += 4;
		    }
	       
		    d2[0] = have_alpha ? d1[3] : 0xFF;
		    d2[1] = d1[2];
		    d2[2] = d1[1];
		    d2[3] = d1[0];

		    d1 += skip;
		    d2 += 4;
		}
		d2 -= 2 * rstr->width;
		dz -= 2 * rstr->width;
	    }
	}
	dynout_rstr->bgd.sequence_number = rstr->sequence_number;
    }
}



/* ======================================================================== */

void ddr_gl_drom_rstr_tex_update (DtObjectStructure *object)
{
    struct rstr *rstr;
    ddt_gl_drom_rstr *dynout_rstr;
    int i;
    int w, h, skip, have_z, have_alpha;
    unsigned char *d1;
    unsigned char *d2;

    if (rstr_classid == -1) {
	rstr_classid = DsInqClassId("DoRaster");
    }

    if (rstr_dataindex == -1) {
	for (i=0; dde_gl_drom_class_data[i].name != 0; i++) {
	    if (dde_gl_drom_class_data[i].id == rstr_classid) {
		rstr_dataindex = dde_gl_drom_class_data[i].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(object->data);
    dynout_rstr = (ddt_gl_drom_rstr *)object->additional_data[rstr_dataindex];

    if ((dynout_rstr->tex.sequence_number != rstr->sequence_number) ||
	(dynout_rstr->tex.data == 0)) {

	switch (rstr->type) {
	    case DcRasterRGB:	skip = 3; have_alpha = 0; have_z = 0; break;
	    case DcRasterRGBA:	skip = 4; have_alpha = 1; have_z = 0; break;
	    case DcRasterRGBZ:	skip = 7; have_alpha = 0; have_z = 3; break;
	    case DcRasterRGBAZ:	skip = 8; have_alpha = 1; have_z = 4; break;
	    case DcRasterABGR:	
		return;
	    default:
		return;
	}
      
	if (dynout_rstr->tex.data) 
	    DDspace_Deallocate (dynout_rstr->tex.data);
	dynout_rstr->tex.data = 0;

	dynout_rstr->tex.data = DDspace_Allocate
				(rstr->width * rstr->height * sizeof(Dt32Bits));
	if (!dynout_rstr->tex.data) {
	    return;
	}

	d1 = (unsigned char *) rstr->data;
	d2 = (unsigned char *) dynout_rstr->tex.data;

	for (h=rstr->height; h--; ) {
	    for (w=rstr->width; w--;) {
		d2[0] = (have_alpha) ? d1[3] : 0xFF;
		d2[1] = d1[2];
		d2[2] = d1[1];
		d2[3] = d1[0];

		d1 += skip;
		d2 += 4;
	    }
	}
    }
}
