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
#include "dore/dore_develop/develop.h"
#include "../gp.h"

#include <stdio.h>
#include <signal.h>
#include <sunwindow/notify.h>
#include <sunwindow/pw_dblbuf.h>

typedef struct {
   DtPtr device;
   Canvas canvas;
   Pixwin *pw;
   int child_pid;
   int user_canvas;
   int width;
   int height;
   DtVisualType visualtype;
   DtFlag auto_size;
   GP_data_ptr gp;
   DtReal *rects;
   DtInt nrects;
   DtInt shade_index;
   DtReal cbtselpsweight[3];
   DtInt cbtselpsmask[3];
   DtReal cwtavgpsweight[3];
   DtReal cwtavgpslim[3];
   DtShadeMode shade_mode;
   short bg_color_index;
   short fg_color_index;
   DtObject previous_view;
   DtObject current_view;
} ddt_sunview_cxp_data;

extern GP_data_ptr dde_sunview_cxp_current_gp;
extern ddt_sunview_cxp_data *dde_sunview_cxp_current_dcm_data;

