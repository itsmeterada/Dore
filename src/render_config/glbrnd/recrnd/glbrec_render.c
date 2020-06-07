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
	int drr_recrnd_render ()
	DtFlag drr_recrnd_render_ray (ray,red,grn,blu)
	int drr_recrnd_render_rectangle (pixelxmin,pixelymin,pixelxmax,pixelymax, pixelarrayred,pixelarraygrn,pixelarrayblu)
	int drr_recrnd_render_scanline_segment (pixelfirst,pixellast,scanlinenumber, pixelarrayred,pixelarraygrn,pixelarrayblu)
	int drr_recrnd_render_screen_point (screenx,screeny,red,grn,blu)

  ======================================================================
 */
#include "recrnd.h"
#include "dore/dore_develop/develop.h"

/*
 ======================================================================
 */

void drr_recrnd_render (void)
{
    DtReal *pixelarrayred, *pixelarraygrn, *pixelarrayblu;
    char pixarr[10000][3];
    DtInt packetlength, packetheight;
    DtInt scanlinemin, scanlinemax;
    DtInt scanlines;
    DtInt pixelmin, pixelmax;
    DtInt pixels;
    DtInt packetsx, packetsy;
    DtInt packetnox, packetnoy;
    DtInt totalpackets;
    DtInt rectanglepixels;
    DtInt packetno;
    DtInt pixelxmin, pixelymin, pixelxmax, pixelymax;
    DtInt pixelno;

    scanlinemin = 0;
    scanlinemax = dre_recrnd_data.resolution_y - 1;

    scanlines = scanlinemax - scanlinemin + 1;

    pixelmin = 0;
    pixelmax = dre_recrnd_data.resolution_x - 1;

    pixels = pixelmax - pixelmin + 1;

    packetlength = pixels;
    packetheight = 1;

    packetsx = pixels / packetlength;
    packetsy = scanlines / packetheight;
    totalpackets = packetsx * packetsy;

    rectanglepixels = packetlength*packetheight;

    sprintf (DDprint_GetLine(), "resolution = (%d,%d)\n",
	     dre_recrnd_data.resolution_x, dre_recrnd_data.resolution_y);
    DDprint (DDprint_GetLine());

    if (rectanglepixels <= 0){
	return;
    }

    pixelarrayred = (DtReal *)DDspace_Allocate(
			 (DtShort)rectanglepixels*(sizeof (DtReal)));
    pixelarraygrn = (DtReal *)DDspace_Allocate(
			 (DtShort)rectanglepixels*(sizeof (DtReal)));
    pixelarrayblu = (DtReal *)DDspace_Allocate(
			 (DtShort)rectanglepixels*(sizeof (DtReal)));

    for (packetno=0;packetno<totalpackets;packetno++){
	packetnoy = packetno / packetsx;

	packetnox = packetno - (packetnoy*packetsx);

	pixelxmin = pixelmin + packetnox*packetlength;
	pixelxmax = pixelxmin + packetlength - 1;
	pixelymin = scanlinemin + packetnoy*packetheight;
	pixelymax = pixelymin + packetheight - 1;

	drr_recrnd_render_rectangle(
		    pixelxmin,pixelymin,pixelxmax,pixelymax,
		    pixelarrayred,pixelarraygrn,pixelarrayblu);

	/*  Write the pixel data out to the device.  */

	for (pixelno=0;pixelno<rectanglepixels;pixelno++){
	    pixarr[pixelno][0] = 
			(char)(((DtShort)
			(pixelarrayred[pixelno]*255.)) & 255);

	    pixarr[pixelno][1] = 
			(char)(((DtShort)
			(pixelarraygrn[pixelno]*255.)) & 255);

	    pixarr[pixelno][2] = 
			(char)(((DtShort)
			(pixelarrayblu[pixelno]*255.)) & 255);
	}

	(*dre_glbrnd_PROM_fcns->write_rectangle_byte_rgb)(
		  dre_recrnd_data.pixel_window_x_minimum + pixelxmin,
		  dre_recrnd_data.pixel_window_y_minimum + pixelymin,
		  packetlength, packetheight, (DtUChar*)(pixarr));

	/*
	  dor_device_flush();
	  */
    }

    DDspace_Deallocate(pixelarrayred);
    DDspace_Deallocate(pixelarraygrn);
    DDspace_Deallocate(pixelarrayblu);

    /*
      drr_rayint_print_run_statistics();
      */
}
/*
 ======================================================================
 */

DtFlag drr_recrnd_render_ray (
    struct ray *ray,
    DtReal *red,
    DtReal *grn,
    DtReal *blu)
{
    DtFlag intersectflag;
    struct intersect intersectstruct;
    struct intersect *intersect = &intersectstruct;
    DtColorRGB color;

    /*  Find the closest intersection of the viewer ray with 
	an object in the environment.  The intersection routine 
	returns a flag indicating whether or not such an intersection 
	was found.  If one was found, then the intersect data structure 
	is guaranteed to contain a pointer to the rayint_object that 
	was intersected, the world coordinate system location of the 
	point of intersection, the parametric value along the viewer 
	ray at which the intersection point occurred, and a flag 
	indicating whether or not the primitive object also returned 
	the normal at the point of intersection.  
	Primitive objects (like cubic solids) sometimes compute the 
	normal at the point of intersection at the same time that they 
	compute the intersection point, since this computation is much 
	faster at that time.  If returned, the normal will still be in 
	the local coordinate system of the rayint object.  */

    intersectflag = drr_rayint_intersect(ray,0,intersect);

    if (!intersectflag){
	*red = dre_recrnd_data.background_color[0];
	*grn = dre_recrnd_data.background_color[1];
	*blu = dre_recrnd_data.background_color[2];

	return(DcFalse);
    }

    drr_glbshd_shade_raytraced(intersect->rayint_object,ray,
				intersect,color);

    *red = color[0];
    *grn = color[1];
    *blu = color[2];

    return(DcTrue);
}
/*
 ======================================================================
 */

void drr_recrnd_render_rectangle (
    DtInt pixelxmin,
    DtInt pixelymin,
    DtInt pixelxmax,
    DtInt pixelymax,
    DtReal *pixelarrayred,
    DtReal *pixelarraygrn,
    DtReal *pixelarrayblu)
{
    DtReal *pixeladdrred, *pixeladdrgrn, *pixeladdrblu;
    DtInt rowlength;
    DtInt scanlineno;

    rowlength = (pixelxmax-pixelxmin+1);

    pixeladdrred = &pixelarrayred[0];
    pixeladdrgrn = &pixelarraygrn[0];
    pixeladdrblu = &pixelarrayblu[0];

    for (scanlineno=pixelymin;scanlineno<=pixelymax;scanlineno++){
	drr_recrnd_render_scanline_segment(
			   pixelxmin,pixelxmax,scanlineno,
			   pixeladdrred,pixeladdrgrn,pixeladdrblu);

	pixeladdrred += rowlength;
	pixeladdrgrn += rowlength;
	pixeladdrblu += rowlength;
    }
}
/*
 ======================================================================
 */

void drr_recrnd_render_scanline_segment (
    DtInt pixelfirst,
    DtInt pixellast,
    DtInt scanlinenumber,
    DtReal pixelarrayred[],
    DtReal pixelarraygrn[],
    DtReal pixelarrayblu[])
{
    DtReal screeny;
    DtReal screenxmin;
    DtReal screenx;
    DtInt pixelnumber;

    screeny = -1. + dre_recrnd_data.pixel_height*.5 + 
		(DtReal)scanlinenumber* dre_recrnd_data.pixel_height;

    screenxmin = -1. + dre_recrnd_data.pixel_width*.5;

    for (pixelnumber=pixelfirst;pixelnumber<=pixellast;pixelnumber++){
	screenx = screenxmin + 
		(DtReal)pixelnumber*dre_recrnd_data.pixel_width;

	drr_recrnd_render_screen_point(screenx,screeny,
		       &(pixelarrayred[pixelnumber-pixelfirst]),
		       &(pixelarraygrn[pixelnumber-pixelfirst]),
		       &(pixelarrayblu[pixelnumber-pixelfirst]));
    }
}
/*
 ======================================================================
 */

void drr_recrnd_render_screen_point (
    DtReal screenx,
    DtReal screeny,
    DtReal *red,
    DtReal *grn,
    DtReal *blu)
{
    DtReal scspt[4];
    DtReal wcspt0[4], wcspt1[4];
    DtReal oneoverw;

    scspt[0] = screenx;
    scspt[1] = screeny;
    scspt[2] = 0.;
    scspt[3] = 1.;

    dor_math_homo_trns_fast(scspt,dre_recrnd_data.fcstowcsmat,wcspt0);

    oneoverw = 1. / wcspt0[3];

    wcspt0[0] *= oneoverw;
    wcspt0[1] *= oneoverw;
    wcspt0[2] *= oneoverw;

    scspt[2] = -1.;
    scspt[3] = 1.;

    dor_math_homo_trns_fast(scspt,dre_recrnd_data.fcstowcsmat,wcspt1);

    oneoverw = 1. / wcspt1[3];

    wcspt1[0] *= oneoverw;
    wcspt1[1] *= oneoverw;
    wcspt1[2] *= oneoverw;

    dor_math_fast_normal(wcspt0,wcspt1,
			dre_recrnd_data.view_ray.wcsdirection);

    dre_recrnd_data.view_ray.wcsorigin[0] = wcspt0[0];
    dre_recrnd_data.view_ray.wcsorigin[1] = wcspt0[1];
    dre_recrnd_data.view_ray.wcsorigin[2] = wcspt0[2];

    dre_recrnd_data.view_ray.current_box = NULL;

    drr_recrnd_render_ray(&dre_recrnd_data.view_ray,red,grn,blu);
}
