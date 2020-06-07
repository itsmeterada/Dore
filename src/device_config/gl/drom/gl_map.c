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
	ddr_gl_drom_set_att_mapbmp
	ddr_gl_drom_set_att_mapbmpswi
	ddr_gl_drom_set_att_mapdifclr
	ddr_gl_drom_set_att_mapdifclrswi
	ddr_gl_drom_set_att_mapenv
	ddr_gl_drom_set_att_mapenvswi
	ddr_gl_drom_set_att_maptrnint
	ddr_gl_drom_set_att_maptrnintswi

  ======================================================================
 */
#include "../gl_driver.h"
#include <dore/dore_develop/private/stdtexatt.h>
#include <dore/dore_develop/private/rstr.h>



/* ====================================================================== */

void ddr_gl_drom_set_att_mapbmp (
    DtInt count,
    DtObject mapping[],
    DtObject raster[],
    dot_stdtexatt attributes[])
{
}

/* ======================================================================== */

void ddr_gl_drom_set_att_mapbmpswi (DtSwitch switchvalue)
{
}

/* ======================================================================== */

void ddr_gl_drom_set_att_mapdifclr (
    DtInt count,
    DtObject mapping[],
    DtObjectStructure *raster[],
    dot_stdtexatt attributes[])
{
    static DtInt rstr_classid = -1;
    static DtInt filerstr_classid = -1;
    static DtInt rstr_dataindex = -1;
    static DtInt filerstr_dataindex = -1;
    DtInt rasterid;
    ddt_gl_drom_rstr *dynout_rstr;
    DtObjectStructure *raster_obj = (DtObjectStructure *)0;
    struct rstr *rstr;
    Matrix im;
    DtInt i;
    DtInt i1, i2, j1, j2;
    float props[40];
    int prop;

    DevStatus.texture_2D = 0;

    if (getgdesc(GD_TEXTURE) == 0) 
	return;

    if (count > 0) {
	DevStatus.texture_uv_index = attributes[0].uv.idx;

	if (raster[0] != (DtObjectStructure *)0) {
	    rasterid = DsInqObjClassId(raster[0]);

	    if (filerstr_classid == -1)
		filerstr_classid = DsInqClassId("DoFileRaster");

	    if (rasterid == filerstr_classid) {
		if (filerstr_dataindex == -1) {
		    for (i=0; dde_gl_drom_class_data[i].name != 0; i++) {
			if (dde_gl_drom_class_data[i].id == filerstr_classid) {
			    filerstr_dataindex = dde_gl_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_gl_drom_filerstr_update (raster[0]);

		raster_obj = (DtObjectStructure *)
			     raster[0]->additional_data[filerstr_dataindex];
	    }
	    

	    if (rstr_classid == -1) rstr_classid = DsInqClassId("DoRaster");

	    if (rasterid == rstr_classid) {
		raster_obj = raster[0];
	    }

	    if (raster_obj != (DtObjectStructure *)0) {
		if (rstr_dataindex == -1) {
		    for (i=0; dde_gl_drom_class_data[i].name != 0; i++) {
			if (dde_gl_drom_class_data[i].id == rstr_classid) {
			    rstr_dataindex = dde_gl_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_gl_drom_rstr_tex_update (raster_obj);

		rstr = (struct rstr *)(raster_obj->data);
		dynout_rstr = (ddt_gl_drom_rstr *)
			       raster_obj->additional_data[rstr_dataindex];

		if (  (dynout_rstr->tex.sequence_number !=rstr->sequence_number)
		   || (dynout_rstr->tex.aa_mode != attributes[0].aamode)
		   || (dynout_rstr->tex.extenduv[0]!=attributes[0].uv.extend[0])
		   || (dynout_rstr->tex.extenduv[1]!=attributes[0].uv.extend[1])
		   )
	       {
		    if (dynout_rstr->tex.id == -1) {
			dynout_rstr->tex.id = ++DevStatus.next_texid;
		    }

		    prop = 0;

		    switch (attributes[0].aamode) {
			case DcTextureAAMIPPoint:
			    props[prop++] = TX_MINFILTER;
			    props[prop++] = TX_MIPMAP_BILINEAR;
			    props[prop++] = TX_MAGFILTER;
			    props[prop++] = TX_BILINEAR;
			    break;
			case DcTextureAABilinear:
			    props[prop++] = TX_MINFILTER;
			    props[prop++] = TX_BILINEAR;
			    props[prop++] = TX_MAGFILTER;
			    props[prop++] = TX_BILINEAR;
			    break;
			default:
			    props[prop++] = TX_MINFILTER;
			    props[prop++] = TX_POINT;
			    props[prop++] = TX_MAGFILTER;
			    props[prop++] = TX_POINT;
			    break;
		    }
			
		    props[prop++] = TX_WRAP_S;
		    switch (attributes[0].uv.extend[0]) {
			case DcExtendRepeat:
			    props[prop++] = TX_REPEAT;
			    break;
			case DcExtendClamp:
			default:
			    props[prop++] = TX_CLAMP;
			    break;
		    }

		    props[prop++] = TX_WRAP_T;
		    switch (attributes[0].uv.extend[1]) {
			case DcExtendRepeat:
			    props[prop++] = TX_REPEAT;
			    break;
			case DcExtendClamp:
			default:
			    props[prop++] = TX_CLAMP;
			    break;
		    }

		    props[prop++] = TX_NULL;

		    if (rstr->type == DcRasterABGR) {
			texdef2d (dynout_rstr->tex.id, 4, rstr->width,
				  rstr->height, rstr->data, prop, props);
		    } else {
			texdef2d (dynout_rstr->tex.id, 4, rstr->width,
				  rstr->height, dynout_rstr->tex.data, prop,
				  props);
		    }

		    dynout_rstr->tex.sequence_number = rstr->sequence_number;
		    dynout_rstr->tex.aa_mode = attributes[0].aamode;
		    dynout_rstr->tex.extenduv[0] = attributes[0].uv.extend[0];
		    dynout_rstr->tex.extenduv[1] = attributes[0].uv.extend[1];
		}

		DevStatus.texture_2D = dynout_rstr->tex.id;
		DevStatus.texture_op = attributes[0].operator;

		/* Set the Texture Matrix */

		ddr_iris_id_matrix (im);

		for (i1=0, i2=0; i1<4; i1++, i2++) {
		    if (i1==3) i2=2;
		    for(j1=0, j2=0; j1<4; j1++, j2++) { 
			if (j1==3) j2=2;

			if (i1==2 && j1==2) {
			    im[i1][j1] = 1.0;
			} else if (i1==2 || j1==2) {
			    im[i1][j1] = 0.0;
			} else {
			    im[i1][j1] = attributes[0].uv.matrix[j2][i2];
			}
		    }
		}

		mmode (MTEXTURE);
		loadmatrix (im);
		mmode (MVIEWING);

		prop = 0;
		props[prop++] = TV_MODULATE;
		props[prop++] = TV_NULL;
		tevdef(1, prop, props);
		tevbind(TV_ENV0, 1);
	    }
	}
    }
}



/* ======================================================================== */

void ddr_gl_drom_set_att_mapdifclrswi (
    DtSwitch switchvalue)
{
    DevStatus.map_diffuse_color_switch = switchvalue;
}



/* ======================================================================== */

void ddr_gl_drom_set_att_mapenv (
    DtInt count,
    DtObject mapping[],
    DtObject raster[],
    dot_stdtexatt attributes[])
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_mapenvswi (DtSwitch switchvalue)
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_maptrnint (
    DtInt count,
    DtObject mapping[],
    DtObject raster[],
    dot_stdtexatt attributes[])
{
}



/* ======================================================================== */

void ddr_gl_drom_set_att_maptrnintswi (DtSwitch switchvalue)
{
}
