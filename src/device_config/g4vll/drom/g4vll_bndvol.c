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
	int  ddr_g4vll_drom_bndvol_execute (object)

  ======================================================================
 */
#include "drom.h"

typedef struct {
    DtVolume volume;
    DtObject alternate_object;
} ddt_g4vll_drom_bndvol_data;

/*
 ======================================================================
 */

static DtInt classid = -1;

/*
 ======================================================================
 */
void ddr_g4vll_drom_bndvol_execute (
    dot_object *object)
{
    static DtMethodPtr std_bndvol_routine;
    static DtMethodPtr bndvolswi_query;
    DtRepType    cur_reptype;
    DtSwitch     cur_bfcull_switch;
    unsigned	 cur_draw_type;
    unsigned	 cur_z_enable;
    unsigned	 cur_z_function;
    unsigned	 cur_stencil_plane_mask;
    DtFlag bndvol_switch;
    DtReal box[36][3];
    ddt_g4vll_drom_bndvol_data *bndvol_data;
    DtReal x0, y0, z0,  x1, y1, z1;
    unsigned int occluded;

    if (classid == -1) {
	classid = DsInqClassId("DoBoundingVol");
    }

    if(!DDcondex_QueryRender(classid)) {
	return;
    }

    if (!bndvolswi_query) {
	bndvolswi_query = DDclass_InqMethod(
			DsInqClassId("AttBoundingVolSwitch"),
			DsInqMethodId("InqGlobalValue"));
    }
    (*bndvolswi_query)(&bndvol_switch);

    if (bndvol_switch == DcOff) {
	return;
    }

    if (ddr_g4vll_dcm_inquire_hw_boundingvol()) {
	/*
	 * Use the Denali hardware to determine an object's degree
	 * of occlusion.  Construct a box using the bounding volume
	 * and render it to the frame buffer.
	 */
	
	bndvol_data = (ddt_g4vll_drom_bndvol_data *)object->data;
	x0 = bndvol_data->volume.bll[0];
	y0 = bndvol_data->volume.bll[1];
	z0 = bndvol_data->volume.bll[2];
	x1 = bndvol_data->volume.fur[0];
	y1 = bndvol_data->volume.fur[1];
	z1 = bndvol_data->volume.fur[2];

	box[0][0]  = x0;  box[0][1]  = y0;  box[0][2]  = z0;
	box[1][0]  = x0;  box[1][1]  = y1;  box[1][2]  = z0;
	box[2][0]  = x1;  box[2][1]  = y1;  box[2][2]  = z0;
	box[3][0]  = x0;  box[3][1]  = y0;  box[3][2]  = z0;
	box[4][0]  = x1;  box[4][1]  = y1;  box[4][2]  = z0;
	box[5][0]  = x1;  box[5][1]  = y0;  box[5][2]  = z0;
	box[6][0]  = x0;  box[6][1]  = y0;  box[6][2]  = z1;
	box[7][0]  = x1;  box[7][1]  = y0;  box[7][2]  = z1;
	box[8][0]  = x1;  box[8][1]  = y1;  box[8][2]  = z1;
	box[9][0]  = x0;  box[9][1]  = y0;  box[9][2]  = z1;
	box[10][0] = x1;  box[10][1] = y1;  box[10][2] = z1;
	box[11][0] = x0;  box[11][1] = y1;  box[11][2] = z1;
	box[12][0] = x0;  box[12][1] = y1;  box[12][2] = z0;
	box[13][0] = x0;  box[13][1] = y1;  box[13][2] = z1;
	box[14][0] = x1;  box[14][1] = y1;  box[14][2] = z1;
	box[15][0] = x0;  box[15][1] = y1;  box[15][2] = z0;
	box[16][0] = x1;  box[16][1] = y1;  box[16][2] = z1;
	box[17][0] = x1;  box[17][1] = y1;  box[17][2] = z0;
	box[18][0] = x0;  box[18][1] = y0;  box[18][2] = z0;
	box[19][0] = x1;  box[19][1] = y0;  box[19][2] = z0;
	box[20][0] = x1;  box[20][1] = y0;  box[20][2] = z1;
	box[21][0] = x0;  box[21][1] = y0;  box[21][2] = z0;
	box[22][0] = x1;  box[22][1] = y0;  box[22][2] = z1;
	box[23][0] = x0;  box[23][1] = y0;  box[23][2] = z1;
	box[24][0] = x0;  box[24][1] = y0;  box[24][2] = z0;
	box[25][0] = x0;  box[25][1] = y0;  box[25][2] = z1;
	box[26][0] = x0;  box[26][1] = y1;  box[26][2] = z1;
	box[27][0] = x0;  box[27][1] = y0;  box[27][2] = z0;
	box[28][0] = x0;  box[28][1] = y1;  box[28][2] = z1;
	box[29][0] = x0;  box[29][1] = y1;  box[29][2] = z0;
	box[30][0] = x1;  box[30][1] = y0;  box[30][2] = z0;
	box[31][0] = x1;  box[31][1] = y1;  box[31][2] = z0;
	box[32][0] = x1;  box[32][1] = y1;  box[32][2] = z1;
	box[33][0] = x1;  box[33][1] = y0;  box[33][2] = z0;
	box[34][0] = x1;  box[34][1] = y1;  box[34][2] = z1;
	box[35][0] = x1;  box[35][1] = y0;  box[35][2] = z1;

	/*
	 * We need to establish the following conditions before doing
	 * the Zquery.  The original state is restored afterwards.
	 */
	cur_reptype = dde_g4vll_drom_data.reptype;
	ddr_g4vll_drom_set_att_reptyp(DcSurface);

	cur_bfcull_switch = dde_g4vll_drom_data.backface_cull_switch;
	ddr_g4vll_drom_set_att_bacfacculswi(DcOn);

	cur_draw_type = VLLquery_draw_type();
	VLLset_draw_type(0x0);

	cur_z_enable = VLLquery_z_enable();
	VLLset_z_enable(VLL_DISABLE);

	cur_z_function = VLLquery_z_function();
	VLLset_z_function(VLL_Z_LT);

	cur_stencil_plane_mask = VLLquery_stencil_plane_mask();
	VLLset_stencil_plane_mask(VLL_SCMP_TRUE);

	VLLset_query(VLL_QUERY_Z);

	VLLtriangle_list(36, (void *)box, (void *)0, (void *)0, (void *)0,
		(void *)0, (void *)0, (void *)0, (void *)0, (void *)0);

	occluded = VLLquery_query();

	ddr_g4vll_drom_set_att_reptyp(cur_reptype);
	ddr_g4vll_drom_set_att_bacfacculswi(cur_bfcull_switch);
	VLLset_draw_type(cur_draw_type);
	VLLset_z_enable(cur_z_enable);
	VLLset_z_function(cur_z_function);
	VLLset_stencil_plane_mask(cur_stencil_plane_mask);

	if (occluded) {
	    /* 
	     * The object's bounding box is fully occluded so abort traversal
	     * of the current group.
	     */
	    DsExecutionReturn();
	} else {
	    /*
             * The object is not fully occluded, so use the kernel's 
	     * standard routine to determine what happens next as far as
	     * rendering goes.
             */
	    if (!std_bndvol_routine) {
            	std_bndvol_routine = DDobject_InqMethod(object, 
				DcMethodStdRenderDisplay);
	    }
            (*std_bndvol_routine)(object);
	}
    } else {
	/*
	 * Use the kernel's standard routine.
	 */
	std_bndvol_routine = DDobject_InqMethod(object, 
			DcMethodStdRenderDisplay);

	if (std_bndvol_routine) {
	    (*std_bndvol_routine)(object);
	}
    }
}
/*
 ======================================================================
 */
