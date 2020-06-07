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
 
#include "dore/internal/dogen.h"
#include "dore/dore_develop/private/zbuffer.h"

#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <signal.h>
#include <sunwindow/notify.h>

typedef struct {
   DtObject device;
   Canvas canvas;
   Pixwin *pw;
   Pixrect *back_buf;
   int child_pid;
   int user_canvas;
   int width;
   int height;
   DtVisualType visualtype;
   int doubleBuffered;
   int dbf_enable;
   int dbf_write;
   int simulator;
   int dither;
   int update_needed;
   DtUShort *depth_buf;
   DtFlag auto_size;
   DtInt shade_index;
   DtReal cbtselpsweight[3];
   DtInt cbtselpsmask[3];
   DtReal cwtavgpsweight[3];
   DtReal cwtavgpslim[3];
   DtInt nred;
   DtInt ngreen;
   DtInt nblue;
   DtReal rscale;
   DtReal gscale;
   DtReal bscale;
   DtShadeMode shade_mode;
   short bg_color_index;
   short fg_color_index;
   short draw_index;
} ddt_sunview_gen_data;


/* ====================================================================== */
/* dev.c */

extern ddt_sunview_gen_data *dde_sunview_gen_current_device_data;

extern DtPtr
ddr_sunview_gen_return_DCM_fcns		();
extern DtPtr 
ddr_sunview_gen_dcm_create_local_data 	(/* DtPtr device */);
extern DtFlag
ddr_sunview_gen_dcm_initialize_device 	(/* DtPtr device, 
					    ddt_sunview_gen_data *device_data, 
					    DtPtr argstring */);
extern void
ddr_sunview_gen_dcm_close_device 	();
extern void
ddr_sunview_gen_dcm_flush 		();
extern void
ddr_sunview_gen_dcm_swap_buffers 	();
extern void
ddr_sunview_gen_dcm_update_geometry 	();
extern void
ddr_sunview_gen_dcm_become_current_driver
					(/* ddt_sunview_gen_data *device_data */);
extern void
ddr_sunview_gen_dcm_set_options		(/* DtPtr argstring */);
extern void
ddr_sunview_gen_dcm_setup_shading	();
extern void
ddr_sunview_gen_InitSimulator		();
extern void
ddr_sunview_gen_FreeBuffer 		();
extern void
ddr_sunview_gen_UpdateSimulator		();

/* ====================================================================== */
/* set.c */

extern void
ddr_sunview_gen_dcm_set_background_color 
					(/* DtColorModel colormodel, DtReal *c */);
extern void
ddr_sunview_gen_dcm_set_foreground_color 
					(/* DtColorModel colormodel, DtReal *c */);
extern void
ddr_sunview_gen_dcm_set_color_entries	(/* DtColorModel colormodel,
					    int start, int n, DtReal *c */);
extern void
ddr_sunview_gen_dcm_set_current_view 	(/* DtObject view */);
extern void
ddr_sunview_gen_dcm_set_depth_buffer_enable 
					(/* DtFlag flag */);
extern void
ddr_sunview_gen_dcm_set_depth_buffer_write 
					(/* DtFlag flag */);
extern void
ddr_sunview_gen_dcm_set_shade_index 	(/* DtInt index */);

/* ====================================================================== */
/* inq.c */

extern void
ddr_sunview_gen_dcm_inquire_color_entries
					(/* DtColorModel colormodel,
					    int start, int n, DtReal *c */);
extern void
ddr_sunview_gen_dcm_inquire_auto_size 	(/* DtFlag *flag */);
extern void
ddr_sunview_gen_dcm_inquire_clip_list 	(/* DtUInt *nrects, DtReal **clip_list, 
					    DtReal extent[4], 
					    DtFlag *partiallyobscuredflag */);
extern void
ddr_sunview_gen_dcm_inquire_device_extent 
					(/* DtVolume *volume */);
extern void
ddr_sunview_gen_dcm_inquire_ncolors 	(/* DtInt *ncolors */);
extern DtFlag
ddr_sunview_gen_dcm_inquire_pixel_data
				 	(/* DtRasterType requesttype, 
					    DtInt *width, 
					    DtInt *height, DtRasterType *type, 
					    DtPtr *data, DtFlag *userdelete */);
extern void
ddr_sunview_gen_dcm_inquire_resolution 
					(/* DtReal *xres, DtReal *yres */);
extern void
ddr_sunview_gen_dcm_inquire_stereo 	(/* DtFlag *stereoswitch */);
extern void
ddr_sunview_gen_dcm_inquire_visual_type 
					(/* DtVisualType *visualtype */);

/* ====================================================================== */
/* output.c */

extern void
ddr_sunview_gen_dcm_clear_rectangle_depth
					(/* DtShort xmin, DtShort ymin, 
					    DtShort xmax, DtShort ymax */);
extern void
ddr_sunview_gen_dcm_clear_rectangle_depth_and_color
					(/* DtShort xmin, DtShort xmax, 
					    DtShort ymin, DtShort ymax */);
extern void
ddr_sunview_gen_dcm_write_scanline_byte 
					(/* DtInt xstart, DtInt ystart, 
					    DtInt length, DtUChar *data */);
extern void
ddr_sunview_gen_DrawPoint_back		(/* DtInt *point, DtColorRGB color */);
extern void
ddr_sunview_gen_DrawLine_back		(/* DtInt *point1, DtInt *point2, 
					    DtColorRGB color1, 
					    DtColorRGB color2 */);
extern void
ddr_sunview_gen_DrawTriangle_back	(/* DtInt *point1, DtInt *point2, 
					    DtInt *point3, 
					    DtColorRGB color1, 
					    DtColorRGB color2, 
					    DtColorRGB color3 */);
extern void
ddr_sunview_gen_PutPixel_back		(/* DtInt x, DtInt y, DtColorRGB color */);
extern void
ddr_sunview_gen_DrawPoint_front		(/* DtInt *point, DtColorRGB color */);
extern void
ddr_sunview_gen_DrawLine_front		(/* DtInt *point1, DtInt *point2, 
					    DtColorRGB color1, 
					    DtColorRGB color2 */);
extern void
ddr_sunview_gen_DrawTriangle_front	(/* DtInt *point1, DtInt *point2, 
					    DtInt *point3, 
					    DtColorRGB color1, 
					    DtColorRGB color2, 
					    DtColorRGB color3 */);
extern void
ddr_sunview_gen_PutPixel_front		(/* DtInt x, DtInt y, DtColorRGB color */);
extern void ddr_sunview_gen_Dither	(/* DtInt x, DtInt y, DtReal *color */);
extern void ddr_sunview_gen_DitherColor	(/* DtInt x, DtInt y, DtReal *color */);

/* ====================================================================== */
