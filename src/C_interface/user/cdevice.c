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

/* ====================================================================== */

DtObject DoDevice (DtPtr devicetype, DtPtr argstring)
{
    return dor_device_create (devicetype, argstring);
}

/* ====================================================================== */

void DdPickObjs (
    DtObject  device,
    DtPoint3  pickpoint,
    DtInt     *hitcount,
    DtInt      indexsize,
    DtInt      index[],
    DtInt      listsize,
    long       hitlist[],
    DtReal     zvalues[],
    DtReal     xyzvalues[],
    DtReal     lcsvalues[],
    DtObject   views[],
    DtInt     *errorword)
{
    dor_device_pick
    (   device, pickpoint, hitcount, indexsize, index, listsize, hitlist,
	zvalues, xyzvalues, lcsvalues, views, errorword
    );
}

/* ====================================================================== */

void DdPick (
    DtObject  device,
    DtPoint3  pickpoint,
    DtInt    *hitcount,
    DtInt     indexsize,
    DtInt     index[],
    DtInt     listsize,
    long      hitlist[],
    DtReal    zvalues[],
    DtObject  views[],
    DtInt    *errorword)
{
    static int msg_printed = 0;

    if (!msg_printed) {
	DDerror (ERR_OBSOLETE_FUNCTION, "DdPick", "DdPickObjs");
	msg_printed = 1;
    }

    dor_device_pick (device, pickpoint, hitcount, indexsize, index,
		    listsize, hitlist, zvalues, (DtReal *)0, (DtReal *)0, 
		    views, errorword);
}

/* ====================================================================== */

void DdUpdate (DtObject device)
{
    dor_device_update (device);
}

/* ====================================================================== */

void DdInqFonts (DtObject device, DtFontPrecision fonts[])
{
    dor_device_inq_fonts (device, fonts);
}

/* ====================================================================== */

DtObject DdInqFrame (DtObject device)
{
    return dor_device_inq_frame (device);
}

/* ====================================================================== */

DtInt DdInqNumFonts (DtObject device)
{
    return dor_device_inq_num_fonts (device);
}

/* ====================================================================== */

void DdInqPickAperture (DtObject device, DtSize3 *aperture)
{
    dor_device_inq_pick_aperture (device, aperture);
}

/* ====================================================================== */

DtObject DdInqPickCallback (DtObject device)
{
    return dor_device_inq_pick_callback (device);
}

/* ====================================================================== */

DtPickPathOrder DdInqPickPathOrder (DtObject device)
{
    return dor_device_inq_pick_path_order (device);
}

/* ====================================================================== */

DtShadeMode DdInqShadeMode (DtObject device)
{
    return dor_device_inq_shade_mode (device);
}

/* ====================================================================== */

void DdInqShadeRanges (
    DtObject device,
    DtInt    start,
    DtInt    count,
    DtInt    entries[])
{
    dor_device_inq_shade_ranges (device, start, count, entries);
}

/* ====================================================================== */

DtVisualType DdInqVisualType (DtObject device)
{
    return dor_device_inq_visual_type (device);
}

/* ====================================================================== */

void DdInqViewport (DtObject device, DtVolume *viewport)
{
    dor_device_inq_viewport (device, viewport);
}

/* ====================================================================== */

void DdInqColorEntries (
    DtObject     device,
    DtColorModel colortype,
    DtInt        start,
    DtInt        count,
    DtReal       entries[])
{
    dor_device_inq_color_entries (device, colortype, start, count, entries);
}

/* ====================================================================== */

DtInt DdInqColorTableSize (DtObject device)
{
    return dor_device_inq_color_table_size (device);
}

/* ====================================================================== */

void DdInqExtent (DtObject device, DtVolume *volume)
{
    dor_device_inq_extent (device, volume);
}

/* ====================================================================== */

DtFlag DdInqPixelData (
    DtObject      device,
    DtRasterType  requesttype,
    DtInt        *width,
    DtInt        *height,
    DtRasterType *type,
    DtPtr        *data,
    DtFlag       *userdelete)
{
    return dor_device_inq_pixeldata (device, requesttype, width, height, type,
				     data, userdelete);
}

/* ====================================================================== */

void DdInqResolution (DtObject device, DtReal *x, DtReal *y)
{
    dor_device_inq_resolution (device, x,y);
}

/* ====================================================================== */

void DdSetFrame (DtObject device, DtObject frame)
{
    dor_device_set_frame (device, frame);
}

/* ====================================================================== */

void DdSetPickAperture (DtObject device, DtSize3 *aperture)
{
    dor_device_set_pick_aperture (device, aperture);
}

/* ====================================================================== */

void DdSetPickCallback (DtObject device, DtObject pickcallbackobj)
{
    dor_device_set_pick_callback (device, pickcallbackobj);
}

/* ====================================================================== */

void DdSetPickPathOrder (DtObject device, DtPickPathOrder pick_path_order)
{
    dor_device_set_pick_path_order (device, pick_path_order);
}

/* ====================================================================== */

void DdSetShadeMode (DtObject device, DtShadeMode mode)
{
    dor_device_set_shade_mode (device, mode);
}

/* ====================================================================== */

void DdSetShadeRanges (
    DtObject device,
    DtInt    start,
    DtInt    count,
    DtInt    entries[])
{
    dor_device_set_shade_ranges (device,start,count,(void*)(entries));
}

/* ====================================================================== */

void DdSetViewport (DtObject device, DtVolume *viewport)
{
    dor_device_set_viewport (device,viewport);
}

/* ====================================================================== */

void DdSetColorEntries (
    DtObject     device,
    DtColorModel colortype,
    DtInt        start,
    DtInt        count,
    DtReal       entries[])
{
    dor_device_set_color_entries (device,colortype,start,count,entries);
}

/* ====================================================================== */

void DdSetOptions (DtObject device, DtPtr argstring)
{
   dor_device_set_options (device, argstring);
}
