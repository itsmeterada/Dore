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
 
/*****************************************************************************
**  This file contains functions and routines that govern the initialization
**  of the device driver and update of the studio and display traversal.
*****************************************************************************/

#include "../pex_driver.h"

#define DEBUG 0

    /* The following structure fields are filled only once after PEX
    ** is initialized.  Each field indicates the particular abilities
    ** of the current PEX server.  */

PexStatus pex_status =
{   DcFalse,    /* light_vector */
    DcFalse,    /* light_point */
    DcFalse,    /* light_spot */
    0,    	/* light_max */
    DcFalse,	/* Linetype Dashed */
    DcFalse,	/* Linetype Dotted */
    DcFalse,	/* Linetype DashDot */
    DcFalse,    /* surfinterp_color */
    DcFalse,    /* surfinterp_dot */
    DcFalse,    /* surfinterp_normal */
    DcFalse,    /* zbuffer */
    DcFalse 	/* ready */
};

    /* This structure contains the PEX modes that most closely approximate
    ** an unsupported mode.  For example, if spot lights are not supported
    ** by the current PEX server, but infinite lights are, then the
    ** light_spot field will contain PEXLightWcsVector. */

PexModes pex_modes =
{   PEXLightWCVector,		/* PEX Light Method */
    PEXLightWCPoint,
    PEXLightWCSpot,
    0,				/* Number of Lights Supported */
    PEXLineTypeDashed,		/* PEX Line Type Method */
    PEXLineTypeDotted,
    PEXLineTypeDashDot,
    PEXSurfaceInterpColor,	/* Surface Interpolation Method */
    PEXSurfaceInterpDotProduct,
    PEXSurfaceInterpNormal
};

static void GetPexStatus (void);



/*****************************************************************************
**  The first pass of the Dynamic Renderer Output Module initialization.
*****************************************************************************/

void ddr_pex_drom_pre_initialization (void)
{
#   if DEBUG_CALL
	print ("ddr_pex_drom_pre_initialization ()\n");
	fflush (stdout);
#   endif

    DROM.matrix_stack_size = 0;
    DROM.matrix_stack_top  = 0;
    DROM.matrix_stack      = 0;
    DROM.device            = 0;
    DROM.view              = 0;

    DROM.light_heap_size   = 0;
    DROM.light_heap_next   = 0;
    DROM.light_heap        = 0;

    DROM.pexview.clip_flags   = PEXClipXY | PEXClipFront | PEXClipBack;
    DROM.pexview.clip_limits.min.x = 0;
    DROM.pexview.clip_limits.min.y = 0;
    DROM.pexview.clip_limits.min.z = 0;
    DROM.pexview.clip_limits.max.x = 1;
    DROM.pexview.clip_limits.max.y = 1;
    DROM.pexview.clip_limits.max.z = 1;

    DROM.modclip_switch    = 0;
    DROM.modclip_list      = 0;

    PEX.pc_value_mask[0]  = 0;
    PEX.pc_value_mask[1]  = 0;
    PEX.pc_value_mask[2]  = 0;

    dor_matrix_load (DROM.M_local_world, Identity4x4);
    dor_matrix_load (DROM.M_local_frustum, Identity4x4);

    dde_pex_drom_pre_init = DcTrue;
}



/*****************************************************************************
**  The last pass of the Dynamic Renderer Output Module initialization.
*****************************************************************************/

void ddr_pex_drom_post_initialization (void)
{
    auto PEXRendererAttributes rendattrs;	/* PEX Renderer Attributes */
    auto unsigned long         rendflags;	/* PEX Renderer Attr Flags */

#   if DEBUG_CALL
	print ("ddr_pex_drom_post_initialization ()\n");
	fflush (stdout);
#   endif

#   if DEBUG
	printf ("  dde_pex_current_dcm_data = %lx\n",dde_pex_current_dcm_data);
	printf ("  display %lx, screen %lx, window %lx\n",
	    dde_pex_current_dcm_data->display,
	    dde_pex_current_dcm_data->screen,
	    dde_pex_current_dcm_data->window
	);
	printf ("  DROM data size is %d bytes.\n", sizeof(DROM));
#   endif

    dde_pex_drom_pre_init = DcFalse;
    rendflags = 0;

    /* Set up illumination components (reflection attributes in PEX-speak). */

    PEX.pc_attrs.reflection_attr.ambient =
		DROM.ambient_switch  ? DROM.ambient_intens  : 0;
    PEX.pc_attrs.reflection_attr.diffuse =
		DROM.diffuse_switch  ? DROM.diffuse_intens  : 0;
    PEX.pc_attrs.reflection_attr.specular =
		DROM.specular_switch ? DROM.specular_intens : 0;
    PEX.pc_attrs.reflection_attr.transmission =
		DROM.transp_switch   ? DROM.transp_intens   : 0;

    PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCReflectionAttr);

    /* Set the culling mode.  PEX supports front face culling as well as back
    ** face culling, but Dore' supports only back face culling. */

    PEX.pc_attrs.culling_mode =
	(DROM.bfcull_switch && DROM.bfcullable) ? PEXBackFaces : 0;
    PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCCullingMode);

    /* Set other miscellaneous values. */

    PEX.pc_attrs.distinguish = False;
    PEXSetPCAttributeMask (PEX.pc_value_mask, PEXPCDistinguishFlag);

    /* Create the PEX pipeline context.  The pipeline context is the set
    ** of default values that PEX refers to at the beginning of every
    ** rendering pass.  */

    rendattrs.pipeline_context = PEXCreatePipelineContext
				 (DCM->display,PEX.pc_value_mask,&PEX.pc_attrs);
    rendflags |= PEXRAPipelineContext;

    /* Create the PEX view lookup table. */

    DROM.pexview_LUT = PEXCreateLookupTable
		       (DCM->display, DCM->window, PEXLUTView);
    rendattrs.view_table = DROM.pexview_LUT;
    rendflags |= PEXRAViewTable;

    /* Create the PEX light lookup table. */

    DROM.light_LUT = PEXCreateLookupTable
		     (DCM->display, DCM->window, PEXLUTLight);
    rendattrs.light_table = DROM.light_LUT;
    rendflags |= PEXRALightTable;

    /* PEX Depth-Cue Lookup Table */

    DROM.depthcue_LUT = PEXCreateLookupTable
			(DCM->display, DCM->window, PEXLUTDepthCue);
    rendattrs.depth_cue_table = DROM.depthcue_LUT;
    rendflags |= PEXRADepthCueTable;

    rendattrs.hlhsr_mode = PEXHLHSRZBuffer;
    rendflags |= PEXRAHLHSRMode;

    /* Set up the background color and double-buffering attributes. */

    rendattrs.clear_image = 1;
    rendattrs.background_color.type = PEXColorTypeRGB;
    rendattrs.background_color.value.rgb.red   = DCM->bg_color[0];
    rendattrs.background_color.value.rgb.green = DCM->bg_color[1];
    rendattrs.background_color.value.rgb.blue  = DCM->bg_color[2];
    rendflags |= PEXRAClearImage | PEXRABackgroundColor;

    /* If we're doing pseudo-color, then set up the color approximation
    ** lookup table. */

    if (DCM->visualtype == DcPseudoColor)
    {
	DROM.color_approx_LUT = PEXCreateLookupTable
				(DCM->display, DCM->window, PEXLUTColorApprox);

	if (DdInqShadeMode(DCM->device) == DcRange)
	{   auto DtInt shademin, shademax;	/* Shade Range Bounds */

	    DDdevice_InqShadeRange
		(DCM->device, DCM->shadeindex, &shademin, &shademax);

	    DROM.color_approx.type = PEXColorRange;
	    DROM.color_approx.model = PEXRenderingColorModelRGB;
	    DROM.color_approx.weight1 = 0.298;
	    DROM.color_approx.weight2 = 0.587;
	    DROM.color_approx.weight3 = 0.114;
	    DROM.color_approx.max1 = shademax - shademin;
	    DROM.color_approx.base_pixel = shademin;
	    DROM.color_approx.dither = 1;
	}
	else
	{   DROM.color_approx.type = PEXColorSpace;
	    DROM.color_approx.model = PEXRenderingColorModelRGB;
	    DROM.color_approx.max1 = 7;
	    DROM.color_approx.max2 = 7;
	    DROM.color_approx.max3 = 3;
	    DROM.color_approx.mult1 = 0x20;
	    DROM.color_approx.mult2 = 0x04;
	    DROM.color_approx.mult3 = 0x01;
	    DROM.color_approx.base_pixel = 0;
	    DROM.color_approx.dither = 1;
	}

	rendattrs.color_approx_table = DROM.color_approx_LUT;
	rendflags |= PEXRAColorApproxTable;

	PEXSetTableEntries
	(   DCM->display,
	    DROM.color_approx_LUT, 0, 1, PEXLUTColorApprox,
	    &(DROM.color_approx)
	);
    }

    /* Set up the renderer attributes. */

#if 0
{
auto DtVolume *viewport;
/*viewport = DDdevice_InqActualViewport (DROM.device);*/
printf ("----- device %lx\n", DROM.device);
/*printf ("----- viewport %lx\n", viewport);*/
if (0 /*viewport*/)
    {
    printf ("----- bll = %g %g %g\n",
	viewport->bll[0], viewport->bll[1], viewport->bll[2]);
    printf ("----- fur = %g %g %g\n",
	viewport->fur[0], viewport->fur[1], viewport->fur[2]);
    }
rendattrs.viewport.min.x =   0;
rendattrs.viewport.min.y = 100;
rendattrs.viewport.min.z = 1.0;
rendattrs.viewport.max.x = 500;
rendattrs.viewport.max.y = 255;
rendattrs.viewport.max.y = 0.0;
rendattrs.viewport.use_drawable = 0;
rendflags |= PEXRAViewport;
}
#endif

    PEX.renderer = PEXCreateRenderer
		   (DCM->display, DCM->window, rendflags, &rendattrs);

    /* Get the PEX server status. */

    if (!pex_status.ready) GetPexStatus ();
}



/*****************************************************************************
**  The renderer calls this function at the beginning of each update of a
**  device/view combination.  `device' is the handle to the device object
**  being updated and `view' is the handle to the view object.  `device_data',
**  `view_data' and `window_data' are pointers to the data previously created
**  by calls to _create_local_device_data(), _create_local_view_data(), and
**  _create_local_window_data().
*****************************************************************************/

void  ddr_pex_drom_start_update  (
    DtObject device,		/* Currently Edited Device Object */
    DtObject view,		/* Handle to the View Object */
    DtPtr    device_data,	/* Created in drom.create_local_device_data() */
    DtPtr    view_data,		/* Created in drom.create_local_view_data() */
    DtPtr    window_data)	/* Created in drom.create_local_window_data() */
{
    auto PEXRendererAttributes rendattrs;	/* PEX Renderer Attributes */
    auto unsigned long         rendflags;	/* PEX Renderer Attr Flags */

#   if DEBUG_CALL
	printf ("ddr_pex_drom_start_update (%lx, %lx, %lx, %lx, %lx)\n",
	    device, view, device_data, view_data, window_data);
	fflush (stdout);
#   endif

    DROM.device = device;
    DROM.view = view;

    DEVICE = device_data;
    VIEW   = view_data;
    WINDOW = window_data;

    /*  Reset the background color here because it may have changed since we
    **  set up the graphics pipeline, and also because the renderer is created
    **  before the background color is set initially.  */

    rendattrs.background_color.type = PEXColorTypeRGB;
    rendattrs.background_color.value.rgb.red   = DCM->bg_color[0];
    rendattrs.background_color.value.rgb.green = DCM->bg_color[1];
    rendattrs.background_color.value.rgb.blue  = DCM->bg_color[2];
    rendflags = PEXRABackgroundColor;

    PEXChangeRenderer (DCM->display, PEX.renderer, rendflags, &rendattrs);

#ifdef PIXMAP
    if (!DCM->singlebuffered)
    {   XFillRectangle
	(DCM->display, DCM->pixmap, DCM->gc, 0, 0, DCM->width, DCM->height);
    }

    PEXBeginRendering
    (   DCM->display,
	(DCM->singlebuffered ? DCM->window : DCM->pixmap),
	PEX.renderer
    );
#endif

    PEXBeginRendering (DCM->display, DCM->dispbuff, PEX.renderer);
}



/*****************************************************************************
**  The renderer calls this function to begin the actual generation of the
**  output image seen on the device.  This routine calls *(traverse_display)()
**  to initiate a traversal of the display group.  As the objects of the
**  display group are executed during this traversal, they will call DROM
**  functions with information about attribute changes and geometry to be
**  rendered.
*****************************************************************************/

void  ddr_pex_drom_update_display  (DtPFI traverse_display)
{
#   if DEBUG_CALL
	printf ("ddr_pex_drom_update_display (%lx)\n", traverse_display);
	fflush (stdout);
#   endif

    /* Turn on the lights. */

    UpdateLightState ();

    PEXSetMarkerType (DCM->display, PEX.renderer, PEXOCRender, PEXMarkerDot);

    /* Set up the depth-cueing. */

    PEXSetTableEntries
    (   DCM->display,
	DROM.depthcue_LUT, 0, 1, PEXLUTDepthCue,
	&(DROM.depthcue)
    );

    /* Traverse the display group. */

#   if DEBUG
	print ("+ Printing Studio Attributes\n");
	PrintStudio();
#   endif

    (*traverse_display) ();

    /* Flush the PEX commands and end the PEX rendering. */

    PEXEndRendering (DCM->display, PEX.renderer, True);
    XFlush (DCM->display);

#   if DEBUG
	print ("+ end of update.\n");
#   endif
}



/*****************************************************************************
**  The renderer calls this function to notify the driver that the local data
**  of the DROM may be out of date with respect to the Dore' kernel.  This
**  function will be called after drom.start_update.  It updates local state,
**  such as background color, by querying the Dore' kernel.
*****************************************************************************/

void ddr_pex_drom_update_local_data (void)
{
    auto DtVolume *actviewport;

#   if DEBUG_CALL
	print ("ddr_pex_drom_update_local_data ()\n");
	fflush (stdout);
#   endif

    actviewport = DDdevice_InqActualViewport (DROM.device);

    WINDOW->actviewxmin = actviewport->bll[0];
    WINDOW->actviewymin = actviewport->bll[1];
    WINDOW->actviewxmax = actviewport->fur[0];
    WINDOW->actviewymax = actviewport->fur[1];

    DDdevice_InqFrustumScaleTrans
	(DROM.device, DEVICE->ctodscale, DEVICE->ctodtrans);

    DDdevice_InqFrameScaleTrans
	(DROM.device, DEVICE->ftodscale, DEVICE->ftodtrans);

    DEVICE->annoscale[0] = DEVICE->ftodscale[0] / DEVICE->ctodscale[0];
    DEVICE->annoscale[1] = DEVICE->ftodscale[1] / DEVICE->ctodscale[1];
    DEVICE->annoscale[2] = DEVICE->ftodscale[2] / DEVICE->ctodscale[2];

    /*VIEW->clear_flag = DvInqClearFlag (DROM.view);*/

    DvInqBackgroundColor
	(DROM.view, &VIEW->background_color_model, VIEW->background_color);
    DDdevice_SetBackgroundColor (DROM.device, DcRGB, VIEW->background_color);
    DvInqBackgroundJust
	(DROM.view, &VIEW->background_just[0], &VIEW->background_just[1]);

    DEVICE->shade_mode  = DdInqShadeMode (DROM.device);
    DEVICE->visual_type = DdInqVisualType (DROM.device);
}



/*****************************************************************************
**  The renderer calls this routine each time the studio environment needs to
**  be updated.  This function initializes the studio state and then calls
**  (*traverse_studio)() to initiate a traversal of the studio group.  As the
**  objects of the studio group are executed during this traversal, they call
**  DROM functions with information about attribute changes and new cameras
**  and lights.
*****************************************************************************/

void  ddr_pex_drom_update_studio  (DtPFI traverse_studio)
{
    auto   DtVolume *clipvolume;
    auto   DtReal    scale[3];
    auto   DtReal    trans[3];
    auto   DtVolume  viewboundary;
    auto   DtReal    xres, yres;

#   if DEBUG_CALL
	printf ("ddr_pex_drom_update_studio (%lx)\n", traverse_studio);
	fflush (stdout);
#   endif

    /* Before traversing the studio, clear out the light list.  Light objects
    ** will be created again during the studio traversal. */

    DROM.light_heap_next = 0;
    DROM.light_index_pex = 1;

    /* Set the default lighting values. */

    DROM.newlight.dore_type = DcLightInfinite;
    DROM.newlight.intensity = 1.0;
    DROM.newlight.status    = DcOn;

    DROM.newlight.pex_entry.direction.x   = 0;
    DROM.newlight.pex_entry.direction.y   = 0;
    DROM.newlight.pex_entry.direction.z   = -1;
    DROM.newlight.pex_entry.point.x       = 0;
    DROM.newlight.pex_entry.point.y       = 0;
    DROM.newlight.pex_entry.point.z       = 0;
    DROM.newlight.pex_entry.concentration = 1;
    DROM.newlight.pex_entry.spread_angle  = M_PI_4;
    DROM.newlight.pex_entry.attenuation1  = 1;
    DROM.newlight.pex_entry.attenuation2  = 1;
    DROM.newlight.pex_entry.color.type    = PEXColorTypeRGB;
    DROM.newlight.pex_entry.color.value.rgb.red   = 1;
    DROM.newlight.pex_entry.color.value.rgb.green = 1;
    DROM.newlight.pex_entry.color.value.rgb.blue  = 1;

    /* Remove the previous light entries from the light lookup-table. */

    PEXDeleteTableEntries
	(DCM->display, DROM.light_LUT, 1, DROM.light_heap_next);

    (*traverse_studio)();	/* Traverse the studio group. */

    /* Compute the world coordinate to camera coordinate transformation
    ** matrix and load it into the PEX viewing orientation matrix.  */

    dor_matrix_load   (DROM.M_world_camera, DROM.M_camera_world);
    dor_matrix_invert (DROM.M_world_camera);
    MATRIX_LOAD (DROM.pexview.orientation, DROM.M_world_camera);
    MATRIX_LOAD (DROM.M_local_frustum, DROM.M_world_camera);

#   if DEBUG
	printf ("Orientation Matrix:\n");
	PrintMatrix (DROM.M_world_camera);
	fflush (stdout);
#   endif

    /* Map from camera coordinates to frustum coordinates where the camera
    ** is at the origin looking down the -Z axis (Y is up, X is to the right).
    */

    dor_matrix_load (DROM.M_camera_frustum, DROM.M_projection);

#   if DEBUG
	printf ("\nInitial Camera->Frustum Matrix:\n");
	PrintMatrix (DROM.M_camera_frustum);
#   endif

    /* Map from normalized space to the volume of the view. */

    if (DROM.camera_type != DcCameraArbitrary)
    {   DvInqBoundary (DROM.view, &viewboundary);

#       if DEBUG
	    printf ("viewboundary.bll  <%#f %#f %#f>\n",
		veclist(viewboundary.bll));
	    printf ("viewboundary.fur  <%#f %#f %#f>\n",
		veclist(viewboundary.fur));
#       endif

	scale[0] = (viewboundary.fur[0] - viewboundary.bll[0]) / 2;
	scale[1] = (viewboundary.fur[1] - viewboundary.bll[1]) / 2;
	scale[2] = (viewboundary.fur[2] - viewboundary.bll[2]);

	trans[0] = (viewboundary.fur[0] + viewboundary.bll[0]) / 2;
	trans[1] = (viewboundary.fur[1] + viewboundary.bll[1]) / 2;
	trans[2] = viewboundary.fur[2];

	/* Adjust the x scaling if the pixels of the screen are non-square. */

	DdInqResolution (DROM.device, &xres, &yres);
	if (xres != 0.0)
	    scale[0] *= (yres / xres);

	dor_matrix_scale (DROM.M_camera_frustum, scale[0], scale[1], scale[2],
			  DcPostConcatenate);

	dor_matrix_translate (DROM.M_camera_frustum,
			      trans[0], trans[1], trans[2], DcPostConcatenate);
    }

#   if DEBUG
	printf ("\tscale <%#f %#f %#f> + trans <%#f %#f %#f>\n",
	    veclist(scale), veclist(trans));
	printf ("\nMapped to the Viewing Frustum:\n");
	PrintMatrix (DROM.M_camera_frustum);
#   endif

    /* Map from the view volume to normalized space (the way that PEX thinks
    ** of normalized space, not Dore').  Dore' normalized spatial coordinates
    ** are [-w,w] x [-w,w] x [-w,0], but PEX normalized spatial coordinates
    ** are [0,1] x [0,1] x [0,1].  The clipping volume is in frame coordinates,
    ** the same coordinates as the view volume.  */

    clipvolume = DDdevice_InqClippingVolume (DROM.device);

#   if DEBUG
	printf ("Clipvolume->bll <%#f %#f %#f>\n", veclist(clipvolume->bll));
	printf ("Clipvolume->fur <%#f %#f %#f>\n", veclist(clipvolume->fur));
#   endif

    trans[0] = (-scale[0] * clipvolume->bll[0]);
    trans[1] = (-scale[1] * clipvolume->bll[1]);
    trans[2] = (-scale[2] * clipvolume->fur[2]) + 1;
    scale[0] = 1 / (clipvolume->fur[0] - clipvolume->bll[0]);
    scale[1] = 1 / (clipvolume->fur[1] - clipvolume->bll[1]);
    scale[2] = 1 / (clipvolume->fur[2] - clipvolume->bll[2]);

    dor_matrix_translate
	(DROM.M_camera_frustum, trans[0],trans[1],trans[2], DcPostConcatenate);
    dor_matrix_scale
	(DROM.M_camera_frustum, scale[0],scale[1],scale[2], DcPostConcatenate);

#   if DEBUG
	printf ("\nMapped Back to Normalized Space:\n");
	printf ("\ttrans <%#f %#f %#f>, scale <%#f %#f %#f>\n",
	    veclist(trans), veclist(scale));
	PrintMatrix (DROM.M_camera_frustum);
#   endif

    /* Set up the local to frustum matrix by initially loading it with the
    ** world to frustum matrix.  Local transformations will later be
    ** preconcatenated to this matrix.  */

    dor_matrix_load (DROM.M_world_frustum, DROM.M_world_camera);
    dor_matrix_concat
	(DROM.M_world_frustum, DROM.M_camera_frustum, DcPostConcatenate);

    dor_matrix_load (DROM.M_local_frustum, DROM.M_local_world);
    dor_matrix_concat
	(DROM.M_local_frustum, DROM.M_world_frustum, DcPostConcatenate);

    /* Load the Dore' projection matrix into the PEX projection matrix. */

    MATRIX_LOAD (DROM.pexview.mapping, DROM.M_camera_frustum);

    /* Set the viewing table entries. */

    PEXSetTableEntries
	(DCM->display, DROM.pexview_LUT, 0, 1, PEXLUTView, &(DROM.pexview));

#   if DEBUG
    {
	auto PEXViewEntry *ventry;
	auto int           status_return;
	auto int           table_type;

	ventry = PEXGetTableEntry
		 (   DCM->display,
		     DROM.pexview_LUT, 0, PEXRealizedValue,
		     &status_return,
		     &table_type
		 );
	printf ("Viewing LUT:\n");
	printf ("  clipFlags:  %lX\n", ventry->clipFlags);
	printf (" clipLimits:  <%f %f %f> <%f %f %f>\n",
	    ventry->clipLimits.minval.x, ventry->clipLimits.minval.y,
	    ventry->clipLimits.minval.z, ventry->clipLimits.maxval.x,
	    ventry->clipLimits.maxval.y, ventry->clipLimits.maxval.z);
	printf ("orientation:  %f %f %f %f\n",vec4list(ventry->orientation[0]));
        printf ("              %f %f %f %f\n",vec4list(ventry->orientation[1]));
        printf ("              %f %f %f %f\n",vec4list(ventry->orientation[2]));
        printf ("              %f %f %f %f\n",vec4list(ventry->orientation[3]));
	printf ("    mapping:  %f %f %f %f\n",vec4list(ventry->mapping[0]));
        printf ("              %f %f %f %f\n",vec4list(ventry->mapping[1]));
        printf ("              %f %f %f %f\n",vec4list(ventry->mapping[2]));
        printf ("              %f %f %f %f\n",vec4list(ventry->mapping[3]));

	PEXFreeTableEntries (PEXLUTView, 1, ventry);
    }
#   endif
}



/*****************************************************************************
**  This procedure queries the PEX server to see what rendering attributes
**  are supported, and to get other miscellaneous information (like the 
**  maximum number of lights).  This function is only called once for this
**  DROM.
*****************************************************************************/

static void GetPexStatus (void)
{
    auto     int              enumindex;	 /* Enum Info Array Index */
    auto     PEXEnumTypeDesc *enuminfo;	         /* Enum Info Array */
    auto     unsigned long   *enumcount;	 /* # Enums per Type */
    register int              ii;		 /* Loop Index */
    auto     PEXTableInfo     light_table_info;  /* Light LUT Info */
    auto     int              type;		 /* Info Type */

#   define NUM_TYPES  4				 /* Number of Info Types */

    static int typelist [NUM_TYPES] =		 /* List of Info Types */
    {   PEXETHLHSRMode,
	PEXETSurfaceInterpMethod,
	PEXETLightType,
	PEXETLineType
    };

    /* Get the number of definable PEX light entries. */

    PEXGetTableInfo (DCM->display, DCM->window, PEXLUTLight, &light_table_info);
    pex_status.light_max = light_table_info.definable_entries;

    /*  Now get information about supported attributes.  The information is
    **  returned in a rather cumbersome form.  For every enum type requested in
    **  `typelist', there are some number of entries packed into the `enuminfo'
    **  array.  The `enumcount' array is a list of the number of enum entries
    **  for each type.  For example:
    **
    **        typelist      enumcount
    **
    **      +-----------+      +---+    This means that there is 1 entry
    **    0 | HlhsrMode |    0 | 1 |    given for the HlhsrMode type, 3
    **      +-----------+      +---+    entries for the surface interpolation
    **    1 | SurfInterp|    1 | 3 |    type, and 2 entries for the light
    **      +-----------+      +---+    type.  Thus, the size of the enuminfo
    **    2 | LightType |    2 | 2 |    array is 6.  Ugly, ain't it?
    **      +-----------+      +---+
    **
    **  To process this array, we need to roll through, a part at a time, the
    **  appropriate sections of the enum array.  We can't just do the whole
    **  thing at once (e.g. if PEXHLHSRZBuffer, set the zbuffer flag), since
    **  the index values overlap (e.g. PEXHLHSRZBuffer==PEXLightWCVector==2).
    */

    PEXGetEnumTypeInfo
    (   DCM->display, DCM->window,
	NUM_TYPES,
	typelist,
	PEXETIndex,
	&enumcount, &enuminfo
    );

    for (enumindex=0, type=0;  type < NUM_TYPES;  ++type)
    {
	for (ii=0;  ii < enumcount[type];  ++ii, ++enumindex)
	{
	    switch (typelist[type])
	    {
		case PEXETHLHSRMode:
		{   if (enuminfo[enumindex].index == PEXHLHSRZBuffer)
			pex_status.zbuffer = 1;
		    break;
		}

		case PEXETSurfaceInterpMethod:
		{   switch (enuminfo[enumindex].index)
		    {   case PEXSurfaceInterpColor:
			    pex_status.surfinterp_color = 1;  break;
			case PEXSurfaceInterpDotProduct:
			    pex_status.surfinterp_dot = 1;    break;
			case PEXSurfaceInterpNormal:
			    pex_status.surfinterp_normal = 1; break;
		    }
		    break;
		}

		case PEXETLightType:
		{   switch (enuminfo[enumindex].index)
		    {   case PEXLightWCVector:
			    pex_status.light_vector = 1; break;
			case PEXLightWCPoint:
			    pex_status.light_point = 1;  break;
			case PEXLightWCSpot:
			    pex_status.light_spot = 1;   break;
		    }
		    break;
		}

		case PEXETLineType:
		{   switch (enuminfo[enumindex].index)
		    {   case PEXLineTypeDashed:
			    pex_status.line_dashed  = 1;  break;
			case PEXLineTypeDotted:
			    pex_status.line_dotted  = 1;  break;
			case PEXLineTypeDashDot:
			    pex_status.line_dashdot = 1;  break;
		    }
		    break;
		}
	    }
	}
    }

    PEXFreeEnumInfo ((unsigned long)(NUM_TYPES), enumcount, enuminfo);

    pex_status.ready = 1;

#   if DEBUG
	printf ("PEX Status:\n");
        printf ("\tlight_vector      %d\n", pex_status.light_vector);
        printf ("\tlight_point       %d\n", pex_status.light_point);
        printf ("\tlight_spot        %d\n", pex_status.light_spot);
        printf ("\tlight_max         %d\n", pex_status.light_max);
	printf ("\tline_dashed       %d\n", pex_status.line_dashed);
	printf ("\tline_dotted       %d\n", pex_status.line_dotted);
	printf ("\tline_dashdot      %d\n", pex_status.line_dashdot);
        printf ("\tsurfinterp_color  %d\n", pex_status.surfinterp_color);
        printf ("\tsurfinterp_dot    %d\n", pex_status.surfinterp_dot);
        printf ("\tsurfinterp_normal %d\n", pex_status.surfinterp_normal);
        printf ("\tzbuffer           %d\n", pex_status.zbuffer);
#   endif

    /* Fill in the pex_modes structure.  This structure contains the nearest
    ** supported PEX method for a requested Dore' method.  */

    pex_modes.light_vector = (pex_status.light_vector)
			   ? PEXLightWCVector : PEXLightAmbient;
    pex_modes.light_point  = (pex_status.light_point)
			   ? PEXLightWCPoint  : pex_modes.light_vector;
    pex_modes.light_spot   = (pex_status.light_spot)
			   ? PEXLightWCSpot   : pex_modes.light_vector;

    pex_modes.line_dashed  = (pex_status.line_dashed)
			   ? PEXLineTypeDashed  : PEXLineTypeSolid;
    pex_modes.line_dotted  = (pex_status.line_dotted)
			   ? PEXLineTypeDotted  : pex_modes.line_dashed;
    pex_modes.line_dashdot = (pex_status.line_dashdot)
			   ? PEXLineTypeDashDot : pex_modes.line_dashed;

    pex_modes.surfinterp_color  = (pex_status.surfinterp_color)
			        ? PEXSurfaceInterpColor
				: PEXSurfaceInterpNone;

    pex_modes.surfinterp_dot    = (pex_status.surfinterp_dot)
			        ? PEXSurfaceInterpDotProduct
			        : pex_modes.surfinterp_color;

    pex_modes.surfinterp_normal = (pex_status.surfinterp_normal)
				? PEXSurfaceInterpNormal
				: pex_modes.surfinterp_dot;
}



/*****************************************************************************
**  This function changes the setttings of the PEX color-range pseudo color
**  settings.
*****************************************************************************/

void ddr_pex_drom_update_shade_index (void)
{
    auto DtInt shademin, shademax;	/* Shade Range Bounds */

#   if DEBUG_CALL
	print ("ddr_pex_drom_update_shade_index ()\n");
	fflush (stdout);
#   endif

    if (dde_pex_drom_pre_init) return;

    DDdevice_InqShadeRange
	(DCM->device, DCM->shadeindex, &shademin, &shademax);

    DROM.color_approx.type = PEXColorRange;
    DROM.color_approx.model = PEXRenderingColorModelRGB;
    DROM.color_approx.weight1 = 0.298;
    DROM.color_approx.weight2 = 0.587;
    DROM.color_approx.weight3 = 0.114;
    DROM.color_approx.max1 = shademax - shademin;
    DROM.color_approx.base_pixel = shademin;
    DROM.color_approx.dither = 1;

    PEXSetTableEntries
    (   DCM->display, DROM.color_approx_LUT, 0, 1, PEXLUTColorApprox,
	&(DROM.color_approx)
    );
}
