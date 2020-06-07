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
 
/**************************************************************************
  
  Functions:
    ddr_kopengl_dcm_inquire_color_entries 
    ddr_kopengl_dcm_inquire_auto_size 
    ddr_kopengl_dcm_inquire_clip_list
    ddr_kopengl_dcm_inquire_device_extent 
    ddr_kopengl_dcm_inquire_ncolors 
    ddr_kopengl_dcm_inquire_pixel_data 
    ddr_kopengl_dcm_inquire_resolution 
    ddr_kopengl_dcm_inquire_stereo
    ddr_kopengl_dcm_inquire_visual_type 

**************************************************************************/
 

#include <stdio.h>
#include "dcm.h"


/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_inquire_color_entries (DtColorModel  colormodel,
                                            int           start,
                                            int           n,
                                            DtReal       *c)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_color_entries\n");
#endif
 }



/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_inquire_auto_size (DtFlag *flag)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_auto_size\n");
#endif

  *flag = DCM->auto_size;
 }




/****************************************************************************
 *
 ****************************************************************************/


void ddr_kopengl_dcm_inquire_clip_list (DtUInt  *nrects,
                                        DtReal **clip_list,
                                        DtReal   extent[4],
                                        DtFlag   *partiallyobscuredflag) 
 {
  static DtReal clist[4];

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_clip_list\n");
#endif

  clist[0] = extent[0] = 0;
  clist[1] = extent[1] = 0;
  clist[2] = extent[2] = DCM->width;
  clist[3] = extent[3] = DCM->height;

  *nrects = 1;
  *clip_list = clist;
  *partiallyobscuredflag = DcFalse;
 }




/****************************************************************************
 *
 ****************************************************************************/


void ddr_kopengl_dcm_inquire_device_extent (DtVolume *vol)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_device_extent\n");
#endif

    vol->bll[0] = 0.0;
    vol->bll[1] = 0.0;
    vol->bll[2] = 0.0;
    vol->fur[0] = DCM->width;
    vol->fur[1] = DCM->height;
    vol->fur[2] = 1.0;
 }


/****************************************************************************
 *
 ****************************************************************************/


void ddr_kopengl_dcm_inquire_ncolors (DtInt *ncolors)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_ncolors\n");
#endif
  *ncolors = 256;
 }




/****************************************************************************
 *
 ****************************************************************************/

DtFlag ddr_kopengl_dcm_inquire_pixel_data (DtRasterType  requesttype, 
                                           DtInt        *width, 
                                           DtInt        *height, 
                                           DtRasterType *type, 
				           DtPtr        *data, 
                                           DtFlag       *userdelete)
 {

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_pixel_data\n");
#endif

  return (DcFalse);
 }






/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_inquire_resolution (DtReal *xres, DtReal *yres)
 {

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_resolution\n");
#endif

  *xres = .28125;
  *yres = .28125;
 }





/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_inquire_stereo (DtFlag *stereoswitch)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_stereo\n");
#endif

  *stereoswitch = DcFalse;
 }



/****************************************************************************
 *
 ****************************************************************************/

void ddr_kopengl_dcm_inquire_visual_type (DtVisualType *visualtype)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] dcm_inquire_visual_type\n");
#endif

  *visualtype = DCM->visualtype;
 }


