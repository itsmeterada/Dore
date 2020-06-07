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
	void ddr_vll_drom_set_att_mapbmp (count, mapping, raster, attributes)
	void ddr_vll_drom_set_att_mapbmpswi (switchvalue)
	void ddr_vll_drom_set_att_mapdifclr (count, mapping, raster, attributes)
	void ddr_vll_drom_set_att_mapdifclrswi (switchvalue)
	void ddr_vll_drom_set_att_mapenv (count, mapping, raster, attributes)
	void ddr_vll_drom_set_att_mapenvswi (switchvalue)
	void ddr_vll_drom_set_att_maptrnint (count, mapping, raster, attributes)
	void ddr_vll_drom_set_att_maptrnintswi (switchvalue)

  ======================================================================
 */
#include "drom.h"
#include "dore/dore_develop/private/stdtexatt.h"
#include "dore/dore_develop/private/rstr.h"

/*
 ======================================================================
 */
void
ddr_vll_drom_set_att_mapbmp (count, mapping, raster, attributes)
	DtInt count;
	DtObject mapping[];
	DtObject raster[];
	dot_stdtexatt attributes[];
{
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_mapbmpswi (switchvalue)
	DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_mapdifclr (count, mapping, raster, attributes)
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
    ddt_vll_drom_rstr *dynout_rstr;
    DtInt i;
    DtObjectStructure *raster_obj = (DtObjectStructure *)0;
    struct rstr *rstr;
    float matrix[4][4];
    int umode, vmode, wmode;
    int aamode;
    int i1, i2, j1, j2;

    dde_vll_drom_data.texture_3D_valid = DcFalse;
    dde_vll_drom_data.texture_2D_valid = DcFalse;
    dde_vll_drom_data.map_extend_black = DcFalse;

    if (dde_vll_drom_pre_init) return;

    /* return if device does not support texture mapping */
    if (!dde_vll_drom_curdevdat->features->uv_texture_mapping &&
	!dde_vll_drom_curdevdat->features->uvw_texture_mapping) return;

    if (count > 0) {
	dde_vll_drom_data.map_diffuse_color_uv_index = attributes[0].uv.idx;
	dde_vll_drom_data.map_diffuse_color_uvw_index = attributes[0].uvw.idx;

	if (raster[0] != (DtObjectStructure *)0) {
	    rasterid = DsInqObjClassId(raster[0]);

	    if (filerstr_classid == -1) filerstr_classid = DsInqClassId("DoFileRaster");
	    if (rasterid == filerstr_classid) {
		if (filerstr_dataindex == -1) {
		    for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
			if (dde_vll_drom_class_data[i].id == filerstr_classid) {
			    filerstr_dataindex = dde_vll_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_vll_drom_filerstr_update (raster[0]);

		raster_obj = (DtObjectStructure *)raster[0]->additional_data[filerstr_dataindex];
	    }
	    

	    if (rstr_classid == -1) rstr_classid = DsInqClassId("DoRaster");

	    if (rasterid == rstr_classid) {
		raster_obj = raster[0];
	    }

	    if (raster_obj != (DtObjectStructure *)0) {
		if (rstr_dataindex == -1) {
		    for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
			if (dde_vll_drom_class_data[i].id == rstr_classid) {
			    rstr_dataindex = dde_vll_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_vll_drom_rstr_update (raster_obj);

		rstr = (struct rstr *)(raster_obj->data);
		dynout_rstr = 
			(ddt_vll_drom_rstr *)raster_obj->additional_data[rstr_dataindex];

		if (dynout_rstr->texture != (VLLtexture *)0) {

		    VLLset_surface_texture(dynout_rstr->texture);
		    
#ifndef VLL_4_0
		    /*
		     * Set AntiAlias mode for textures
		     */

		    switch(attributes[0].aamode) {
		    case DcTextureAAMIPPoint:
			aamode = VLL_MIP_MAP; break;
		    case DcTextureAASumArea:
			aamode = VLL_SUMMED_AREA; break;
		    case DcTextureAABilinear:
			aamode = VLL_BI_LINEAR; break;
		    default:
			aamode = VLL_POINT_SAMPLE;
		    }
		    VLLset_texture_filter_mode(aamode);
#endif

		    if (mapping[0] == DcStdTableLookup && rstr->depth == 1) {
			dde_vll_drom_data.texture_2D_valid = DcTrue;

#ifndef VLL_4_0
			/*
			 * Set the extend mode 
			 */

			switch (attributes[0].uv.extend[0]) {
			case DcExtendNone:   umode = VLL_EXTEND_NONE;   break;
			case DcExtendBlack:  umode = VLL_EXTEND_NONE;   break;
			case DcExtendClamp:  umode = VLL_EXTEND_CLAMP;  break;
			case DcExtendRepeat: umode = VLL_EXTEND_REPEAT; break;
			default:	     umode = VLL_EXTEND_SPECIAL;
			}
			switch (attributes[0].uv.extend[1]) {
			case DcExtendNone:   vmode = VLL_EXTEND_NONE;   break;
			case DcExtendBlack:  vmode = VLL_EXTEND_NONE;   break;
			case DcExtendClamp:  vmode = VLL_EXTEND_CLAMP;  break;
			case DcExtendRepeat: vmode = VLL_EXTEND_REPEAT; break;
			default:	     vmode = VLL_EXTEND_SPECIAL;
			}
			VLLset_texture_extend(umode, vmode, VLL_EXTEND_SPECIAL);

			/*
			 * extend none has precedence over black so only extend
			 * black if not none.
			 */

			if ((attributes[0].uv.extend[0] == DcExtendBlack ||
			     attributes[0].uv.extend[1] == DcExtendBlack) &&
			    attributes[0].uv.extend[0] != DcExtendNone &&
			    attributes[0].uv.extend[1] != DcExtendNone) {
			    dde_vll_drom_data.map_extend_black = DcTrue;
			} else {
			    dde_vll_drom_data.map_extend_black = DcFalse;
			}

			/*
			 * Only allow operator of Multiply or Replace
			 * if not one of these then set to Replace.
			 */

			if (attributes[0].operator != DcTextureMultiply) {
			    dde_vll_drom_data.map_operator = DcTextureReplace;
			} else {
			    dde_vll_drom_data.map_operator = DcTextureMultiply;
			}

			/*
			 * Set the Texture Matrix
			 */
			for (i1=0, i2=0; i1<4; i1++, i2++) {
			    if (i1==3) i2=2;
			    for(j1=0, j2=0; j1<4; j1++, j2++) { 
				if (j1==3) j2=2;

				if (i1==2 && j1==2) {
				    matrix[i1][j1] = 1.0;
				} else if (i1==2 || j1==2) {
				    matrix[i1][j1] = 0.0;
				} else {
				    matrix[i1][j1] = attributes[0].uv.matrix[i2][j2];
				}
			    }
			}
			
			VLLset_matrix(VLL_TEXTURE_MATRIX, matrix);
#endif
		    } else if (mapping[0] == DcStd3dTableLookup && 
				rstr->depth > 1) {
			dde_vll_drom_data.texture_3D_valid = DcTrue;
			/*
			 * Set the extend mode 
			 */

			switch (attributes[0].uvw.extend[0]) {
			case DcExtendNone:   umode = VLL_EXTEND_NONE;   break;
			case DcExtendBlack:  umode = VLL_EXTEND_NONE;  break;
			case DcExtendClamp:  umode = VLL_EXTEND_CLAMP;  break;
			case DcExtendRepeat: umode = VLL_EXTEND_CLAMP;  break;
			default:	     umode = VLL_EXTEND_SPECIAL;
			}
			switch (attributes[0].uvw.extend[1]) {
			case DcExtendNone:   vmode = VLL_EXTEND_NONE;   break;
			case DcExtendBlack:  vmode = VLL_EXTEND_NONE;  break;
			case DcExtendClamp:  vmode = VLL_EXTEND_CLAMP;  break;
			case DcExtendRepeat: vmode = VLL_EXTEND_CLAMP;  break;
			default:	     vmode = VLL_EXTEND_SPECIAL;
			}
			switch (attributes[0].uvw.extend[2]) {
			case DcExtendNone:   wmode = VLL_EXTEND_NONE;   break;
			case DcExtendBlack:  wmode = VLL_EXTEND_NONE;  break;
			case DcExtendClamp:  wmode = VLL_EXTEND_CLAMP;  break;
			case DcExtendRepeat: wmode = VLL_EXTEND_CLAMP;  break;
			default:	     wmode = VLL_EXTEND_SPECIAL;
			}
			VLLset_texture_extend(umode, vmode, wmode);

			/*
			 * ExtendNone has precedence over Black, so extend
			 * Black only if all of u,v,w are not None.
			 */

			if ((attributes[0].uvw.extend[0] == DcExtendBlack ||
			     attributes[0].uvw.extend[1] == DcExtendBlack ||
			     attributes[0].uvw.extend[2] == DcExtendBlack) &&
			    attributes[0].uvw.extend[0] != DcExtendNone &&
			    attributes[0].uvw.extend[1] != DcExtendNone &&
			    attributes[0].uvw.extend[2] != DcExtendNone) {
			    dde_vll_drom_data.map_extend_black = DcTrue;
			} else {
			    dde_vll_drom_data.map_extend_black = DcFalse;
			}

			/*
			 * Only allow operator of Multiply or Replace.
			 * If not one of these then set to Replace.
			 */

			if (attributes[0].operator != DcTextureMultiply) {
			    dde_vll_drom_data.map_operator = DcTextureReplace;
			} else {
			    dde_vll_drom_data.map_operator = DcTextureMultiply;
			}

			/*
			 * Set the Texture Matrix (vll expects floats ...)
			 */
       			if (sizeof(float) != sizeof(DtReal)) {
            		    for (i1=0; i1<4; i1++) {
                	        for (j1=0; j1<4; j1++) {
                    		    matrix[i1][j1] = 
					attributes[0].uvw.matrix[i1][j1];
                		}
            		    }
			    VLLset_matrix(VLL_TEXTURE_MATRIX, matrix);
        		} else {
			    VLLset_matrix(VLL_TEXTURE_MATRIX, 
					attributes[0].uvw.matrix);
        		}
		    }
		}
	    }
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_mapdifclrswi (switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.map_diffuse_color = switchvalue;

    if (dde_vll_drom_pre_init) return;

    ddr_vll_drom_set_prim_execute();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_mapenv (count, mapping, raster, attributes)
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
    ddt_vll_drom_rstr *dynout_rstr;
    DtInt i;
    DtObjectStructure *raster_obj = (DtObjectStructure *)0;
    struct rstr *rstr;

    dde_vll_drom_data.texture_env_valid = DcFalse;

    if (dde_vll_drom_pre_init) return;

    if (!dde_vll_drom_curdevdat->features->env_texture_mapping) return;

    if (count > 0) {
	if (raster[0] != (DtObjectStructure *)0) {
	    rasterid = DsInqObjClassId(raster[0]);

	    if (filerstr_classid == -1) filerstr_classid = DsInqClassId("DoFileRaster");
	    if (rasterid == filerstr_classid) {
		if (filerstr_dataindex == -1) {
		    for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
			if (dde_vll_drom_class_data[i].id == filerstr_classid) {
			    filerstr_dataindex = dde_vll_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_vll_drom_filerstr_update (raster[0]);

		raster_obj = (DtObjectStructure *)raster[0]->additional_data[filerstr_dataindex];
	    }
	    

	    if (rstr_classid == -1) rstr_classid = DsInqClassId("DoRaster");

	    if (rasterid == rstr_classid) {
		raster_obj = raster[0];
	    }

	    if (raster_obj != (DtObjectStructure *)0) {
		if (rstr_dataindex == -1) {
		    for (i=0; dde_vll_drom_class_data[i].name != 0; i++) {
			if (dde_vll_drom_class_data[i].id == rstr_classid) {
			    rstr_dataindex = dde_vll_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_vll_drom_rstr_update_env (raster_obj);

		rstr = (struct rstr *)(raster_obj->data);
		dynout_rstr = 
			(ddt_vll_drom_rstr *)raster_obj->additional_data[rstr_dataindex];

		if (dynout_rstr->env_texture != (VLLtexture *)0) {

		    if (mapping[0] == DcStdCubeEnvironMap && rstr->depth == 6) {
			VLLset_surface_texture(dynout_rstr->env_texture);
	       
			dde_vll_drom_data.texture_env_valid = DcTrue;
		    }
		}
	    }
	}
    }
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_mapenvswi (switchvalue)
	DtSwitch switchvalue;
{
    dde_vll_drom_data.map_env = switchvalue;

    if (dde_vll_drom_pre_init) return;

    ddr_vll_drom_set_prim_execute();
}
/*
 ======================================================================
 */
void 
ddr_vll_drom_set_att_maptrnint (count, mapping, raster, attributes)
	DtInt count;
	DtObject mapping[];
	DtObject raster[];
	dot_stdtexatt attributes[];
{
}
/*
 ======================================================================
 */
void
ddr_vll_drom_set_att_maptrnintswi (switchvalue)
	DtSwitch switchvalue;
{
}
/*
 ======================================================================
 */
