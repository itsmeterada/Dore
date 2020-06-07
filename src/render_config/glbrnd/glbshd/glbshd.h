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

struct glbshd_static_data {
	DtInt lightcount;
	DtInt max_lightcount;
	DtInt maxraylvl;
	DtInt raylvl;
	DtReal background_color[3];
	};

struct glbshd_object_data {
	DtReal ambint;
	DtSwitch ambswi;
	DtColorRGB difclr;
	DtReal difint;
	DtSwitch difswi;
	DtInt reflectlimit;
	DtSwitch reflectswi;
	DtSwitch shadowswi;
	DtColorRGB spcclr;
	DtReal spcfct;
	DtReal spcint;
	DtSwitch spcswi;
	DtSwitch sldtexswi;
	DtInt sldtexindex;
	DtColorRGB transpclr;
	DtReal transpint;
	DtSwitch transpswi;
	};

struct glbshd_light_data {
	DtColorRGB ambient_reflectance;
	DtRealTriple *cosine;
	DtReal *intens;
	DtColorRGB *color;
	DtRealTriple *h;
	};

struct glbshd_surface_data {
	DtFlag userdata_available;
	DtReal userdata[3];
	DtFlag wcsnrm_available;
	DtReal wcsnrm[3];
	DtFlag wcsloc_available;
	DtReal wcsloc[3];
	struct ray *ray;
	struct intersect *intersect;
	};


extern struct glbshd_static_data dre_glbshd_static_data;
extern struct glbshd_light_data dre_glbshd_light_data;
extern struct glbshd_object_data *dre_glbshd_object_data;
extern struct glbshd_surface_data *dre_glbshd_surface_data;
