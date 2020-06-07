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
 
#include <dore/internal/dogen.h>
#include <dore/internal/driver.h>
#include <dore/dore_develop/develop.h>

/*
 ======================================================================
 */
typedef struct {
    DtPtr device;
    char *filename;
    FILE *fp;
    DtFlag begin_page;
    DtInt page_no;
    DtInt shade_index;
    DtReal cwtavgpsweight[3];
    DtReal cwtavgpslim[3];
    DtShadeMode shade_mode;
    DtReal width;
    DtReal height;
    DtReal bg_color[3];
    unsigned long bg_color_index;
    unsigned long fg_color_index;
    DtVisualType visualtype;
    DtFlag auto_size;
    DtObject current_view;
    DtObject previous_view;
    DtFlag border;
} ddt_ps_data;

extern ddt_ps_data *dde_ps_current_device_data;


    /***  Function Prototypes  ***/

void	ddr_ps_DrawLine  (DtInt *, DtInt *, DtColorRGB, DtColorRGB);
void	ddr_ps_DrawPoint  (DtInt *, DtColorRGB);
void	ddr_ps_DrawTriangle  (
		DtInt*, DtInt*, DtInt*, DtColorRGB, DtColorRGB, DtColorRGB);
void	ddr_ps_dcm_become_current_driver  (DtPtr);
void	ddr_ps_dcm_clear_rectangle_depth  (DtShort, DtShort, DtShort, DtShort);
void	ddr_ps_dcm_clear_rectangle_depth_and_color  (
		DtShort, DtShort, DtShort, DtShort);
void	ddr_ps_dcm_close_device  (void);
DtPtr	ddr_ps_dcm_create_local_data  (DtObject);
void	ddr_ps_dcm_flush  (void);
DtFlag	ddr_ps_dcm_initialize_device  (DtObject, DtPtr, DtPtr, DtPtr);
void	ddr_ps_dcm_inquire_auto_size  (DtFlag *);
void	ddr_ps_dcm_inquire_clip_list  (DtUInt*, DtReal**, DtReal[4], DtFlag*);
void	ddr_ps_dcm_inquire_color_entries  (DtColorModel, int, int, DtReal *);
void	ddr_ps_dcm_inquire_device_extent  (DtVolume *);
void	ddr_ps_dcm_inquire_ncolors  (DtInt *);
DtFlag	ddr_ps_dcm_inquire_pixel_data  (
		DtRasterType, DtInt*, DtInt*, DtRasterType*, DtPtr*, DtFlag*);
void	ddr_ps_dcm_inquire_resolution  (DtReal *, DtReal *);
void	ddr_ps_dcm_inquire_stereo  (DtFlag *);
void	ddr_ps_dcm_inquire_visual_type  (DtVisualType *);
void	ddr_ps_dcm_set_background_color  (DtColorModel, DtReal *);
void	ddr_ps_dcm_set_color_entries  (DtColorModel, int, int, DtReal *);
void	ddr_ps_dcm_set_current_view  (DtObject);
void	ddr_ps_dcm_set_depth_buffer_enable  (DtFlag);
void	ddr_ps_dcm_set_depth_buffer_write  (DtFlag);
void	ddr_ps_dcm_set_foreground_color  (DtColorModel, DtReal *);
void	ddr_ps_dcm_set_options  (DtPtr);
void	ddr_ps_dcm_set_shade_index  (DtInt);
void	ddr_ps_dcm_start_page  (void);
void	ddr_ps_dcm_swap_buffers  (void);
void	ddr_ps_dcm_update_geometry  (void);
void	ddr_ps_dcm_write_scanline_byte  (DtInt, DtInt, DtInt, DtUChar *);
DtPtr	ddr_ps_return_DCM_fcns  (void);
