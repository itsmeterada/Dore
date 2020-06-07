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
	void ddr_ps_dcm_set_color_entries (colormodel,start,n,c)
	void ddr_ps_dcm_set_background_color (colormodel, c)
	void ddr_ps_dcm_set_foreground_color (colormodel, c)
	void ddr_ps_dcm_set_current_view (view)
	void ddr_ps_dcm_set_depth_buffer_enable (flag)
	void ddr_ps_dcm_set_depth_buffer_write (flag)
	void ddr_ps_dcm_set_shade_index (index)

  ======================================================================
 */
#include "dcm.h"

/* ====================================================================== */

void ddr_ps_dcm_set_color_entries (
    DtColorModel colormodel,
    int start,
    int n,
    DtReal *c)
{
    ddt_ps_data *dev;

#   ifdef DEBUG
	printf("... processing device set color entries ...\n");
	fflush(stdout);
#   endif
        
    dev = dde_ps_current_device_data;
}

/* ====================================================================== */

void ddr_ps_dcm_set_background_color (
    DtColorModel colormodel,
    DtReal *c)
{
    ddt_ps_data *device_data;
    DtInt index;

#   ifdef DEBUG
	printf("... processing device set background color ...\n");
	fflush(stdout);
#   endif

    device_data = dde_ps_current_device_data;

    device_data->bg_color[0] = c[0];
    device_data->bg_color[1] = c[1];
    device_data->bg_color[2] = c[2];
}

/* ====================================================================== */

void ddr_ps_dcm_set_foreground_color (
    DtColorModel colormodel,
    DtReal *c)
{
    ddt_ps_data *device_data;

#   ifdef DEBUG
	printf("...processing device set foreground color \n");
	fflush(stdout);
#   endif
	
    device_data = dde_ps_current_device_data;
}

/* ====================================================================== */

void ddr_ps_dcm_set_current_view (
    DtObject view)
{
    dde_ps_current_device_data->previous_view =
	    dde_ps_current_device_data->current_view;
    dde_ps_current_device_data->current_view = view;
}

/* ====================================================================== */

void ddr_ps_dcm_set_depth_buffer_enable (
    DtFlag flag)
{
#   ifdef DEBUG
	printf("... processing device set depth buffer enable ...\n");
	fflush(stdout);
#   endif
}

/* ====================================================================== */

void ddr_ps_dcm_set_depth_buffer_write (
    DtFlag flag)
{
#   ifdef DEBUG
	printf("... processing device set depth buffer write ...\n");
	printf("Not Implemented.\n");
	fflush(stdout);
#   endif
}

/* ====================================================================== */

void ddr_ps_dcm_set_shade_index (
    DtInt index)
{
#   ifdef DEBUG
	printf("...processing device set shade index\n");
	fflush(stdout);
#   endif

    dde_ps_current_device_data->shade_index = index;

#   ifdef DEBUG
	printf("shade index = %d\n",index);
	fflush(stdout);
#   endif
}
