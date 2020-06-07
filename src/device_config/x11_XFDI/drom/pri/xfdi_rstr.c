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
	ddt_x11_XFDI_drom_rstr * ddr_x11_XFDI_drom_rstr_create (object)
	int ddr_x11_XFDI_drom_rstr_print (object, data)
	int ddr_x11_XFDI_drom_rstr_delete (object, data)
	ddr_x11_XFDI_drom_rstr_update ddr_x11_XFDI_drom_rstr_update (object)

  ======================================================================
 */
#include "../drom.h"
#include "dore/dore_develop/private/rstr.h"
/*
 ======================================================================
 */
ddt_x11_XFDI_drom_rstr *
ddr_x11_XFDI_drom_rstr_create(object)
     DtObject object;
{
    ddt_x11_XFDI_drom_rstr *dynout_rstr;

    dynout_rstr =  (ddt_x11_XFDI_drom_rstr *)
	    DDspace_Allocate(sizeof(ddt_x11_XFDI_drom_rstr));

    dynout_rstr->sequence_number = -1;
    dynout_rstr->texture_id = -1;
    dynout_rstr->pixmap = -1;

    return dynout_rstr;
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_rstr_print(object, data)
     DtObject object;
     ddt_x11_XFDI_drom_rstr *data;
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_rstr_delete(object, data)
     DtObject object;
     ddt_x11_XFDI_drom_rstr *data;
{
    if  (data != (ddt_x11_XFDI_drom_rstr *)0) {
	if (data->texture_id != -1) {
	    XFDIDestroyTexture(dde_x11_XFDI_drom_curdevdat->display,
			       data->texture_id);
	}
	if (data->pixmap != -1) {
	    XFreePixmap (dde_x11_XFDI_drom_curdevdat->display,
			 data->pixmap);
	}
	DDspace_Deallocate(data);
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_rstr_update (object)
     DtObjectStructure *object;
{
    static DtInt dataindex = -1;
    static DtInt classid = -1;
    struct rstr *rstr;
    ddt_x11_XFDI_drom_rstr *dynout_rstr;
    DtInt i;
    DtInt skip;
    unsigned char *r_ptr;
    unsigned char *g_ptr;
    unsigned char *b_ptr;
    unsigned char *p;
    DtInt w, h, d;
    XimPixmap tmp_pixmap;
    unsigned long image_width;
    unsigned long image_size;
    unsigned long offset;

    if (classid == -1) {
	classid = DsInqClassId("DoRaster");
    }

    if (dataindex == -1) {
	for (i=0; dde_x11_XFDI_drom_class_data[i].name != 0; i++) {
	    if (dde_x11_XFDI_drom_class_data[i].id == classid) {
		dataindex = dde_x11_XFDI_drom_class_data[i].index;
		break;
	    }
	}
    }

    rstr = (struct rstr *)(object->data);
    dynout_rstr = (ddt_x11_XFDI_drom_rstr *)object->additional_data[dataindex];

    switch (rstr->type) {
    case DcRasterRGB:	skip = 3;	break;
    case DcRasterRGBA:	skip = 4;	break;
    case DcRasterRGBZ:	skip = 7;	break;
    case DcRasterRGBAZ:	skip = 8;	break;
    default:		return;
    }

    if ((dynout_rstr->sequence_number != rstr->sequence_number) ||
	dynout_rstr->texture_id == -1) {
	tmp_pixmap = XimCreateSharedPixmap
		(dde_x11_XFDI_drom_curdevdat->display,
		 DefaultRootWindow(dde_x11_XFDI_drom_curdevdat->display),
		 rstr->width, rstr->height * rstr->depth, 24);
					 
	image_width = tmp_pixmap->rowwidth;
	image_size = rstr->height * image_width;
	r_ptr = ((unsigned char *)rstr->data);
	g_ptr = ((unsigned char *)rstr->data) + 1;
	b_ptr = ((unsigned char *)rstr->data) + 2;

	p = (unsigned char *)tmp_pixmap->address;

	for (d=0; d<rstr->depth; d++) {
	    h = rstr->height;
	    while (h--) {
		offset = d*image_size + h*image_width;
		w = rstr->width;
		while (w--) {
		    p[offset+1] = *r_ptr;
		    p[offset+2] = *g_ptr;
		    p[offset+3] = *b_ptr;
		    offset += 4;
		    r_ptr += skip;
		    g_ptr += skip;
		    b_ptr += skip;
		}
	    }
	}

	if (dynout_rstr->texture_id != -1) {
	    XFDIDestroyTexture(dde_x11_XFDI_drom_curdevdat->display,
			       dynout_rstr->texture_id);
	}
	if (rstr->depth > 1) {
	    dynout_rstr->texture_id = 
		    XFDICreate3DTexture(dde_x11_XFDI_drom_curdevdat->display,
					tmp_pixmap->pid, 
					rstr->width, rstr->height, rstr->depth, 5);
	} else {
	    dynout_rstr->texture_id = 
		    XFDICreateTexture(dde_x11_XFDI_drom_curdevdat->display,
				      tmp_pixmap->pid, XFDITexturePixmap);
	}
	XimFreeSharedPixmap(dde_x11_XFDI_drom_curdevdat->display,
			    tmp_pixmap);

	dynout_rstr->sequence_number = rstr->sequence_number;
    }
}
