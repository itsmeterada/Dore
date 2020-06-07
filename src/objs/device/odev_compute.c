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
	void dor_device_compute_actual_viewport (device)
	void dor_device_compute_clip_volume (device,view_object)
	void dor_device_compute_frame_trans (device)
	void dor_device_compute_pick_volume (device,view_object)
	void dor_device_resize (device_object)
	void dor_device_viewport_size (device_object)

  ======================================================================
 */
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/device.h>
/*
 ======================================================================
 */
void dor_device_compute_actual_viewport (dot_device *device)
{
    DtVolume *clipvol;
    DtVolume *viewport;
    DtReal *scale,*translate;

    viewport  = &(device->actual_viewport);
    clipvol   = &(device->clipping_volume);

    /* compute actual device viewport (image of clip volume on device's view 
       surface) */

    scale = device->ftodscale;
    translate = device->ftodtranslate;

    viewport->bll[0] = clipvol->bll[0]*scale[0]+translate[0];
    viewport->bll[1] = clipvol->bll[1]*scale[1]+translate[1];
    viewport->bll[2] = clipvol->bll[2]*scale[2]+translate[2];

    viewport->fur[0] = clipvol->fur[0]*scale[0]+translate[0];
    viewport->fur[1] = clipvol->fur[1]*scale[1]+translate[1];
    viewport->fur[2] = clipvol->fur[2]*scale[2]+translate[2];

    /* compute scale and translation coefficients to map from
       cannonical clipping volume to device viewport */

    scale = device->ctodscale;
    translate = device->ctodtranslate;

    scale[0] = (viewport->fur[0] - viewport->bll[0])/2.;
    scale[1] = (viewport->fur[1] - viewport->bll[1])/2.;
    scale[2] = viewport->fur[2] - viewport->bll[2];
    translate[0] = (viewport->fur[0] + viewport->bll[0])/2.;
    translate[1] = (viewport->fur[1] + viewport->bll[1])/2.;
    translate[2] = viewport->fur[2];	
}
/*
 ======================================================================
 */
void dor_device_compute_clip_volume (
    dot_device *device,
    dot_object *view_object)
{
    DtVolume *projvprt, *clipvol, *window;

    projvprt = dor_view_inq_boundary_ptr(view_object);
    window = dor_frame_inq_boundary_ptr(device->frame);
    clipvol  = &(device->clipping_volume);

    clipvol->bll[0] = max(projvprt->bll[0],window->bll[0]); 
    clipvol->bll[1] = max(projvprt->bll[1],window->bll[1]);
    clipvol->bll[2] = max(projvprt->bll[2],window->bll[2]);

    clipvol->fur[0] = min(projvprt->fur[0],window->fur[0]);
    clipvol->fur[1] = min(projvprt->fur[1],window->fur[1]);
    clipvol->fur[2] = min(projvprt->fur[2],window->fur[2]);

    device->all_clipped =
	    ((clipvol->bll[0] >= clipvol->fur[0]) ||
	     (clipvol->bll[1] >= clipvol->fur[1]) ||
	     (clipvol->bll[2] >= clipvol->fur[2]));
}
/*
 ======================================================================
 */
void dor_device_compute_frame_trans (dot_device *device) 
{
    DtReal scale,xscale,yscale;
    DtReal xtrans,ytrans;
    DtVolume *window,*viewport,*extent;
    DtReal left,bottom;
    DtReal dx,dy;

    window = dor_frame_inq_boundary_ptr(device->frame);
    dor_frame_inq_justification(device->frame,&left,&bottom);
    viewport = &(device->viewport);
    extent = &(device->extent);

    dx = viewport->fur[0]-viewport->bll[0];
    dy = viewport->fur[1]-viewport->bll[1];

    xscale = dx/(window->fur[0]-window->bll[0]);

    yscale = dy/(window->fur[1]-window->bll[1]);

    scale  = min(xscale,yscale);

    if (xscale > yscale){
	xtrans = -window->bll[0]*scale+(1-yscale/xscale)*dx*left+
		viewport->bll[0];
	ytrans = -window->bll[1]*scale+viewport->bll[1];
    }
    else{
	xtrans = -window->bll[0]*scale+viewport->bll[0];
	ytrans = -window->bll[1]*scale+
			(1-xscale/yscale)*dy*bottom+
			viewport->bll[1];
    }

    device->ftodscale[0] = scale;
    device->ftodscale[1] = scale;
    device->ftodscale[2] = (extent->fur[2]-extent->bll[2]) /
	    (window->fur[2]-window->bll[2]);

    device->ftodtranslate[0] = xtrans;
    device->ftodtranslate[1] = ytrans;
    device->ftodtranslate[2] = -window->bll[2]*(device->ftodscale[2])
				 + extent->bll[2];
}
/*
 ======================================================================
 */
void dor_device_compute_pick_volume (
    dot_device *device,
    dot_object *view_object)
{
    DtReal pickpoint[3];
    DtVolume *projvprt,*clipvol,*window;
    DtSize3 *pckapp;
    DtVolume pick_volume;
    DtReal *scale,*translate;

    pckapp = &(device->pick_aperture);
    scale = device->ftodscale;
    translate = device->ftodtranslate;

#ifdef DEBUG
    printf ("ftodscale (%g %g %g)\n", 
		scale[0], scale[1], scale[2]);
    printf ("ftodtrans (%g %g %g)\n", 
		translate[0], translate[1], translate[2]);
#endif

    pickpoint[0] = doe_device_pick_point[0] + device->extent.bll[0];
    pickpoint[1] = doe_device_pick_point[1] + device->extent.bll[1];
    pickpoint[2] = doe_device_pick_point[2];

    /* pick volume in device coordinates */

    device->device_pick_volume.bll[0] = 
				pickpoint[0]-0.5*pckapp->width;
    device->device_pick_volume.bll[1] = 
				pickpoint[1]-0.5*pckapp->height;
    device->device_pick_volume.bll[2] = 
				pickpoint[2]-0.5*pckapp->depth;

    device->device_pick_volume.fur[0] = 
				pickpoint[0]+0.5*pckapp->width;
    device->device_pick_volume.fur[1] = 
				pickpoint[1]+0.5*pckapp->height;
    device->device_pick_volume.fur[2] = 
				pickpoint[2]+0.5*pckapp->depth;

    /* pick volume in frame coordinates */

    pick_volume.bll[0] = (device->device_pick_volume.bll[0]-
			  translate[0])/scale[0];	
    pick_volume.bll[1] = (device->device_pick_volume.bll[1]-
			  translate[1])/scale[1];	
    pick_volume.bll[2] = (device->device_pick_volume.bll[2]-
			  translate[2])/scale[2];	

    pick_volume.fur[0] = (device->device_pick_volume.fur[0]-
			  translate[0])/scale[0];	
    pick_volume.fur[1] = (device->device_pick_volume.fur[1]-
			  translate[1])/scale[1];	
    pick_volume.fur[2] = (device->device_pick_volume.fur[2]-
			  translate[2])/scale[2];	

    projvprt = dor_view_inq_boundary_ptr(view_object);
    window = dor_frame_inq_boundary_ptr(device->frame);
    clipvol  = &(device->clipping_volume);

    /* compute intersection of frame and view boundaries */

    clipvol->bll[0] = max(projvprt->bll[0],window->bll[0]); 
    clipvol->bll[1] = max(projvprt->bll[1],window->bll[1]);
    clipvol->bll[2] = max(projvprt->bll[2],window->bll[2]);

    clipvol->fur[0] = min(projvprt->fur[0],window->fur[0]);
    clipvol->fur[1] = min(projvprt->fur[1],window->fur[1]);
    clipvol->fur[2] = min(projvprt->fur[2],window->fur[2]);

    /* compute intersection of result and pick volume */

    clipvol->bll[0] = max(clipvol->bll[0],pick_volume.bll[0]);
    clipvol->bll[1] = max(clipvol->bll[1],pick_volume.bll[1]);
    clipvol->bll[2] = max(clipvol->bll[2],pick_volume.bll[2]);

    clipvol->fur[0] = min(clipvol->fur[0],pick_volume.fur[0]);
    clipvol->fur[1] = min(clipvol->fur[1],pick_volume.fur[1]);
    clipvol->fur[2] = min(clipvol->fur[2],pick_volume.fur[2]);

#ifdef DEBUG
    printf("device pickvolume = (%lf,%lf,%lf) (%lf,%lf,%lf)\n",
	   device->device_pick_volume.bll[0],
	   device->device_pick_volume.bll[1],
	   device->device_pick_volume.bll[2],
	   device->device_pick_volume.fur[0],
	   device->device_pick_volume.fur[1],
	   device->device_pick_volume.fur[2]);

    printf("frame pickvolume = (%lf,%lf,%lf) (%lf,%lf,%lf)\n",
	   pick_volume.bll[0],pick_volume.bll[1],pick_volume.bll[2],
	   pick_volume.fur[0],pick_volume.fur[1],pick_volume.fur[2]);

    printf("view boundary = (%lf,%lf,%lf) (%lf,%lf,%lf)\n",
	   projvprt->bll[0],projvprt->bll[1],projvprt->bll[2],
	   projvprt->fur[0],projvprt->fur[1],projvprt->fur[2]);

    printf("frame boundary = (%lf,%lf,%lf) (%lf,%lf,%lf)\n",
	   window->bll[0],window->bll[1],window->bll[2],
	   window->fur[0],window->fur[1],window->fur[2]);

    printf("clipping vol = (%lf,%lf,%lf) (%lf,%lf,%lf)\n",
	   clipvol->bll[0],clipvol->bll[1],clipvol->bll[2],
	   clipvol->fur[0],clipvol->fur[1],clipvol->fur[2]);
#endif

    device->all_clipped =
	    ((clipvol->bll[0] >= clipvol->fur[0]) ||
	     (clipvol->bll[1] >= clipvol->fur[1]) ||
	     (clipvol->bll[2] >= clipvol->fur[2]));
}
/*
 ======================================================================
 */
void dor_device_resize (dot_object *device_object)
{
    dor_device_update_notify(device_object);
}
/*
 ======================================================================
 */
void dor_device_viewport_size (dot_object *device_object)
{
    dot_device *device = device_object->data;

    dor_device_inq_extent (device_object, &(device->extent));

    device->viewport = device->extent;
}
