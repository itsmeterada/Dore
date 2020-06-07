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

    /***  Macro Definitions  ***/
 
/* character set definitions for keyboard interaction */
#define GENSET		0x0
#define TEXSET		0x1

#define print(string)	fputs(string,stdout)


    /***  Global Variables  ***/

#if defined(titan)
extern int use_XB;
extern int use_Xtitan;
#endif

extern DtFlag		clearflag;
extern DtInt		shadetype;
extern DtFlag		multiple_views;
extern DtFlag		pseudocolor;
extern DtSwitch		singlebuffered;
extern DtSwitch		ztc;
extern DtSwitch		hw_boundingvol;
extern DtVisualType	visualtype;
extern DtSwitch		stereoflag;
extern DtSwitch		multi_pass_transp;
extern DtSwitch		separate_disp_groups;
extern DtSwitch 	use_sample;
extern DtSwitch		use_stdin;
extern DtSwitch		use_spheres;
extern int		cylinders;
extern DtSwitch		dither_flag;
extern DtSwitch		script_flag;
extern FILE		*scr_fd;

extern char		devicetype[200];
extern DtInt		use_zbuffer;

extern DtFlag		user_window;
extern DtFlag		use_X;
extern DtObject		device;
extern DtObject		frame;
extern DtObject		view;
extern DtObject		m_view[4];
extern DtVolume 	m_volume[4];
extern DtColorRGB	backgroundcolor;

extern DtInt		gray_scale_size;
extern DtInt		shade_rng_size;
extern DtReal		gray_scale_map[4096*3];
extern DtReal		color_scale_map[4096*3];
extern DtReal		color1_scale_map[4096*3];
extern DtReal		shade_rng_map[4096*3];
extern DtInt		shade_ranges[2*9];
extern DtRealTriple	range_color[8*3];

extern char *print_visual();

extern int 		gr_type;
