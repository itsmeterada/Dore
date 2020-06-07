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
	void ddr_sunview_cxp_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
	void ddr_sunview_cxp_dcm_clear_rectangle_depth_and_color (xmin,ymin,xmax,ymax)
	void ddr_sunview_cxp_dcm_write_scanline_byte (xstart, ystart, length, data)
	int ddr_sunview_cxp_dcm_blt_front_to_back ()

  ======================================================================
 */
#include "dcm.h"
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_clear_rectangle_depth (xmin, ymin, xmax, ymax)
     DtShort xmin;
     DtShort ymin;
     DtShort xmax;
     DtShort ymax;
{
}

			     
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_clear_rectangle_depth_and_color (xmin,ymin,xmax,ymax)
     DtShort xmin;
     DtShort xmax;
     DtShort ymin;
     DtShort ymax;
{
    GP_clear_Z_and_color (dde_sunview_cxp_current_dcm_data->gp, 
			  dde_sunview_cxp_current_dcm_data->bg_color_index,
			  xmin, ymin, xmax, ymax);

#ifdef PRINTOUT
    fprintf (stderr, "Called: ddr_sunview_cxp_dcm_clear_rectangle_depth_and_color\n");
#endif
}
/*
 ======================================================================
 */

void
ddr_sunview_cxp_dcm_write_scanline_byte (xstart, ystart, length, data)
     DtInt xstart;
     DtInt ystart;
     DtInt length;
     DtUChar *data;
{
    static struct pixrect *pr = NULL;
    static int ddr_l = 0;
    DtInt windowheight;
    u_char *loc;
    DtInt index;
    int i;

    windowheight = dde_sunview_cxp_current_dcm_data->height + 1;

    ystart = windowheight - ystart;

    if (ddr_l == 0 || ddr_l < length) {
	if (pr != NULL) pr_destroy (pr);
	pr = mem_create (length, 1, 8);
	ddr_l = length;
    }

    ddr_sunview_cxp_dcm_setup_shading(dde_sunview_cxp_current_dcm_data);

    if (dde_sunview_cxp_current_dcm_data->shade_mode == DcComponent) {
	for (i=0; i<length; i++) {
	    loc = mprd8_addr(mpr_d(pr), i, 0, 8);

	    index = ((int)(dde_sunview_cxp_current_dcm_data->cbtselpsweight[0]*data[i*3+0]) 
		     & dde_sunview_cxp_current_dcm_data->cbtselpsmask[0]) + 
			     ((int)(dde_sunview_cxp_current_dcm_data->cbtselpsweight[1]*data[i*3+1]) 
			      & dde_sunview_cxp_current_dcm_data->cbtselpsmask[1]) +
				      ((int)(dde_sunview_cxp_current_dcm_data->cbtselpsweight[2]*data[i*3+2])
				       & dde_sunview_cxp_current_dcm_data->cbtselpsmask[2]);
	    index = index < 0 ? 0 : index > 255 ? 255 : index;
	    *loc = (u_char)index;
	}
    } else if (dde_sunview_cxp_current_dcm_data->shade_mode == DcRange) {
	for (i=0; i<length; i++) {
	    loc = mprd8_addr(mpr_d(pr), i, 0, 8);

	    index = dde_sunview_cxp_current_dcm_data->cwtavgpslim[0] +
		    (dde_sunview_cxp_current_dcm_data->cwtavgpsweight[0]*data[i*3+0] +
		     dde_sunview_cxp_current_dcm_data->cwtavgpsweight[1]*data[i*3+1] +
		     dde_sunview_cxp_current_dcm_data->cwtavgpsweight[2]*data[i*3+2])*
			     dde_sunview_cxp_current_dcm_data->cwtavgpslim[2];
	    if (index > dde_sunview_cxp_current_dcm_data->cwtavgpslim[1]) 
		    index = dde_sunview_cxp_current_dcm_data->cwtavgpslim[1];
	    index = index < 0 ? 0 : index > 255 ? 255 : index;
	    *loc = (u_char)index;
	}
    }

    pw_rop (dde_sunview_cxp_current_dcm_data->pw, 
	    xstart, ystart, length, 1, PIX_SRC, pr, 0, 0);

}
/*
 ======================================================================
 */

ddr_sunview_cxp_dcm_blt_front_to_back()
{
    short w, h;
    short xfront, yfront;
    short xback, yback;
    int i;
    DtReal *p;


    pw_dbl_set (dde_sunview_cxp_current_dcm_data->pw, 
		PW_DBL_READ, PW_DBL_FORE, 0);
#ifdef DEBUG
    printf ("rectangles = %d\n", dde_sunview_cxp_current_dcm_data->nrects);
    printf ("DB_write = %s\nDB_read = %s\n",
	    ((i = pw_dbl_get(dde_sunview_cxp_current_dcm_data->pw, 
			     PW_DBL_WRITE)) == PW_DBL_FORE) ? "FORE" :
	    (i == PW_DBL_BACK) ? "BACK" : (i == PW_DBL_BOTH) ? "BOTH" : 
	    "ERROR",
	    ((i = pw_dbl_get(dde_sunview_cxp_current_dcm_data->pw, 
			     PW_DBL_READ)) == PW_DBL_FORE) ? "FORE" :
	    (i == PW_DBL_BACK) ? "BACK" : (i == PW_DBL_BOTH) ? "BOTH" : 
	    "ERROR" );
#endif

    for (i=0, p = dde_sunview_cxp_current_dcm_data->rects; 
	 i<dde_sunview_cxp_current_dcm_data->nrects;
	 i++, p += 4) {
	w = (short)(p[2] - p[0] + 1);
	h = (short)(p[3] - p[1] + 1);
	xfront = (short)p[0];
	yfront = (short)p[1];
	xback = (short)p[0];
	yback = (short)p[1];
#ifdef DEBUG
	printf ("(%d %d) (%d %d) -> (%d %d)\n", xfront, yfront,
		w, h, xback, yback);
#endif
	GP_rop_ff(dde_sunview_cxp_current_dcm_data->gp,
		  xfront, yfront, w, h, xback, yback);
    }
}
