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
 
#include <string.h>
#include "dore/fortran.h"
#include <dore/dore_develop/private/device.h>


/* ===================================================================== */

DtObject dod_ (devicetype, n1, argstring, n2, fstrlen1, fstrlen2)
    DtPtr  devicetype;
    DtInt *n1;
    char  *argstring;
    DtInt *n2;
    DtInt fstrlen1;
    DtInt fstrlen2;
{
    static char *s1, *s2;
    static int sn1, sn2;

    sn1 = fstr (&s1, sn1, devicetype, *n1);
    sn2 = fstr (&s2, sn2, argstring, *n2);
    return dor_device_create (s1, s2);
}

/* ===================================================================== */

void ddp_ (device, pickpoint, hitcount, indexsize, index, listsize,
	   hitlist, zvalues, views, errorword)
    DtObject *device;
    DtPoint3  pickpoint;
    DtInt    *hitcount;       /* RETURN */
    DtInt    *indexsize;
    DtInt     index[];         /* RETURN */
    DtInt    *listsize;
    long      hitlist[];       /* RETURN */
    DtReal    zvalues[];      /* RETURN */
    DtObject  views[];      /* RETURN */
    DtInt    *errorword;      /* RETURN */
{
    static int msg_printed = 0;

    if (!msg_printed) {
        DDerror (ERR_OBSOLETE_FUNCTION, "DDP", "DDPO"); 
        msg_printed = 1;
    }

    dor_device_pick
    (   *device, pickpoint, hitcount, *indexsize, index, *listsize, hitlist, 
	FORTRAN_PTR (zvalues), DcNullPtr, DcNullPtr, FORTRAN_PTR (views),
        errorword
    );
}

/* ===================================================================== */

void ddpo_ (device, pickpoint, hitcount, indexsize, index, listsize,
	    hitlist, zvalues, xyzvalues, lcsvalues, views, errorword)
    DtObject *device;
    DtPoint3  pickpoint;
    DtInt    *hitcount;
    DtInt    *indexsize;
    DtInt     index[];
    DtInt    *listsize;
    long      hitlist[];
    DtReal    zvalues[];
    DtReal    xyzvalues[];
    DtReal    lcsvalues[];
    DtObject  views[];
    DtInt    *errorword;
{
    dor_device_pick
    (   *device, pickpoint, hitcount, *indexsize, index, *listsize, hitlist, 
	FORTRAN_PTR (zvalues), FORTRAN_PTR (xyzvalues),
	FORTRAN_PTR (lcsvalues), FORTRAN_PTR (views), errorword
    );
}

/* ===================================================================== */

void ddu_ (device)
    DtObject *device;
{
    dor_device_update (*device);
}

/* ==================================================================== */

void ddqft_ (device, fonts)
    DtObject *device;
    DtFontPrecision fonts[];
{
    dor_device_inq_fonts(*device, fonts);
}

/* ==================================================================== */

DtObject ddqfr_ (device)
    DtObject *device;
{
    return dor_device_inq_frame(*device);
}

/* ==================================================================== */

DtInt ddqnf_ (device)
    DtObject *device;
{
    return dor_device_inq_num_fonts(*device);
}

/* ==================================================================== */

void ddqpa_ (device, aperture)
    DtObject *device;
    DtSize3 *aperture;
{
    dor_device_inq_pick_aperture(*device, aperture);
}

/* ==================================================================== */

DtObject ddqpc_ (device)
    DtObject *device;
{
    return dor_device_inq_pick_callback(*device);
}

/* ==================================================================== */

DtPickPathOrder ddqppo_ (device)
    DtObject *device;
{
    return(dor_device_inq_pick_path_order(*device));
}

/* ==================================================================== */

DtShadeMode ddqsm_ (device)
    DtObject *device;
{
    return(dor_device_inq_shade_mode(*device));
}

/* ==================================================================== */

void ddqsr_ (device, start, count, entries)
    DtObject *device;
    DtInt *start;
    DtInt *count;
    DtInt entries[];
{
    dor_device_inq_shade_ranges(*device, *start, *count, entries);
}

/* ==================================================================== */

DtVisualType ddqvt_ (device)
    DtObject *device;
{
    return dor_device_inq_visual_type(*device);
}

/* ==================================================================== */

void ddqv_ (device, viewport)
    DtObject *device;
    DtVolume *viewport;
{
    dor_device_inq_viewport(*device,viewport);
}

/* ==================================================================== */

void ddqce_ (device, colortype, start, count, entries)
    DtObject     *device;
    DtColorModel *colortype;
    DtInt        *start;
    DtInt        *count;
    DtReal        entries[];
{
    dor_device_inq_color_entries
	(*device, *colortype, *start, *count, entries);
}

/* ==================================================================== */

DtInt ddqcts_ (device)
    DtObject *device;
{
	return(dor_device_inq_color_table_size(*device));
}

/* ==================================================================== */

void ddqe_ (device, volume)
    DtObject *device;
    DtVolume *volume;
{
	dor_device_inq_extent(*device,volume);
}

/* ======================================================================== */

DtFlag ddqpxd_ (device, requesttype, width, height, type, data, userdelete)
    DtObject     *device;
    DtRasterType *requesttype;
    DtInt        *width;
    DtInt        *height;
    DtRasterType *type;
    DtPtr         data;
    DtFlag       *userdelete;
{
    return dor_device_inq_pixeldata
	       (*device, *requesttype, width, height, type, data, userdelete);
}

/* ==================================================================== */

void ddqr_ (device, x, y)
    DtObject *device;
    DtReal *x;
    DtReal *y;
{
    dor_device_inq_resolution (*device,x,y);
}

/* ==================================================================== */

void ddsf_ (device, frame)
    DtObject *device;
    DtObject *frame;
{
    dor_device_set_frame (*device, *frame);
}

/* ===================================================================== */

void ddspa_ (device, aperture)
    DtObject *device;
    DtSize3 *aperture;
{
    dor_device_set_pick_aperture (*device, aperture);
}

/* ===================================================================== */

void ddspcb_ (device, pickcallbackobj)
    DtObject *device;
    DtObject *pickcallbackobj;
{
    dor_device_set_pick_callback (*device, *pickcallbackobj);
}

/* ===================================================================== */

void ddsppo_ (device, pick_path_order)
    DtObject *device;
    DtPickPathOrder *pick_path_order;
{
    dor_device_set_pick_path_order (*device, *pick_path_order);
}

/* ===================================================================== */

void ddssm_ (device, mode)
    DtObject *device;
    DtShadeMode *mode;
{
    dor_device_set_shade_mode (*device, *mode);
}

/* ===================================================================== */

void ddssr_ (device, start, count, entries)
    DtObject *device;
    DtInt *start;
    DtInt *count;
    DtInt entries[][2];
{
    dor_device_set_shade_ranges (*device, *start, *count, entries);
}

/* ===================================================================== */

void ddsdv_ (device, viewport)
    DtObject *device;
    DtVolume *viewport;
{
    dor_device_set_viewport (*device, viewport);
}

/* ===================================================================== */

void ddsce_ (device, colortype, start, count, entries)
    DtObject *device;
    DtColorModel *colortype;
    DtInt *start;
    DtInt *count;
    DtReal entries[];
{
    dor_device_set_color_entries (*device, *colortype, *start, *count, entries);
}

/* ===================================================================== */
 
void ddso_ (device, argstring, n, fstrlen)
    DtObject *device;
    char     *argstring;
    int      *n;
    int       fstrlen;
{
   static char *s;
   static int sn;

   sn = fstr (&s, sn, argstring, *n);

   dor_device_set_options (*device, s);
}
