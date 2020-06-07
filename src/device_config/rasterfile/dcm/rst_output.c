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
 
/* ======================================================================
  Functions:
	ddr_rasterfile_dcm_write_scanline_byte
  ====================================================================== */

#include <stdio.h>
#include "dcm.h"

/* =================================================================== */

void ddr_rasterfile_dcm_write_scanline_byte (
    DtInt xstart,
    DtInt ystart,
    DtInt length,
    DtUChar *data)
{
    ddt_rasterfile_data *device_data;
    long offset;
    extern int errno;

    device_data = dde_rasterfile_current_device_data;
#ifdef DEBUG
    printf ("xstart %d, ystart %d, length %d\n", xstart, ystart, length);
#endif
   
    offset = device_data->bindata +
	    (device_data->height - 1 - ystart) * device_data->width *
		    3 + xstart * 3;

#ifdef DEBUG
    printf ("offset %d bindata %d width %d height %d\n", offset,
	    device_data->bindata, device_data->width, device_data->height);
#endif
    if (fseek (dde_rasterfile_current_device_data->fp, offset, 0) != 0) {
	sprintf (DDerror_GetLine(), "fseek error %d", errno);
	DDerror(ERR_IO_ERROR, "ddr_rasterfile_dcm_write_scanline_byte",
		DDerror_GetLine());
    }
#ifdef DEBUG
    printf ("current file position before write: %ld\n", ftell(device_data->fp));
#endif
    fwrite (data, sizeof (DtUChar), 3*length, device_data->fp);
}
