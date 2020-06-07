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

 This file contains functions and routines that govern the initialization
 of the device driver and update of the studio and display traversal.

  Functions:
    ddr_kopengl_drom_pre_initialization 
    ddr_kopengl_drom_post_initialization 
    ddr_kopengl_drom_lighting 
    ddr_kopengl_drom_update_local_data 
    ddr_kopengl_drom_start_update 
    ddr_kopengl_drom_update_studio 
    ddr_kopengl_drom_update_background
    ddr_kopengl_drom_update_display 

***************************************************************************/


#include "drom.h"
#include "../dcm/dcm.h"



/*****************************************************************************
 *  The first pass of the Dynamic Renderer Output Module initialization.
 ****************************************************************************/

void ddr_kopengl_drom_pre_initialization (void)
 {
  static DtMatrix4x4 identmat = IDENTITY_MATRIX_4x4; 

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] drom.pre_initialization\n");
#endif


  if (!dde_kopengl_drom_initialized) 
    {
     DROM.view         = (DtObject)0;

     DROM.mapbmp.max_count  = 0;
     DROM.mapbmp.count      = 0;
     DROM.mapbmp.mapping    = (DtObject *)0;
     DROM.mapbmp.raster     = (DtObject *)0;
     DROM.mapbmp.attributes = (dot_stdtexatt *)0;

     DROM.mapdifclr.max_count  = 0;
     DROM.mapdifclr.count      = 0;
     DROM.mapdifclr.mapping    = (DtObject *)0;
     DROM.mapdifclr.raster     = (DtObject *)0;
     DROM.mapdifclr.attributes = (dot_stdtexatt *)0;

     DROM.mapenv.max_count  = 0;
     DROM.mapenv.count      = 0;
     DROM.mapenv.mapping    = (DtObject *)0;
     DROM.mapenv.raster     = (DtObject *)0;
     DROM.mapenv.attributes = (dot_stdtexatt *)0;

     DROM.maptrnint.max_count  = 0;
     DROM.maptrnint.count      = 0;
     DROM.maptrnint.mapping    = (DtObject *)0;
     DROM.maptrnint.raster     = (DtObject *)0;
     DROM.maptrnint.attributes = (dot_stdtexatt *)0;

     dde_kopengl_drom_initialized = DcTrue;
    }

  /* 
   * Set the lighting model and material properties
   */

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  ddr_kopengl_drom_lighting ();
 
  dde_kopengl_drom_pre_init = DcTrue;
 }


/*****************************************************************************
 * This routine sets the lighting model and the material properties 
 ****************************************************************************/

void 
ddr_kopengl_drom_lighting (void)
{
 GLfloat param[4];

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] drom.lighting\n");
#endif

 /* 
  * Material properties. All values used here are default values.
  */
 param[0] = 0.2;
 param[1] = 0.2;
 param[2] = 0.2;
 param[3] = 1.0;
 glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, param);
 param[0] = 0.8;
 param[1] = 0.8;
 param[2] = 0.8;
 param[3] = 1.0;
 glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, param);
 param[0] = 0.0;
 param[1] = 0.0;
 param[2] = 0.0;
 param[3] = 1.0;
 glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, param);
 param[0] = 0.0;
 param[1] = 0.0;
 param[2] = 0.0;
 param[3] = 1.0;
 glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, param);

 /* 
  * Emmision and Specular reflectance are by default
  * (0.0, 0.0, 0.0, 1.0) in OpenGL. Default diffuse
  * reflectance of material is (0.8, 0.8, 0.8, 1.0)
  */

 /* 
  * two-sided lighting calculations and use an infinitely
  * distant viewpoint for reflection calculations
  */
 
 param[0] = 0.2;
 param[1] = 0.2;
 param[2] = 0.2;
 param[3] = 0.2;
 glLightModelfv (GL_LIGHT_MODEL_AMBIENT, param);
 glLightModelf (GL_LIGHT_MODEL_TWO_SIDE, 1.0);
 glLightModelf (GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);

 DROM.lighting = 0;
}



/*****************************************************************************
 *  The last pass of the Dynamic Renderer Output Module initialization.
 ****************************************************************************/

void ddr_kopengl_drom_post_initialization (void)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] drom.post_initialization\n");
#endif

  dde_kopengl_drom_pre_init = DcFalse;
 }



/*****************************************************************************
 *  The renderer calls this function to notify the driver that the local data
 *  of the DROM may be out of date with respect to the Dore' kernel.  This
 *  function will be called after drom_start_update.  It updates local state,
 *  such as background color, by querying the Dore' kernel.
 ****************************************************************************/

void ddr_kopengl_drom_update_local_data (void)      
 {
  DtInt     shaderange[2];
  DtVolume *actviewport;

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] drom.update_local_data\n");
#endif

  actviewport = DDdevice_InqActualViewport (DROM.device);
			   
  WINDOW->actviewxmin = actviewport->bll[0];
  WINDOW->actviewymin = actviewport->bll[1];
  WINDOW->actviewxmax = actviewport->fur[0];
  WINDOW->actviewymax = actviewport->fur[1];

  DEVICE->stereoflag = DDdevice_InqStereo (DROM.device);



  DDdevice_InqFrustumScaleTrans (DROM.device, 
                                 DEVICE->camera_to_device_scale, 
                                 DEVICE->camera_to_device_trans);
	    
  DDdevice_InqFrameScaleTrans (DROM.device,
                               DEVICE->frame_to_device_scale, 
                               DEVICE->frame_to_device_trans);
     
  DEVICE->annoscale[0] = DEVICE->frame_to_device_scale[0] / 
                         DEVICE->camera_to_device_scale[0];
  DEVICE->annoscale[1] = DEVICE->frame_to_device_scale[1] / 
                         DEVICE->camera_to_device_scale[1];
  DEVICE->annoscale[2] = DEVICE->frame_to_device_scale[2] / 
                         DEVICE->camera_to_device_scale[2];

  VIEW->clear_flag = DvInqClearFlag (DROM.view);

  DvInqBackgroundColor (DROM.view,
			&VIEW->background_color_model,
			VIEW->background_color);
  DvInqBackgroundJust (DROM.view,
		       &VIEW->background_just[0],
		       &VIEW->background_just[1]);

  VIEW->shade_index   = DvInqShadeIndex(DROM.view);
  DEVICE->shade_mode  = DdInqShadeMode (DROM.device);
  DEVICE->visual_type = DdInqVisualType (DROM.device);

  return;
 }




/*****************************************************************************
 *  The renderer calls this function at the beginning of each update of a
 *  device/view combination.  `device' is the handle to the device object
 *  being updated and `view' is the handle to the view object.  `device_data',
 *  `view_data' and `window_data' are pointers to the data previously created
 *  by calls to _create_local_device_data(), _create_local_view_data(), and
 *  _create_local_window_data().
 ****************************************************************************/

void ddr_kopengl_drom_start_update (DtObject device,
                                    DtObject view,
                                    DtPtr    device_data,
                                    DtPtr    view_data,
                                    DtPtr    window_data)
 {
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] drom.start_update\n");
#endif    

    DROM.device = device;
    DROM.view = view;

    DEVICE = device_data;
    VIEW   = view_data;
    WINDOW = window_data;
    CAMERA = &(VIEW->camera_data);
 }



/*****************************************************************************
 *  The renderer calls this routine each time the studio environment needs to
 *  be updated.  This function initializes the studio state and then calls
 *  (*traverse_studio)() to initiate a traversal of the studio group.  As the
 *  objects of the studio group are executed during this traversal, they call
 *  DROM functions with information about attribute changes and new cameras
 *  and lights.
 ****************************************************************************/

void ddr_kopengl_drom_update_studio (DtPFI traverse_studio)
 {
  auto   DtVolume    *clipvolume;
  auto   DtVolume     viewboundary;
  auto   DtReal       xres, yres;
  auto   DtReal       scale[3];
  auto   DtReal       trans[3];

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] in drom.update_studio\n");
#endif

  /* Initialize DROM matrices */
  LOAD_OPENGL_ID_MATRIX (CAMERA->world_to_cam_matrix);
  LOAD_OPENGL_ID_MATRIX (CAMERA->projection_matrix);

  /* Clear the projection and model view matrices */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  /* initialize the total number of lights to zero */
  VIEW->total_lights = 0;

  /* traverse the studio group */
  (*traverse_studio)();

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "done travesring studio\n");
#endif
    
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();

  /*
   * map from the view volume back to a normalized space
   * ( -w<x<w, -w<y<w, -w<z<0) but this normalized space is just
   * the visible portion of the volume
   * The clipping volume is in frame coordinates, the same coordinates
   * as the view volume
   */

  clipvolume = DDdevice_InqClippingVolume (DROM.device);

  scale[0] = 2.0/(clipvolume->fur[0]-clipvolume->bll[0]);
  scale[1] = 2.0/(clipvolume->fur[1]-clipvolume->bll[1]);
  scale[2] = 1.0/(clipvolume->fur[2]-clipvolume->bll[2]);

  trans[0] = -scale[0]*clipvolume->bll[0]-1;
  trans[1] = -scale[1]*clipvolume->bll[1]-1;
  trans[2] = -scale[2]*clipvolume->fur[2];

  SET_GL_TRANSLATE (trans[0], trans[1], trans[2]); 
  SET_GL_SCALE (scale[0], scale[1], scale[2]); 

  /*
   * map from normalized space to the volume of the view
   */

  DvInqBoundary (DROM.view, &viewboundary);

  scale[0] = (viewboundary.fur[0]-viewboundary.bll[0])/2.0;
  scale[1] = (viewboundary.fur[1]-viewboundary.bll[1])/2.0;
  scale[2] = (viewboundary.fur[2]-viewboundary.bll[2]);

  trans[0] = (viewboundary.fur[0]+viewboundary.bll[0])/2.0;
  trans[1] = (viewboundary.fur[1]+viewboundary.bll[1])/2.0;
  trans[2] = viewboundary.fur[2];

  /*
   * adjust the x scaling if the pixels of the screen are non-square
   */

  DdInqResolution (DROM.device, &xres, &yres);
  if (xres != 0.0) 
    {
     scale[0] *= (yres / xres);
    }

  SET_GL_TRANSLATE (trans[0], trans[1], trans[2]);
  SET_GL_SCALE (scale[0], scale[1], scale[2]);

  GL_MULT_MATRIX (CAMERA->projection_matrix);
  GET_GL_MATRIX (GL_PROJECTION_MATRIX, CAMERA->cam_to_frustum_matrix);

  glMatrixMode (GL_MODELVIEW);
  LOAD_GL_MATRIX (CAMERA->world_to_cam_matrix);

  ddr_kopengl_drom_update_display_lights ();

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] done with drom.update_studio\n");
#endif
 }



/*****************************************************************************
 ****************************************************************************/

void 
ddr_gl_drom_update_background (void)
{
 DtInt        vwidth, vheight;
 DtObject     raster_object;
 DtInt        deltax, deltay;
 DtInt        offsetx, offsety;
 DtInt        rwidth, rheight, rdepth;
 DtInt        width, height;
 DtRasterType rtype;
 DtPtr        rtypestring;
 DtPtr        data;
 DtInt        x, y;


#ifdef _DEBUG_KOPENGL_
 fprintf (stdout, "[KOpenGL Driver] drom.update_background\n");
#endif

 if (VIEW->clear_flag)
   {
    /*
     * Clear the view to the background color and draw the background
     * raster if it exists.
     */

    DDdevice_SetBackgroundColor (DROM.device,
                                 VIEW->background_color_model,
                                 VIEW->background_color);

    DDdevice_ClearRectangleDepthColor (DROM.device,
                                       (DtShort)WINDOW->actviewxmin,
                                       (DtShort)WINDOW->actviewymin,
                                       (DtShort)WINDOW->actviewxmax,
                                       (DtShort)WINDOW->actviewymax);

    vwidth  = (WINDOW->actviewxmax - WINDOW->actviewxmin);
    vheight = (WINDOW->actviewymax - WINDOW->actviewymin);

    raster_object = DDview_ResizeRaster (DROM.view, vwidth, vheight);
    if (raster_object)
      {
       if (DsInqObjClassId (raster_object) == DsInqClassId("DoRaster"))
         {
          DsInqRaster (raster_object, &rwidth, &rheight, &rdepth,
                             &rtype, &rtypestring, &data);

          if (rtype == DcRasterRGB)
            {
             /*
              * we will only deal with RGB rasters and ignore
              * all other rasters for backgrounds
              */

             /*
              * compute how the background is aligned to the view
              */

             deltax = (vwidth - rwidth);
             deltay = (vheight - rheight);
             offsetx = deltax*VIEW->background_just[0];
             offsety = deltay*(1.0 - VIEW->background_just[1]);

             if (deltay < 0)
               {
                /*
                 * the raster is bigger in y than the view
                 * so we offset into the data to get the starting
                 * scanline, and reduce the height of the raster
                 * to be written
                 */

                data = (DtPtr)((int)(data) + (-offsety)*rwidth*3);
                height = rheight + deltay;
                offsety = 0;
               }
             else
               {
                height = rheight;
               }

             if (deltax < 0)
               {
                /*
                 * the raster is bigger in x that the view
                 * so we offset into the data to get the
                 * starting pixel, and reduce the width of the
                 * raster to be written
                 */
                data = (DtPtr)((int)(data) + (-offsetx)*3);
                width = rwidth + deltax;
                offsetx = 0;
               }
             else
               {
                width = rwidth;
               }

             /*
              * rasters have their origin at the upper left hand
              * corner of the raster while the view coordinates
              * have positive y extending up.  As a result we
              * must compensate for the difference in coordinate
              * systems
              */
             x = WINDOW->actviewxmin + offsetx;
             y = WINDOW->actviewymax - offsety;

             for (; height > 0 ; height--, y--)
               {
                ddr_kopengl_dcm_write_scanline_byte (x, y, width, data);
                data = (DtPtr)((int)(data) + rwidth*3);
               }
            }
         }
      }
   }
}



/*****************************************************************************
 *  The renderer calls this function to begin the actual generation of the
 *  output image seen on the device.  This routine calls *(traverse_display)()
 *  to initiate a traversal of the display group.  As the objects of the
 *  display group are executed during this traversal, they will call DROM
 *  functions with information about attribute changes and geometry to be
 *  rendered.
 ****************************************************************************/

void ddr_kopengl_drom_update_display (DtPFI traverse_display)
 {
    
#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] drom.update_display\n");
#endif

  glViewport (WINDOW->actviewxmin, 
              WINDOW->actviewymin,
              WINDOW->actviewxmax - WINDOW->actviewxmin,
              WINDOW->actviewymax - WINDOW->actviewymin);

  ddr_gl_drom_update_background ();

  glMatrixMode (GL_MODELVIEW);
  LOAD_GL_MATRIX (CAMERA->world_to_cam_matrix);

  /*
   * Traverse the display group 
   */

#ifdef _DEBUG_KOPENGL_
  ddr_kopengl_drom_print_studio();
#endif
  
  (*traverse_display) ();

#ifdef _DEBUG_KOPENGL_
  fprintf (stdout, "[KOpenGL Driver] done drom.update_display\n");
#endif
 }
