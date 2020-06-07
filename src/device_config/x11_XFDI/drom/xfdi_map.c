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
	ddr_x11_XFDI_drom_set_att_mapbmp ddr_x11_XFDI_drom_set_att_mapbmp (count, mapping, raster, attributes)
	ddr_x11_XFDI_drom_set_att_mapbmpswi ddr_x11_XFDI_drom_set_att_mapbmpswi (switchvalue)
	ddr_x11_XFDI_drom_set_att_mapdifclr ddr_x11_XFDI_drom_set_att_mapdifclr (count, mapping, raster, attributes)
	ddr_x11_XFDI_drom_set_att_mapdifclrswi ddr_x11_XFDI_drom_set_att_mapdifclrswi (switchvalue)
	ddr_x11_XFDI_drom_set_att_mapenv ddr_x11_XFDI_drom_set_att_mapenv (count, mapping, raster, attributes)
	ddr_x11_XFDI_drom_set_att_mapenvswi ddr_x11_XFDI_drom_set_att_mapenvswi (switchvalue)
	ddr_x11_XFDI_drom_set_att_maptrnint ddr_x11_XFDI_drom_set_att_maptrnint (count, mapping, raster, attributes)
	ddr_x11_XFDI_drom_set_att_maptrnintswi ddr_x11_XFDI_drom_set_att_maptrnintswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/stdtexatt.h"
#include "dore/dore_develop/private/rstr.h"
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_mapbmp (count, mapping, raster, attributes)
     DtInt count;
     DtObject mapping[];
     DtObject raster[];
     dot_stdtexatt attributes[];
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_mapbmpswi (switchvalue)
     DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_mapdifclr (count, mapping, raster, attributes)
     DtInt count;
     DtObject mapping[];
     DtObjectStructure *raster[];
     dot_stdtexatt attributes[];
{
    static DtInt rstr_classid = -1;
    static DtInt filerstr_classid = -1;
    static DtInt rstr_dataindex = -1;
    static DtInt filerstr_dataindex = -1;
    DtInt rasterid;
    ddt_x11_XFDI_drom_rstr *drom_rstr;
    DtInt i;
    DtObjectStructure *raster_obj = (DtObjectStructure *)0;
    struct rstr *rstr;

    if (dde_x11_XFDI_drom_pre_init) return;

    dde_x11_XFDI_drom_data.map_diffuse_color_uv_index = attributes[0].uv.idx;
    dde_x11_XFDI_drom_data.map_diffuse_color_uvw_index = attributes[0].uvw.idx;

    dde_x11_XFDI_drom_data.texture_3D_valid = DcFalse;
    dde_x11_XFDI_drom_data.texture_2D_valid = DcFalse;

    /*
     * XFDI currently does not support texture mapping in PseudoColor
     * so no need to do any work on the rasters if in PseudoColor
     */
    if (dde_x11_XFDI_drom_curdevdat->visual_type == DcPseudoColor)
	    return;

    if (count > 0) {
	if (raster[0] != (DtObjectStructure *)0) {
	    rasterid = DsInqObjClassId(raster[0]);

	    if (filerstr_classid == -1) filerstr_classid = DsInqClassId("DoFileRaster");
	    if (rasterid == filerstr_classid) {
		if (filerstr_dataindex == -1) {
		    for (i=0; dde_x11_XFDI_drom_class_data[i].name != 0; i++) {
			if (dde_x11_XFDI_drom_class_data[i].id == filerstr_classid) {
			    filerstr_dataindex = dde_x11_XFDI_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_x11_XFDI_drom_filerstr_update (raster[0]);

		raster_obj = (DtObjectStructure *)raster[0]->additional_data[filerstr_dataindex];
	    }
	    

	    if (rstr_classid == -1) rstr_classid = DsInqClassId("DoRaster");

	    if (rasterid == rstr_classid) {
		raster_obj = raster[0];
	    }

	    if (raster_obj != (DtObjectStructure *)0) {
		if (rstr_dataindex == -1) {
		    for (i=0; dde_x11_XFDI_drom_class_data[i].name != 0; i++) {
			if (dde_x11_XFDI_drom_class_data[i].id == rstr_classid) {
			    rstr_dataindex = dde_x11_XFDI_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_x11_XFDI_drom_rstr_update (raster_obj);

		rstr = (struct rstr *)(raster_obj->data);
		drom_rstr = 
			(ddt_x11_XFDI_drom_rstr *)raster_obj->additional_data[rstr_dataindex];

		if (drom_rstr->texture_id != -1) {
		    XFDISetSurfaceTexture(dde_x11_XFDI_drom_curdevdat->display,
					  dde_x11_XFDI_drom_curdevdat->gc,
					  drom_rstr->texture_id);
	       
		    if (mapping[0] == DcStdTableLookup && rstr->depth == 1) {
			dde_x11_XFDI_drom_data.texture_2D_valid = DcTrue;
		    } else if (mapping[0] == DcStd3dTableLookup && rstr->depth > 1) {
			dde_x11_XFDI_drom_data.texture_3D_valid = DcTrue;
		    }
		}
	    }
	}
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_mapdifclrswi (switchvalue)
     DtSwitch switchvalue;
{
    dde_x11_XFDI_drom_data.map_diffuse_color = switchvalue;

    if (dde_x11_XFDI_drom_pre_init) return;

    if (dde_x11_XFDI_drom_data.texture_pass) {
	if (!dde_x11_XFDI_drom_data.map_diffuse_color) {
	    DDcondex_SetDisablePrimExecFlag(DcTrue);
	} else {
	    DDcondex_SetDisablePrimExecFlag(DcFalse);
	}
    }
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_mapenv (count, mapping, raster, attributes)
     DtInt count;
     DtObject mapping[];
     DtObject raster[];
     dot_stdtexatt attributes[];
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_mapenvswi (switchvalue)
     DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_maptrnint (count, mapping, raster, attributes)
     DtInt count;
     DtObject mapping[];
     DtObject raster[];
     dot_stdtexatt attributes[];
{
}
/*
 ======================================================================
 */
ddr_x11_XFDI_drom_set_att_maptrnintswi (switchvalue)
     DtSwitch switchvalue;
{
}
