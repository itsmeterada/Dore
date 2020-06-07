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
	void ddr_template_drom_pre_initialization ()
	void ddr_template_drom_post_initialization ()
	void ddr_template_drom_update_local_data ()
	void ddr_template_drom_start_update (device, view, device_data, view_data, window_data)
	void ddr_template_drom_update_studio (traverse_studio)
	void ddr_template_drom_update_display (traverse_display)

  ======================================================================
 */
#include "drom.h"
/*
 ======================================================================
 */
void ddr_template_drom_pre_initialization (void)
{
    static DtMatrix4x4 identmat = { 1.,0.,0.,0.,
				    0.,1.,0.,0.,
				    0.,0.,1.,0.,
				    0.,0.,0.,1.};


    if (!dde_template_drom_initialized) {
	dde_template_drom_data.max_matrices = 0;
	dde_template_drom_data.num_matrices = 0;
	dde_template_drom_data.matrix_stack = (ddt_template_drom_matrix *)0;
	dde_template_drom_data.device = (DtObject)0;
	dde_template_drom_data.view = (DtObject)0;

	ddr_template_matrix_concat (dde_template_drom_data.lcstowcsmat, 
				    identmat, DcReplace);
	ddr_template_matrix_concat (dde_template_drom_data.lcstofcsmat, 
				    identmat, DcReplace);
	ddr_template_matrix_concat (dde_template_drom_data.nrmlcstowcsmat,
				    identmat, DcReplace);

	dde_template_drom_data.mapbmp.max_count = 0;
	dde_template_drom_data.mapbmp.count = 0;
	dde_template_drom_data.mapbmp.mapping = (DtObject *)0;
	dde_template_drom_data.mapbmp.raster = (DtObject *)0;
	dde_template_drom_data.mapbmp.attributes = (dot_stdtexatt *)0;

	dde_template_drom_data.mapdifclr.max_count = 0;
	dde_template_drom_data.mapdifclr.count = 0;
	dde_template_drom_data.mapdifclr.mapping = (DtObject *)0;
	dde_template_drom_data.mapdifclr.raster = (DtObject *)0;
	dde_template_drom_data.mapdifclr.attributes = (dot_stdtexatt *)0;

	dde_template_drom_data.mapenv.max_count = 0;
	dde_template_drom_data.mapenv.count = 0;
	dde_template_drom_data.mapenv.mapping = (DtObject *)0;
	dde_template_drom_data.mapenv.raster = (DtObject *)0;
	dde_template_drom_data.mapenv.attributes = (dot_stdtexatt *)0;

	dde_template_drom_data.maptrnint.max_count = 0;
	dde_template_drom_data.maptrnint.count = 0;
	dde_template_drom_data.maptrnint.mapping = (DtObject *)0;
	dde_template_drom_data.maptrnint.raster = (DtObject *)0;
	dde_template_drom_data.maptrnint.attributes = (dot_stdtexatt *)0;

	dde_template_drom_initialized = DcTrue;
    }

    dde_template_drom_pre_init = DcTrue;
}
/*
 ======================================================================
 */
void ddr_template_drom_post_initialization (void)
{
    dde_template_drom_pre_init = DcFalse;
}
/*
 ======================================================================
 */
void ddr_template_drom_update_local_data (void)      
{
    DtInt 	shaderange[2];
    DtVolume *	actviewport;
    DtReal	ftodtrans[3];

#ifdef DEBUG
    printf ("ddr_template_update_local_data\n");
#endif

    actviewport = DDdevice_InqActualViewport (dde_template_drom_data.device);
			   
    dde_template_drom_curwindat->actviewxmin = actviewport->bll[0];
    dde_template_drom_curwindat->actviewymin = actviewport->bll[1];
    dde_template_drom_curwindat->actviewxmax = actviewport->fur[0];
    dde_template_drom_curwindat->actviewymax = actviewport->fur[1];

    dde_template_drom_curdevdat->stereoswitch = 
	    DDdevice_InqStereo (dde_template_drom_data.device);

    DDdevice_InqFrustumScaleTrans 
	    (dde_template_drom_data.device,
	     dde_template_drom_curdevdat->ctodscale,
	     dde_template_drom_curdevdat->ctodtrans);

    DDdevice_InqFrameScaleTrans (dde_template_drom_data.device,
				 dde_template_drom_curdevdat->ftodscale,
				 ftodtrans);
     
    dde_template_drom_curdevdat->annoscale[0] = 
	    dde_template_drom_curdevdat->ftodscale[0] / 
		    dde_template_drom_curdevdat->ctodscale[0];
    dde_template_drom_curdevdat->annoscale[1] = 
	    dde_template_drom_curdevdat->ftodscale[1] /
		    dde_template_drom_curdevdat->ctodscale[1];
    dde_template_drom_curdevdat->annoscale[2] = 
	    dde_template_drom_curdevdat->ftodscale[2] /
		    dde_template_drom_curdevdat->ctodscale[2];

    dde_template_drom_curviwdat->clear_flag = 
	    DvInqClearFlag(dde_template_drom_data.view);

    DvInqBackgroundColor (dde_template_drom_data.view,
			  &dde_template_drom_curviwdat->background_color_model,
			  dde_template_drom_curviwdat->background_color);
    DvInqBackgroundJust (dde_template_drom_data.view,
			 &dde_template_drom_curviwdat->background_just[0],
			 &dde_template_drom_curviwdat->background_just[1]);

    dde_template_drom_curviwdat->shade_index =
	    DvInqShadeIndex(dde_template_drom_data.view);

    dde_template_drom_curdevdat->shade_mode = 
	    DdInqShadeMode (dde_template_drom_data.device);

    dde_template_drom_curdevdat->visual_type = 
	    DdInqVisualType (dde_template_drom_data.device);

}
/*
 ======================================================================
 */
void ddr_template_drom_start_update (
    DtObject device,
    DtObject view,
    DtPtr device_data,
    DtPtr view_data,
    DtPtr window_data)
{
#ifdef DEBUG
    printf ("ddr_template_drom_start_update\n");
    printf ("\tdevice:		0x%x\n", device);
    printf ("\tview:		0x%x\n", view);
    printf ("\tdevice_data:	0x%x\n", device_data);
    printf ("\tview_data:	0x%x\n", view_data);
    printf ("\twindow_data:	0x%x\n", window_data);
#endif    

    dde_template_drom_data.device = device;
    dde_template_drom_data.view = view;

    dde_template_drom_curdevdat = device_data;
    dde_template_drom_curviwdat = view_data;
    dde_template_drom_curwindat = window_data;

    dde_template_drom_curcamdat = &(dde_template_drom_curviwdat->camera_data);
}
/*
 ======================================================================
 */
void ddr_template_drom_update_studio (
    DtPFI traverse_studio)
{
    static DtReal lcscamloc[4]  = { 0., 0., 0., 1.};
    static DtReal lcscamnorm[4] = { 0., 0., 1., 1.};
    DtReal wcscamloc[4];
    DtReal wcscamnorm[4];
    DtMatrix4x4 matrix;
    DtVolume *clipvolume;
    DtVolume viewboundary;
    DtReal xres, yres;
    DtReal scale[3];
    DtReal trans[3];

#ifdef DEBUG
    printf ("ddr_template_drom_update_studio\n");
    printf ("\ttraverse_studio	0x%x\n", traverse_studio);
#endif

    /*
     * initialize the total number of lights to zero
     */

    dde_template_drom_curviwdat->total_lights = 0;

    /*
     * traverse the studio group
     */

    (*traverse_studio)();
    
    /*
     * compute the world coordinate to camera coordinate transformation matrix
     */

    ddr_template_matrix_concat(dde_template_drom_curcamdat->wcstocammat,
			       dde_template_drom_curcamdat->camtowcsmat,
			       DcReplace);
    ddr_template_matrix_invert(dde_template_drom_curcamdat->wcstocammat);

    /*
     * compute the location of the camera in world coordinates
     * It is located at the origin looking down -Z in camera coordinates
     */

    ddr_template_math_pnttrns(lcscamloc, 
			      dde_template_drom_curcamdat->camtowcsmat,
			      wcscamloc);
    if (wcscamloc[3] != 0.0) {
	wcscamloc[0] /=	wcscamloc[3];
	wcscamloc[1] /=	wcscamloc[3];
	wcscamloc[2] /=	wcscamloc[3];
    }

    /*
     * compute the vector toward the camera location
     * the camera is looking in the -Z direction so (0, 0, 1) is the vector
     * toward the camera in camera coordinates
     */

    ddr_template_math_pnttrns(lcscamnorm,
			      dde_template_drom_curcamdat->camtowcsmat,
			      wcscamnorm);
    
    if (wcscamnorm[3] != 0.0) {
	wcscamnorm[0] /= wcscamnorm[3];
	wcscamnorm[1] /= wcscamnorm[3];
	wcscamnorm[2] /= wcscamnorm[3];
    }

    wcscamnorm[0] -= wcscamloc[0];
    wcscamnorm[1] -= wcscamloc[1];
    wcscamnorm[2] -= wcscamloc[2];
    ddr_template_math_renormalize(wcscamnorm);
    
    dde_template_drom_curcamdat->wcscamloc[0] = wcscamloc[0];
    dde_template_drom_curcamdat->wcscamloc[1] = wcscamloc[1];
    dde_template_drom_curcamdat->wcscamloc[2] = wcscamloc[2];

    dde_template_drom_curcamdat->wcscamnorm[0] = wcscamnorm[0];
    dde_template_drom_curcamdat->wcscamnorm[1] = wcscamnorm[1];
    dde_template_drom_curcamdat->wcscamnorm[2] = wcscamnorm[2];

    /* 
     * map from world coordinates to camera coordinates where the
     * camera is at the origin looking down the -Z axis with Y up.
     */

    ddr_template_matrix_concat(dde_template_drom_curcamdat->wcstofcsmat,
			       dde_template_drom_curcamdat->wcstocammat,
			       DcReplace);

    /* 
     * map from the camera coordinates to the projection volume of
     * -w < x < w, -w < y < w, -w < z < 0
     */

    ddr_template_matrix_concat(dde_template_drom_curcamdat->wcstofcsmat,
			       dde_template_drom_curcamdat->projmat,
			       DcPostConcatenate);

    /*
     * map from normalized space to the volume of the view
     */

    if (dde_template_drom_curcamdat->camera_type != DcCameraArbitrary) {
	DvInqBoundary(dde_template_drom_data.view, &viewboundary);

	scale[0] = (viewboundary.fur[0]-viewboundary.bll[0])/2.;
	scale[1] = (viewboundary.fur[1]-viewboundary.bll[1])/2.;
	scale[2] = (viewboundary.fur[2]-viewboundary.bll[2]);

	trans[0] = (viewboundary.fur[0]+viewboundary.bll[0])/2.;
	trans[1] = (viewboundary.fur[1]+viewboundary.bll[1])/2.;
	trans[2] = viewboundary.fur[2];

	/*
	 * adjust the x scaling if the pixels of the screen are non-square
	 */

	DdInqResolution(dde_template_drom_data.device, &xres,&yres);
	if (xres != 0.0) {
	    scale[0] *= (yres / xres);
	}

	ddr_template_matrix_scale(dde_template_drom_curcamdat->wcstofcsmat,
				 scale[0], scale[1], scale[2], 
				  DcPostConcatenate);

	ddr_template_matrix_translate(dde_template_drom_curcamdat->wcstofcsmat,
				      trans[0], trans[1], trans[2], 
				      DcPostConcatenate);
    }

    /*
     * map from the view volume back to a normalized space
     * ( -w<x<w, -w<y<w, -w<z<0) but this normalized space is just
     * the visible portion of the volume
     * The clipping volume is in frame coordinates, the same coordinates
     * as the view volume
     */

    clipvolume = DDdevice_InqClippingVolume(dde_template_drom_data.device);

    scale[0] = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
    scale[1] = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
    scale[2] = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

    trans[0] = -scale[0]*clipvolume->bll[0]-1;
    trans[1] = -scale[1]*clipvolume->bll[1]-1;
    trans[2] = -scale[2]*clipvolume->fur[2];

    ddr_template_matrix_scale(dde_template_drom_curcamdat->wcstofcsmat,
			      scale[0], scale[1], scale[2], 
			      DcPostConcatenate);

    ddr_template_matrix_translate(dde_template_drom_curcamdat->wcstofcsmat,
				  trans[0], trans[1], trans[2], 
				  DcPostConcatenate);
}
/*
 ======================================================================
 */
void ddr_template_drom_update_display (
    DtPFI traverse_display)
{
    DtInt vwidth, vheight;
    DtObject raster_object;
    DtInt deltax, deltay;
    DtInt offsetx, offsety;
    DtInt rwidth, rheight, rdepth;
    DtInt width, height;
    DtRasterType rtype;
    DtPtr rtypestring;
    DtPtr data;
    DtInt x, y;
    
#ifdef DEBUG
    printf ("ddr_template_drom_update_display\n");
    printf ("\ttraverse_display	0x%x\n", traverse_display);
#endif

    if (dde_template_drom_curviwdat->clear_flag) {
	/*
	 * Clear the view to the background color and draw the background
	 * raster if it exists.
	 */

	DDdevice_SetBackgroundColor
		(dde_template_drom_data.device,
		 dde_template_drom_curviwdat->background_color_model,
		 dde_template_drom_curviwdat->background_color);
	DDdevice_ClearRectangleDepthColor
                (dde_template_drom_data.device,
                 (DtShort)dde_template_drom_curwindat->actviewxmin,
                 (DtShort)dde_template_drom_curwindat->actviewymin,
                 (DtShort)dde_template_drom_curwindat->actviewxmax,
                 (DtShort)dde_template_drom_curwindat->actviewymax);

	vwidth = (dde_template_drom_curwindat->actviewxmax -
		  dde_template_drom_curwindat->actviewxmin);
	vheight = (dde_template_drom_curwindat->actviewymax -
		   dde_template_drom_curwindat->actviewymin);

	raster_object = DDview_ResizeRaster(dde_template_drom_data.view,
					    vwidth, vheight);

	if (raster_object != (DtObject)0) {

            if (DsInqObjClassId(raster_object) == DsInqClassId("DoRaster")) {
		DsInqRaster (raster_object, &rwidth, &rheight, &rdepth, 
			     &rtype, &rtypestring, &data);

		if (rtype == DcRasterRGB) {
		    /*
		     * we will only deal with RGB rasters and ignore 
		     * all other rasters for backgrounds
		     */

		    /*
		     * compute how the background is aligned to the view
		     */

		    deltax = (vwidth - rwidth);
		    deltay = (vheight - rheight);
		    offsetx = deltax*dde_template_drom_curviwdat->background_just[0];
		    offsety = deltay*(1.0 - dde_template_drom_curviwdat->background_just[1]);

		    if (deltay < 0) {
			/*
			 * the raster is bigger in y than the view
			 * so we offset into the data to get the starting
			 * scanline, and reduce the height of the raster
			 * to be written
			 */

			data = (DtPtr)((int)(data) + (-offsety)*rwidth*3);
			height = rheight + deltay;
			offsety = 0;
		    } else {
			height = rheight;
		    }

		    if (deltax < 0) {
			/*
			 * the raster is bigger in x that the view
			 * so we offset into the data to get the 
			 * starting pixel, and reduce the width of the
			 * raster to be written
			 */

			data = (DtPtr)((int)(data) + (-offsetx)*3);
			width = rwidth + deltax;
			offsetx = 0;
		    } else {
			width = rwidth;
		    }

		    /*
		     * rasters have their origin at the upper left hand
		     * corner of the raster while the view coordinates 
		     * have positive y extending up.  As a result we
		     * must compensate for the difference in coordinate
		     * systems
		     */
		    x = dde_template_drom_curwindat->actviewxmin + offsetx;
		    y = dde_template_drom_curwindat->actviewymax - offsety;

		    for (; height > 0 ; height--, y--) {
			ddr_template_dcm_write_scanline_byte(x, y, width, data);
			data = (DtPtr)((int)(data) + rwidth*3);
		    }
		}
	    }
	}
    }

    /*
     * traverse the display group 
     */

#ifdef DEBUG
    ddr_template_drom_print_studio();
#endif

    (*traverse_display) ();

#ifdef DEBUG
    printf ("end of update\n");
#endif
}
