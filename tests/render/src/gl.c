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
 
#include <stdio.h>
#include "dore.h"
#include "trnsfrmr.h"
#include "render.h"
#include <gl.h>
#include <device.h>

/*
  ======================================================================
 */
int setup_iris_window (
    int topx,
    int topy,
    int width,
    int height,
    DtFlag use_position)
{
   foreground();
   if (use_position) {
       topy = 1024 - topy;
       prefposition (topx, topx + (width - 1), topy, topy - (height - 1));
   }
   return(winopen("User Made Dore Window"));
}
/*
  ======================================================================
 */
int iris_process_input (void)
{
   int dev;
   short val;

   qdevice(KEYBD);
   qdevice(LEFTMOUSE);
   qdevice(REDRAW);

   while (1) {

      dev = qread(&val);

      if (dev == LEFTMOUSE && val) {

	 long x0,y0;
	 DtPoint3 pck_pnt;

	 getorigin(&x0,&y0);

	 pck_pnt[0] = getvaluator(MOUSEX)-x0;
	 pck_pnt[1] = getvaluator(MOUSEY)-y0;
	 pck_pnt[2] = 0.;

	 process_pick(pck_pnt);
      } else if (dev == KEYBD && process_character(val)) {
	  break;
      } else if (dev == REDRAW) {
	  DdUpdate(device);
      }

   }
}
