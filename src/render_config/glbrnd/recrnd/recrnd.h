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
 
#include "../glbrnd.h"


struct recrnd_data {
	DtFlag debug_switch;
	DtInt screen_pixel_resolution;
	DtReal film_window_x_minimum;
	DtReal film_window_y_minimum;
	DtReal film_window_x_maximum;
	DtReal film_window_y_maximum;
	DtInt pixel_window_x_minimum;
	DtInt pixel_window_y_minimum;
	DtInt pixel_window_x_maximum;
	DtInt pixel_window_y_maximum;
	DtReal screen_ccs_x_minimum;
	DtReal screen_ccs_y_minimum;
	DtReal screen_ccs_pixel_size;
	DtMatrix4x4 ccstowcsmat;
	DtReal background_color[3];
	struct ray view_ray;
	struct recrnd_statistics *recrnd_statistics;
	DtSwitch antialiasswitch;
	DtReal antialiasvalue;
	DtMatrix4x4 fcstowcsmat;
	DtInt resolution_x;
	DtInt resolution_y;
	DtReal pixel_width;
	DtReal pixel_height;
	DtReal aspect_ratio;
	};

struct recrnd_statistics {
	DtInt pixels;
	};


extern struct recrnd_data dre_recrnd_data;
