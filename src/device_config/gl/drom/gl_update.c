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
	ddr_gl_drom_pre_initialization
	ddr_gl_drom_lighting
	ddr_gl_drom_post_initialization
	ddr_gl_drom_update_local_data
	ddr_gl_drom_get_frame_to_view_matrix
	ddr_gl_drom_start_update
	ddr_gl_drom_update_studio
	ddr_gl_drom_update_background
	ddr_gl_drom_update_display

  ======================================================================
 */
#include "../gl_driver.h"
#include <dore/dore_develop/private/rstr.h>



/* ======================================================================== */

void ddr_gl_drom_pre_initialization (void)
{
    static DtFlag initialized = DcFalse;
    Matrix im;

#   ifdef DEBUGP
	printf("Pre_init() called\n");
#   endif

    DevStatus.view_data   = (struct view_data   *)0;
    DevStatus.window_data = (struct window_data *)0;

    DevStatus.visualtype = DcTrueColor;

    DevStatus.cull       = 0;
    DevStatus.cullable   = 0;

    DevStatus.reptype    = 0;

    DevStatus.shadeindex = 0;
    DevStatus.rampmin    = 0;
    DevStatus.rampmax    = 0;

    DevStatus.lighttype     = DcLightInfinite;
    DevStatus.lightcolor[0] = 1.;
    DevStatus.lightcolor[1] = 1.;
    DevStatus.lightcolor[2] = 1.;
    DevStatus.lightintens   = 1.;

    DevStatus.lighting        = 1;
    DevStatus.diffusecolor[0] = 1.;
    DevStatus.diffusecolor[1] = 1.;
    DevStatus.diffusecolor[2] = 1.;
    DevStatus.diffuseintens   = .8;
    DevStatus.diffuseswitch   = 1;
    DevStatus.ambientswitch   = 1;
    DevStatus.ambientintens   = 0.2;

    DevStatus.specularswitch   = 0;
    DevStatus.specularfactor   = 10.;
    DevStatus.specularcolor[0] = 1.;
    DevStatus.specularcolor[1] = 1.;
    DevStatus.specularcolor[2] = 1.;
    DevStatus.specularintens   = 1.;

    DevStatus.transpswitch     = 0;
    DevStatus.transpintens     = 0.5;

    DevStatus.smoothswitch     = 1;

    DevStatus.depcuecolor[0] = 0.;
    DevStatus.depcuecolor[1] = 0.;
    DevStatus.depcuecolor[2] = 0.;
    DevStatus.depcuezfront   = 0.;
    DevStatus.depcuezback    = -1.;
    DevStatus.depcuesfront   = 0.;
    DevStatus.depcuesback    = -1.;
    DevStatus.depcueswitch   = 0;

    DevStatus.map_diffuse_color_switch = DcFalse;
    DevStatus.texture_2D = 0;

    if (!initialized) {
	DevStatus.next_texid = 0;
	initialized = DcTrue;
    }

    /* Done in the studio initialization, but better
    ** safe than hit by a large truck on your way
    ** to your psychotherapist.  */

    mmode(MVIEWING);
    ddr_iris_id_matrix(im);
    loadmatrix(im);

    ddr_gl_drom_lighting();
}



/* ======================================================================== */

void ddr_gl_drom_lighting (void)
{
    float props[40];
    int prop;

    prop = 0;
    props[prop++] = EMISSION;
    props[prop++] = 0.;
    props[prop++] = 0.;
    props[prop++] = 0.;
    props[prop++] = AMBIENT;
    props[prop++] = 0.;
    props[prop++] = 0.;
    props[prop++] = 0.;
    props[prop++] = SPECULAR;
    props[prop++] = 0.;
    props[prop++] = 0.;
    props[prop++] = 0.;
    props[prop++] = SHININESS;
    props[prop++] = 120.;	/* Iris Bug Fix */
    props[prop++] = COLORINDEXES;
    props[prop++] = 0.;
    props[prop++] = 15.;
    props[prop++] = 15.;
    props[prop++] = LMNULL;

    lmdef (DEFMATERIAL, 1, 0, props);

    props[0] = LMNULL;

    lmdef (DEFLIGHT, 1, 0, props);

    prop=0;
    if (getgdesc(GD_LIGHTING_TWOSIDE)) {
	props[prop++] = TWOSIDE;
	props[prop++] = 1.;
    }
    props[prop++] = LOCALVIEWER;
    props[prop++] = 1.0;
    props[prop++] = LMNULL;

    lmdef (DEFLMODEL, 1, 0, props);

    lmbind (MATERIAL, 1);
    lmbind (LMODEL, 1);
}



/* ======================================================================== */

void ddr_gl_drom_post_initialization (void)
{
#   ifdef DEBUGP
	printf("Post_init() called\n");
#   endif
}



/* ======================================================================== */

void ddr_gl_drom_update_local_data (void)      
{
    DtColorModel colormodel;
    DtVolume *viewbounds;

#   ifdef DEBUGP
	printf("Update_local_data() called\n");
#   endif

    ddr_gl_drom_set_att_shdidx(DevStatus.shadeindex);

    viewbounds = DDdevice_InqActualViewport(DevStatus.dore_device);
    DevStatus.window_data->minx = viewbounds->bll[0];
    DevStatus.window_data->miny = viewbounds->bll[1];
    DevStatus.window_data->maxx = viewbounds->fur[0];
    DevStatus.window_data->maxy = viewbounds->fur[1];
#   ifdef DEBUGP
	printf("   ... InqVprt returns: <%f,%f>,<%f,%f>\n",
	    viewbounds->bll[0], viewbounds->bll[1],
	    viewbounds->fur[0], viewbounds->fur[1]);
#   endif

    DevStatus.view_data->clear_flag =
	    DvInqClearFlag(DevStatus.dore_view);
    DevStatus.view_data->backgroundindex = 
	    DvInqShadeIndex(DevStatus.dore_view);
    DvInqBackgroundColor(DevStatus.dore_view, &colormodel,
				 DevStatus.view_data->backgroundRGB);
    DvInqBackgroundJust(DevStatus.dore_view, 
			&(DevStatus.view_data->background_just[0]),
			&(DevStatus.view_data->background_just[1]));

    ddr_gl_drom_get_frame_to_view_matrix();
}



/* ======================================================================== */

void ddr_gl_drom_get_frame_to_view_matrix (void)
{
    DtReal viewtodevscale[3];
    DtReal viewtodevtrans[3];
    DtReal framtodevscale[3];
    DtReal framtodevtrans[3];

    DDdevice_InqFrustumScaleTrans(DevStatus.dore_device,
				  viewtodevscale,viewtodevtrans);
    DDdevice_InqFrameScaleTrans(DevStatus.dore_device,
				framtodevscale, framtodevtrans);

    DevStatus.window_data->frame_to_view_scale[0] = 
	    framtodevscale[0] / viewtodevscale[0];
    DevStatus.window_data->frame_to_view_scale[1] = 
	    framtodevscale[1] / viewtodevscale[1];
    DevStatus.window_data->frame_to_view_scale[2] = 
	    framtodevscale[2] / viewtodevscale[2];

#   ifdef DEBUGP
	printf("  ... View to Device scale: %f,%f,%f\n",
	       viewtodevscale[0],
	       viewtodevscale[1],
	       viewtodevscale[2]);
	printf("  ... View to Device trans: %f,%f,%f\n",
	       viewtodevtrans[0],
	       viewtodevtrans[1],
	       viewtodevtrans[2]);
	printf("  ... Fram to Device scale: %f,%f,%f\n",
	       framtodevscale[0],
	       framtodevscale[1],
	       framtodevscale[2]);
	printf("  ... Fram to Device trans: %f,%f,%f\n",
	       framtodevtrans[0],
	       framtodevtrans[1],
	       framtodevtrans[2]);
	printf("  ... FRAME TO VIEW  SCALE: %f,%f,%f\n",
	       DevStatus.window_data->frame_to_view_scale[0],
	       DevStatus.window_data->frame_to_view_scale[1],
	       DevStatus.window_data->frame_to_view_scale[2]);
#   endif
}



/* ======================================================================== */

void ddr_gl_drom_start_update (
    DtObject device,
    DtObject view,
    DtPtr device_data,
    DtPtr view_data,
    DtPtr window_data)
{
#   ifdef DEBUGP
	printf("Start_update() called...\n");
#   endif

    DevStatus.dore_device = device;
    DevStatus.dore_view   = view;
    DevStatus.view_data   = (struct view_data   *)view_data;
    DevStatus.window_data = (struct window_data *)window_data;
    DevStatus.visualtype = DdInqVisualType (DevStatus.dore_device);
}



/* ======================================================================== */

void ddr_gl_drom_update_studio (DtPFI traverse_studio)
{
    float sc[3];
    float trans[3];
    DtVolume *clipvolume;
    DtVolume viewboundary;
    DtReal xres, yres;

#   ifdef DEBUGP
	printf ("Traversing studio group...\n");
#   endif

    ddr_iris_id_matrix(DevStatus.view_data->world_to_cam_matrix);
    ddr_iris_id_matrix(DevStatus.view_data->projection_matrix);

    mmode(MPROJECTION);
    ddr_iris_loadmatrix_id();
    mmode(MVIEWING);
    ddr_iris_loadmatrix_id();

    DevStatus.view_data->numlights = 0;

    (*traverse_studio)();

    mmode(MPROJECTION);
    ddr_iris_loadmatrix_id();

    /*
     * map from the view volume back to a normalized space
     * ( -w<x<w, -w<y<w, -w<z<0) but this normalized space is just
     * the visible portion of the volume
     * The clipping volume is in frame coordinates, the same coordinates
     * as the view volume
     */

    clipvolume = DDdevice_InqClippingVolume(DevStatus.dore_device);

    sc[0] = 2./(clipvolume->fur[0]-clipvolume->bll[0]);
    sc[1] = 2./(clipvolume->fur[1]-clipvolume->bll[1]);
    sc[2] = 1./(clipvolume->fur[2]-clipvolume->bll[2]);

    trans[0] = -sc[0]*clipvolume->bll[0]-1;
    trans[1] = -sc[1]*clipvolume->bll[1]-1;
    trans[2] = -sc[2]*clipvolume->fur[2];

    translate (trans[0], trans[1], trans[2]);
    scale (sc[0], sc[1], sc[2]);

    /*
     * map from normalized space to the volume of the view
     */

    DvInqBoundary(DevStatus.dore_view, &viewboundary);

    sc[0] = (viewboundary.fur[0]-viewboundary.bll[0])/2.;
    sc[1] = (viewboundary.fur[1]-viewboundary.bll[1])/2.;
    sc[2] = (viewboundary.fur[2]-viewboundary.bll[2]);

    trans[0] = (viewboundary.fur[0]+viewboundary.bll[0])/2.;
    trans[1] = (viewboundary.fur[1]+viewboundary.bll[1])/2.;
    trans[2] = viewboundary.fur[2];

    /*
     * adjust the x scaling if the pixels of the screen are non-square
     */

    DdInqResolution(DevStatus.dore_device, &xres,&yres);
    if (xres != 0.0) {
	sc[0] *= (yres / xres);
    }

    translate (trans[0], trans[1], trans[2]);
    scale(sc[0], sc[1], sc[2]);

    /*
     * pre-concatenate the projection matrix
     * this transforms from camera coordinates to frustum coordinates
     */

    multmatrix(DevStatus.view_data->projection_matrix);
    getmatrix(DevStatus.view_data->cam_to_frustum_matrix);

    mmode(MVIEWING);
    loadmatrix(DevStatus.view_data->world_to_cam_matrix);

    /*
     * This has to be here, because the iris DOES in fact
     * look at the projection matrix when positioning lights.
     * (And it can't just have the identity matrix in the
     *  projection matrix either, or the Z component get's
     *  messed up!)
     */

    ddr_gl_drom_update_display_lights();
}



/* ======================================================================== */

void ddr_gl_drom_update_background (void)
{
    auto   Dt32Bits          *data;
    auto   DtInt              deltax, deltay;
    auto   ddt_gl_drom_rstr  *dynout_rstr;
    static DtInt              filerstr_classid = -1;
    static DtInt              filerstr_dataindex = -1;
    auto   DtReal             ftodscale[3];
    auto   DtReal             ftodtrans[3];
    auto   DtInt              i;
    auto   DtInt              offsetx, offsety;
    auto   DtObjectStructure *raster_object;
    auto   DtInt              rasterid;
    auto   struct rstr       *rstr;
    static DtInt              rstr_classid = -1;
    static DtInt              rstr_dataindex = -1;
    auto   DtVolume           volume;
    auto   DtInt              vwidth, vheight;
    auto   DtInt              width, height;
    auto   int                x1, x2, y1, y2;
    auto   Dt32Bits          *z;
    
    if (DevStatus.view_data->clear_flag)
    {   if (DevStatus.visualtype == DcPseudoColor)
	{   DDdevice_SetShadeIndex
		(DevStatus.dore_device, DevStatus.view_data->backgroundindex);
	}

	DDdevice_SetBackgroundColor
	    (DevStatus.dore_device, DcRGB, DevStatus.view_data->backgroundRGB);

	DDdevice_ClearRectangleDepthColor (DevStatus.dore_device, 0,0, 1,1);

	DDdevice_InqFrameScaleTrans
	    (DevStatus.dore_device, ftodscale, ftodtrans);

	DvInqBoundary (DevStatus.dore_view, &volume);

	vwidth  = (volume.fur[0] - volume.bll[0]) * ftodscale[0];
	vheight = (volume.fur[1] - volume.bll[1]) * ftodscale[1];

	raster_object = DDview_ResizeRaster(DevStatus.dore_view,vwidth,vheight);
	if (raster_object)
	{   if (rstr_classid == -1)
		rstr_classid = DsInqClassId("DoRaster");
	    if (filerstr_classid == -1) 
		filerstr_classid = DsInqClassId("DoFileRaster");

	    rasterid = DsInqObjClassId(raster_object);

	    if (rasterid == filerstr_classid) {
		if (filerstr_dataindex == -1) {
		    for (i=0; dde_gl_drom_class_data[i].name != 0; i++) {
			if (dde_gl_drom_class_data[i].id == filerstr_classid) {
			    filerstr_dataindex =dde_gl_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_gl_drom_filerstr_update (raster_object);

		raster_object = (DtObjectStructure *)
			raster_object->additional_data[filerstr_dataindex];
		if (!raster_object) return;
		rasterid = DsInqObjClassId(raster_object);
	    }

	    if (rasterid == rstr_classid) {
		if (rstr_dataindex == -1) {
		    for (i=0; dde_gl_drom_class_data[i].name != 0; i++) {
			if (dde_gl_drom_class_data[i].id == rstr_classid) {
			    rstr_dataindex = dde_gl_drom_class_data[i].index;
			    break;
			}
		    }
		}

		ddr_gl_drom_rstr_update(raster_object);

		rstr = (struct rstr *)raster_object->data;
		dynout_rstr = (ddt_gl_drom_rstr *)
			raster_object->additional_data[rstr_dataindex];

		if (dynout_rstr->bgd.data == 0) return;

		data = (Dt32Bits*) dynout_rstr->bgd.data;
		z    = (Dt32Bits*) dynout_rstr->bgd.z;

		/* Compute how the background is aligned to the view.  */

		deltax  = (vwidth  - (rstr->width));
		deltay  = (vheight - (rstr->height));
		offsetx = deltax * DevStatus.view_data->background_just[0];
		offsety = deltay * DevStatus.view_data->background_just[1];

		if (deltay < 0) {
		    /* The raster is bigger in Y than the view, so we offset
		    ** into the data to get the starting scanline, and reduce
		    ** the height of the raster to be written. */

		    data -= offsety * rstr->width;
		    if (z != 0)
			z -= offsety * rstr->width;
		    height = (rstr->height) + deltay;
		    offsety = 0;
		} else {
		    height = (rstr->height);
		}

		y1 = volume.bll[1]*ftodscale[1] + ftodtrans[1] + offsety;

		if (y1 < DevStatus.window_data->miny) {
		    /* Starting location outside actual viewport, so offset
		    ** into the data to get the starting scanline, and reduce
		    ** the height of the raster to be written.  */

		    offsety = DevStatus.window_data->miny - y1;
		    data += offsety * rstr->width;
		    if (z != 0) 
			z += offsety * rstr->width;
		    height -= offsety;
		    y1 = DevStatus.window_data->miny;
		}

		if (deltax < 0) {
		    /* The raster is bigger in x than the view so we offset
		    ** into the data to get the starting pixel, and reduce
		    ** the width of the raster to be written.  */

		    data -= offsetx;
		    if (z != 0) 
			z -= offsetx;
				
		    width = (rstr->width) + deltax;
		    offsetx = 0;
		} else {
		    width = (rstr->width);
		}

		x1 = volume.bll[0]*ftodscale[0] + ftodtrans[0] + offsetx;

		if (x1 < DevStatus.window_data->minx) {
		    /* The starting location in x is outside the actual
		    ** viewport so offset into the data to get the starting
		    ** pixel and reduce the width of the raster.  */

		    offsetx = DevStatus.window_data->minx - x1;
		    data += offsetx;
		    if (z != 0) 
			z += offsetx;
		    width -= offsetx;
		    x1 = DevStatus.window_data->minx;
		}

		x2 = x1 + width - 1;
		y2 = y2 + height - 1;

		if (x2 > DevStatus.window_data->maxx) {
		    x2 = DevStatus.window_data->maxx;
		}

		if (y2 > DevStatus.window_data->maxy) {
		    y2 = DevStatus.window_data->maxy;
		}

		pixmode (PM_STRIDE, rstr->width);
		lrectwrite (x1, y1, x2, y2, (unsigned long *)(data));
		if (z != 0) {
		    zbuffer(FALSE);
		    zdraw(TRUE);
		    lrectwrite (x1, y1, x2, y2, (unsigned long *)(z));
		    zdraw(FALSE);
		    zbuffer(TRUE);
		}
		pixmode (PM_STRIDE, 0);
	    }
	}
    }
}



/* ======================================================================== */

void ddr_gl_drom_update_display (DtPFI traverse_display)
{
#   ifdef DEBUGP
	printf("Dynout_update_display called.\n");
#   endif

    if (ddr_gl_dcm_check_device_extent()) {
	DDdevice_SetAbortedDraw(DevStatus.dore_device, DcTrue);
	return;
    }

#   ifdef DEBUGP
	printf("Setting viewport to: <%d,%d>,<%d,%d>\n",
	       DevStatus.window_data->minx,
	       DevStatus.window_data->miny,
	       DevStatus.window_data->maxx,
	       DevStatus.window_data->maxy);
#   endif
    viewport(DevStatus.window_data->minx,
	     DevStatus.window_data->maxx,
	     DevStatus.window_data->miny,
	     DevStatus.window_data->maxy);

    ddr_gl_drom_update_background();
 
    /* The viewing matrix contains the world to camera transformation
    ** because there is the restriction that the projection matrix
    ** can not contain any rotations for inscene light sources to
    ** be displayed correctly.  */

    mmode(MVIEWING);
    loadmatrix(DevStatus.view_data->world_to_cam_matrix);

#   ifdef DEBUGP
	printf("Traversing display group...\n");
#   endif

    /* There is a bug somewhere (it is not known where) that causes
    ** lighting to be really screwed up after frame coordinate geometry
    ** is drawn.  It only affects geometry drawn after the frame coordinate
    ** geometry and only for the current frame.  The work around
    ** for this problem is to set a flag if frame coordinate geometry
    ** is found (and not draw it or change transforms).  If frame coordinate
    ** geometry has been found then perform a second traversal and 
    ** draw only this geometry ignoring all other geometry in the scene.  */

    DevStatus.traversal_number = 1;
    DevStatus.have_frame_coord_prim = DcFalse;
    DevStatus.frame_coord_prim = DcFalse;

    (*traverse_display) ();

    if (DevStatus.have_frame_coord_prim) {
	DevStatus.traversal_number = 2;
	(*traverse_display)();
    }
}
