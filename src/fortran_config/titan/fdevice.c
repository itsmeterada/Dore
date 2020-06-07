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
 
#include <dore/internal/dogen.h>
#include <dore/dore_develop/private/device.h>

int _DCNULL[2];

/* ====================================================================== */

DtObject DOD (devicetype, n1, argstring, n2)
    DtPtr devicetype;
    int *n1;
    DtPtr argstring;
    int *n2;
{
    static char *s1, *s2;
    static int sn1, sn2;

    sn1 = fstr(&s1, sn1, devicetype, *n1);
    sn2 = fstr(&s2, sn2, argstring, *n2);
    return(dor_device_create(s1, s2));
}

/* ====================================================================== */

void DDPO (device, pickpoint, hitcount, indexsize, index, listsize, hitlist,
	   zvalues, xyzvalues, lcsvalues, views, errorword)
    DtObject *device;
    DtPoint3 pickpoint;
    DtInt *hitcount;
    DtInt *indexsize;
    DtInt index[];
    DtInt *listsize;
    long hitlist[];
    DtReal zvalues[];
    DtReal xyzvalues[];
    DtReal lcsvalues[];
    DtObject views[];
    DtInt *errorword;
{
    dor_device_pick(*device, pickpoint, hitcount, 
	   *indexsize, index, *listsize, hitlist, 
	   ((int*)zvalues == _DCNULL)? (DtReal*)0:zvalues,
	   ((int*)xyzvalues == _DCNULL)? (DtReal*)0:xyzvalues,
	   ((int*)lcsvalues == _DCNULL)? (DtReal*)0:lcsvalues,
	   ((int*)views == _DCNULL)? (DtObject *)0 : views, 
	   errorword);
}

/* ====================================================================== */

void DDP (device, pickpoint, hitcount, indexsize, index, listsize, hitlist,
	  zvalues, views, errorword)
    DtObject *device;
    DtPoint3 pickpoint;
    DtInt *hitcount;
    DtInt *indexsize;
    DtInt index[];
    DtInt *listsize;
    long hitlist[];
    DtReal zvalues[];
    DtObject views[];
    DtInt *errorword;
{
   static int msg_printed = 0;

   if (!msg_printed) {
      DDerror (ERR_OBSOLETE_FUNCTION, "DDP", "DDPO"); 
      msg_printed = 1;
   }

   dor_device_pick(*device, pickpoint, hitcount, *indexsize, index,
		   *listsize, hitlist, 
		   ((int*)zvalues == _DCNULL)? (DtReal*)0:zvalues,
		   (DtReal *)0,
		   (DtReal *)0,
		   ((int*)views == _DCNULL)? (DtObject *)0 : views, 
		   errorword);
}

/* ====================================================================== */

void DDU (device)
    DtObject *device;
{
    dor_device_update(*device);
}

/* ====================================================================== */

void DDQFT (device, fonts)
    DtObject *device;
    DtFontPrecision fonts[];
{
    dor_device_inq_fonts(*device, fonts);
}

/* ====================================================================== */

DtObject DDQFR (device)
    DtObject *device;
{
    return dor_device_inq_frame(*device);
}

/* ====================================================================== */

DtInt DDQNF (device)
    DtObject *device;
{
    return dor_device_inq_num_fonts(*device);
}

/* ====================================================================== */

void DDQPA (device, aperture)
    DtObject *device;
    DtSize3 *aperture;
{
    dor_device_inq_pick_aperture(*device, aperture);
}

/* ====================================================================== */

DtObject DDQPC (device)
    DtObject *device;
{
    return dor_device_inq_pick_callback(*device);
}

/* ====================================================================== */

DtPickPathOrder DDQPPO (device)
    DtObject *device;
{
    return(dor_device_inq_pick_path_order(*device));
}

/* ====================================================================== */

DtShadeMode DDQSM (device)
    DtObject *device;
{
    return(dor_device_inq_shade_mode(*device));
}

/* ====================================================================== */

void DDQSR (device, start, count, entries)
    DtObject *device;
    DtInt *start;
    DtInt *count;
    DtInt entries[];
{
    dor_device_inq_shade_ranges(*device, *start, *count, entries);
}

/* ====================================================================== */

DtVisualType DDQVT (device)
    DtObject *device;
{
    return dor_device_inq_visual_type(*device);
}

/* ====================================================================== */

void DDQV (device, viewport)
    DtObject *device;
    DtVolume *viewport;
{
    dor_device_inq_viewport(*device,viewport);
}

/* ====================================================================== */

void DDQCE (device, colortype, start, count, entries)
    DtObject *device;
    int *colortype;
    DtInt *start;
    DtInt *count;
    DtReal entries[];
{
    dor_device_inq_color_entries(*device,(DtColorModel)*colortype,*start,
    	*count,entries);
}

/* ====================================================================== */

DtInt DDQCTS (device)
    DtObject *device;
{
    return(dor_device_inq_color_table_size(*device));
}

/* ====================================================================== */

void DDQE (device, volume)
    DtObject *device;
    DtVolume *volume;
{
    dor_device_inq_extent(*device,volume);
}

/* ====================================================================== */

DtFlag DDQPXD (device, requesttype, width, height, type, data, userdelete)
    DtObject *device;
    int *requesttype;
    DtInt *width;
    DtInt *height;
    int *type;
    DtPtr data;
    int *userdelete;
{
    return dor_device_inq_pixeldata (*device, *requesttype, width, height,
				     type, data, userdelete);
}

/* ====================================================================== */

void DDQR (device, x, y)
    DtObject *device;
    DtReal *x;
    DtReal *y;
{
    dor_device_inq_resolution(*device,x,y);
}

/* ====================================================================== */

void DDSF (device, frame)
    DtObject *device;
    DtObject *frame;
{
    dor_device_set_frame(*device, *frame);
}

/* ====================================================================== */

void DDSPA (device, aperture)
    DtObject *device;
    DtSize3 *aperture;
{
    dor_device_set_pick_aperture(*device, aperture);
}

/* ====================================================================== */

void DDSPCB (device, pickcallbackobj)
    DtObject *device;
    DtObject *pickcallbackobj;
{
    dor_device_set_pick_callback(*device, *pickcallbackobj);
}

/* ====================================================================== */

void DDSPPO (device, pick_path_order)
    DtObject *device;
    int *pick_path_order;
{
    dor_device_set_pick_path_order (*device, (DtPickPathOrder)*pick_path_order);
}

/* ====================================================================== */

void DDSSM (device, mode)
    DtObject *device;
    int *mode;
{
    dor_device_set_shade_mode(*device, (DtShadeMode)*mode);
}

/* ====================================================================== */

void DDSSR (device, start, count, entries)
    DtObject *device;
    DtInt *start;
    DtInt *count;
    DtInt entries[];
{
    dor_device_set_shade_ranges(*device, *start, *count, entries);
}

/* ====================================================================== */

void DDSDV (device, viewport)
    DtObject *device;
    DtVolume *viewport;
{
    dor_device_set_viewport(*device, viewport);
}

/* ====================================================================== */

void DDSCE (device, colortype, start, count, entries)
    DtObject *device;
    int *colortype;
    DtInt *start;
    DtInt *count;
    DtReal entries[];
{
    dor_device_set_color_entries
	(*device, (DtColorModel)*colortype, *start, *count, entries);
}

/* ====================================================================== */

void DDSO (device, argstring, n)
    DtObject *device;
    DtPtr argstring;
    int *n;
{
    static char *s;
    static int sn;

    sn = fstr(&s, sn, argstring, *n);

    dor_device_set_options(*device, s);
}
